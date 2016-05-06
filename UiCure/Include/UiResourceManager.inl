
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



namespace UiCure {



template<class ResourceType>
UserUiTypeResource<ResourceType>::UserUiTypeResource(GameUiManager* ui_manager):
	UiResource(ui_manager) {
}

template<class ResourceType>
UserUiTypeResource<ResourceType>::~UserUiTypeResource() {
}

template<class ResourceType>
cure::Resource* UserUiTypeResource<ResourceType>::CreateResource(
	cure::ResourceManager* manager, const str& name) const {
	return (new ResourceType(GetUiManager(), manager, name));
}



template<class ResourceType, class SubtypeExtraType>
UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::UserUiExtraTypeResource(GameUiManager* ui_manager, const ExtraType& extra_data):
	UiResource(ui_manager),
	extra_data_(extra_data) {
}

template<class ResourceType, class SubtypeExtraType>
UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::~UserUiExtraTypeResource() {
}

template<class ResourceType, class SubtypeExtraType>
cure::Resource* UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::CreateResource(
	cure::ResourceManager* manager, const str& name) const {
	return (new ResourceType(GetUiManager(), manager, name, GetExtraData()));
}

template<class ResourceType, class SubtypeExtraType>
SubtypeExtraType& UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::GetExtraData() const {
	return (extra_data_);
}

template<class ResourceType, class SubtypeExtraType>
void UserUiExtraTypeResource<ResourceType, SubtypeExtraType>::SetExtraData(const SubtypeExtraType& extra_data) {
	extra_data_ = extra_data;
}



}
