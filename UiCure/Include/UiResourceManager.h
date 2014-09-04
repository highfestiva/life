
// Author: Jonas Byström
// Copyright (c) 2002-, Pixel Doctrine



#pragma once

#include "../../Cure/Include/ResourceManager.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Tbc/Include/GeometryReference.h"
#include "../../UiLepra/Include/UiSoundManager.h"
#include "../../UiTbc/Include/UiChunkyClass.h"
#include "../../UiTbc/Include/UiChunkyLoader.h"
#include "../../UiTbc/Include/UiPainter.h"
#include "../../UiTbc/Include/UiRenderer.h"
#include "UiCure.h"



namespace UiCure
{



class GameUiManager;



class UiResource
{
protected:
	UiResource(GameUiManager* pUiManager);
	virtual ~UiResource();

	GameUiManager* GetUiManager() const;

private:
	GameUiManager* mUiManager;
};



template<class ResourceType>
class UserUiTypeResource: public Cure::UserTypeResourceBase<UserUiTypeResource<ResourceType>, ResourceType>,
	public UiResource
{
public:
	UserUiTypeResource(GameUiManager* pUiManager);
	virtual ~UserUiTypeResource();

protected:
	Cure::Resource* CreateResource(Cure::ResourceManager* pManager, const str& pName) const;
};



template<class ResourceType, class SubtypeExtraType>
class UserUiExtraTypeResource:
	public Cure::UserTypeResourceBase<UserUiExtraTypeResource<ResourceType, SubtypeExtraType>, ResourceType>,
	public UiResource
{
public:
	typedef SubtypeExtraType ExtraType;

	UserUiExtraTypeResource(GameUiManager* pUiManager, const ExtraType& pExtraData);
	virtual ~UserUiExtraTypeResource();

	ExtraType& GetExtraData() const;
	void SetExtraData(const ExtraType& pExtraData);

protected:
	Cure::Resource* CreateResource(Cure::ResourceManager* pManager, const str& pName) const;

private:
	mutable ExtraType mExtraData;
};



class PainterImageResource: public Cure::OptimizedResource<Canvas*, UiTbc::Painter::ImageID>, public UiResource
{
	typedef Cure::OptimizedResource<Canvas*, UiTbc::Painter::ImageID> Parent;
public:
	enum ImageReleaseMode
	{
		RELEASE_DELETE = 1,
		RELEASE_FREE_BUFFER,
		RELEASE_NONE,
	};
	typedef UiTbc::Painter::ImageID UserData;

	PainterImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	PainterImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName, ImageReleaseMode pReleaseMode);
	virtual ~PainterImageResource();
	const str GetType() const;
	UserData GetUserData(const Cure::UserResource* pUserResource) const;
	bool Load();
	Cure::ResourceLoadState PostProcess();

private:
	ImageReleaseMode mReleaseMode;
};



class RendererImageBaseResource: public Cure::OptimizedResource<UiTbc::Texture*, UiTbc::Renderer::TextureID>, public UiResource
{
	typedef Cure::OptimizedResource<UiTbc::Texture*, UiTbc::Renderer::TextureID> Parent;
public:
	typedef UiTbc::Renderer::TextureID UserData;

	UserData GetUserData(const Cure::UserResource* pUserResource) const;

	Cure::ResourceLoadState PostProcess();

protected:
	RendererImageBaseResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~RendererImageBaseResource();
};

struct ImageProcessSettings
{
	Canvas::ResizeHint mResizeHint;
	bool mMipMap;
	inline ImageProcessSettings(Canvas::ResizeHint pResizeHint, bool pMipMap):
		mResizeHint(pResizeHint),
		mMipMap(pMipMap)
	{
	}
};

class RendererImageResource: public RendererImageBaseResource
{
public:
	RendererImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName, const ImageProcessSettings& pLoadSettings);
	virtual ~RendererImageResource();
	const str GetType() const;
	bool Load();

	ImageProcessSettings mSettings;
};

/*class TextureResource: public RendererImageBaseResource
{
public:
	TextureResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~TextureResource();
	const str GetType() const;
	bool Load();
};*/



class GeometryResource: public Cure::OptimizedResource<Tbc::GeometryBase*, UiTbc::Renderer::GeometryID>, public UiResource
{
	typedef Cure::OptimizedResource<Tbc::GeometryBase*, UiTbc::Renderer::GeometryID> Parent;
public:
	typedef UiTbc::Renderer::GeometryID UserData;

	GeometryResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~GeometryResource();
	void ReleaseGeometry();
	const str GetType() const;

	UserData GetUserData(const Cure::UserResource*) const;
	bool Load();
	Cure::ResourceLoadState PostProcess();

	int GetCastsShadows() const;

protected:
	int mCastsShadows;

private:
	logclass();
};



class GeometryReferenceResource: public GeometryResource
{
	typedef GeometryResource Parent;
public:
	typedef UserUiTypeResource<GeometryResource> ClassResource;

	GeometryReferenceResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~GeometryReferenceResource();
	void ReleaseGeometry();
	const str GetType() const;

