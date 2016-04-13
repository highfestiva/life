
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "FireConsoleManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "FireManager.h"
#include "RtVar.h"



namespace Fire
{



// Must lie before FireConsoleManager to compile.
const FireConsoleManager::CommandPair FireConsoleManager::mCommandIdList[] =
{
	{_T("prev-level"), COMMAND_PREV_LEVEL},
	{_T("next-level"), COMMAND_NEXT_LEVEL},
	{_T("set-level-index"), COMMAND_SET_LEVEL_INDEX},
};



FireConsoleManager::FireConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	Parent(pResourceManager, pGameManager, pUiManager, pVariableScope, pArea)
{
	InitCommands();
	SetSecurityLevel(1);
}

FireConsoleManager::~FireConsoleManager()
{
}

bool FireConsoleManager::Start()
{
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // Touch
	return true;	// Touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned FireConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const FireConsoleManager::CommandPair& FireConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int FireConsoleManager::OnCommand(const HashedString& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = Parent::OnCommand(pCommand, pParameterVector);
	if (lResult < 0)
	{
		lResult = 0;

		CommandClient lCommand = (CommandClient)TranslateCommand(pCommand);
		switch ((int)lCommand)
		{
			case COMMAND_PREV_LEVEL:
			{
				GetGameManager()->GetTickLock()->Acquire();
				((FireManager*)GetGameManager())->StepLevel(-1);
				GetGameManager()->GetTickLock()->Release();
				return 0;
			}
			break;
			case COMMAND_NEXT_LEVEL:
			{
				GetGameManager()->GetTickLock()->Acquire();
				((FireManager*)GetGameManager())->StepLevel(+1);
				GetGameManager()->GetTickLock()->Release();
				return 0;
			}
			break;
			case COMMAND_SET_LEVEL_INDEX:
			{
				int lTargetLevelIndex = -1;
				if (pParameterVector.size() == 1 && strutil::StringToInt(pParameterVector[0], lTargetLevelIndex))
				{
					GetGameManager()->GetTickLock()->Acquire();
					const int lLevelDelta = lTargetLevelIndex - ((FireManager*)GetGameManager())->GetCurrentLevelNumber();
					((FireManager*)GetGameManager())->StepLevel(lLevelDelta);
					GetGameManager()->GetTickLock()->Release();
					return 0;
				}
				else
				{
					mLog.Warningf(_T("usage: %s <index>"), pCommand.c_str());
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



loginstance(CONSOLE, FireConsoleManager);



}
