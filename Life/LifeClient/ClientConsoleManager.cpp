
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/RuntimeVariableName.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/Path.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiResourceManager.h"
#include "../../UiTBC/Include/GUI/UiConsoleLogListener.h"
#include "../../UiTBC/Include/GUI/UiConsolePrompt.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFileNameField.h"
#include "../../UiTBC/Include/GUI/UiTextArea.h"
#include "../LifeApplication.h"
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
	{_T("zombie"), COMMAND_ZOMBIE},
	{_T("echo-msgbox"), COMMAND_ECHO_MSGBOX},
	{_T("start-login"), COMMAND_START_LOGIN},
	{_T("wait-login"), COMMAND_WAIT_LOGIN},
	{_T("logout"), COMMAND_LOGOUT},
	{_T("start-reset-ui"), COMMAND_START_RESET_UI},
	{_T("wait-reset-ui"), COMMAND_WAIT_RESET_UI},
	{_T("add-player"), COMMAND_ADD_PLAYER},
	{_T("set-mesh-visible"), COMMAND_SET_MESH_VISIBLE},
};



ClientConsoleManager::ClientConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	ConsoleManager(pResourceManager, pGameManager, pVariableScope, new UiTbc::ConsoleLogListener,
		new UiTbc::ConsolePrompt)
{
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
	mUiConsole->Open();
	return Parent::Start();
}

void ClientConsoleManager::Join()
{
	mUiConsole->Close();
	Parent::Join();
}



bool ClientConsoleManager::ToggleVisible()
{
	const bool lConsoleActive = mUiConsole->ToggleVisible();
	mUiConsole->GetUiManager()->GetInputManager()->SetCursorVisible(lConsoleActive);
	return lConsoleActive;
}

UiConsole* ClientConsoleManager::GetUiConsole() const
{
	return (mUiConsole);
}



int ClientConsoleManager::FilterExecuteCommand(const str& pCommandLine)
{
	const str lCommandDelimitors(_T(" \t\v\r\n"));
	const strutil::strvec lCommandList = strutil::BlockSplit(pCommandLine, _T(";"), true, true);
	const int lAllowedCount = 5;
	const str lAllowedList[lAllowedCount] =
	{
		str(_T("#") _T(RTVAR_PHYSICS_FPS)),
		str(_T("#") _T(RTVAR_PHYSICS_RTR)),
		str(_T("#") _T(RTVAR_PHYSICS_HALT)),
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
				if (!pParameterVector.empty() && pParameterVector[0] == _T("!"))
				{
					mLog.AWarning("Hard process termination due to user command!");
					SystemManager::ExitProcess(0);
				}
				else
				{
					mLog.AInfo("Terminating due to user command.");
					SystemManager::AddQuitRequest(+1);
				}
			}
			break;
			case COMMAND_BYE:
			{
				((GameClientSlaveManager*)GetGameManager())->SetIsQuitting();
			}
			break;
			case COMMAND_ZOMBIE:
			{
				Application::GetApplication()->SetZombieTick(Application::ZombieTick(this, &ClientConsoleManager::HeadlessTick));
				while (Application::GetApplication()->GetTicker())
				{
					Thread::Sleep(0.5f);
				}
				for (size_t x = 0; x < pParameterVector.size(); ++x)
				{
					PushYieldCommand(pParameterVector[x]);
				}
				MemberThread<Cure::ConsoleManager>* lConsoleThread = mConsoleThread;
				mConsoleThread = 0;
				lConsoleThread->RequestSelfDestruct();
				lConsoleThread->RequestStop();
			}
			break;
			case COMMAND_ECHO_MSGBOX:
			{
				if (pParameterVector.size() == 2)
				{
					mUiConsole->GetUiManager()->GetDisplayManager()->ShowMessageBox(pParameterVector[1], pParameterVector[0]);
				}
				else
				{
					mLog.Warningf(_T("usage: %s <title> <msg>"), pCommand.c_str());
					lResult = 1;
				}
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
					lResult = 1;
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
			case COMMAND_SET_MESH_VISIBLE:
			{
				bool lVisible = false;
				if (pParameterVector.size() == 2 && strutil::StringToBool(pParameterVector[1], lVisible))
				{
					int lAffectedMeshCount = 0;
					typedef Cure::ResourceManager::ResourceList ResourceList;
					ResourceList lResourceList = mResourceManager->HookAllResourcesOfType(_T("GeometryRef"));
					for (ResourceList::iterator x = lResourceList.begin(); x != lResourceList.end(); ++x)
					{
						UiCure::GeometryReferenceResource* lMeshRefResource = (UiCure::GeometryReferenceResource*)*x;
						if (lMeshRefResource->GetName().find(pParameterVector[0]) != str::npos)
						{
							TBC::GeometryBase* lMesh = lMeshRefResource->GetRamData();
							if (lMesh)
							{
								lMesh->SetAlwaysVisible(lVisible);
								++lAffectedMeshCount;
							}
						}
					}
					mResourceManager->UnhookResources(lResourceList);
					mLog.Infof(_T("%i meshes affected."), lAffectedMeshCount);
				}
				else
				{
					mLog.Warningf(_T("usage: %s <mesh> <bool>"), pCommand.c_str());
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



void ClientConsoleManager::HeadlessTick()
{
	// Check What state our application zombie is in.
	if (mUiConsole)
	{
		// Pre-destroy.
		delete mUiConsole;
		mUiConsole = 0;
		GetGameManager()->SetConsoleManager(0);	// Snip. Now we're free floating.
		SetGameManager(0);
	}
	else if (!Application::GetApplication()->GetTicker())
	{
		// Post-destroy, pre-init.
#ifdef LEPRA_WINDOWS
		::MessageBox(NULL, _T("Ready?"), _T("Waiting..."), MB_OK);
#endif // Windows
		while (ExecuteYieldCommand() >= 0)
			;
	}
	else
	{
		// Post-init.
		SetGameManager(((Life::GameClientMasterTicker*)Application::GetApplication()->GetTicker())->GetSlave(0));
		Life::Application::GetApplication()->SetZombieTick(Life::Application::ZombieTick());
		delete this;	// Nice...
	}
}



LOG_CLASS_DEFINE(CONSOLE, ClientConsoleManager);



}
