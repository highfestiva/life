
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTbc/Include/GUI/UiConsolePrompt.h"
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../UiTbc/Include/GUI/UiFileNameField.h"
#include "../RtVar.h"
#include "GameClientSlaveManager.h"
#include "GameClientMasterTicker.h"
#include "ClientConsoleManager.h"



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

	{_T("export-all-types"), 1001001},
};



ClientConsoleManager::ClientConsoleManager(Cure::GameManager* pGameManager, UiCure::GameUiManager* pUiManager,
	Cure::RuntimeVariableScope* pVariableScope, const Lepra::PixelRect& pArea):
	ConsoleManager(pGameManager, pVariableScope, new UiTbc::ConsoleLogListener(), new UiTbc::ConsolePrompt()),
	mUiManager(pUiManager),
	mArea(pArea),
	mConsoleComponent(0),
	mConsoleOutput(0),
	mConsoleInput(0),
	mIsConsoleActive(false),
	mIsFirstConsoleUse(true),
	mConsoleTargetPosition(0)
{
	Init();

#ifdef NO_LOG_DEBUG_INFO
	const Lepra::Log::LogLevel lAllowedLevel = Lepra::Log::LEVEL_INFO;
#else // Allow debug logging.
	const Lepra::Log::LogLevel lAllowedLevel = Lepra::Log::LEVEL_LOWEST_TYPE;
#endif // Disallow/allow debug logging.
	for (int x = lAllowedLevel; x < Lepra::Log::LEVEL_TYPE_COUNT; ++x)
	{
		Lepra::LogType::GetLog(Lepra::LogType::SUB_ROOT)->AddListener(GetConsoleLogger(), (Lepra::Log::LogLevel)x);
	}
}

ClientConsoleManager::~ClientConsoleManager()
{
	Join();
	delete (GetConsoleLogger());
	mUiManager = 0;
}



bool ClientConsoleManager::Start()
{
	InitGraphics();

	((UiTbc::ConsoleLogListener*)GetConsoleLogger())->SetOutputComponent(mConsoleOutput);
	((UiTbc::ConsolePrompt*)GetConsolePrompt())->SetInputComponent(mConsoleInput);

	OnConsoleChange();

	return (Parent::Start());
}

void ClientConsoleManager::Join()
{
	Parent::Join();

	((UiTbc::ConsoleLogListener*)GetConsoleLogger())->SetOutputComponent(0);
	((UiTbc::ConsolePrompt*)GetConsolePrompt())->SetInputComponent(0);
	CloseGraphics();
}



void ClientConsoleManager::SetRenderArea(const Lepra::PixelRect& pRenderArea)
{
	mArea = pRenderArea;

	if (mConsoleComponent)
	{
		Lepra::PixelCoords lSize = mArea.GetSize();
		lSize.y = (int)(lSize.y*0.6);	// TODO: use setting for how high console should be.
		mConsoleComponent->SetPreferredSize(lSize);
		int lInputHeight = mUiManager->GetPainter()->GetFontHeight()+4;
		lSize.y -= lInputHeight;
		mConsoleOutput->SetPreferredSize(lSize);
		lSize.y = lInputHeight;
		mConsoleInput->SetPreferredSize(lSize);
	}
}

bool ClientConsoleManager::Toggle()
{
	mIsConsoleActive = !mIsConsoleActive;
	OnConsoleChange();
	return (mIsConsoleActive);
}

