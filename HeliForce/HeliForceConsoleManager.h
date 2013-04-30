
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
#include "HeliForce.h"



namespace HeliForce
{



class HeliForceConsoleManager: public Life::ClientConsoleManager
{
	typedef Life::ClientConsoleManager Parent;
public:
	HeliForceConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~HeliForceConsoleManager();

protected:
	enum CommandClient
	{
		COMMAND_SET_AVATAR = Parent::COMMAND_COUNT_LIFE_CLIENT,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	static const CommandPair mCommandIdList[];
	LOG_CLASS_DECLARE();
};



}
