
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../lepra/include/consolecommandmanager.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/memberthread.h"
#include "../../lepra/include/string.h"
#include "../include/cure.h"



namespace lepra {
class InteractiveConsoleLogListener;
}



namespace cure {



class RuntimeVariableScope;



class ConsoleManager {
public:
	ConsoleManager(RuntimeVariableScope* variable_scope, InteractiveConsoleLogListener* console_logger,
		ConsolePrompt* console_prompt);
	virtual ~ConsoleManager();
	void SetConsoleLogger(InteractiveConsoleLogListener* logger);

	virtual bool Start();
	virtual void Join();

	void PushYieldCommand(const str& command);
	int ExecuteCommand(const str& command);
	int ExecuteYieldCommand();

	ConsoleCommandManager* GetConsoleCommandManager() const;

	virtual LogDecorator& GetLogger() const;

	void AddFork(Thread* thread);
	void RemoveFork(Thread* thread);

	InteractiveConsoleLogListener* GetConsoleLogger() const;
	ConsolePrompt* GetConsolePrompt() const;
	RuntimeVariableScope* GetVariableScope() const;

protected:
	struct CommandPair {
		const char* command_name_;
		int command_id_;
	};
	typedef std::unordered_map<HashedString, int, HashedStringHasher> CommandLookupMap;
	typedef std::list<Thread*> ForkList;

	bool ForkExecuteCommand(const str& command);

	virtual void InitCommands();
	std::list<str> GetCommandList() const;
	virtual int TranslateCommand(const HashedString& command) const;
	void PrintCommandList(const std::list<str>& command_list);

	void AddCommands();
	void ConsoleThreadEntry();
	int OnCommandLocal(const str& command, const strutil::strvec& parameter_vector);
	void OnCommandError(const str& command, const strutil::strvec& parameter_vector, int result);

	virtual unsigned GetCommandCount() const = 0;
	virtual const CommandPair& GetCommand(unsigned index) const = 0;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) = 0;

	RuntimeVariableScope* variable_scope_;
	InteractiveConsoleLogListener* console_logger_;
	ConsolePrompt* console_prompt_;
	ConsoleCommandManager* console_command_manager_;
	MemberThread<ConsoleManager>* console_thread_;
	CommandLookupMap command_lookup_;
	Lock lock_;
	std::list<str> yield_command_list_;
	ForkList fork_list_;
	bool history_silent_until_next_execute_;

	logclass();
};



}
