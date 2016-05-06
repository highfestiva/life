
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <list>
#include "../cure/include/hiscoreagent.h"
#include "../cure/include/runtimevariable.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/application.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/fileopener.h"
#include "../lepra/include/loglistener.h"
#include "../lepra/include/network.h"
#include "../lepra/include/obfuxator.h"
#include "../lepra/include/random.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "../tbc/include/physicsengine.h"
#include "../uicure/include/uicppcontextobject.h"
#include "../uicure/include/uicure.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiiconbutton.h"
#include "../uicure/include/uimusicplayer.h"
#include "../uicure/include/uiruntimevariablename.h"
#include "../uicure/include/uisound.h"
#include "../uilepra/include/mac/uiiosinput.h"
#include "../uilepra/include/uicore.h"
#include "../uilepra/include/uidisplaymanager.h"
#include "../uilepra/include/uiinput.h"
#include "../uilepra/include/uiopenglextensions.h"	// Included to get the gl-headers.
#include "../uilepra/include/uisoundmanager.h"
#include "../uilepra/include/uisoundstream.h"
#include "../uitbc/include/gui/uicustombutton.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uimessagedialog.h"
#include "../uitbc/include/gui/uiscrollbar.h"
#include "../uitbc/include/gui/uitextfield.h"
#include "../uitbc/include/uifontmanager.h"
#include "vehicle.h"
#include "game.h"
#ifdef LEPRA_MAC
#include "../lepra/include/posix/maclog.h"
#endif // iOS



#define UIKEY(name)			uilepra::InputManager::IN_KBD_##name
#define BGCOLOR_DIALOG			Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG			Color(170, 170, 170, 255)
#define ICONBTN(i,n)			new UiCure::IconButton(ui_manager_, resource_manager_, i, n)
#define ICONBTNA(i,n)			ICONBTN(i, n)
#define kContentLevels			"levels"
#define kContentVehicles		"vehicles"
#define kRtvarContentLevels		"Content.Levels"
#define kRtvarContentVehicles		"Content.Vehicles"
#define kRtvarHiscoreName		"Hiscore.Name"	// Last entered name.



namespace tirefire {



FingerMoveList g_finger_move_list;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
const str kPlatform = "touch";
#else // Computer.
const str kPlatform = "computer";
#endif // touch / computer.



class HiscoreTextField;

class App: public Application, public uilepra::DisplayResizeObserver, public uilepra::KeyCodeInputObserver {
public:
	typedef Application Parent;

	App(const strutil::strvec& argument_list);
	virtual ~App();

	static App* GetApp();

	bool Open();
	void Close();
	virtual void Init();
	virtual int Run();
	void DisplayLogo();
	bool Poll();
	void PollTaps();
	void PrintText(const str& text, float angle, int center_x, int center_y) const;
	void Layout();
	void MainMenu();
	void UpdateHiscore(bool error);
	void HiscoreMenu(int direction);
	void EnterHiscore(const str& message, const Color& color);
	void SuperReset();
	void CreateHiscoreAgent();
	void Purchase(const str& product_name);
	void SetIsPurchasing(bool is_purchasing);

	virtual void Suspend();
	virtual void Resume();

	bool Steer(uilepra::InputManager::KeyCode key_code, float factor);
	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
#if !defined(LEPRA_TOUCH) && defined(LEPRA_TOUCH_LOOKANDFEEL)
	void OnMouseInput(uilepra::InputElement* element);
	bool is_mouse_down_;
#endif // Computer emulating touch device
	virtual int PollTap(FingerMovement& movement);

	void OnResize(int _width, int _height);
	void OnMinimize();
	void OnMaximize(int _width, int _height);

	void OnAction(uitbc::Button* button);
	void OnTapSound(uitbc::Button* button);
	void OnOk(uitbc::Button* button);
	void OnMainMenuAction(uitbc::Button* button);
	void OnEnterHiscoreAction(uitbc::Button* button);
	void OnLevelAction(uitbc::Button* button);
	void OnVehicleAction(uitbc::Button* button);
	void OnHiscoreAction(uitbc::Button* button);
	void OnPreHiscoreAction(uitbc::Button* button);
	void OnPreEnterAction(uitbc::Button* button);
	void OnCreditsAction(uitbc::Button* button);
	void DoPause();
	void OnPauseClickWithSound(uitbc::Button* button);
	void OnPauseAction(uitbc::Button* button);
	void OnGetiPhoneClick(uitbc::Button*);
	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource);
	void SoundLoadCallback(UiCure::UserSound2dResource* resource);

	typedef void (App::*ButtonAction)(uitbc::Button*);
	uitbc::Dialog* CreateTbcDialog(ButtonAction action);
	static uitbc::Button* CreateButton(const str& text, const Color& color, uitbc::Component* parent);
	void Transpose(int& x, int& y, float& angle) const;
	void Transpose(float& x, float& y, float& angle) const;
	static str Int2Str(int number);

	StdioConsoleLogListener console_logger_;
	DebuggerLogListener debug_logger_;

	static App* app_;
#ifdef LEPRA_TOUCH
	AnimatedApp* animated_app_;
#endif // touch
	Game* game_;


	double average_loop_time_;
	double average_fast_loop_time_;
	HiResTimer loop_timer_;

	struct InfoTextData {
		str text_;
		vec2 coord_;
		float angle_;
	};

	bool is_loaded_;
	bool do_layout_;
	cure::ResourceManager* resource_manager_;
	cure::RuntimeVariableScope* variable_scope_;
	UiCure::GameUiManager* ui_manager_;
	UiCure::MusicPlayer* music_player_;
	uitbc::FontManager::FontId big_font_id_;
	uitbc::FontManager::FontId monospaced_font_id_;
	float reverse_and_brake_;
	uitbc::Dialog::Action button_delegate_;
	uitbc::Dialog* dialog_;
	mutable StopWatch start_timer_;
	mutable StopWatch game_over_timer_;
	bool flip_draw_;
	HiResTimer boot_logo_timer_;
	int hiscore_level_index_;
	int hiscore_vehicle_index_;
	HiscoreTextField* hiscore_text_field_;
	cure::HiscoreAgent* hiscore_agent_;
	bool is_purchasing_;
	bool is_money_icon_added_;
	UiCure::UserSound2dResource* tap_click_;
	int my_hiscore_index_;
	unsigned frame_counter_;

	logclass();
};

