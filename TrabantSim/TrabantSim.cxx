
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
#ifdef LEPRA_IOS
#include "../UiLepra/Include/Mac/UiMacDisplayManager.h"
#include "PythonRunner.h"
#endif // iOS
#include "../UiTbc/Include/UiTbc.h"
#include "TrabantSim.h"
#include "TrabantSimTicker.h"
#include "RtVar.h"
#include "Version.h"



LEPRA_RUN_APPLICATION(TrabantSim::TrabantSim, UiLepra::UiMain);



namespace TrabantSim
{



void FoldSimulator()
{
	TrabantSim::mApp->FoldSimulator();
}



TrabantSim* TrabantSim::GetApp()
{
	return mApp;
}



TrabantSim::TrabantSim(const strutil::strvec& pArgumentList):
	Parent(_T("TrabantSim"), pArgumentList),
	mUiManager(0),
	mActiveCounter(-100),
	mIsInTick(false)
{
	mApp = this;
}

TrabantSim::~TrabantSim()
{
	Destroy();

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void TrabantSim::Init()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

#if defined(LEPRA_IOS)
	Thread::Sleep(3.0);	// Wait a bit longer so Pixel Doctrine splash is visible.
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	int lScale = [[UIScreen mainScreen] scale];
	const int lDisplayWidth = lSize.width * lScale;
	const int lDisplayHeight = lSize.height * lScale;
#else // Computer L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
#endif // Touch / Computer L&F
	int lDisplayBpp = 0;
	int lDisplayFrequency = 0;
	bool lDisplayFullScreen = false;
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
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLECLEAR, true);
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
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTRED, 0.4);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTGREEN, 0.4);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTBLUE, 0.4);
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

void TrabantSim::Destroy()
{
	Parent::Destroy();
	delete mUiManager;
	mUiManager = 0;
}

bool TrabantSim::MainLoop()
{
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return true;
#endif // !iOS/iOS
}

bool TrabantSim::Tick()
{
	if (mActiveCounter > 0)
	{
		mIsInTick = true;
		const bool lOk = Parent::Tick();
		mIsInTick = false;
		if (mActiveCounter <= 0)
		{
			const int c = mActiveCounter;
			mActiveCounter = 1;
			Suspend(false);
			mActiveCounter = c;
		}
		return lOk;
	}
	return true;
}



void TrabantSim::Resume(bool pHard)
{
	if (++mActiveCounter != 1)
	{
		return;
	}

#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	mUiManager->GetSoundManager()->Resume();
	mGameTicker->Resume(pHard);
}

void TrabantSim::Suspend(bool pHard)
{
	if (pHard)
	{
		mGameTicker->Suspend(pHard);
	}
	if (--mActiveCounter != 0)
	{
		return;
	}
	if (mIsInTick)
	{
		return;
	}

	mGameTicker->Suspend(pHard);
	mUiManager->GetSoundManager()->Suspend();
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
#endif // iOS
}

void TrabantSim::FoldSimulator()
{
#ifdef LEPRA_IOS
	astr lStdOut = PythonRunner::GetStdOut();
	Suspend(false);
	UIWindow* window = ((UiLepra::MacDisplayManager*)TrabantSim::TrabantSim::mApp->mUiManager->GetDisplayManager())->GetWindow();
	[window setHidden:YES];
	[[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationNone];
	[TrabantSim::TrabantSim::mApp->mAnimatedApp.window makeKeyAndVisible];
	PythonRunner::Break();
	if (!lStdOut.empty())
	{
		[TrabantSim::TrabantSim::mApp->mAnimatedApp handleStdOut:lStdOut];
	}
#endif // iOS
}

void TrabantSim::SavePurchase()
{
}



str TrabantSim::GetTypeName() const
{
	return _T("Viewer");
}

str TrabantSim::GetVersion() const
{
	return _T(VIEWER_VERSION);
}

Cure::ApplicationTicker* TrabantSim::CreateTicker() const
{
	return new TrabantSimTicker(mUiManager, mResourceManager, 200, 5, 5);
}



TrabantSim* TrabantSim::mApp;
loginstance(GAME, TrabantSim);



}
