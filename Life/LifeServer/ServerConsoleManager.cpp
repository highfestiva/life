
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/SystemManager.h"
#include "GameServerManager.h"
#include "ServerConsoleManager.h"



namespace Life
{



// Must lie before ServerConsoleManager to compile.
const ConsoleManager::CommandPair ServerConsoleManager::mCommandIdList[] =
{
	{"quit", COMMAND_QUIT},

	// Info/debug stuff.
	{"list-users", COMMAND_LIST_USERS},
	{"build", COMMAND_BUILD},

	// Communication.
	{"broadcast-msg", COMMAND_BROADCAST_CHAT_MESSAGE},
	{"priv-msg", COMMAND_SEND_PRIVATE_CHAT_MESSAGE},
};



ServerConsoleManager::ServerConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	Cure::RuntimeVariableScope* pVariableScope, InteractiveConsoleLogListener* pConsoleLogger,
	ConsolePrompt* pConsolePrompt):
	ConsoleManager(pResourceManager, pGameManager, pVariableScope, pConsoleLogger, pConsolePrompt)
{
	SetSecurityLevel(1);
	InitCommands();
}

ServerConsoleManager::~ServerConsoleManager()
{
}



unsigned ServerConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const ServerConsoleManager::CommandPair& ServerConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	const unsigned lLocalIndex = pIndex-Parent::GetCommandCount();
	return mCommandIdList[lLocalIndex];
}

int ServerConsoleManager::OnCommand(const HashedString& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = Parent::OnCommand(pCommand, pParameterVector);
	if (lResult < 0)
	{
		lResult = 0;

		CommandServer lCommand = (CommandServer)TranslateCommand(pCommand);
		switch (lCommand)
		{
			case COMMAND_QUIT:
			{
				int lClientCount = ((GameServerManager*)GetGameManager())->GetLoggedInClientCount();
				if (lClientCount > 0)
				{
					if (pParameterVector.size() == 1 && pParameterVector[0] == "!")
					{
						mLog.Warningf("Forced termination with %i logged-in clients.", lClientCount);
						SystemManager::AddQuitRequest(+1);
					}
					else
					{
						mLog.Warningf("Not allowed to terminate (%i logged-in clients). Use '!' to override.", lClientCount);
					}
				}
				else
				{
					mLog.Headline("Terminating due to user command.");
					SystemManager::AddQuitRequest(+1);
				}
			}
			break;
			case COMMAND_BROADCAST_CHAT_MESSAGE:
			{
				if (pParameterVector.size() == 1)
				{
					str lMessage = pParameterVector[0];
					if (((GameServerManager*)GetGameManager())->BroadcastChatMessage(lMessage))
					{
						mLog.Infof("BROADCAST CHAT: %s", pParameterVector[0].c_str());
					}
					else
					{
						mLog.Error("Could not broadcast chat message!");
						lResult = 1;
					}
				}
				else
				{
					mLog.Warningf("usage: %s <message>", pCommand.c_str());
					lResult = 1;
				}
			}
			break;
			case COMMAND_SEND_PRIVATE_CHAT_MESSAGE:
			{
				if (pParameterVector.size() == 2)
				{
					str lClientUserName = pParameterVector[0];
					str lMessage = pParameterVector[1];
					if (((GameServerManager*)GetGameManager())->SendChatMessage(lClientUserName, lMessage))
					{
						mLog.Infof("PRIVATE CHAT ServerAdmin->%s: %s", pParameterVector[0].c_str(), pParameterVector[1].c_str());
					}
					else
					{
						mLog.Error("Could not send private chat message!");
						lResult = 1;
					}
				}
				else
				{
					mLog.Warningf("usage: %s <user> <message>", pCommand.c_str());
					lResult = 1;
				}
			}
			break;
			case COMMAND_LIST_USERS:
			{
				strutil::strvec lUserNameList;
				lUserNameList = ((GameServerManager*)GetGameManager())->ListUsers();
				mLog.Info("Listing logged on users:");
				for (size_t x = 0; x < lUserNameList.size(); ++x)
				{
					mLog.Info("\t\"" + lUserNameList[x] + "\"");
				}
				mLog.Infof("A total of %u users logged in.", (unsigned)lUserNameList.size());
			}
			break;
			case COMMAND_BUILD:
			{
				if (pParameterVector.size() == 1)
				{
					((GameServerManager*)GetGameManager())->Build(pParameterVector[0]);
				}
				else
				{
					mLog.Warningf("usage: %s <what>", pCommand.c_str());
					lResult = 1;
				}
			}
			break;
			default:
			{
				lResult = -1;
			}
			break;
		}
	}
	return (lResult);
}

loginstance(CONSOLE, ServerConsoleManager);



}