class HiscoreTextField: public uitbc::TextField {
public:
	typedef uitbc::TextField Parent;
	App* app_;
	HiscoreTextField(Component* top_parent, unsigned border_style, int border_width,
		const Color& color, const str& _name):
		Parent(top_parent, border_style, border_width, color, _name) {
	}
	virtual bool OnChar(tchar _c) {
		bool b = false;
		if (GetText().length() < 13 || _c < ' ') {	// Limit character length.
			b = Parent::OnChar(_c);
			if (GetText().length() > 13) {	// Shouln't happen...
				SetText(GetText().substr(0, 13));
			}
		}
		if (_c == '\r' || _c == '\n') {
			const str _text = strutil::Strip(GetText(), " \t\v\r\n");
			if (!_text.empty()) {
				app_->dialog_->Dismiss();
				b = true;
			}
		}
		return b;
	}
};



struct IsPressing {
	IsPressing(int tag): tag_(tag) {}
	bool operator()(const FingerMovement& touch) { return (touch.tag_ == tag_); }
	int tag_;
};



}



LEPRA_RUN_APPLICATION(tirefire::App, uilepra::UiMain);



namespace tirefire {



App::App(const strutil::strvec& argument_list):
	Application(argument_list),
	game_(0),
	variable_scope_(0),
	average_loop_time_(1.0/(kFps+1)),
	average_fast_loop_time_(1.0/(kFps+1)),
	is_loaded_(false),
	do_layout_(true),
	music_player_(0),
	big_font_id_(uitbc::FontManager::kInvalidFontid),
	monospaced_font_id_(uitbc::FontManager::kInvalidFontid),
	reverse_and_brake_(0),
	dialog_(0),
	flip_draw_(false),
	hiscore_level_index_(0),
	hiscore_vehicle_index_(0),
	hiscore_text_field_(0),
	hiscore_agent_(0),
	is_purchasing_(false),
	is_money_icon_added_(false),
	my_hiscore_index_(0),
	frame_counter_(0) {
	app_ = this;
}

App::~App() {
	delete hiscore_agent_;
	hiscore_agent_ = 0;
	variable_scope_ = 0;
	UiCure::Shutdown();
	uitbc::Shutdown();
	uilepra::Shutdown();
}

App* App::GetApp() {
	return app_;
}

bool App::Open() {
#ifdef LEPRA_TOUCH
	CGSize __size = [UIScreen mainScreen].bounds.size;
	const int display_width = __size.height;
	const int display_height = __size.width;
	bool display_full_screen = true;
#else // !touch
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	const int display_width = 480;
	const int display_height = 320;
#else // !touch L&F
	const int display_width = 760;
	const int display_height = 524;
#endif // touch / Computer L&F
	bool display_full_screen = false;
#endif // touch/!touch
	int display_bpp = 0;
	int display_frequency = 0;
	v_set(variable_scope_, kRtvarUiDisplayRenderengine, "OpenGL");
	v_set(variable_scope_, kRtvarUiDisplayWidth, display_width);
	v_set(variable_scope_, kRtvarUiDisplayHeight, display_height);
	v_set(variable_scope_, kRtvarUiDisplayBitsperpixel, display_bpp);
	v_set(variable_scope_, kRtvarUiDisplayFrequency, display_frequency);
	v_set(variable_scope_, kRtvarUiDisplayFullscreen, display_full_screen);
	v_set(variable_scope_, kRtvarUiDisplayOrientation, "AllowUpsideDown");

	v_set(variable_scope_, kRtvarUiSoundEngine, "OpenAL");

	v_set(variable_scope_, kRtvarUiDisplayEnablevsync, false);
	v_set(variable_scope_, kRtvarUi3DPixelshaders, false);
	v_set(variable_scope_, kRtvarUi3DEnablelights, true);
	v_set(variable_scope_, kRtvarUi3DEnabletrilinearfiltering, false);
	v_set(variable_scope_, kRtvarUi3DEnablebilinearfiltering, false);
	v_set(variable_scope_, kRtvarUi3DEnablemipmapping, false);
	v_set(variable_scope_, kRtvarUi3DFov, 60.0);
	v_set(variable_scope_, kRtvarUi3DClipnear, 1.0);
	v_set(variable_scope_, kRtvarUi3DClipfar, 1000.0);
	v_set(variable_scope_, kRtvarUi3DShadows, "None");
	v_set(variable_scope_, kRtvarUi3DAmbientred, 0.5);
	v_set(variable_scope_, kRtvarUi3DAmbientgreen, 0.5);
	v_set(variable_scope_, kRtvarUi3DAmbientblue, 0.5);
	v_set(variable_scope_, kRtvarUiSoundRolloff, 0.7);
	v_set(variable_scope_, kRtvarUiSoundDoppler, 1.0);

#ifndef LEPRA_TOUCH_LOOKANDFEEL
	v_set(variable_scope_, kRtvarUi3DPixelshaders, true);
	v_set(variable_scope_, kRtvarUiSoundRolloff, 0.5);
	v_set(variable_scope_, kRtvarUi3DEnabletrilinearfiltering, true);
	v_set(variable_scope_, kRtvarUi3DEnablemipmapping, true);
	v_set(variable_scope_, kRtvarUi3DShadows, "Force:Volume");
#endif // !touch

	v_set(variable_scope_, kRtvarContentLevels, false);
	v_set(variable_scope_, kRtvarContentVehicles, false);
#ifdef LEPRA_IOS
	[AnimatedApp updateContent];
#endif // iOS

	ui_manager_ = new UiCure::GameUiManager(variable_scope_, 0);
	bool ok = ui_manager_->OpenDraw();
	if (ok) {
		uilepra::Core::ProcessMessages();
		ui_manager_->GetPainter()->ResetClippingRect();
		ui_manager_->GetPainter()->Clear(BLACK);
		DisplayLogo();
		boot_logo_timer_.PopTimeDiff();
	}
	if (ok) {
		ok = ui_manager_->OpenRest();
	}
	if (ok) {
		ui_manager_->GetDisplayManager()->SetCaption("Tire Fire");
		ui_manager_->GetDisplayManager()->AddResizeObserver(this);
		ui_manager_->GetInputManager()->AddKeyCodeInputObserver(this);
#if !defined(LEPRA_TOUCH) && defined(LEPRA_TOUCH_LOOKANDFEEL)
		is_mouse_down_ = false;
		ui_manager_->GetInputManager()->GetMouse()->AddFunctor(new uilepra::TInputFunctor<App>(this, &App::OnMouseInput));
#endif // Computer emulating touch
	}
	if (ok) {
		uitbc::FontManager::FontId default_font_id = ui_manager_->GetFontManager()->GetActiveFontId();
		big_font_id_ = ui_manager_->GetFontManager()->QueryAddFont("Helvetica", 24);
		monospaced_font_id_ = ui_manager_->GetFontManager()->QueryAddFont("Courier New", 14);
		ui_manager_->GetFontManager()->SetActiveFont(default_font_id);
	}
	if (ok) {
		/*music_player_ = new UiCure::MusicPlayer(ui_manager_->GetSoundManager());
		music_player_->SetVolume(0.5f);
		music_player_->SetSongPauseTime(9, 15);
		music_player_->AddSong("ButterflyRide.xm");
		music_player_->AddSong("BehindTheFace.xm");
		music_player_->AddSong("BrittiskBensin.xm");
		music_player_->AddSong("DontYouWantMe'97.xm");
		music_player_->AddSong("CloseEncounters.xm");
		music_player_->Shuffle();
		ok = music_player_->Playback();*/
	}
	if (ok) {
		struct ResourceOpener: public FileOpener {
			cure::ResourceManager* resource_manager_;

			ResourceOpener(cure::ResourceManager* resource_manager):
				resource_manager_(resource_manager) {
			}

			virtual File* Open(const str& filename) {
				return resource_manager_->QueryFile(filename);
			}
		};
		ui_manager_->GetSoundManager()->SetFileOpener(new ResourceOpener(resource_manager_));
	}
	if (ok) {
		tap_click_ = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
		tap_click_->Load(resource_manager_, "tap.wav",
			UiCure::UserSound2dResource::TypeLoadCallback(this, &App::SoundLoadCallback));
	}

	uilepra::Core::ProcessMessages();
	return (ok);
}

void App::Close() {
	ui_manager_->GetInputManager()->RemoveKeyCodeInputObserver(this);
	ui_manager_->GetDisplayManager()->RemoveResizeObserver(this);

	// Poll system to let go of old windows.
	uilepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	uilepra::Core::ProcessMessages();

	delete music_player_;
	music_player_ = 0;

	delete game_;
	game_ = 0;

	ui_manager_->DeleteDesktopWindow();

	delete resource_manager_;	// Resource manager lives long enough for all volontary resources to disappear.
	resource_manager_ = 0;

	delete ui_manager_;
	ui_manager_ = 0;

	// Poll system to let go of old windows.
	uilepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	uilepra::Core::ProcessMessages();
}

void App::Init() {
	deb_assert(Int2Str(-123) == "-123");
	deb_assert(Int2Str(-1234) == "-1,234");
	deb_assert(Int2Str(-12345) == "-12,345");
	deb_assert(Int2Str(-123456) == "-123,456");
	deb_assert(Int2Str(-1234567) == "-1,234,567");
	deb_assert(Int2Str(+123) == "123");
	deb_assert(Int2Str(+1234) == "1,234");
	deb_assert(Int2Str(+12345) == "12,345");
	deb_assert(Int2Str(+123456) == "123,456");
	deb_assert(Int2Str(+1234567) == "1,234,567");
}


int App::Run() {
	uilepra::Init();
	uitbc::Init();
	UiCure::Init();
	Network::Start();

	const str log_name = Path::JoinPath(SystemManager::GetIoDirectory("TireFire"), "log", "txt");
	FileLogListener file_logger(log_name);
	{
		LogType::GetLogger(LogType::SUB_ROOT)->SetupBasicListeners(&console_logger_, &debug_logger_, &file_logger, 0, 0);
		const std::vector<Logger*> log_array = LogType::GetLoggers();
		std::vector<Logger*>::const_iterator x = log_array.begin();
		for (; x != log_array.end(); ++x) {
			(*x)->SetLevelThreashold(kLevelInfo);
		}
	}

	bool ok = true;
	if (ok) {
		variable_scope_ = UiCure::GetSettings();
		v_set(variable_scope_, kRtvarPhysicsParallel, false);	// Let's do it same on all platforms, so we can render stuff from physics data.
		v_set(variable_scope_, kRtvarPhysicsMicrosteps, 15);
		v_set(variable_scope_, kRtvarPhysicsFps, kFps);
		v_set(variable_scope_, kRtvarPhysicsIsfixedfps, true);
		//v_set(variable_scope_, kRtvarUi3DEnablelights, false);
	}
	if (ok) {
		resource_manager_ = new cure::ResourceManager(1);
	}
	if (ok) {
		ok = Open();
	}
	if (ok) {
		game_ = new Game(ui_manager_, variable_scope_, resource_manager_);
		ok = game_->SetLevelName("level_1");
	}
	if (ok) {
		game_->cure::GameTicker::GetTimeManager()->Tick();
		game_->cure::GameTicker::GetTimeManager()->Clear(1);
		ok = resource_manager_->InitDefault();
	}
	if (ok) {
		boot_logo_timer_.EnableShadowCounter(true);
		loop_timer_.EnableShadowCounter(true);
		game_over_timer_.EnableShadowCounter(true);
		start_timer_.EnableShadowCounter(true);
	}
	loop_timer_.PopTimeDiff();
#ifndef LEPRA_IOS
	bool quit = (SystemManager::GetQuitRequest() != 0);
	while (!quit) {
		quit = !Poll();
	}
	Close();
	return 0;
#else // iOS
	animated_app_ = [[AnimatedApp alloc] init:ui_manager_->GetCanvas()];
	return 0;
#endif // !iOS/iOS
}

void App::DisplayLogo() {
	UiCure::PainterImageResource* logo = new UiCure::PainterImageResource(ui_manager_, resource_manager_, "logo.png", UiCure::PainterImageResource::kReleaseFreeBuffer);
	if (logo->Load()) {
		if (logo->PostProcess() == cure::kResourceLoadComplete) {
			//ui_manager_->BeginRender(vec3(0, 1, 0));
			ui_manager_->PreparePaint(true);
			const Canvas* canvas = ui_manager_->GetCanvas();
			const Canvas* image = logo->GetRamData();
			ui_manager_->GetPainter()->DrawImage(logo->GetUserData(0), canvas->GetWidth()/2 - image->GetWidth()/2, canvas->GetHeight()/2 - image->GetHeight()/2);
			ui_manager_->GetDisplayManager()->UpdateScreen();
		}
	}
	delete logo;
}

bool App::Poll() {
	bool ok = true;
	if (ok) {
		HiResTimer::StepCounterShadow();
	}
	if (ok) {
		if (2.0 - boot_logo_timer_.QueryTimeDiff() > 0) {
			Thread::Sleep(0.1);
			uilepra::Core::ProcessMessages();
			resource_manager_->Tick();
			return true;
		}
	}
	if (ok) {
		const double instant_loop_time = loop_timer_.QueryTimeDiff();
		if (++frame_counter_ > 2) {
			// Adjust frame rate, or it will be hopelessly high... on most reasonable platforms.
			average_loop_time_ = lepra::Math::Lerp(average_loop_time_, instant_loop_time, 0.05);
			average_fast_loop_time_ = lepra::Math::Lerp(average_fast_loop_time_, instant_loop_time, 0.7);
		}
		const double delay_time = 1.0/kFps - average_loop_time_;
		if (delay_time > 0) {
			Thread::Sleep(delay_time-0.001);
			uilepra::Core::ProcessMessages();
		}
		loop_timer_.PopTimeDiff();
		if (instant_loop_time >= 1.0/kFps && average_fast_loop_time_ > 1.0/kFps) {
			// This should be a temporary slow-down, due to something like rendering of
			// lots of transparent OpenGL triangles.
			int frame_rate = (int)(1.0/average_fast_loop_time_);
			if (frame_rate < 10) {
				frame_rate = 10;
			}
			v_set(variable_scope_, kRtvarPhysicsFps, frame_rate);
		} else {
			v_set(variable_scope_, kRtvarPhysicsFps, kFps);
			average_fast_loop_time_ = instant_loop_time;	// Immediately drop back sliding average to current value when kFps is normal again.
		}

	}
	if (ok) {
		// Download any pending hiscore request.
		if (hiscore_agent_) {
			cure::ResourceLoadState load_state = hiscore_agent_->Poll();
			if (load_state != cure::kResourceLoadInProgress) {
				switch (hiscore_agent_->GetAction()) {
					case cure::HiscoreAgent::kActionDownloadList: {
						UpdateHiscore(load_state != cure::kResourceLoadComplete);
						delete hiscore_agent_;
						hiscore_agent_ = 0;
					} break;
					case cure::HiscoreAgent::kActionUploadScore: {
						my_hiscore_index_ = hiscore_agent_->GetUploadedPlace();
						delete hiscore_agent_;
						hiscore_agent_ = 0;
						if (load_state == cure::kResourceLoadComplete) {
							HiscoreMenu(+1);
						} else {
							EnterHiscore("Please retry; score server obstipated", LIGHT_RED);
						}
					} break;
					default: {
						delete hiscore_agent_;
						hiscore_agent_ = 0;
						log_.AError("Oops! Completed hiscore communication, but something went wrong.");
						deb_assert(false);
						MainMenu();	// Well... assume some super-shitty state...
					} break;
				}
			}
		}
	}
	if (ok) {
		// Take care of the "brake and reverse" steering.
		if (game_->GetVehicle() && game_->GetVehicle()->IsLoaded()) {
			const bool is_moving_forward = (game_->GetVehicle()->GetForwardSpeed() > 3.0f*SCALE_FACTOR);
			if (reverse_and_brake_) {
				game_->GetVehicle()->SetEnginePower(0, is_moving_forward? 0 : -1*reverse_and_brake_);	// Reverse.
				game_->GetVehicle()->SetEnginePower(2, is_moving_forward? +1*reverse_and_brake_ : 0);	// Brake.
			}
		}
	}
	if (ok) {
		ok = (SystemManager::GetQuitRequest() == 0);
	}
	if (!is_loaded_ && resource_manager_->IsLoading()) {
		resource_manager_->Tick();
		return ok;
	}
	is_loaded_ = true;
	if (ok && do_layout_) {
		Layout();
	}
	if (ok) {
		float r, g, b;
		v_get(r, =(float), variable_scope_, kRtvarUi3DClearred, 0.75);
		v_get(g, =(float), variable_scope_, kRtvarUi3DCleargreen, 0.80);
		v_get(b, =(float), variable_scope_, kRtvarUi3DClearblue, 0.85);
		vec3 _color(r, g, b);
		ui_manager_->BeginRender(_color);
	}
	if (ok) {
		ui_manager_->InputTick();
		PollTaps();
	}
	if (ok) {
		if (game_->GetFlybyMode() == Game::kFlybyInactive) {
			game_->AddScore(+3);
		}
		game_->BeginTick();
	}
	bool render = false;
	if (ok) {
		render = ui_manager_->CanRender();
	}
	if (ok && render) {
		ok = game_->Render();
	}
	if (ok && render) {
		if (dialog_) {
			ui_manager_->PreparePaint(false);
		} else {
			ui_manager_->Paint(false);
		}

		if (game_->GetFlybyMode() == Game::kFlybyInactive &&
			game_->IsScoreCountingEnabled()) {
			ui_manager_->GetPainter()->SetColor(WHITE);
			const str score = "Score: " + Int2Str((int)game_->GetScore());
			int sy = 8 + ui_manager_->GetPainter()->GetFontHeight()/2;
			const int sx = ui_manager_->GetCanvas()->GetWidth()/2;
			PrintText(score, 0, sx, sy);
		}

		game_->Paint();

		if (dialog_) {
			ui_manager_->Paint(false);
		}

		if (is_purchasing_ || hiscore_agent_) {
			ui_manager_->GetPainter()->SetColor(WHITE, 0);
			const str info = is_purchasing_? "Communicating with App Store..." : "Speaking to score server";
			PrintText(info, 0,
				ui_manager_->GetCanvas()->GetWidth()/2,
				ui_manager_->GetCanvas()->GetHeight() - ui_manager_->GetPainter()->GetFontHeight());
		}
	}
	if (ok) {
		game_->PreEndTick();
		game_->WaitPhysicsTick();
		ok = game_->EndTick();
	}
	if (ok) {
		ok = game_->Tick();
	}
	resource_manager_->Tick();
	ui_manager_->EndRender(1.0f/kFps);

	if (music_player_) {
		music_player_->Update();
	}

	if (game_over_timer_.IsStarted()) {
		if (game_over_timer_.QueryTimeDiff() > 11.0) {
			if (game_->GetScore() >= 1000.0) {		// Negative score isn't any good - at least be positive.
				EnterHiscore(str(), FGCOLOR_DIALOG);
			} else {
				// Score wasn't high enough, not allowed in on hiscore list.
				SuperReset();
			}
		}
	}

	return ok;
}

void App::PollTaps() {
	if (is_purchasing_) {
		g_finger_move_list.clear();
		return;
	}

#ifdef LEPRA_TOUCH_LOOKANDFEEL
	UiCure::CppContextObject* vehicle = game_->GetVehicle();
	if (!vehicle || !vehicle->IsLoaded()) {
		return;
	}
	FingerMoveList::iterator x = g_finger_move_list.begin();
	float throttle = 0;
	for (; x != g_finger_move_list.end();) {
		x->tag_ = PollTap(*x);
		if (x->tag_ > 0) {
			++x;
			throttle = 1;
		} else {
			g_finger_move_list.erase(x++);
		}
	}
	if (vehicle->GetPhysics()->GetEngineCount() >= 3) {
		game_->SetThrottle(throttle);
	}
#ifndef LEPRA_TOUCH
	// Test code.
	if (!g_finger_move_list.empty() && !g_finger_move_list.back().is_press_) {
		g_finger_move_list.clear();
	}
#endif // Computer
#endif // touch L&F
}

void App::PrintText(const str& text, float angle, int center_x, int center_y) const {
	if (angle) {
		::glMatrixMode(GL_PROJECTION);
		::glPushMatrix();
		::glRotatef(angle*180/PIF, 0, 0, 1);
	}
	const int cx = (int)(center_x*cos(angle) + center_y*sin(angle));
	const int cy = (int)(center_y*cos(angle) - center_x*sin(angle));
	const int w = ui_manager_->GetPainter()->GetStringWidth(text);
	const int h = ui_manager_->GetPainter()->GetFontHeight();
	ui_manager_->GetPainter()->PrintText(text, cx-w/2, cy-h/2);
	if (angle) {
		::glPopMatrix();
		::glMatrixMode(GL_MODELVIEW);
	}
}


void App::Layout() {
	if (dialog_) {
		dialog_->Center();
	}
}



void App::Suspend() {
	if (music_player_) {
		music_player_->Pause();
	}
	DoPause();
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
}

void App::Resume() {
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
	HiResTimer::StepCounterShadow();
	loop_timer_.PopTimeDiff();
	if (music_player_) {
		music_player_->Stop();
		music_player_->Playback();
	}
}


bool App::Steer(uilepra::InputManager::KeyCode key_code, float factor) {
	if (!game_ || game_->GetFlybyMode() != Game::kFlybyInactive || dialog_) {
		return false;
	}
	UiCure::CppContextObject* vehicle = game_->GetVehicle();
	if (!vehicle || !vehicle->IsLoaded()) {
		return false;
	}
	enum Directive {
		kDirectiveNone,
		kDirectiveUp,
		kDirectiveDown,
		kDirectiveLeft,
		kDirectiveRight,
		kDirectiveFunction,
	};
	Directive directive = kDirectiveNone;
	switch (key_code) {
		case UIKEY(UP):
		case UIKEY(NUMPAD_8):	directive = kDirectiveUp;		break;
		case UIKEY(W):		directive = kDirectiveUp;		break;
		case UIKEY(DOWN):
		case UIKEY(NUMPAD_2):
		case UIKEY(NUMPAD_5):	directive = kDirectiveDown;		break;
		case UIKEY(S):		directive = kDirectiveDown;		break;
		case UIKEY(LEFT):
		case UIKEY(NUMPAD_4):	directive = kDirectiveLeft;		break;
		case UIKEY(A):		directive = kDirectiveLeft;		break;
		case UIKEY(RIGHT):
		case UIKEY(NUMPAD_6):	directive = kDirectiveRight;		break;
		case UIKEY(D):		directive = kDirectiveRight;		break;
		case UIKEY(LCTRL):
		case UIKEY(RCTRL):
		case UIKEY(INSERT):
		case UIKEY(NUMPAD_0):	directive = kDirectiveFunction;	break;
		case UIKEY(E):
		case UIKEY(F):		directive = kDirectiveFunction;	break;

#ifdef LEPRA_DEBUG
		case UIKEY(0): {
			if (!factor) {
				do_layout_ = true;
			}
		} break;
		case UIKEY(PLUS): {
			if (!factor) {
				const cure::ObjectPositionalData* position = 0;
				vehicle->UpdateFullPosition(position);
				if (position) {
					cure::ObjectPositionalData* new_placement = (cure::ObjectPositionalData*)position->Clone();
					new_placement->position_.transformation_.GetPosition().x -= 10;
					vehicle->SetFullPosition(*new_placement, 0);
				}
			}
		} break;
		case UIKEY(9): {
			if (!factor) {
				const cure::ObjectPositionalData* position = 0;
				vehicle->UpdateFullPosition(position);
				if (position) {
					cure::ObjectPositionalData* new_placement = (cure::ObjectPositionalData*)position->Clone();
					new_placement->position_.transformation_.GetOrientation().RotateAroundOwnY(PIF*0.4f);
					vehicle->SetFullPosition(*new_placement, 0);
				}
			}
		} break;
		case UIKEY(8): {
			if (!factor) {
				game_->FlipRenderSides();
			}
		} break;
		case UIKEY(7): {
			game_->GetVehicle()->DrainHealth(1);
		} break;
		case UIKEY(6): {
			if (!factor) {
				const cure::ObjectPositionalData* position = 0;
				vehicle->UpdateFullPosition(position);
				if (position) {
					cure::ObjectPositionalData* new_placement = (cure::ObjectPositionalData*)position->Clone();
					new_placement->position_.transformation_.GetPosition().x += 30;
					new_placement->position_.transformation_.GetPosition().y += 20;
					new_placement->position_.transformation_.GetPosition().z += 15;
					vehicle->SetFullPosition(*new_placement, 0);
				}
			}
		} break;
		case UIKEY(4): {
			if (!factor) {
				if (ui_manager_->GetCanvas()->GetDeviceOutputRotation() == 0) {
					ui_manager_->GetCanvas()->SetOutputRotation(180);
				} else {
					ui_manager_->GetCanvas()->SetOutputRotation(0);
				}
			}
		} break;
		case UIKEY(3): {
			if (!factor) {
				delete dialog_;
				dialog_ = 0;
				game_->AddScore(Random::Uniform(500.0f, 5000.0f));
				EnterHiscore(str(), WHITE);
			}
		} break;
#endif // Debug
	}

	switch (directive) {
		case kDirectiveNone: {
		} break;
		case kDirectiveUp: {
			vehicle->SetEnginePower(0, +1*factor);
		} break;
		case kDirectiveDown: {
			reverse_and_brake_ = factor;
			if (!reverse_and_brake_) {
				vehicle->SetEnginePower(0, 0);
				vehicle->SetEnginePower(2, 0);
			}
		} break;
		case kDirectiveLeft: {
			vehicle->SetEnginePower(1, -1*factor);
		} break;
		case kDirectiveRight: {
			vehicle->SetEnginePower(1, +1*factor);
		} break;
		case kDirectiveFunction: {
			//game_->Shoot();
		} break;
	}
	return false;
}

bool App::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	return Steer(key_code, 1);
}

