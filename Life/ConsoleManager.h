
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Cure/Include/ConsoleManager.h"
#include "Life.h"



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
		InteractiveConsoleLogListener* pConsoleLogger,
		ConsolePrompt* pConsolePrompt);
	virtual ~ConsoleManager();

	void Init();

protected:
	enum CommandCommon
	{
		COMMAND_NOT_IMPLEMENTED = -1,

		COMMAND_ALIAS,
		COMMAND_ALIAS_VALUE,
		COMMAND_ECHO,
		COMMAND_EXECUTE_FILE,
		COMMAND_EXECUTE_VARIABLE,
		COMMAND_FORK,
		COMMAND_LIST_ACTIVE_RESOURCES,
		COMMAND_PUSH,
		COMMAND_SAVE_SYSTEM_CONFIG_FILE,
		COMMAND_SAVE_APPLICATION_CONFIG_FILE,
		COMMAND_SET_DEFAULT_CONFIG,
		COMMAND_SET_STDOUT_LOG_LEVEL,
		COMMAND_SET_SUBSYSTEM_LOG_LEVEL,
		COMMAND_SLEEP,

		COMMAND_DUMP_PERFORMANCE_INFO,
		COMMAND_CLEAR_PERFORMANCE_INFO,
		COMMAND_SHOW_SYSTEM_INFO,
		COMMAND_DEBUG_BREAK,
		COMMAND_HELP,

		COMMAND_COUNT_COMMON
	};

	unsigned GetCommandCount() const;
	const CommandPair& GetCommand(unsigned pIndex) const;
	int TranslateCommand(const str& pCommand) const;
	int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	virtual bool SaveApplicationConfigFile(File* pFile, const wstr& pUserConfig);

	Cure::GameManager* mGameManager;

private:
	wstr LoadUserConfig(File* pFile);
	bool SaveSystemConfigFile(int pScopeSkipCount, File* pFile, const wstr& pUserConfig);
	bool SaveConfigFile(File* pFile, const str& pPrefix, std::list<str>& pVariableList, const wstr& pUserConfig);

	static const CommandPair mCommandIdList[];
	typedef std::hash_map<str, str> AliasMap;
	AliasMap mAliasMap;
	LogListener* mLogger;
	LOG_CLASS_DECLARE();
};



}
