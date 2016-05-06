
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "uigameservermanager.h"
#include "../../uicure/include/uicppcontextobject.h"
#include "gameclientmasterticker.h"
#include "uiserverconsolemanager.h"
#include "uiconsole.h"



namespace life {



UiGameServerManager::UiGameServerManager(const cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager, const PixelRect& area):
	Parent(time, variable_scope, resource_manager),
	ui_manager_(ui_manager),
	render_area_(area),
	options_(variable_scope, 0),
	console_active_(false) {
	GetContext()->GetObjectTable();
}

UiGameServerManager::~UiGameServerManager() {
	ui_manager_ = 0;
}

void UiGameServerManager::SetRenderArea(const PixelRect& render_area) {
	render_area_ = render_area;
	if (GetConsoleManager()) {
		((UiServerConsoleManager*)GetConsoleManager())->GetUiConsole()->SetRenderArea(render_area);
	}
}



void UiGameServerManager::StartConsole(InteractiveConsoleLogListener* console_logger, ConsolePrompt* console_prompt) {
	SetConsoleManager(new UiServerConsoleManager(GetResourceManager(), this, ui_manager_, GetVariableScope(), render_area_,
		console_logger, console_prompt));
	Parent::StartConsole(0, 0);
}

void UiGameServerManager::ToggleConsole() {
	console_active_ = ((UiServerConsoleManager*)GetConsoleManager())->ToggleVisible();
}



bool UiGameServerManager::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	options_.RefreshConfiguration();

	options_.UpdateInput(key_code, true);
	if (options_.IsToggleConsole()) {
		options_.ResetToggles();
		ToggleConsole();
		return (true);	// This key ends here.
	}
	return (false);
}

bool UiGameServerManager::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	options_.UpdateInput(key_code, false);
	return (false);
}

void UiGameServerManager::OnInput(uilepra::InputElement* element) {
	options_.RefreshConfiguration();

	options_.UpdateInput(element);
	if (options_.IsToggleConsole()) {
		options_.ResetToggles();
		ToggleConsole();
	}
}



void UiGameServerManager::TickInput() {
	((UiServerConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	Parent::TickInput();
}



void UiGameServerManager::StoreMovement(int client_frame_index, cure::MessageObjectMovement* movement) {
	const GameClientMasterTicker* ticker = (const GameClientMasterTicker*)GetTicker();
	if (!ticker->IsLocalObject(movement->GetObjectId())) {
		Parent::StoreMovement(client_frame_index, movement);
	}
}



loginstance(kGame, UiGameServerManager);



}
