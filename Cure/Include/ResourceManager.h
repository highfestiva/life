
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once
#pragma warning(disable: 4505)	// Crappy warning from STL/MSVC.

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/OrderedMap.h"
#include "../../TBC/Include/ChunkyClass.h"
#include "../../TBC/Include/ChunkyLoader.h"
#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "Cure.h"



namespace Lepra
{
class Canvas;
class ZipArchive;
}
namespace TBC
{
class TerrainPatch;
}



namespace Cure
{



class Resource;
class ResourceManager;
class TerrainFunctionManager;



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

	const str& GetName() const;
	ResourceLoadState GetLoadState() const;
	// JB-TRICKY: for some god-awful reason polymorfism stops working here, so I changed name for this method.
	const Resource* GetConstResource() const;

protected:
	friend class Resource;
	friend class ResourceManager;
	Resource* GetResource() const;
	void SetResource(Resource* pResource);
	virtual void PostProcess();
	virtual Resource* CreateResource(ResourceManager* pManager, const str& pName) const = 0;

private:
	Resource* mResource;
	UserResource* mParentResource;
	int mCallbackBlockCount;
};



template<class UserResourceType, class ResourceType>
class UserTypeResourceBase: public UserResource
{
public:
	typedef UserResourceType TypeUserResourceType;
	typedef ResourceType TypeResourceType;
	typedef fastdelegate::FastDelegate1<UserResourceType*, void> TypeLoadCallback;

	UserTypeResourceBase();
	virtual ~UserTypeResourceBase();

	void Load(ResourceManager* pResourceManager, const str& pName, TypeLoadCallback pCallback, bool pKeep = true);
	void LoadUnique(ResourceManager* pResourceManager, const str& pName, TypeLoadCallback pCallback);

	typename ResourceType::UserRamData GetRamData() const;
	typename ResourceType::UserData GetData() const;
};



template<class ResourceType>
class UserTypeResource: public UserTypeResourceBase<UserTypeResource<ResourceType>, ResourceType>
{
public:
	UserTypeResource();
	virtual ~UserTypeResource();

protected:
	virtual Resource* CreateResource(ResourceManager* pManager, const str& pName) const;
};



template<class ResourceType, class SubtypeExtraType>
class UserExtraTypeResource: public UserTypeResourceBase<
	UserExtraTypeResource<ResourceType, SubtypeExtraType>, ResourceType>
{
public:
	typedef SubtypeExtraType ExtraType;

	UserExtraTypeResource(const ExtraType& pExtraData);
	virtual ~UserExtraTypeResource();

	ExtraType& GetExtraData() const;
	void SetExtraData(const ExtraType& pExtraData);

protected:
	virtual Resource* CreateResource(ResourceManager* pManager, const str& pName) const;

private:
	mutable ExtraType mExtraData;
};



template<class _UserResourceType>
class UserResourceOwner
{
public:
	UserResourceOwner(_UserResourceType* pUserResource, Cure::ResourceManager* pManager, const str& pName);
	virtual ~UserResourceOwner();

protected:
	void OnLoadCallback(_UserResourceType* pUserResource);

	_UserResourceType* mUserResource;
};



class Resource
{
public:
	Resource(ResourceManager* pManager, const str& pName);
	virtual ~Resource();

	ResourceManager* GetManager() const;
	const str& GetName() const;
	virtual const str GetType() const = 0;

	// Increases/decreases the reference counter. Returns the new value.
	int Reference();
	int Dereference();
	int GetReferenceCount() const;
	virtual void Resume();	// Resource goes from cache -> live.
	virtual void Suspend();	// Resource enters cache.
	bool IsUnique() const;
	void SetIsUnique(bool pIsUnique);
	virtual bool IsReferenceType() const;

	ResourceLoadState GetLoadState() const;
	void SetLoadState(ResourceLoadState pState);

	void AddCaller(UserResource* pUserResource, const UserResource::LoadCallback& pCallback);
	void RemoveCaller(UserResource* pUserResource);

	virtual bool Load() = 0;
	virtual ResourceLoadState PostProcess();
	void UserPostProcess();
	void Callback();
	virtual void FreeDiversified(UserResource*);
	UserResource* GetFirstUserResource() const;

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
	const str mName;
	int mReferenceCount;
	ResourceLoadState mState;
	typedef std::list<UserResourceCallbackInfo> CallbackList;
	CallbackList mLoadCallbackList;
	bool mIsUnique;
	static Lock mMutex;

