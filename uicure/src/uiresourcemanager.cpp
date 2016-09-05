
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiresourcemanager.h"
#include "../../lepra/include/lepraassert.h"
#include "../../cure/include/terrainfunctionmanager.h"
#include "../../lepra/include/memfile.h"
#include "../../uitbc/include/uitrianglebasedgeometry.h"
#include "../include/uicure.h"
#include "../include/uigameuimanager.h"



namespace UiCure {



UiResource::UiResource(GameUiManager* ui_manager):
	ui_manager_(ui_manager) {
}

UiResource::~UiResource() {
}

GameUiManager* UiResource::GetUiManager() const {
	return (ui_manager_);
}



// ----------------------------------------------------------------------------



PainterImageResource::PainterImageResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name):
	Parent(manager, name),
	UiResource(ui_manager),
	release_mode_(kReleaseDelete) {
}

PainterImageResource::PainterImageResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name, ImageReleaseMode release_mode):
	Parent(manager, name),
	UiResource(ui_manager),
	release_mode_(release_mode) {
}

PainterImageResource::~PainterImageResource() {
	// TRICKY: delete of data must lie in this class, and not in parent class RamResource.
	// This is so since types (uilepra::SoundManager::SoundID) cannot be deleted!
	SetRamData(0);

	if (optimized_data_ != uitbc::Painter::kInvalidImageid) {
		GetUiManager()->GetPainter()->RemoveImage(optimized_data_);
		optimized_data_ = uitbc::Painter::kInvalidImageid;
	}
}

const str PainterImageResource::GetType() const {
	return ("PaintImg");
}

PainterImageResource::UserData PainterImageResource::GetUserData(const cure::UserResource*) const {
	return (optimized_data_);
}

bool PainterImageResource::Load() {
	//deb_assert(!IsUnique());
	deb_assert(GetRamData() == 0);
	SetRamData(new Canvas());
	File* file = GetManager()->QueryFile(GetName());
	bool ok = (file != 0);
	if (ok) {
		ImageLoader loader;
		ok = loader.Load(ImageLoader::GetFileTypeFromName(GetName()), *file, *GetRamData());
	}
	delete file;
	return ok;
}

cure::ResourceLoadState PainterImageResource::PostProcess() {
	deb_assert(optimized_data_ == uitbc::Painter::kInvalidImageid);
	optimized_data_ = GetUiManager()->GetPainter()->AddImage(GetRamData(), 0);
	deb_assert(optimized_data_ != uitbc::Painter::kInvalidImageid);
	switch (release_mode_) {
		case kReleaseDelete:		SetRamData(0);			break;
		case kReleaseFreeBuffer:	GetRamData()->SetBuffer(0);	break;
		case kReleaseNone:						break;
	}
	cure::ResourceLoadState load_state;
	if (optimized_data_ == uitbc::Painter::kInvalidImageid) {
		load_state = cure::kResourceLoadError;
	} else {
		load_state = cure::kResourceLoadComplete;
	}

	Parent::PostProcess();

	return (load_state);
}



// ----------------------------------------------------------------------------



RendererImageBaseResource::RendererImageBaseResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name):
	Parent(manager, name),
	UiResource(ui_manager) {
}

RendererImageBaseResource::~RendererImageBaseResource() {
	// TRICKY: delete of data must lie in this class, and not in parent class RamResource.
	// This is so since types (uilepra::SoundManager::SoundID) cannot be deleted!
	SetRamData(0);

	if (optimized_data_ != uitbc::Renderer::INVALID_TEXTURE) {
		GetUiManager()->GetRenderer()->RemoveTexture(optimized_data_);
		optimized_data_ = uitbc::Renderer::INVALID_TEXTURE;
	}
}

RendererImageBaseResource::UserData RendererImageBaseResource::GetUserData(const cure::UserResource*) const {
	return (optimized_data_);
}

cure::ResourceLoadState RendererImageBaseResource::PostProcess() {
	deb_assert(optimized_data_ == uitbc::Renderer::INVALID_TEXTURE);
	optimized_data_ = GetUiManager()->GetRenderer()->AddTexture(GetRamData());
	deb_assert(optimized_data_ != uitbc::Renderer::INVALID_TEXTURE);
	SetRamData(0);
	cure::ResourceLoadState load_state;
	if (optimized_data_ == uitbc::Renderer::INVALID_TEXTURE) {
		load_state = cure::kResourceLoadError;
	} else {
		load_state = cure::kResourceLoadComplete;
	}

	Parent::PostProcess();

	return (load_state);
}

