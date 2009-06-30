
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



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
#include "../../UiTBC/Include/UiPainter.h"
#include "../../UiTBC/Include/UiRenderer.h"
#include "UiCure.h"
#include "UiGameUiManager.h"



namespace UiCure
{



class GameUiManager;
class SoundManager;



template<class ResourceType, class SubtypeExtraType = int>
class UserUiTypeResource: public Cure::UserTypeResourceBase<ResourceType,
	fastdelegate::FastDelegate1<UserUiTypeResource<ResourceType, SubtypeExtraType>*, void>, SubtypeExtraType>
{
	typedef fastdelegate::FastDelegate1<UserUiTypeResource<ResourceType, SubtypeExtraType>*, void> ParentTypeLoadCallback;
public:
	UserUiTypeResource(GameUiManager* pUiManager, const typename SubtypeExtraType& pExtraData = SubtypeExtraType());
	virtual ~UserUiTypeResource();

protected:
	Cure::Resource* CreateResource(Cure::ResourceManager* pManager, const Lepra::String& pName) const;

	GameUiManager* mUiManager;
};



class UiResource
{
protected:
	UiResource(GameUiManager* pUiManager);
	virtual ~UiResource();

	GameUiManager* GetUiManager() const;

private:
	GameUiManager* mUiManager;
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



/*class StaticGeometryResource: public Cure::OptimizedResource<TBC::GeometryBase*, UiTbc::Renderer::GeometryID>
{
public:
	typedef UiTbc::Renderer::GeometryID UserData;

	StaticGeometryResource(const Lepra::String& pName);

	bool Load();
	void Optimize();
};*/



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

private:
	int Reference();
	int Dereference();

	void OnLoadClass(ClassResource*);
	ClassResource* mClassResource;

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



/*class PhysicsResource: public OptimizedResource<void*, int>	// Perhaps a physics ID may be used on physics accelerated hardware (i.e. PhysX or PS3)?
{
public:
	typedef void* UserData;

	PhysicsResource(const Lepra::String& pName);

	bool Load();
};



class AnimationResource: public OptimizedResource<void*, int>	// Perhaps a animation ID may be used on graphics accelerated hardware (i.e. PhysX or PS3)?
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



struct GeometryOffset
{
	GeometryOffset(unsigned pPhysicsNodeId):
		mPhysicsNodeId(pPhysicsNodeId)
	{
	}
	GeometryOffset(unsigned pPhysicsNodeId, Lepra::Vector3DF pOffset):
		mPhysicsNodeId(pPhysicsNodeId)
	{
		mOffset.SetPosition(pOffset);
	}
	GeometryOffset(unsigned pPhysicsNodeId, Lepra::TransformationF pOffset):
		mPhysicsNodeId(pPhysicsNodeId),
		mOffset(pOffset)
	{
	}

	unsigned mPhysicsNodeId;
	Lepra::TransformationF mOffset;
};

typedef UserUiTypeResource<PainterImageResource>			UserPainterImageResource;
typedef UserUiTypeResource<RendererImageResource>			UserRendererImageResource;
typedef UserUiTypeResource<TextureResource>				UserTextureResource;
typedef UserUiTypeResource<GeometryReferenceResource, GeometryOffset>	UserGeometryReferenceResource;
typedef UserUiTypeResource<SoundResource2d>				UserSound2dResource;
typedef UserUiTypeResource<SoundResource3d>				UserSound3dResource;
//typedef Cure::UserTypeResource<TBC::...>				UserPhysicsResource;
//typedef UserUiTypeResource<TBC::...>					UserAnimationResource;
//typedef Cure::UserTypeResource<...>					UserTerrainResource;



}



#include "UiResourceManager.inl"
