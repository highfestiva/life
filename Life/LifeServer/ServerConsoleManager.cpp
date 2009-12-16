
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/SystemManager.h"
#include "GameServerManager.h"
#include "ServerConsoleManager.h"



namespace Life
{



// Must lie before ServerConsoleManager to compile.
const ConsoleManager::CommandPair ServerConsoleManager::mCommandIdList[] =
{
	{_T("quit"), COMMAND_QUIT},

	// Info/debug stuff.
	{_T("list-users"), COMMAND_LIST_USERS},

	// Communication.
	{_T("broadcast-msg"), COMMAND_BROADCAST_CHAT_MESSAGE},
	{_T("priv-msg"), COMMAND_SEND_PRIVATE_CHAT_MESSAGE},
};



ServerConsoleManager::ServerConsoleManager(Cure::GameManager* pGameManager, Cure::RuntimeVariableScope* pVariableScope,
	Lepra::InteractiveConsoleLogListener* pConsoleLogger, Lepra::ConsolePrompt* pConsolePrompt):
	ConsoleManager(pGameManager, pVariableScope, pConsoleLogger, pConsolePrompt)
{
	Init();
}

ServerConsoleManager::~ServerConsoleManager()
{
}



unsigned ServerConsoleManager::GetCommandCount() const
{
	return (Parent::GetCommandCount() + sizeof(mCommandIdList)/sizeof(mCommandIdList[0]));
}

const ServerConsoleManager::CommandPair& ServerConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int ServerConsoleManager::OnCommand(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector)
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
				int lClientCount = ((GameServerManager*)mGameManager)->GetLoggedInClientCount();
				if (lClientCount > 0)
				{
					if (pParameterVector.size() == 1 && pParameterVector[0] == _T("!"))
					{
						mLog.Warningf(_T("Forced termination with %i logged-in clients."), lClientCount);
						Lepra::SystemManager::AddQuitRequest(+1);
					}
					else
					{
						mLog.Warningf(_T("Not allowed to terminate (%i logged-in clients). Use '!' to override."), lClientCount);
					}
				}
				else
				{
					mLog.AHeadline("Terminating due to user command.");
					Lepra::SystemManager::AddQuitRequest(+1);
				}
			}
			break;
			case COMMAND_BROADCAST_CHAT_MESSAGE:
			{
				if (pParameterVector.size() == 1)
				{
					Lepra::UnicodeString lMessage = Lepra::UnicodeStringUtility::ToOwnCode(pParameterVector[0]);
					if (((GameServerManager*)mGameManager)->BroadcastChatMessage(lMessage))
					{
						mLog.Infof(_T("BROADCAST CHAT: %s"), pParameterVector[0].c_str());
					}
					else
					{
						mLog.AError("Could not broadcast chat message!");
						lResult = 1;
					}
				}
				else
				{
					mLog.Warningf(_T("usage: %s <message>"), pCommand.c_str());
					lResult = 1;
				}
			}
			break;
			case COMMAND_SEND_PRIVATE_CHAT_MESSAGE:
			{
				if (pParameterVector.size() == 2)
				{
					Lepra::UnicodeString lClientUserName = Lepra::UnicodeStringUtility::ToOwnCode(pParameterVector[0]);
					Lepra::UnicodeString lMessage = Lepra::UnicodeStringUtility::ToOwnCode(pParameterVector[1]);
					if (((GameServerManager*)mGameManager)->SendChatMessage(lClientUserName, lMessage))
					{
						mLog.Infof(_T("PRIVATE CHAT ServerAdmin->%s: %s"), pParameterVector[0].c_str(), pParameterVector[1].c_str());
					}
					else
					{
						mLog.AError("Could not send private chat message!");
						lResult = 1;
					}
				}
				else
				{
					mLog.Warningf(_T("usage: %s <user> <message>"), pCommand.c_str());
					lResult = 1;
				}
			}
			break;
			case COMMAND_LIST_USERS:
			{
				Lepra::UnicodeStringUtility::StringVector lUserNameList;
				lUserNameList = ((GameServerManager*)mGameManager)->ListUsers();
				mLog.AInfo("Listing logged on users:");
				for (size_t x = 0; x < lUserNameList.size(); ++x)
				{
					mLog.Info(_T("\t\"") + Lepra::UnicodeStringUtility::ToCurrentCode(lUserNameList[x]) + _T("\""));
				}
				mLog.Infof(_T("A total of %u users logged in."), (unsigned)lUserNameList.size());
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

LOG_CLASS_DEFINE(CONSOLE, ServerConsoleManager);



}
