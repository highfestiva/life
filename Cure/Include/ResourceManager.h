
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once
#pragma warning(disable: 4505)	// Crappy warning from STL/MSVC.

#include <list>
#include <set>
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/OrderedMap.h"
#include "../../Lepra/Include/String.h"
#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "Cure.h"



namespace TBC
{
class TerrainPatch;
}



namespace Cure
{



class ContextObject;
class ContextObjectFactory;
class Resource;
class ResourceManager;
class TerrainFunctionManager;



enum ResourceLoadState
{
	RESOURCE_UNLOADED		= 1,
	RESOURCE_LOAD_IN_PROGRESS	= 2,
	RESOURCE_LOAD_COMPLETE		= 3,
	RESOURCE_LOAD_ERROR		= 4,
};



class UserResource
{
public:
	typedef fastdelegate::FastDelegate1<UserResource*, void> LoadCallback;

	UserResource();
	virtual ~UserResource();
	void SafeRelease(ResourceManager* pManager);

	void SetParentResource(UserResource* pResource);
	void CallbackParent(ResourceLoadState pChildLoadState);
	void IncreaseCallbackBlockCount();
	int DecreaseCallbackBlockCount();

	const Lepra::String& GetName() const;
	ResourceLoadState GetLoadState() const;
	// JB-TRICKY: for some god-awful reason polymorfism stops working here, so I changed name for this method.
	const Resource* GetConstResource() const;

protected:
	friend class Resource;
	friend class ResourceManager;
	Resource* GetResource() const;
	void SetResource(Resource* pResource);
	virtual void PostProcess();
	virtual Resource* CreateResource(ResourceManager* pManager, const Lepra::String& pName) const = 0;

private:
	Resource* mResource;
	UserResource* mParentResource;
	int mCallbackBlockCount;
};



template<class ResourceType, class SubtypeLoadCallback, class SubtypeExtraData = int>
class UserTypeResourceBase: public UserResource
{
public:
	typedef SubtypeLoadCallback TypeLoadCallback;
	typedef SubtypeExtraData ExtraType;

	UserTypeResourceBase(const ExtraType& pExtraData = ExtraType());
	virtual ~UserTypeResourceBase();

	void Load(ResourceManager* pResourceManager, const Lepra::String& pName, TypeLoadCallback pCallback);

	void ReleaseRamResource();

	typename ResourceType::UserRamData GetRamData() const;
	typename ResourceType::UserData GetData() const;
	ExtraType& GetExtraData() const;

private:
	mutable ExtraType mExtraData;
};



template<class ResourceType> class UserTypeResource: public UserTypeResourceBase<ResourceType,
	fastdelegate::FastDelegate1<UserTypeResource<ResourceType>*, void> >
{
public:
	UserTypeResource();
	virtual ~UserTypeResource();

protected:
	virtual Resource* CreateResource(ResourceManager* pManager, const Lepra::String& pName) const;
};



template<class ResourceType, class SubtypeExtraType> class UserExtraCreateTypeResource: public UserTypeResourceBase<ResourceType,
	fastdelegate::FastDelegate1<UserExtraCreateTypeResource<ResourceType, SubtypeExtraType>*, void>, SubtypeExtraType>
{
	typedef UserTypeResourceBase<ResourceType,
		fastdelegate::FastDelegate1<UserExtraCreateTypeResource<ResourceType, SubtypeExtraType>*, void>,
		SubtypeExtraType> Parent;
public:
	UserExtraCreateTypeResource(SubtypeExtraType pExtraData);

protected:
	Resource* CreateResource(ResourceManager* pManager, const Lepra::String& pName) const;
};


class Resource
{
public:
	Resource(ResourceManager* pManager, const Lepra::String& pName);
	virtual ~Resource();

	ResourceManager* GetManager() const;
	const Lepra::String& GetName() const;
	virtual const Lepra::String GetType() const = 0;

