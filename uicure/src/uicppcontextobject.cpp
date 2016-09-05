
// Author: Jonas Byström
// Copyright (c) 2002-, Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include <math.h>
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/random.h"
#include "../include/uicppcontextobject.h"
#include "../include/uicure.h"
#include "../include/uigameuimanager.h"



namespace UiCure {



CppContextObject::CppContextObject(cure::ResourceManager* resource_manager, const str& class_id, GameUiManager* ui_manager):
	cure::CppContextObject(resource_manager, class_id),
	ui_manager_(ui_manager),
	ui_class_resource_(0),
	enable_ui_(true),
	allow_root_shadow_(true),
	use_default_texture_(false),
	enable_pixel_shader_(true),
	enable_mesh_move_(true),
	enable_mesh_slide_(false),
	sink_speed_(0),
	sink_offset_(0),
	mesh_load_count_(0),
	texture_load_count_(0),
	lerp_mode_(kLerpStop) {
	log_volatile(log_.Tracef("Construct CppCO %s.", class_id.c_str()));
}

CppContextObject::~CppContextObject() {
	log_volatile(log_.Tracef("Delete CppCO %X:%s.", GetInstanceId(), GetClassId().c_str()));

	for (MeshArray::iterator x = mesh_resource_array_.begin(); x != mesh_resource_array_.end(); ++x) {
		delete (*x);
	}
	mesh_resource_array_.clear();

	for (TextureArray::iterator x = texture_resource_array_.begin(); x != texture_resource_array_.end(); ++x) {
		delete (*x);
	}
	texture_resource_array_.clear();

	delete (ui_class_resource_);
	ui_class_resource_ = 0;

	ui_manager_ = 0;
}



void CppContextObject::EnableUi(bool enable) {
	enable_ui_ = enable;
}

void CppContextObject::EnableRootShadow(bool enable) {
	allow_root_shadow_ = enable;
}

void CppContextObject::SetUseDefaultTexture(bool use_default_texture) {
	use_default_texture_ = use_default_texture;
}

void CppContextObject::EnablePixelShader(bool enable) {
	enable_pixel_shader_ = enable;
}

void CppContextObject::EnableMeshMove(bool enable) {
	enable_mesh_move_ = enable;
}

void CppContextObject::EnableMeshSlide(bool enable) {
	enable_mesh_slide_ = enable;
}

void CppContextObject::SetPositionFinalized() {
	Parent::SetPositionFinalized();
	lerp_mode_ = kLerpStop;
}



void CppContextObject::StartLoading() {
	deb_assert(ui_class_resource_ == 0);
	ui_class_resource_ = new UserClassResource(ui_manager_);
	const str class_name = "UI:"+GetClassId()+".class";
	ui_class_resource_->Load(GetResourceManager(), class_name,
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));
}



void CppContextObject::OnTick() {
	if (!enable_ui_ || !enable_mesh_move_) {
		return;
	}
	if (!physics_) {
		log_.Warningf("Physical body for %s not loaded!", GetClassId().c_str());
		return;
	}

	if (!GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId())) {
		// Only move kUi representation of objects that are not owned by other split screen players.
		UiMove();
	}
}

