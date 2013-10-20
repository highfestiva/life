
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../Life/LifeApplication.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiSoundManager.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiTBC/Include/UiTBC.h"
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
	virtual bool MainLoop();
	virtual bool Tick();

	virtual void Suspend();
	virtual void Resume();

	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	static Downwash* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;

	LOG_CLASS_DECLARE();
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
	Parent(_T("Downwash"), pArgumentList),
	mUiManager(0)
{
	mApp = this;
}

Downwash::~Downwash()
{
	Destroy();

	delete (mUiManager);
	mUiManager = 0;

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
	const int lDisplayWidth = lSize.height;
	const int lDisplayHeight = lSize.width;
#elif defined(EMULATE_TOUCH)
	const int lDisplayWidth = 1024;
	const int lDisplayHeight = 768;
#else // Computer L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
#endif // Touch / Emulated / Computer L&F
	int lDisplayBpp = 0;
	int lDisplayFrequency = 0;
	bool lDisplayFullScreen = false;
	double lPhysicalScreenSize = 24.0;	// An average computer's physical screen size (inches across).
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_RENDERENGINE, _T("OpenGL"));
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_WIDTH, lDisplayWidth);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_HEIGHT, lDisplayHeight);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_BITSPERPIXEL, lDisplayBpp);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_FREQUENCY, lDisplayFrequency);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_FULLSCREEN, lDisplayFullScreen);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ORIENTATION, _T("AllowUpsideDown"));
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_PHYSICALSIZE, lPhysicalScreenSize);

	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));

	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ENABLEVSYNC, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLELIGHTS, true);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	//CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_FOV, 60.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 1.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 1000.0);
	//CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("None"));
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("Force:Volumes"));
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_CLEARRED, 0.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_CLEARGREEN, 0.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_CLEARBLUE, 0.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTRED, 0.5);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTGREEN, 0.5);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTBLUE, 0.5);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_ROLLOFF, 0.1);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_DOPPLER, 1.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_MUSICVOLUME, 0.6);

#if !defined(LEPRA_TOUCH) && !defined(EMULATE_TOUCH)
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, true);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, true);
	//CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	//CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("Force:VolumesAndMaps"));
#endif // Computer

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_CONNECT_TIMEOUT, 4.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 4.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings());

	Parent::Init();
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

bool Downwash::Tick()
{
	HiResTimer::StepCounterShadow();

#if defined(EMULATE_TOUCH)
	mDragManager.UpdateDragByMouse(mUiManager->GetInputManager());
#elif defined(LEPRA_TOUCH)
	mDragManager.UpdateMouseByDrag(mUiManager->GetInputManager());
#endif // Touch
	mDragManager.UpdateTouchsticks(mUiManager->GetInputManager());
	mDragManager.DropReleasedDrags();

	return Parent::Tick();
}



void Downwash::Suspend()
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

void Downwash::Resume()
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



str Downwash::GetTypeName() const
{
	return _T("Client");
}

str Downwash::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* Downwash::CreateTicker() const
{
	return new DownwashTicker(mUiManager, mResourceManager, 200, 5, 5);
}



Downwash* Downwash::mApp;
LOG_CLASS_DEFINE(GAME, Downwash);



}