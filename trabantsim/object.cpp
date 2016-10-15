
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "object.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/gamemanager.h"
#include "../cure/include/gameticker.h"
#include "../cure/include/timemanager.h"
#include "../uitbc/include/uitrianglebasedgeometry.h"



namespace TrabantSim {



using namespace cure;



Object::Object(ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	generated_physics_(0),
	physics_index_(0),
	same_as_previous_(false),
	scale_(1,1,1),
	clazz_(new uitbc::ChunkyClass) {
}

Object::~Object() {
	generated_physics_ = 0;	// Will auto-delete when physics goes missing.
	delete clazz_;
}



const tbc::ChunkyClass* Object::GetClass() const {
	return clazz_;
}

void Object::CreatePhysics(tbc::ChunkyPhysics* physics) {
	deb_assert(physics_resource_ == 0);

	const str physics_name = strutil::Format("RawPhys%i.phys", physics_index_);
	const str physics_ref_name = strutil::Format("%s;%i", physics_name.c_str(), GetInstanceId());
	PhysicsSharedInitData init_data(position_.position_.transformation_, position_.position_.velocity_, physics_override_,
		((GameTicker*)manager_->GetGameManager()->GetTicker())->GetPhysicsLock(), manager_->GetGameManager()->GetPhysicsManager(),
		manager_->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps(), GetInstanceId());
	physics_resource_ = new UserPhysicsReferenceResource(init_data);
	UserPhysicsReferenceResource* user_physics_ref = physics_resource_;
	PhysicsSharedResource* physics_ref_resource = (PhysicsSharedResource*)user_physics_ref->CreateResource(GetResourceManager(), physics_ref_name);
	user_physics_ref->SetResource(physics_ref_resource);
	physics_ref_resource->SetIsUnique(true);
	UserResource::LoadCallback callback_cast;
	callback_cast.SetMemento(UserPhysicsReferenceResource::TypeLoadCallback(this, &Object::OnLoadPhysics).GetMemento());
	physics_ref_resource->AddCaller(user_physics_ref, callback_cast);
	tbc::ChunkyPhysics* copy = new tbc::ChunkyPhysics(*physics);
	physics_ref_resource->SetRamDataType(copy);
	PhysicsSharedResource::ClassResource* user_physics = physics_ref_resource->GetParent();
	PhysicsResource* physics_resource = (PhysicsResource*)GetResourceManager()->ReferenceResource(physics_name);
	if (!physics_resource) {
		physics_resource = (PhysicsResource*)user_physics->CreateResource(GetResourceManager(), physics_name);
		physics_resource->SetIsUnique(true);
		physics_resource->SetRamDataType(physics);
		user_physics->SetResource(physics_resource);
		physics_resource->SetLoadState(kResourceLoadInProgress);	// Handle pushing to physics engine in postprocessing by some other thread at a later stage.
		GetResourceManager()->AddLoaded(user_physics);
	} else {
		user_physics->SetResource(physics_resource);
	}
	physics_ref_resource->SetLoadState(kResourceLoadInProgress);	// We're waiting for the root resource to get loaded.
	GetResourceManager()->AddLoaded(user_physics_ref);
}

uitbc::TriangleBasedGeometry* Object::CreateGfxMesh(const std::vector<float>& vertices, const std::vector<int>& indices, const vec3& color, float alpha, bool is_smooth) {
	if (vertices.empty() || indices.empty()) {
		return 0;
	}
	uitbc::TriangleBasedGeometry* gfx_mesh = new uitbc::TriangleBasedGeometry(&vertices[0], 0, 0, 0, uitbc::TriangleBasedGeometry::kColorRgb, (const uint32*)&indices[0],
			vertices.size()/3, indices.size(), tbc::GeometryBase::kTriangles, tbc::GeometryBase::kGeomStatic);
	if (!is_smooth) {
		gfx_mesh->SplitVertices();
		gfx_mesh->ClearVertexNormalData();
		gfx_mesh->ClearSurfaceNormalData();
	}
	gfx_mesh->GetBasicMaterialSettings().diffuse_	= color;
	gfx_mesh->GetBasicMaterialSettings().specular_	= vec3();
	gfx_mesh->GetBasicMaterialSettings().shininess_	= !is_smooth;
	gfx_mesh->GetBasicMaterialSettings().smooth_	= is_smooth;
	gfx_mesh->GetBasicMaterialSettings().alpha_	= alpha;
	gfx_mesh->SetGeometryVolatility(tbc::GeometryBase::kGeomSemiStatic);
	return gfx_mesh;
}

void Object::AddMeshInfo(const str& mesh_name, const str& shader, const str& texture, const vec3& color, float alpha, bool is_smooth) {
	clazz_->AddMesh(0, mesh_name, xform(), 1);
	uitbc::ChunkyClass::Material material;
	material.diffuse_ = color;
	material.shininess_ = 1;
	material.alpha_ = alpha;
	material.smooth_ = is_smooth;
	material.resize_hint_ = Canvas::kResizeFast;
	material.texture_list_.push_back(texture);
	material.shader_name_ = shader;
	clazz_->SetLastMeshMaterial(material);
}



void Object::OnLoaded() {
	Parent::OnLoaded();
	const uitbc::ChunkyClass::Material& material = clazz_->GetMaterial(0);
	GetMesh(0)->GetBasicMaterialSettings().diffuse_ = material.diffuse_;
	if (GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kStatic) {
		GetManager()->DisableTickCallback(this);
	}
}



void Object::OnLoadPhysics(UserPhysicsReferenceResource* physics_resource) {
	Parent::OnLoadPhysics(physics_resource);
}



}
