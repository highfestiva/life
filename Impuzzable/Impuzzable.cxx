
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/LepraOS.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../Life/LifeApplication.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiSoundManager.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiTbc/Include/UiTbc.h"
#include "Impuzzable.h"
#include "ImpuzzableTicker.h"
#include "RtVar.h"
#include "Version.h"



namespace Impuzzable
{



class Impuzzable: public Life::Application
{
	typedef Life::Application Parent;
public:
	static Impuzzable* GetApp();

	Impuzzable(const strutil::strvec& pArgumentList);
	virtual ~Impuzzable();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();

	virtual void Suspend();
	virtual void Resume();
	void SavePurchase();

	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	static Impuzzable* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Impuzzable::Impuzzable, UiLepra::UiMain);



namespace Impuzzable
{



Impuzzable* Impuzzable::GetApp()
{
	return mApp;
}



Impuzzable::Impuzzable(const strutil::strvec& pArgumentList):
	Parent(_T("Impuzzable"), pArgumentList),
	mUiManager(0)
{
	mApp = this;
}

Impuzzable::~Impuzzable()
{
	Destroy();

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void Impuzzable::Init()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

#if defined(LEPRA_IOS)
	Thread::Sleep(3.0);	// Wait a bit longer so Pixel Doctrine splash is visible.
	CGSize lSize = [UIScreen mainScreen].impuzzables.size;
	int lScale = [[UIScreen mainScreen] scale];
	const int lDisplayWidth = lSize.height * lScale;
	const int lDisplayHeight = lSize.width * lScale;
	bool lDisplayFullScreen = true;
#else // Computer L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
	bool lDisplayFullScreen = false;
#endif // Touch / Computer L&F
	int lDisplayBpp = 0;
	int lDisplayFrequency = 0;
	double lPhysicalScreenSize = 24.0;	// An average computer's physical screen size (inches across).
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_RENDERENGINE, _T("OpenGL"));
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_WIDTH, lDisplayWidth);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_HEIGHT, lDisplayHeight);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_BITSPERPIXEL, lDisplayBpp);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_FREQUENCY, lDisplayFrequency);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_FULLSCREEN, lDisplayFullScreen);
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ORIENTATION, _T("Fixed"));
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_PHYSICALSIZE, lPhysicalScreenSize);

	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));

	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ENABLEVSYNC, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLECLEAR, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_PIXELSHADERS, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLELIGHTS, true);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMIPMAPPING, false);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_FOV, 20.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 0.1);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 20.0);
	//v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("None"));
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_SHADOWS, _T("Force:Volumes"));
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARRED, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARGREEN, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_CLEARBLUE, 0.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTRED, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTGREEN, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTBLUE, 0.5);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_ROLLOFF, 0.1);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_DOPPLER, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_SOUND_MUSICVOLUME, 0.6);

	v_override(UiCure::GetSettings(), RTVAR_CTRL_EMULATETOUCH, true);

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_CONNECT_TIMEOUT, 4.0);
	v_override(UiCure::GetSettings(), RTVAR_NETWORK_LOGIN_TIMEOUT, 4.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings(), &mDragManager);

	Parent::Init();
}

void Impuzzable::Destroy()
{
	Parent::Destroy();
	delete mUiManager;
	mUiManager = 0;
}

bool Impuzzable::MainLoop()
{
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return true;
#endif // !iOS/iOS
}



void Impuzzable::Suspend()
{
	mGameTicker->Suspend(true);
	mUiManager->GetSoundManager()->Suspend();
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
#endif // iOS
}

void Impuzzable::Resume()
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	mUiManager->GetSoundManager()->Resume();
	mGameTicker->Resume(true);
}

void Impuzzable::SavePurchase()
{
	Life::GameClientMasterTicker* lTicker = (Life::GameClientMasterTicker*)mGameTicker;
	Life::GameClientSlaveManager* lManager = lTicker->GetSlave(0);
	v_set(lManager->GetVariableScope(), RTVAR_GAME_LEVELSHAPEALTERNATE, true);
	v_set(lManager->GetVariableScope(), RTVAR_GAME_RUNADS, false);
	lTicker->SaveRtvars(mUiManager->GetVariableScope());
}

void Impuzzable__ShowAd()
{
#ifdef LEPRA_IOS
        [Impuzzable::GetApp()->mAnimatedApp showAd];
#endif // iOS
}

void Impuzzable__Buy()
{
#ifdef LEPRA_IOS
	[Impuzzable::GetApp()->mAnimatedApp startPurchase:@"ImpuzzableFullInAppPurchase"];
#endif // iOS
}



str Impuzzable::GetTypeName() const
{
	return _T("Client");
}

str Impuzzable::GetVersion() const
{
	return _T(GAME_VERSION);
}

Cure::ApplicationTicker* Impuzzable::CreateTicker() const
{
	return new ImpuzzableTicker(mUiManager, mResourceManager, 200, 5, 5);
}



Impuzzable* Impuzzable::mApp;
loginstance(GAME, Impuzzable);



}
