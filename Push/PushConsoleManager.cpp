
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "PushConsoleManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "PushManager.h"
#include "RtVar.h"



namespace Push
{



// Must lie before PushConsoleManager to compile.
const PushConsoleManager::CommandPair PushConsoleManager::mCommandIdList[] =
{
	{_T("set-avatar"), COMMAND_SET_AVATAR},
	{_T("set-avatar-engine-power"), COMMAND_SET_AVATAR_ENGINE_POWER},
#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS)
	{_T("builddata"), COMMAND_BUILD_DATA},
#endif // Debug & Windows
};



PushConsoleManager::PushConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	Parent(pResourceManager, pGameManager, pUiManager, pVariableScope, pArea)
{
	InitCommands();
}

PushConsoleManager::~PushConsoleManager()
{
}



unsigned PushConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const PushConsoleManager::CommandPair& PushConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int PushConsoleManager::OnCommand(const str& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = Parent::OnCommand(pCommand, pParameterVector);
	if (lResult < 0)
	{
		lResult = 0;

		CommandClient lCommand = (CommandClient)TranslateCommand(pCommand);
		switch ((int)lCommand)
		{
			case COMMAND_SET_AVATAR:
			{
				if (pParameterVector.size() == 1)
				{
					((PushManager*)GetGameManager())->SelectAvatar(pParameterVector[0]);
				}
				else
				{
					mLog.Warningf(_T("usage: %s <avatar>"), pCommand.c_str());
				}
			}
			break;
			case COMMAND_SET_AVATAR_ENGINE_POWER:
			{
				if (pParameterVector.size() == 2)
				{
					log_adebug("Setting avatar engine power.");
					int lAspect = 0;
					strutil::StringToInt(pParameterVector[0], lAspect);
					double lPower;
					strutil::StringToDouble(pParameterVector[1], lPower);
					if (!((PushManager*)GetGameManager())->SetAvatarEnginePower(lAspect, (float)lPower))
					{
						mLog.AError("Could not set avatar engine power!");
						lResult = 1;
					}
				}
				else
				{
					mLog.Warningf(_T("usage: %s <aspect> <power>"), pCommand.c_str());
				}
			}
			break;
#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS)
			case COMMAND_BUILD_DATA:
			{
				const Cure::GameObjectId lAvatarId = ((PushManager*)GetGameManager())->GetAvatarInstanceId();
				if (!lAvatarId)
				{
					break;
				}
				const str lAvatarType = GetGameManager()->GetContext()->GetObject(lAvatarId)->GetClassId();
				((PushManager*)GetGameManager())->Logout();
				Thread::Sleep(0.5);
				GetResourceManager()->ForceFreeCache();
				const str lCurrentDir = SystemManager::GetCurrentDirectory();
				::SetCurrentDirectoryA(astrutil::Encode(Path::GetParentDirectory(lCurrentDir)).c_str());
				::system("c:/Program/Python31/python.exe -B Tools/build/rgo.py builddata");
				::SetCurrentDirectoryA(astrutil::Encode(lCurrentDir).c_str());
				const str lUserName = CURE_RTVAR_SLOW_GET(GetVariableScope(), RTVAR_LOGIN_USERNAME, EmptyString);
				const str lServer = CURE_RTVAR_SLOW_GET(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, EmptyString);
				wstr lPw(L"CarPassword");
				const Cure::LoginId lLoginId(wstrutil::Encode(lUserName), Cure::MangledPassword(lPw));
				((PushManager*)GetGameManager())->RequestLogin(lServer, lLoginId);
				((PushManager*)GetGameManager())->ToggleConsole();
				ExecuteCommand(_T("wait-login"));
				((PushManager*)GetGameManager())->SelectAvatar(lAvatarType);
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



LOG_CLASS_DEFINE(CONSOLE, PushConsoleManager);



}
