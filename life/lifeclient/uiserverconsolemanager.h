
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lifeserver/serverconsolemanager.h"

namespace lepra {
class PixelRect;
};
namespace UiCure {
class GameUiManager;
}



namespace life {



class UiConsole;



class UiServerConsoleManager: public ServerConsoleManager {
	typedef ServerConsoleManager Parent;
public:
	UiServerConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
		UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area,
		InteractiveConsoleLogListener* console_logger, ConsolePrompt* console_prompt);
	virtual ~UiServerConsoleManager();

	virtual bool Start();
	virtual void Join();

	bool ToggleVisible();
	UiConsole* GetUiConsole() const;

private:
	UiConsole* ui_console_;
	logclass();
};



}
