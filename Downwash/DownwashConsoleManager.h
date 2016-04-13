
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
#include "Downwash.h"



namespace Downwash
{



class DownwashConsoleManager: public Life::ClientConsoleManager
{
	typedef Life::ClientConsoleManager Parent;
public:
	DownwashConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~DownwashConsoleManager();
	virtual bool Start();

protected:
	enum CommandClient
	{
		COMMAND_SET_AVATAR = Parent::COMMAND_COUNT_LIFE_CLIENT,
		COMMAND_PREV_LEVEL,
		COMMAND_NEXT_LEVEL,
		COMMAND_SET_LEVEL_INDEX,
		COMMAND_DIE,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const HashedString& pCommand, const strutil::strvec& pParameterVector);

	static const CommandPair mCommandIdList[];
	logclass();
};



}
