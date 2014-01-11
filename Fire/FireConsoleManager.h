
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
#include "Fire.h"



namespace Fire
{



class FireConsoleManager: public Life::ClientConsoleManager
{
	typedef Life::ClientConsoleManager Parent;
public:
	FireConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~FireConsoleManager();
	virtual bool Start();

protected:
	enum CommandClient
	{
		COMMAND_PREV_LEVEL = Parent::COMMAND_COUNT_LIFE_CLIENT,
		COMMAND_NEXT_LEVEL,
		COMMAND_SET_LEVEL_INDEX,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	static const CommandPair mCommandIdList[];
	LOG_CLASS_DECLARE();
};



}
