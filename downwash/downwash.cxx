
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
#include "../uilepra/include/uicore.h"
#include "../uilepra/include/uisoundmanager.h"
#include "../uilepra/include/uitouchdrag.h"
#include "../uitbc/include/uitbc.h"
#include "downwash.h"
#include "downwashticker.h"
#include "rtvar.h"
#include "version.h"



namespace Downwash {



class Downwash: public life::Application {
	typedef life::Application Parent;
public:
	static Downwash* GetApp();

	Downwash(const strutil::strvec& argument_list);
	virtual ~Downwash();
	virtual void Init();
	virtual void Destroy();
	virtual LogListener* CreateConsoleLogListener() const;

	virtual bool MainLoop();

	virtual void Suspend(bool hard);
	virtual void Resume(bool hard);

	str GetTypeName() const;
	str GetVersion() const;
	cure::ApplicationTicker* CreateTicker() const;

	static Downwash* app_;
#ifdef LEPRA_TOUCH
	AnimatedApp* animated_app_;
#endif // touch

	UiCure::GameUiManager* ui_manager_;
	uilepra::touch::DragManager drag_manager_;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Downwash::Downwash, uilepra::UiMain);



namespace Downwash {



Downwash* Downwash::GetApp() {
	return app_;
}



Downwash::Downwash(const strutil::strvec& argument_list):
	Parent("downwash", argument_list),
	ui_manager_(0) {
	app_ = this;
}

Downwash::~Downwash() {
	Destroy();

	UiCure::Shutdown();
	uitbc::Shutdown();
	uilepra::Shutdown();
}

void Downwash::Init() {
	uilepra::Init();
	uitbc::Init();
	UiCure::Init();

#if defined(LEPRA_IOS)
	Thread::Sleep(3.0);	// Wait a bit longer so Pixel Doctrine splash is visible.
	CGSize __size = [UIScreen mainScreen].bounds.size;
	int __scale = [[UIScreen mainScreen] scale];
	const int display_width = __size.height * __scale;
	const int display_height = __size.width * __scale;
	bool display_full_screen = true;
#elif defined(EMULATE_TOUCH)
	const int display_width = 1024;
	const int display_height = 768;
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
	v_override(UiCure::GetSettings(), kRtvarUi2DPaintmode, "AlphaBlend");
	v_override(UiCure::GetSettings(), kRtvarUi3DPixelshaders, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablelights, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnabletrilinearfiltering, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablebilinearfiltering, false);
	//v_override(UiCure::GetSettings(), kRtvarUi3DEnablemipmapping, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablemipmapping, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DFov, 60.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DClipnear, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DClipfar, 1000.0);
	//v_override(UiCure::GetSettings(), kRtvarUi3DShadows, "None");
	v_override(UiCure::GetSettings(), kRtvarUi3DShadows, "Force:Volumes");
	v_override(UiCure::GetSettings(), kRtvarUi3DClearred, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCleargreen, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DClearblue, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientred, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientgreen, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientblue, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUiSoundRolloff, 0.1);
	v_override(UiCure::GetSettings(), kRtvarUiSoundDoppler, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUiSoundMusicvolume, 0.6);

#if !defined(LEPRA_TOUCH) && !defined(EMULATE_TOUCH)
	v_override(UiCure::GetSettings(), kRtvarUi3DPixelshaders, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnabletrilinearfiltering, true);
	//v_override(UiCure::GetSettings(), kRtvarUi3DEnablemipmapping, true);
	//v_override(UiCure::GetSettings(), kRtvarUi3DShadows, "Force:VolumesAndMaps");
#endif // Computer

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	v_override(UiCure::GetSettings(), kRtvarNetworkEnableopenserver, false);
	v_override(UiCure::GetSettings(), kRtvarNetworkConnectTimeout, 4.0);
	v_override(UiCure::GetSettings(), kRtvarNetworkLoginTimeout, 4.0);

	ui_manager_ = new UiCure::GameUiManager(UiCure::GetSettings(), &drag_manager_);

	Parent::Init();
}

void Downwash::Destroy() {
	Parent::Destroy();
	delete ui_manager_;
	ui_manager_ = 0;
}

LogListener* Downwash::CreateConsoleLogListener() const {
	return new StdioConsoleLogListener(LogListener::kFormatThreadex);
}



bool Downwash::MainLoop() {
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	animated_app_ = [[AnimatedApp alloc] init:ui_manager_->GetCanvas()];
	return true;
#endif // !iOS/iOS
}



void Downwash::Suspend(bool hard) {
	game_ticker_->Suspend(hard);
	//if (music_player_)
	//{
	//	music_player_->Pause();
	//}
	//DoPause();
	ui_manager_->GetSoundManager()->Suspend();
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
}

void Downwash::Resume(bool hard) {
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
	ui_manager_->GetSoundManager()->Resume();
	game_ticker_->Resume(hard);
	//if (music_player_)
	//{
	//	music_player_->Stop();
	//	music_player_->Playback();
	//}
}



str Downwash::GetTypeName() const {
	return "Client";
}

str Downwash::GetVersion() const {
	return kPlatformVersion;
}

cure::ApplicationTicker* Downwash::CreateTicker() const {
	return new DownwashTicker(ui_manager_, resource_manager_, 200, 5, 5);
}



Downwash* Downwash::app_;
loginstance(kGame, Downwash);



}
