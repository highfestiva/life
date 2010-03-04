
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Cure/Include/ResourceManager.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/OrderedMap.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../TBC/Include/GeometryReference.h"
#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "../../UiLepra/Include/UiSoundManager.h"
#include "../../UiTBC/Include/UiChunkyClass.h"
#include "../../UiTBC/Include/UiChunkyLoader.h"
#include "../../UiTBC/Include/UiPainter.h"
#include "../../UiTBC/Include/UiRenderer.h"
#include "UiCure.h"
#include "UiGameUiManager.h"



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
	typedef UiTbc::Painter::ImageID UserData;

	PainterImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~PainterImageResource();
	const str GetType() const;
	UserData GetUserData(const Cure::UserResource* pUserResource) const;
	bool Load();
	Cure::ResourceLoadState PostProcess();
};



class RendererImageBaseResource: public Cure::OptimizedResource<UiTbc::Texture*, UiTbc::Renderer::TextureID>, public UiResource
{
	typedef Cure::OptimizedResource<UiTbc::Texture*, UiTbc::Renderer::TextureID> Parent;
public:
	typedef UiTbc::Renderer::TextureID UserData;

	UserData GetUserData(const Cure::UserResource* pUserResource) const;

protected:
	RendererImageBaseResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~RendererImageBaseResource();
	Cure::ResourceLoadState PostProcess();
};

class RendererImageResource: public RendererImageBaseResource
{
public:
	RendererImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~RendererImageResource();
	const str GetType() const;
	bool Load();
};

class TextureResource: public RendererImageBaseResource
{
public:
	TextureResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~TextureResource();
	const str GetType() const;
	bool Load();
};



class GeometryResource: public Cure::OptimizedResource<TBC::GeometryBase*, UiTbc::Renderer::GeometryID>, public UiResource
{
	typedef Cure::OptimizedResource<TBC::GeometryBase*, UiTbc::Renderer::GeometryID> Parent;
public:
	typedef UiTbc::Renderer::GeometryID UserData;

	GeometryResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName);
	virtual ~GeometryResource();
	void ReleaseGeometry();
	const str GetType() const;

	UserData GetUserData(const Cure::UserResource*) const;
	bool Load();
	Cure::ResourceLoadState PostProcess();

private:
	LOG_CLASS_DECLARE();
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

	bool Load();
	Cure::ResourceLoadState PostProcess();
	Cure::ResourceLoadState CreateInstance();

private:
	void Resume();
	void Suspend();

	void OnLoadClass(ClassResource*);
	ClassResource* mClassResource;

	LOG_CLASS_DECLARE();
};

struct GeometryOffset
{
	GeometryOffset(unsigned pPhysicsNodeId):
		mGeometryIndex(pPhysicsNodeId)
	{
	}
	GeometryOffset(unsigned pPhysicsNodeId, Vector3DF pOffset):
		mGeometryIndex(pPhysicsNodeId)
	{
		mOffset.SetPosition(pOffset);
	}
	GeometryOffset(unsigned pPhysicsNodeId, TransformationF pOffset):
		mGeometryIndex(pPhysicsNodeId),
		mOffset(pOffset)
	{
	}

	unsigned mGeometryIndex;
	TransformationF mOffset;
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
	//TBC::GeometryBase* GetRamData() const;
	//UiTbc::Renderer::GeometryID GetData() const;

protected:
	Cure::Resource* CreateResource(Cure::ResourceManager* pManager, const str& pName) const;

private:
	GeometryOffset mOffset;
	//TBC::GeometryBase* mGeometryReference;
	//UiTbc::Renderer::GeometryID mGeometryReferenceId;

	LOG_CLASS_DECLARE();
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

	bool Load();
	UserData CreateDiversifiedData() const;
	void ReleaseDiversifiedData(UserData pData) const;

protected:
	SoundResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName,
		SoundDimension pDimension, LoopMode pLoopMode);
	virtual ~SoundResource();

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



typedef UserUiTypeResource<PainterImageResource>				UserPainterImageResource;
typedef UserUiTypeResource<RendererImageResource>				UserRendererImageResource;
typedef UserUiTypeResource<TextureResource>					UserTextureResource;
typedef UserUiExtraTypeResource<SoundResource2d, SoundResource::LoopMode>	UserSound2dResource;
typedef UserUiExtraTypeResource<SoundResource3d, SoundResource::LoopMode>	UserSound3dResource;
typedef UserUiTypeResource<ClassResource>					UserClassResource;
//typedef Cure::UserTypeResource<TBC::...>					UserPhysicsResource;
//typedef UserUiTypeResource<TBC::...>						UserAnimationResource;
//typedef Cure::UserTypeResource<...>						UserTerrainResource;



}



#include "UiResourceManager.inl"
