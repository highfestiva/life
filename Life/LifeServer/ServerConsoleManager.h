
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../ConsoleManager.h"



namespace Life
{



class ServerConsoleManager: public ConsoleManager
{
	typedef ConsoleManager Parent;
public:
	ServerConsoleManager(Cure::GameManager* pGameServerManager, Cure::RuntimeVariableScope* pVariableScope,
		Lepra::InteractiveConsoleLogListener* pConsoleLogger, Lepra::ConsolePrompt* pConsolePrompt);
	virtual ~ServerConsoleManager();

private:
	enum CommandServer
	{
		COMMAND_QUIT = COMMAND_COUNT_COMMON,

		COMMAND_LOGIN_ENABLE,
		COMMAND_LOGIN_DISABLE,

		COMMAND_LIST_USERS,

		COMMAND_BROADCAST_CHAT_MESSAGE,
		COMMAND_SEND_PRIVATE_CHAT_MESSAGE,
	};

	unsigned GetCommandCount() const;
	const CommandPair& GetCommand(unsigned pIndex) const;
	int OnCommand(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector);

	static const CommandPair mCommandIdList[];
	LOG_CLASS_DECLARE();
};



}
