
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../../Lepra/Include/ImageLoader.h"
#include "../../TBC/Include/TerrainPatch.h"



namespace Cure
{



template<class ResourceType, class SubtypeLoadCallback, class SubtypeExtraData>
UserTypeResourceBase<ResourceType, SubtypeLoadCallback, SubtypeExtraData>::UserTypeResourceBase(const SubtypeExtraData& pExtraData):
	UserResource(),
	mExtraData(pExtraData)
{
}

template<class ResourceType, class SubtypeLoadCallback, class SubtypeExtraData>
UserTypeResourceBase<ResourceType, SubtypeLoadCallback, SubtypeExtraData>::~UserTypeResourceBase()
{
	// Parent class dereferences Resource*.
}

template<class ResourceType, class SubtypeLoadCallback, class SubtypeExtraData>
void UserTypeResourceBase<ResourceType, SubtypeLoadCallback, SubtypeExtraData>::Load(ResourceManager* pResourceManager,
	const Lepra::String& pName, TypeLoadCallback pCallback)
{
	// JB-TRICKY: this is what I gather that it takes to cast a callback parameter.
	//            The memento contains the "this" and "method" pointers...
	LoadCallback lCallbackCast;
	lCallbackCast.SetMemento(pCallback.GetMemento());
	pResourceManager->Load(pName, this, lCallbackCast);
}

template<class ResourceType, class SubtypeLoadCallback, class SubtypeExtraData>
void UserTypeResourceBase<ResourceType, SubtypeLoadCallback, SubtypeExtraData>::ReleaseRamResource()
{
	((ResourceType*)GetResource())->SetRamDataType(0);
}

template<class ResourceType, class SubtypeLoadCallback, class SubtypeExtraData>
typename ResourceType::UserRamData UserTypeResourceBase<ResourceType, SubtypeLoadCallback, SubtypeExtraData>::GetRamData() const
{
	return (((ResourceType*)GetConstResource())->GetRamData());
}

template<class ResourceType, class SubtypeLoadCallback, class SubtypeExtraData>
typename ResourceType::UserData UserTypeResourceBase<ResourceType, SubtypeLoadCallback, SubtypeExtraData>::GetData() const
{
	return (((ResourceType*)GetConstResource())->GetUserData(this));
}

template<class ResourceType, class SubtypeLoadCallback, class SubtypeExtraData>
SubtypeExtraData& UserTypeResourceBase<ResourceType, SubtypeLoadCallback, SubtypeExtraData>::GetExtraData() const
{
	return (mExtraData);
}



template<class ResourceType>
UserTypeResource<ResourceType>::UserTypeResource()
{
}

template<class ResourceType>
UserTypeResource<ResourceType>::~UserTypeResource()
{
}

template<class ResourceType>
Resource* UserTypeResource<ResourceType>::CreateResource(ResourceManager* pManager, const Lepra::String& pName) const
{
	return (new ResourceType(pManager, pName));
}



template<class ResourceType, class SubtypeExtraType>
UserExtraCreateTypeResource<ResourceType, SubtypeExtraType>::UserExtraCreateTypeResource(SubtypeExtraType pExtraData):
	UserTypeResourceBase(pExtraData)
{
}

template<class ResourceType, class SubtypeExtraType>
Resource* UserExtraCreateTypeResource<ResourceType, SubtypeExtraType>::CreateResource(ResourceManager* pManager, const Lepra::String& pName) const
{
	return (new ResourceType(pManager, pName, Parent::GetExtraData()));
}



template<class RamData> RamData RamResource<RamData>::GetRamData() const
{
	return (mRamData);
}

template<class RamData> void RamResource<RamData>::SetRamDataType(RamData pData)
{
	mRamData = pData;
}

template<class RamData> RamResource<RamData>::RamResource(ResourceManager* pManager, const Lepra::String& pName):
	Resource(pManager, pName),
	mRamData((RamData)0)
{
}

template<class RamData> RamResource<RamData>::~RamResource()
{
}

template<class RamData> void RamResource<RamData>::SetRamData(RamData pData)
{
	delete (mRamData);
	SetRamDataType(pData);
}



template<class RamData, class OptimizedData> OptimizedResource<RamData, OptimizedData>::OptimizedResource(
	ResourceManager* pManager, const Lepra::String& pName):
	RamResource<RamData>(pManager, pName),
	mOptimizedData((OptimizedData)0)
{
}

template<class RamData, class OptimizedData> OptimizedResource<RamData, OptimizedData>::~OptimizedResource()
{
	mOptimizedData = (OptimizedData)0;
}

template<class RamData, class OptimizedData> void OptimizedResource<RamData, OptimizedData>::SetOptimizedData(OptimizedData pData)
{
	mOptimizedData = pData;
}



template<class RamData, class DiversifiedData> DiversifiedResource<RamData, DiversifiedData>::DiversifiedResource(ResourceManager* pManager, const Lepra::String& pName):
	RamResource<RamData>(pManager, pName)
{
}

template<class RamData, class DiversifiedData> DiversifiedResource<RamData, DiversifiedData>::~DiversifiedResource()
{
	// Diviersified table must be entirely free'd and cleared out by child class.
	assert(mUserDiversifiedTable.IsEmpty());
}

template<class RamData, class DiversifiedData> DiversifiedData DiversifiedResource<RamData, DiversifiedData>::GetUserData(const UserResource* pUserResource)
{
	DiversifiedData lInstanceId = (DiversifiedData)0;
	typename UserDataTable::Iterator x = mUserDiversifiedTable.Find(pUserResource);
	if (x != mUserDiversifiedTable.End())
	{
		lInstanceId = *x;
	}
	else
	{
		lInstanceId = CreateDiversifiedData();
		mUserDiversifiedTable.Insert(pUserResource, lInstanceId);
	}
	assert(lInstanceId != 0);
	return (lInstanceId);
}

template<class RamData, class DiversifiedData> void DiversifiedResource<RamData, DiversifiedData>::FreeDiversified(UserResource* pUserResource)
{
	typename UserDataTable::Iterator x = mUserDiversifiedTable.Find(pUserResource);
	if (x != mUserDiversifiedTable.End())
	{
		DiversifiedData lInstanceId = *x;
		mUserDiversifiedTable.Remove(x);
		ReleaseDiversifiedData(lInstanceId);
	}
}



}
