
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#include "FireTicker.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiMusicPlayer.h"
#include "../UiCure/Include/UiParticleLoader.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "RtVar.h"
#include "FireManager.h"



namespace Fire
{



FireTicker::FireTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pUiManager, pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mEnvMap(0)
{
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_PHYSICS_ISFIXEDFPS, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, 48.0);

	AddBackedRtvar(_T(RTVAR_GAME_FIRSTRUN));
	AddBackedRtvar(_T(RTVAR_GAME_STARTLEVEL));
	AddBackedRtvar(_T(RTVAR_UI_SOUND_MASTERVOLUME));
}

FireTicker::~FireTicker()
{
	CloseMainMenu();
	delete mEnvMap;
	mEnvMap = 0;
}



void FireTicker::Suspend()
{
	Parent::Suspend();
}

void FireTicker::Resume()
{
	Parent::Resume();
}

bool FireTicker::CreateSlave()
{
	return (Parent::CreateSlave(&FireTicker::CreateSlaveManager));
}

void FireTicker::OnSlavesKilled()
{
	CreateSlave();
}

void FireTicker::OnServerCreated(Life::UiGameServerManager*)
{
}



bool FireTicker::OpenUiManager()
{
	bool lOk = true;
	if (lOk)
	{
		lOk = mUiManager->OpenDraw();
	}
	if (lOk)
	{
		UiLepra::Core::ProcessMessages();
	}
	if (lOk)
	{
		mUiManager->UpdateSettings();
		UiTbc::Renderer* lRenderer = mUiManager->GetRenderer();
		lRenderer->AddDynamicRenderer(_T("particle"), new UiTbc::ParticleRenderer(lRenderer, 0));
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
		if (mUiManager->GetCanvas()->GetHeight() < 600)
		{
			double lFontHeight;
			CURE_RTVAR_GET(lFontHeight, =, UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, 48.0);
			if (lFontHeight > 47.0)
			{
				lFontHeight *= mUiManager->GetCanvas()->GetHeight()/600.0;
				CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, lFontHeight);
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

void FireTicker::BeginRender(Vector3DF& pColor)
{
	Parent::BeginRender(pColor);
	mUiManager->GetRenderer()->SetOutlineFillColor(OFF_BLACK);
}

void FireTicker::PreWaitPhysicsTick()
{
	Parent::PreWaitPhysicsTick();
}



void FireTicker::CloseMainMenu()
{
}

bool FireTicker::QueryQuit()
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



Life::GameClientSlaveManager* FireTicker::CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new FireManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}



LOG_CLASS_DEFINE(GAME, FireTicker);



}
