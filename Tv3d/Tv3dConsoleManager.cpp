
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Tv3dConsoleManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "Tv3dManager.h"
#include "RtVar.h"



namespace Tv3d
{



// Must lie before Tv3dConsoleManager to compile.
const Tv3dConsoleManager::CommandPair Tv3dConsoleManager::mCommandIdList[] =
{
	{_T("create-object"), COMMAND_CREATE_OBJECT},
	{_T("delete-objects"), COMMAND_DELETE_OBJECTS},
	{_T("clear-phys", COMMAND_CLEAR_PHYS},
	{_T("prep-phys-box", COMMAND_PREP_PHYS_BOX},
	{_T("prep-phys-mesh"), COMMAND_PREP_PHYS_MESH},
	{_T("prep-gfx-mesh"), COMMAND_PREP_GFX_MESH},
	{_T("set-vertices"), COMMAND_SET_VERTICES},
	{_T("set-indices"), COMMAND_SET_INDICES},
};



Tv3dConsoleManager::Tv3dConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea):
	Parent(pResourceManager, pGameManager, pUiManager, pVariableScope, pArea)
{
	InitCommands();
	SetSecurityLevel(1);
}

Tv3dConsoleManager::~Tv3dConsoleManager()
{
}

bool Tv3dConsoleManager::Start()
{
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // Touch
	return true;	// Touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned Tv3dConsoleManager::GetCommandCount() const
{
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(mCommandIdList);
}

const Tv3dConsoleManager::CommandPair& Tv3dConsoleManager::GetCommand(unsigned pIndex) const
{
	if (pIndex < Parent::GetCommandCount())
	{
		return (Parent::GetCommand(pIndex));
	}
	return (mCommandIdList[pIndex-Parent::GetCommandCount()]);
}

int Tv3dConsoleManager::OnCommand(const str& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = Parent::OnCommand(pCommand, pParameterVector);
	if (lResult < 0)
	{
		lResult = 0;

		CommandClient lCommand = (CommandClient)TranslateCommand(pCommand);
		switch ((int)lCommand)
		{
			case COMMAND_CREATE_OBJECT:
			{
				GetGameManager()->GetTickLock()->Acquire();
				((Tv3dManager*)GetGameManager())->CreateObject(mGfxMesh, mPhysMeshes);
				GetGameManager()->GetTickLock()->Release();
				return 1;
			}
			break;
			case COMMAND_DELETE_OBJECTS:
			{
				GetGameManager()->GetTickLock()->Acquire();
				((Tv3dManager*)GetGameManager())->DeleteObjects();
				GetGameManager()->GetTickLock()->Release();
				return 1;
			}
			break;
			case COMMAND_CLEAR_PHYS:
			{
				return 1;
			}
			break;
			case COMMAND_PREP_PHYS_BOX:
			{
				return 1;
			}
			break;
			case COMMAND_PREP_PHYS_MESH:
			{
				return 1;
			}
			break;
			case COMMAND_PREP_GFX_MESH:
			{
				return 1;
			}
			break;
			case COMMAND_SET_VERTICES:
			{
				mVertices.clear();
				double d = 0;
				strutil::strvec::const_iterator p;
				for (p = pParameterVector.begin(); p != pParameterVector.end(); ++p)
				{
					strutil::StringToDouble(*p, d);
					mVertices.push_back((float)d);
				}
				return 1;
			}
			break;
			case COMMAND_SET_INDICES:
			{
				mIndices.clear();
				int i = 0;
				strutil::strvec::const_iterator p;
				for (p = pParameterVector.begin(); p != pParameterVector.end(); ++p)
				{
					strutil::StringToInt(*p, i);
					mIndices.push_back(i);
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



loginstance(CONSOLE, Tv3dConsoleManager);



}
