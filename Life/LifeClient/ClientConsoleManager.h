
// Author: Jonas Bystr�m
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../../Lepra/Include/Graphics2D.h"
#include "../../UiTbc/Include/GUI/UiTextField.h"
#include "../ConsoleManager.h"



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
		Cure::RuntimeVariableScope* pVariableScope, const Lepra::PixelRect& pArea);
	virtual ~ClientConsoleManager();

	bool Start();
	void Join();

	void SetRenderArea(const Lepra::PixelRect& pRenderArea);
	bool Toggle();
	void Tick();

protected:
	bool SaveApplicationConfigFile(Lepra::File* pFile, const Lepra::UnicodeString& pUserConfig);

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
	int OnCommand(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector);

	UiCure::GameUiManager* mUiManager;
	Lepra::PixelRect mArea;
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