	// Increases/decreases the reference counter. Returns the new value.
	virtual int Reference();
	virtual int Dereference();
	int GetReferenceCount();
	virtual bool IsUnique() const;

	ResourceLoadState GetLoadState() const;
	void SetLoadState(ResourceLoadState pState);

	void AddCaller(UserResource* pUserResource, const UserResource::LoadCallback& pCallback);
	void RemoveCaller(UserResource* pUserResource);

	virtual bool Load() = 0;
	// Return true if postprocessing is done, false to go again next frame.
	virtual ResourceLoadState PostProcess();
	void UserPostProcess();
	void Callback();
	virtual void FreeDiversified(UserResource*);

private:
	struct UserResourceCallbackInfo
	{
		UserResourceCallbackInfo()
		{
		}
		UserResourceCallbackInfo(UserResource* pUserResource, UserResource::LoadCallback pCallback):
			mUserResource(pUserResource),
			mCallback(pCallback)
		{
		}
		UserResource* mUserResource;
		UserResource::LoadCallback mCallback;
	};

	ResourceManager* mManager;
	const Lepra::String mName;
	int mReferenceCount;
	ResourceLoadState mState;
	typedef std::list<UserResourceCallbackInfo> CallbackList;
	CallbackList mLoadCallbackList;
	LOG_CLASS_DECLARE();