RendererImageResource::RendererImageResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name, const ImageProcessSettings& settings):
	RendererImageBaseResource(ui_manager, manager, name),
	settings_(settings) {
}

RendererImageResource::~RendererImageResource() {
	// Handled by parent.
}

const str RendererImageResource::GetType() const {
	return ("RenderImg");
}

bool RendererImageResource::Load() {
	deb_assert(!IsUnique());
	deb_assert(GetRamData() == 0);
	Canvas image;
	File* file = GetManager()->QueryFile(GetName());
	bool ok = (file != 0);
	if (ok) {
		ImageLoader loader;
		ok = loader.Load(ImageLoader::GetFileTypeFromName(GetName()), *file, image);
	}
	delete file;
	if (ok) {
		SetRamData(new uitbc::Texture(image, settings_.resize_hint_, settings_.mip_map_? 0 : 1));
	}
	return (ok);
}



// ----------------------------------------------------------------------------



/*TextureResource::TextureResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name):
	RendererImageBaseResource(ui_manager, manager, name) {
}

TextureResource::~TextureResource() {
	// Handled by parent.
}

const str TextureResource::GetType() const {
	return ("Texture");
}

bool TextureResource::Load() {
	deb_assert(!IsUnique());
	deb_assert(GetRamData() == 0);
	SetRamData(new uitbc::Texture());
	uitbc::TEXLoader loader;
	bool ok = false;
	for (int x = 0; x < 3 && !ok; ++x) {	// Retry file loading, file might be held by anti-virus/Windoze/similar shit.
		ok = (loader.Load(GetName(), *GetRamData(), true) == uitbc::TEXLoader::kStatusSuccess);
	}
	return ok;
}*/



// ----------------------------------------------------------------------------



GeometryResource::GeometryResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name):
	Parent(manager, name),
	UiResource(ui_manager),
	casts_shadows_(0) {
}

GeometryResource::~GeometryResource() {
	ReleaseGeometry();
}

void GeometryResource::ReleaseGeometry() {
	if (optimized_data_) {
		GetUiManager()->GetRenderer()->RemoveGeometry(optimized_data_);
		optimized_data_ = uitbc::Renderer::INVALID_GEOMETRY;
	}
	SetRamData(0);
	SetLoadState(cure::kResourceUnloaded);
}

const str GeometryResource::GetType() const {
	return ("Geometry");
}

GeometryResource::UserData GeometryResource::GetUserData(const cure::UserResource*) const {
	return (optimized_data_);
}

bool GeometryResource::Load() {
	deb_assert(!IsUnique());

	//float lCubeMappingScale = -1;
	tbc::GeometryBase::BasicMaterialSettings material;
	material.SetColor(0.5f, 0.5f, 0.5f);

	uitbc::TriangleBasedGeometry* geometry = 0;

	//strutil::strvec parts = strutil::Split(GetName(), ";");
	//const str& filename = parts[0];
	const str& filename = GetName();

	File* file = GetManager()->QueryFile(filename);
	bool ok = (file != 0);
	if (ok) {
		uitbc::ChunkyMeshLoader loader(file, false);
		geometry = new uitbc::TriangleBasedGeometry();
		LEPRA_DEBUG_CODE(geometry->name_ = filename);
		ok = loader.Load(geometry, casts_shadows_);
		if (!ok) {
			deb_assert(false);
			delete (geometry);
			geometry = 0;
		}
	}
	delete file;
	file = 0;
	deb_assert(geometry);

	if (geometry) {
		geometry->SetBasicMaterialSettings(material);
		/*if (lCubeMappingScale > 0) {
			geometry->AddEmptyUVSet();
			Vector2DD uv_offset(0.5, 0.5);
			uitbc::UVMapper::ApplyCubeMapping(geometry, 0, lCubeMappingScale, uv_offset);
			//geometry->DupUVSet(0);
		}*/

		SetRamData(geometry);
	}
	return (geometry != 0);
}

