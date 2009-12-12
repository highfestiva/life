
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
	int TranslateCommand(const Lepra::String& pCommand) const;
	int OnCommand(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector);

	virtual bool SaveApplicationConfigFile(Lepra::File* pFile, const Lepra::UnicodeString& pUserConfig);

	Cure::GameManager* mGameManager;

private:
	Lepra::UnicodeString LoadUserConfig(Lepra::File* pFile);
	bool SaveSystemConfigFile(int pScopeSkipCount, Lepra::File* pFile, const Lepra::UnicodeString& pUserConfig);
	bool SaveConfigFile(Lepra::File* pFile, std::list<Lepra::String>& pVariableList, const Lepra::UnicodeString& pUserConfig);

	static const CommandPair mCommandIdList[];
	typedef std::hash_map<Lepra::String, Lepra::String> AliasMap;
	AliasMap mAliasMap;
	Lepra::LogListener* mLogger;
	LOG_CLASS_DECLARE();
};



}
