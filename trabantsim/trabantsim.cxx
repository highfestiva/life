
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../cure/include/runtimevariable.h"
#include "../lepra/include/lepraos.h"
#include "../lepra/include/systemmanager.h"
#include "../life/lifeclient/gameclientslavemanager.h"
#include "../life/lifeserver/masterserverconnection.h"
#include "../life/lifeapplication.h"
#include "../uicure/include/uicure.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uilepra/include/uicore.h"
#include "../uilepra/include/uisoundmanager.h"
#include "../uilepra/include/uitouchdrag.h"
#ifdef LEPRA_IOS
#include "../uilepra/include/mac/eaglview.h"
#include "../uilepra/include/mac/rotatingcontroller.h"
#include "../uilepra/include/mac/uimacdisplaymanager.h"
#include "../lepra/include/posix/maclog.h"
#include "pythonrunner.h"
#endif // iOS
#include "../uitbc/include/uitbc.h"
#include "animatedapp.h"
#include "trabantsimticker.h"
#include "rtvar.h"
#include "version.h"



LEPRA_RUN_APPLICATION(TrabantSim::TrabantSim, uilepra::UiMain);



namespace TrabantSim {



void FoldSuspendSimulator() {
	TrabantSim::app_->FoldSimulator();
	TrabantSim::app_->Suspend(false);
}

void FoldSimulator() {
	TrabantSim::app_->FoldSimulator();
}

void UnfoldSimulator() {
	TrabantSim::app_->UnfoldSimulator();
}

void DidSyncFiles() {
	TrabantSim::app_->DidSyncFiles();
}

bool ConnectQuery(const str& hostname) {
	return TrabantSim::app_->ConnectQuery(hostname);
}


TrabantSim* TrabantSim::GetApp() {
	return app_;
}



TrabantSim::TrabantSim(const strutil::strvec& argument_list):
	Parent("trabant", argument_list),
	active_counter_(-100),
	is_in_tick_(false),
	ui_manager_(0) {
	app_ = this;
}

TrabantSim::~TrabantSim() {
	Destroy();

	UiCure::Shutdown();
	uitbc::Shutdown();
	uilepra::Shutdown();
}

void TrabantSim::Init() {
	uilepra::Init();
	uitbc::Init();
	UiCure::Init();

#if defined(LEPRA_IOS)
	CGSize __size = [UIScreen mainScreen].bounds.size;
	int __scale = [[UIScreen mainScreen] scale];
	int display_width = __size.width * __scale;
	int display_height = __size.height * __scale;
	if (display_height > display_width) {
		// Phone might start up in portrait, but our game will always be in landscape mode.
		std::swap(display_width, display_height);
	}
	const double font_height = 30.0*__scale;
#else // Computer L&F
	const int display_width = 760;
	const int display_height = 524;
	const double font_height = 30.0;
#endif // touch / Computer L&F
	v_override(UiCure::GetSettings(), kRtvarUi2DFontheight, font_height);
	int display_bpp = 0;
	int display_frequency = 0;
	bool display_full_screen = false;
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
	v_override(UiCure::GetSettings(), kRtvarUi3DEnableclear, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DPixelshaders, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablelights, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnabletrilinearfiltering, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablebilinearfiltering, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablemipmapping, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DFov, 20.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DClipnear, 0.1);
	v_override(UiCure::GetSettings(), kRtvarUi3DClipfar, 20.0);
	//v_override(UiCure::GetSettings(), kRtvarUi3DShadows, "None");
	v_override(UiCure::GetSettings(), kRtvarUi3DShadows, "Force:Volumes");
	v_override(UiCure::GetSettings(), kRtvarUi3DClearred, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCleargreen, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DClearblue, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientred, 0.4);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientgreen, 0.4);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientblue, 0.4);
	v_override(UiCure::GetSettings(), kRtvarUiSoundRolloff, 0.1);
	v_override(UiCure::GetSettings(), kRtvarUiSoundDoppler, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUiSoundMusicvolume, 0.6);

	v_override(UiCure::GetSettings(), kRtvarCtrlEmulatetouch, true);

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	v_override(UiCure::GetSettings(), kRtvarNetworkEnableopenserver, false);
	v_override(UiCure::GetSettings(), kRtvarNetworkConnectTimeout, 4.0);
	v_override(UiCure::GetSettings(), kRtvarNetworkLoginTimeout, 4.0);

	ui_manager_ = new UiCure::GameUiManager(UiCure::GetSettings(), &drag_manager_);

	Parent::Init();
}

void TrabantSim::Destroy() {
	Parent::Destroy();
	delete ui_manager_;
	ui_manager_ = 0;
}

bool TrabantSim::MainLoop() {
#ifndef LEPRA_IOS
	active_counter_ = 1;
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	animated_app_ = [[AnimatedApp alloc] init:ui_manager_->GetCanvas()];
	return true;
#endif // !iOS/iOS
}

bool TrabantSim::Tick() {
	if (active_counter_ > 0) {
		is_in_tick_ = true;
		const bool ok = Parent::Tick();
		is_in_tick_ = false;
		if (active_counter_ <= 0) {
			const int c = active_counter_;
			active_counter_ = 1;
			Suspend(false);
			active_counter_ = c;
		}
		return ok;
	}
	return true;
}



