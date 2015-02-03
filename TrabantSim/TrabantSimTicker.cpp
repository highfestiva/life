
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Lepra/Include/LepraTarget.h"
#include "TrabantSimTicker.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiMusicPlayer.h"
#include "../UiCure/Include/UiParticleLoader.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../UiTbc/Include/GUI/UiFloatingLayout.h"
#include "../UiTbc/Include/UiParticleRenderer.h"
#include "RtVar.h"
#include "TrabantSimManager.h"



namespace TrabantSim
{



TrabantSimTicker::TrabantSimTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pUiManager, pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mEnvMap(0)
{
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMLOOKATX, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMLOOKATY, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMLOOKATZ, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMSLIDE, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMDISTANCE, 3.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMANGLEX, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMANGLEY, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMANGLEZ, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMROTATEX, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMROTATEY, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMROTATEZ, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMTARGETOBJECT, 0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CAMANGLERELATIVE, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_LIGHTANGLEX, -1.4);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_LIGHTANGLEZ, 0.1);
	v_override(UiCure::GetSettings(), RTVAR_UI_PENRED, 0.7);
	v_override(UiCure::GetSettings(), RTVAR_UI_PENGREEN, 0.3);
	v_override(UiCure::GetSettings(), RTVAR_UI_PENBLUE, 0.6);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLECLEAR, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARRED, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARGREEN, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARBLUE, 1.0);

	v_override(UiCure::GetSettings(), RTVAR_GAME_ALLOWPOWERDOWN, true);
	v_override(UiCure::GetSettings(), RTVAR_GAME_USERMESSAGE, _T(""));

	v_override(UiCure::GetSettings(), RTVAR_PHYSICS_FPS, 60);
	v_override(UiCure::GetSettings(), RTVAR_PHYSICS_ISFIXEDFPS, true);
	v_override(UiCure::GetSettings(), RTVAR_PHYSICS_BOUNCE, 0.2);
	v_override(UiCure::GetSettings(), RTVAR_PHYSICS_FRICTION, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_PHYSICS_GRAVITYX, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_PHYSICS_GRAVITYY, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_PHYSICS_GRAVITYZ, -9.8);
	v_override(UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, 30.0);
}

TrabantSimTicker::~TrabantSimTicker()
{
	CloseMainMenu();
	delete mEnvMap;
	mEnvMap = 0;
}



bool TrabantSimTicker::CreateSlave()
{
	return (Parent::CreateSlave(&TrabantSimTicker::CreateSlaveManager));
}

void TrabantSimTicker::OnSlavesKilled()
{
	CreateSlave();
}

void TrabantSimTicker::OnServerCreated(Life::UiGameServerManager*)
{
}

float TrabantSimTicker::GetPowerSaveAmount() const
{
	TrabantSimManager* lManager = (TrabantSimManager*)mSlaveArray[0];
	if (!lManager)
	{
		return 0;
	}
	return lManager->IsControlled()? Parent::GetPowerSaveAmount() : 0.2f;
}



bool TrabantSimTicker::OpenUiManager()
{
	bool lOk = true;
	if (lOk)
	{
		lOk = mUiManager->OpenDraw();
	}
	if (lOk)
	{
		mUiManager->GetDisplayManager()->SetCaption(_T("Trabant Simulator"));
		UiLepra::Core::ProcessMessages();
	}
	if (lOk)
	{
		mUiManager->UpdateSettings();
		UiTbc::Renderer* lRenderer = mUiManager->GetRenderer();
		lRenderer->AddDynamicRenderer(_T("particle"), new UiTbc::ParticleRenderer(lRenderer, 1));
		UiCure::ParticleLoader lLoader(mResourceManager, lRenderer, _T("explosion.png"), 4, 5);
	}
	if (lOk)
	{
		mEnvMap = new UiCure::RendererImageResource(mUiManager, mResourceManager, _T("env.png"), UiCure::ImageProcessSettings(Canvas::RESIZE_FAST, true));
		if (mEnvMap->Load())
		{
			if (mEnvMap->PostProcess() == Cure::RESOURCE_LOAD_COMPLETE)
			{
				UiTbc::Renderer::TextureID lTextureId = mEnvMap->GetUserData(0);
				mUiManager->GetRenderer()->SetEnvironmentMap(lTextureId);
			}
		}
	}
	if (lOk)
	{
		if (mUiManager->GetCanvas()->GetHeight() < 500)
		{
			double lFontHeight;
			v_get(lFontHeight, =, UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, 30.0);
			if (lFontHeight > 29.0)
			{
				lFontHeight *= mUiManager->GetCanvas()->GetHeight()/500.0;
				v_override(UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, lFontHeight);
			}
		}
		lOk = mUiManager->OpenRest();
	}
	if (lOk)
	{
		mUiManager->GetDesktopWindow()->CreateLayer(new UiTbc::FloatingLayout());
	}
	return lOk;
}

void TrabantSimTicker::BeginRender(vec3& pColor)
{
	Parent::BeginRender(pColor);
	mUiManager->GetRenderer()->SetOutlineFillColor(OFF_BLACK);
}



void TrabantSimTicker::CloseMainMenu()
{
}

bool TrabantSimTicker::QueryQuit()
{
	if (Parent::QueryQuit())
	{
		PrepareQuit();
		for (int x = 0; x < 4; ++x)
		{
			DeleteSlave(mSlaveArray[x], false);
		}
		DeleteServer();
		return (true);
	}
	return (false);
}



Life::GameClientSlaveManager* TrabantSimTicker::CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new TrabantSimManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}



loginstance(GAME, TrabantSimTicker);



}
