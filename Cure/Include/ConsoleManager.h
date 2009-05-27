
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once



#include "../../Lepra/Include/ConsoleCommandManager.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/LogListener.h"
#include "../../Lepra/Include/MemberThread.h"



namespace Cure
{



class RuntimeVariableScope;



class ConsoleManager
{
public:
	ConsoleManager(RuntimeVariableScope* pVariableScope, Lepra::InteractiveConsoleLogListener* pConsoleLogger,
		Lepra::ConsolePrompt* pConsolePrompt);
	virtual ~ConsoleManager();
	void SetConsoleLogger(Lepra::InteractiveConsoleLogListener* pLogger);

	virtual bool Start();
	virtual void Join();

	void PushYieldCommand(const Lepra::String& pCommand);
	int ExecuteCommand(const Lepra::String& pCommand);

protected:
	bool ForkExecuteCommand(const Lepra::String& pCommand);

	struct CommandPair
	{
		const Lepra::tchar* mCommandName;
		int mCommandId;
	};

	virtual void Init();
	std::list<Lepra::String> GetCommandList() const;
	virtual int TranslateCommand(const Lepra::String& pCommand) const;
	void PrintCommandList(const std::list<Lepra::String>& pCommandList);
	Lepra::InteractiveConsoleLogListener* GetConsoleLogger() const;
	Lepra::ConsolePrompt* GetConsolePrompt() const;
	Lepra::ConsoleCommandManager* GetConsoleCommandManager() const;

	RuntimeVariableScope* GetVariableScope();

private:
	void AddCommands();
	void ConsoleThreadEntry();
	int OnCommandLocal(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector);
	void OnCommandError(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector, int pResult);

	virtual unsigned GetCommandCount() const = 0;
	virtual const CommandPair& GetCommand(unsigned pIndex) const = 0;
	virtual int OnCommand(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector) = 0;

	RuntimeVariableScope* mVariableScope;
	Lepra::InteractiveConsoleLogListener* mConsoleLogger;
	Lepra::ConsolePrompt* mConsolePrompt;
	Lepra::ConsoleCommandManager* mConsoleCommandManager;
	Lepra::MemberThread<ConsoleManager> mConsoleThread;
	Lepra::Lock mLock;
	std::list<Lepra::String> mYieldCommandList;

	LOG_CLASS_DECLARE();
};



}