void TrabantSim::Resume(bool hard) {
	if (hard) {
		// If we're only coming back from background all we don't
		// want to start ticking the simulator.
		game_ticker_->Resume(true);	// Resume's hard/soft has no effect.
		active_counter_ = 0;
		return;
	}

	if (++active_counter_ != 1) {
		return;
	}

	game_ticker_->Resume(false);	// Resume sets some internal states of the simulator state machine.
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
	ui_manager_->GetSoundManager()->Resume();
}

void TrabantSim::Suspend(bool hard) {
	if (active_counter_ == 0 && !hard) {
		return;
	}
	active_counter_ = 0;
	if (is_in_tick_ && !hard) {
		return;
	}
#ifdef LEPRA_IOS
	const bool is_running_locally = PythonRunner::IsRunning();
#endif // iOS
	game_ticker_->Suspend(hard);	// Hard means cut the chord, soft just sends "disconnect" to remote end.
	ui_manager_->GetSoundManager()->Suspend();
#ifdef LEPRA_IOS
	[animated_app_ tick_];
	if (!is_running_locally) {
		FoldSimulator();
	}
#endif // iOS
}

void TrabantSim::FoldSimulator() {
#ifdef LEPRA_IOS
	dispatch_block_t Fold = ^
	{
		const int was_active = active_counter_;
		str std_out = PythonRunner::GetStdOut();
		Suspend(false);
		//UIWindow* window = ((uilepra::MacDisplayManager*)TrabantSim::TrabantSim::app_->ui_manager_->GetDisplayManager())->GetWindow();
		//[window setHidden:YES];
		[[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationNone];
		//[TrabantSim::TrabantSim::app_->animated_app_.window makeKeyAndVisible];
		PythonRunner::Break();
		if (!std_out.empty() && was_active > 0) {
			[TrabantSim::TrabantSim::app_->animated_app_ handleStdOut:std_out];
		} else {
			[TrabantSim::TrabantSim::animated_app_ if_game];
		}
	};
	if ([NSThread isMainThread]) {
		Fold();
	} else {
		dispatch_sync(dispatch_get_main_queue(), Fold);
	}
#endif // iOS
}

void TrabantSim::UnfoldSimulator() {
#ifdef LEPRA_IOS
	if ([TrabantSim::TrabantSim::app_->animated_app_ showingSimulator_]) {
		return;
	}

	dispatch_block_t Unfold = ^
	{
		//[TrabantSim::TrabantSim::animated_app_.window setHidden:YES];
		//UIWindow* window = ((uilepra::MacDisplayManager*)TrabantSim::TrabantSim::app_->ui_manager_->GetDisplayManager())->GetWindow();
		//if (!window.rootViewController)
		UIViewController* controller = [[UIViewController alloc] init];
		controller.view = [EAGLView sharedView_];
		//window.rootViewController = controller;
		[TrabantSim::TrabantSim::app_->animated_app_ pushSimulatorController:controller];
		//[window makeKeyAndVisible];
		TrabantSim::TrabantSim::app_->active_counter_ = 0;	// Make sure no lost event causes a halt.
		TrabantSim::TrabantSim::app_->Resume(false);
		//[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
		PythonRunner::ClearStdOut();
	};
	if ([NSThread isMainThread]) {
		Unfold();
	} else {
		dispatch_sync(dispatch_get_main_queue(), Unfold);
	}
#endif // iOS
}

void TrabantSim::DidSyncFiles() {
#ifdef LEPRA_IOS
	dispatch_async(dispatch_get_main_queue(), ^{
		[TrabantSim::TrabantSim::animated_app_.__controller reloadPrototypes];
	});
#endif // iOS
}

bool TrabantSim::ConnectQuery(const str& hostname) {
	(void)hostname;
#ifdef LEPRA_IOS
	str denied_hosts;
	v_get(denied_hosts, =, UiCure::GetSettings(), "Simulator.DeniedHosts", "");
	strutil::strvec hosts = strutil::Split(denied_hosts, ":");
	if (std::find(hosts.begin(), hosts.end(), hostname) != hosts.end()) {
		return false;
	}
	str allowed_hosts;
	v_get(allowed_hosts, =, UiCure::GetSettings(), "Simulator.AllowedHosts", "");
	hosts = strutil::Split(allowed_hosts, ":");
	if (std::find(hosts.begin(), hosts.end(), hostname) != hosts.end()) {
		return true;
	}
	// TODO: check if address already present in registered hosts.
	dispatch_async(dispatch_get_main_queue(), ^{
		[TrabantSim::TrabantSim::animated_app_ showNetworkControlFor:MacLog::Encode(hostname)];
	});
	return false;
#else // !iOS
	return true;
#endif // iOS
}

	void TrabantSim::SavePurchase() {
}



str TrabantSim::GetTypeName() const {
	return "Simulator";
}

str TrabantSim::GetVersion() const {
	return kTrabantVersion;
}

cure::ApplicationTicker* TrabantSim::CreateTicker() const {
	return new TrabantSimTicker(ui_manager_, resource_manager_, 200, 5, 5);
}



TrabantSim* TrabantSim::app_;
loginstance(kGame, TrabantSim);



}
