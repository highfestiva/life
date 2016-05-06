
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "uiserverconsolemanager.h"
#include "../../lepra/include/loglistener.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uilepra/include/uiinput.h"
#include "uiconsole.h"



namespace life {



UiServerConsoleManager::UiServerConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area,
	InteractiveConsoleLogListener* console_logger, ConsolePrompt* console_prompt):
	ServerConsoleManager(resource_manager, game_manager, variable_scope, console_logger, console_prompt) {
	ui_console_ = new UiConsole(this, ui_manager, area);
}

UiServerConsoleManager::~UiServerConsoleManager() {
	Join();
	delete GetConsoleLogger();
	SetConsoleLogger(0);
	delete (ui_console_);
	ui_console_ = 0;
}



bool UiServerConsoleManager::Start() {
	bool ok = Parent::Start();
	ui_console_->SetColor(Color(60, 10, 10, 255));
	ui_console_->Open();
	return (ok);
}

void UiServerConsoleManager::Join() {
	ui_console_->Close();
	Parent::Join();
}



bool UiServerConsoleManager::ToggleVisible() {
	const bool console_active = ui_console_->ToggleVisible();
	ui_console_->GetUiManager()->GetInputManager()->SetCursorVisible(console_active);
	return console_active;
}

UiConsole* UiServerConsoleManager::GetUiConsole() const {
	return (ui_console_);
}



loginstance(kConsole, UiServerConsoleManager);



}
