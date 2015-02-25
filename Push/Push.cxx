
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
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiTbc/Include/UiTbc.h"
#include "PushMaster/MasterServerPort.h"
#include "Push.h"
#include "PushTicker.h"
#include "RtVar.h"
#include "Version.h"



namespace Push
{



class Push: public Life::Application
{
	typedef Life::Application Parent;
public:
	static Push* GetApp();

	Push(const strutil::strvec& pArgumentList);
	virtual ~Push();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();

	virtual void Suspend();
	virtual void Resume();

	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	static Push* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Push::Push, UiLepra::UiMain);



namespace Push
{



Push* Push::GetApp()
{
	return mApp;
}



Push::Push(const strutil::strvec& pArgumentList):
	Parent(_T(PUSH_APPLICATION_NAME), pArgumentList),
	mUiManager(0)
{
	mApp = this;
}

Push::~Push()
{
	Destroy();

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void Push::Init()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const bool lStartLogo = false;
	const bool lOnlineMaster = false;
#else // Computer
	const bool lStartLogo = true;
	const bool lOnlineMaster = true;
#endif // Touch / computer
#if defined(LEPRA_IOS)
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	const int lDisplayWidth = lSize.height;
	const int lDisplayHeight = lSize.width;
	bool lDisplayFullScreen = true;
#elif defined(EMULATE_TOUCH)
	const int lDisplayWidth = 480;
	const int lDisplayHeight = 320;
	bool lDisplayFullScreen = false;
#else // Computer L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
	bool lDisplayFullScreen = false;
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

	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));

	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ENABLEVSYNC, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLELIGHTS, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_FOV, 60.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 1000.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("None"));
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTRED, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTGREEN, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTBLUE, 0.5);
#ifdef LEPRA_TOUCH
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ROLLOFF, 0.9);
#endif // Touch device.
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_DOPPLER, 1.0);

#if !defined(LEPRA_TOUCH) && !defined(EMULATE_TOUCH)
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ROLLOFF, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("Force:Volume"));	
#endif // Computer

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	v_override(UiCure::GetSettings(), RTVAR_GAME_ENABLESTARTLOGO, lStartLogo);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEONLINEMASTER, lOnlineMaster);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_CONNECT_TIMEOUT, 4.0);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 4.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings(), &mDragManager);

	Parent::Init();

}

void Push::Destroy()
{
	Parent::Destroy();
	delete mUiManager;
	mUiManager = 0;
}

bool Push::MainLoop()
{
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return true;
#endif // !iOS/iOS
}



void Push::Suspend()
{
	//if (mMusicPlayer)
	//{
	//	mMusicPlayer->Pause();
	//}
	//DoPause();
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
#endif // iOS
}

void Push::Resume()
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	//if (mMusicPlayer)
	//{
	//	mMusicPlayer->Stop();
	//	mMusicPlayer->Playback();
	//}
}



str Push::GetTypeName() const
{
	return _T("Client");
}

str Push::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* Push::CreateTicker() const
{
	PushTicker* lTicker = new PushTicker(mUiManager, mResourceManager, 2000, 7, 1);
	lTicker->SetMasterServerConnection(new Life::MasterServerConnection(_T(MASTER_SERVER_ADDRESS) _T(":") _T(MASTER_SERVER_PORT)));
	return lTicker;
}



Push* Push::mApp;
loginstance(GAME, Push);



}
