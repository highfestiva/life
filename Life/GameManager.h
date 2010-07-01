
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Cure/Include/GameManager.h"
#include "Life.h"



namespace Cure
{
class ContextObject;
}



namespace Life
{



class GameManager: public Cure::GameManager
{
	typedef Cure::GameManager Parent;
public:
	GameManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager);
	virtual ~GameManager();

	virtual Cure::ContextObject* CreateTriggerHandler(const str& pType) const;
};



}
