
// Author: Jonas Byström
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
#include "HeliForce.h"
#include "HeliForceTicker.h"
#include "RtVar.h"
#include "Version.h"



namespace HeliForce
{



class HeliForce: public Life::Application
{
	typedef Life::Application Parent;
public:
	static HeliForce* GetApp();

	HeliForce(const strutil::strvec& pArgumentList);
	virtual ~HeliForce();
	virtual void Init();
	virtual bool MainLoop();
	virtual bool Tick();

	virtual void Suspend();
	virtual void Resume();

	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	static HeliForce* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;

	LOG_CLASS_DECLARE();
};



}



LEPRA_RUN_APPLICATION(HeliForce::HeliForce, UiLepra::UiMain);



namespace HeliForce
{



HeliForce* HeliForce::GetApp()
{
	return mApp;
}



HeliForce::HeliForce(const strutil::strvec& pArgumentList):
	Parent(_T("HeliForce"), pArgumentList),
	mUiManager(0)
{
	mApp = this;
}

HeliForce::~HeliForce()
{
	Destroy();

	delete (mUiManager);
	mUiManager = 0;

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void HeliForce::Init()
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
#elif defined(EMULATE_TOUCH)
	const int lDisplayWidth = 480;
	const int lDisplayHeight = 320;
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
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_VOLUME, 10.0);

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
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTRED, 0.5);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTGREEN, 0.5);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTBLUE, 0.5);
#ifdef LEPRA_TOUCH
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_ROLLOFF, 0.01);
#endif // Touch device.
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_DOPPLER, 1.0);

#if !defined(LEPRA_TOUCH) && !defined(EMULATE_TOUCH)
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, true);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_ROLLOFF, 0.5);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, true);
	//CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	//CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("Force:VolumesAndMaps"));
#endif // Computer

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_GAME_ENABLESTARTLOGO, lStartLogo);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEONLINEMASTER, lOnlineMaster);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_CONNECT_TIMEOUT, 4.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 4.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings());

	Parent::Init();

	/*Vector3DF lDelta(1, 0, 1);
	QuaternionF q8(PIF/8, lDelta);
	QuaternionF q4(PIF/4, lDelta);
	QuaternionF q2(PIF/2, lDelta);
	QuaternionF q1(PIF/1, lDelta);
	Vector3DF v(0,1,0);
	struct c { static str s(Vector3DF v) { return strutil::Format(_T("%.2f,%.2f,%.2f"), v.x, v.y, v.z); } };
	mLog.Infof(_T("8:%s, 4:%s, 2:%s, 1:%s"),
		c::s(q8*v).c_str(),
		c::s(q4*v).c_str(),
		c::s(q2*v).c_str(),
		c::s(q1*v).c_str());
	::exit(0);*/
}

bool HeliForce::MainLoop()
{
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return true;
#endif // !iOS/iOS
}

bool HeliForce::Tick()
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



void HeliForce::Suspend()
{
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

void HeliForce::Resume()
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	mUiManager->GetSoundManager()->Resume();
	//if (mMusicPlayer)
	//{
	//	mMusicPlayer->Stop();
	//	mMusicPlayer->Playback();
	//}
}



str HeliForce::GetTypeName() const
{
	return _T("Client");
}

str HeliForce::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* HeliForce::CreateTicker() const
{
	return new HeliForceTicker(mUiManager, mResourceManager, 200, 5, 5);
}



HeliForce* HeliForce::mApp;
LOG_CLASS_DEFINE(GAME, HeliForce);



}
