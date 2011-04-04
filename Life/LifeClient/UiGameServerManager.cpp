
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "UiGameServerManager.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "UiServerConsoleManager.h"
#include "UiConsole.h"



namespace Life
{



UiGameServerManager::UiGameServerManager(const Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, const PixelRect& pArea):
	GameServerManager(pTime, pVariableScope, pResourceManager),
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
	mConsoleActive = ((UiServerConsoleManager*)GetConsoleManager())->GetUiConsole()->ToggleVisible();
}



bool UiGameServerManager::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	mOptions.RefreshConfiguration();

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
	mOptions.RefreshConfiguration();

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
	// TRICKY: must be of UI object to not clash with client slaves
	// that are running in the same process.
	UiCure::CppContextObject* lObject = new UiCure::CppContextObject(GetResourceManager(), pClassId, mUiManager);
	lObject->EnableUi(false);
	return (lObject);
}



LOG_CLASS_DEFINE(GAME, UiGameServerManager);



}
