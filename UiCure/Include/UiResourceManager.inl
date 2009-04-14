
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



namespace UiCure
{



template<class ResourceType, class SubtypeExtraType>
UserUiTypeResource<ResourceType, SubtypeExtraType>::UserUiTypeResource(GameUiManager* pUiManager, const SubtypeExtraType& pExtraData):
	Cure::UserTypeResourceBase<ResourceType, ParentTypeLoadCallback, SubtypeExtraType>(pExtraData),
	mUiManager(pUiManager)
{
}

template<class ResourceType, class SubtypeExtraType>
UserUiTypeResource<ResourceType, SubtypeExtraType>::~UserUiTypeResource()
{
	mUiManager = 0;
}

template<class ResourceType, class SubtypeExtraType>
Cure::Resource* UserUiTypeResource<ResourceType, SubtypeExtraType>::CreateResource(
	Cure::ResourceManager* pManager, const Lepra::String& pName) const
{
	return (new ResourceType(mUiManager, pManager, pName));
}



}
