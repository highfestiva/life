
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#pragma warning(disable: 4505)	// Crappy warning from STL/MSVC.

#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/memberthread.h"
#include "../../lepra/include/orderedmap.h"
#include "../../tbc/include/chunkyclass.h"
#include "../../tbc/include/chunkyloader.h"
#include "../../thirdparty/FastDelegate/FastDelegate.h"
#include "cure.h"



namespace lepra {
class Canvas;
class ZipArchive;
}
namespace tbc {
class TerrainPatch;
}



namespace cure {



class Resource;
class ResourceManager;
class TerrainFunctionManager;



struct ResourceInfo {
	str name_;
	str type_;
	int reference_count_;
};



class UserResource {
public:
	typedef fastdelegate::FastDelegate1<UserResource*, void> LoadCallback;

	UserResource();
	virtual ~UserResource();
	void SafeRelease(ResourceManager* manager);

	void SetParentResource(UserResource* resource);
	void CallbackParent(ResourceLoadState child_load_state);
	void IncreaseCallbackBlockCount();
	int DecreaseCallbackBlockCount();

	const str& GetName() const;
	ResourceLoadState GetLoadState() const;
	// JB-TRICKY: for some god-awful reason polymorfism stops working here, so I changed name for this method.
	const Resource* GetConstResource() const;

	Resource* GetResource() const;
	void SetResource(Resource* resource);
	virtual void PostProcess();
	virtual void FinalizeLoad();
	virtual Resource* CreateResource(ResourceManager* manager, const str& name) const = 0;

private:
	Resource* resource_;
	UserResource* parent_resource_;
	int callback_block_count_;
};



template<class UserResourceType, class ResourceType>
class UserTypeResourceBase: public UserResource {
public:
	typedef UserResourceType TypeUserResourceType;
	typedef ResourceType TypeResourceType;
	typedef fastdelegate::FastDelegate1<UserResourceType*, void> TypeLoadCallback;

	UserTypeResourceBase();
	virtual ~UserTypeResourceBase();

	void Load(ResourceManager* resource_manager, const str& name, TypeLoadCallback callback, bool keep = true);
	void LoadUnique(ResourceManager* resource_manager, const str& name, TypeLoadCallback callback);

	virtual void FinalizeLoad();

	typename ResourceType::UserRamData GetRamData() const;
	typename ResourceType::UserData GetData() const;
};



template<class ResourceType>
class UserTypeResource: public UserTypeResourceBase<UserTypeResource<ResourceType>, ResourceType>
{
public:
	UserTypeResource();
	virtual ~UserTypeResource();

	virtual Resource* CreateResource(ResourceManager* manager, const str& name) const;
};



template<class ResourceType, class SubtypeExtraType>
class UserExtraTypeResource: public UserTypeResourceBase<
	UserExtraTypeResource<ResourceType, SubtypeExtraType>, ResourceType>
{
public:
	typedef SubtypeExtraType ExtraType;

	UserExtraTypeResource(const ExtraType& extra_data);
	virtual ~UserExtraTypeResource();

	ExtraType& GetExtraData() const;
	void SetExtraData(const ExtraType& extra_data);

	virtual Resource* CreateResource(ResourceManager* manager, const str& name) const;

private:
	mutable ExtraType extra_data_;
};



template<class _UserResourceType>
class UserResourceOwner {
public:
	UserResourceOwner(_UserResourceType* user_resource, cure::ResourceManager* manager, const str& name);
	virtual ~UserResourceOwner();

protected:
	void OnLoadCallback(_UserResourceType* user_resource);

	_UserResourceType* user_resource_;
};



class Resource {
public:
	Resource(ResourceManager* manager, const str& name);
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
	void SetIsUnique(bool is_unique);
	virtual bool IsReferenceType() const;

	ResourceLoadState GetLoadState() const;
	void SetLoadState(ResourceLoadState state);

	void AddCaller(UserResource* user_resource, const UserResource::LoadCallback& callback);
	void RemoveCaller(UserResource* user_resource);

	virtual bool Load() = 0;
	virtual ResourceLoadState PostProcess();
	void UserPostProcess();
	void Callback();
	virtual void FreeDiversified(UserResource*);
	UserResource* GetFirstUserResource() const;

	virtual void PatchInfo(ResourceInfo& info) const;

private:
	struct UserResourceCallbackInfo {
		UserResourceCallbackInfo() {
		}
		UserResourceCallbackInfo(UserResource* user_resource, UserResource::LoadCallback callback):
			user_resource_(user_resource),
			callback_(callback) {
		}
		UserResource* user_resource_;
		UserResource::LoadCallback callback_;
	};

