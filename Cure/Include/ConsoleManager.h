
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../Lepra/Include/ConsoleCommandManager.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/String.h"
#include "../Include/Cure.h"



namespace Lepra
{
class InteractiveConsoleLogListener;
}



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

	virtual LogDecorator& GetLogger() const;

	void AddFork(Thread* pThread);
	void RemoveFork(Thread* pThread);

	InteractiveConsoleLogListener* GetConsoleLogger() const;
	ConsolePrompt* GetConsolePrompt() const;
	RuntimeVariableScope* GetVariableScope() const;

protected:
	struct CommandPair
	{
		const tchar* mCommandName;
		int mCommandId;
	};
	typedef std::unordered_map<HashedString, int, HashedStringHasher> CommandLookupMap;
	typedef std::list<Thread*> ForkList;

	bool ForkExecuteCommand(const str& pCommand);

	virtual void InitCommands();
	std::list<str> GetCommandList() const;
	virtual int TranslateCommand(const str& pCommand) const;
	void PrintCommandList(const std::list<str>& pCommandList);

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
	MemberThread<ConsoleManager>* mConsoleThread;
	CommandLookupMap mCommandLookup;
	Lock mLock;
	std::list<str> mYieldCommandList;
	ForkList mForkList;
	bool mHistorySilentUntilNextExecute;

	logclass();
};



}
