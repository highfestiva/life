
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "HoverTankViewer.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Tbc/Include/ChunkyPhysics.h"
#include "../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../UiTbc/Include/GUI/UiCenterLayout.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiMachine.h"
#include "../Life/LifeClient/ClientConsoleManager.h"
#include "../Life/LifeClient/GameClientMasterTicker.h"
#include "../Life/LifeClient/Level.h"
#include "../Life/LifeClient/UiConsole.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "RtVar.h"



namespace HoverTank
{



HoverTankViewer::HoverTankViewer(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mServerListView(0)
{
	mCameraPosition = vec3(-22, -5, 43.1f);
	mCameraOrientation = vec3(-PIF*1.1f/2, PIF*0.86f/2, 0.05f);
}

HoverTankViewer::~HoverTankViewer()
{
	CloseJoinServerView();
}



void HoverTankViewer::LoadSettings()
{
	v_set(GetVariableScope(), RTVAR_DEBUG_INPUT_PRINT, false);

	v_internal(GetVariableScope(), RTVAR_GAME_DRAWSCORE, false);
	v_internal(GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	v_internal(GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	v_internal(GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
	v_internal(GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
}

void HoverTankViewer::SaveSettings()
{
}

bool HoverTankViewer::Open()
{
	return Life::GameClientSlaveManager::Open();
}

void HoverTankViewer::TickUiInput()
{
}

void HoverTankViewer::TickUiUpdate()
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
	mCameraPosition = mCameraPivotPosition - vec3(10, 0, 0);
	mCameraPreviousPosition = mCameraPosition;
	mCameraOrientation = vec3(0, PIF/2, 0);*/
}

void HoverTankViewer::CreateLoginView()
{
	quat lFlip;
	lFlip.RotateAroundOwnZ(PIF);
	CreateButton(-0.2f, +0.2f,  6.0f, "1",	"road_sign_02", "road_sign_1p.png", RoadSignButton::SHAPE_BOX);
	RoadSignButton* lButton = CreateButton(+0.2f, +0.2f,  6.0f, "2",	"road_sign_02", "road_sign_2p.png", RoadSignButton::SHAPE_BOX);
	lButton->SetOrientation(lFlip);
	CreateButton(-0.2f, -0.2f,  6.0f, "3",	"road_sign_02", "road_sign_3p.png", RoadSignButton::SHAPE_BOX);
	lButton = CreateButton(+0.2f, -0.2f,  6.0f, "4",	"road_sign_02", "road_sign_4p.png", RoadSignButton::SHAPE_BOX);
	lButton->SetOrientation(lFlip);

	CreateButton(-0.4f, +0.4f, 12.0f, "server",	"road_sign_01", "road_sign_roundabout.png", RoadSignButton::SHAPE_ROUND);

	CreateButton(+0.4f, +0.4f, 12.0f, "quit",	"road_sign_01", "road_sign_nostop.png", RoadSignButton::SHAPE_ROUND);
}

bool HoverTankViewer::InitializeUniverse()
{
	if (!Parent::InitializeUniverse())
	{
		return (false);
	}

	UiCure::GravelEmitter* lGravelParticleEmitter = new UiCure::GravelEmitter(GetResourceManager(), mUiManager, 0.5f, 1, 10, 2);
	mLevel = new Life::Level(GetResourceManager(), "level_02", mUiManager, lGravelParticleEmitter);
	AddContextObject(mLevel, Cure::NETWORK_OBJECT_REMOTE_CONTROLLED, 0);
	mLevel->EnableRootShadow(false);
	mLevel->SetAllowNetworkLogic(false);
	mLevel->StartLoading();

	Cure::ContextObject* lVehicle = new UiCure::Machine(GetResourceManager(), "hover_tank_01", mUiManager);
	GetContext()->AddLocalObject(lVehicle);
	lVehicle->SetInitialTransform(xform(gIdentityQuaternionF, vec3(-23, -80, 53)));
	lVehicle->StartLoading();
	mAvatarId = lVehicle->GetInstanceId();
	GetConsoleManager()->ExecuteCommand("fork execute-file Data/Steering.rec");
	return (true);
}

void HoverTankViewer::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
	}
	else
	{
		Parent::OnLoadCompleted(pObject, pOk);
	}
}

void HoverTankViewer::OnCancelJoinServer()
{
	CloseJoinServerView();
}

void HoverTankViewer::OnRequestJoinServer(const str& pServerAddress)
{
	v_set(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, pServerAddress);
	v_internal(UiCure::GetSettings(), RTVAR_LOGIN_ISSERVERSELECTED, true);
	mLog.Infof("Will use server %s when logging in.", pServerAddress.c_str());
	CloseJoinServerView();
}

bool HoverTankViewer::UpdateServerList(Life::ServerInfoList& pServerList) const
{
	if (GetMaster()->GetMasterServerConnection())	// TRICKY: uses master ticker's connection, as this is the one that downloads server lists!
	{
		return GetMaster()->GetMasterServerConnection()->UpdateServerList(pServerList);
	}
	return false;
}

bool HoverTankViewer::IsMasterServerConnectError() const
{
	if (GetMaster()->GetMasterServerConnection())	// TRICKY: uses master ticker's connection, as this is the one that downloads server lists!
	{
		return GetMaster()->GetMasterServerConnection()->IsConnectError();
	}
	return true;
}

void HoverTankViewer::CloseJoinServerView()
{
	if (mServerListView)
	{
		mUiManager->GetDesktopWindow()->RemoveChild(mServerListView, 1);
		delete (mServerListView);
		mServerListView = 0;
	}
}

RoadSignButton* HoverTankViewer::CreateButton(float x, float y, float z, const str& pName, const str& pClass, const str& pTexture, RoadSignButton::Shape pShape)
{
	RoadSignButton* lButton = new RoadSignButton(this, GetResourceManager(), mUiManager, pName, pClass, pTexture, pShape);
	GetContext()->AddLocalObject(lButton);
	lButton->SetTrajectory(vec2(x, y), z);
	lButton->GetButton().SetOnClick(HoverTankViewer, OnButtonClick);
	mRoadSignMap.insert(RoadSignMap::value_type(lButton->GetInstanceId(), lButton));
	lButton->StartLoading();
	return (lButton);
}

void HoverTankViewer::OnButtonClick(UiTbc::Button* pButton)
{
	if (pButton->GetName() == "server")
	{
		if (!mServerListView)
		{
			GetMaster()->DownloadServerList();
			mServerListView = new ServerListView(this);
			mUiManager->AssertDesktopLayout(new UiTbc::CenterLayout, 1);
			mUiManager->GetDesktopWindow()->AddChild(mServerListView, 0, 0, 1);
		}
		v_set(GetVariableScope(), RTVAR_NETWORK_ENABLEONLINEMASTER, true);
		return;
	}
	if (pButton->GetName() == "quit")
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



loginstance(GAME, HoverTankViewer);



}
