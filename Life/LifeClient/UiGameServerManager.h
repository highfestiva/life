
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../LifeServer/GameServerManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiLepra/Include/UiInput.h"
#include "OptionsManager.h"



namespace Life
{



class UiGameServerManager: public GameServerManager
{
	typedef GameServerManager Parent;
public:
	UiGameServerManager(const Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, const PixelRect& pArea);
	virtual ~UiGameServerManager();
	void SetRenderArea(const PixelRect& pRenderArea);

	virtual void StartConsole(InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt);
	void ToggleConsole();

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnInput(UiLepra::InputElement* pElement);

private:
	virtual void TickInput();

	virtual void StoreMovement(int pClientFrameIndex, Cure::MessageObjectMovement* pMovement);

	UiCure::GameUiManager* mUiManager;
	PixelRect mRenderArea;
	Options::OptionsManager mOptions;
	bool mConsoleActive;

	LOG_CLASS_DECLARE();
};



}
