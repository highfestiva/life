
// Author: Jonas Bystr�m
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
#include "TrabantSim.h"
#include "TrabantSimTicker.h"
#include "RtVar.h"
#include "Version.h"



namespace TrabantSim
{



class TrabantSim: public Life::Application
{
	typedef Life::Application Parent;
public:
	TrabantSim(const strutil::strvec& pArgumentList);
	virtual ~TrabantSim();
	virtual void Init();
	virtual void Destroy();

	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;

	logclass();
};



}



LEPRA_RUN_APPLICATION(TrabantSim::TrabantSim, UiLepra::UiMain);



namespace TrabantSim
{



TrabantSim::TrabantSim(const strutil::strvec& pArgumentList):
	Parent(_T("TrabantSim"), pArgumentList),
	mUiManager(0)
{
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
	CGSize lSize = [UIScreen mainScreen].trabant_sims.size;
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
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTRED, 0.1);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTGREEN, 0.1);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_AMBIENTBLUE, 0.1);
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



loginstance(GAME, TrabantSim);



}