void CppContextObject::UiMove() {
	tbc::PhysicsManager* physics_manager = manager_->GetGameManager()->GetPhysicsManager();
	const float frame_time = GetManager()->GetGameManager()->GetTimeManager()->GetRealNormalFrameTime();
	const float lerp_factor = Math::GetIterateLerpTime(0.2f, frame_time);
	vec3 root_position;
	xform physics_transform;
	if (sink_speed_) {
		const int geometry_count = physics_->GetBoneCount();
		for (int x = 0; x < geometry_count; ++x) {
			const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(x);
			if (geometry && geometry->GetBodyId()) {
				vec3 position = physics_manager->GetBodyPosition(geometry->GetBodyId());
				position.z -= sink_speed_ * GetManager()->GetGameManager()->GetTimeManager()->GetNormalGameFrameTime();
				physics_manager->SetBodyPosition(geometry->GetBodyId(), position);
			}
		}
	}
	/*if (lerp_mode_ == kLerpStart) {
		lerp_offset_.SetIdentity();
		//log_volatile(log_.Debugf("Starting slide of mesh on object %u/%s.", GetInstanceId(), GetClassId().c_str()));
	}*/
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UserGeometryReferenceResource* resource = mesh_resource_array_[x];
		if (resource->GetLoadState() != cure::kResourceLoadComplete) {
			continue;
		}
		tbc::GeometryBase* gfx_geometry = resource->GetRamData();

		if (physics_override_ != cure::kPhysicsOverrideBones) {
			tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(resource->GetOffset().geometry_index_);
			tbc::PhysicsManager::BodyID body_id = geometry->GetBodyId();
			if (!geometry || body_id == tbc::INVALID_BODY) {
				log_.Warningf("Physical body (index %u) for %s not loaded!", resource->GetOffset().geometry_index_, resource->GetName().c_str());
				continue;
			}
			physics_manager->GetBodyTransform(body_id, physics_transform);
			/*if (mAutoDisableMeshMove && physics_->GetPhysicsType() == tbc::ChunkyPhysics::kStatic && physics_->GetBodyType(geometry) == tbc::PhysicsManager::kStatic) {
				enable_mesh_move_ = false;
			}*/

			if (lerp_mode_ == kLerpStart) {
				if (x == 0) {
					// Start out by fetching offset.
					lerp_offset_ = gfx_geometry->GetBaseTransformation();
					lerp_offset_.GetOrientation() *= physics_transform.GetOrientation().GetInverse();
					lerp_offset_.GetPosition() -= physics_transform.GetPosition();
					lerp_offset_.Interpolate(lerp_offset_, kIdentityTransformationF, lerp_factor);
				}
			}
			if (lerp_mode_ == kLerpStart || lerp_mode_ == kLerpRun) {
				// Phys + offset = current lerped mesh position. Also account for this physical offset to object root.
				if (x == 0) {
					root_position = physics_transform.GetPosition();
				}
				vec3 mesh_offset_with_root_orientation = physics_transform.GetPosition() - root_position;
				mesh_offset_with_root_orientation = lerp_offset_.GetOrientation() * mesh_offset_with_root_orientation - mesh_offset_with_root_orientation;
				physics_transform.GetOrientation() = lerp_offset_.GetOrientation() * physics_transform.GetOrientation();
				physics_transform.GetPosition() += lerp_offset_.GetPosition() + mesh_offset_with_root_orientation;
			}
		} else {
			physics_transform = physics_->GetBoneTransformation(resource->GetOffset().geometry_index_);
		}

		gfx_geometry->SetTransformation(physics_transform);
	}
	if (lerp_mode_ == kLerpStart) {
		lerp_mode_ = kLerpRun;
		if (lerp_offset_.GetPosition().GetLengthSquared() >= 20*20) {
			lerp_offset_.SetIdentity();
		}
	} else if (lerp_mode_ == kLerpRun) {
		if (lerp_offset_.GetPosition().GetLengthSquared() < 0.1f && lerp_offset_.GetOrientation().a > 0.999f) {
			lerp_mode_ = kLerpStop;
			lerp_offset_.SetIdentity();
		} else {
			lerp_offset_.Interpolate(lerp_offset_, kIdentityTransformationF, lerp_factor);
		}
	}
}

void CppContextObject::ActivateLerp() {
	if (enable_mesh_slide_) {
		lerp_mode_ = kLerpStart;
	}
}

void CppContextObject::SetSinking(float sink_speed) {
	sink_speed_ = sink_speed;
}

void CppContextObject::ShrinkMeshBigOrientationThreshold(float threshold) {
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UserGeometryReferenceResource* resource = mesh_resource_array_[x];
		if (resource->GetLoadState() != cure::kResourceLoadComplete) {
			continue;
		}
		tbc::GeometryBase* geometry = resource->GetRamData();
		if (geometry->GetBigOrientationThreshold() > threshold) {
			geometry->SetBigOrientationThreshold(threshold);
		}
	}
}



tbc::GeometryBase* CppContextObject::GetMesh(int index) const {
	const UserGeometryReferenceResource* resource = GetMeshResource(index);
	if (resource && resource->GetLoadState() == cure::kResourceLoadComplete) {
		return (resource->GetRamData());
	}
	return (0);
}