	bool IsReferenceType() const;
	ClassResource* GetParent() const;

	bool Load();
	Cure::ResourceLoadState PostProcess();
	Cure::ResourceLoadState CreateInstance();

private:
	void Resume();
	void Suspend();

	void OnLoadClass(ClassResource*);
	ClassResource* mClassResource;

	logclass();
};

struct GeometryOffset
{
	GeometryOffset(unsigned pPhysicsNodeId):
		mGeometryIndex(pPhysicsNodeId),
		mScale(1)
	{
	}
	GeometryOffset(unsigned pPhysicsNodeId, vec3 pOffset):
		mGeometryIndex(pPhysicsNodeId),
		mScale(1)
	{
		mOffset.SetPosition(pOffset);
	}
	GeometryOffset(unsigned pPhysicsNodeId, xform pOffset, float pScale):
		mGeometryIndex(pPhysicsNodeId),
		mOffset(pOffset),
		mScale(pScale)
	{
	}

	unsigned mGeometryIndex;
	xform mOffset;
	float mScale;
};



class UserGeometryReferenceResource: public Cure::UserTypeResourceBase<
	UserGeometryReferenceResource, GeometryReferenceResource>, public UiResource
{
	typedef Cure::UserTypeResourceBase<UserGeometryReferenceResource, GeometryReferenceResource> Parent;
public:
	UserGeometryReferenceResource(GameUiManager* pUiManager, const GeometryOffset& pOffset = GeometryOffset(0));
	virtual ~UserGeometryReferenceResource();

	//virtual void PostProcess();

	const GeometryOffset& GetOffset() const;
	//Tbc::GeometryBase* GetRamData() const;
	//UiTbc::Renderer::GeometryID GetData() const;

protected:
	Cure::Resource* CreateResource(Cure::ResourceManager* pManager, const str& pName) const;

private:
	GeometryOffset mOffset;
	//Tbc::GeometryBase* mGeometryReference;
	//UiTbc::Renderer::GeometryID mGeometryReferenceId;

	logclass();
};



class SoundResource: public Cure::DiversifiedResource<UiLepra::SoundManager::SoundID, UiLepra::SoundManager::SoundInstanceID>, public UiResource
{
	typedef Cure::DiversifiedResource<UiLepra::SoundManager::SoundID, UiLepra::SoundManager::SoundInstanceID> Parent;
public:
	enum SoundDimension
	{
		DIMENSION_2D = 1,
		DIMENSION_3D = 2,
	};
	typedef UiLepra::SoundManager::SoundInstanceID UserData;
	typedef UiLepra::SoundManager::LoopMode LoopMode;

	void Release();
	bool Load();
	virtual UserData CreateDiversifiedData() const;
	virtual void ReleaseDiversifiedData(UserData pData) const;

protected:
	SoundResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName,
		SoundDimension pDimension, LoopMode pLoopMode);
	virtual ~SoundResource();

	virtual void PatchInfo(Cure::ResourceInfo& pInfo) const;

private:
	SoundDimension mDimension;
	LoopMode mLoopMode;
};

class SoundResource2d: public SoundResource
{
public:
	SoundResource2d(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName,
		LoopMode pLoopMode);
	const str GetType() const;
};

class SoundResource3d: public SoundResource
{
public:
	SoundResource3d(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName,
		LoopMode pLoopMode);
	const str GetType() const;
};



class ClassResource: public Cure::ClassResourceBase<UiTbc::ChunkyClass, UiTbc::ChunkyClassLoader>, public UiResource
{
	typedef Cure::ClassResourceBase<UiTbc::ChunkyClass, UiTbc::ChunkyClassLoader> Parent;
public:
	ClassResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~ClassResource();

	virtual bool Load();
};



/*class AnimationResource: public OptimizedResource<void*, int>	// Perhaps a animation ID may be used on graphics accelerated hardware (i.e. PhysX or PS3)?
{
public:
	typedef void* UserData;

	AnimationResource(const str& pName);

	bool Load();
};*/



/*class TerrainResource: public ???Resource<void*>	// ???
{
public:
	TerrainResource(const str& pName);

	bool Load();
};*/



typedef UserUiTypeResource<PainterImageResource>						UserPainterImageResource;
typedef UserUiExtraTypeResource<PainterImageResource, PainterImageResource::ImageReleaseMode>	UserPainterKeepImageResource;
typedef UserUiExtraTypeResource<RendererImageResource, ImageProcessSettings>			UserRendererImageResource;
//typedef UserUiTypeResource<TextureResource>							UserTextureResource;
typedef UserUiExtraTypeResource<SoundResource2d, SoundResource::LoopMode>			UserSound2dResource;
typedef UserUiExtraTypeResource<SoundResource3d, SoundResource::LoopMode>			UserSound3dResource;
typedef UserUiTypeResource<ClassResource>							UserClassResource;
//typedef Cure::UserTypeResource<Tbc::...>							UserPhysicsResource;
//typedef UserUiTypeResource<Tbc::...>								UserAnimationResource;
//typedef Cure::UserTypeResource<...>								UserTerrainResource;



}



#include "UiResourceManager.inl"
