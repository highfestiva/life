
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
#include "../uitbc/include/uitbc.h"
#include "impuzzable.h"
#include "impuzzableticker.h"
#include "rtvar.h"
#include "version.h"



namespace Impuzzable {



class Impuzzable: public life::Application {
	typedef life::Application Parent;
public:
	static Impuzzable* GetApp();

	Impuzzable(const strutil::strvec& argument_list);
	virtual ~Impuzzable();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();

	virtual void Suspend();
	virtual void Resume();
	void SavePurchase();

	str GetTypeName() const;
	str GetVersion() const;
	cure::ApplicationTicker* CreateTicker() const;

	static Impuzzable* app_;
#ifdef LEPRA_TOUCH
	AnimatedApp* animated_app_;
#endif // touch

	UiCure::GameUiManager* ui_manager_;
	uilepra::touch::DragManager drag_manager_;

	logclass();
};



}



LEPRA_RUN_APPLICATION(Impuzzable::Impuzzable, uilepra::UiMain);



namespace Impuzzable {



Impuzzable* Impuzzable::GetApp() {
	return app_;
}



Impuzzable::Impuzzable(const strutil::strvec& argument_list):
	Parent("Impuzzable", argument_list),
	ui_manager_(0) {
	app_ = this;
}

Impuzzable::~Impuzzable() {
	Destroy();

	UiCure::Shutdown();
	uitbc::Shutdown();
	uilepra::Shutdown();
}

void Impuzzable::Init() {
	uilepra::Init();
	uitbc::Init();
	UiCure::Init();

#if defined(LEPRA_IOS)
	Thread::Sleep(3.0);	// Wait a bit longer so Pixel Doctrine splash is visible.
	CGSize __size = [UIScreen mainScreen].impuzzables.size;
	int __scale = [[UIScreen mainScreen] scale];
	const int display_width = __size.height * __scale;
	const int display_height = __size.width * __scale;
	bool display_full_screen = true;
#else // Computer L&F
	const int display_width = 760;
	const int display_height = 524;
	bool display_full_screen = false;
#endif // touch / Computer L&F
	int display_bpp = 0;
	int display_frequency = 0;
	double physical_screen_size = 24.0;	// An average computer's physical screen size (inches across).
	v_override(UiCure::GetSettings(), kRtvarUiDisplayRenderengine, "OpenGL");
	v_override(UiCure::GetSettings(), kRtvarUiDisplayWidth, display_width);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayHeight, display_height);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayBitsperpixel, display_bpp);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayFrequency, display_frequency);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayFullscreen, display_full_screen);
	v_override(UiCure::GetSettings(), kRtvarUiDisplayOrientation, "Fixed");
	v_override(UiCure::GetSettings(), kRtvarUiDisplayPhysicalsize, physical_screen_size);

	v_override(UiCure::GetSettings(), kRtvarUiSoundEngine, "OpenAL");

	v_override(UiCure::GetSettings(), kRtvarUiDisplayEnablevsync, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnableclear, false);
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
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientred, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientgreen, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUi3DAmbientblue, 0.5);
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

void Impuzzable::Destroy() {
	Parent::Destroy();
	delete ui_manager_;
	ui_manager_ = 0;
}

bool Impuzzable::MainLoop() {
#ifndef LEPRA_IOS
	return Parent::MainLoop();
#else // iOS
	// iOS has uses timer callbacks instead of a main loop.
	animated_app_ = [[AnimatedApp alloc] init:ui_manager_->GetCanvas()];
	return true;
#endif // !iOS/iOS
}



void Impuzzable::Suspend() {
	game_ticker_->Suspend(true);
	ui_manager_->GetSoundManager()->Suspend();
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
}

void Impuzzable::Resume() {
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
	ui_manager_->GetSoundManager()->Resume();
	game_ticker_->Resume(true);
}

void Impuzzable::SavePurchase() {
	life::GameClientMasterTicker* ticker = (life::GameClientMasterTicker*)game_ticker_;
	life::GameClientSlaveManager* manager = ticker->GetSlave(0);
	v_set(manager->GetVariableScope(), kRtvarGameLevelshapealternate, true);
	v_set(manager->GetVariableScope(), kRtvarGameRunads, false);
	ticker->SaveRtvars(ui_manager_->GetVariableScope());
}

void Impuzzable__ShowAd() {
#ifdef LEPRA_IOS
        [Impuzzable::GetApp()->animated_app_ ad_];
#endif // iOS
}

void Impuzzable__Buy() {
#ifdef LEPRA_IOS
	[Impuzzable::GetApp()->animated_app_ startPurchase:@"ImpuzzableFullInAppPurchase"];
#endif // iOS
}



str Impuzzable::GetTypeName() const {
	return "Client";
}

str Impuzzable::GetVersion() const {
	return kGameVersion;
}

cure::ApplicationTicker* Impuzzable::CreateTicker() const {
	return new ImpuzzableTicker(ui_manager_, resource_manager_, 200, 5, 5);
}



Impuzzable* Impuzzable::app_;
loginstance(kGame, Impuzzable);



}
