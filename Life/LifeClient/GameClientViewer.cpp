
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientViewer.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "RoadSignButton.h"
#include "Vehicle.h"



namespace Life
{



GameClientViewer::GameClientViewer(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
	const PixelRect& pRenderArea):
	Parent(pMaster, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mBackdropVehicleId(0)
{
}

GameClientViewer::~GameClientViewer()
{
}



void GameClientViewer::TickUiUpdate()
{
	((ClientConsoleManager*)GetConsoleManager())->Tick();

	mCameraPreviousPosition = mCameraPosition;
	Cure::ContextObject* lObject = GetContext()->GetObject(mBackdropVehicleId);
	if (!lObject)
	{
		return;
	}
	mCameraPivotPosition = lObject->GetPosition();
	mCameraPosition = mCameraPivotPosition - Vector3DF(10, 0, 0);
	mCameraPreviousPosition = mCameraPosition;
	mCameraOrientation = Vector3DF(0, PIF/2, 0);
}

void GameClientViewer::CreateLoginView()
{
	RoadSignButton* lButton = new RoadSignButton(this, GetResourceManager(), mUiManager, _T("?"), _T("road_sign"), _T("?.png;-1"), RoadSignButton::SHAPE_BOX);
	GetContext()->AddLocalObject(lButton);
	const float x = 0.3f;
	const float y = 0.3f;
	lButton->SetTrajectory(Vector2DF(x, y), 4);
	//lButton->GetButton().SetOnClick(GameClientSlaveManager, OnAvatarSelect);
	mRoadSignMap.insert(RoadSignMap::value_type(lButton->GetInstanceId(), lButton));
	lButton->StartLoading();
}

bool GameClientViewer::InitializeTerrain()
{
	Cure::ContextObject* lVehicle = new Vehicle(GetResourceManager(), _T("monster_02"), mUiManager);
	GetContext()->AddLocalObject(lVehicle);
	lVehicle->SetPhysicsTypeOverride(Cure::ContextObject::PHYSICS_OVERRIDE_STATIC);
	lVehicle->StartLoading();
	return (true);
}

void GameClientViewer::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		pObject->GetPhysics()->EnableGravity(GetPhysicsManager(), false);
		if (dynamic_cast<Vehicle*>(pObject))
		{
			mBackdropVehicleId = pObject->GetInstanceId();
		}
	}
	else
	{
		Parent::OnLoadCompleted(pObject, pOk);
	}
}



}
