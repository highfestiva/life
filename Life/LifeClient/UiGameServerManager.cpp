
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "UiGameServerManager.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "GameClientMasterTicker.h"
#include "UiServerConsoleManager.h"
#include "UiConsole.h"



namespace Life
{



UiGameServerManager::UiGameServerManager(const Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, const PixelRect& pArea):
	Parent(pTime, pVariableScope, pResourceManager),
	mUiManager(pUiManager),
	mRenderArea(pArea),
	mOptions(pVariableScope, 0),
	mConsoleActive(false)
{
	GetContext()->GetObjectTable();
}

UiGameServerManager::~UiGameServerManager()
{
	mUiManager = 0;
}

void UiGameServerManager::SetRenderArea(const PixelRect& pRenderArea)
{
	mRenderArea = pRenderArea;
	if (GetConsoleManager())
	{
		((UiServerConsoleManager*)GetConsoleManager())->GetUiConsole()->SetRenderArea(pRenderArea);
	}
}



void UiGameServerManager::StartConsole(InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt)
{
	SetConsoleManager(new UiServerConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea,
		pConsoleLogger, pConsolePrompt));
	Parent::StartConsole(0, 0);
}

void UiGameServerManager::ToggleConsole()
{
	mConsoleActive = ((UiServerConsoleManager*)GetConsoleManager())->ToggleVisible();
}



bool UiGameServerManager::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	mOptions.RefreshConfiguration();

	mOptions.UpdateInput(pKeyCode, true);
	if (mOptions.IsToggleConsole())
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
	mOptions.RefreshConfiguration();

	mOptions.UpdateInput(pElement);
	if (mOptions.IsToggleConsole())
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



void UiGameServerManager::StoreMovement(int pClientFrameIndex, Cure::MessageObjectMovement* pMovement)
{
	const GameClientMasterTicker* lTicker = (const GameClientMasterTicker*)GetTicker();
	if (!lTicker->IsLocalObject(pMovement->GetObjectId()))
	{
		Parent::StoreMovement(pClientFrameIndex, pMovement);
	}
}



LOG_CLASS_DEFINE(GAME, UiGameServerManager);



}
