
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/consolemanager.h"
#include "../lepra/include/file.h"
#include "../lepra/include/unordered.h"
#include "life.h"



namespace lepra {
class LogListener;
}
namespace cure {
class GameManager;
class ResourceManager;
}



namespace life {



class ConsoleManager: public cure::ConsoleManager {
	typedef cure::ConsoleManager Parent;
public:
	ConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
		cure::RuntimeVariableScope* variable_scope, InteractiveConsoleLogListener* console_logger,
		ConsolePrompt* console_prompt);
	virtual ~ConsoleManager();

	virtual void InitCommands();

	void SetSecurityLevel(int level);
	int GetSecurityLevel() const;

	cure::GameManager* GetGameManager() const;
	void SetGameManager(cure::GameManager* game_manager);

	void OnKey(const str& key_name);

protected:
	enum CommandCommon {
		kCommandNotImplemented = -1,

		kCommandAlias,
		kCommandAliasValue,
		kCommandBindKey,
		kCommandEcho,
		kCommandExecuteFile,
		kCommandExecuteVariable,
		kCommandFork,
		kCommandNop,
		kCommandListActiveResources,
		kCommandPush,
		kCommandRepeat,
		kCommandSaveSystemConfigFile,
		kCommandSaveApplicationConfigFile,
		kCommandSetDefaultConfig,
		kCommandSetStdoutLogLevel,
		kCommandSetSubsystemLogLevel,
		kCommandSleep,
		kCommandWaitLoaded,

		kCommandDumpPerformanceInfo,
		kCommandClearPerformanceInfo,
		kCommandShellExecute,
		kCommandShowSystemInfo,
		kCommandShowGameInfo,
		kCommandHangGame,
		kCommandUnhangGame,
		kCommandDebugBreak,
		kCommandHelp,

		kCommandCountCommon
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned index) const;
	int TranslateCommand(const HashedString& command) const;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector);

	virtual bool SaveApplicationConfigFile(File* file, const str& user_config);

	cure::ResourceManager* GetResourceManager() const;

protected:
	str LoadUserConfig(File* file);
	bool SaveSystemConfigFile(int scope_skip_count, File* file, const str& user_config);
	bool SaveConfigFile(File* file, const str& prefix, std::list<str>& variable_list, const str& user_config);
	static str GetQuoted(const str& s);

	static const CommandPair command_id_list_[];
	typedef std::unordered_map<HashedString, str, HashedStringHasher> AliasMap;
	typedef AliasMap KeyMap;

	int security_level_;
	cure::GameManager* game_manager_;
	cure::ResourceManager* resource_manager_;
	AliasMap alias_map_;
	KeyMap key_map_;
	LogListener* logger_;
	logclass();
};



}
