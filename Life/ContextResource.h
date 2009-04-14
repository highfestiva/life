
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../Cure/Include/Cure.h"
#include "../Cure/Include/ResourceManager.h"



namespace Cure
{
class ContextManager;
class UserResource;
}



namespace Life
{



struct ContextObjectInfo
{
	ContextObjectInfo(Cure::ContextManager* pContextManager, Cure::UserResource* pLoadingResource,
		Cure::GameObjectId pInstanceId, Cure::NetworkObjectType pNetworkType):
		mContextManager(pContextManager),
		mLoadingResource(pLoadingResource),
		mInstanceId(pInstanceId),
		mNetworkType(pNetworkType)
	{
	}
	Cure::ContextManager* mContextManager;
	Cure::UserResource* mLoadingResource;
	Cure::GameObjectId mInstanceId;
	Cure::NetworkObjectType mNetworkType;
};



class ContextObjectInfoResource: public Cure::ContextObjectResource
{
	typedef Cure::ContextObjectResource Parent;
public:
	ContextObjectInfoResource(Cure::ResourceManager* pManager, const Lepra::String& pName, ContextObjectInfo pInfo);
	bool Load();

protected:
	ContextObjectInfo mInfo;
};



typedef Cure::UserExtraCreateTypeResource<ContextObjectInfoResource, ContextObjectInfo> UserContextObjectInfoResource;



}
