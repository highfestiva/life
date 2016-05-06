
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../cure/include/runtimevariable.h"
#include "../lepra/include/lepraos.h"
#include "../lepra/include/systemmanager.h"
#include "../life/lifeserver/masterserverconnection.h"
#include "../life/lifeapplication.h"
#include "../uicure/include/uicure.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uilepra/include/uitouchdrag.h"
#include "../uilepra/include/uicore.h"
#include "../uitbc/include/uitbc.h"
#include "pushmaster/masterserverport.h"
#include "push.h"
#include "pushticker.h"
#include "rtvar.h"
#include "version.h"



namespace Push {



class Push: public life::Application {
	typedef life::Application Parent;
public:
	static Push* GetApp();

	Push(const strutil::strvec& argument_list);
	virtual ~Push();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();

	virtual void Suspend();
	virtual void Resume();

	str GetTypeName() const;
	str GetVersion() const;
	cure::ApplicationTicker* CreateTicker() const;

	static Push* app_;
#ifdef LEPRA_TOUCH
	AnimatedApp* animated_app_;
#endif // touch

	UiCure::GameUiManager* ui_manager_;
	uilepra::touch::DragManager drag_manager_;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Push::Push, uilepra::UiMain);



namespace Push {



Push* Push::GetApp() {
	return app_;
}



Push::Push(const strutil::strvec& argument_list):
	Parent(kPushApplicationName, argument_list),
	ui_manager_(0) {
	app_ = this;
}

Push::~Push() {
	Destroy();

	UiCure::Shutdown();
	uitbc::Shutdown();
	uilepra::Shutdown();
}

void Push::Init() {
	uilepra::Init();
	uitbc::Init();
	UiCure::Init();

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const bool start_logo = false;
	const bool online_master = false;
#else // Computer
	const bool start_logo = true;
	const bool online_master = true;
#endif // touch / computer
#if defined(LEPRA_IOS)
	CGSize __size = [UIScreen mainScreen].bounds.size;
	const int display_width = __size.height;
	const int display_height = __size.width;
	bool display_full_screen = true;
#elif defined(EMULATE_TOUCH)
	const int display_width = 480;
	const int display_height = 320;
	bool display_full_screen = false;
#else // Computer L&F
	const int display_width = 760;
	const int display_height = 524;
	bool display_full_screen = false;
#endif // touch / Emulated / Computer L&F
	int display_bpp = 0;
	int display_frequency = 0;
	double physical_screen_size = 24.0;	// An average computer's physical screen size (inches across).
	v_override(UiCure::GetSettings(), kRtvarUiDisplayRenderengine, "OpenGL");
	v_override(UiCure::GetSettings(), kRtvarUiDisplayWidth, display_width);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayHeight, display_height);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayBitsperpixel, display_bpp);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayFrequency, display_frequency);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayFullscreen, display_full_screen);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayOrientation, "AllowUpsideDown");
	v_override(UiCure::GetSettings(), kRtvarUiDisplayPhysicalsize, physical_screen_size);

	v_override(UiCure::GetSettings(), kRtvarUiSoundEngine, "OpenAL");

	v_override(UiCure::GetSettings(), kRtvarUiDisplayEnablevsync, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DPixelshaders, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablelights, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnabletrilinearfiltering, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablebilinearfiltering, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablemipmapping, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DFov, 60.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DClipnear, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DClipfar, 1000.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DShadows, "None");
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientred, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientgreen, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientblue, 0.5);
#ifdef LEPRA_TOUCH
	v_override(UiCure::GetSettings(), kRtvarUiSoundRolloff, 0.9);
#endif // touch device.
	v_override(UiCure::GetSettings(), kRtvarUiSoundDoppler, 1.0);

#if !defined(LEPRA_TOUCH) && !defined(EMULATE_TOUCH)
	v_override(UiCure::GetSettings(), kRtvarUi3DPixelshaders, true);
	v_override(UiCure::GetSettings(), kRtvarUiSoundRolloff, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnabletrilinearfiltering, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablemipmapping, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DShadows, "Force:Volume");
#endif // Computer

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	v_override(UiCure::GetSettings(), kRtvarGameEnablestartlogo, start_logo);
	v_override(UiCure::GetSettings(), kRtvarNetworkEnableonlinemaster, online_master);
	v_override(UiCure::GetSettings(), kRtvarNetworkEnableopenserver, false);
	v_override(UiCure::GetSettings(), kRtvarNetworkConnectTimeout, 4.0);
	v_override(UiCure::GetSettings(), kRtvarNetworkLoginTimeout, 4.0);

	ui_manager_ = new UiCure::GameUiManager(UiCure::GetSettings(), &drag_manager_);

	Parent::Init();

}

void Push::Destroy() {
	Parent::Destroy();
	delete ui_manager_;
	ui_manager_ = 0;
}

bool Push::MainLoop() {
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	animated_app_ = [[AnimatedApp alloc] init:ui_manager_->GetCanvas()];
	return true;
#endif // !iOS/iOS
}



void Push::Suspend() {
	//if (music_player_)
	//{
	//	music_player_->Pause();
	//}
	//DoPause();
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
}

void Push::Resume() {
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
	//if (music_player_)
	//{
	//	music_player_->Stop();
	//	music_player_->Playback();
	//}
}



str Push::GetTypeName() const {
	return "Client";
}

str Push::GetVersion() const {
	return kPlatformVersion;
}

cure::ApplicationTicker* Push::CreateTicker() const {
	PushTicker* ticker = new PushTicker(ui_manager_, resource_manager_, 2000, 7, 1);
	ticker->SetMasterServerConnection(new life::MasterServerConnection(kMasterServerAddress ":" kMasterServerPort));
	return ticker;
}



Push* Push::app_;
loginstance(kGame, Push);



}
