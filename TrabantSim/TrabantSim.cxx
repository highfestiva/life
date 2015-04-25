
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
#include "../UiLepra/Include/Mac/EAGLView.h"
#include "../UiLepra/Include/Mac/RotatingController.h"
#include "../UiLepra/Include/Mac/UiMacDisplayManager.h"
#include "PythonRunner.h"
#endif // iOS
#include "../UiTbc/Include/UiTbc.h"
#include "AnimatedApp.h"
#include "TrabantSimTicker.h"
#include "RtVar.h"
#include "Version.h"



LEPRA_RUN_APPLICATION(TrabantSim::TrabantSim, UiLepra::UiMain);



namespace TrabantSim
{



void FoldSuspendSimulator()
{
	TrabantSim::mApp->FoldSimulator();
	TrabantSim::mApp->Suspend(false);
}

void FoldSimulator()
{
	TrabantSim::mApp->FoldSimulator();
}

void UnfoldSimulator()
{
	TrabantSim::mApp->UnfoldSimulator();
}
	
void DidSyncFiles()
{
	TrabantSim::mApp->DidSyncFiles();
}



TrabantSim* TrabantSim::GetApp()
{
	return mApp;
}



TrabantSim::TrabantSim(const strutil::strvec& pArgumentList):
	Parent(_T("Trabant"), pArgumentList),
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
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	int lScale = [[UIScreen mainScreen] scale];
	int lDisplayWidth = lSize.width * lScale;
	int lDisplayHeight = lSize.height * lScale;
	if (lDisplayHeight > lDisplayWidth)
	{
		// Phone might start up in portrait, but our game will always be in landscape mode.
		std::swap(lDisplayWidth, lDisplayHeight);
	}
	const double lFontHeight = 30.0*lScale;
	v_override(UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, lFontHeight);
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
	v_override(UiCure::GetSettings(), RTVAR_UI_DISPLAY_ORIENTATION, _T("AllowUpsideDown"));
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
	mActiveCounter = 1;
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
	if (pHard)
	{
		// If we're only coming back from background all we don't
		// want to start ticking the simulator.
		mGameTicker->Resume(true);	// Resume's hard/soft has no effect.
		mActiveCounter = 0;
		return;
	}

	if (++mActiveCounter != 1)
	{
		return;
	}

	mGameTicker->Resume(false);	// Resume sets some internal states of the simulator state machine.
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	mUiManager->GetSoundManager()->Resume();
}

void TrabantSim::Suspend(bool pHard)
{
	if (mActiveCounter == 0 && !pHard)
	{
		return;
	}
	mActiveCounter = 0;
	if (mIsInTick && !pHard)
	{
		return;
	}
#ifdef LEPRA_IOS
	const bool lIsRunningLocally = PythonRunner::IsRunning();
#endif // iOS
	mGameTicker->Suspend(pHard);	// Hard means cut the chord, soft just sends "disconnect" to remote end.
	mUiManager->GetSoundManager()->Suspend();
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
	if (!lIsRunningLocally)
	{
		FoldSimulator();
	}
#endif // iOS
}

void TrabantSim::FoldSimulator()
{
#ifdef LEPRA_IOS
	dispatch_block_t Fold = ^
	{
		const int lWasActive = mActiveCounter;
		astr lStdOut = PythonRunner::GetStdOut();
		Suspend(false);
		//UIWindow* window = ((UiLepra::MacDisplayManager*)TrabantSim::TrabantSim::mApp->mUiManager->GetDisplayManager())->GetWindow();
		//[window setHidden:YES];
		[[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationNone];
		//[TrabantSim::TrabantSim::mApp->mAnimatedApp.window makeKeyAndVisible];
		PythonRunner::Break();
		if (!lStdOut.empty() && lWasActive > 0)
		{
			[TrabantSim::TrabantSim::mApp->mAnimatedApp handleStdOut:lStdOut];
		}
		else
		{
			[TrabantSim::TrabantSim::mAnimatedApp popIfGame];
		}
	};
	if ([NSThread isMainThread])
	{
		Fold();
	}
	else
	{
		dispatch_sync(dispatch_get_main_queue(), Fold);
	}
#endif // iOS
}

void TrabantSim::UnfoldSimulator()
{
#ifdef LEPRA_IOS
	if ([TrabantSim::TrabantSim::mApp->mAnimatedApp showingSimulator])
	{
		return;
	}

	dispatch_block_t Unfold = ^
	{
		//[TrabantSim::TrabantSim::mAnimatedApp.window setHidden:YES];
		//UIWindow* window = ((UiLepra::MacDisplayManager*)TrabantSim::TrabantSim::mApp->mUiManager->GetDisplayManager())->GetWindow();
		//if (!window.rootViewController)
		UIViewController* controller = [[UIViewController alloc] init];
		controller.view = [EAGLView sharedView];
		//window.rootViewController = controller;
		[TrabantSim::TrabantSim::mApp->mAnimatedApp pushSimulatorController:controller];
		//[window makeKeyAndVisible];
		TrabantSim::TrabantSim::mApp->mActiveCounter = 0;	// Make sure no lost event causes a halt.
		TrabantSim::TrabantSim::mApp->Resume(false);
		//[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
		PythonRunner::ClearStdOut();
	};
	if ([NSThread isMainThread])
	{
		Unfold();
	}
	else
	{
		dispatch_sync(dispatch_get_main_queue(), Unfold);
	}
#endif // iOS
}

void TrabantSim::DidSyncFiles()
{
#ifdef LEPRA_IOS
	dispatch_async(dispatch_get_main_queue(), ^{
		[TrabantSim::TrabantSim::mAnimatedApp.listController reloadPrototypes];
	});
#endif // iOS
}
void TrabantSim::SavePurchase()
{
}



str TrabantSim::GetTypeName() const
{
	return _T("Simulator");
}

str TrabantSim::GetVersion() const
{
	return _T(TRABANT_VERSION);
}

Cure::ApplicationTicker* TrabantSim::CreateTicker() const
{
	return new TrabantSimTicker(mUiManager, mResourceManager, 200, 5, 5);
}



TrabantSim* TrabantSim::mApp;
loginstance(GAME, TrabantSim);



}
