
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "PushViewer.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../TBC/Include/ChunkyPhysics.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiCenterLayout.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiMachine.h"
#include "../Life/LifeClient/ClientConsoleManager.h"
#include "../Life/LifeClient/GameClientMasterTicker.h"
#include "../Life/LifeClient/Level.h"
#include "../Life/LifeClient/UiConsole.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "RtVar.h"



namespace Push
{



PushViewer::PushViewer(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mServerListView(0)
{
	mCameraPosition = Vector3DF(-22, -5, 43.1f);
	mCameraOrientation = Vector3DF(-PIF*1.1f/2, PIF*0.86f/2, 0.05f);
}

PushViewer::~PushViewer()
{
	CloseJoinServerView();
}



void PushViewer::LoadSettings()
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_DEBUG_INPUT_PRINT, false);

	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
}

void PushViewer::SaveSettings()
{
}

bool PushViewer::Open()
{
	return Life::GameClientSlaveManager::Open();
}

void PushViewer::TickUiInput()
{
}

void PushViewer::TickUiUpdate()
{
	((Life::ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();

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

void PushViewer::CreateLoginView()
{
	QuaternionF lFlip;
	lFlip.RotateAroundOwnZ(PIF);
	CreateButton(-0.2f, +0.2f,  6.0f, _T("1"),	_T("road_sign_02"), _T("road_sign_1p.png"), RoadSignButton::SHAPE_BOX);
	RoadSignButton* lButton = CreateButton(+0.2f, +0.2f,  6.0f, _T("2"),	_T("road_sign_02"), _T("road_sign_2p.png"), RoadSignButton::SHAPE_BOX);
	lButton->SetOrientation(lFlip);
	CreateButton(-0.2f, -0.2f,  6.0f, _T("3"),	_T("road_sign_02"), _T("road_sign_3p.png"), RoadSignButton::SHAPE_BOX);
	lButton = CreateButton(+0.2f, -0.2f,  6.0f, _T("4"),	_T("road_sign_02"), _T("road_sign_4p.png"), RoadSignButton::SHAPE_BOX);
	lButton->SetOrientation(lFlip);

	CreateButton(-0.4f, +0.4f, 12.0f, _T("server"),	_T("road_sign_01"), _T("road_sign_roundabout.png"), RoadSignButton::SHAPE_ROUND);

	CreateButton(+0.4f, +0.4f, 12.0f, _T("quit"),	_T("road_sign_01"), _T("road_sign_nostop.png"), RoadSignButton::SHAPE_ROUND);
}

bool PushViewer::InitializeUniverse()
{
	if (!Parent::InitializeUniverse())
	{
		return (false);
	}

	UiCure::GravelEmitter* lGravelParticleEmitter = new UiCure::GravelEmitter(GetResourceManager(), mUiManager, 0.5f, 1, 10, 2);
	mLevel = new Life::Level(GetResourceManager(), _T("level_01"), mUiManager, lGravelParticleEmitter);
	AddContextObject(mLevel, Cure::NETWORK_OBJECT_REMOTE_CONTROLLED, 0);
	mLevel->DisableRootShadow();
	mLevel->SetAllowNetworkLogic(false);
	mLevel->StartLoading();

	Cure::ContextObject* lVehicle = new UiCure::Machine(GetResourceManager(), _T("monster_02"), mUiManager);
	GetContext()->AddLocalObject(lVehicle);
	lVehicle->SetInitialTransform(TransformationF(gIdentityQuaternionF, Vector3DF(-23, -80, 53)));
	lVehicle->StartLoading();
	mAvatarId = lVehicle->GetInstanceId();
	GetConsoleManager()->ExecuteCommand(_T("fork execute-file Data/Steering.rec"));
	return (true);
}

void PushViewer::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
	}
	else
	{
		Parent::OnLoadCompleted(pObject, pOk);
	}
}

void PushViewer::OnCancelJoinServer()
{
	CloseJoinServerView();
}

void PushViewer::OnRequestJoinServer(const str& pServerAddress)
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, pServerAddress);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_LOGIN_ISSERVERSELECTED, true);
	mLog.Infof(_T("Will use server %s when logging in."), pServerAddress.c_str());
	CloseJoinServerView();
}

bool PushViewer::UpdateServerList(Life::ServerInfoList& pServerList) const
{
	if (GetMaster()->GetMasterServerConnection())	// TRICKY: uses master ticker's connection, as this is the one that downloads server lists!
	{
		return GetMaster()->GetMasterServerConnection()->UpdateServerList(pServerList);
	}
	return false;
}

bool PushViewer::IsMasterServerConnectError() const
{
	if (GetMaster()->GetMasterServerConnection())	// TRICKY: uses master ticker's connection, as this is the one that downloads server lists!
	{
		return GetMaster()->GetMasterServerConnection()->IsConnectError();
	}
	return true;
}

void PushViewer::CloseJoinServerView()
{
	if (mServerListView)
	{
		mUiManager->GetDesktopWindow()->RemoveChild(mServerListView, 1);
		delete (mServerListView);
		mServerListView = 0;
	}
}

RoadSignButton* PushViewer::CreateButton(float x, float y, float z, const str& pName, const str& pClass, const str& pTexture, RoadSignButton::Shape pShape)
{
	RoadSignButton* lButton = new RoadSignButton(this, GetResourceManager(), mUiManager, pName, pClass, pTexture, pShape);
	GetContext()->AddLocalObject(lButton);
	lButton->SetTrajectory(Vector2DF(x, y), z);
	lButton->GetButton().SetOnClick(PushViewer, OnButtonClick);
	mRoadSignMap.insert(RoadSignMap::value_type(lButton->GetInstanceId(), lButton));
	lButton->StartLoading();
	return (lButton);
}

void PushViewer::OnButtonClick(UiTbc::Button* pButton)
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
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_ENABLEONLINEMASTER, true);
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
		deb_assert(false);
	}
}



LOG_CLASS_DEFINE(GAME, PushViewer);



}
