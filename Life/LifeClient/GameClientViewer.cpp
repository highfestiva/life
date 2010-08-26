
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientViewer.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiCenterLayout.h"
#include "../LifeServer/MasterServerConnection.h"
#include "GameClientMasterTicker.h"
#include "UiConsole.h"
#include "Vehicle.h"



namespace Life
{



GameClientViewer::GameClientViewer(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
	const PixelRect& pRenderArea):
	Parent(pMaster, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mServerListView(0)
{
	mCameraPosition = Vector3DF(-22, -5, 43.1f);
	mCameraOrientation = Vector3DF(-PIF*1.1f/2, PIF*0.86f/2, 0.05f);
}

GameClientViewer::~GameClientViewer()
{
	CloseJoinServerView();
}



void GameClientViewer::TickUiInput()
{
}

void GameClientViewer::TickUiUpdate()
{
	((ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();

	if (mServerListView)
	{
		mServerListView->Tick();
	}

	/*mCameraPreviousPosition = mCameraPosition;
	Cure::ContextObject* lObject = GetContext()->GetObject(mBackdropVehicleId);
	if (!lObject)
	{
		return;
	}
	mCameraPivotPosition = lObject->GetPosition();
	mCameraPosition = mCameraPivotPosition - Vector3DF(10, 0, 0);
	mCameraPreviousPosition = mCameraPosition;
	mCameraOrientation = Vector3DF(0, PIF/2, 0);*/
}

void GameClientViewer::CreateLoginView()
{
	QuaternionF lFlip;
	lFlip.RotateAroundOwnZ(PIF);
	CreateButton(-0.2f, +0.2f,  6.0f, _T("1"),	_T("road_sign_02"), _T("Data/road_sign_1p.png"), RoadSignButton::SHAPE_BOX);
	RoadSignButton* lButton = CreateButton(+0.2f, +0.2f,  6.0f, _T("2"),	_T("road_sign_02"), _T("Data/road_sign_2p.png"), RoadSignButton::SHAPE_BOX);
	lButton->SetOrientation(lFlip);
	CreateButton(-0.2f, -0.2f,  6.0f, _T("3"),	_T("road_sign_02"), _T("Data/road_sign_3p.png"), RoadSignButton::SHAPE_BOX);
	lButton = CreateButton(+0.2f, -0.2f,  6.0f, _T("4"),	_T("road_sign_02"), _T("Data/road_sign_4p.png"), RoadSignButton::SHAPE_BOX);
	lButton->SetOrientation(lFlip);

	CreateButton(-0.4f, +0.4f, 12.0f, _T("server"),	_T("road_sign_01"), _T("Data/road_sign_roundabout.png"), RoadSignButton::SHAPE_ROUND);

	CreateButton(+0.4f, +0.4f, 12.0f, _T("quit"),	_T("road_sign_01"), _T("Data/road_sign_nostop.png"), RoadSignButton::SHAPE_ROUND);
}

bool GameClientViewer::InitializeTerrain()
{
	if (!Parent::InitializeTerrain())
	{
		return (false);
	}

	Cure::ContextObject* lVehicle = new Vehicle(GetResourceManager(), _T("monster_02"), mUiManager);
	GetContext()->AddLocalObject(lVehicle);
	lVehicle->SetInitialTransform(TransformationF(QuaternionF(), Vector3DF(-23, -80, 33)));
	lVehicle->StartLoading();
	mAvatarId = lVehicle->GetInstanceId();
	GetConsoleManager()->ExecuteCommand(_T("fork execute-file Data/Steering.rec"));
	return (true);
}

void GameClientViewer::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
	}
	else
	{
		Parent::OnLoadCompleted(pObject, pOk);
	}
}

void GameClientViewer::OnCancelJoinServer()
{
	CloseJoinServerView();
}

void GameClientViewer::OnRequestJoinServer(const str& pServerAddress)
{
	GetVariableScope()->SetValue(Cure::RuntimeVariable::USAGE_NORMAL, _T(RTVAR_NETWORK_SERVERADDRESS), pServerAddress);
	mLog.Infof(_T("Will use server %s when logging in."), pServerAddress.c_str());
	CloseJoinServerView();
}

bool GameClientViewer::UpdateServerList(ServerInfoList& pServerList) const
{
	return GetMaster()->GetMasterConnection()->UpdateServerList(pServerList);
}

bool GameClientViewer::IsMasterServerConnectError() const
{
	return GetMaster()->GetMasterConnection()->IsConnectError();
}

void GameClientViewer::CloseJoinServerView()
{
	if (mServerListView)
	{
		mUiManager->GetDesktopWindow()->RemoveChild(mServerListView, 1);
		delete (mServerListView);
		mServerListView = 0;
	}
}

RoadSignButton* GameClientViewer::CreateButton(float x, float y, float z, const str& pName, const str& pClass, const str& pTexture, RoadSignButton::Shape pShape)
{
	RoadSignButton* lButton = new RoadSignButton(this, GetResourceManager(), mUiManager, pName, pClass, pTexture, pShape);
	GetContext()->AddLocalObject(lButton);
	lButton->SetTrajectory(Vector2DF(x, y), z);
	lButton->GetButton().SetOnClick(GameClientViewer, OnButtonClick);
	mRoadSignMap.insert(RoadSignMap::value_type(lButton->GetInstanceId(), lButton));
	lButton->StartLoading();
	return (lButton);
}

void GameClientViewer::OnButtonClick(UiTbc::Button* pButton)
{
	if (pButton->GetName() == _T("server"))
	{
		if (!mServerListView)
		{
			GetMaster()->DownloadServerList();
			mServerListView = new ServerListView(this);
			mUiManager->AssertDesktopLayout(new UiTbc::CenterLayout, 1);
			mUiManager->GetDesktopWindow()->AddChild(mServerListView, 0, 0, 1);
		}
		return;
	}
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



LOG_CLASS_DEFINE(GAME, GameClientViewer);



}
