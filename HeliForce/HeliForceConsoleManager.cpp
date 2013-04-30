
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



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
};



HeliForceConsoleManager::HeliForceConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	Parent(pResourceManager, pGameManager, pUiManager, pVariableScope, pArea)
{
	InitCommands();
}

HeliForceConsoleManager::~HeliForceConsoleManager()
{
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
