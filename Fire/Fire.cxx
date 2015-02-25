
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/LepraOS.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../Life/LifeApplication.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiSoundManager.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiTbc/Include/UiTbc.h"
#include "Fire.h"
#include "FireTicker.h"
#include "RtVar.h"
#include "Version.h"



namespace Fire
{



class Fire: public Life::Application
{
	typedef Life::Application Parent;
public:
	static Fire* GetApp();

	Fire(const strutil::strvec& pArgumentList);
	virtual ~Fire();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();

	virtual void Suspend();
	virtual void Resume();

	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	static Fire* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Fire::Fire, UiLepra::UiMain);



namespace Fire
{



Fire* Fire::GetApp()
{
	return mApp;
}



Fire::Fire(const strutil::strvec& pArgumentList):
	Parent(_T("Fire"), pArgumentList),
	mUiManager(0)
{
	mApp = this;
}

Fire::~Fire()
{
	Destroy();

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void Fire::Init()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

#if defined(LEPRA_IOS)
	Thread::Sleep(3.0);	// Wait a bit longer so Pixel Doctrine splash is visible.
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	int lScale = [[UIScreen mainScreen] scale];
	const int lDisplayWidth = lSize.height * lScale;
	const int lDisplayHeight = lSize.width * lScale;
	const bool lDisplayFullScreen = true;
#elif defined(EMULATE_TOUCH)
	const int lDisplayWidth = 1024;
	const int lDisplayHeight = 768;
	const bool lDisplayFullScreen = false;
#else // Computer L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
	const bool lDisplayFullScreen = false;
#endif // Touch / Emulated / Computer L&F
	int lDisplayBpp = 0;
	int lDisplayFrequency = 0;
	double lPhysicalScreenSize = 24.0;	// An average computer's physical screen size (inches across).
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_RENDERENGINE, _T("OpenGL"));
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_WIDTH, lDisplayWidth);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_HEIGHT, lDisplayHeight);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_BITSPERPIXEL, lDisplayBpp);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_FREQUENCY, lDisplayFrequency);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_FULLSCREEN, lDisplayFullScreen);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ORIENTATION, _T("AllowUpsideDown"));
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_PHYSICALSIZE, lPhysicalScreenSize);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ENABLEVSYNC, false);

	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));

	v_override(UiCure::GetSettings(), RTVAR_UI_2D_PAINTMODE, _T("AlphaBlend"));
	v_override(UiCure::GetSettings(), RTVAR_UI_2D_FONT, _T("Verdana"));
	v_override(UiCure::GetSettings(), RTVAR_UI_2D_FONTFLAGS, 0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLELIGHTS, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_FOV, 60.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 1000.0);
	//v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("None"));
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("Force:Volumes"));
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWDEVIATION, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARRED, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARGREEN, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARBLUE, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTRED, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTGREEN, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTBLUE, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ROLLOFF, 0.02);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_DOPPLER, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_MUSICVOLUME, 0.6);

	v_override(UiCure::GetSettings(), RTVAR_PHYSICS_MICROSTEPS, 1);

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_CONNECT_TIMEOUT, 4.0);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 4.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings(), &mDragManager);

	Parent::Init();
}

void Fire::Destroy()
{
	Parent::Destroy();
	delete mUiManager;
	mUiManager = 0;
}

bool Fire::MainLoop()
{
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return true;
#endif // !iOS/iOS
}



void Fire::Suspend()
{
	mGameTicker->Suspend();
	//if (mMusicPlayer)
	//{
	//	mMusicPlayer->Pause();
	//}
	//DoPause();
	mUiManager->GetSoundManager()->Suspend();
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
#endif // iOS
}

void Fire::Resume()
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	mUiManager->GetSoundManager()->Resume();
	mGameTicker->Resume();
	//if (mMusicPlayer)
	//{
	//	mMusicPlayer->Stop();
	//	mMusicPlayer->Playback();
	//}
}



str Fire::GetTypeName() const
{
	return _T("Client");
}

str Fire::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* Fire::CreateTicker() const
{
	return new FireTicker(mUiManager, mResourceManager, 200, 5, 5);
}



Fire* Fire::mApp;
loginstance(GAME, Fire);



}
