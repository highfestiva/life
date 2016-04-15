
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Lepra/Include/ImageLoader.h"
#include "../../Tbc/Include/TerrainPatch.h"



namespace Cure
{



template<class UserResourceType, class ResourceType>
UserTypeResourceBase<UserResourceType, ResourceType>::UserTypeResourceBase()
{
}

template<class UserResourceType, class ResourceType>
UserTypeResourceBase<UserResourceType, ResourceType>::~UserTypeResourceBase()
{
	// Parent class dereferences Resource*.
}

template<class UserResourceType, class ResourceType>
void UserTypeResourceBase<UserResourceType, ResourceType>::Load(
	ResourceManager* pResourceManager, const str& pName, TypeLoadCallback pCallback, bool pKeep)
{
	// JB-TRICKY: this is what I gather that it takes to cast a callback parameter.
	//            The memento contains the "this" and "method" pointers...
	LoadCallback lCallbackCast;
	lCallbackCast.SetMemento(pCallback.GetMemento());
	pResourceManager->Load(pName, this, lCallbackCast);
	GetResource()->SetIsUnique(!pKeep);

}

template<class UserResourceType, class ResourceType>
void UserTypeResourceBase<UserResourceType, ResourceType>::LoadUnique(
	ResourceManager* pResourceManager, const str& pName, TypeLoadCallback pCallback)
{
	// JB-TRICKY: this is what I gather that it takes to cast a callback parameter.
	//            The memento contains the "this" and "method" pointers...
	LoadCallback lCallbackCast;
	lCallbackCast.SetMemento(pCallback.GetMemento());
	pResourceManager->LoadUnique(pName, this, lCallbackCast);
}

template<class UserResourceType, class ResourceType>
void UserTypeResourceBase<UserResourceType, ResourceType>::FinalizeLoad()
{
	if (GetConstResource()->GetLoadState() == RESOURCE_LOAD_COMPLETE)
	{
		typedef typename ResourceType::UserData UData;
		if (GetData() == UData(0))
		{
			GetResource()->SetLoadState(RESOURCE_LOAD_ERROR);
		}
	}
}

template<class UserResourceType, class ResourceType>
typename ResourceType::UserRamData UserTypeResourceBase<UserResourceType, ResourceType>::GetRamData() const
{
	return (((const ResourceType*)GetConstResource())->GetRamData());
}

template<class UserResourceType, class ResourceType>
typename ResourceType::UserData UserTypeResourceBase<UserResourceType, ResourceType>::GetData() const
{
	// TODO: fix this unsafe cast from const!
	return (((ResourceType*)GetConstResource())->GetUserData(this));
}



// ----------------------------------------------------------------------------



template<class ResourceType>
UserTypeResource<ResourceType>::UserTypeResource()
{
}

template<class ResourceType>
UserTypeResource<ResourceType>::~UserTypeResource()
{
}

template<class ResourceType>
Resource* UserTypeResource<ResourceType>::CreateResource(ResourceManager* pManager, const str& pName) const
{
	return (new ResourceType(pManager, pName));
}



// ----------------------------------------------------------------------------



template<class ResourceType, class SubtypeExtraType>
UserExtraTypeResource<ResourceType, SubtypeExtraType>::UserExtraTypeResource(const SubtypeExtraType& pExtraData):
	mExtraData(pExtraData)
{
}

template<class ResourceType, class SubtypeExtraType>
UserExtraTypeResource<ResourceType, SubtypeExtraType>::~UserExtraTypeResource()
{
}

template<class ResourceType, class SubtypeExtraType>
Resource* UserExtraTypeResource<ResourceType, SubtypeExtraType>::CreateResource(ResourceManager* pManager, const str& pName) const
{
	return (new ResourceType(pManager, pName, GetExtraData()));
}

template<class ResourceType, class SubtypeExtraType>
SubtypeExtraType& UserExtraTypeResource<ResourceType, SubtypeExtraType>::GetExtraData() const
{
	return (mExtraData);
}

template<class ResourceType, class SubtypeExtraType>
void UserExtraTypeResource<ResourceType, SubtypeExtraType>::SetExtraData(const SubtypeExtraType& pExtraData)
{
	mExtraData = pExtraData;
}



// ----------------------------------------------------------------------------



template<class _UserResourceType>
UserResourceOwner<_UserResourceType>::UserResourceOwner(_UserResourceType* pUserResource, Cure::ResourceManager* pManager, const str& pName):
	mUserResource(pUserResource)
{
	mUserResource->Load(pManager, pName, typename _UserResourceType::TypeLoadCallback(this, &UserResourceOwner::OnLoadCallback));
}

template<class _UserResourceType>
UserResourceOwner<_UserResourceType>::~UserResourceOwner()
{
	delete mUserResource;
	mUserResource = 0;
}

template<class _UserResourceType>
void UserResourceOwner<_UserResourceType>::OnLoadCallback(_UserResourceType* pUserResource)
{
	(void)pUserResource;
}



// ----------------------------------------------------------------------------



template<class RamData>
RamData RamResource<RamData>::GetRamData() const
{
	return (mRamData);
}

template<class RamData>
void RamResource<RamData>::SetRamDataType(RamData pData)
{
	mRamData = pData;
}

template<class RamData>
RamResource<RamData>::RamResource(ResourceManager* pManager, const str& pName):
	Parent(pManager, pName),
	mRamData((RamData)0)
{
}

template<class RamData>
RamResource<RamData>::~RamResource()
{
}

template<class RamData>
void RamResource<RamData>::SetRamData(RamData pData)
{
	delete (mRamData);
	SetRamDataType(pData);
}



// ----------------------------------------------------------------------------



template<class RamData, class OptimizedData>
OptimizedResource<RamData, OptimizedData>::OptimizedResource(
	ResourceManager* pManager, const str& pName):
	RamResource<RamData>(pManager, pName),
	mOptimizedData((OptimizedData)0)
{
}

template<class RamData, class OptimizedData>
OptimizedResource<RamData, OptimizedData>::~OptimizedResource()
{
	mOptimizedData = (OptimizedData)0;
}

template<class RamData, class OptimizedData>
void OptimizedResource<RamData, OptimizedData>::SetOptimizedData(OptimizedData pData)
{
	mOptimizedData = pData;
}



// ----------------------------------------------------------------------------



template<class RamData, class DiversifiedData>
DiversifiedResource<RamData, DiversifiedData>::DiversifiedResource(ResourceManager* pManager, const str& pName):
	RamResource<RamData>(pManager, pName)
{
}

template<class RamData, class DiversifiedData>
DiversifiedResource<RamData, DiversifiedData>::~DiversifiedResource()
{
	// Diviersified table must be entirely free'd and cleared out by child class.
	deb_assert(mUserDiversifiedTable.empty());
}

template<class RamData, class DiversifiedData>
DiversifiedData DiversifiedResource<RamData, DiversifiedData>::GetUserData(const UserResource* pUserResource)
{
	ScopeLock lLock(&mLock);
	DiversifiedData lInstanceId = (DiversifiedData)0;
	typename UserDataTable::iterator x = mUserDiversifiedTable.find(pUserResource);
	if (x != mUserDiversifiedTable.end())
	{
		lInstanceId = x->second;
		deb_assert(lInstanceId != 0);
	}
	else
	{
		lInstanceId = CreateDiversifiedData();
		if (lInstanceId)
		{
			mUserDiversifiedTable.insert(typename UserDataTable::value_type(pUserResource, lInstanceId));
		}
	}
	return (lInstanceId);
}

template<class RamData, class DiversifiedData>
void DiversifiedResource<RamData, DiversifiedData>::FreeDiversified(UserResource* pUserResource)
{
	ScopeLock lLock(&mLock);
	typename UserDataTable::iterator x = mUserDiversifiedTable.find(pUserResource);
	if (x != mUserDiversifiedTable.end())
	{
		DiversifiedData lInstanceId = x->second;
		mUserDiversifiedTable.erase(x);
		ReleaseDiversifiedData(lInstanceId);
	}
}

template<class RamData, class DiversifiedData>
const typename DiversifiedResource<RamData, DiversifiedData>::UserDataTable& DiversifiedResource<RamData, DiversifiedData>::GetDiversifiedData() const
{
	return mUserDiversifiedTable;
}



// ----------------------------------------------------------------------------



template<class _Class, class _ClassLoader>
ClassResourceBase<_Class, _ClassLoader>::ClassResourceBase(Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName)
{
}

template<class _Class, class _ClassLoader>
ClassResourceBase<_Class, _ClassLoader>::~ClassResourceBase()
{
	Parent::SetRamData((_Class*)0);
}

template<class _Class, class _ClassLoader>
const str ClassResourceBase<_Class, _ClassLoader>::GetType() const
{
	return ("Class");
}

template<class _Class, class _ClassLoader>
typename ClassResourceBase<_Class, _ClassLoader>::UserData ClassResourceBase<_Class, _ClassLoader>::GetUserData(const Cure::UserResource*) const
{
	return (Parent::GetRamData());
}

template<class _Class, class _ClassLoader>
bool ClassResourceBase<_Class, _ClassLoader>::Load()
{
	return LoadWithName(Resource::GetName());
}

template<class _Class, class _ClassLoader>
bool ClassResourceBase<_Class, _ClassLoader>::LoadWithName(const str& pName)
{
	deb_assert(Parent::GetRamData() == 0);
	Parent::SetRamData(new _Class());
	File* lFile = Resource::GetManager()->QueryFile(pName);
	bool lOk = (lFile != 0);
	if (lOk)
	{
		_ClassLoader lLoader(lFile, false);
		lOk = lLoader.Load(Parent::GetRamData());
	}
	delete lFile;
	return (lOk);
}



}
