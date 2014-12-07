
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
	{_T("clear-phys"), COMMAND_CLEAR_PHYS},
	{_T("prep-phys-box"), COMMAND_PREP_PHYS_BOX},
	{_T("prep-phys-mesh"), COMMAND_PREP_PHYS_MESH},
	{_T("prep-gfx-mesh"), COMMAND_PREP_GFX_MESH},
	{_T("set-vertices"), COMMAND_SET_VERTICES},
	{_T("set-indices"), COMMAND_SET_INDICES},
};



std::vector<float> Strs2Flts(const strutil::strvec& pStrs)
{
	std::vector<float> lFlts;
	strutil::strvec::const_iterator x = pStrs.begin();
	for(; x != pStrs.end(); ++x)
	{
		double d = 0;
		strutil::StringToDouble(*x, d);
		lFlts.push_back((float)d);
	}
	return lFlts;
}



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
				((Tv3dManager*)GetGameManager())->CreateObject(mGfxMesh, mPhysObjects);
				GetGameManager()->GetTickLock()->Release();
			}
			break;
			case COMMAND_DELETE_OBJECTS:
			{
				GetGameManager()->GetTickLock()->Acquire();
				((Tv3dManager*)GetGameManager())->DeleteObjects();
				GetGameManager()->GetTickLock()->Release();
			}
			break;
			case COMMAND_CLEAR_PHYS:
			{
				PhysObjectArray::iterator x;
				for (x = mPhysObjects.begin(); x != mPhysObjects.end(); ++x)
				{
					delete *x;
				}
				mPhysObjects.clear();
			}
			break;
			case COMMAND_PREP_PHYS_BOX:
			{
				if (pParameterVector.size() != 10)
				{
					mLog.Warningf(_T("usage: %s followed by ten float arguments (quaternion, position, size)"), pCommand.c_str());
					return 1;
				}
				std::vector<float> lFloats = Strs2Flts(pParameterVector);
				PlacedObject* lBox = new BoxObject(quat(&lFloats[0]), vec3(&lFloats[4]), vec3(&lFloats[7]));
				mPhysObjects.push_back(lBox);
			}
			break;
			case COMMAND_PREP_PHYS_MESH:
			{
				if (pParameterVector.size() != 7)
				{
					mLog.Warningf(_T("usage: %s followed by seven float arguments (quaternion and position)"), pCommand.c_str());
					return 1;
				}
				std::vector<float> lFloats = Strs2Flts(pParameterVector);
				MeshObject* lMesh = new MeshObject(quat(&lFloats[0]), vec3(&lFloats[4]));
				lMesh->mVertices.insert(lMesh->mVertices.end(), mVertices.begin(), mVertices.end());
				lMesh->mIndices.insert(lMesh->mIndices.end(), mIndices.begin(), mIndices.end());
				mPhysObjects.push_back(lMesh);
			}
			break;
			case COMMAND_PREP_GFX_MESH:
			{
				if (pParameterVector.size() != 7)
				{
					mLog.Warningf(_T("usage: %s followed by seven float arguments (quaternion and position)"), pCommand.c_str());
					return 1;
				}
				std::vector<float> lFloats = Strs2Flts(pParameterVector);
				mGfxMesh = MeshObject(quat(&lFloats[0]), vec3(&lFloats[4]));
				mGfxMesh.mVertices.insert(mGfxMesh.mVertices.end(), mVertices.begin(), mVertices.end());
				mGfxMesh.mIndices.insert(mGfxMesh.mIndices.end(), mIndices.begin(), mIndices.end());
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
