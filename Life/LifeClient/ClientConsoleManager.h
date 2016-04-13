
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../ConsoleManager.h"



namespace Lepra
{
class PixelRect;
};
namespace UiCure
{
class GameUiManager;
}



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
		COMMAND_ZOMBIE,
		COMMAND_ECHO_MSGBOX,
		COMMAND_START_LOGIN,
		COMMAND_WAIT_LOGIN,
		COMMAND_LOGOUT,
		COMMAND_START_RESET_UI,
		COMMAND_WAIT_RESET_UI,
		COMMAND_ADD_PLAYER,
		COMMAND_SET_MESH_VISIBLE,

		COMMAND_COUNT_LIFE_CLIENT
	};

	bool SaveApplicationConfigFile(File* pFile, const wstr& pUserConfig);

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const HashedString& pCommand, const strutil::strvec& pParameterVector);

	void HeadlessTick();

	static const CommandPair mCommandIdList[];
	UiConsole* mUiConsole;
	bool mWasCursorVisible;
	logclass();
};



}
