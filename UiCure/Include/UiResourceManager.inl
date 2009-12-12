
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
	Cure::ResourceManager* pManager, const Lepra::String& pName) const
{
	return (new ResourceType(GetUiManager(), pManager, pName));
}



}
