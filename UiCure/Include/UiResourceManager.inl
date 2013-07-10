
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



namespace UiCure
{



template<class ResourceType>
UserUiTypeResource<ResourceType>::UserUiTypeResource(GameUiManager* pUiManager):
	UiResource(pUiManager)
{
}

template<class ResourceType>
UserUiTypeResource<ResourceType>::~UserUiTypeResource()
{
}

template<class ResourceType>
Cure::Resource* UserUiTypeResource<ResourceType>::CreateResource(
	Cure::ResourceManager* pManager, const str& pName) const
{
	return (new ResourceType(GetUiManager(), pManager, pName));
}



template<class ResourceType, class SubtypeExtraType>
UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::UserUiExtraTypeResource(GameUiManager* pUiManager, const ExtraType& pExtraData):
	UiResource(pUiManager),
	mExtraData(pExtraData)
{
}

template<class ResourceType, class SubtypeExtraType>
UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::~UserUiExtraTypeResource()
{
}

template<class ResourceType, class SubtypeExtraType>
Cure::Resource* UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::CreateResource(
	Cure::ResourceManager* pManager, const str& pName) const
{
	return (new ResourceType(GetUiManager(), pManager, pName, GetExtraData()));
}

template<class ResourceType, class SubtypeExtraType>
SubtypeExtraType& UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::GetExtraData() const
{
	return (mExtraData);
}

template<class ResourceType, class SubtypeExtraType>
void UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::SetExtraData(const SubtypeExtraType& pExtraData)
{
	mExtraData = pExtraData;
}



}
