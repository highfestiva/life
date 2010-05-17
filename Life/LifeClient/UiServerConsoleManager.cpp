
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "UiServerConsoleManager.h"
#include "UiConsole.h"



namespace Life
{



UiServerConsoleManager::UiServerConsoleManager(Cure::GameManager* pGameManager, UiCure::GameUiManager* pUiManager,
	Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea,
	InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt):
	ServerConsoleManager(pGameManager, pVariableScope, pConsoleLogger, pConsolePrompt)
{
	mUiConsole = new UiConsole(this, pUiManager, pArea);
}

UiServerConsoleManager::~UiServerConsoleManager()
{
	delete (mUiConsole);
	mUiConsole = 0;
}



bool UiServerConsoleManager::Start()
{
	bool lOk = Parent::Start();
	mUiConsole->SetColor(Color(60, 10, 10, 255));
	mUiConsole->Open();
	return (lOk);
}

void UiServerConsoleManager::Join()
{
	mUiConsole->Close();
	Parent::Join();
}



UiConsole* UiServerConsoleManager::GetUiConsole() const
{
	return (mUiConsole);
}



LOG_CLASS_DEFINE(CONSOLE, UiServerConsoleManager);



}