UserGeometryReferenceResource* CppContextObject::GetMeshResource(int index) const {
	if (index < (int)mesh_resource_array_.size()) {
		return mesh_resource_array_[index];
	} else {
		deb_assert(false);
	}
	return (0);
}

void CppContextObject::AddMeshResource(tbc::GeometryBase* mesh, int casts_shadows) {
	static int mesh_counter = 0;
	str _mesh_name = strutil::Format("RawMesh%i", mesh_counter++);
	DoAddMeshResource(_mesh_name, mesh, casts_shadows);
}

void CppContextObject::AddMeshResourceRef(const str& mesh_name, int casts_shadows) {
	DoAddMeshResource(mesh_name, 0, casts_shadows);
}

void CppContextObject::CenterMeshes() {
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UserGeometryReferenceResource* resource = mesh_resource_array_[x];
		if (resource->GetLoadState() != cure::kResourceLoadComplete) {
			continue;
		}
		tbc::GeometryReference* gfx_geometry = (tbc::GeometryReference*)resource->GetRamData();
		xform offset = gfx_geometry->GetOffsetTransformation();
		offset.SetPosition(vec3(0, 0, 0));
		gfx_geometry->SetOffsetTransformation(offset);
	}
}

void CppContextObject::LoadTexture(const str& name, Canvas::ResizeHint resize_hint) {
	UserRendererImageResource* texture = new UserRendererImageResource(ui_manager_, ImageProcessSettings(resize_hint, ui_manager_->GetRenderer()->GetMipMappingEnabled()));
	texture_resource_array_.push_back(texture);
	texture->Load(GetResourceManager(), name, UserRendererImageResource::TypeLoadCallback(this, &CppContextObject::OnLoadTexture));
}

void CppContextObject::UpdateMaterial(int mesh_index) {
	if (!ui_manager_->CanRender()) {
		return;
	}
	const uitbc::ChunkyClass* clazz = (const uitbc::ChunkyClass*)GetClass();
	if (!clazz || (int)clazz->GetMeshCount() <= mesh_index) {
		uitbc::Renderer::MaterialType material_type = enable_pixel_shader_? uitbc::Renderer::kMatSingleColorSolidPxs : uitbc::Renderer::kMatSingleColorSolid;
		ui_manager_->GetRenderer()->ChangeMaterial(mesh_resource_array_[mesh_index]->GetData(), material_type);
		return;
	}
	UserGeometryReferenceResource* _mesh = mesh_resource_array_[mesh_index];
	const bool transparent = (_mesh->GetRamData()->GetBasicMaterialSettings().alpha_ < 1);
	UserRendererImageResource* texture = 0;
	if (_mesh->GetRamData()->GetUVData(0) && texture_resource_array_.size() > 0) {
		TextureArray::iterator tx = texture_resource_array_.begin();
		for (; !texture && tx != texture_resource_array_.end(); ++tx) {
			const std::vector<str>& texture_list = clazz->GetMaterial(mesh_index).texture_list_;
			std::vector<str>::const_iterator y = texture_list.begin();
			for (; !texture && y != texture_list.end(); ++y) {
				if ((*tx)->GetName() == *y) {
					texture = *tx;
				}
			}
		}
		if (!texture && use_default_texture_) {
			texture = texture_resource_array_[0];
		}
	}
	if (post_load_material_delegate_) {
		post_load_material_delegate_(_mesh);
	}
	static const str blend("blend");
	static const str highlight("highlight");
	static const str env("env");
	static const str env_blend("env_blend");
	if (_mesh->GetRamData()->GetUVData(0) && texture) {
		const str shader = clazz->GetMaterial(mesh_index).shader_name_;
		const bool is_blended = (transparent || shader == blend);
		const bool is_highlight = (shader == highlight);
		const bool is_env = (shader == env);
		const bool is_env_blend = ((transparent && is_env) || shader == env_blend);
		uitbc::Renderer::MaterialType material_type = enable_pixel_shader_? uitbc::Renderer::kMatSingleTextureSolidPxs : uitbc::Renderer::kMatSingleTextureSolid;
		if (is_env_blend) {
			material_type = uitbc::Renderer::kMatSingleTextureEnvmapBlended;
		} else if (is_env) {
			material_type = uitbc::Renderer::kMatSingleTextureEnvmapSolid;
		} else if (is_highlight) {
			material_type = uitbc::Renderer::kMatSingleTextureHighlight;
		} else if (is_blended) {
			material_type = uitbc::Renderer::kMatSingleTextureBlended;
		}
		ui_manager_->GetRenderer()->ChangeMaterial(_mesh->GetData(), material_type);
		ui_manager_->GetRenderer()->TryAddGeometryTexture(_mesh->GetData(), texture->GetData());
	} else {
		uitbc::Renderer::MaterialType material_type = enable_pixel_shader_? uitbc::Renderer::kMatSingleColorSolidPxs : uitbc::Renderer::kMatSingleColorSolid;
		const str shader = clazz->GetMaterial(mesh_index).shader_name_;
		const bool is_blended = (transparent || shader == blend);
		const bool is_env = (shader == env);
		const bool is_env_blend = ((transparent && is_env) || shader == env_blend);
		if (is_env_blend) {
			material_type = uitbc::Renderer::kMatSingleColorEnvmapBlended;
		} else if (is_env) {
			material_type = uitbc::Renderer::kMatSingleColorEnvmapSolid;
		} else if (is_blended) {
			material_type = uitbc::Renderer::kMatSingleColorBlended;
		}
		ui_manager_->GetRenderer()->ChangeMaterial(_mesh->GetData(), material_type);
	}
}

