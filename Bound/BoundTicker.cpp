
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Lepra/Include/LepraTarget.h"
#include "BoundTicker.h"
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
#include "BoundManager.h"



namespace Bound
{



BoundTicker::BoundTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pUiManager, pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mIsPlayerCountViewActive(false),
	mMusicPlayer(0),
	mEnvMap(0)
{
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, false);
	v_set(UiCure::GetSettings(), RTVAR_PHYSICS_FPS, 45);
	v_set(UiCure::GetSettings(), RTVAR_PHYSICS_ISFIXEDFPS, true);
	v_set(UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, 30.0);

	AddBackedRtvar(RTVAR_GAME_FIRSTTIME);
	AddBackedRtvar(RTVAR_GAME_LEVEL);
	AddBackedRtvar(RTVAR_GAME_LEVELSHAPEALTERNATE);
	AddBackedRtvar(RTVAR_GAME_RUNADS);
	AddBackedRtvar(RTVAR_GAME_SCORE);
}

BoundTicker::~BoundTicker()
{
	CloseMainMenu();
	delete mMusicPlayer;
	mMusicPlayer = 0;
	delete mEnvMap;
	mEnvMap = 0;
}



void BoundTicker::Suspend(bool pHard)
{
	Parent::Suspend(pHard);

	if (mMusicPlayer)
	{
		mMusicPlayer->Pause();
	}
}

void BoundTicker::Resume(bool pHard)
{
	Parent::Resume(pHard);

	if (mMusicPlayer)
	{
		mMusicPlayer->Stop();
		mMusicPlayer->Playback();
	}
}

bool BoundTicker::CreateSlave()
{
	return (Parent::CreateSlave(&BoundTicker::CreateSlaveManager));
}

void BoundTicker::OnSlavesKilled()
{
	CreateSlave();
}

void BoundTicker::OnServerCreated(Life::UiGameServerManager*)
{
}



bool BoundTicker::OpenUiManager()
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
		lRenderer->AddDynamicRenderer("particle", new UiTbc::ParticleRenderer(lRenderer, 0));
		UiCure::ParticleLoader lLoader(mResourceManager, lRenderer, "explosion.png", 4, 5);
	}
	if (lOk)
	{
		mEnvMap = new UiCure::RendererImageResource(mUiManager, mResourceManager, "env.png", UiCure::ImageProcessSettings(Canvas::RESIZE_FAST, true));
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
			v_get(lFontHeight, =, UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, 30.0);
			if (lFontHeight > 29.0)
			{
				lFontHeight *= mUiManager->GetCanvas()->GetHeight()/600.0;
				v_set(UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, lFontHeight);
			}
		}
		lOk = mUiManager->OpenRest();
	}
	if (lOk)
	{
		mMusicPlayer = new UiCure::MusicPlayer(mUiManager->GetSoundManager());
		mMusicPlayer->SetVolume(0.5f);
		mMusicPlayer->SetSongPauseTime(2, 6);
		/*mMusicPlayer->AddSong("Dub Feral.ogg");
		mMusicPlayer->AddSong("Easy Jam.ogg");
		mMusicPlayer->AddSong("Firmament.ogg");
		mMusicPlayer->AddSong("Mandeville.ogg");
		mMusicPlayer->AddSong("Slow Ska Game Loop.ogg");
		mMusicPlayer->AddSong("Stealth Groover.ogg");
		mMusicPlayer->AddSong("Yallahs.ogg");*/
		mMusicPlayer->Shuffle();
		lOk = mMusicPlayer->Playback();
	}
	if (lOk)
	{
		mUiManager->GetDesktopWindow()->CreateLayer(new UiTbc::FloatingLayout());
	}
	return lOk;
}

void BoundTicker::BeginRender(vec3& pColor)
{
	Parent::BeginRender(pColor);
	mUiManager->GetRenderer()->SetOutlineFillColor(OFF_BLACK);
}

void BoundTicker::PreWaitPhysicsTick()
{
	Parent::PreWaitPhysicsTick();
	if (mMusicPlayer)
	{
		mMusicPlayer->Update();
	}
}



void BoundTicker::CloseMainMenu()
{
	if (mIsPlayerCountViewActive)
	{
		DeleteSlave(mSlaveArray[0], false);
		mIsPlayerCountViewActive = false;
	}
}

bool BoundTicker::QueryQuit()
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



Life::GameClientSlaveManager* BoundTicker::CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new BoundManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}



loginstance(GAME, BoundTicker);



}
