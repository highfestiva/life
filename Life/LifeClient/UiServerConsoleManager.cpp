
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "UiServerConsoleManager.h"
#include "../../Lepra/Include/LogListener.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiLepra/Include/UiInput.h"
#include "UiConsole.h"



namespace Life
{



UiServerConsoleManager::UiServerConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea,
	InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt):
	ServerConsoleManager(pResourceManager, pGameManager, pVariableScope, pConsoleLogger, pConsolePrompt)
{
	mUiConsole = new UiConsole(this, pUiManager, pArea);
}

UiServerConsoleManager::~UiServerConsoleManager()
{
	Join();
	delete GetConsoleLogger();
	SetConsoleLogger(0);
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



bool UiServerConsoleManager::ToggleVisible()
{
	const bool lConsoleActive = mUiConsole->ToggleVisible();
	mUiConsole->GetUiManager()->GetInputManager()->SetCursorVisible(lConsoleActive);
	return lConsoleActive;
}

UiConsole* UiServerConsoleManager::GetUiConsole() const
{
	return (mUiConsole);
}



loginstance(CONSOLE, UiServerConsoleManager);



}
