
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
#include "HoverTank.h"



namespace HoverTank
{



class HoverTankConsoleManager: public Life::ClientConsoleManager
{
	typedef Life::ClientConsoleManager Parent;
public:
	HoverTankConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~HoverTankConsoleManager();

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
