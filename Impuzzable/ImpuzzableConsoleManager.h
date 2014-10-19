
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
#include "Impuzzable.h"



namespace Impuzzable
{



class ImpuzzableConsoleManager: public Life::ClientConsoleManager
{
	typedef Life::ClientConsoleManager Parent;
public:
	ImpuzzableConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~ImpuzzableConsoleManager();
	virtual bool Start();

protected:
	enum CommandClient
	{
		COMMAND_STEP_LEVEL = Parent::COMMAND_COUNT_LIFE_CLIENT,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	static const CommandPair mCommandIdList[];
	logclass();
};



}