	void operator=(const Resource&);
};


// Alex/NOTE: A regular resource which is stored in some data structure in RAM.
template<class RamData> class RamResource: public Resource
{
public:
	typedef RamData UserRamData;

	UserRamData GetRamData() const;
	void SetRamDataType(RamData pData);

protected:
	RamResource(ResourceManager* pManager, const Lepra::String& pName);
	virtual ~RamResource();

	void SetRamData(RamData pData);

private:
	RamData mRamData;
};


// Alex/NOTE: An optimized resource is usually a resource which is stored in other memory
// other than the internal RAM. E.g. textures and images uploaded to the graphics memory.
template<class RamData, class OptimizedData> class OptimizedResource: public RamResource<RamData>
{
public:
	OptimizedResource(ResourceManager* pManager, const Lepra::String& pName);
	virtual ~OptimizedResource();

	void SetOptimizedData(OptimizedData pData);

protected:
	OptimizedData mOptimizedData;
};



template<class RamData, class DiversifiedData> class DiversifiedResource: public RamResource<RamData>
{
public:
	DiversifiedResource(ResourceManager* pManager, const Lepra::String& pName);
	virtual ~DiversifiedResource();

	DiversifiedData GetUserData(const UserResource* pUserResource);
	void FreeDiversified(UserResource* pUserResource);

protected:
	virtual DiversifiedData CreateDiversifiedData() const = 0;
	virtual void ReleaseDiversifiedData(DiversifiedData pData) const = 0;

	typedef Lepra::HashTable<const UserResource*, DiversifiedData, std::hash<const void*> > UserDataTable;
	UserDataTable mUserDiversifiedTable;
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



class ContextObjectResource: public RamResource<ContextObject*>
{
protected:
	typedef RamResource<ContextObject*> Parent;
public:
	typedef ContextObject* UserData;

	ContextObjectResource(ResourceManager* pManager, const Lepra::String& pName);
	virtual ~ContextObjectResource();
	const Lepra::String GetType() const;
	UserData GetUserData(const Cure::UserResource*) const;
	bool Load();
	ResourceLoadState PostProcess();	// TODO: remove this method when ContextObject::LoadGroup has been implemented correctly (thread safe).
	bool IsUnique() const;

private:
	LOG_CLASS_DECLARE();
};



class PhysicalTerrainResource: public RamResource<TBC::TerrainPatch*>
{
protected:
	typedef RamResource<TBC::TerrainPatch*> Parent;
public:
	typedef TBC::TerrainPatch* UserData;

	PhysicalTerrainResource(ResourceManager* pManager, const Lepra::String& pName);
	virtual ~PhysicalTerrainResource();
	const Lepra::String GetType() const;
	UserData GetUserData(const Cure::UserResource*) const;
	bool Load();

private:
	LOG_CLASS_DECLARE();
};



//typedef UserTypeResource<TBC::...>			UserPhysicsResource;
//typedef UserTypeResource<TBC::...>			UserAnimationResource;
typedef UserTypeResource<ContextObjectResource>		UserContextObjectResource;
typedef UserTypeResource<PhysicalTerrainResource>	UserPhysicalTerrainResource;



class ResourceManager
{
public:
	typedef std::pair<Lepra::String, Lepra::String> StringPair;
	typedef std::list<StringPair> NameTypeList;

	ResourceManager(unsigned pLoaderThreadCount);
	virtual ~ResourceManager();
	bool InitDefault(ContextObjectFactory* pContextObjectFactory);
	void StopClear();

	ContextObjectFactory* GetContextObjectFactory() const;
	void SetContextObjectFactory(ContextObjectFactory* pContextObjectFactory);
	TerrainFunctionManager* GetTerrainFunctionManager() const;
	void SetTerrainFunctionManager(TerrainFunctionManager* pTerrainFunctionManager);	// May not be changed while its resources are loaded.

	// Note that a specific resource NAME may only be loaded as one TYPE, but
	// it may be loaded as many times as you wish. For instance: loading an
	// image as a painter resource and at the same time loading the same image
	// name as a renderer resource is an error with undefined behaviour.
	void Load(const Lepra::String& pName, UserResource* pUserResource, UserResource::LoadCallback pCallback);
	bool IsCreated(const Lepra::String& pName) const;
	void SafeRelease(UserResource* pUserResource);
	void Release(Resource* pResource);

	void Tick();	// Call often, preferably every frame.
	void ForceFreeCache();	// Called to force immediate freeing of all resources.

	NameTypeList QueryActiveResourceNames();

protected:
	Resource* QueryCachedResource(const Lepra::String& pName, UserResource* pUserResource, bool& pMustLoad);

	// Called by Tick (main thread) to push objects into the active table, optimize them and callback waiters.
	void InjectResourceLoop();
	bool InjectSingleResource(Resource* pResource);

	void FreeCache();	// Called by Tick (main thread) to delete old/large resources.

	void ThreadLoaderLoop();	// Called by worker thread to load objects asynchronously.
	void SynchronousLoadLoop();	// Called by main thread to load all requested resources.
	bool PrepareDeleteInLoadProgress(Resource* pResource);	// Drops the resource from the load list.
	void LoadSingleResource();

private:
	Resource* CreateResource(UserResource* pUserResource, const Lepra::String& pName);
	void DeleteResource(Resource* pResource);

	typedef Lepra::HashTable<Lepra::String, Resource*> ResourceTable;
	typedef Lepra::OrderedMap<Lepra::String, Resource*> ResourceMap;
	typedef std::set<Resource*> ResourceSet;

	ContextObjectFactory* mContextObjectFactory;
	TerrainFunctionManager* mTerrainFunctionManager;

	unsigned mLoaderThreadCount;
	Lepra::MemberThread<ResourceManager> mLoaderThread;	// TODO: increase max loader thread count (put in list).
	Lepra::Semaphore mLoadSemaphore;
	Lepra::Lock mThreadLock;
	ResourceTable mActiveResourceTable;	// In use. Data owner for Resource*.
	ResourceTable mCachedResourceTable;	// On the way out. Data owner for Resource*.
	ResourceMap mRequestLoadList;	// Under way to be loaded by worker thread. TODO: priority map thingie!
	ResourceMap mLoadedList;	// Loaded by worker thread, worker thread will injected into the system at end of tick.
	ResourceSet mResourceSafeLookup;

	LOG_CLASS_DECLARE();
};



}



#include "ResourceManager.inl"
