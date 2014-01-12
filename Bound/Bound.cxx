
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../Life/LifeApplication.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiSoundManager.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiTBC/Include/UiTBC.h"
#include "Bound.h"
#include "BoundTicker.h"
#include "RtVar.h"
#include "Version.h"



namespace Bound
{



class Bound: public Life::Application
{
	typedef Life::Application Parent;
public:
	static Bound* GetApp();

	Bound(const strutil::strvec& pArgumentList);
	virtual ~Bound();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();

	virtual void Suspend();
	virtual void Resume();
	void SavePurchase();

	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	static Bound* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;

	LOG_CLASS_DECLARE();
};



}



LEPRA_RUN_APPLICATION(Bound::Bound, UiLepra::UiMain);



namespace Bound
{



Bound* Bound::GetApp()
{
	return mApp;
}



Bound::Bound(const strutil::strvec& pArgumentList):
	Parent(_T("Bound"), pArgumentList),
	mUiManager(0)
{
	mApp = this;
}

Bound::~Bound()
{
	Destroy();

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void Bound::Init()
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
#else // Computer L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
#endif // Touch / Computer L&F
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
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ORIENTATION, _T("Fixed"));
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_PHYSICALSIZE, lPhysicalScreenSize);

	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));

	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ENABLEVSYNC, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLECLEAR, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLELIGHTS, true);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_FOV, 20.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 0.1);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 20.0);
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

	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_CTRL_EMULATETOUCH, true);

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_CONNECT_TIMEOUT, 4.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 4.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings(), &mDragManager);

	Parent::Init();
}

void Bound::Destroy()
{
	Parent::Destroy();
	delete mUiManager;
	mUiManager = 0;
}

bool Bound::MainLoop()
{
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return true;
#endif // !iOS/iOS
}



void Bound::Suspend()
{
	mGameTicker->Suspend();
	mUiManager->GetSoundManager()->Suspend();
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
#endif // iOS
}

void Bound::Resume()
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	mUiManager->GetSoundManager()->Resume();
	mGameTicker->Resume();
}

void Bound::SavePurchase()
{
	Life::GameClientMasterTicker* lTicker = (Life::GameClientMasterTicker*)mGameTicker;
	Life::GameClientSlaveManager* lManager = lTicker->GetSlave(0);
	CURE_RTVAR_SET(lManager->GetVariableScope(), RTVAR_GAME_LEVELSHAPEALTERNATE, true);
	CURE_RTVAR_SET(lManager->GetVariableScope(), RTVAR_GAME_RUNADS, false);
	lTicker->SaveRtvars(mUiManager->GetVariableScope());
}

void Bound__ShowAd()
{
#ifdef LEPRA_IOS
        [Bound::GetApp()->mAnimatedApp showAd];
#endif // iOS
}

void Bound__Buy()
{
#ifdef LEPRA_IOS
	[Bound::GetApp()->mAnimatedApp startPurchase:@"BoundFullInAppPurchase"];
#endif // iOS
}



str Bound::GetTypeName() const
{
	return _T("Client");
}

str Bound::GetVersion() const
{
	return _T(GAME_VERSION);
}

Cure::ApplicationTicker* Bound::CreateTicker() const
{
	return new BoundTicker(mUiManager, mResourceManager, 200, 5, 5);
}



Bound* Bound::mApp;
LOG_CLASS_DEFINE(GAME, Bound);



}
