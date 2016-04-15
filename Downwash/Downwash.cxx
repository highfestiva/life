
// Author: Jonas Byström
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
#include "Downwash.h"
#include "DownwashTicker.h"
#include "RtVar.h"
#include "Version.h"



namespace Downwash
{



class Downwash: public Life::Application
{
	typedef Life::Application Parent;
public:
	static Downwash* GetApp();

	Downwash(const strutil::strvec& pArgumentList);
	virtual ~Downwash();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();

	virtual void Suspend(bool pHard);
	virtual void Resume(bool pHard);

	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	static Downwash* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Downwash::Downwash, UiLepra::UiMain);



namespace Downwash
{



Downwash* Downwash::GetApp()
{
	return mApp;
}



Downwash::Downwash(const strutil::strvec& pArgumentList):
	Parent("Downwash", pArgumentList),
	mUiManager(0)
{
	mApp = this;
}

Downwash::~Downwash()
{
	Destroy();

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void Downwash::Init()
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
	bool lDisplayFullScreen = true;
#elif defined(EMULATE_TOUCH)
	const int lDisplayWidth = 1024;
	const int lDisplayHeight = 768;
	bool lDisplayFullScreen = false;
#else // Computer L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
	bool lDisplayFullScreen = false;
#endif // Touch / Emulated / Computer L&F
	int lDisplayBpp = 0;
	int lDisplayFrequency = 0;
	double lPhysicalScreenSize = 24.0;	// An average computer's physical screen size (inches across).
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_RENDERENGINE, "OpenGL");
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_WIDTH, lDisplayWidth);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_HEIGHT, lDisplayHeight);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_BITSPERPIXEL, lDisplayBpp);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_FREQUENCY, lDisplayFrequency);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_FULLSCREEN, lDisplayFullScreen);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ORIENTATION, "AllowUpsideDown");
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_PHYSICALSIZE, lPhysicalScreenSize);

	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ENGINE, "OpenAL");

	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ENABLEVSYNC, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_2D_PAINTMODE, "AlphaBlend");
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLELIGHTS, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	//v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_FOV, 60.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 1000.0);
	//v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, "None");
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, "Force:Volumes");
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARRED, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARGREEN, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARBLUE, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTRED, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTGREEN, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTBLUE, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ROLLOFF, 0.1);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_DOPPLER, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_MUSICVOLUME, 0.6);

#if !defined(LEPRA_TOUCH) && !defined(EMULATE_TOUCH)
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, true);
	//v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	//v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, "Force:VolumesAndMaps");
#endif // Computer

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_CONNECT_TIMEOUT, 4.0);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 4.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings(), &mDragManager);

	Parent::Init();
}

void Downwash::Destroy()
{
	Parent::Destroy();
	delete mUiManager;
	mUiManager = 0;
}

bool Downwash::MainLoop()
{
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return true;
#endif // !iOS/iOS
}



void Downwash::Suspend(bool pHard)
{
	mGameTicker->Suspend(pHard);
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

void Downwash::Resume(bool pHard)
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	mUiManager->GetSoundManager()->Resume();
	mGameTicker->Resume(pHard);
	//if (mMusicPlayer)
	//{
	//	mMusicPlayer->Stop();
	//	mMusicPlayer->Playback();
	//}
}



str Downwash::GetTypeName() const
{
	return "Client";
}

str Downwash::GetVersion() const
{
	return PLATFORM_VERSION;
}

Cure::ApplicationTicker* Downwash::CreateTicker() const
{
	return new DownwashTicker(mUiManager, mResourceManager, 200, 5, 5);
}



Downwash* Downwash::mApp;
loginstance(GAME, Downwash);



}
