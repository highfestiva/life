
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/resourcemanager.h"
#include <algorithm>
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/hirestimer.h"
#include "../../lepra/include/memfile.h"
#include "../../lepra/include/path.h"
#include "../../lepra/include/resourcetracker.h"
#include "../../lepra/include/systemmanager.h"
#include "../../lepra/include/ziparchive.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../include/cure.h"
#include "../include/terrainfunctionmanager.h"
#include "../include/terrainpatchloader.h"



namespace cure {



UserResource::UserResource():
	resource_(0),
	parent_resource_(0),
	callback_block_count_(0) {
}

UserResource::~UserResource() {
	if (resource_) {
		SafeRelease(resource_->GetManager());
	}
	/*else {
		deb_assert(false);
	}*/
	SetParentResource(0);
}

void UserResource::SafeRelease(ResourceManager* manager) {
	if (manager) {
		manager->SafeRelease(this);
	}
	resource_ = 0;
}

void UserResource::SetParentResource(UserResource* parent_resource) {
	if (parent_resource) {
		deb_assert(!parent_resource_);
		deb_assert(parent_resource != this);
		parent_resource_ = parent_resource;
		parent_resource_->IncreaseCallbackBlockCount();
	} else {
		if (parent_resource_) {
			parent_resource_->DecreaseCallbackBlockCount();
		}
		parent_resource_ = parent_resource;
	}
}

void UserResource::CallbackParent(ResourceLoadState child_load_state) {
	UserResource* parent = parent_resource_;
	parent_resource_ = 0;
	if (parent) {
		if (child_load_state == kResourceLoadError) {
			parent->GetResource()->SetLoadState(child_load_state);
		}
		parent->GetResource()->Callback();
	}
}

void UserResource::IncreaseCallbackBlockCount() {
	++callback_block_count_;
}

int UserResource::DecreaseCallbackBlockCount() {
	int callback_block_count = callback_block_count_;
	if (callback_block_count_) {
		--callback_block_count_;
	}
	return (callback_block_count);
}

const str& UserResource::GetName() const {
	return (resource_->GetName());
}

ResourceLoadState UserResource::GetLoadState() const {
	ResourceLoadState _state = kResourceUnloaded;
	if (resource_) {
		_state = resource_->GetLoadState();
	}
	return (_state);
}

const Resource* UserResource::GetConstResource() const {
	return (resource_);
}

Resource* UserResource::GetResource() const {
	return (resource_);
}

void UserResource::SetResource(Resource* resource) {
	resource_ = resource;
}

void UserResource::PostProcess() {
}

void UserResource::FinalizeLoad() {
}



// ----------------------------------------------------------------------------



Resource::Resource(ResourceManager* manager, const str& name):
	manager_(manager),
	name_(name),
	reference_count_(0),
	state_(kResourceLoadError),
	is_unique_(false) {
}

Resource::~Resource() {
	state_ = kResourceLoadError;
	log_volatile(log_.Tracef(("Deleting resource "+name_).c_str(), kLevelTrace));
	manager_ = 0;

	CallbackList::iterator x = load_callback_list_.begin();
	while (x != load_callback_list_.end()) {
		UserResourceCallbackInfo& caller = *x;
		if (caller.user_resource_->GetConstResource() == this) {
			caller.user_resource_->SetResource(0);
		} else {
			x = load_callback_list_.erase(x);
		}
	}
	load_callback_list_.clear();
}

ResourceManager* Resource::GetManager() const {
	return (manager_);
}

const str& Resource::GetName() const {
	return (name_);
}

int Resource::Reference() {
	BusLock::Add(&reference_count_, 1);
	return (reference_count_);
}

int Resource::Dereference() {
	BusLock::Add(&reference_count_, -1);
	deb_assert(reference_count_ >= 0);
	return (reference_count_);
}

int Resource::GetReferenceCount() const {
	return (reference_count_);
}

void Resource::Resume() {
}

void Resource::Suspend() {
}

bool Resource::IsUnique() const {
	return (is_unique_);
}

void Resource::SetIsUnique(bool is_unique) {
	is_unique_ = is_unique;
}

bool Resource::IsReferenceType() const {
	return (false);
}

ResourceLoadState Resource::GetLoadState() const {
	return (state_);
}

void Resource::SetLoadState(ResourceLoadState state) {
	state_ = state;
}

void Resource::AddCaller(UserResource* user_resource, const UserResource::LoadCallback& callback) {
	ScopeLock mutex(&mutex_);
	load_callback_list_.push_back(UserResourceCallbackInfo(user_resource, callback));
}

void Resource::RemoveCaller(UserResource* user_resource) {
	ScopeLock mutex(&mutex_);

	CallbackList::iterator x = load_callback_list_.begin();
	while (x != load_callback_list_.end()) {
		UserResourceCallbackInfo& caller = *x;
		if (caller.user_resource_ == user_resource) {
			load_callback_list_.erase(x);
			break;
		} else {
			++x;
		}
	}
}

ResourceLoadState Resource::PostProcess() {
	UserPostProcess();
	return (kResourceLoadComplete);	// No post-processing required; we're done.
}

void Resource::UserPostProcess() {
	CallbackList callback_list;
	{
		ScopeLock mutex(&mutex_);
		callback_list = load_callback_list_;
	}

	CallbackList::iterator x = callback_list.begin();
	for (; x != callback_list.end(); ++x) {
		UserResourceCallbackInfo& caller = *x;
		caller.user_resource_->PostProcess();
	}
}

void Resource::Callback() {
	Reference();	// Is here to stop a callback from dereferencing+deleting this resource.

	CallbackList callback_list;
	{
		ScopeLock mutex(&mutex_);
		callback_list.splice(callback_list.begin(), load_callback_list_);
	}
	size_t callback_count = callback_list.size();
	for (size_t x = 0; x < callback_count; ++x) {
		UserResourceCallbackInfo caller = *callback_list.begin();
		callback_list.pop_front();
		if (caller.user_resource_->DecreaseCallbackBlockCount() > 0) {
			// Move to back of list = don't call yet.
			callback_list.push_back(caller);
		} else {
			//const int lPreviousCount = (int)callback_list.size();
			caller.user_resource_->FinalizeLoad();
			ResourceLoadState load_state = caller.user_resource_->GetResource()->GetLoadState();
			caller.callback_(caller.user_resource_);
			caller.user_resource_->CallbackParent(load_state);
			/*// Skip the once deleted during the parent's callback.
			const int lCurrentCount = (int)callback_list.size();
			if (lCurrentCount < lPreviousCount) {
				callback_count -= lPreviousCount-lCurrentCount;
			}*/
		}
	}
	if (callback_list.size() > 0) {
		ScopeLock mutex(&mutex_);
		load_callback_list_.splice(load_callback_list_.begin(), callback_list);
	}

	manager_->Release(this);
}

void Resource::FreeDiversified(UserResource*) {
}

UserResource* Resource::GetFirstUserResource() const {
	deb_assert(load_callback_list_.size() == 1);
	if (!load_callback_list_.empty()) {
		return (load_callback_list_.front().user_resource_);
	}
	return (0);
}

void Resource::PatchInfo(ResourceInfo&) const {
}

void Resource::operator=(const Resource&) {
	deb_assert(false);
}

Lock Resource::mutex_;

loginstance(kGeneralResources, Resource);



// ----------------------------------------------------------------------------



PhysicsResource::PhysicsResource(cure::ResourceManager* manager, const str& name):
	Parent(manager, name) {
}

PhysicsResource::~PhysicsResource() {
	SetRamData(0);
}

const str PhysicsResource::GetType() const {
	return ("Physics");
}

PhysicsResource::UserData PhysicsResource::GetUserData(const cure::UserResource*) const {
	return (GetRamData());
}

bool PhysicsResource::Load() {
	return LoadName(GetName());
}

bool PhysicsResource::LoadName(const str& name) {
	deb_assert(GetRamData() == 0);
	SetRamData(new tbc::ChunkyPhysics(tbc::ChunkyPhysics::kTransformLocal2World));
	File* file = GetManager()->QueryFile(name);
	bool ok = (file != 0);
	if (ok) {
		tbc::ChunkyPhysicsLoader loader(file, false);
		ok = loader.Load(GetRamData());
	}
	delete file;
	return (ok);
}



ClassResource::ClassResource(cure::ResourceManager* manager, const str& name):
	Parent(manager, name) {
}

ClassResource::~ClassResource() {
}



// ----------------------------------------------------------------------------



PhysicalTerrainResource::PhysicalTerrainResource(ResourceManager* manager, const str& name):
	Parent(manager, name) {
}

PhysicalTerrainResource::~PhysicalTerrainResource() {
	// TRICKY: delete of data must lie in this class, and not in parent class RamResource.
	// This is so since types (uilepra::SoundManager::SoundID) cannot be deleted!
	SetRamData(0);

	log_trace("Deleting terrain patch resource.");
}

const str PhysicalTerrainResource::GetType() const {
	return ("PhysTerrain");
}

PhysicalTerrainResource::UserData PhysicalTerrainResource::GetUserData(const UserResource*) const {
	return (GetRamData());
}

bool PhysicalTerrainResource::Load() {
	log_trace("Loading actual Tbc::TerrainPatch.");

	deb_assert(!IsUnique());
	// TODO: parse constants out of resource name string.
	const TerrainPatchLoader::PatchArea area(0, 0, 0, 0);
	const float lod = 0;

	TerrainPatchLoader loader(GetManager());
	tbc::TerrainPatch* patch = 0;
	for (int x = 0; x < 3 && !patch; ++x) {	// Retry file loading, file might be held by anti-virus/Windoze/similar shit.
		patch = loader.LoadPatch(area, lod);
	}
	if (patch) {
		SetRamData(patch);
	}
	return (patch != 0);
}

loginstance(kPhysics, PhysicalTerrainResource);



// ----------------------------------------------------------------------------



RamImageResource::RamImageResource(cure::ResourceManager* manager, const str& name):
	Parent(manager, name) {
}

RamImageResource::~RamImageResource() {
}

const str RamImageResource::GetType() const {
	return ("RamImg");
}

RamImageResource::UserData RamImageResource::GetUserData(const cure::UserResource*) const {
	return (GetRamData());
}

bool RamImageResource::Load() {
	deb_assert(!IsUnique());
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



// ----------------------------------------------------------------------------



ResourceManager::ResourceManager(unsigned loader_thread_count):
	terrain_function_manager_(0),
	loader_thread_count_(loader_thread_count),
	path_prefix_(SystemManager::GetDataDirectory()),
	loader_thread_("ResourceLoader"),
	zip_file_(new ZipArchive),
	inject_time_limit_(0.01),
	load_intermission_(0) {
	if (zip_file_->OpenArchive(path_prefix_ + "data.pk3", ZipArchive::kReadOnly) != kIoOk) {
		delete zip_file_;
		zip_file_ = 0;
	}
}

ResourceManager::~ResourceManager() {
	StopClear();
	SetTerrainFunctionManager(0);

	delete zip_file_;
	zip_file_ = 0;
}

bool ResourceManager::InitDefault() {
	// Resources below deleted in the resource manager destructor.
	SetTerrainFunctionManager(new TerrainFunctionManager());
	return (loader_thread_.Start(this, &ResourceManager::ThreadLoaderLoop));
}

void ResourceManager::StopClear() {
	// Join thread. 1: request stop, 2: signal, 3: sleep, 4: wait for termination.
	loader_thread_.RequestStop();
	for (int x = 0; x < 10; ++x) {
		load_semaphore_.Signal();
		if (loader_thread_.Join(1.0)) {
			break;
		}
	}
	loader_thread_.Kill();

	ScopeLock lock(&thread_lock_);	// Just here for mutex lock verification.

	request_load_list_.RemoveAll();	// We claim nothin's cookin'.

	// Iteratively free memory. First free all resources that ARE of reference type,
	// then all cure resources. Inside that top-level order, we first kill resources,
	// without any references, then those with only 1 ref, etc. This should yield
	// correct hierarchical destruction.
	bool kill_references_only = true;	// Kill all references first.
	int ref_count_threshold = 0;
	while (!active_resource_table_.empty()) {
		ForceFreeCache();	// Must be before and after active resources' are deleted (caused by hierarchical resource structures).
		bool killed = false;
		bool are_references_left = false;
		ResourceTable::iterator x = active_resource_table_.begin();
		while (x != active_resource_table_.end()) {
			Resource* _resource = x->second;
			deb_assert(request_load_list_.Find(_resource) == request_load_list_.End());
			if ((!kill_references_only || _resource->IsReferenceType()) &&
				_resource->GetReferenceCount() <= ref_count_threshold) {
				x = active_resource_table_.erase(x);
				// Check that no-one else has deleted our resource.
				if (resource_safe_lookup_.find(_resource) != resource_safe_lookup_.end()) {
					if (_resource->IsReferenceType()) {
						--ref_count_threshold;
					}
					DeleteResource(_resource);
					killed = true;
				}
			} else {
				are_references_left |= _resource->IsReferenceType();
				++x;
			}
		}
		if (kill_references_only && !are_references_left) {
			kill_references_only = false;
			ref_count_threshold = 0;
		} else if (!killed) {
			++ref_count_threshold;
		}
	}
	for (int x = 0; x < 10; ++x) {	// Also frees base resources, which may have just had their referencing resourced freed.
		ForceFreeCache();
	}

	// First kill all reference resources.
	bool delete_more_references = true;
	while (delete_more_references) {
		delete_more_references = false;
		for (ResourceSet::iterator x = resource_safe_lookup_.begin(); x != resource_safe_lookup_.end(); ++x) {
			Resource* _resource = *x;
			if (_resource->IsReferenceType()) {
				log_.Errorf("Reference resource '%s' not freed! Going for the kill!", _resource->GetName().c_str());
				DeleteResource(_resource);
				delete_more_references = true;
				break;
			}
		}
	}

	// Then kill the rest (non-reference resources).
	while (!resource_safe_lookup_.empty()) {
		Resource* _resource = *resource_safe_lookup_.begin();
		log_.Errorf("Base resource '%s' not freed! Going for the kill!", _resource->GetName().c_str());
		DeleteResource(_resource);
	}

	// These just contain pointers, they are not data owners. Thus no delete required.
	request_load_list_.RemoveAll();
	loaded_list_.RemoveAll();
}

void ResourceManager::SetLoadIntermission(double load_intermission) {
	load_intermission_ = load_intermission;
}

void ResourceManager::SetInjectTimeLimit(double inject_time_limit) {
	inject_time_limit_ = inject_time_limit;
}



File* ResourceManager::QueryFile(const str& filename) {
	str _filename = strutil::ReplaceAll(filename, '\\', '/');
	if (zip_file_) {
		ScopeLock lock(&zip_lock_);
		if (zip_file_->FileOpen(_filename)) {
			const size_t __size = (size_t)zip_file_->FileSize();
			MemFile* file = new MemFile;
			if (zip_file_->FileRead(file->GetBuffer(__size), __size) != kIoOk) {
				delete file;
				file = 0;
			}
			zip_file_->FileClose();
			if (file) {
				file->SeekSet(0);
				return file;
			}
		}
	}

	_filename = path_prefix_ + _filename;
	DiskFile* file = new DiskFile;
	bool ok = false;
	for (int x = 0; x < 3 && !ok; ++x) {	// Retry file open, file might be held by anti-virus/Windoze/similar shit.
		ok = file->Open(_filename, DiskFile::kModeRead);
	}
	if (ok) {
		return file;
	} else {
		log_.Errorf("Could not load file with name '%s'.", _filename.c_str());
	}
	delete file;
	return 0;
}

bool ResourceManager::QueryFileExists(const str& filename) {
	if (zip_file_) {
		ScopeLock lock(&zip_lock_);
		if (zip_file_->FileExist(filename)) {
			return true;
		}
	}
	const str _filename = path_prefix_ + strutil::ReplaceAll(filename, '\\', '/');
	return DiskFile::Exists(_filename);
}

strutil::strvec ResourceManager::ListFiles(const str& wildcard) {
	strutil::strvec filename_array;
	if (zip_file_) {
		ScopeLock lock(&zip_lock_);
		str _filename;
		for (_filename = zip_file_->FileFindFirst(); !_filename.empty(); _filename = zip_file_->FileFindNext()) {
			if (Path::IsWildcardMatch(wildcard, _filename)) {
				filename_array.push_back(_filename);
			}
		}
		return filename_array;
	}

	const str _wildcard = path_prefix_ + strutil::ReplaceAll(wildcard, '\\', '/');
	DiskFile::FindData info;
	for (bool ok = DiskFile::FindFirst(_wildcard, info); ok; ok = DiskFile::FindNext(info)) {
		filename_array.push_back(info.GetName());
	}
	return filename_array;
}


TerrainFunctionManager* ResourceManager::GetTerrainFunctionManager() const {
	return (terrain_function_manager_);
}

void ResourceManager::SetTerrainFunctionManager(TerrainFunctionManager* terrain_function_manager) {
	if (terrain_function_manager_) {
		delete (terrain_function_manager_);
	}
	terrain_function_manager_ = terrain_function_manager;
}



void ResourceManager::Load(const str& name, UserResource* user_resource, UserResource::LoadCallback callback) {
	Resource* _resource;
	{
		bool _must_load = true;
		ScopeLock lock(&thread_lock_);
		_resource = GetAddCachedResource(name, user_resource, _must_load);
		user_resource->SetResource(_resource);
		_resource->AddCaller(user_resource, callback);
		if (_must_load) {
			StartLoad(_resource);
			return;	// TRICKY: RAII simplifies.
		}
	}

	// Resource is already loaded.
	if (_resource->GetLoadState() == kResourceLoadInProgress) {
		// Loader thread is going to put this object in the request load list when it's done.
		// We have already placed the caller in the list, so we don't need to do anything else.
	} else {
		// The object is already loaded by some previous call. All we need to to is to inform the
		// current caller of resource load result.
		if (_resource->GetLoadState() == kResourceLoadComplete) {
			_resource->UserPostProcess();
		}
		_resource->Callback();
	}
}

void ResourceManager::LoadUnique(const str& name, UserResource* user_resource, UserResource::LoadCallback callback) {
	ScopeLock lock(&thread_lock_);
	Resource* _resource = CreateResource(user_resource, name);
	_resource->SetIsUnique(true);
	user_resource->SetResource(_resource);
	_resource->AddCaller(user_resource, callback);
	_resource->Reference();
	StartLoad(_resource);
}

void ResourceManager::AddLoaded(UserResource* user_resource) {
	deb_assert(user_resource->GetLoadState() != kResourceLoadError);
	ScopeLock lock(&thread_lock_);
	Resource* _resource = user_resource->GetResource();
	_resource->Reference();
	resource_safe_lookup_.insert(_resource);
	active_resource_table_.insert(ResourceTable::value_type(_resource->GetName(), _resource));
	loaded_list_.PushBack(_resource, _resource);
}

void ResourceManager::SafeRelease(UserResource* user_resource) {
	Resource* _resource = user_resource->GetResource();
	if (_resource) {
		user_resource->SetResource(0);

		ScopeLock lock(&thread_lock_);
		if (resource_safe_lookup_.find(_resource) != resource_safe_lookup_.end()) {
			_resource->RemoveCaller(user_resource);
			_resource->FreeDiversified(user_resource);
			Release(_resource);
		} else {
			deb_assert(false);
		}
	} else {
		deb_assert(false);
	}
}

void ResourceManager::Release(Resource* resource) {
	if (resource->Dereference() <= 0) {
		deb_assert(resource->GetReferenceCount() == 0);
		ScopeLock mutex(&thread_lock_);

		if (!resource->IsUnique()) {
			active_resource_table_.erase(resource->GetName());
			if (resource->GetLoadState() == kResourceLoadComplete) {
				// A completely loaded resource dropped, place it "on the way out" of the system.
				cached_resource_table_.insert(ResourceTable::value_type(resource->GetName(), resource));
				log_volatile(log_.Trace("Loaded resource "+resource->GetName()+" dereferenced. Placed in cache."));
				resource->Suspend();
			} else {
				if (PrepareRemoveInLoadProgress(resource)) {
					log_volatile(log_.Trace("Incomplete resource "+resource->GetName()+" dereferenced. Not cached - deleted immediately."));
					deb_assert(request_load_list_.Find(resource) == request_load_list_.End());
					DeleteResource(resource);
				} else {
					log_volatile(log_.Trace("Currently loading resource "+resource->GetName()+" dereferenced. Not cached - will be deleted immediately after loader thread is done."));
					deb_assert(request_load_list_.Find(resource) == request_load_list_.End());
				}
			}
		} else {
			active_resource_table_.erase(resource->GetName());	// TRICKY: this is for shared resources that are not to be kept any more when dereferenced.
			if (PrepareRemoveInLoadProgress(resource)) {
				log_volatile(log_.Trace("Resource "+resource->GetName()+" (unique) dereferenced. Deleted immediately."));
				deb_assert(request_load_list_.Find(resource) == request_load_list_.End());
				DeleteResource(resource);
			} else {
				log_volatile(log_.Trace("Resource "+resource->GetName()+" (unique) dereferenced. Will be deleted immediately after loader thread is done."));
				deb_assert(request_load_list_.Find(resource) == request_load_list_.End());
			}
		}
	} else {
		log_volatile(log_.Trace("Resource "+resource->GetName()+" dereferenced, but has other references."));
	}
}

bool ResourceManager::IsLoading() {
	ScopeLock lock(&thread_lock_);
	return (request_load_list_.GetCount() || loaded_list_.GetCount());
}

bool ResourceManager::WaitLoading() {
	for (int x = 0; IsLoading() && SystemManager::GetQuitRequest() == 0 && x < 200; ++x) {
		Thread::Sleep(0.1);
	}
	return !IsLoading();
}



void ResourceManager::Tick() {
	if (loader_thread_count_ == 0) {
		SynchronousLoadLoop();
	}
	InjectResourceLoop();
	FreeCache();
}

unsigned ResourceManager::ForceFreeCache() {
	return ForceFreeCache(strutil::strvec());
}

unsigned ResourceManager::ForceFreeCache(const strutil::strvec& resource_type_list) {
	ScopeLock lock(&thread_lock_);
	// TODO: optimize by keeping objects in cache for a while!

	/*log_.Headline("ForceFreeCache...");
	ResourceTable::iterator x = cached_resource_table_.begin();
	for (; x != cached_resource_table_.end(); ++x) {
		log_.Headlinef("  - %s @ %p.", (*x)->GetName().c_str(), *x);
	}
	log_.Headline("---------------");*/
	int prev_dropped_resource_count = -1;
	unsigned dropped_resource_count = 0;
	while (prev_dropped_resource_count != dropped_resource_count) {
		prev_dropped_resource_count = dropped_resource_count;
		ResourceTable::iterator x = cached_resource_table_.begin();
		while (x != cached_resource_table_.end()) {
			Resource* _resource = x->second;
			deb_assert(request_load_list_.Find(_resource) == request_load_list_.End());
			bool drop = resource_type_list.empty();
			const str& _type = _resource->GetType();
			strutil::strvec::const_iterator y = resource_type_list.begin();
			for (; !drop && y != resource_type_list.end(); ++y) {
				drop = (_type == *y);
			}
			if (drop) {
				x = cached_resource_table_.erase(x);
				DeleteResource(_resource);
				++dropped_resource_count;
				break;
			} else {
				++x;
			}
		}
	}
	return dropped_resource_count;
}



size_t ResourceManager::QueryResourceCount() const {
	ScopeLock lock(&thread_lock_);
	return (resource_safe_lookup_.size());
}

size_t ResourceManager::QueryCachedResourceCount() const {
	ScopeLock lock(&thread_lock_);
	return (cached_resource_table_.size());
}

ResourceManager::ResourceInfoList ResourceManager::QueryResourceNames() {
	ResourceInfoList names;
	ScopeLock lock(&thread_lock_);
	ResourceSet::iterator x = resource_safe_lookup_.begin();
	for (; x != resource_safe_lookup_.end(); ++x) {
		Resource* _resource = *x;
		ResourceInfo info;
		info.name_ = _resource->GetName();
		info.type_ = _resource->GetType();
		info.reference_count_ = _resource->GetReferenceCount();
		_resource->PatchInfo(info);
		names.push_back(info);
	}
	return names;
}

ResourceManager::ResourceList ResourceManager::HookAllResourcesOfType(const str& type) {
	ResourceList list;
	thread_lock_.Acquire();
	ResourceSet::iterator x = resource_safe_lookup_.begin();
	for (; x != resource_safe_lookup_.end(); ++x) {
		Resource* _resource = *x;
		if (strutil::StartsWith(_resource->GetType(), type)) {
			_resource->Reference();
			list.push_back(_resource);
		}
	}
	return list;
}

void ResourceManager::UnhookResources(ResourceList& resource_list) {
	ResourceList::iterator x = resource_list.begin();
	for (; x != resource_list.end(); ++x) {
		Resource* _resource = *x;
		_resource->Dereference();
	}
	resource_list.clear();
	thread_lock_.Release();
}



Resource* ResourceManager::GetAddCachedResource(const str& name, UserResource* user_resource, bool& must_load) {
	must_load = false;
	Resource* _resource = 0;
	ResourceTable::iterator x = active_resource_table_.find(name);
	if (x != active_resource_table_.end()) {
		_resource = x->second;
	} else {
		ResourceTable::iterator x = cached_resource_table_.find(name);
		if (x != cached_resource_table_.end()) {
			_resource = x->second;
			// Resource found on the way out, move it back into the system.
			cached_resource_table_.erase(_resource->GetName());
			deb_assert(active_resource_table_.find(_resource->GetName()) == active_resource_table_.end());
			deb_assert(_resource->GetLoadState() == kResourceLoadComplete);
			active_resource_table_.insert(ResourceTable::value_type(_resource->GetName(), _resource));
			_resource->Resume();
		}
	}
	if (!_resource) {
		//deb_assert(request_load_list_.Find(name) == request_load_list_.End());
		_resource = CreateResource(user_resource, name);
		deb_assert(active_resource_table_.find(_resource->GetName()) == active_resource_table_.end());
		deb_assert(_resource->GetLoadState() == kResourceLoadError);
		active_resource_table_.insert(ResourceTable::value_type(_resource->GetName(), _resource));
		deb_assert(request_load_list_.Find(_resource) == request_load_list_.End());
		must_load = true;
		log_volatile(log_.Trace("Resource "+name+" created + starts loading."));
	} else {
		if (_resource->GetLoadState() == kResourceUnloaded) {
			log_volatile(log_.Trace("Resource "+name+" will reload."));
			must_load = true;
		} else if (_resource->GetLoadState() == kResourceLoadInProgress) {
			log_volatile(log_.Debug("Resource "+name+" currently loading, will use it when done."));
		} else {
			log_volatile(log_.Trace("Resource "+name+" already loaded, will use it instead of reloading."));
		}
	}
	//deb_assert(!_resource->IsUnique());
	_resource->Reference();
	return (_resource);
}

void ResourceManager::StartLoad(Resource* resource) {
	AssertIsMutexOwner();

	// Pass on to loader thread.
	deb_assert(resource->GetReferenceCount() > 0);
	resource->SetLoadState(kResourceLoadInProgress);
	deb_assert(request_load_list_.GetCount() < 10000);	// Just run GetCount() to validate internal integrity.
	log_volatile(const str& _name = resource->GetName());
	log_volatile(log_.Tracef("Requesting load of '%s' (%s).", _name.c_str(), resource->GetType().c_str()));
	deb_assert(request_load_list_.Find(resource) == request_load_list_.End());
	request_load_list_.PushBack(resource, resource);
	load_semaphore_.Signal();
}



void ResourceManager::InjectResourceLoop() {
	HiResTimer timer(false);

	// ---------------------------
	// NOTE: this lock must be here, as the state otherwise changes outside of the lock state, which causes all sorts
	//       of problems, especially when deleting just loaded resources which have not yet been postprocessed. Also,
	//       there is no way to stop injection once the resource has been postprocessed by the injection thread.
	ScopeLock lock(&thread_lock_);
	// ---------------------------

	for (ResourceMapList::Iterator x = loaded_list_.First(); x != loaded_list_.End();) {
		Resource* _resource = x.GetObject();
		if (InjectSingleResource(_resource)) {
			ResourceMapList::Iterator y = x;
			++y;
			loaded_list_.Remove(x);
			x = y;
		} else {
			++x;
		}
		if (timer.QueryTimeDiff() > inject_time_limit_) {	// Time's up, have a go later.
			break;
		}
	}
}

bool ResourceManager::InjectSingleResource(Resource* resource) {
	bool inject_done = true;
	if (resource->GetLoadState() == kResourceLoadInProgress) {
		ResourceLoadState load_state = resource->PostProcess();
		if (load_state != kResourceLoadInProgress) {
			resource->SetLoadState(load_state);
			inject_done = true;
		} else {
			inject_done = false;
		}
	}
	if (inject_done) {
		resource->Callback();
	}
	return (inject_done);
}



void ResourceManager::FreeCache() {
	// TODO: optimize this algoritm by keeping track of time an object has spent
	// in cache, dropping the oldest first, and so forth.
	if (SystemManager::GetAvailRam() < SystemManager::GetAmountRam()/4) {
		ForceFreeCache();
	}
}



void ResourceManager::ThreadLoaderLoop() {
	while (!loader_thread_.GetStopRequest()) {
		LoadSingleResource();
		// Smooth out loads, so all the heavy lifting won't end up in a single frame.
		size_t count = 0;
		{
			ScopeLock lock(&thread_lock_);
			count = request_load_list_.GetCount();
		}
		if (count > 0 && count < 50) {
			Thread::Sleep(load_intermission_);
		}
		load_semaphore_.Wait();
	}
}

void ResourceManager::SynchronousLoadLoop() {
	while (request_load_list_.GetCount() > 0) {
		LoadSingleResource();
	}
}

bool ResourceManager::PrepareRemoveInLoadProgress(Resource* resource) {
	AssertIsMutexOwner();

	// We assume that only the main thread is at work. If more threads are working
	// the system, the following race condition can and will happen:
	//
	//   1.          Thread A:  starts loading resource R.
	//   2. Resource thread T:  loads resource R and places in LOADED list.
	//   3.          Thread A:  ticks the resource, picking it out of the LOADED list, RM now unlocked.
	//   4.          Thread B:  tries to destroy resource R in loading state, but cannot find it in LOADED
	//                          nor REQUEST_LOADED lists.
	bool allow_delete = true;
	if (resource->GetLoadState() == kResourceLoadInProgress) {
		// This is a bit complex, but the resource can enter the "loaded list", which it does when it's
		// "Load" method is first called. However, it may still depend asynchronously on other resources
		// which are currently loading. So it keeps its "load in progress" state, but remains in the
		// "loaded list".
		//deb_assert(request_load_list_.Exists(resource) && !loaded_list_.Exists(resource));

		// Only the first object in the 'request load' list may be currently loading.
		if (request_load_list_.GetCount() > 0 && resource == request_load_list_.First().GetObject()) {
			allow_delete = false;
			post_load_delete_array_.push_back(resource);
			load_semaphore_.Signal();	// Kick it, this may be a recursive delete request.
		} else {
			// If it's in the 'request load' list, but not first, simply remove it
			// and it's safe to delete.
			deb_assert(request_load_list_.Find(resource) != request_load_list_.End() ||
				loaded_list_.Find(resource) != loaded_list_.End());
		}
		request_load_list_.Remove(resource);
	}
	deb_assert(request_load_list_.GetCount() < 10000);
	loaded_list_.Remove(resource);
	deb_assert(loaded_list_.GetCount() < 10000);
	return (allow_delete);
}

void ResourceManager::LoadSingleResource() {
	// 1. Lock, retrieve resource, unlock.
	// 2. Load resource.
	// 3. Lock, move resource to loaded list, unlock.

	size_t list_count = 0;
	Resource* _resource = 0;
	{
		ScopeLock lock(&thread_lock_);
		list_count = request_load_list_.GetCount();
		if (list_count > 0) {
			_resource = request_load_list_.First().GetObject();
		}
	}

	if (_resource) {
		deb_assert(_resource->GetLoadState() == kResourceLoadInProgress);
		log_volatile(log_.Tracef("Loading %s with %i resources in list (inclusive).",
			_resource->GetName().c_str(), list_count));
		const bool is_loaded = _resource->Load();
		deb_assert(_resource->GetLoadState() == kResourceLoadInProgress);
		if (!is_loaded) {
			_resource->SetLoadState(kResourceLoadError);
		}
		{
			ScopeLock lock(&thread_lock_);
			if (!request_load_list_.IsEmpty() && _resource == request_load_list_.First().GetObject()) {
				deb_assert(request_load_list_.GetCount() < 10000);
				request_load_list_.Remove(_resource);
				deb_assert(request_load_list_.GetCount() < 10000);
				loaded_list_.PushBack(_resource, _resource);
			} else {
				// Resource was released while loading, but could not be deleted since this
				// thread was loading it. Therefore we delete it here. No use trying to cache
				// this resource, since it needs both Suspend() and PostProcess(); in addition
				// the last call must also come from the main thread.
				deb_assert(request_load_list_.Find(_resource) == request_load_list_.End());
				deb_assert(_resource->GetReferenceCount() == 0);
				log_.Info("Deleting just loaded resource '"+_resource->GetName()+"'.");
				DeleteResource(_resource);
			}

			// Delete any remaining (usually depending-upon) resources.
			while (!post_load_delete_array_.empty()) {
				DeleteResource(post_load_delete_array_.back());
			}
		}
	} else {
		ScopeLock lock(&thread_lock_);
		// Delete any remaining (usually depending-upon) resources.
		while (!post_load_delete_array_.empty()) {
			DeleteResource(post_load_delete_array_.back());
		}
	}
}



Resource* ResourceManager::CreateResource(UserResource* user_resource, const str& name) {
	AssertIsMutexOwner();

	Resource* resource = user_resource->CreateResource(this, name);
	resource_safe_lookup_.insert(resource);
	return (resource);
}

void ResourceManager::DeleteResource(Resource* resource) {
	AssertIsMutexOwner();

	deb_assert(resource_safe_lookup_.find(resource) != resource_safe_lookup_.end());
	deb_assert(request_load_list_.Find(resource) == request_load_list_.End());
	resource_safe_lookup_.erase(resource);

	// Drop resource from post load delete array.
	post_load_delete_array_.erase(std::remove(post_load_delete_array_.begin(), post_load_delete_array_.end(), resource), post_load_delete_array_.end());

	{
		const int lock_count = thread_lock_.GetReferenceCount();
		for (int i = 0; i < lock_count; ++i) thread_lock_.Release();	// Required since we may need other system-wide locks in destructor.
		deb_assert(!thread_lock_.IsOwner());
		delete (resource);
		for (int i = 0; i < lock_count; ++i) thread_lock_.Acquire();
	}
}



loginstance(kGeneralResources, ResourceManager);



}