bool App::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	return Steer(key_code, 0);
}

#if !defined(LEPRA_TOUCH) && defined(LEPRA_TOUCH_LOOKANDFEEL)
void App::OnMouseInput(uilepra::InputElement* element) {
	if (element->GetType() == uilepra::InputElement::kDigital) {
		is_mouse_down_ = element->GetBooleanValue();
	}
	uilepra::InputManager* input = ui_manager_->GetInputManager();
	if (is_mouse_down_) {
		const int y = (int)((1+input->GetCursorX())*ui_manager_->GetCanvas()->GetWidth()/2);
		const int x = (int)((1-input->GetCursorY())*ui_manager_->GetCanvas()->GetHeight()/2);
		if (g_finger_move_list.empty()) {
			g_finger_move_list.push_back(FingerMovement(x, y));
		}
		FingerMovement& _movement = g_finger_move_list.back();
		_movement.Update(_movement.last_x_, _movement.last_y_, x, y);
	} else {
		FingerMovement& _movement = g_finger_move_list.back();
		_movement.is_press_ = false;
	}
}
#endif // Computer emulating touch device.

int App::PollTap(FingerMovement& movement) {
#ifdef LEPRA_TOUCH
	ui_manager_->GetInputManager()->SetMousePosition(movement.last_y_, movement.last_x_);
	if (movement.is_press_) {
		ui_manager_->GetInputManager()->GetMouse()->GetButton(0)->SetValue(1);
	} else {
		// If releasing, we click-release to make sure we don't miss anything.
		ui_manager_->GetInputManager()->GetMouse()->GetButton(0)->SetValue(1);
		ui_manager_->GetInputManager()->GetMouse()->GetButton(0)->SetValue(0);
	}
	ui_manager_->GetInputManager()->GetMouse()->GetAxis(0)->SetValue(movement.last_x_);
	ui_manager_->GetInputManager()->GetMouse()->GetAxis(1)->SetValue(movement.last_y_);
#endif // touch

	if (!game_ || game_->GetFlybyMode() != Game::kFlybyInactive) {
		return 0;
	}

	return movement.is_press_? 1 : 0;
}



