
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../consolemanager.h"



namespace life {



class ServerConsoleManager: public ConsoleManager {
	typedef ConsoleManager Parent;
public:
	ServerConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_server_manager,
		cure::RuntimeVariableScope* variable_scope, InteractiveConsoleLogListener* console_logger,
		ConsolePrompt* console_prompt);
	virtual ~ServerConsoleManager();

private:
	enum CommandServer {
		kCommandQuit = kCommandCountCommon,

		kCommandLoginEnable,
		kCommandLoginDisable,

		kCommandListUsers,
		kCommandBuild,

		kCommandBroadcastChatMessage,
		kCommandSendPrivateChatMessage,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned index) const;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector);

	static const CommandPair command_id_list_[];
	logclass();
};



}
