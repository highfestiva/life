
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

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
// lConsoleManager.Execute(_T("ls -R; ps Afwl"));



#pragma once



#include <hash_map>
#include <list>
#include <set>
#include <vector>
#include "String.h"



namespace Lepra
{



class CommandCompleter
{
public:
	virtual std::list<String> CompleteCommand(const String& pPartialCommand) const = 0;
};



// Console manager handles the basics of the console: commands, completion, history, etc.
class ConsoleCommandManager: public CommandCompleter
{
public:
	typedef std::list<String> CommandList;

	ConsoleCommandManager();
	virtual ~ConsoleCommandManager();

	void SetComment(const String& pComment);
	bool AddCommand(const String& pCommand);
	int Execute(const String& pCommand, bool pAppendToHistory);
	void AddCompleter(CommandCompleter* pCompleter);
	void RemoveCompleter(CommandCompleter* pCompleter);
	CommandList GetCommandCompletionList(const String& pPartialCommand, String& pCompleted) const;
	std::list<String> CompleteCommand(const String& pPartialCommand) const;
	unsigned GetHistoryCount() const;
	void SetCurrentHistoryIndex(int pIndex);
	int GetCurrentHistoryIndex() const;
	String GetHistory(int pIndex) const;
	void AppendHistory(const String& pCommand);

protected:
	virtual int OnExecute(const String& pCommand, const StringUtility::StringVector& pParameterList) = 0;
	virtual void OnExecutionError(const String& pCommand, const StringUtility::StringVector& pParameterList, int pResult) = 0;

private:
	typedef std::set<String> CommandSet;
	typedef std::vector<String> CommandVector;
	typedef std::set<CommandCompleter*> CommandCompleterSet;
	CommandCompleterSet mCommandCompleterList;
	CommandSet mCommandSet;
	CommandVector mHistoryVector;
	String mComment;
	int mCurrentHistoryIndex;
};

template<class _Base> class ConsoleExecutor: public ConsoleCommandManager
{
public:
	typedef int (_Base::*CommandCallback)(const String&, const StringUtility::StringVector&);
	typedef void (_Base::*CommandErrorCallback)(const String&, const StringUtility::StringVector&, int);

	ConsoleExecutor(_Base* pInstance, CommandCallback pCommandListener, CommandErrorCallback pCommandErrorListener);
	virtual ~ConsoleExecutor();

protected:
	int OnExecute(const String& pCommand, const StringUtility::StringVector& pParameterList);
	void OnExecutionError(const String& pCommand, const StringUtility::StringVector& pParameterList, int pResult);

private:
	_Base* mInstance;
	CommandCallback mCommandListener;
	CommandErrorCallback mCommandErrorListener;
};



// Can be used for any type of console: graphical, OS/stdio, telnet-style, etc.
class ConsolePrompt
{
public:
	enum
	{
		CON_KEY_ESCAPE		= -1,
		CON_KEY_LEFT		= -2,
		CON_KEY_RIGHT		= -3,
		CON_KEY_UP		= -4,
		CON_KEY_DOWN		= -5,
		CON_KEY_HOME		= -6,
		CON_KEY_END		= -7,
		CON_KEY_PAGE_UP		= -8,
		CON_KEY_PAGE_DOWN	= -9,
		CON_KEY_INSERT		= -10,
		CON_KEY_DELETE		= -11,

		CON_KEY_CTRL_LEFT	= -20,
		CON_KEY_CTRL_RIGHT	= -21,
		CON_KEY_CTRL_UP		= -22,
		CON_KEY_CTRL_DOWN	= -23,
		CON_KEY_CTRL_HOME	= -24,
		CON_KEY_CTRL_END	= -25,

		CON_KEY_F1		= -30,
		CON_KEY_F2		= -31,
		CON_KEY_F3		= -32,
		CON_KEY_F4		= -33,
		CON_KEY_F5		= -34,
		CON_KEY_F6		= -35,
		CON_KEY_F7		= -36,
		CON_KEY_F8		= -37,
		CON_KEY_F9		= -38,
		CON_KEY_F10		= -39,
		CON_KEY_F11		= -40,
		CON_KEY_F12		= -41,
	};

	ConsolePrompt();
	virtual ~ConsolePrompt();
	virtual void SetFocus(bool pFocus) = 0;
	virtual int WaitChar() = 0;
	virtual void ReleaseWaitCharThread() = 0;
	virtual void Backspace(size_t pCount) = 0;
	virtual void EraseText(size_t pCount) = 0;
	virtual void PrintPrompt(const Lepra::String& pPrompt, const Lepra::String& pInputText, size_t pEditIndex) = 0;
};

// Uses ::printf() and termios (or corresponding).
class StdioConsolePrompt: public ConsolePrompt
{
public:
	StdioConsolePrompt();
	virtual ~StdioConsolePrompt();

	void SetFocus(bool pFocus);
	int WaitChar();
	void ReleaseWaitCharThread();
	void Backspace(size_t pCount);
	void EraseText(size_t pCount);
	void PrintPrompt(const Lepra::String& pPrompt, const Lepra::String& pInputText, size_t pEditIndex);
};



}



#include "ConsoleCommandManager.inl"
