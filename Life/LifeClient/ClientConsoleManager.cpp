
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/RuntimeVariableName.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/Path.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTBC/Include/GUI/UiConsoleLogListener.h"
#include "../../UiTBC/Include/GUI/UiConsolePrompt.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFileNameField.h"
#include "../../UiTBC/Include/GUI/UiTextArea.h"
#include "GameClientSlaveManager.h"
#include "GameClientMasterTicker.h"
#include "ClientConsoleManager.h"
#include "RtVar.h"
#include "UiConsole.h"



namespace Life
{



// Must lie before ClientConsoleManager to compile.
const ClientConsoleManager::CommandPair ClientConsoleManager::mCommandIdList[] =
{
	{_T("quit"), COMMAND_QUIT},
	{_T("bye"), COMMAND_BYE},
	{_T("start-login"), COMMAND_START_LOGIN},
	{_T("wait-login"), COMMAND_WAIT_LOGIN},
	{_T("logout"), COMMAND_LOGOUT},
	{_T("start-reset-ui"), COMMAND_START_RESET_UI},
	{_T("wait-reset-ui"), COMMAND_WAIT_RESET_UI},
	{_T("add-player"), COMMAND_ADD_PLAYER},
	{_T("set-avatar-engine-power"), COMMAND_SET_AVATAR_ENGINE_POWER},
#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS)
	{_T("builddata"), COMMAND_BUILD_DATA},
#endif // Debug & Windows
};



ClientConsoleManager::ClientConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	ConsoleManager(pResourceManager, pGameManager, pVariableScope, new UiTbc::ConsoleLogListener,
		new UiTbc::ConsolePrompt)
{
	InitCommands();
	mUiConsole = new UiConsole(this, pUiManager, pArea);
}

ClientConsoleManager::~ClientConsoleManager()
{
	delete GetConsoleLogger();
	SetConsoleLogger(0);
	delete (mUiConsole);
	mUiConsole = 0;
}



bool ClientConsoleManager::Start()
{
	bool lOk = Parent::Start();
	mUiConsole->Open();
	return (lOk);
}

void ClientConsoleManager::Join()
{
	mUiConsole->Close();
	Parent::Join();
}



UiConsole* ClientConsoleManager::GetUiConsole() const
{
	return (mUiConsole);
}



int ClientConsoleManager::FilterExecuteCommand(const str& pCommandLine)
{
	const str lCommandDelimitors(_T(" \t\v\r\n"));
	const strutil::strvec lCommandList = strutil::BlockSplit(pCommandLine, _T(";"), true, true);
	const int lAllowedCount = 4;
	const str lAllowedList[lAllowedCount] =
	{
		str(_T("#") _T(RTVAR_PHYSICS_FPS)),
		str(_T("#") _T(RTVAR_PHYSICS_RTR)),
		str(_T("#Ui.3D.")),
		str(_T("echo ")),
	};
	int lResult = 0;
	for (size_t lCommandIndex = 0; lResult == 0 && lCommandIndex < lCommandList.size(); ++lCommandIndex)
	{
		lResult = -1;
		const str& lTempCommand = lCommandList[lCommandIndex];
		const str lCommand = strutil::StripLeft(lTempCommand, lCommandDelimitors);
		for (int x = 0; x < lAllowedCount; ++x)
		{
			if (strutil::StartsWith(lCommand, lAllowedList[x]))
			{
				lResult = ExecuteCommand(lCommand);
				break;
			}
		}
	}
	return lResult;
}



bool ClientConsoleManager::SaveApplicationConfigFile(File* pFile, const wstr& pUserConfig)
{
	bool lOk = Parent::SaveApplicationConfigFile(pFile, pUserConfig);
	if (lOk && pUserConfig.empty())
	{
		pFile->WriteString<wchar_t>(L"//push \"start-login server:port username password\"\n");
		lOk = true;	// TODO: check if all writes went well.
	}
	return (lOk);
}



unsigned ClientConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const ClientConsoleManager::CommandPair& ClientConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int ClientConsoleManager::OnCommand(const str& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = Parent::OnCommand(pCommand, pParameterVector);
	if (lResult < 0)
	{
		lResult = 0;

		CommandClient lCommand = (CommandClient)TranslateCommand(pCommand);
		switch ((int)lCommand)
		{
			case COMMAND_SET_DEFAULT_CONFIG:
			{
				UiCure::SetDefault(GetVariableScope());
			}
			break;
			case COMMAND_QUIT:
			{
				mLog.AInfo("Terminating due to user command.");
				SystemManager::AddQuitRequest(+1);
			}
			break;
			case COMMAND_BYE:
			{
				((GameClientSlaveManager*)GetGameManager())->SetIsQuitting();
			}
			break;
			case COMMAND_START_LOGIN:
			{
				((GameClientSlaveManager*)GetGameManager())->Logout();

				if (pParameterVector.size() == 3)
				{
					wstr lUsername = wstrutil::Encode(pParameterVector[1]);
					wstr lReadablePassword = wstrutil::Encode(pParameterVector[2]);
					//pParameterVector[2] = _T("        ");
					// Convert into login format.
					Cure::MangledPassword lPassword(lReadablePassword);
					lReadablePassword.clear();	// Clear out password traces in string.
					Cure::LoginId lLoginToken(lUsername, lPassword);
					((GameClientSlaveManager*)GetGameManager())->RequestLogin(pParameterVector[0], lLoginToken);
				}
				else
				{
					mLog.Warningf(_T("usage: %s <server> <username> <password>"), pCommand.c_str());
				}
			}
			break;
			case COMMAND_WAIT_LOGIN:
			{
				mLog.AInfo("Waiting for login to finish...");
				while (((GameClientSlaveManager*)GetGameManager())->IsLoggingIn())
				{
					Thread::Sleep(0.01);
				}
			}
			break;
			case COMMAND_LOGOUT:
			{
				mLog.AInfo("Logging off due to user command.");
				((GameClientSlaveManager*)GetGameManager())->Logout();
			}
			break;
			case COMMAND_START_RESET_UI:
			{
				mLog.AInfo("Running UI restart...");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->StartResetUi())
				{
					mLog.AInfo("UI is restarting.");
				}
				else
				{
					mLog.AError("Could not run UI restart!");
					lResult = 1;
				}
			}
			break;
			case COMMAND_WAIT_RESET_UI:
			{
				mLog.AInfo("Waiting for UI to be restarted...");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->WaitResetUi())
				{
					mLog.AInfo("UI is up and running.");
				}
				else
				{
					mLog.AError("UI restarted was not completed in time!");
					lResult = 1;
				}
			}
			break;
			case COMMAND_ADD_PLAYER:
			{
				mLog.AInfo("Adding another player.");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->CreateSlave())
				{
					mLog.AInfo("Another player added.");
				}
				else
				{
					mLog.AError("Could not add another player!");
					lResult = 1;
				}
			}
			break;
			case COMMAND_SET_AVATAR_ENGINE_POWER:
			{
				if (pParameterVector.size() == 3)
				{
					log_adebug("Setting avatar engine power.");
					int lAspect = 0;
					strutil::StringToInt(pParameterVector[0], lAspect);
					double lPower;
					strutil::StringToDouble(pParameterVector[1], lPower);
					double lAngle;
					strutil::StringToDouble(pParameterVector[2], lAngle);
					if (!((GameClientSlaveManager*)GetGameManager())->SetAvatarEnginePower(lAspect, (float)lPower, (float)lAngle))
					{
						mLog.AError("Could not set avatar engine power!");
						lResult = 1;
					}
				}
				else
				{
					mLog.Warningf(_T("usage: %s <aspect> <power> <angle>"), pCommand.c_str());
				}
			}
			break;
#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS)
			case COMMAND_BUILD_DATA:
			{
				const Cure::GameObjectId lAvatarId = ((GameClientSlaveManager*)GetGameManager())->GetAvatarInstanceId();
				if (!lAvatarId)
				{
					break;
				}
				const str lAvatarType = GetGameManager()->GetContext()->GetObject(lAvatarId)->GetClassId();
				((GameClientSlaveManager*)GetGameManager())->Logout();
				Thread::Sleep(0.5);
				GetResourceManager()->ForceFreeCache();
				const str lCurrentDir = SystemManager::GetCurrentDirectory();
				::SetCurrentDirectoryA(astrutil::Encode(Path::GetParentDirectory(lCurrentDir)).c_str());
				::system("c:/Program/Python31/python.exe -B Tools/build/rgo.py builddata");
				::SetCurrentDirectoryA(astrutil::Encode(lCurrentDir).c_str());
				const str lUserName = CURE_RTVAR_SLOW_GET(GetVariableScope(), RTVAR_LOGIN_USERNAME, EmptyString);
				const str lServer = CURE_RTVAR_SLOW_GET(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, EmptyString);
				str lPw(_T("CarPassword"));
				const Cure::LoginId lLoginId(lUserName, Cure::MangledPassword(lPw));
				((GameClientSlaveManager*)GetGameManager())->RequestLogin(lServer, lLoginId);
				((GameClientSlaveManager*)GetGameManager())->ToggleConsole();
				ExecuteCommand(_T("wait-login"));
				((GameClientSlaveManager*)GetGameManager())->SelectAvatar(lAvatarType);
			}
			break;
#endif // Debug & Windows
			default:
			{
				lResult = -1;
			}
			break;
		}
	}
	return (lResult);
}



LOG_CLASS_DEFINE(CONSOLE, ClientConsoleManager);



}
