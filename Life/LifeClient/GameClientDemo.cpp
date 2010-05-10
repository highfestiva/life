
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientDemo.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "GameClientMasterTicker.h"
#include "Vehicle.h"



namespace Life
{



GameClientDemo::GameClientDemo(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
	const PixelRect& pRenderArea):
	Parent(pMaster, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea)
{
}

GameClientDemo::~GameClientDemo()
{
}



void GameClientDemo::TickUiInput()
{
}

void GameClientDemo::TickUiUpdate()
{
	mCameraPreviousPosition = mCameraPosition;
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (!lObject)
	{
		return;
	}
	mCameraPivotPosition = lObject->GetPosition();
	mCameraPosition = mCameraPivotPosition - Vector3DF(10, 0, 0);
	mCameraPreviousPosition = mCameraPosition;
	mCameraOrientation = Vector3DF(0, PIF/2, 0);
}

void GameClientDemo::CreateLoginView()
{
}

bool GameClientDemo::InitializeTerrain()
{
	Cure::ContextObject* lVehicle = new Vehicle(GetResourceManager(), _T("saucer_01"), mUiManager);
	GetContext()->AddLocalObject(lVehicle);
	mAvatarId = lVehicle->GetInstanceId();
	lVehicle->SetPhysicsTypeOverride(Cure::ContextObject::PHYSICS_OVERRIDE_STATIC);
	lVehicle->StartLoading();
	return (true);
}

void GameClientDemo::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
	}
	else
	{
		Parent::OnLoadCompleted(pObject, pOk);
	}
}



bool GameClientDemo::OnKeyDown(UiLepra::InputManager::KeyCode)
{
	return (true);
}

bool GameClientDemo::OnKeyUp(UiLepra::InputManager::KeyCode)
{
	return (true);
}

void GameClientDemo::OnInput(UiLepra::InputElement*)
{
}



}
