
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "HoverTankConsoleManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "HoverTankManager.h"
#include "RtVar.h"



namespace HoverTank
{



// Must lie before HoverTankConsoleManager to compile.
const HoverTankConsoleManager::CommandPair HoverTankConsoleManager::mCommandIdList[] =
{
	{_T("set-avatar"), COMMAND_SET_AVATAR},
};



HoverTankConsoleManager::HoverTankConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	Parent(pResourceManager, pGameManager, pUiManager, pVariableScope, pArea)
{
	InitCommands();
}

HoverTankConsoleManager::~HoverTankConsoleManager()
{
}



bool HoverTankConsoleManager::Start()
{
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // Touch
	return true;	// Touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned HoverTankConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const HoverTankConsoleManager::CommandPair& HoverTankConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int HoverTankConsoleManager::OnCommand(const str& pCommand, const strutil::strvec& pParameterVector)
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
					((HoverTankManager*)GetGameManager())->SelectAvatar(pParameterVector[0]);
				}
				else
				{
					mLog.Warningf(_T("usage: %s <avatar>"), pCommand.c_str());
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



loginstance(CONSOLE, HoverTankConsoleManager);



}
