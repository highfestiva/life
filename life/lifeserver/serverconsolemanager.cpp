
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/number.h"
#include "../../lepra/include/systemmanager.h"
#include "gameservermanager.h"
#include "serverconsolemanager.h"



namespace life {



// Must lie before ServerConsoleManager to compile.
const ConsoleManager::CommandPair ServerConsoleManager::command_id_list_[] =
{
	{"quit", kCommandQuit},

	// Info/debug stuff.
	{"list-users", kCommandListUsers},
	{"build", kCommandBuild},

	// Communication.
	{"broadcast-msg", kCommandBroadcastChatMessage},
	{"priv-msg", kCommandSendPrivateChatMessage},
};



ServerConsoleManager::ServerConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	cure::RuntimeVariableScope* variable_scope, InteractiveConsoleLogListener* console_logger,
	ConsolePrompt* console_prompt):
	ConsoleManager(resource_manager, game_manager, variable_scope, console_logger, console_prompt) {
	SetSecurityLevel(1);
	InitCommands();
}

ServerConsoleManager::~ServerConsoleManager() {
}



unsigned ServerConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const ServerConsoleManager::CommandPair& ServerConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	const unsigned local_index = index-Parent::GetCommandCount();
	return command_id_list_[local_index];
}

int ServerConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;

		CommandServer _command = (CommandServer)TranslateCommand(command);
		switch (_command) {
			case kCommandQuit: {
				int client_count = ((GameServerManager*)GetGameManager())->GetLoggedInClientCount();
				if (client_count > 0) {
					if (parameter_vector.size() == 1 && parameter_vector[0] == "!") {
						log_.Warningf("Forced termination with %i logged-in clients.", client_count);
						SystemManager::AddQuitRequest(+1);
					} else {
						log_.Warningf("Not allowed to terminate (%i logged-in clients). Use '!' to override.", client_count);
					}
				} else {
					log_.Headline("Terminating due to user command.");
					SystemManager::AddQuitRequest(+1);
				}
			} break;
			case kCommandBroadcastChatMessage: {
				if (parameter_vector.size() == 1) {
					str message = parameter_vector[0];
					if (((GameServerManager*)GetGameManager())->BroadcastChatMessage(message)) {
						log_.Infof("BROADCAST CHAT: %s", parameter_vector[0].c_str());
					} else {
						log_.Error("Could not broadcast chat message!");
						result = 1;
					}
				} else {
					log_.Warningf("usage: %s <message>", command.c_str());
					result = 1;
				}
			} break;
			case kCommandSendPrivateChatMessage: {
				if (parameter_vector.size() == 2) {
					str client_user_name = parameter_vector[0];
					str message = parameter_vector[1];
					if (((GameServerManager*)GetGameManager())->SendChatMessage(client_user_name, message)) {
						log_.Infof("PRIVATE CHAT ServerAdmin->%s: %s", parameter_vector[0].c_str(), parameter_vector[1].c_str());
					} else {
						log_.Error("Could not send private chat message!");
						result = 1;
					}
				} else {
					log_.Warningf("usage: %s <user> <message>", command.c_str());
					result = 1;
				}
			} break;
			case kCommandListUsers: {
				strutil::strvec user_name_list;
				user_name_list = ((GameServerManager*)GetGameManager())->ListUsers();
				log_.Info("Listing logged on users:");
				for (size_t x = 0; x < user_name_list.size(); ++x) {
					log_.Info("\t\"" + user_name_list[x] + "\"");
				}
				log_.Infof("A total of %u users logged in.", (unsigned)user_name_list.size());
			} break;
			case kCommandBuild: {
				if (parameter_vector.size() == 1) {
					((GameServerManager*)GetGameManager())->Build(parameter_vector[0]);
				} else {
					log_.Warningf("usage: %s <what>", command.c_str());
					result = 1;
				}
			} break;
			default: {
				result = -1;
			} break;
		}
	}
	return (result);
}

loginstance(kConsole, ServerConsoleManager);



}
