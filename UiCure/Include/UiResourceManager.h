
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
class SoundManager;



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
	Cure::Resource* CreateResource(Cure::ResourceManager* pManager, const Lepra::String& pName) const;
};



class PainterImageResource: public Cure::OptimizedResource<Lepra::Canvas*, UiTbc::Painter::ImageID>, public UiResource
{
	typedef Cure::OptimizedResource<Lepra::Canvas*, UiTbc::Painter::ImageID> Parent;
public:
	typedef UiTbc::Painter::ImageID UserData;

	PainterImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	virtual ~PainterImageResource();
	const Lepra::String GetType() const;
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
	RendererImageBaseResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	virtual ~RendererImageBaseResource();
	Cure::ResourceLoadState PostProcess();
};

class RendererImageResource: public RendererImageBaseResource
{
public:
	RendererImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	virtual ~RendererImageResource();
	const Lepra::String GetType() const;
	bool Load();
};

class TextureResource: public RendererImageBaseResource
{
public:
	TextureResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	virtual ~TextureResource();
	const Lepra::String GetType() const;
	bool Load();
};



class GeometryResource: public Cure::OptimizedResource<TBC::GeometryBase*, UiTbc::Renderer::GeometryID>, public UiResource
{
	typedef Cure::OptimizedResource<TBC::GeometryBase*, UiTbc::Renderer::GeometryID> Parent;
public:
	typedef UiTbc::Renderer::GeometryID UserData;

	GeometryResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	virtual ~GeometryResource();
	void ReleaseGeometry();
	const Lepra::String GetType() const;

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

	GeometryReferenceResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	virtual ~GeometryReferenceResource();
	void ReleaseGeometry();
	const Lepra::String GetType() const;

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
	GeometryOffset(unsigned pPhysicsNodeId, Lepra::Vector3DF pOffset):
		mGeometryIndex(pPhysicsNodeId)
	{
		mOffset.SetPosition(pOffset);
	}
	GeometryOffset(unsigned pPhysicsNodeId, Lepra::TransformationF pOffset):
		mGeometryIndex(pPhysicsNodeId),
		mOffset(pOffset)
	{
	}

	unsigned mGeometryIndex;
	Lepra::TransformationF mOffset;
};



class UserGeometryReferenceResource: public Cure::UserTypeResourceBase<
	UserGeometryReferenceResource, GeometryReferenceResource>, public UiResource
{
	typedef Cure::UserTypeResourceBase<UserGeometryReferenceResource, GeometryReferenceResource> Parent;
public:
	UserGeometryReferenceResource(GameUiManager* pUiManager, const GeometryOffset& pOffset);
	virtual ~UserGeometryReferenceResource();

	//virtual void PostProcess();

	const GeometryOffset& GetOffset() const;
	//TBC::GeometryBase* GetRamData() const;
	//UiTbc::Renderer::GeometryID GetData() const;

protected:
	Cure::Resource* CreateResource(Cure::ResourceManager* pManager, const Lepra::String& pName) const;

private:
	GeometryOffset mOffset;
	//TBC::GeometryBase* mGeometryReference;
	//UiTbc::Renderer::GeometryID mGeometryReferenceId;

	LOG_CLASS_DECLARE();
};



class SoundResource: public Cure::DiversifiedResource<UiLepra::SoundManager::SoundID, UiLepra::SoundManager::SoundInstanceID>, public UiResource
{
public:
	enum SoundDimension
	{
		DIMENSION_2D = 1,
		DIMENSION_3D = 2,
	};
	typedef UiLepra::SoundManager::SoundInstanceID UserData;

	bool Load();
	UserData CreateDiversifiedData() const;
	void ReleaseDiversifiedData(UserData pData) const;

protected:
	SoundResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName, SoundDimension pDimension);
	virtual ~SoundResource();

private:
	SoundDimension mDimension;
};

class SoundResource2d: public SoundResource
{
public:
	SoundResource2d(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	const Lepra::String GetType() const;
};

class SoundResource3d: public SoundResource
{
public:
	SoundResource3d(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	const Lepra::String GetType() const;
};



class ClassResource: public Cure::ClassResourceBase<UiTbc::ChunkyClass, UiTbc::ChunkyClassLoader>, public UiResource
{
	typedef Cure::ClassResourceBase<UiTbc::ChunkyClass, UiTbc::ChunkyClassLoader> Parent;
public:
	ClassResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName);
	virtual ~ClassResource();
};



/*class AnimationResource: public OptimizedResource<void*, int>	// Perhaps a animation ID may be used on graphics accelerated hardware (i.e. PhysX or PS3)?
{
public:
	typedef void* UserData;

	AnimationResource(const Lepra::String& pName);

	bool Load();
};*/



/*class TerrainResource: public ???Resource<void*>	// ???
{
public:
	TerrainResource(const Lepra::String& pName);

	bool Load();
};*/



typedef UserUiTypeResource<PainterImageResource>			UserPainterImageResource;
typedef UserUiTypeResource<RendererImageResource>			UserRendererImageResource;
typedef UserUiTypeResource<TextureResource>				UserTextureResource;
typedef UserUiTypeResource<SoundResource2d>				UserSound2dResource;
typedef UserUiTypeResource<SoundResource3d>				UserSound3dResource;
typedef UserUiTypeResource<ClassResource>				UserClassResource;
//typedef Cure::UserTypeResource<TBC::...>				UserPhysicsResource;
//typedef UserUiTypeResource<TBC::...>					UserAnimationResource;
//typedef Cure::UserTypeResource<...>					UserTerrainResource;



}



#include "UiResourceManager.inl"