cure::ResourceLoadState GeometryResource::PostProcess() {
	typedef uitbc::Renderer R;
	deb_assert(optimized_data_ == R::INVALID_GEOMETRY);
	if (!GetUiManager()->CanRender()) {
		return cure::kResourceLoadInProgress;
	}
	R::Shadows shadows = R::kNoShadows;
	switch (casts_shadows_) {
		case 1:		shadows = R::kCastShadows;	break;
		case -1:	shadows = R::kForceNoShadows;	break;
	}
	log_volatile(log_.Debugf("%s has casts shadows = %i.", GetName().c_str(), casts_shadows_));
	optimized_data_ = GetUiManager()->GetRenderer()->AddGeometry(GetRamData(), R::kMatNull, shadows);
	deb_assert(optimized_data_ != R::INVALID_GEOMETRY);
	cure::ResourceLoadState load_state;
	if (optimized_data_ == R::INVALID_GEOMETRY) {
		load_state = cure::kResourceLoadError;
	} else {
		load_state = cure::kResourceLoadComplete;
	}

	Parent::PostProcess();

	return (load_state);
}

int GeometryResource::GetCastsShadows() const {
	return (casts_shadows_);
}

void GeometryResource::SetCastsShadows(int casts_shadows) {
	casts_shadows_ = casts_shadows;
}

loginstance(kUiGfx3D, GeometryResource);



// ----------------------------------------------------------------------------



GeometryReferenceResource::GeometryReferenceResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name):
	GeometryResource(ui_manager, manager, name),
	class_resource_(new ClassResource(ui_manager)) {
}

GeometryReferenceResource::~GeometryReferenceResource() {
	ReleaseGeometry();
}

void GeometryReferenceResource::ReleaseGeometry() {
	delete (class_resource_);
	class_resource_ = 0;
	Parent::ReleaseGeometry();
}

const str GeometryReferenceResource::GetType() const {
	return ("GeometryRef");
}

bool GeometryReferenceResource::IsReferenceType() const {
	return true;
}

GeometryReferenceResource::ClassResource* GeometryReferenceResource::GetParent() const {
	return class_resource_;
}

bool GeometryReferenceResource::Load() {
	//deb_assert(IsUnique());
	bool ok = (class_resource_ != 0);
	deb_assert(ok);
	if (ok) {
		const str filename = strutil::Split(GetName(), ";", 1)[0];
		deb_assert(filename != GetName());
		class_resource_->Load(GetManager(), filename, ClassResource::TypeLoadCallback(this,
			&GeometryReferenceResource::OnLoadClass));
	}
	return (ok);
}

cure::ResourceLoadState GeometryReferenceResource::PostProcess() {
	deb_assert(class_resource_);
	return (CreateInstance());
}

cure::ResourceLoadState GeometryReferenceResource::CreateInstance() {
	deb_assert(class_resource_);
	cure::ResourceLoadState load_state = class_resource_->GetLoadState();
	if (load_state == cure::kResourceLoadComplete) {
		SetRamData(new tbc::GeometryReference(class_resource_->GetRamData()));
		//GetRamData()->SetAlwaysVisible(true);
		//GetRamData()->SetBasicMaterialSettings(class_resource_->GetRamData()->GetBasicMaterialSettings());
		load_state = Parent::PostProcess();
	}
	return (load_state);
}

void GeometryReferenceResource::Resume() {
	deb_assert(!class_resource_);
	class_resource_ = new ClassResource(GetUiManager());
}

void GeometryReferenceResource::Suspend() {
	ReleaseGeometry();
}

void GeometryReferenceResource::OnLoadClass(ClassResource* resource) {
	if (casts_shadows_ == 0) {
		casts_shadows_ = ((const GeometryResource*)resource->GetConstResource())->GetCastsShadows();
	}
}

loginstance(kUiGfx3D, GeometryReferenceResource);



// ----------------------------------------------------------------------------



UserGeometryReferenceResource::UserGeometryReferenceResource(GameUiManager* ui_manager, const GeometryOffset& offset):
	UiResource(ui_manager),
	offset_(offset) {
}

UserGeometryReferenceResource::~UserGeometryReferenceResource() {
}

const GeometryOffset& UserGeometryReferenceResource::GetOffset() const {
	return (offset_);
}