void CppContextObject::SetPostLoadMaterialDelegate(const PostLoadMaterialDelegate& delegate) {
	post_load_material_delegate_ = delegate;
}



void CppContextObject::ReplaceTexture(int texture_index, const str& new_texture_name) {
	UserRendererImageResource* texture = texture_resource_array_[texture_index];
	if (texture->GetName() == new_texture_name) {
		return;
	}
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UserGeometryReferenceResource* _mesh = mesh_resource_array_[x];
		if (_mesh->GetLoadState() == cure::kResourceLoadComplete) {
			if (ui_manager_->GetRenderer()->DisconnectGeometryTexture(_mesh->GetData(), texture->GetData())) {
				ui_manager_->GetRenderer()->ChangeMaterial(_mesh->GetData(), uitbc::Renderer::kMatNull);
			}
		}
	}
	use_default_texture_ = true;
	--texture_load_count_;
	UserRendererImageResource* new_texture = new UserRendererImageResource(ui_manager_, ((const RendererImageResource*)texture->GetConstResource())->settings_);
	texture_resource_array_[texture_index] = new_texture;
	new_texture->Load(GetResourceManager(), new_texture_name,
		UserRendererImageResource::TypeLoadCallback(this, &CppContextObject::OnLoadTexture));
	delete texture;
}



GameUiManager* CppContextObject::GetUiManager() const {
	return (ui_manager_);
}

const tbc::ChunkyClass* CppContextObject::GetClass() const {
	if (ui_class_resource_ && ui_class_resource_->GetLoadState() == cure::kResourceLoadComplete) {
		return (ui_class_resource_->GetRamData());
	}
	return (0);
}



void CppContextObject::DoAddMeshResource(const str& mesh_name, tbc::GeometryBase* mesh, int casts_shadows) {
	const str _mesh_name = mesh_name + ".mesh";
	const str mesh_ref_name = strutil::Format("%s;%i", _mesh_name.c_str(), GetInstanceId());
	int phys_index = 0;
	xform transform;
	float scale = 1;
	UserGeometryReferenceResource* geometry_ref = new UserGeometryReferenceResource(ui_manager_, GeometryOffset(phys_index, transform, scale));
	mesh_resource_array_.push_back(geometry_ref);
	if (mesh) {
		GeometryReferenceResource* geometry_ref_resource = (GeometryReferenceResource*)geometry_ref->CreateResource(GetResourceManager(), mesh_ref_name);
		geometry_ref_resource->SetIsUnique(true);
		geometry_ref->SetResource(geometry_ref_resource);
		cure::UserResource::LoadCallback callback_cast;
		callback_cast.SetMemento(UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh).GetMemento());
		geometry_ref_resource->AddCaller(geometry_ref, callback_cast);
		geometry_ref_resource->SetLoadState(cure::kResourceLoadInProgress);	// We're waiting for the root resource to get loaded.
		geometry_ref_resource->SetCastsShadows(casts_shadows);
		GeometryReferenceResource::ClassResource* geometry = geometry_ref_resource->GetParent();
		GeometryResource* geometry_resource = (GeometryResource*)geometry->CreateResource(GetResourceManager(), _mesh_name);
		geometry_resource->SetIsUnique(true);
		geometry->SetResource(geometry_resource);
		geometry_resource->SetRamDataType(mesh);
		geometry_resource->SetLoadState(cure::kResourceLoadInProgress);	// Handle pushing to gfx card in postprocessing by some other thread at a later stage.
		GetResourceManager()->AddLoaded(geometry);
		GetResourceManager()->AddLoaded(geometry_ref);
	} else {
		geometry_ref->Load(GetResourceManager(),
			mesh_ref_name,
			UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh),
			false);
		((GeometryReferenceResource*)geometry_ref->GetResource())->SetCastsShadows(casts_shadows);
	}
}



