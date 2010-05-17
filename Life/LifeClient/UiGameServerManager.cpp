
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "UiGameServerManager.h"
#include "UiServerConsoleManager.h"
#include "UiConsole.h"
#include "Vehicle.h"



namespace Life
{



UiGameServerManager::UiGameServerManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, const PixelRect& pArea):
	GameServerManager(pVariableScope, pResourceManager),
	mUiManager(pUiManager),
	mRenderArea(pArea),
	mOptions(pVariableScope, 0),
	mConsoleActive(false)
{
}

UiGameServerManager::~UiGameServerManager()
{
	mUiManager = 0;
}

void UiGameServerManager::SetRenderArea(const PixelRect& pRenderArea)
{
	mRenderArea = pRenderArea;
	((UiServerConsoleManager*)GetConsoleManager())->GetUiConsole()->SetRenderArea(pRenderArea);
}



void UiGameServerManager::StartConsole(InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt)
{
	SetConsoleManager(new UiServerConsoleManager(this, mUiManager, GetVariableScope(), mRenderArea,
		pConsoleLogger, pConsolePrompt));
	Parent::StartConsole(0, 0);
}

void UiGameServerManager::ToggleConsole()
{
	mConsoleActive = ((UiServerConsoleManager*)GetConsoleManager())->GetUiConsole()->ToggleVisible();
}



bool UiGameServerManager::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	mOptions.UpdateInput(pKeyCode, true);
	if (mOptions.GetConsoleToggle() >= 0.5f)
	{
		mOptions.ResetToggles();
		ToggleConsole();
		return (true);	// This key ends here.
	}
	return (false);
}

bool UiGameServerManager::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	mOptions.UpdateInput(pKeyCode, false);
	return (false);
}

void UiGameServerManager::OnInput(UiLepra::InputElement* pElement)
{
	mOptions.UpdateInput(pElement);
	if (mOptions.GetConsoleToggle() >= 0.5f)
	{
		mOptions.ResetToggles();
		ToggleConsole();
	}
}



void UiGameServerManager::TickInput()
{
	((UiServerConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	Parent::TickInput();
}



Cure::ContextObject* UiGameServerManager::CreateContextObject(const str& pClassId) const
{
	return (new Vehicle(GetResourceManager(), pClassId, mUiManager));
}



LOG_CLASS_DEFINE(GAME, UiGameServerManager);



}