cure::Resource* UserGeometryReferenceResource::CreateResource(cure::ResourceManager* manager, const str& name) const {
	return (new GeometryReferenceResource(GetUiManager(), manager, name));
}

loginstance(kUiGfx3D, UserGeometryReferenceResource);



// ----------------------------------------------------------------------------



SoundResource::SoundResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name,
	SoundDimension dimension, LoopMode loop_mode):
	Parent(manager, name),
	UiResource(ui_manager),
	dimension_(dimension),
	loop_mode_(loop_mode) {
	SetRamDataType(uilepra::INVALID_SOUNDID);
}

SoundResource::~SoundResource() {
	Release();
}

void SoundResource::Release() {
	UserDataTable::iterator x = user_diversified_table_.begin();
	for (; x != user_diversified_table_.end(); ++x) {
		UserData instance_id = x->second;
		GetUiManager()->GetSoundManager()->DeleteSoundInstance(instance_id);
	}
	user_diversified_table_.clear();
	if (GetRamData() != uilepra::INVALID_SOUNDID) {
		GetUiManager()->GetSoundManager()->Release(GetRamData());
		SetRamDataType(uilepra::INVALID_SOUNDID);
	}
}

void SoundResource::PatchInfo(cure::ResourceInfo& info) const {
	info.type_ += strutil::Format(" (%u instances)", GetDiversifiedData().size());
}

bool SoundResource::Load() {
	deb_assert(!IsUnique());
	deb_assert(GetRamData() == uilepra::INVALID_SOUNDID);
	File* file = GetManager()->QueryFile(GetName());
	bool ok = (file != 0);
	bool own_data = false;
	void* _data = 0;
	size_t data_size = 0;
	if (ok) {
		data_size = (size_t)file->GetSize();
		MemFile* mem_file = dynamic_cast<MemFile*>(file);
		if (mem_file) {
			_data = mem_file->GetBuffer();
		} else {
			ok = (file->AllocReadData(&_data, data_size) == kIoOk);
			own_data = true;
		}
	}
	if (ok) {
		uilepra::SoundManager::SoundID sound_id;
		if (dimension_ == kDimension2D) {
			sound_id = GetUiManager()->GetSoundManager()->LoadSound2D(GetName(), _data, data_size, loop_mode_, 0);
		} else {
			sound_id = GetUiManager()->GetSoundManager()->LoadSound3D(GetName(), _data, data_size, loop_mode_, 0);
		}
		ok = (sound_id != uilepra::INVALID_SOUNDID);
		deb_assert(ok);
		if (ok) {
			SetRamDataType(sound_id);
		}
	}
	if (own_data) {
		delete (char*)_data;
	}
	delete file;
	return ok;
}

SoundResource::UserData SoundResource::CreateDiversifiedData() const {
	UserData instance_id = GetUiManager()->GetSoundManager()->CreateSoundInstance(GetRamData());
	if (!instance_id) {
		log_.Errorf("Unable to create diversified sound data for %s!", GetName().c_str());
	}
	return (instance_id);
}

void SoundResource::ReleaseDiversifiedData(UserData data) const {
	GetUiManager()->GetSoundManager()->DeleteSoundInstance(data);
}


loginstance(kUiSound, SoundResource);



SoundResource2d::SoundResource2d(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name,
	LoopMode loop_mode):
	SoundResource(ui_manager, manager, name, kDimension2D, loop_mode) {
}

const str SoundResource2d::GetType() const {
	return ("Sound2D");
}

SoundResource3d::SoundResource3d(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name,
	LoopMode loop_mode):
	SoundResource(ui_manager, manager, name, kDimension3D, loop_mode) {
}

const str SoundResource3d::GetType() const {
	return ("Sound3D");
}



// ----------------------------------------------------------------------------



ClassResource::ClassResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name):
	Parent(manager, name),
	UiResource(ui_manager) {
}

ClassResource::~ClassResource() {
}

bool ClassResource::Load() {
	return LoadWithName(Resource::GetName().substr(3));	// TRICKY: cut out "UI:" prefix (to distinguish from those used by non-UiCure classes).
}



/*AnimationResource::AnimationResource(const str& name):
	OptimizedResource(name) {
}

bool AnimationResource::Load() {
}*/



}