void CppContextObject::OnLoadClass(UserClassResource* class_resource) {
	uitbc::ChunkyClass* clazz = class_resource->GetData();
	if (class_resource->GetLoadState() != cure::kResourceLoadComplete) {
		log_.Errorf("Could not load class '%s'.", class_resource->GetName().c_str());
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
		return;
	}

	const size_t mesh_count = clazz->GetMeshCount();
	if (enable_ui_) {
		deb_assert(mesh_count > 0);
		for (size_t x = 0; x < mesh_count; ++x) {
			int phys_index = -1;
			str _mesh_name;
			xform transform;
			float scale;
			clazz->GetMesh(x, phys_index, _mesh_name, transform, scale);
			UserGeometryReferenceResource* _mesh = new UserGeometryReferenceResource(
				ui_manager_, GeometryOffset(phys_index, transform, scale));
			mesh_resource_array_.push_back(_mesh);
		}
	}

	StartLoadingPhysics(clazz->GetPhysicsBaseName());

	if (!enable_ui_) {
		return;
	}

	deb_assert(mesh_load_count_ == 0);
	//deb_assert(texture_load_count_ == 0);
	deb_assert(mesh_count == mesh_resource_array_.size());
	size_t x = 0;
	MeshArray::iterator y = mesh_resource_array_.begin();
	for (; y != mesh_resource_array_.end(); ++x, ++y) {
		int phys_index = -1;
		str _mesh_name;
		xform transform;
		float scale;
		clazz->GetMesh(x, phys_index, _mesh_name, transform, scale);
		str mesh_instance;
		strutil::strvec mesh_name_list = strutil::Split(_mesh_name, ";", 1);
		_mesh_name = mesh_name_list[0];
		if (mesh_name_list.size() == 1) {
			mesh_instance = strutil::Format("%s", GetMeshInstanceId().c_str());
		} else {
			mesh_instance = strutil::Format("%s_%s", mesh_name_list[1].c_str(), GetMeshInstanceId().c_str());
		}
		// TRICKY: load non-unique, since this mesh reference is shared. However, we set it as
		// "don't keep", which makes sure it doesn't get cached. Example: client 0's car 1 mesh
		// is the same as client 1's car 1 mesh. But when car 1 dies, the mesh REFERENCE should
		// also die immediately. (The MESH, on the other hand, is a totally different topic.)
		(*y)->Load(GetResourceManager(),
			strutil::Format("%s.mesh;%s", _mesh_name.c_str(), mesh_instance.c_str()),
			UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh),
			false);
	}
	LoadTextures();
}

void CppContextObject::LoadTextures() {
	const uitbc::ChunkyClass* clazz = ((uitbc::ChunkyClass*)ui_class_resource_->GetRamData());
	deb_assert(clazz);
	for (size_t x = 0; x < clazz->GetMeshCount(); ++x) {
		const Canvas::ResizeHint _resize_hint = clazz->GetMaterial(x).resize_hint_;
		const std::vector<str>& texture_list = clazz->GetMaterial(x).texture_list_;
		for (std::vector<str>::const_iterator y = texture_list.begin(); y != texture_list.end(); ++y) {
			LoadTexture(*y, _resize_hint);
		}
	}
}

