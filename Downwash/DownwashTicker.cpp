
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Lepra/Include/LepraTarget.h"
#include "DownwashTicker.h"
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
#include "DownwashManager.h"



namespace Downwash
{



DownwashTicker::DownwashTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pUiManager, pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mIsPlayerCountViewActive(false),
	mMusicPlayer(0),
	mEnvMap(0)
{
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, false);
	v_set(UiCure::GetSettings(), RTVAR_PHYSICS_ISFIXEDFPS, true);
	v_set(UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, 30.0);

	AddBackedRtvar(_T(RTVAR_GAME_STARTLEVEL));
	AddBackedRtvar(_T(RTVAR_GAME_CHILDISHNESS));
	for (int x = 0; x < 20; ++x)
	{
		const str wr = strutil::Format(str(_T(RTVAR_GAME_WORLDRECORD_LEVEL) _T("_%i")).c_str(), x);
		AddBackedRtvar(wr);
		const str pr = strutil::Format(str(_T(RTVAR_GAME_PERSONALRECORD_LEVEL) _T("_%i")).c_str(), x);
		AddBackedRtvar(pr);
	}
	AddBackedRtvar(_T(RTVAR_PHYSICS_RTR_OFFSET));
	AddBackedRtvar(_T(RTVAR_GAME_ALLOWTOYMODE));
	AddBackedRtvar(_T(RTVAR_GAME_PILOTNAME));
	AddBackedRtvar(_T(RTVAR_UI_SOUND_MASTERVOLUME));
}

DownwashTicker::~DownwashTicker()
{
	CloseMainMenu();
	delete mMusicPlayer;
	mMusicPlayer = 0;
	delete mEnvMap;
	mEnvMap = 0;
}



void DownwashTicker::Suspend()
{
	Parent::Suspend();

	if (mMusicPlayer)
	{
		mMusicPlayer->Pause();
	}
}

void DownwashTicker::Resume()
{
	Parent::Resume();

	double lRtrOffset;
	v_get(lRtrOffset, =, mSlaveArray[0]->GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
	v_set(mSlaveArray[0]->GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0+lRtrOffset);

	if (mMusicPlayer)
	{
		mMusicPlayer->Stop();
		mMusicPlayer->Playback();
	}
}

bool DownwashTicker::CreateSlave()
{
	return (Parent::CreateSlave(&DownwashTicker::CreateSlaveManager));
}

void DownwashTicker::OnSlavesKilled()
{
	/*DeleteServer();
	//deb_assert(!mIsPlayerCountViewActive);
	mIsPlayerCountViewActive = true;
	mSlaveTopSplit = 1.0f;
	Parent::CreateSlave(&DownwashTicker::CreateViewer);*/
	CreateSlave();
}

void DownwashTicker::OnServerCreated(Life::UiGameServerManager*)
{
}



bool DownwashTicker::OpenUiManager()
{
	bool lOk = true;
	if (lOk)
	{
		lOk = mUiManager->OpenDraw();
	}
	if (lOk)
	{
		UiLepra::Core::ProcessMessages();
		//mUiManager->GetPainter()->ResetClippingRect();
		//mUiManager->GetPainter()->Clear(BLACK);
		//DisplaySplashLogo();
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
		mMusicPlayer->AddSong(_T("Dub Feral.ogg"));
		mMusicPlayer->AddSong(_T("Easy Jam.ogg"));
		mMusicPlayer->AddSong(_T("Firmament.ogg"));
		mMusicPlayer->AddSong(_T("Mandeville.ogg"));
		mMusicPlayer->AddSong(_T("Slow Ska Game Loop.ogg"));
		mMusicPlayer->AddSong(_T("Stealth Groover.ogg"));
		mMusicPlayer->AddSong(_T("Yallahs.ogg"));
		mMusicPlayer->Shuffle();
		lOk = mMusicPlayer->Playback();
	}
	if (lOk)
	{
		mUiManager->GetDesktopWindow()->CreateLayer(new UiTbc::FloatingLayout());
	}
	return lOk;
}

void DownwashTicker::BeginRender(vec3& pColor)
{
	Parent::BeginRender(pColor);
	mUiManager->GetRenderer()->SetOutlineFillColor(OFF_BLACK);
}

void DownwashTicker::PreWaitPhysicsTick()
{
	Parent::PreWaitPhysicsTick();
	if (mMusicPlayer)
	{
		mMusicPlayer->Update();
	}
}



void DownwashTicker::CloseMainMenu()
{
	if (mIsPlayerCountViewActive)
	{
		DeleteSlave(mSlaveArray[0], false);
		mIsPlayerCountViewActive = false;
	}
}

bool DownwashTicker::QueryQuit()
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



Life::GameClientSlaveManager* DownwashTicker::CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new DownwashManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}

/*Life::GameClientSlaveManager* DownwashTicker::CreateViewer(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new DownwashViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}*/



loginstance(GAME, DownwashTicker);



}
