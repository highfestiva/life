
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../ConsoleManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"



namespace Life
{



class UiConsole;



class ClientConsoleManager: public ConsoleManager
{
	typedef ConsoleManager Parent;
public:
	ClientConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~ClientConsoleManager();

	virtual bool Start();
	virtual void Join();

	bool ToggleVisible();
	UiConsole* GetUiConsole() const;

	int FilterExecuteCommand(const str& pCommand);

protected:
	enum CommandClient
	{
		COMMAND_QUIT = COMMAND_COUNT_COMMON,
		COMMAND_BYE,
		COMMAND_START_LOGIN,
		COMMAND_WAIT_LOGIN,
		COMMAND_LOGOUT,
		COMMAND_START_RESET_UI,
		COMMAND_WAIT_RESET_UI,
		COMMAND_ADD_PLAYER,

		COMMAND_COUNT_LIFE_CLIENT
	};

	bool SaveApplicationConfigFile(File* pFile, const wstr& pUserConfig);

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	static const CommandPair mCommandIdList[];
	UiConsole* mUiConsole;
	LOG_CLASS_DECLARE();
};



}