void CppContextObject::OnLoadMesh(UserGeometryReferenceResource* mesh_resource) {
	DispatchOnLoadMesh(mesh_resource);
}

void CppContextObject::DispatchOnLoadMesh(UserGeometryReferenceResource* mesh_resource) {
	++mesh_load_count_;
	if (mesh_resource->GetLoadState() == cure::kResourceLoadComplete) {
		size_t _mesh_index = (size_t)-1;
		for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
			if (mesh_resource_array_[x] == mesh_resource) {
				_mesh_index = x;
				break;
			}
		}
		deb_assert((int)_mesh_index >= 0);
		if (_mesh_index == 0 && !allow_root_shadow_) {
			ui_manager_->GetRenderer()->SetShadows(mesh_resource->GetData(), uitbc::Renderer::kForceNoShadows);
		}
		if (ui_class_resource_) {
			const uitbc::ChunkyClass::Material& loaded_material =
				((uitbc::ChunkyClass*)ui_class_resource_->GetRamData())->GetMaterial(_mesh_index);
			tbc::GeometryBase::BasicMaterialSettings material(loaded_material.ambient_,
				loaded_material.diffuse_, loaded_material.specular_,
				loaded_material.shininess_, loaded_material.alpha_, loaded_material.smooth_);
			mesh_resource->GetRamData()->SetBasicMaterialSettings(material);
		}

		((tbc::GeometryReference*)mesh_resource->GetRamData())->SetOffsetTransformation(mesh_resource->GetOffset().offset_);
		mesh_resource->GetRamData()->SetScale(mesh_resource->GetOffset().scale_);
		TryAddTexture();
	} else {
		log_.Error("Could not load mesh! Sheit.");
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
	}
}

void CppContextObject::OnLoadTexture(UserRendererImageResource* texture_resource) {
	++texture_load_count_;
	if (texture_resource->GetLoadState() == cure::kResourceLoadComplete) {
		TryAddTexture();
	} else {
		log_.Errorf("Could not load texture %s. Gah!", texture_resource->GetName().c_str());
		deb_assert(false);
	}
}

void CppContextObject::TryAddTexture() {
	if (!ui_manager_->CanRender()) {
		return;
	}
	for (size_t x = 0; x < texture_resource_array_.size(); ++x) {
		UserRendererImageResource* texture = texture_resource_array_[x];
		if (texture->GetLoadState() != cure::kResourceLoadComplete) {
			return;
		}
	}
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UserGeometryReferenceResource* _mesh = mesh_resource_array_[x];
		if (_mesh->GetLoadState() != cure::kResourceLoadComplete) {
			return;
		}
	}
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UserGeometryReferenceResource* _mesh = mesh_resource_array_[x];
		if (ui_manager_->GetRenderer()->GetMaterialType(_mesh->GetData()) == uitbc::Renderer::kMatNull) {
			UpdateMaterial(x);
		}
	}
	TryComplete();
}

bool CppContextObject::TryComplete() {
	// Meshes/textures must be both 1) all attempted, and 2) all loaded OK. Plus, server (or otherwise
	// headless) should totally ignore this.
	if (enable_ui_ &&
		(texture_load_count_ != texture_resource_array_.size() || mesh_load_count_ != mesh_resource_array_.size())) {
		return (false);
	}
	for (size_t x = 0; x < texture_resource_array_.size(); ++x) {
		UserRendererImageResource* texture = texture_resource_array_[x];
		if (texture->GetLoadState() != cure::kResourceLoadComplete) {
			return (false);
		}
	}
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UserGeometryReferenceResource* _mesh = mesh_resource_array_[x];
		if (_mesh->GetLoadState() != cure::kResourceLoadComplete) {
			return (false);
		}
	}

	if (!Parent::TryComplete()) {
		return (false);
	}

	UiMove();

	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UserGeometryReferenceResource* _mesh = mesh_resource_array_[x];
		_mesh->GetRamData()->SetAlwaysVisible(true);
	}

	return (true);
}

str CppContextObject::GetMeshInstanceId() const {
	return (strutil::IntToString(GetInstanceId(), 10));
}



loginstance(kGameContextCpp, CppContextObject);



}
