
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lifeserver/gameservermanager.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uilepra/include/uiinput.h"
#include "optionsmanager.h"



namespace life {



class UiGameServerManager: public GameServerManager {
	typedef GameServerManager Parent;
public:
	UiGameServerManager(const cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
		cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager, const PixelRect& area);
	virtual ~UiGameServerManager();
	void SetRenderArea(const PixelRect& render_area);

	virtual void StartConsole(InteractiveConsoleLogListener* console_logger, ConsolePrompt* console_prompt);
	void ToggleConsole();

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
	virtual void OnInput(uilepra::InputElement* element);

private:
	virtual void TickInput();

	virtual void StoreMovement(int client_frame_index, cure::MessageObjectMovement* movement);

	UiCure::GameUiManager* ui_manager_;
	PixelRect render_area_;
	options::OptionsManager options_;
	bool console_active_;

	logclass();
};



}
