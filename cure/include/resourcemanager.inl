
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../lepra/include/imageloader.h"
#include "../../tbc/include/terrainpatch.h"



namespace cure {



template<class UserResourceType, class ResourceType>
UserTypeResourceBase<UserResourceType, ResourceType>::UserTypeResourceBase() {
}

template<class UserResourceType, class ResourceType>
UserTypeResourceBase<UserResourceType, ResourceType>::~UserTypeResourceBase() {
	// Parent class dereferences Resource*.
}

template<class UserResourceType, class ResourceType>
void UserTypeResourceBase<UserResourceType, ResourceType>::Load(
	ResourceManager* resource_manager, const str& name, TypeLoadCallback callback, bool keep) {
	// JB-TRICKY: this is what I gather that it takes to cast a callback parameter.
	//            The memento contains the "this" and "method" pointers...
	LoadCallback callback_cast;
	callback_cast.SetMemento(callback.GetMemento());
	resource_manager->Load(name, this, callback_cast);
	GetResource()->SetIsUnique(!keep);

}

template<class UserResourceType, class ResourceType>
void UserTypeResourceBase<UserResourceType, ResourceType>::LoadUnique(
	ResourceManager* resource_manager, const str& name, TypeLoadCallback callback) {
	// JB-TRICKY: this is what I gather that it takes to cast a callback parameter.
	//            The memento contains the "this" and "method" pointers...
	LoadCallback callback_cast;
	callback_cast.SetMemento(callback.GetMemento());
	resource_manager->LoadUnique(name, this, callback_cast);
}

template<class UserResourceType, class ResourceType>
void UserTypeResourceBase<UserResourceType, ResourceType>::FinalizeLoad() {
	if (GetConstResource()->GetLoadState() == kResourceLoadComplete) {
		typedef typename ResourceType::UserData UData;
		if (GetData() == UData(0)) {
			GetResource()->SetLoadState(kResourceLoadError);
		}
	}
}

template<class UserResourceType, class ResourceType>
typename ResourceType::UserRamData UserTypeResourceBase<UserResourceType, ResourceType>::GetRamData() const {
	return (((const ResourceType*)GetConstResource())->GetRamData());
}

template<class UserResourceType, class ResourceType>
typename ResourceType::UserData UserTypeResourceBase<UserResourceType, ResourceType>::GetData() const {
	// TODO: fix this unsafe cast from const!
	return (((ResourceType*)GetConstResource())->GetUserData(this));
}



// ----------------------------------------------------------------------------



template<class ResourceType>
UserTypeResource<ResourceType>::UserTypeResource() {
}

template<class ResourceType>
UserTypeResource<ResourceType>::~UserTypeResource() {
}

template<class ResourceType>
Resource* UserTypeResource<ResourceType>::CreateResource(ResourceManager* manager, const str& name) const {
	return (new ResourceType(manager, name));
}



// ----------------------------------------------------------------------------



template<class ResourceType, class SubtypeExtraType>
UserExtraTypeResource<ResourceType, SubtypeExtraType>::UserExtraTypeResource(const SubtypeExtraType& extra_data):
	extra_data_(extra_data) {
}

template<class ResourceType, class SubtypeExtraType>
UserExtraTypeResource<ResourceType, SubtypeExtraType>::~UserExtraTypeResource() {
}

template<class ResourceType, class SubtypeExtraType>
Resource* UserExtraTypeResource<ResourceType, SubtypeExtraType>::CreateResource(ResourceManager* manager, const str& name) const {
	return (new ResourceType(manager, name, GetExtraData()));
}

template<class ResourceType, class SubtypeExtraType>
SubtypeExtraType& UserExtraTypeResource<ResourceType, SubtypeExtraType>::GetExtraData() const {
	return (extra_data_);
}

template<class ResourceType, class SubtypeExtraType>
void UserExtraTypeResource<ResourceType, SubtypeExtraType>::SetExtraData(const SubtypeExtraType& extra_data) {
	extra_data_ = extra_data;
}



// ----------------------------------------------------------------------------



template<class _UserResourceType>
UserResourceOwner<_UserResourceType>::UserResourceOwner(_UserResourceType* user_resource, cure::ResourceManager* manager, const str& name):
	user_resource_(user_resource) {
	user_resource_->Load(manager, name, typename _UserResourceType::TypeLoadCallback(this, &UserResourceOwner::OnLoadCallback));
}

template<class _UserResourceType>
UserResourceOwner<_UserResourceType>::~UserResourceOwner() {
	delete user_resource_;
	user_resource_ = 0;
}

template<class _UserResourceType>
void UserResourceOwner<_UserResourceType>::OnLoadCallback(_UserResourceType* user_resource) {
	(void)user_resource;
}



// ----------------------------------------------------------------------------



template<class RamData>
RamData RamResource<RamData>::GetRamData() const {
	return (ram_data_);
}

template<class RamData>
void RamResource<RamData>::SetRamDataType(RamData data) {
	ram_data_ = data;
}

template<class RamData>
RamResource<RamData>::RamResource(ResourceManager* manager, const str& name):
	Parent(manager, name),
	ram_data_((RamData)0) {
}

template<class RamData>
RamResource<RamData>::~RamResource() {
}

template<class RamData>
void RamResource<RamData>::SetRamData(RamData data) {
	delete (ram_data_);
	SetRamDataType(data);
}



// ----------------------------------------------------------------------------



template<class RamData, class OptimizedData>
OptimizedResource<RamData, OptimizedData>::OptimizedResource(
	ResourceManager* manager, const str& name):
	RamResource<RamData>(manager, name),
	optimized_data_((OptimizedData)0) {
}

template<class RamData, class OptimizedData>
OptimizedResource<RamData, OptimizedData>::~OptimizedResource() {
	optimized_data_ = (OptimizedData)0;
}

template<class RamData, class OptimizedData>
void OptimizedResource<RamData, OptimizedData>::SetOptimizedData(OptimizedData data) {
	optimized_data_ = data;
}



// ----------------------------------------------------------------------------



template<class RamData, class DiversifiedData>
DiversifiedResource<RamData, DiversifiedData>::DiversifiedResource(ResourceManager* manager, const str& name):
	RamResource<RamData>(manager, name) {
}

template<class RamData, class DiversifiedData>
DiversifiedResource<RamData, DiversifiedData>::~DiversifiedResource() {
	// Diviersified table must be entirely free'd and cleared out by child class.
	deb_assert(user_diversified_table_.empty());
}

template<class RamData, class DiversifiedData>
DiversifiedData DiversifiedResource<RamData, DiversifiedData>::GetUserData(const UserResource* user_resource) {
	ScopeLock lock(&lock_);
	DiversifiedData instance_id = (DiversifiedData)0;
	typename UserDataTable::iterator x = user_diversified_table_.find(user_resource);
	if (x != user_diversified_table_.end()) {
		instance_id = x->second;
		deb_assert(instance_id != 0);
	} else {
		instance_id = CreateDiversifiedData();
		if (instance_id) {
			user_diversified_table_.insert(typename UserDataTable::value_type(user_resource, instance_id));
		}
	}
	return (instance_id);
}

template<class RamData, class DiversifiedData>
void DiversifiedResource<RamData, DiversifiedData>::FreeDiversified(UserResource* user_resource) {
	ScopeLock lock(&lock_);
	typename UserDataTable::iterator x = user_diversified_table_.find(user_resource);
	if (x != user_diversified_table_.end()) {
		DiversifiedData instance_id = x->second;
		user_diversified_table_.erase(x);
		ReleaseDiversifiedData(instance_id);
	}
}

template<class RamData, class DiversifiedData>
const typename DiversifiedResource<RamData, DiversifiedData>::UserDataTable& DiversifiedResource<RamData, DiversifiedData>::GetDiversifiedData() const {
	return user_diversified_table_;
}



// ----------------------------------------------------------------------------



template<class _Class, class _ClassLoader>
ClassResourceBase<_Class, _ClassLoader>::ClassResourceBase(cure::ResourceManager* manager, const str& name):
	Parent(manager, name) {
}

template<class _Class, class _ClassLoader>
ClassResourceBase<_Class, _ClassLoader>::~ClassResourceBase() {
	Parent::SetRamData((_Class*)0);
}

template<class _Class, class _ClassLoader>
const str ClassResourceBase<_Class, _ClassLoader>::GetType() const {
	return ("Class");
}

template<class _Class, class _ClassLoader>
typename ClassResourceBase<_Class, _ClassLoader>::UserData ClassResourceBase<_Class, _ClassLoader>::GetUserData(const cure::UserResource*) const {
	return (Parent::GetRamData());
}

template<class _Class, class _ClassLoader>
bool ClassResourceBase<_Class, _ClassLoader>::Load() {
	return LoadWithName(Resource::GetName());
}

template<class _Class, class _ClassLoader>
bool ClassResourceBase<_Class, _ClassLoader>::LoadWithName(const str& name) {
	deb_assert(Parent::GetRamData() == 0);
	Parent::SetRamData(new _Class());
	File* file = Resource::GetManager()->QueryFile(name);
	bool ok = (file != 0);
	if (ok) {
		_ClassLoader loader(file, false);
		ok = loader.Load(Parent::GetRamData());
	}
	delete file;
	return (ok);
}



}
