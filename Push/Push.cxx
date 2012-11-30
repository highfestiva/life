
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiLepra/Include/UiTouchstick.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiTBC/Include/UiTBC.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../Life/LifeApplication.h"
#include "PushMaster/MasterServerPort.h"
#include "Push.h"
#include "PushTicker.h"
#include "RtVar.h"
#include "Version.h"



#ifndef LEPRA_TOUCH
#define EMULATE_TOUCH	1
#endif // !iOS



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
	virtual bool MainLoop();
	virtual bool Tick();

	virtual void Suspend();
	virtual void Resume();

	str GetName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	UiCure::GameUiManager* mUiManager;

	static Push* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch
#ifdef EMULATE_TOUCH
	UiLepra::Touch::DragManager mDragManager;
	typedef UiLepra::Touch::TouchstickInputDevice Touchstick;
	Touchstick* mStick1;
#endif // Emulate touch

	LOG_CLASS_DECLARE();
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
#ifdef EMULATE_TOUCH
	mStick1 = 0;
#endif // Emulate touch
}

Push::~Push()
{
	Destroy();

	delete (mUiManager);
	mUiManager = 0;

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void Push::Init()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_CHILDISHNESS, 1.0);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_ENABLESTARTLOGO, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMROTATE, 0.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings());

	Parent::Init();

}

bool Push::MainLoop()
{
#ifdef EMULATE_TOUCH
	if (!mStick1)
	{
		mStick1 = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 100, 100));
	}
#endif // Emulate touch

#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return true;
#endif // !iOS/iOS
}

bool Push::Tick()
{
#ifdef EMULATE_TOUCH
	mDragManager.UpdateDragByMouse(mUiManager->GetInputManager());
	mDragManager.UpdateTouchsticks(mUiManager->GetInputManager());
	mDragManager.DropReleasedDrags();
#endif // Emulate touch
	return Parent::Tick();
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



str Push::GetName() const
{
	return _T("Push");
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
LOG_CLASS_DEFINE(GAME, Push);



}