void App::MainMenu() {
	// TRICKY: leave these here, since this call comes from >1 place.
	game_over_timer_.Stop();
	game_->EnableScoreCounting(false);
	game_->SetFlybyMode(Game::kFlybyPause);
	// TRICKY-END!

	uitbc::Dialog* d = CreateTbcDialog(&App::OnMainMenuAction);
	d->AddButton(1, ICONBTNA("btn_1p.png", "Single player"), true);
	d->AddButton(2, ICONBTNA("btn_2p.png", "Two players"), true);
	d->AddButton(3, ICONBTNA("btn_hiscore.png", "Hiscore"), true);
	d->AddButton(4, ICONBTNA("btn_credits.png", "Credits"), true);
}

void App::UpdateHiscore(bool error) {
	if (!dialog_) {
		return;
	}
	if (error) {
		uitbc::Label* _text = new uitbc::Label(LIGHT_RED, "Network problem, try again l8r.");
		_text->SetVericalAlignment(uitbc::Label::kValignTop);
		dialog_->AddChild(_text, 135, 75);
		return;
	}
	str last_hiscore_name;
	v_get(last_hiscore_name, =, variable_scope_, kRtvarHiscoreName, "");
	typedef cure::HiscoreAgent::Entry HiscoreEntry;
	typedef cure::HiscoreAgent::List HiscoreList;
	const HiscoreList& hiscore_list = hiscore_agent_->GetDownloadedList();
	str hiscore;
	const int base_place = hiscore_list.offset_;
	const int entry_count = 10;
	const double log_exponent = ::log10((double)(base_place+entry_count)) + 1e-12;
	const int position_digits = (int)::floor(log_exponent) + 1;
	for (int x = 0; x < (int)hiscore_list.entry_list_.size() && x < entry_count; ++x) {
		const int place = x + 1 + base_place;
		const HiscoreEntry& entry = hiscore_list.entry_list_[x];
		const str score = Int2Str(entry.score_);
		char pointer = ' ';
		char pointer2 = ' ';
		if (last_hiscore_name == entry.name_) {
			pointer  = '>';
			pointer2 = '<';
		}
		const str format_place = strutil::Format("%i", position_digits);
		// TRICKY: ugly circumvention for string that won't vswprintf()!
		str __name = entry.name_;
		if (__name.size() < 13) {
			__name.append(13-__name.size(), ' ');
		}
		const str format1 = "%c%" + format_place + "i ";
		const str format2 = " %10s%c\n";
		hiscore += strutil::Format(format1.c_str(), pointer, place) +
			__name +
			strutil::Format(format2.c_str(), score.c_str(), pointer2);
	}
	if (hiscore.empty()) {
		hiscore = "No score entered. Yet.";
	}
	uitbc::Label* _text = new uitbc::Label(FGCOLOR_DIALOG, hiscore);
	_text->SetFontId(monospaced_font_id_);
	_text->SetVericalAlignment(uitbc::Label::kValignTop);
	const uitbc::FontManager::FontId previous_font_id = ui_manager_->GetFontManager()->GetActiveFontId();
	ui_manager_->GetFontManager()->SetActiveFont(monospaced_font_id_);
	const int char_width = ui_manager_->GetFontManager()->GetStringWidth(" ");
	ui_manager_->GetFontManager()->SetActiveFont(previous_font_id);
	dialog_->AddChild(_text, 110 - position_digits/2 * char_width, 75);
}

