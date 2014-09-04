
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
#include "Push.h"



namespace Push
{



class PushConsoleManager: public Life::ClientConsoleManager
{
	typedef Life::ClientConsoleManager Parent;
public:
	PushConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~PushConsoleManager();

protected:
	enum CommandClient
	{
		COMMAND_SET_AVATAR = Parent::COMMAND_COUNT_LIFE_CLIENT,
		COMMAND_SET_AVATAR_ENGINE_POWER,
#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS)
		COMMAND_BUILD_DATA,
#endif // Debug & Windows
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	static const CommandPair mCommandIdList[];
	logclass();
};



}
