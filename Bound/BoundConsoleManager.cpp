
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "BoundConsoleManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "BoundManager.h"
#include "RtVar.h"



namespace Bound
{



// Must lie before BoundConsoleManager to compile.
const BoundConsoleManager::CommandPair BoundConsoleManager::mCommandIdList[] =
{
	{"step-level", COMMAND_STEP_LEVEL},
};



BoundConsoleManager::BoundConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	Parent(pResourceManager, pGameManager, pUiManager, pVariableScope, pArea)
{
	InitCommands();
	SetSecurityLevel(1);
}

BoundConsoleManager::~BoundConsoleManager()
{
}

bool BoundConsoleManager::Start()
{
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // Touch
	return true;	// Touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned BoundConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const BoundConsoleManager::CommandPair& BoundConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int BoundConsoleManager::OnCommand(const HashedString& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = Parent::OnCommand(pCommand, pParameterVector);
	if (lResult < 0)
	{
		lResult = 0;

		CommandClient lCommand = (CommandClient)TranslateCommand(pCommand);
		switch ((int)lCommand)
		{
			case COMMAND_STEP_LEVEL:
			{
				int lStep = 0;
				if (pParameterVector.size() == 1 && strutil::StringToInt(pParameterVector[0], lStep))
				{
					GetGameManager()->GetTickLock()->Acquire();
					((BoundManager*)GetGameManager())->StepLevel(lStep);
					GetGameManager()->GetTickLock()->Release();
					return 0;
				}
				return 1;
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



loginstance(CONSOLE, BoundConsoleManager);



}
