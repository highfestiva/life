
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../ContextResource.h"



namespace Life
{



struct ContextObjectAccountInfo: public ContextObjectInfo
{
	ContextObjectAccountInfo(Cure::ContextManager* pContextManager, Cure::UserResource* pLoadingResource,
		Cure::GameObjectId pInstanceId, Cure::NetworkObjectType pNetworkType,
		Cure::UserAccount::AccountId pAccountId):
		ContextObjectInfo(pContextManager, pLoadingResource, pInstanceId, pNetworkType),
		mAccountId(pAccountId)
	{
	}
	Cure::UserAccount::AccountId mAccountId;
};

typedef Cure::UserExtraCreateTypeResource<ContextObjectInfoResource, ContextObjectAccountInfo> UserContextObjectAccountInfoResource;



}
