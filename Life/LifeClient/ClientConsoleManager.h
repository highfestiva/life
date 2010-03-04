
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../ConsoleManager.h"



namespace UiTbc
{
class Component;
class TextArea;
class TextField;
}
namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class ClientConsoleManager: public ConsoleManager
{
	typedef ConsoleManager Parent;
public:
	ClientConsoleManager(Cure::GameManager* pGameManager, UiCure::GameUiManager* pUiManager,
		Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~ClientConsoleManager();

	bool Start();
	void Join();

	void SetRenderArea(const PixelRect& pRenderArea);
	bool Toggle();
	void Tick();

protected:
	bool SaveApplicationConfigFile(File* pFile, const wstr& pUserConfig);

private:
	void InitGraphics();
	void CloseGraphics();
	void OnConsoleChange();
	void PrintHelp();

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
	};

	unsigned GetCommandCount() const;
	const CommandPair& GetCommand(unsigned pIndex) const;
	int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	UiCure::GameUiManager* mUiManager;
	PixelRect mArea;
	UiTbc::Component* mConsoleComponent;
	UiTbc::TextArea* mConsoleOutput;
	UiTbc::TextField* mConsoleInput;
	bool mIsConsoleActive;
	bool mIsFirstConsoleUse;
	double mConsoleTargetPosition;

	static const CommandPair mCommandIdList[];
	LOG_CLASS_DECLARE();
};



}
