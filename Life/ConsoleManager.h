
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ConsoleManager.h"
#include "../Lepra/Include/File.h"
#include "../Lepra/Include/Unordered.h"
#include "Life.h"



namespace Lepra
{
class LogListener;
}
namespace Cure
{
class GameManager;
class ResourceManager;
}



namespace Life
{



class ConsoleManager: public Cure::ConsoleManager
{
	typedef Cure::ConsoleManager Parent;
public:
	ConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		Cure::RuntimeVariableScope* pVariableScope, InteractiveConsoleLogListener* pConsoleLogger,
		ConsolePrompt* pConsolePrompt);
	virtual ~ConsoleManager();

	virtual void InitCommands();

	void SetSecurityLevel(int pLevel);
	int GetSecurityLevel() const;

	Cure::GameManager* GetGameManager() const;
	void SetGameManager(Cure::GameManager* pGameManager);

	void OnKey(const str& pKeyName);

protected:
	enum CommandCommon
	{
		COMMAND_NOT_IMPLEMENTED = -1,

		COMMAND_ALIAS,
		COMMAND_ALIAS_VALUE,
		COMMAND_BIND_KEY,
		COMMAND_ECHO,
		COMMAND_EXECUTE_FILE,
		COMMAND_EXECUTE_VARIABLE,
		COMMAND_FORK,
		COMMAND_NOP,
		COMMAND_LIST_ACTIVE_RESOURCES,
		COMMAND_PUSH,
		COMMAND_REPEAT,
		COMMAND_SAVE_SYSTEM_CONFIG_FILE,
		COMMAND_SAVE_APPLICATION_CONFIG_FILE,
		COMMAND_SET_DEFAULT_CONFIG,
		COMMAND_SET_STDOUT_LOG_LEVEL,
		COMMAND_SET_SUBSYSTEM_LOG_LEVEL,
		COMMAND_SLEEP,
		COMMAND_WAIT_LOADED,

		COMMAND_DUMP_PERFORMANCE_INFO,
		COMMAND_CLEAR_PERFORMANCE_INFO,
		COMMAND_SHELL_EXECUTE,
		COMMAND_SHOW_SYSTEM_INFO,
		COMMAND_SHOW_GAME_INFO,
		COMMAND_HANG_GAME,
		COMMAND_UNHANG_GAME,
		COMMAND_DEBUG_BREAK,
		COMMAND_HELP,

		COMMAND_COUNT_COMMON
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	int TranslateCommand(const HashedString& pCommand) const;
	virtual int OnCommand(const HashedString& pCommand, const strutil::strvec& pParameterVector);

	virtual bool SaveApplicationConfigFile(File* pFile, const str& pUserConfig);

	Cure::ResourceManager* GetResourceManager() const;

protected:
	str LoadUserConfig(File* pFile);
	bool SaveSystemConfigFile(int pScopeSkipCount, File* pFile, const str& pUserConfig);
	bool SaveConfigFile(File* pFile, const str& pPrefix, std::list<str>& pVariableList, const str& pUserConfig);
	static str GetQuoted(const str& s);

	static const CommandPair mCommandIdList[];
	typedef std::unordered_map<HashedString, str, HashedStringHasher> AliasMap;
	typedef AliasMap KeyMap;

	int mSecurityLevel;
	Cure::GameManager* mGameManager;
	Cure::ResourceManager* mResourceManager;
	AliasMap mAliasMap;
	KeyMap mKeyMap;
	LogListener* mLogger;
	logclass();
};



}