void App::HiscoreMenu(int direction) {
	// Start downloading the highscore.
	CreateHiscoreAgent();
	const str level_name = "any";
	const str vehicle_name = "monster_01";
	const int offset = std::max(0, my_hiscore_index_-5);
	if (!hiscore_agent_->StartDownloadingList(kPlatform, level_name, vehicle_name, offset, 10)) {
		delete hiscore_agent_;
		hiscore_agent_ = 0;
	}
	my_hiscore_index_ = 0;

	uitbc::Dialog* d = CreateTbcDialog(&App::OnHiscoreAction);
	d->SetPreClickTarget(uitbc::Dialog::Action(this, &App::OnPreHiscoreAction));
	d->SetDirection(direction, true);
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel("Hiscore " + level_name + "/" + vehicle_name, big_font_id_);
	uitbc::Button* main_menu_button = ICONBTNA("btn_back.png", "");
	main_menu_button->SetPreferredSize(d->GetPreferredWidth() / 2, d->GetPreferredHeight());
	d->AddButton(-1, main_menu_button, true);
	main_menu_button->SetPos(d->GetPreferredWidth()/4, 0);
	if (!hiscore_agent_) {
		UpdateHiscore(true);
	}
}

void App::EnterHiscore(const str& message, const Color& color) {
	game_over_timer_.Stop();

	uitbc::Dialog* d = CreateTbcDialog(&App::OnEnterHiscoreAction);
	d->SetPreClickTarget(uitbc::Dialog::Action(this, &App::OnPreEnterAction));
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel("Enter hiscore name ("+Int2Str((int)game_->GetScore())+" points", big_font_id_);
	if (!message.empty()) {
		uitbc::Label* _message = new uitbc::Label(color, message);
		const int string_width = ui_manager_->GetPainter()->GetStringWidth(message);
		d->AddChild(_message, d->GetSize().x/2 - string_width/2, 80);
	}
	hiscore_text_field_ = new HiscoreTextField(d, uitbc::TextField::kBorderSunken, 2, WHITE, "hiscore");
	hiscore_text_field_->app_ = this;
	hiscore_text_field_->SetText(v_slowget(variable_scope_, kRtvarHiscoreName, ""));
	hiscore_text_field_->SetPreferredSize(205, 25, false);
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	d->AddChild(hiscore_text_field_, 70, 97);
#else // Computer
	d->AddChild(hiscore_text_field_, 70, 130);
#endif // touch / computer
	hiscore_text_field_->SetKeyboardFocus();	// TRICKY: focus after adding.
	uitbc::Button* cancel_button = new uitbc::Button("cancel");
	Color c = Color(180, 50, 40);
	cancel_button->SetBaseColor(c);
	cancel_button->SetText("Cancel", FGCOLOR_DIALOG, CLEAR_COLOR);
	cancel_button->SetRoundedRadius(8);
	cancel_button->SetPreferredSize(300-hiscore_text_field_->GetPreferredWidth()-8, hiscore_text_field_->GetPreferredHeight()+1);
	d->AddButton(-1, cancel_button, true);
	cancel_button->SetPos(hiscore_text_field_->GetPos().x+hiscore_text_field_->GetPreferredWidth()+8, hiscore_text_field_->GetPos().y);
}

