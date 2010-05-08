
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientViewer.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "GameClientMasterTicker.h"
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
	CreateButton(-0.2f, +0.2f,  6.0f, _T("1"),	_T("road_sign"), _T("?.png;-1"), RoadSignButton::SHAPE_BOX);
	CreateButton(+0.2f, +0.2f,  6.0f, _T("2"),	_T("road_sign"), _T("?.png;-1"), RoadSignButton::SHAPE_BOX);
	CreateButton(-0.2f, -0.2f,  6.0f, _T("3"),	_T("road_sign"), _T("?.png;-1"), RoadSignButton::SHAPE_BOX);
	CreateButton(+0.2f, -0.2f,  6.0f, _T("4"),	_T("road_sign"), _T("?.png;-1"), RoadSignButton::SHAPE_BOX);
	CreateButton(+0.4f, +0.4f, 18.0f, _T("quit"),	_T("road_sign"), _T("?.png;-1"), RoadSignButton::SHAPE_ROUND);
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

void GameClientViewer::CreateButton(float x, float y, float z, const str& pName, const str& pClass, const str& pTexture, RoadSignButton::Shape pShape)
{
	RoadSignButton* lButton = new RoadSignButton(this, GetResourceManager(), mUiManager, pName, pClass, pTexture, pShape);
	GetContext()->AddLocalObject(lButton);
	lButton->SetTrajectory(Vector2DF(x, y), z);
	lButton->GetButton().SetOnClick(GameClientViewer, OnButtonClick);
	mRoadSignMap.insert(RoadSignMap::value_type(lButton->GetInstanceId(), lButton));
	lButton->StartLoading();
}

void GameClientViewer::OnButtonClick(UiTbc::Button* pButton)
{
	if (pButton->GetName() == _T("quit"))
	{
		GetMaster()->OnExit();
		return;
	}
	int lValue = 0;
	if (strutil::StringToInt(pButton->GetName(), lValue))
	{
		GetMaster()->OnSetPlayerCount(lValue);
	}
	else
	{
		assert(false);
	}
}



bool GameClientViewer::OnKeyDown(UiLepra::InputManager::KeyCode)
{
	return (false);
}

bool GameClientViewer::OnKeyUp(UiLepra::InputManager::KeyCode)
{
	return (false);
}

void GameClientViewer::OnInput(UiLepra::InputElement*)
{
}



}
