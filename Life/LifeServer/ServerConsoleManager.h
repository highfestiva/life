
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../ConsoleManager.h"



namespace Life
{



class ServerConsoleManager: public ConsoleManager
{
	typedef ConsoleManager Parent;
public:
	ServerConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameServerManager,
		Cure::RuntimeVariableScope* pVariableScope, InteractiveConsoleLogListener* pConsoleLogger,
		ConsolePrompt* pConsolePrompt);
	virtual ~ServerConsoleManager();

private:
	enum CommandServer
	{
		COMMAND_QUIT = COMMAND_COUNT_COMMON,

		COMMAND_LOGIN_ENABLE,
		COMMAND_LOGIN_DISABLE,

		COMMAND_LIST_USERS,
		COMMAND_BUILD,

		COMMAND_BROADCAST_CHAT_MESSAGE,
		COMMAND_SEND_PRIVATE_CHAT_MESSAGE,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	static const CommandPair mCommandIdList[];
	logclass();
};



}