void App::SuperReset() {
	game_over_timer_.Stop();

	game_->EndSlowmo();

	game_->ResetScore();

	game_->SetVehicleName(game_->GetVehicleName());
	resource_manager_->Tick();
	strutil::strvec resource_types;
	resource_types.push_back("RenderImg");
	resource_types.push_back("Geometry");
	resource_types.push_back("GeometryRef");
	resource_types.push_back("Physics");
	resource_types.push_back("PhysicsShared");
	resource_types.push_back("RamImg");
	resource_manager_->ForceFreeCache(resource_types);
	resource_manager_->ForceFreeCache(resource_types);	// Call again to release any dependent resources.

	//is_loaded_ = false;
	do_layout_ = true;
}

void App::CreateHiscoreAgent() {
	delete hiscore_agent_;
	const str host = _O("7y=196h5+;/,9p.5&92r:/;*(,509p;/1", "gamehiscore.pixeldoctrine.com");
	hiscore_agent_ = new cure::HiscoreAgent(host, 80, "tire_fire");
	//hiscore_agent_ = new cure::HiscoreAgent("localhost", 8080, "tire_fire");
}

void App::Purchase(const str& product_name) {
#ifdef LEPRA_IOS
	[animated_app_ startPurchase:MacLog::Encode(product_name)];
#else // !iOS
	(void)product_name;
	button_delegate_ = uitbc::Dialog::Action(this, &App::OnOk);
	uitbc::MessageDialog* _message = new uitbc::MessageDialog(ui_manager_->GetDesktopWindow(), button_delegate_,
		"Content purchase not yet implemented on this platform.");
	_message->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	_message->AddButton(+10, "OK", true);
	_message->AddButton(+33, "Nooo!", true);
	dialog_ = _message;
#endif // iOS
}

