
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/RuntimeVariableName.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/LepraOS.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/Path.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiResourceManager.h"
#include "../../UiTbc/Include/GUI/UiConsoleLogListener.h"
#include "../../UiTbc/Include/GUI/UiConsolePrompt.h"
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../UiTbc/Include/GUI/UiFileNameField.h"
#include "../../UiTbc/Include/GUI/UiTextArea.h"
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
	{"quit", COMMAND_QUIT},
	{"bye", COMMAND_BYE},
	{"zombie", COMMAND_ZOMBIE},
	{"echo-msgbox", COMMAND_ECHO_MSGBOX},
	{"start-login", COMMAND_START_LOGIN},
	{"wait-login", COMMAND_WAIT_LOGIN},
	{"logout", COMMAND_LOGOUT},
	{"start-reset-ui", COMMAND_START_RESET_UI},
	{"wait-reset-ui", COMMAND_WAIT_RESET_UI},
	{"add-player", COMMAND_ADD_PLAYER},
	{"set-mesh-visible", COMMAND_SET_MESH_VISIBLE},
};



ClientConsoleManager::ClientConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	ConsoleManager(pResourceManager, pGameManager, pVariableScope, new UiTbc::ConsoleLogListener,
		new UiTbc::ConsolePrompt),
	mWasCursorVisible(true)
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
	if (lConsoleActive)
	{
		mWasCursorVisible = mUiConsole->GetUiManager()->GetInputManager()->IsCursorVisible();
		mUiConsole->GetUiManager()->GetInputManager()->SetCursorVisible(lConsoleActive);
	}
	else
	{
		mUiConsole->GetUiManager()->GetInputManager()->SetCursorVisible(mWasCursorVisible);
	}
	return lConsoleActive;
}

UiConsole* ClientConsoleManager::GetUiConsole() const
{
	return (mUiConsole);
}



int ClientConsoleManager::FilterExecuteCommand(const str& pCommandLine)
{
	const str lCommandDelimitors(" \t\v\r\n");
	const strutil::strvec lCommandList = strutil::BlockSplit(pCommandLine, ";", true, true);
	const int lAllowedCount = 5;
	const str lAllowedList[lAllowedCount] =
	{
		str("#" RTVAR_PHYSICS_FPS),
		str("#" RTVAR_PHYSICS_RTR),
		str("#" RTVAR_PHYSICS_HALT),
		str("#Ui.3D."),
		str("echo "),
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



bool ClientConsoleManager::SaveApplicationConfigFile(File* pFile, const str& pUserConfig)
{
	bool lOk = Parent::SaveApplicationConfigFile(pFile, pUserConfig);
	if (lOk && pUserConfig.empty())
	{
		pFile->WriteString("//push \"start-login server:port username password\"\n");
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

int ClientConsoleManager::OnCommand(const HashedString& pCommand, const strutil::strvec& pParameterVector)
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
				if (!pParameterVector.empty() && pParameterVector[0] == "!")
				{
					mLog.Warning("Hard process termination due to user command!");
					SystemManager::ExitProcess(0);
				}
				else
				{
					mLog.Info("Terminating due to user command.");
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
					mLog.Warningf("usage: %s <title> <msg>", pCommand.c_str());
					lResult = 1;
				}
			}
			break;
			case COMMAND_START_LOGIN:
			{
				((GameClientSlaveManager*)GetGameManager())->Logout();

				if (pParameterVector.size() == 3)
				{
					str lUsername = pParameterVector[1];
					str lReadablePassword = pParameterVector[2];
					//pParameterVector[2] = "        ";
					// Convert into login format.
					Cure::MangledPassword lPassword(lReadablePassword);
					lReadablePassword.clear();	// Clear out password traces in string.
					Cure::LoginId lLoginToken(lUsername, lPassword);
					((GameClientSlaveManager*)GetGameManager())->RequestLogin(pParameterVector[0], lLoginToken);
				}
				else
				{
					mLog.Warningf("usage: %s <server> <username> <password>", pCommand.c_str());
					lResult = 1;
				}
			}
			break;
			case COMMAND_WAIT_LOGIN:
			{
				mLog.Info("Waiting for login to finish...");
				while (((GameClientSlaveManager*)GetGameManager())->IsLoggingIn())
				{
					Thread::Sleep(0.01);
				}
			}
			break;
			case COMMAND_LOGOUT:
			{
				mLog.Info("Logging off due to user command.");
				((GameClientSlaveManager*)GetGameManager())->Logout();
			}
			break;
			case COMMAND_START_RESET_UI:
			{
				mLog.Info("Running UI restart...");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->StartResetUi())
				{
					mLog.Info("UI is restarting.");
				}
				else
				{
					mLog.Error("Could not run UI restart!");
					lResult = 1;
				}
			}
			break;
			case COMMAND_WAIT_RESET_UI:
			{
				mLog.Info("Waiting for UI to be restarted...");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->WaitResetUi())
				{
					mLog.Info("UI is up and running.");
				}
				else
				{
					mLog.Error("UI restarted was not completed in time!");
					lResult = 1;
				}
			}
			break;
			case COMMAND_ADD_PLAYER:
			{
				mLog.Info("Adding another player.");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->CreateSlave())
				{
					mLog.Info("Another player added.");
				}
				else
				{
					mLog.Error("Could not add another player!");
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
					ResourceList lResourceList = mResourceManager->HookAllResourcesOfType("GeometryRef");
					for (ResourceList::iterator x = lResourceList.begin(); x != lResourceList.end(); ++x)
					{
						UiCure::GeometryReferenceResource* lMeshRefResource = (UiCure::GeometryReferenceResource*)*x;
						if (lMeshRefResource->GetName().find(pParameterVector[0]) != str::npos)
						{
							Tbc::GeometryBase* lMesh = lMeshRefResource->GetRamData();
							if (lMesh)
							{
								lMesh->SetAlwaysVisible(lVisible);
								++lAffectedMeshCount;
							}
						}
					}
					mResourceManager->UnhookResources(lResourceList);
					mLog.Infof("%i meshes affected.", lAffectedMeshCount);
				}
				else
				{
					mLog.Warningf("usage: %s <mesh> <bool>", pCommand.c_str());
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
		::MessageBox(NULL, "Ready?", "Waiting...", MB_OK);
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



loginstance(CONSOLE, ClientConsoleManager);



}
