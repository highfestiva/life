
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Lepra/Include/ConsoleCommandManager.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/LogListener.h"
#include "../../Lepra/Include/MemberThread.h"
#include "../Include/Cure.h"



namespace Cure
{



class RuntimeVariableScope;



class ConsoleManager
{
public:
	ConsoleManager(RuntimeVariableScope* pVariableScope, InteractiveConsoleLogListener* pConsoleLogger,
		ConsolePrompt* pConsolePrompt);
	virtual ~ConsoleManager();
	void SetConsoleLogger(InteractiveConsoleLogListener* pLogger);

	virtual bool Start();
	virtual void Join();

	void PushYieldCommand(const str& pCommand);
	int ExecuteCommand(const str& pCommand);
	int ExecuteYieldCommand();

	ConsoleCommandManager* GetConsoleCommandManager() const;

protected:
	bool ForkExecuteCommand(const str& pCommand);

	struct CommandPair
	{
		const tchar* mCommandName;
		int mCommandId;
	};

	virtual void Init();
	std::list<str> GetCommandList() const;
	virtual int TranslateCommand(const str& pCommand) const;
	void PrintCommandList(const std::list<str>& pCommandList);
	InteractiveConsoleLogListener* GetConsoleLogger() const;
	ConsolePrompt* GetConsolePrompt() const;

	RuntimeVariableScope* GetVariableScope() const;

private:
	void AddCommands();
	void ConsoleThreadEntry();
	int OnCommandLocal(const str& pCommand, const strutil::strvec& pParameterVector);
	void OnCommandError(const str& pCommand, const strutil::strvec& pParameterVector, int pResult);

	virtual unsigned GetCommandCount() const = 0;
	virtual const CommandPair& GetCommand(unsigned pIndex) const = 0;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector) = 0;

	RuntimeVariableScope* mVariableScope;
	InteractiveConsoleLogListener* mConsoleLogger;
	ConsolePrompt* mConsolePrompt;
	ConsoleCommandManager* mConsoleCommandManager;
	MemberThread<ConsoleManager> mConsoleThread;
	Lock mLock;
	std::list<str> mYieldCommandList;

	LOG_CLASS_DECLARE();
};



}