	LOG_CLASS_DECLARE();

	void operator=(const Resource&);
};


template<class RamData>
class RamResource: public Resource
{
public:
	typedef RamData UserRamData;

	UserRamData GetRamData() const;
	void SetRamDataType(RamData pData);

protected:
	RamResource(ResourceManager* pManager, const str& pName);
	virtual ~RamResource();

	void SetRamData(RamData pData);

private:
	RamData mRamData;
};



// NOTE: An optimized resource is usually a resource which is stored in other memory
// other than the internal RAM. E.g. textures and images uploaded to the graphics memory.
template<class RamData, class OptimizedData>
class OptimizedResource: public RamResource<RamData>
{
public:
	OptimizedResource(ResourceManager* pManager, const str& pName);
	virtual ~OptimizedResource();

	void SetOptimizedData(OptimizedData pData);

protected:
	OptimizedData mOptimizedData;
};



// A type of resource that needs it's own clone data, but has much in common with
// the related clones. Used for sounds.
template<class RamData, class DiversifiedData>
class DiversifiedResource: public RamResource<RamData>
{
public:
	DiversifiedResource(ResourceManager* pManager, const str& pName);
	virtual ~DiversifiedResource();

	DiversifiedData GetUserData(const UserResource* pUserResource);
	void FreeDiversified(UserResource* pUserResource);

protected:
	virtual DiversifiedData CreateDiversifiedData() const = 0;
	virtual void ReleaseDiversifiedData(DiversifiedData pData) const = 0;

	typedef std::hash_map<const UserResource*, DiversifiedData, LEPRA_VOIDP_HASHER> UserDataTable;

	Lock mLock;
	UserDataTable mUserDiversifiedTable;
};



class PhysicsResource: public RamResource<TBC::ChunkyPhysics*>
{
	typedef RamResource<TBC::ChunkyPhysics*> Parent;
public:
	typedef TBC::ChunkyPhysics* UserData;

	PhysicsResource(ResourceManager* pManager, const str& pName);
	virtual ~PhysicsResource();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();

private:
	LOG_CLASS_DECLARE();
};



/*class AnimationResource: public OptimizedResource<void*, int>	// Perhaps a animation ID may be used on graphics accelerated hardware (i.e. PhysX or PS3)?
{
public:
	typedef void* UserData;

	AnimationResource(const str& pName);

	bool Load();
};*/



template<class _Class, class _ClassLoader>
class ClassResourceBase: public RamResource<_Class*>
{
	typedef RamResource<_Class*> Parent;
public:
	typedef _Class* UserData;

	ClassResourceBase(ResourceManager* pManager, const str& pName);
	virtual ~ClassResourceBase();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();

private:
	LOG_CLASS_DECLARE();
};

class ClassResource: public ClassResourceBase<TBC::ChunkyClass, TBC::ChunkyClassLoader>
{
	typedef ClassResourceBase<TBC::ChunkyClass, TBC::ChunkyClassLoader> Parent;
public:
	ClassResource(ResourceManager* pManager, const str& pName);
	virtual ~ClassResource();
};



/*class ContextObjectResource: public RamResource<ContextObject*>
{
	typedef RamResource<ContextObject*> Parent;
public:
	typedef ContextObject* UserData;

	ContextObjectResource(ResourceManager* pManager, const str& pName);
	virtual ~ContextObjectResource();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();
	ResourceLoadState PostProcess();	// TODO: remove this method when ContextObject::LoadGroup has been implemented correctly (thread safe).

private:
	LOG_CLASS_DECLARE();
};*/



class PhysicalTerrainResource: public RamResource<TBC::TerrainPatch*>
{
	typedef RamResource<TBC::TerrainPatch*> Parent;
public:
	typedef TBC::TerrainPatch* UserData;

	PhysicalTerrainResource(ResourceManager* pManager, const str& pName);
	virtual ~PhysicalTerrainResource();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();

private:
	LOG_CLASS_DECLARE();
};



class RamImageResource: public RamResource<Canvas*>
{
	typedef RamResource<Canvas*> Parent;
public:
	typedef Canvas* UserData;

	RamImageResource(Cure::ResourceManager* pManager, const str& pName);
	virtual ~RamImageResource();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();
};



