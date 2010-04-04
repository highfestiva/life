
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameManager.h"
#include "Elevator.h"



namespace Life
{



GameManager::GameManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Cure::GameManager(pVariableScope, pResourceManager)
{
}

GameManager::~GameManager()
{
}



Cure::ContextObject* GameManager::CreateTriggerHandler(const str& pType) const
{
	if (pType == _T("elevator"))
	{
		return (new Elevator);
	}
	assert(false);
	return (0);
}



}