	ResourceManager* manager_;
	const str name_;
	int reference_count_;
	ResourceLoadState state_;
	typedef std::list<UserResourceCallbackInfo> CallbackList;
	CallbackList load_callback_list_;
	bool is_unique_;
	static Lock mutex_;

	logclass();

	void operator=(const Resource&);
};


template<class RamData>
class RamResource: public Resource {
	typedef Resource Parent;
public:
	typedef RamData UserRamData;

	UserRamData GetRamData() const;
	void SetRamDataType(RamData data);

protected:
	RamResource(ResourceManager* manager, const str& name);
	virtual ~RamResource();

	void SetRamData(RamData data);

private:
	RamData ram_data_;
};



// NOTE: An optimized resource is usually a resource which is stored in other memory
// other than the internal RAM. E.g. textures and images uploaded to the graphics memory.
template<class RamData, class OptimizedData>
class OptimizedResource: public RamResource<RamData>
{
public:
	OptimizedResource(ResourceManager* manager, const str& name);
	virtual ~OptimizedResource();

	void SetOptimizedData(OptimizedData data);

protected:
	OptimizedData optimized_data_;
};



// A type of resource that needs it's own clone data, but has much in common with
// the related clones. Used for sounds.
template<class RamData, class DiversifiedData>
class DiversifiedResource: public RamResource<RamData>
{
public:
	typedef std::unordered_map<const UserResource*, DiversifiedData, LEPRA_VOIDP_HASHER> UserDataTable;

	DiversifiedResource(ResourceManager* manager, const str& name);
	virtual ~DiversifiedResource();

	DiversifiedData GetUserData(const UserResource* user_resource);
	void FreeDiversified(UserResource* user_resource);

	const UserDataTable& GetDiversifiedData() const;

protected:
	virtual DiversifiedData CreateDiversifiedData() const = 0;
	virtual void ReleaseDiversifiedData(DiversifiedData data) const = 0;

	Lock lock_;
	UserDataTable user_diversified_table_;
};



class PhysicsResource: public RamResource<tbc::ChunkyPhysics*>
{
	typedef RamResource<tbc::ChunkyPhysics*> Parent;
public:
	typedef tbc::ChunkyPhysics* UserData;

	PhysicsResource(ResourceManager* manager, const str& name);
	virtual ~PhysicsResource();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();
	bool LoadName(const str& name);

private:
	logclass();
};



/*class AnimationResource: public OptimizedResource<void*, int>	// Perhaps a animation ID may be used on graphics accelerated hardware (i.e. PhysX or PS3)?
{
public:
	typedef void* UserData;

	AnimationResource(const str& name);

	bool Load();
};*/



template<class _Class, class _ClassLoader>
class ClassResourceBase: public RamResource<_Class*>
{
	typedef RamResource<_Class*> Parent;
public:
	typedef _Class* UserData;

	ClassResourceBase(ResourceManager* manager, const str& name);
	virtual ~ClassResourceBase();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();
	bool LoadWithName(const str& name);

private:
	logclass();
};

class ClassResource: public ClassResourceBase<tbc::ChunkyClass, tbc::ChunkyClassLoader>
{
	typedef ClassResourceBase<tbc::ChunkyClass, tbc::ChunkyClassLoader> Parent;
public:
	ClassResource(ResourceManager* manager, const str& name);
	virtual ~ClassResource();
};



class PhysicalTerrainResource: public RamResource<tbc::TerrainPatch*>
{
	typedef RamResource<tbc::TerrainPatch*> Parent;
public:
	typedef tbc::TerrainPatch* UserData;

	PhysicalTerrainResource(ResourceManager* manager, const str& name);
	virtual ~PhysicalTerrainResource();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();

private:
	logclass();
};



class RamImageResource: public RamResource<Canvas*>
{
	typedef RamResource<Canvas*> Parent;
public:
	typedef Canvas* UserData;

	RamImageResource(cure::ResourceManager* manager, const str& name);
	virtual ~RamImageResource();
	const str GetType() const;
	UserData GetUserData(const UserResource*) const;
	bool Load();
};



typedef UserTypeResource<PhysicsResource>		UserPhysicsResource;
typedef UserTypeResource<ClassResource>			UserClassResource;
//typedef UserTypeResource<tbc::...>			UserAnimationResource;
typedef UserTypeResource<PhysicalTerrainResource>	UserPhysicalTerrainResource;
typedef UserTypeResource<RamImageResource>		UserRamImageResource;



class ResourceManager {
public:
	typedef std::list<ResourceInfo> ResourceInfoList;
	typedef std::list<Resource*> ResourceList;