void App::SetIsPurchasing(bool is_purchasing) {
	is_purchasing_ = is_purchasing;
	if (!is_purchasing_ && !dialog_) {
		MainMenu();
	}
}

void App::OnResize(int /*_width*/, int /*_height*/) {
	do_layout_ = true;
	boot_logo_timer_.ReduceTimeDiff(-10);
}

void App::OnMinimize() {
}

void App::OnMaximize(int _width, int _height) {
	OnResize(_width, _height);
}

void App::OnAction(uitbc::Button* button) {
	do_layout_ = true;
	uitbc::Dialog* d = dialog_;
	button_delegate_(button);
	if (dialog_ == d) {	// No news? Just drop it.
		button_delegate_.clear();
		dialog_ = 0;
	}
}

void App::OnTapSound(uitbc::Button*) {
	if (tap_click_->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetSoundManager()->Play(tap_click_->GetData(), 1, Random::Uniform(0.7f, 1.4f));
	}
}

void App::OnOk(uitbc::Button* button) {
	if (button->GetTag() == 33) {
		SystemManager::EmailTo(
			"info@pixeldoctrine.com",
			"I want the complete game!",
			"Hiya Game Slave Bitches,\n\n"
			"I enjoyed the TireFire Demo [for PC/Mac?] and would like the complete game!\n\n"
			"Get a move on,\n"
			"Yours Truly");
	}
	MainMenu();
}

void App::OnMainMenuAction(uitbc::Button* button) {
	switch (button->GetTag()) {
		case 1: {
			// 1P
		} break;
		case 2: {
			// 2P
		} break;
		case 3: {
			HiscoreMenu(+1);
			//EnterHiscore("Press enter when you're done", FGCOLOR_DIALOG);
		}
		return;
		case 4: {
			uitbc::Dialog* d = CreateTbcDialog(&App::OnCreditsAction);
			d->SetOffset(PixelCoord(0, -30));
			d->SetQueryLabel("Credits", big_font_id_);
			str s =	"Game    Pixel Doctrine\n"
				"Music   Jonas Kapla\n"
				"Thanks  ODE, STLport, ChibiXM, Ogg/Vorbis, OpenAL, ALUT\n"
				"        libpng, Minizip, zlib, FastDelegate, UTF-8 CPP,\n"
				"        DMI, freesound, HappyHTTP, GAE, Python, py-cgkit\n"
				"\n"
				"Idiots kill civilians for real. Visit Avaaz.org if you\n"
				"too belive media attention eventually can crush tyrants.";
			uitbc::Label* _text = new uitbc::Label(FGCOLOR_DIALOG, s);
			_text->SetFontId(monospaced_font_id_);
			_text->SetVericalAlignment(uitbc::Label::kValignTop);
			d->AddChild(_text, 25, 85);
			uitbc::Button* back_button = new uitbc::CustomButton("back");
			back_button->SetPreferredSize(d->GetPreferredSize());
			d->AddButton(-1, back_button, true);
			back_button->SetPos(0, 0);
		}
		return;
	}
	/*uitbc::Dialog* d = CreateTbcDialog(&App::OnLevelAction);
	d->SetQueryLabel("Select level", big_font_id_);
	d->AddButton(1, ICONBTN("btn_tutorial.png", "Tutorial"));
	d->AddButton(2, ICONBTN("btn_lvl2.png", kLevels[0]));
	d->AddButton(3, ICONBTN("btn_lvl3.png", kLevels[1]));
	d->AddButton(4, ICONBTN("btn_lvl4.png", kLevels[2]));
	if (is_money_icon_added_ && !v_slowget(variable_scope_, kRtvarContentLevels, false)) {
		AddCostIcon(kLevels[1]);
		AddCostIcon(kLevels[2]);
	}*/
}

