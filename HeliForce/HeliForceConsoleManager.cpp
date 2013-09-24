
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "HeliForceConsoleManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "HeliForceManager.h"
#include "RtVar.h"



namespace HeliForce
{



// Must lie before HeliForceConsoleManager to compile.
const HeliForceConsoleManager::CommandPair HeliForceConsoleManager::mCommandIdList[] =
{
	{_T("set-avatar"), COMMAND_SET_AVATAR},
	{_T("prev-level"), COMMAND_PREV_LEVEL},
	{_T("next-level"), COMMAND_NEXT_LEVEL},
	{_T("die"), COMMAND_DIE},
};



HeliForceConsoleManager::HeliForceConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	Parent(pResourceManager, pGameManager, pUiManager, pVariableScope, pArea)
{
	InitCommands();
	SetSecurityLevel(1);
}

HeliForceConsoleManager::~HeliForceConsoleManager()
{
}

bool HeliForceConsoleManager::Start()
{
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // Touch
	return true;	// Touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned HeliForceConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const HeliForceConsoleManager::CommandPair& HeliForceConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int HeliForceConsoleManager::OnCommand(const str& pCommand, const strutil::strvec& pParameterVector)
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
					//((HeliForceManager*)GetGameManager())->SelectAvatar(pParameterVector[0]);
				}
				else
				{
					mLog.Warningf(_T("usage: %s <avatar>"), pCommand.c_str());
				}
			}
			break;
			case COMMAND_PREV_LEVEL:
			{
				GetGameManager()->GetTickLock()->Acquire();
				const str lNewLevelName = ((HeliForceManager*)GetGameManager())->StepLevel(-1);
				CURE_RTVAR_SET(((HeliForceManager*)GetGameManager())->GetVariableScope(), RTVAR_GAME_STARTLEVEL, lNewLevelName);
				GetGameManager()->GetTickLock()->Release();
				return OnCommand(_T("die"), pParameterVector);
			}
			break;
			case COMMAND_NEXT_LEVEL:
			{
				GetGameManager()->GetTickLock()->Acquire();
				const str lNewLevelName = ((HeliForceManager*)GetGameManager())->StepLevel(+1);
				if (!lNewLevelName.empty())
				{
					CURE_RTVAR_SET(((HeliForceManager*)GetGameManager())->GetVariableScope(), RTVAR_GAME_STARTLEVEL, lNewLevelName);
				}
				GetGameManager()->GetTickLock()->Release();
				return OnCommand(_T("die"), pParameterVector);
			}
			break;
			case COMMAND_DIE:
			{
				GetGameManager()->GetTickLock()->Acquire();
				const Cure::GameObjectId lAvatarId = ((HeliForceManager*)GetGameManager())->GetAvatarInstanceId();
				GetGameManager()->GetContext()->PostKillObject(lAvatarId);
				GetGameManager()->GetTickLock()->Release();
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



LOG_CLASS_DEFINE(CONSOLE, HeliForceConsoleManager);



}