	ResourceManager(unsigned loader_thread_count);
	virtual ~ResourceManager();
	bool InitDefault();
	void StopClear();
	void SetLoadIntermission(double load_intermission);
	void SetInjectTimeLimit(double inject_time_limit);

	File* QueryFile(const str& filename);
	bool QueryFileExists(const str& filename);
	strutil::strvec ListFiles(const str& wildcard);

	TerrainFunctionManager* GetTerrainFunctionManager() const;
	void SetTerrainFunctionManager(TerrainFunctionManager* terrain_function_manager);	// May not be changed while its resources are loaded.

	// Note that a specific resource NAME may only be loaded as one TYPE, but
	// it may be loaded as many times as you wish. For instance: loading an
	// image as a painter resource and at the same time loading the same image
	// name as a renderer resource is an error with undefined behaviour.
	void Load(const str& name, UserResource* user_resource, UserResource::LoadCallback callback);
	void LoadUnique(const str& name, UserResource* user_resource, UserResource::LoadCallback callback);
	void AddLoaded(UserResource* user_resource);
	void SafeRelease(UserResource* user_resource);
	void Release(Resource* resource);
	Resource* ReferenceResource(const str& resource_name);
	bool IsLoading();
	bool WaitLoading();

	void Tick();	// Call often, preferably every frame.
	unsigned ForceFreeCache();	// Called to force immediate freeing of all cached resources.
	unsigned ForceFreeCache(const strutil::strvec& resource_type_list);	// Empty type list = all.

	size_t QueryResourceCount() const;
	size_t QueryCachedResourceCount() const;
	ResourceInfoList QueryResourceNames();

	ResourceList HookAllResourcesOfType(const str& type);	// Be cautious!
	void UnhookResources(ResourceList& resource_list);

protected:
	Resource* GetAddCachedResource(const str& name, UserResource* user_resource, bool& must_load);
	void StartLoad(Resource* resource);

	// Called by Tick (main thread) to push objects into the active table, optimize them and callback waiters.
	void InjectResourceLoop();
	bool InjectSingleResource(Resource* resource);

	void FreeCache();	// Called by Tick (main thread) to delete old/large resources.

	void ThreadLoaderLoop();	// Called by worker thread to load objects asynchronously.
	void SynchronousLoadLoop();	// Called by main thread to load all requested resources.
	bool PrepareRemoveInLoadProgress(Resource* resource);	// Drops the resource from the load list.
	void LoadSingleResource();

	inline void AssertIsMutexOwner() {
		deb_assert(thread_lock_.IsOwner());
	}

	Resource* CreateResource(UserResource* user_resource, const str& name);
	void DeleteResource(Resource* resource);

private:
	typedef std::unordered_map<str, Resource*> ResourceTable;
	typedef OrderedMap<str, Resource*> ResourceMap;
	typedef OrderedMap<Resource*, Resource*, LEPRA_VOIDP_HASHER> ResourceMapList;
	typedef std::unordered_set<Resource*, LEPRA_VOIDP_HASHER> ResourceSet;
	typedef std::vector<Resource*> ResourceArray;

	TerrainFunctionManager* terrain_function_manager_;

	unsigned loader_thread_count_;
	const str path_prefix_;
	MemberThread<ResourceManager> loader_thread_;
	Semaphore load_semaphore_;
	mutable Lock thread_lock_;
	ResourceTable active_resource_table_;	// In use. Holds non-unique resources.
	ResourceTable cached_resource_table_;	// On the way out. Holds non-unique resources.
	ResourceMapList request_load_list_;	// Under way to be loaded by worker thread. TODO: priority map thingie!
	ResourceMapList loaded_list_;		// Loaded by worker thread, worker thread will injected into the system at end of tick.
	ResourceArray post_load_delete_array_;	// Currently loading, or probably loading, worker thread will delete on next iteration.
	ResourceSet resource_safe_lookup_;	// Data owner for Resource*.
	Lock zip_lock_;
	ZipArchive* zip_file_;
	double inject_time_limit_;
	double load_intermission_;

	logclass();
};



}



#include "resourcemanager.inl"