typedef UserTypeResource<PhysicsResource>		UserPhysicsResource;
typedef UserTypeResource<ClassResource>			UserClassResource;
//typedef UserTypeResource<TBC::...>			UserAnimationResource;
//typedef UserTypeResource<ContextObjectResource>	UserContextObjectResource;
typedef UserTypeResource<PhysicalTerrainResource>	UserPhysicalTerrainResource;
typedef UserTypeResource<RamImageResource>		UserRamImageResource;



class ResourceManager
{
public:
	typedef std::pair<str, str> StringPair;
	typedef std::list<StringPair> NameTypeList;

	ResourceManager(unsigned pLoaderThreadCount, const str& pPathPrefix);
	virtual ~ResourceManager();
	bool InitDefault();
	void StopClear();

	File* QueryFile(const str& pFilename);
	bool QueryFileExists(const str& pFilename);
	strutil::strvec ListFiles(const str& pWildcard);

	TerrainFunctionManager* GetTerrainFunctionManager() const;
	void SetTerrainFunctionManager(TerrainFunctionManager* pTerrainFunctionManager);	// May not be changed while its resources are loaded.

	// Note that a specific resource NAME may only be loaded as one TYPE, but
	// it may be loaded as many times as you wish. For instance: loading an
	// image as a painter resource and at the same time loading the same image
	// name as a renderer resource is an error with undefined behaviour.
	void Load(const str& pName, UserResource* pUserResource, UserResource::LoadCallback pCallback);
	void LoadUnique(const str& pName, UserResource* pUserResource, UserResource::LoadCallback pCallback);
	void SafeRelease(UserResource* pUserResource);
	void Release(Resource* pResource);
	bool IsLoading();
	bool WaitLoading();

	void Tick();	// Call often, preferably every frame.
	unsigned ForceFreeCache();	// Called to force immediate freeing of all cached resources.
	unsigned ForceFreeCache(const strutil::strvec& pResourceTypeList);	// Empty type list = all.

	size_t QueryResourceCount() const;
	size_t QueryCachedResourceCount() const;
	NameTypeList QueryResourceNames();

protected:
	Resource* GetAddCachedResource(const str& pName, UserResource* pUserResource, bool& pMustLoad);
	void StartLoad(Resource* pResource);

	// Called by Tick (main thread) to push objects into the active table, optimize them and callback waiters.
	void InjectResourceLoop();
	bool InjectSingleResource(Resource* pResource);

	void FreeCache();	// Called by Tick (main thread) to delete old/large resources.

	void ThreadLoaderLoop();	// Called by worker thread to load objects asynchronously.
	void SynchronousLoadLoop();	// Called by main thread to load all requested resources.
	bool PrepareRemoveInLoadProgress(Resource* pResource);	// Drops the resource from the load list.
	void LoadSingleResource();

	inline void AssertIsMutexOwner()
	{
		assert(mThreadLock.IsOwner());
	}

private:
	Resource* CreateResource(UserResource* pUserResource, const str& pName);
	void DeleteResource(Resource* pResource);

	typedef HashTable<str, Resource*> ResourceTable;
	typedef OrderedMap<str, Resource*> ResourceMap;
	typedef OrderedMap<Resource*, Resource*, LEPRA_VOIDP_HASHER> ResourceMapList;
	typedef std::hash_set<Resource*, LEPRA_VOIDP_HASHER> ResourceSet;

	TerrainFunctionManager* mTerrainFunctionManager;

	unsigned mLoaderThreadCount;
	const str mPathPrefix;
	MemberThread<ResourceManager> mLoaderThread;	// TODO: increase max loader thread count (put in list).
	Semaphore mLoadSemaphore;
	mutable Lock mThreadLock;
	ResourceTable mActiveResourceTable;	// In use. Holds non-unique resources.
	ResourceTable mCachedResourceTable;	// On the way out. Holds non-unique resources.
	ResourceMapList mRequestLoadList;	// Under way to be loaded by worker thread. TODO: priority map thingie!
	ResourceMapList mLoadedList;		// Loaded by worker thread, worker thread will injected into the system at end of tick.
	ResourceSet mResourceSafeLookup;	// Data owner for Resource*.
	Lock* mZipLock;
	ZipArchive* mZipFile;

	LOG_CLASS_DECLARE();
};



}



#include "ResourceManager.inl"