void ClientConsoleManager::Tick()
{
	const double CONSOLE_SPEED = 0.3;
	if (mIsConsoleActive)
	{
		if (mArea.mTop == 0)	// Slide down.
		{
			mConsoleTargetPosition = Lepra::Math::Lerp(mConsoleTargetPosition, (double)mArea.mTop, CONSOLE_SPEED);
			mConsoleComponent->SetPos(mArea.mLeft, (int)mConsoleTargetPosition);
		}
		else	// Slide sideways.
		{
			mConsoleTargetPosition = Lepra::Math::Lerp(mConsoleTargetPosition, (double)mArea.mLeft, CONSOLE_SPEED);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
		}
	}
	else
	{
		const int lMargin = 3;
		if (mArea.mTop == 0)	// Slide out top.
		{
			const int lTarget = -mConsoleComponent->GetSize().y-lMargin;
			mConsoleTargetPosition = Lepra::Math::Lerp(mConsoleTargetPosition, (double)lTarget, CONSOLE_SPEED);
			mConsoleComponent->SetPos(mArea.mLeft, (int)mConsoleTargetPosition);
			if (mConsoleComponent->GetPos().y <= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
		else if (mArea.mLeft == 0)	// Slide out left.
		{
			const int lTarget = -mConsoleComponent->GetSize().x-lMargin;
			mConsoleTargetPosition = Lepra::Math::Lerp(mConsoleTargetPosition, (double)lTarget, CONSOLE_SPEED);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
			if (mConsoleComponent->GetPos().x <= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
		else	// Slide out right.
		{
			const int lTarget = mUiManager->GetDisplayManager()->GetWidth()+lMargin;
			mConsoleTargetPosition = Lepra::Math::Lerp(mConsoleTargetPosition, (double)lTarget, CONSOLE_SPEED);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
			if (mConsoleComponent->GetPos().x >= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
	}
}



bool ClientConsoleManager::SaveApplicationConfigFile(Lepra::File* pFile, const Lepra::UnicodeString& pUserConfig)
{
	bool lOk = Parent::SaveApplicationConfigFile(pFile, pUserConfig);
	if (lOk && pUserConfig.empty())
	{
		pFile->WriteString<wchar_t>(L"//push \"start-login server:port username password\"\n");
		lOk = true;	// TODO: check if all writes went well.
	}
	return (lOk);
}



void ClientConsoleManager::InitGraphics()
{
	CloseGraphics();

	mConsoleComponent = new UiTbc::Component(_T("CON:"), new UiTbc::ListLayout());
	mConsoleOutput = new UiTbc::TextArea(Lepra::Color(20, 20, 30, 150));
	mConsoleInput = new UiTbc::TextField(mConsoleComponent, Lepra::Color(20, 20, 30, 150), _T("CONI:"));

	SetRenderArea(mArea);

	mConsoleOutput->SetFocusAnchor(UiTbc::TextArea::ANCHOR_BOTTOM_LINE);
	mConsoleOutput->SetFontColor(Lepra::WHITE);
	mConsoleInput->SetFontColor(Lepra::WHITE);

	mConsoleComponent->AddChild(mConsoleOutput);
	mConsoleComponent->AddChild(mConsoleInput);

	mUiManager->GetDesktopWindow()->AddChild(mConsoleComponent, 0, 0, 1);
	mConsoleComponent->SetPos(mArea.mLeft, mArea.mTop);
	mConsoleComponent->SetVisible(false);

	// This is just for getting some basic metrics (such as font height).
	mConsoleComponent->Repaint(mUiManager->GetPainter());
}

void ClientConsoleManager::CloseGraphics()
{
	if (mUiManager && mConsoleComponent)
	{
		mUiManager->GetDesktopWindow()->RemoveChild(mConsoleComponent, 1);
		mConsoleComponent->RemoveChild(mConsoleOutput, 0);
		mConsoleComponent->RemoveChild(mConsoleInput, 0);
		mConsoleComponent->SetVisible(false);
	}

	delete (mConsoleComponent);
	mConsoleComponent = 0;
	delete (mConsoleOutput);
	mConsoleOutput = 0;
	delete (mConsoleInput);
	mConsoleInput = 0;
}

void ClientConsoleManager::OnConsoleChange()
{
	if (mIsConsoleActive)
	{
		mConsoleComponent->SetVisible(true);
		mUiManager->GetDesktopWindow()->UpdateLayout();
		GetConsolePrompt()->SetFocus(true);
		if (mIsFirstConsoleUse)
		{
			mIsFirstConsoleUse = false;
			PrintHelp();
		}
	}
	else
	{
		GetConsolePrompt()->SetFocus(false);
	}
}

void ClientConsoleManager::PrintHelp()
{
	Lepra::String lKeys = CURE_RTVAR_GET(GetVariableScope(), RTVAR_CTRL_UI_CONTOGGLE, _T("???"));
	typedef Lepra::StringUtility::StringVector SV;
	SV lKeyArray = Lepra::StringUtility::Split(lKeys, _T(", \t"));
	SV lNiceKeys;
	for (SV::iterator x = lKeyArray.begin(); x != lKeyArray.end(); ++x)
	{
		const Lepra::String lKey = Lepra::StringUtility::ReplaceAll(*x, _T("Key."), _T(""));
		lNiceKeys.push_back(lKey);
	}
	Lepra::String lKeyInfo;
	if (lKeyArray.size() == 1)
	{
		lKeyInfo = _T("key ");
	}
	else
	{
		lKeyInfo = _T("any of the following keys: ");
	}
	lKeyInfo += Lepra::StringUtility::Join(lNiceKeys, _T(", "));
	mLog.Infof(_T("To bring this console up again press %s."), lKeyInfo.c_str());
}



unsigned ClientConsoleManager::GetCommandCount() const
{
	return (Parent::GetCommandCount() + sizeof(mCommandIdList)/sizeof(mCommandIdList[0]));
}

const ClientConsoleManager::CommandPair& ClientConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int ClientConsoleManager::OnCommand(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector)
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
				Lepra::SystemManager::SetQuitRequest(true);
			}
			break;
			case COMMAND_BYE:
			{
				((GameClientSlaveManager*)mGameManager)->SetIsQuitting();
			}
			break;
			case COMMAND_START_LOGIN:
			{
				((GameClientSlaveManager*)mGameManager)->Logout();

				if (pParameterVector.size() == 3)
				{
					Lepra::UnicodeString lUsername = Lepra::UnicodeStringUtility::ToOwnCode(pParameterVector[1]);
					Lepra::UnicodeString lReadablePassword = Lepra::UnicodeStringUtility::ToOwnCode(pParameterVector[2]);
					//pParameterVector[2] = _T("        ");
					// Convert into login format.
					Cure::MangledPassword lPassword(lReadablePassword);
					lReadablePassword.clear();	// Clear out password traces in string.
					Cure::LoginId lLoginToken(lUsername, lPassword);
					((GameClientSlaveManager*)mGameManager)->RequestLogin(pParameterVector[0], lLoginToken);
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
				while (((GameClientSlaveManager*)mGameManager)->IsLoggingIn())
				{
					Lepra::Thread::Sleep(0.01);
				}
			}
			break;
			case COMMAND_LOGOUT:
			{
				mLog.AInfo("Logging off due to user command.");
				((GameClientSlaveManager*)mGameManager)->Logout();
			}
			break;
			case COMMAND_START_RESET_UI:
			{
				mLog.AInfo("Running UI restart...");
				if (((GameClientSlaveManager*)mGameManager)->GetMaster()->StartResetUi())
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
				if (((GameClientSlaveManager*)mGameManager)->GetMaster()->WaitResetUi())
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
				if (((GameClientSlaveManager*)mGameManager)->GetMaster()->CreateSlave())
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
			case 1001001:	// TODO: remove entirely when import is up and running!
			{
				mLog.AWarning("This is a command used for development purposes only. Don't use unless you know what you're doing!");
				if (pParameterVector.size() == 1 && pParameterVector[0] == _T("!"))
				{
					((GameClientSlaveManager*)mGameManager)->ExportAll(_T("."));
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



LOG_CLASS_DEFINE(CONSOLE, ClientConsoleManager);



}
