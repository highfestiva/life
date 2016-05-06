
// Author: Jonas Byström
// Copyright (c) 2002-, Pixel Doctrine



#pragma once

#include "../../cure/include/resourcemanager.h"
#include "../../lepra/include/canvas.h"
#include "../../tbc/include/geometryreference.h"
#include "../../uilepra/include/uisoundmanager.h"
#include "../../uitbc/include/uichunkyclass.h"
#include "../../uitbc/include/uichunkyloader.h"
#include "../../uitbc/include/uipainter.h"
#include "../../uitbc/include/uirenderer.h"
#include "uicure.h"



namespace UiCure {



class GameUiManager;



class UiResource {
protected:
	UiResource(GameUiManager* ui_manager);
	virtual ~UiResource();

	GameUiManager* GetUiManager() const;

private:
	GameUiManager* ui_manager_;
};



template<class ResourceType>
class UserUiTypeResource: public cure::UserTypeResourceBase<UserUiTypeResource<ResourceType>, ResourceType>,
	public UiResource {
public:
	UserUiTypeResource(GameUiManager* ui_manager);
	virtual ~UserUiTypeResource();

	cure::Resource* CreateResource(cure::ResourceManager* manager, const str& name) const;
};



template<class ResourceType, class SubtypeExtraType>
class UserUiExtraTypeResource:
	public cure::UserTypeResourceBase<UserUiExtraTypeResource<ResourceType, SubtypeExtraType>, ResourceType>,
	public UiResource {
public:
	typedef SubtypeExtraType ExtraType;

	UserUiExtraTypeResource(GameUiManager* ui_manager, const ExtraType& extra_data);
	virtual ~UserUiExtraTypeResource();

	ExtraType& GetExtraData() const;
	void SetExtraData(const ExtraType& extra_data);

	cure::Resource* CreateResource(cure::ResourceManager* manager, const str& name) const;

private:
	mutable ExtraType extra_data_;
};



class PainterImageResource: public cure::OptimizedResource<Canvas*, uitbc::Painter::ImageID>, public UiResource {
	typedef cure::OptimizedResource<Canvas*, uitbc::Painter::ImageID> Parent;
public:
	enum ImageReleaseMode {
		kReleaseDelete = 1,
		kReleaseFreeBuffer,
		kReleaseNone,
	};
	typedef uitbc::Painter::ImageID UserData;

	PainterImageResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name);
	PainterImageResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name, ImageReleaseMode release_mode);
	virtual ~PainterImageResource();
	const str GetType() const;
	UserData GetUserData(const cure::UserResource* user_resource) const;
	bool Load();
	cure::ResourceLoadState PostProcess();

private:
	ImageReleaseMode release_mode_;
};



class RendererImageBaseResource: public cure::OptimizedResource<uitbc::Texture*, uitbc::Renderer::TextureID>, public UiResource {
	typedef cure::OptimizedResource<uitbc::Texture*, uitbc::Renderer::TextureID> Parent;
public:
	typedef uitbc::Renderer::TextureID UserData;

	UserData GetUserData(const cure::UserResource* user_resource) const;

	cure::ResourceLoadState PostProcess();

protected:
	RendererImageBaseResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name);
	virtual ~RendererImageBaseResource();
};

struct ImageProcessSettings {
	Canvas::ResizeHint resize_hint_;
	bool mip_map_;
	inline ImageProcessSettings(Canvas::ResizeHint resize_hint, bool mip_map):
		resize_hint_(resize_hint),
		mip_map_(mip_map) {
	}
};

class RendererImageResource: public RendererImageBaseResource {
public:
	RendererImageResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name, const ImageProcessSettings& load_settings);
	virtual ~RendererImageResource();
	const str GetType() const;
	bool Load();

	ImageProcessSettings settings_;
};

/*class TextureResource: public RendererImageBaseResource {
public:
	TextureResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name);
	virtual ~TextureResource();
	const str GetType() const;
	bool Load();
};*/



class GeometryResource: public cure::OptimizedResource<tbc::GeometryBase*, uitbc::Renderer::GeometryID>, public UiResource {
	typedef cure::OptimizedResource<tbc::GeometryBase*, uitbc::Renderer::GeometryID> Parent;
public:
	typedef uitbc::Renderer::GeometryID UserData;

	GeometryResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name);
	virtual ~GeometryResource();
	void ReleaseGeometry();
	const str GetType() const;

	UserData GetUserData(const cure::UserResource*) const;
	bool Load();
	cure::ResourceLoadState PostProcess();

	int GetCastsShadows() const;
	void SetCastsShadows(int casts_shadows);

protected:
	int casts_shadows_;

private:
	logclass();
};



class GeometryReferenceResource: public GeometryResource {
	typedef GeometryResource Parent;
public:
	typedef UserUiTypeResource<GeometryResource> ClassResource;

	GeometryReferenceResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name);
	virtual ~GeometryReferenceResource();
	void ReleaseGeometry();
	const str GetType() const;

	bool IsReferenceType() const;
	ClassResource* GetParent() const;

	bool Load();
	cure::ResourceLoadState PostProcess();
	cure::ResourceLoadState CreateInstance();

private:
	void Resume();
	void Suspend();

	void OnLoadClass(ClassResource*);
	ClassResource* class_resource_;

	logclass();
};

struct GeometryOffset {
	GeometryOffset(unsigned physics_node_id):
		geometry_index_(physics_node_id),
		scale_(1) {
	}
	GeometryOffset(unsigned physics_node_id, vec3 offset):
		geometry_index_(physics_node_id),
		scale_(1) {
		offset_.SetPosition(offset);
	}
	GeometryOffset(unsigned physics_node_id, xform offset, float scale):
		geometry_index_(physics_node_id),
		offset_(offset),
		scale_(scale) {
	}

	unsigned geometry_index_;
	xform offset_;
	float scale_;
};



class UserGeometryReferenceResource: public cure::UserTypeResourceBase<
	UserGeometryReferenceResource, GeometryReferenceResource>, public UiResource {
	typedef cure::UserTypeResourceBase<UserGeometryReferenceResource, GeometryReferenceResource> Parent;
public:
	UserGeometryReferenceResource(GameUiManager* ui_manager, const GeometryOffset& offset = GeometryOffset(0));
	virtual ~UserGeometryReferenceResource();

	const GeometryOffset& GetOffset() const;

	cure::Resource* CreateResource(cure::ResourceManager* manager, const str& name) const;

	GeometryOffset offset_;
	logclass();
};



class SoundResource: public cure::DiversifiedResource<uilepra::SoundManager::SoundID, uilepra::SoundManager::SoundInstanceID>, public UiResource {
	typedef cure::DiversifiedResource<uilepra::SoundManager::SoundID, uilepra::SoundManager::SoundInstanceID> Parent;
public:
	enum SoundDimension {
		kDimension2D = 1,
		kDimension3D = 2,
	};
	typedef uilepra::SoundManager::SoundInstanceID UserData;
	typedef uilepra::SoundManager::LoopMode LoopMode;

	void Release();
	bool Load();
	virtual UserData CreateDiversifiedData() const;
	virtual void ReleaseDiversifiedData(UserData data) const;

protected:
	SoundResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name,
		SoundDimension dimension, LoopMode loop_mode);
	virtual ~SoundResource();

	virtual void PatchInfo(cure::ResourceInfo& info) const;

private:
	SoundDimension dimension_;
	LoopMode loop_mode_;

	logclass();
};

class SoundResource2d: public SoundResource {
public:
	SoundResource2d(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name,
		LoopMode loop_mode);
	const str GetType() const;
};

class SoundResource3d: public SoundResource {
public:
	SoundResource3d(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name,
		LoopMode loop_mode);
	const str GetType() const;
};



class ClassResource: public cure::ClassResourceBase<uitbc::ChunkyClass, uitbc::ChunkyClassLoader>, public UiResource {
	typedef cure::ClassResourceBase<uitbc::ChunkyClass, uitbc::ChunkyClassLoader> Parent;
public:
	ClassResource(GameUiManager* ui_manager, cure::ResourceManager* manager, const str& name);
	virtual ~ClassResource();

	virtual bool Load();
};



/*class AnimationResource: public OptimizedResource<void*, int>	// Perhaps a animation ID may be used on graphics accelerated hardware (i.e. PhysX or PS3)?
{
public:
	typedef void* UserData;

	AnimationResource(const str& name);

	bool Load();
};*/



/*class TerrainResource: public ???Resource<void*>	// ???
{
public:
	TerrainResource(const str& name);

	bool Load();
};*/



typedef UserUiTypeResource<PainterImageResource>						UserPainterImageResource;
typedef UserUiExtraTypeResource<PainterImageResource, PainterImageResource::ImageReleaseMode>	UserPainterKeepImageResource;
typedef UserUiExtraTypeResource<RendererImageResource, ImageProcessSettings>			UserRendererImageResource;
//typedef UserUiTypeResource<TextureResource>							UserTextureResource;
typedef UserUiExtraTypeResource<SoundResource2d, SoundResource::LoopMode>			UserSound2dResource;
typedef UserUiExtraTypeResource<SoundResource3d, SoundResource::LoopMode>			UserSound3dResource;
typedef UserUiTypeResource<ClassResource>							UserClassResource;
//typedef cure::UserTypeResource<tbc::...>							UserPhysicsResource;
//typedef UserUiTypeResource<tbc::...>								UserAnimationResource;
//typedef cure::UserTypeResource<...>								UserTerrainResource;



}



#include "uiresourcemanager.inl"
