
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../Cure/Include/ConsoleManager.h"



namespace Cure
{
class GameManager;
}



namespace Life
{



class ConsoleManager: public Cure::ConsoleManager
{
	typedef Cure::ConsoleManager Parent;
public:
	ConsoleManager(Cure::GameManager* pGameManager,
		Cure::RuntimeVariableScope* pVariableScope,
		Lepra::InteractiveConsoleLogListener* pConsoleLogger,
		Lepra::ConsolePrompt* pConsolePrompt);
	virtual ~ConsoleManager();

protected:
	enum CommandCommon
	{
		COMMAND_NOT_IMPLEMENTED = -1,

		COMMAND_ECHO,
		COMMAND_SET_SUBSYSTEM_LOG_LEVEL,
		COMMAND_SET_STDOUT_LOG_LEVEL,
		COMMAND_FORK,
		COMMAND_EXECUTE_VARIABLE,
		COMMAND_EXECUTE_FILE,
		COMMAND_LIST_ACTIVE_RESOURCES,
		COMMAND_SLEEP,
		COMMAND_PUSH,
		COMMAND_SET_DEFAULT_CONFIG,
		COMMAND_SAVE_SYSTEM_CONFIG_FILE,
		COMMAND_SAVE_APPLICATION_CONFIG_FILE,

		COMMAND_DUMP_PERFORMANCE_INFO,
		COMMAND_CLEAR_PERFORMANCE_INFO,
		COMMAND_SHOW_SYSTEM_INFO,
		COMMAND_DEBUG_BREAK,
		COMMAND_HELP,

		COMMAND_COUNT_COMMON
	};

	void Init();
	unsigned GetCommandCount() const;
	const CommandPair& GetCommand(unsigned pIndex) const;
	int OnCommand(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector);

	virtual bool SaveApplicationConfigFile(Lepra::File* pFile);

	Cure::GameManager* mGameManager;

private:
	bool SaveSystemConfigFile(Lepra::File* pFile);
	bool SaveConfigFile(Lepra::File* pFile, std::list<Lepra::String>& pVariableList);

	static const CommandPair mCommandIdList[];
	Lepra::LogListener* mLogger;
	LOG_CLASS_DECLARE();
};



}