void App::OnEnterHiscoreAction(uitbc::Button* button) {
	if (!button) {
		str last_hiscore_name = strutil::Strip(hiscore_text_field_->GetText(), " \t\v\r\n");
		hiscore_text_field_ = 0;
		if (!last_hiscore_name.empty()) {
			v_set(variable_scope_, kRtvarHiscoreName, last_hiscore_name);
#ifdef LEPRA_IOS
			[AnimatedApp hiscore_name_];
#endif // iOS
			const str level_name = "any";
			const str vehicle_name = "any";
			CreateHiscoreAgent();
			if (!hiscore_agent_->StartUploadingScore(kPlatform, level_name, vehicle_name, last_hiscore_name, (int)Math::Round(game_->GetScore()))) {
				delete hiscore_agent_;
				hiscore_agent_ = 0;
			}
		} else {
			MainMenu();
		}
	} else if (button->GetTag() == -1) {
		MainMenu();
	}
}

void App::OnLevelAction(uitbc::Button* button) {
	if (button->GetTag() >= 3 && !v_slowget(variable_scope_, kRtvarContentLevels, false)) {
		Purchase(kContentLevels);
		return;
	}

	str level = "level_2";
	switch (button->GetTag()) {
		case 1:	level = "level_2";			hiscore_level_index_ = 0;	break;
		case 2:	level = "level_2";			hiscore_level_index_ = 0;	break;
		case 3:	level = "level_elevate";		hiscore_level_index_ = 1;	break;
		case 4:	level = "level_balls_castle";	hiscore_level_index_ = 2;	break;
	}
	if (game_->GetLevelName() != level) {
		game_->SetLevelName(level);
	}
	if (button->GetTag() == 1) {
		// Tutorial.
		game_over_timer_.Stop();
		game_->EnableScoreCounting(true);
		game_->SetFlybyMode(Game::kFlybyIntroduction);
		game_->SetVehicleName("monster_01");
		// If all else fails, fall thru add keep going. Makes more sense to the user than
		// a tutorial without any instructions.
	}
	if (is_money_icon_added_ && !v_slowget(variable_scope_, kRtvarContentVehicles, false)) {
		/*AddCostIcon(kVehicles[1]);
		AddCostIcon(kVehicles[2]);
		AddCostIcon(kVehicles[3]);*/
	}
	/*if (game_->GetComputerIndex() != -1) {
		d->SetOffset(PixelCoord(0, -40));
		d->UpdateLayout();
	}*/

	strutil::strvec resource_types;
	resource_types.push_back("RenderImg");
	resource_types.push_back("Geometry");
	resource_types.push_back("GeometryRef");
	resource_types.push_back("Physics");
	resource_types.push_back("PhysicsShared");
	resource_types.push_back("RamImg");
	resource_manager_->ForceFreeCache(resource_types);
	resource_manager_->ForceFreeCache(resource_types);	// Call again to release any dependent resources.
}

void App::OnHiscoreAction(uitbc::Button* /*button*/) {
	delete hiscore_agent_;
	hiscore_agent_ = 0;
	MainMenu();
}

void App::OnPreHiscoreAction(uitbc::Button* button) {
	OnTapSound(button);
	switch (button->GetTag()) {
		case -1: {
			dialog_->SetDirection(+1, false);
		} break;
		case -2:
		case -4: {
			dialog_->SetDirection(-1, false);
		} break;
		case -3:
		case -5: {
			dialog_->SetDirection(+1, false);
		} break;
	}
}

void App::OnPreEnterAction(uitbc::Button* button) {
	OnTapSound(button);
}

void App::OnCreditsAction(uitbc::Button* /*button*/) {
	MainMenu();
}

void App::DoPause() {
	if (dialog_) {
		return;
	}
	uitbc::Dialog* d = CreateTbcDialog(&App::OnPauseAction);
	d->AddButton(1, ICONBTNA("btn_resume.png", "Resume"), true);
	if (game_->GetFlybyMode() == Game::kFlybyInactive) {	// Restart not available in tutorial mode.
		d->AddButton(2, ICONBTNA("btn_restart.png", "Restart"), true);
	}
	d->AddButton(3, ICONBTNA("btn_main_menu.png", "Main menu"), true);
}

void App::OnPauseClickWithSound(uitbc::Button* button) {
	if (dialog_) {
		return;
	}
	OnTapSound(button);
	DoPause();
}

void App::OnPauseAction(uitbc::Button* button) {
	if (button->GetTag() == 2) {
		SuperReset();
	} else if (button->GetTag() == 3) {
		MainMenu();
	}
}

void App::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource) {
	(void)resource;
}

void App::SoundLoadCallback(UiCure::UserSound2dResource* resource) {
	(void)resource;
}

uitbc::Dialog* App::CreateTbcDialog(ButtonAction action) {
	button_delegate_ = uitbc::Dialog::Action(this, action);
	uitbc::Dialog* d = new uitbc::Dialog(ui_manager_->GetDesktopWindow(), uitbc::Dialog::Action(this, &App::OnAction));
	d->SetPreClickTarget(uitbc::Dialog::Action(this, &App::OnTapSound));
	d->SetSize(440, 280);
	d->SetPreferredSize(440, 280);
	d->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	dialog_ = d;
	return d;
}

uitbc::Button* App::CreateButton(const str& text, const Color& color, uitbc::Component* parent) {
	uitbc::Button* _button = new uitbc::Button(uitbc::BorderComponent::kLinear, 6, color, "");
	_button->SetText(text);
	_button->SetPreferredSize(44, 44);
	parent->AddChild(_button);
	_button->SetVisible(false);
	_button->UpdateLayout();
	return _button;
}

void App::Transpose(int& x, int& y, float& angle) const {
	float fx = (float)x;
	float fy = (float)y;
	Transpose(fx, fy, angle);
	x = (int)fx;
	y = (int)fy;
}

void App::Transpose(float& x, float& y, float& angle) const {
	if (flip_draw_) {
		const int w = ui_manager_->GetCanvas()->GetWidth();
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		x = w - x;
		y = ui_manager_->GetCanvas()->GetHeight() - y;
		angle += PIF;
#else // Computer
		(void)y;
		(void)angle;
		const int w2 = w/2;
		if (x < w2) {
			x += w2;
		} else {
			x -= w2;
		}
#endif // touch / Computer
	}
}

str App::Int2Str(int number) {
	str s = strutil::IntToString(number, 10);
	size_t l = s.length();
	if (number < 0) {
		--l;
	}
	for (size_t y = 3; y < l; y += 4) {
		s.insert(s.length()-y, 1, ',');
		++l;
	}
	return s;
}



App* App::app_ = 0;
loginstance(kGame, App);



}
