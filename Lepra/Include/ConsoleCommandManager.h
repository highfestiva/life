
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// This is a high-level functionality inside a low-level class (however proven well-working).
//
// The ConsoleCommandManager class contains functionality for completion (normally used
// with the TAB key in a console), execution and history (normally UP/DOWN keys).
// Commands can be separated with a semi-colon, and given parameters.
//
// Example usage for adding and executing commands:
//
// ConsoleCommandManager lConsoleManager(this, &MyClass::OnUnknownCommand);
// lConsoleManager.AddCommand("ls", &MyClass::OnCommand);
// lConsoleManager.AddCommand("ps", &MyClass::OnCommand);
// lConsoleManager.Execute("ls -R; ps Afwl");



#pragma once



#include "unordered.h"
#include <list>
#include <vector>
#include "string.h"



namespace lepra {



class CommandCompleter {
public:
	CommandCompleter();
	virtual ~CommandCompleter();
	virtual std::list<str> CompleteCommand(const str& partial_command) const = 0;
};



class ConsoleCommandExecutor {
public:
	ConsoleCommandExecutor();
	virtual ~ConsoleCommandExecutor();

	virtual int Execute(const str& command, const strutil::strvec& parameter_list) = 0;
	virtual void OnExecutionError(const str& command, const strutil::strvec& parameter_list, int result) = 0;
};

template<class _Base> class ConsoleExecutor: public ConsoleCommandExecutor {
public:
	typedef int (_Base::*CommandCallback)(const str&, const strutil::strvec&);
	typedef void (_Base::*CommandErrorCallback)(const str&, const strutil::strvec&, int);

	ConsoleExecutor(_Base* instance, CommandCallback command_listener, CommandErrorCallback command_error_listener);
	virtual ~ConsoleExecutor();

protected:
	int Execute(const str& command, const strutil::strvec& parameter_list);
	void OnExecutionError(const str& command, const strutil::strvec& parameter_list, int result);

private:
	_Base* instance_;
	CommandCallback command_listener_;
	CommandErrorCallback command_error_listener_;
};



// Console manager handles the basics of the console: commands, completion, history, etc.
class ConsoleCommandManager: public CommandCompleter {
public:
	typedef std::list<str> CommandList;

	ConsoleCommandManager();
	virtual ~ConsoleCommandManager();

	void AddExecutor(ConsoleCommandExecutor* executor);	// Takes ownership!
	void DeleteExecutor(ConsoleCommandExecutor* executor);

	void SetComment(const str& comment);
	bool AddCommand(const str& command);
	bool RemoveCommand(const str& command);
	int Execute(const str& command, bool append_to_history);
	void AddCompleter(CommandCompleter* completer);
	void RemoveCompleter(CommandCompleter* completer);
	CommandList GetCommandCompletionList(const str& partial_command, str& completed) const;
	std::list<str> CompleteCommand(const str& partial_command) const;
	unsigned GetHistoryCount() const;
	void SetCurrentHistoryIndex(int index);
	int GetCurrentHistoryIndex() const;
	str GetHistory(int index) const;
	void AppendHistory(const str& command);

private:
	typedef std::unordered_set<ConsoleCommandExecutor*, LEPRA_VOIDP_HASHER> CommandExecutorSet;
	typedef std::unordered_set<str> CommandSet;
	typedef std::vector<str> CommandVector;
	typedef std::unordered_set<CommandCompleter*, LEPRA_VOIDP_HASHER> CommandCompleterSet;
	CommandExecutorSet command_executor_set_;
	CommandCompleterSet command_completer_list_;
	CommandSet command_set_;
	CommandVector history_vector_;
	str comment_;
	int current_history_index_;
};



// Can be used for any type of console: graphical, OS/stdio, telnet-style, etc.
class ConsolePrompt {
public:
	enum {
		kConKeyEscape		= -1,
		kConKeyLeft		= -2,
		kConKeyRight		= -3,
		kConKeyUp		= -4,
		kConKeyDown		= -5,
		kConKeyHome		= -6,
		kConKeyEnd		= -7,
		kConKeyPageUp		= -8,
		kConKeyPageDown	= -9,
		kConKeyInsert		= -10,
		kConKeyDelete		= -11,

		kConKeyCtrlLeft	= -20,
		kConKeyCtrlRight	= -21,
		kConKeyCtrlUp		= -22,
		kConKeyCtrlDown	= -23,
		kConKeyCtrlHome	= -24,
		kConKeyCtrlEnd	= -25,

		kConKeyF1		= -30,
		kConKeyF2		= -31,
		kConKeyF3		= -32,
		kConKeyF4		= -33,
		kConKeyF5		= -34,
		kConKeyF6		= -35,
		kConKeyF7		= -36,
		kConKeyF8		= -37,
		kConKeyF9		= -38,
		kConKeyF10		= -39,
		kConKeyF11		= -40,
		kConKeyF12		= -41,
	};

	ConsolePrompt();
	virtual ~ConsolePrompt();
	virtual void SetFocus(bool focus) = 0;
	virtual int WaitChar() = 0;
	virtual void ReleaseWaitCharThread() = 0;
	virtual void Backspace(size_t count) = 0;
	virtual void EraseText(size_t count) = 0;
	virtual void PrintPrompt(const str& prompt, const str& input_text, size_t edit_index) = 0;
};

// Uses ::printf() and termios (or corresponding).
class StdioConsolePrompt: public ConsolePrompt {
public:
	StdioConsolePrompt();
	virtual ~StdioConsolePrompt();

	void SetFocus(bool focus);
	int WaitChar();
	void ReleaseWaitCharThread();
	void Backspace(size_t count);
	void EraseText(size_t count);
	void PrintPrompt(const str& prompt, const str& input_text, size_t edit_index);
};



}



#include "consolecommandmanager.inl"
