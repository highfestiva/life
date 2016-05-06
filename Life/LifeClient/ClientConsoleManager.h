
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../consolemanager.h"



namespace lepra {
class PixelRect;
};
namespace UiCure {
class GameUiManager;
}



namespace life {



class UiConsole;



class ClientConsoleManager: public ConsoleManager {
	typedef ConsoleManager Parent;
public:
	ClientConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
		UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area);
	virtual ~ClientConsoleManager();

	virtual bool Start();
	virtual void Join();

	bool ToggleVisible();
	UiConsole* GetUiConsole() const;

	int FilterExecuteCommand(const str& command);

protected:
	enum CommandClient {
		kCommandQuit = kCommandCountCommon,
		kCommandBye,
		kCommandZombie,
		kCommandEchoMsgbox,
		kCommandStartLogin,
		kCommandWaitLogin,
		kCommandLogout,
		kCommandStartResetUi,
		kCommandWaitResetUi,
		kCommandAddPlayer,
		kCommandSetMeshVisible,

		kCommandCountLifeClient
	};

	bool SaveApplicationConfigFile(File* file, const str& user_config);

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned index) const;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector);

	void HeadlessTick();

	static const CommandPair command_id_list_[];
	UiConsole* ui_console_;
	bool was_cursor_visible_;
	logclass();
};



}
