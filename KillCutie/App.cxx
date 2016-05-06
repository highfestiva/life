
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
#include "../uicure/include/uiparticleloader.h"
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
#include "../uitbc/include/uiparticlerenderer.h"
#include "cutie.h"
#include "game.h"
#include "launcher.h"
#ifdef LEPRA_MAC
#include "../lepra/include/posix/maclog.h"
#endif // iOS



#define UIKEY(name)			uilepra::InputManager::kInKbd##name
#define kButtonWidth			40
#define kButtonMargin			2
#define kMeterHeight			52
#define BGCOLOR_DIALOG			Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG			Color(170, 170, 170, 255)
#define ICONBTN(i,n)			new UiCure::IconButton(ui_manager_, resource_manager_, i, n)
#define ICONBTNA(i,n)			ICONBTN(i, n)
#define kHeartPoints			4
#define kBarrelCompassLineCount	16
#define kBarrelCompassLineSpacing	3
#define BARREL_COMPASS_HEIGHT		((kBarrelCompassLineCount-1)*kBarrelCompassLineSpacing + 1)
#define kTouchOffset			92
#define GET_NAME_INDEX(idx, a)		((idx) < 0)? LEPRA_ARRAY_COUNT(a)-1 : (idx)%LEPRA_ARRAY_COUNT(a)
#define GET_NAME(idx, a)		a[GET_NAME_INDEX(idx, a)]
#define kContentLevels			"levels"
#define kContentVehicles		"vehicles"
#define kRtvarContentLevels		"Content.Levels"
#define kRtvarContentVehicles		"Content.Vehicles"
#define kRtvarHiscoreName		"Hiscore.Name"	// Last entered name.
#define kKcFullVersion			1
//#define KC_DEV_TESTING			1	// TODO!!!!!!!!!!!!!!!!!!!!!



namespace grenaderun {



FingerMoveList g_finger_move_list;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
const str kPlatform = "touch";
#else // Computer.
const str kPlatform = "computer";
#endif // touch / computer.
const str kLevels[] = { "Pendulum", "Elevate", "RoboCastle" };
const str kVehicles[] = { "Cutie", "Hardie", "Speedie", "Sleepie" };



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
	void DrawMatchStatus();
	void DrawVehicleSteering();
	void DrawHealthBar();
	void DrawLauncherIndicators(int computer_index);
	void DrawHearts();
	void DrawKeys();
	void DrawInfoTexts() const;
	void ClearInfoTexts() const;
	bool PreDrawHud();
	void DrawLine(float x1, float y1, float x2, float y2);
	void DrawImage(uitbc::Painter::ImageID image_id, float x, float y, float w, float h, float angle) const;
	void DrawRoundedPolygon(int x, int y, int radius, const Color& color, float scale_x, float scale_y) const;
	void DrawRoundedPolygon(float x, float y, float radius, const Color& color, float scale_x, float scale_y, int corner_radius) const;
	void DrawMeter(int x, int y, float angle, float _size, float min_value, float max_value, int _width = 40, int spacing = 2, int bar_count = 13) const;
	void DrawTapIndicator(int tag, int x, int y, float angle) const;
	void DrawBarrelCompass(int x, int  y, float angle, int _size, float value1, float value2) const;
	void InfoText(int player, const wstr& info, float angle, float dx = 0, float dy = 0) const;
	void PrintText(const wstr& text, float angle, int center_x, int center_y) const;
	float GetSteeringWheelGuiInfo(float w, float h, float& x, float& y) const;
	void Layout();
	void MainMenu();
	void UpdateHiscore(bool error);
	void HiscoreMenu(int direction);
	void EnterHiscore(const wstr& message, const Color& color);
	void SuperReset(bool game_over);
	void CreateHiscoreAgent();
	void Purchase(const str& product_name);
	void SetIsPurchasing(bool is_purchasing);

	virtual void Suspend(bool hard);
	virtual void Resume(bool hard);

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
	void KillIntroVoice();

	typedef void (App::*ButtonAction)(uitbc::Button*);
	uitbc::Dialog* CreateTbcDialog(ButtonAction action);
	void AddCostIcon(const str& _name);
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

	uilepra::SoundStream* intro_streamer_;

	double average_loop_time_;
	double average_fast_loop_time_;
	HiResTimer loop_timer_;

	struct InfoTextData {
		wstr text_;
		vec2 coord_;
		float angle_;
	};

	bool is_loaded_;
	bool do_layout_;
	bool is_paused_;
	bool playback_voice_instruction_;
	cure::ResourceManager* resource_manager_;
	cure::RuntimeVariableScope* variable_scope_;
	UiCure::GameUiManager* ui_manager_;
	UiCure::MusicPlayer* music_player_;
	int layout_frame_counter_;
	uitbc::Button* lazy_button_;
	uitbc::Button* hard_button_;
	uitbc::Button* original_button_;
	uitbc::Button* m1PButton_;
	uitbc::Button* m2PButton_;
	uitbc::Button* next_button_;
	uitbc::Button* reset_button_;
	uitbc::Button* retry_button_;
	uitbc::Button* pause_button_;
	uitbc::Button* geti_phone_button_;
	UiCure::UserPainterKeepImageResource* heart_;
	UiCure::UserPainterKeepImageResource* grey_heart_;
#ifndef LEPRA_TOUCH_LOOKANDFEEL
	UiCure::UserPainterKeepImageResource* keyboard_button_;
#endif // Computer
	UiCure::UserPainterKeepImageResource* arrow_;
	UiCure::UserPainterKeepImageResource* steering_wheel_;
	UiCure::UserPainterKeepImageResource* grenade_;
	UiCure::UserPainterKeepImageResource* money_;
	UiCure::UserPainterKeepImageResource* score_hud_;
	UiCure::UserPainterKeepImageResource* back_;
	float grenade_size_factor_;
	mutable vec3 heart_pos_[kHeartPoints];
	uitbc::RectComponent* player_splitter_;
	float angle_time_;
	Color info_text_color_;
	mutable float pen_x_;
	mutable float pen_y_;
	typedef std::vector<InfoTextData> InfoTextArray;
	mutable InfoTextArray info_text_array_;
	mutable HiResTimer player1_last_touch_;
	mutable HiResTimer player2_last_touch_;
	mutable HiResTimer player1_touch_delay_;
	mutable HiResTimer player2_touch_delay_;
	uitbc::FontManager::FontId big_font_id_;
	uitbc::FontManager::FontId monospaced_font_id_;
	float reverse_and_brake_;
	uitbc::Dialog::Action button_delegate_;
	uitbc::Dialog* dialog_;
	mutable StopWatch start_timer_;
	mutable StopWatch game_over_timer_;
	UiCure::PainterImageResource* scroll_bar_image_;
	uitbc::Label* difficulty_label_;
	uitbc::ScrollBar* difficulty_slider_;
	int slow_shadow_count_;
	float base_throttle_;
	bool is_throttling_;
	int throttle_meter_offset_;
	float lift_meter_offset_;
	float yaw_meter_offset_;
	float steering_;
	float base_steering_;
	bool flip_draw_;
	HiResTimer boot_logo_timer_;
	int hiscore_level_index_;
	int hiscore_vehicle_index_;
	HiscoreTextField* hiscore_text_field_;
	float throttle_;
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
	HiscoreTextField(Component* top_parent, unsigned border_style, int border_width, const Color& color):
		Parent(top_parent, border_style, border_width, color) {
	}
	virtual bool OnChar(wchar_t _c) {
		bool b = false;
		if (GetText().length() < 13 || _c < ' ') {	// Limit character length.
			b = Parent::OnChar(_c);
			if (GetText().length() > 13) {	// Shouln't happen...
				SetText(GetText().substr(0, 13));
			}
		}
		if (_c == '\r' || _c == '\n') {
			const wstr _text = wstrutil::Strip(GetText(), L" \t\v\r\n");
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



LEPRA_RUN_APPLICATION(grenaderun::App, uilepra::UiMain);



namespace grenaderun {



App::App(const strutil::strvec& argument_list):
	Application(argument_list),
	game_(0),
	intro_streamer_(0),
	layout_frame_counter_(-10),
	variable_scope_(0),
	average_loop_time_(1.0/(kFps+1)),
	average_fast_loop_time_(1.0/(kFps+1)),
	is_loaded_(false),
	do_layout_(true),
	is_paused_(false),
	playback_voice_instruction_(false),
	pause_button_(0),
	geti_phone_button_(0),
	angle_time_(0),
	big_font_id_(uitbc::FontManager::kInvalidFontid),
	monospaced_font_id_(uitbc::FontManager::kInvalidFontid),
	reverse_and_brake_(0),
	dialog_(0),
	scroll_bar_image_(0),
	difficulty_label_(0),
	difficulty_slider_(0),
	slow_shadow_count_(0),
	base_throttle_(0),
	is_throttling_(false),
	throttle_meter_offset_(0),
	lift_meter_offset_(0),
	yaw_meter_offset_(0),
	steering_(0),
	base_steering_(0),
	flip_draw_(false),
	hiscore_level_index_(0),
	hiscore_vehicle_index_(0),
	hiscore_text_field_(0),
	throttle_(0),
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

#if defined(kKcFullVersion) || defined(KC_DEV_TESTING)
	v_set(variable_scope_, kRtvarContentLevels, true);
	v_set(variable_scope_, kRtvarContentVehicles, true);
#endif // Kill Cutie full version compile

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
		ui_manager_->GetDisplayManager()->SetCaption("Kill Cutie");
		ui_manager_->GetDisplayManager()->AddResizeObserver(this);
		ui_manager_->GetInputManager()->AddKeyCodeInputObserver(this);
		ui_manager_->GetInputManager()->SetCursorVisible(true);
#if !defined(LEPRA_TOUCH) && defined(LEPRA_TOUCH_LOOKANDFEEL)
		is_mouse_down_ = false;
		ui_manager_->GetInputManager()->GetMouse()->AddFunctor(new uilepra::TInputFunctor<App>(this, &App::OnMouseInput));
#endif // Computer emulating touch
	}
	if (ok) {
		uitbc::DesktopWindow* desktop_window = ui_manager_->GetDesktopWindow();

		player_splitter_ = new uitbc::RectComponent(BLACK);
		desktop_window->AddChild(player_splitter_);

		pause_button_ = ICONBTNA("btn_pause.png", L"");
		pause_button_->SetBaseColor(BGCOLOR_DIALOG);
		pause_button_->SetRoundedRadius(10);
		desktop_window->AddChild(pause_button_);
		pause_button_->SetPreferredSize(PixelCoord(44, 44), false);
		pause_button_->SetSize(pause_button_->GetPreferredSize());
		pause_button_->SetVisible(true);
		pause_button_->SetOnClick(App, OnPauseClickWithSound);

#if !defined(LEPRA_TOUCH_LOOKANDFEEL) && !defined(kKcFullVersion)
		geti_phone_button_ = ICONBTNA("btn_iphone.png", L"");
		desktop_window->AddChild(geti_phone_button_);
		geti_phone_button_->SetVisible(true);
		geti_phone_button_->SetOnClick(App, OnGetiPhoneClick);
#endif // touch L&F AND not full version
	}
	if (ok) {
		heart_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		heart_->Load(resource_manager_, "heart.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		grey_heart_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		grey_heart_->Load(resource_manager_, "grey_heart.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
#ifndef LEPRA_TOUCH_LOOKANDFEEL
		keyboard_button_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		keyboard_button_->Load(resource_manager_, "btn_key.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
#endif // Computer
		arrow_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		arrow_->Load(resource_manager_, "arrow.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		steering_wheel_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		steering_wheel_->Load(resource_manager_, "steering_wheel.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		grenade_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		grenade_->Load(resource_manager_, "grenade.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		money_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		money_->Load(resource_manager_, "money.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		score_hud_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		score_hud_->Load(resource_manager_, "score_hud.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		grenade_size_factor_ = 1.0f;
	}
	if (ok) {
		uitbc::FontManager::FontId default_font_id = ui_manager_->GetFontManager()->GetActiveFontId();
		big_font_id_ = ui_manager_->GetFontManager()->QueryAddFont("Helvetica", 24);
		monospaced_font_id_ = ui_manager_->GetFontManager()->QueryAddFont("Courier New", 14);
		ui_manager_->GetFontManager()->SetActiveFont(default_font_id);
	}
	if (ok) {
		music_player_ = new UiCure::MusicPlayer(ui_manager_->GetSoundManager());
		music_player_->SetVolume(0.5f);
		music_player_->SetSongPauseTime(9, 15);
		music_player_->AddSong("ButterflyRide.xm");
		music_player_->AddSong("BehindTheFace.xm");
		music_player_->AddSong("BrittiskBensin.xm");
		music_player_->AddSong("DontYouWantMe'97.xm");
		music_player_->AddSong("CloseEncounters.xm");
		music_player_->Shuffle();
		ok = music_player_->Playback();
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
		uitbc::Renderer* renderer = ui_manager_->GetRenderer();
		renderer->AddDynamicRenderer("particle", new uitbc::ParticleRenderer(renderer, 1));
		UiCure::ParticleLoader loader(resource_manager_, renderer, "explosion.png", 4, 5);
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
	delete scroll_bar_image_;
	scroll_bar_image_ = 0;

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

	const str log_name = Path::JoinPath(SystemManager::GetIoDirectory("KillCutie"), "log", "txt");
	FileLogListener file_logger(log_name);
	{
		LogType::GetLogger(LogType::kRoot)->SetupBasicListeners(&console_logger_, &debug_logger_, &file_logger, 0, 0);
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
		v_set(variable_scope_, kRtvarPhysicsMicrosteps, 2);
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
//#ifndef KC_DEV_TESTING
		game_->SetComputerDifficulty(0.0f);
		ok = game_->SetLevelName("level_2");
/*#else // Dev testing
		game_->SetComputerDifficulty(1.0f);
		game_->SetComputerIndex(1);
		ok = game_->SetLevelName("level_balls_castle");
#endif // Dev testing / !dev testing*/
	}
	if (ok) {
		game_->cure::GameTicker::GetTimeManager()->Tick();
		game_->cure::GameTicker::GetTimeManager()->Clear(1);

//#ifndef KC_DEV_TESTING
		MainMenu();
/*#else // Dev testing
		uitbc::Button* _button = new uitbc::Button("Apa");
		_button->SetTag(4);
		OnLevelAction(_button);
#endif // Dev testing / !dev testing*/

		ok = resource_manager_->InitDefault();
	}
	if (ok) {
		boot_logo_timer_.EnableShadowCounter(true);
		loop_timer_.EnableShadowCounter(true);
		player1_last_touch_.EnableShadowCounter(true);
		player2_last_touch_.EnableShadowCounter(true);
		player1_touch_delay_.EnableShadowCounter(true);
		player2_touch_delay_.EnableShadowCounter(true);
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
#ifndef LEPRA_TOUCH_LOOKANDFEEL
		if (average_loop_time_ > 1.0/kFps) {
			if (++slow_shadow_count_ > 20) {
				// Shadows is not such a good idea on this system.
				v_set(variable_scope_, kRtvarUi3DShadows, "None");
				slow_shadow_count_ = 0;
			}
		}
#endif // !touch L&F
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
							EnterHiscore(L"Please retry; score server obstipated", LIGHT_RED);
						}
					} break;
					default: {
						delete hiscore_agent_;
						hiscore_agent_ = 0;
						log_.Error("Oops! Completed hiscore communication, but something went wrong.");
						deb_assert(false);
						MainMenu();	// Well... assume some super-shitty state...
					} break;
				}
			}
		}
	}
	if (ok) {
		// Take care of the "brake and reverse" steering.
		if (game_->GetCutie() && game_->GetCutie()->IsLoaded()) {
			const bool is_moving_forward = (game_->GetCutie()->GetForwardSpeed() > 3.0f*SCALE_FACTOR);
			if (reverse_and_brake_) {
				game_->GetCutie()->SetEnginePower(0, is_moving_forward? 0 : -1*reverse_and_brake_);	// Reverse.
				game_->GetCutie()->SetEnginePower(2, is_moving_forward? +1*reverse_and_brake_ : 0);	// Brake.
			}
		}
	}
	if (ok) {
		angle_time_ += 1.0f/kFps/10;
		angle_time_ -= (angle_time_ > 2*PIF)? 2*PIF : 0;
		ok = (SystemManager::GetQuitRequest() == 0);
	}
	if (!is_loaded_ && resource_manager_->IsLoading()) {
		resource_manager_->Tick();
		return ok;
	}
	is_loaded_ = true;
	if (ok) {
		const bool lShowPause = !is_paused_ && !dialog_ &&
			(game_->GetFlybyMode() == Game::kFlybyInactive || game_->GetFlybyMode() == Game::kFlybyIntroduction || game_->GetFlybyMode() == Game::kFlybyIntroductionFinishingUp);
		pause_button_->SetVisible(lShowPause);
	}
	if (ok) {
		if (!is_paused_ && game_->GetFlybyMode() != Game::kFlybyInactive) {
			game_->TickFlyby();
		}
	}
	if (ok && do_layout_) {
		Layout();
	}
	if (ok) {
		if (difficulty_label_ && difficulty_slider_) {
			wstr s = L"Opponent difficulty ";
			const float difficulty = (float)difficulty_slider_->GetScrollPos();
			if (difficulty < 0.1f) {
				s += L"(wanker)";
			} else if (difficulty < 0.3f) {
				s += L"(silly)";
			} else if (difficulty < 0.7f) {
				s += L"(alright)";
			} else if (difficulty < 0.9f) {
				s += L"(tough)";
			} else {
				s += L"(bigot)";
			}
			difficulty_label_->SetText(s);
			game_->SetComputerDifficulty(difficulty);
		}
	}
	if (ok) {
		// Set orientation restriction.
		if (game_->GetComputerIndex() == -1) {
			v_set(variable_scope_, kRtvarUiDisplayOrientation, "Fixed");
		} else {
			v_set(variable_scope_, kRtvarUiDisplayOrientation, "AllowUpsideDown");
		}
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
	const bool lTRICKY_IsLoopPaused = is_paused_;
	if (ok && !lTRICKY_IsLoopPaused) {
		if (game_->GetFlybyMode() == Game::kFlybyInactive) {
			game_->AddScore(+3, 0);
		}
		game_->BeginTick();
		game_->StartPhysicsTick();
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
		info_text_color_ = Color(255, (uint8)(127*(1+::sin(angle_time_*27)*0.9f)), 255);
		if (PreDrawHud()) {
			if (game_->GetFlybyMode() == Game::kFlybyIntroductionFinishingUp) {
				if (game_->GetComputerIndex() == -1) {
					DrawLauncherIndicators(0);
					DrawLauncherIndicators(1);
				} else {
					DrawLauncherIndicators(0);	// Computer is vehicle == we play launcher!
				}
				DrawHearts();
				player1_last_touch_.ClearTimeDiff();
				player2_last_touch_.ClearTimeDiff();
				player1_touch_delay_.ClearTimeDiff();
				player2_touch_delay_.ClearTimeDiff();
			} else {
				DrawMatchStatus();
				DrawVehicleSteering();
				DrawHealthBar();
				DrawLauncherIndicators(game_->GetComputerIndex());
				DrawHearts();
				DrawKeys();
				DrawInfoTexts();
			}
		}
		ClearInfoTexts();

		if (game_->GetFlybyMode() == Game::kFlybyInactive &&
			(game_->IsScoreCountingEnabled() || game_->GetWinnerIndex() >= 0) &&
			game_->GetComputerIndex() != -1) {
			if (score_hud_->GetLoadState() == cure::kResourceLoadComplete) {
				const float x = ui_manager_->GetCanvas()->GetWidth() / 2.0f;
				float y = score_hud_->GetRamData()->GetHeight()/2.0f - 5;
				float a = PIF;
				if (game_->GetComputerIndex() == 1) {
					y = (float)ui_manager_->GetCanvas()->GetHeight() - y;
					a = 0;
				}
				DrawImage(score_hud_->GetData(), x, y, (float)score_hud_->GetRamData()->GetWidth(),
					(float)score_hud_->GetRamData()->GetHeight(), a);
			}
			ui_manager_->GetPainter()->SetColor(WHITE);
			const wstr score = wstrutil::Encode("Score: " + Int2Str((int)game_->GetScore()));
			int sy = 8 + ui_manager_->GetPainter()->GetFontHeight()/2;
			if (game_->GetComputerIndex() == 1) {
				sy = ui_manager_->GetCanvas()->GetHeight() - sy;
			}
			const int sx = ui_manager_->GetCanvas()->GetWidth()/2;
			PrintText(score, 0, sx, sy);
		}

		game_->Paint();

		if (dialog_) {
			ui_manager_->Paint(false);
		}

		if (is_purchasing_ || hiscore_agent_) {
			ui_manager_->GetPainter()->SetColor(WHITE, 0);
			const wstr _info = is_purchasing_? L"Communicating with App Store..." : L"Speaking to score server";
			PrintText(_info, 0,
				ui_manager_->GetCanvas()->GetWidth()/2,
				ui_manager_->GetCanvas()->GetHeight() - ui_manager_->GetPainter()->GetFontHeight());
		}
	}
	if (ok && !lTRICKY_IsLoopPaused) {
		game_->PreEndTick();
		game_->WaitPhysicsTick();
		ok = game_->EndTick();
	}
	if (ok && !lTRICKY_IsLoopPaused) {
		ok = game_->Tick();
	}
	resource_manager_->Tick();
	float real_time_ratio;
	v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
	ui_manager_->EndRender(real_time_ratio/kFps);

	if (music_player_) {
		music_player_->Update();
	}

	if (money_->GetLoadState() == cure::kResourceLoadComplete && !is_money_icon_added_) {
		is_money_icon_added_ = true;
		ui_manager_->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*money_->GetRamData(), money_->GetData(),
			uitbc::GUIImageManager::kCentered, uitbc::GUIImageManager::kAlphablend, 255);
	}

	if (playback_voice_instruction_) {
		playback_voice_instruction_ = false;
		str lInstructionFile;
		if (game_->GetComputerIndex() == 0) {
			lInstructionFile = "voice_shoot.wav";
		} else if (game_->GetComputerIndex() == 1) {
			lInstructionFile = "voice_drive.wav";
		}
		if (!lInstructionFile.empty()) {
			game_->AddContextObject(new UiCure::Sound(game_->GetResourceManager(), lInstructionFile, ui_manager_), cure::kNetworkObjectLocalOnly, 0);
		}
	}

	if (intro_streamer_ && intro_streamer_->Update()) {
		if (!is_paused_ && !intro_streamer_->IsPlaying()) {
			KillIntroVoice();
		}
	}

	if (game_over_timer_.IsStarted() && !is_paused_) {
		if (game_over_timer_.QueryTimeDiff() > 11.0 ||
			(game_->GetComputerIndex() == 1 && game_->GetCutie()->GetHealth() <= 0 && game_over_timer_.QueryTimeDiff() > 7.0) ||
			(game_->GetComputerIndex() == 0 && game_over_timer_.QueryTimeDiff() > 9.0)) {
			const int heart_balance = game_->GetHeartBalance();
			if (heart_balance == -kHeartPoints/2 || heart_balance == +kHeartPoints/2) {	// Somebody won.
				if (game_->GetComputerIndex() != -1 &&	// Computer in the game.
					game_->GetComputerIndex() != game_->GetWinnerIndex() &&	// Computer didn't win = user won over computer.
					game_->GetScore() >= 1000.0) {		// Negative score isn't any good - at least be positive.
#ifdef LEPRA_TOUCH_LOOKANDFEEL
					EnterHiscore(wstr(), FGCOLOR_DIALOG);
#else // Computer
					EnterHiscore(L"Press enter when you're done", FGCOLOR_DIALOG);
#endif // touch/computer
				} else {
					// Score wasn't high enough, not allowed in on hiscore list.
					SuperReset(true);
				}
			} else {
				// Game's not over, next round!
				SuperReset(true);
				playback_voice_instruction_ = true;
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
	UiCure::CppContextObject* avatar1 = game_->GetP1();
	UiCure::CppContextObject* avatar2 = game_->GetP2();
	if (!avatar1 || !avatar1->IsLoaded() || !avatar2 ||!avatar2->IsLoaded()) {
		return;
	}
	if (avatar1->GetPhysics()->GetEngineCount() >= 3) {
		game_->SetThrottle(avatar1, throttle_);
		steering_ = Math::Clamp(steering_, -1.0f, 1.0f);
		avatar1->SetEnginePower(1, steering_);
	}
	game_->SetThrottle(avatar2, 0);
	avatar2->SetEnginePower(1, 0);
	FingerMoveList::iterator x = g_finger_move_list.begin();
	is_throttling_ = false;
	for (; x != g_finger_move_list.end();) {
		x->tag_ = PollTap(*x);
		if (x->tag_ > 0) {
			++x;
		} else {
			g_finger_move_list.erase(x++);
		}
	}
#ifndef LEPRA_TOUCH
	// Test code.
	if (!g_finger_move_list.empty() && !g_finger_move_list.back().is_press_) {
		g_finger_move_list.clear();
	}
#endif // Computer
#endif // touch L&F
}

void App::DrawMatchStatus() {
	const float w = (float)ui_manager_->GetCanvas()->GetWidth();
	const float h = (float)ui_manager_->GetCanvas()->GetHeight();

#ifdef LEPRA_TOUCH_LOOKANDFEEL
	const float _angle = (game_->GetComputerIndex() != 1)? PIF/2 : 0;
#else // Computer.
	const float _angle = 0;
#endif // touch / computer.
	const int lWinner = game_->GetWinnerIndex();
	if (lWinner >= 0) {
		const int lSmallFontHeight = ui_manager_->GetFontManager()->GetFontHeight();
		uitbc::FontManager::FontId font_id = ui_manager_->GetFontManager()->GetActiveFontId();
		ui_manager_->GetFontManager()->SetActiveFont(big_font_id_);
		const bool is_game_over = (game_->GetHeartBalance() == -kHeartPoints/2 || game_->GetHeartBalance() == +kHeartPoints/2);
		if (is_game_over) {
			game_->EnableScoreCounting(false);
		}
		const wstr lWon = is_game_over? L"You rule!" : L"Won heart";
		const wstr lLost = is_game_over?  L"Defeat!" : L"Lost heart";
		const int lBackgroundSize = 100;
		if (game_->GetComputerIndex() == -1) {
			wstr lText1;
			wstr lText2;
			Color lColor1;
			Color lColor2;
			if (!!lWinner == game_->IsFlipRenderSide()) {
				lText1 = lWon;
				lText2 = lLost;
				lColor1 = LIGHT_GREEN;
				lColor2 = LIGHT_RED;
			} else {
				lText1 = lLost;
				lText2 = lWon;
				lColor1 = LIGHT_RED;
				lColor2 = LIGHT_GREEN;
			}
			const int x1 = (int)(w*1/4);
			const int x2 = (int)(w*3/4);
			const int y  = (int)(h/2);
			ui_manager_->GetPainter()->SetRenderMode(uitbc::Painter::kRmAlphablend);
			DrawRoundedPolygon(x1, y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f);
			DrawRoundedPolygon(x2, y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f);
			//ui_manager_->GetPainter()->SetRenderMode(uitbc::Painter::kRmNormal);
			ui_manager_->GetPainter()->SetColor(lColor1, 0);
			PrintText(lText1, +_angle, x1, y);
			ui_manager_->GetPainter()->SetColor(lColor2, 0);
			PrintText(lText2, -_angle, x2, y);
		} else {
			const int x = (int)(w/2);
			int y  = (int)(h/2);
			ui_manager_->GetPainter()->SetRenderMode(uitbc::Painter::kRmAlphablend);
			DrawRoundedPolygon(x, y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f);
			//ui_manager_->GetPainter()->SetRenderMode(uitbc::Painter::kRmNormal);
			const int lBigFontHeight = ui_manager_->GetFontManager()->GetFontHeight();
			const bool lShowHealth = (game_->GetComputerIndex() == 0 && game_->GetRoundIndex()+1 >= 2 && !is_game_over);
			if (lShowHealth) {
				y -= lSmallFontHeight + 8;
			}
			const float a = 0;
			if (lWinner != game_->GetComputerIndex()) {
				ui_manager_->GetPainter()->SetColor(LIGHT_GREEN, 0);
				PrintText(lWon, a, x, y);
			} else {
				ui_manager_->GetPainter()->SetColor(LIGHT_RED, 0);
				PrintText(lLost, a, x, y);
			}
			if (lShowHealth) {
				ui_manager_->GetFontManager()->SetActiveFont(font_id);
				y += lBigFontHeight+8;
				ui_manager_->GetPainter()->SetColor(FGCOLOR_DIALOG, 0);
				const wstr lRound = wstrutil::Format(L"Upcoming round %i", (game_->GetRoundIndex()+1)/2+1);
				PrintText(lRound, a, x, y);
				const float lInitialHealth = game_->GetVehicleStartHealth(game_->GetRoundIndex()+1);
				const wstr health = wstrutil::Format(L"You start with %i%% health", (int)Math::Round(100*lInitialHealth));
				y += lSmallFontHeight+2;
				PrintText(health, a, x, y);
			}
		}
		ui_manager_->GetFontManager()->SetActiveFont(font_id);

		game_over_timer_.TryStart();
	} else {
		float real_time_ratio;
		v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
		if (real_time_ratio < 0.95f) {
			const int lFrameMod = game_->cure::GameTicker::GetTimeManager()->GetCurrentPhysicsFrame() % 20;
			if (lFrameMod > 12) {
				return;
			}
			const float lBackgroundSize = 20;
			const wstr _text = L"S";
			ui_manager_->GetPainter()->SetRenderMode(uitbc::Painter::kRmAlphablend);
			if (game_->GetComputerIndex() == -1) {
				const int x1 = (int)(w*1/4);
				const int x2 = (int)(w*3/4);
				const int y  = (int)(h/2);
				DrawRoundedPolygon((float)x1, (float)y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f, 10);
				DrawRoundedPolygon((float)x2, (float)y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f, 10);
				ui_manager_->GetPainter()->SetColor(FGCOLOR_DIALOG, 0);
				PrintText(_text, +_angle, x1, y);
				ui_manager_->GetPainter()->SetColor(FGCOLOR_DIALOG, 0);
				PrintText(_text, -_angle, x2, y);
			} else {
				const int x = (int)(w/2);
				const int y = (int)(h*2/5);
				DrawRoundedPolygon((float)x, (float)y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f, 10);
				ui_manager_->GetPainter()->SetColor(FGCOLOR_DIALOG, 0);
				PrintText(_text, 0, x, y);
			}
		}
	}
}

void App::DrawVehicleSteering() {
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	const float w = (float)ui_manager_->GetCanvas()->GetWidth();
	const float h = (float)ui_manager_->GetCanvas()->GetHeight();
	const float m = kButtonMargin;
	const UiCure::CppContextObject* avatar1 = game_->GetP1();

	// Draw throttle and steering indicators on iOS.
	if (game_->GetComputerIndex() != 0 &&
		arrow_->GetLoadState() == cure::kResourceLoadComplete &&
		steering_wheel_->GetLoadState() == cure::kResourceLoadComplete &&
		avatar1->GetPhysics()->GetEngineCount() >= 3) {
		float v0 = 0.5f;
		float v1 = throttle_*0.5f+0.5f;
		if (throttle_ < 0) {
			std::swap(v0, v1);
		}
		const float lMeterHalfWidth = 52/2;
		const float aw = (float)arrow_->GetRamData()->GetWidth();
		const float ah = (float)arrow_->GetRamData()->GetHeight();
		const float mw = kMeterHeight*0.5f + (ah-5)*0.5f;
		int o = (int)(m + kMeterHeight*0.5f + ah-5);
		if (is_throttling_) {
			o += kTouchOffset;
		}
		throttle_meter_offset_ = o = Math::Lerp(throttle_meter_offset_, o, 0.8f);

		// Left player.
		if (game_->GetComputerIndex() == -1) {	// 2P?
			const int x = o;
			if (throttle_ == 0 && !is_throttling_) {
				DrawMeter(x, (int)(m+lMeterHalfWidth), -PIF/2, kMeterHeight, 0, 1);
			} else {
				DrawMeter(x, (int)(m+lMeterHalfWidth), -PIF/2, kMeterHeight, v0, v1);
			}
			InfoText(1, L"Throttle/brake", 0, 14, 0);
			DrawTapIndicator(1, 24, -(int)(throttle_*(kMeterHeight/2-1.3f))-1, -PIF/2);
			DrawImage(arrow_->GetData(), x+m+mw-2,		m+lMeterHalfWidth, aw, ah, -PIF/2);
			DrawImage(arrow_->GetData(), x-m-mw+1.5f,	m+lMeterHalfWidth, aw, ah, +PIF/2);

			float px;
			float py;
			const float s = GetSteeringWheelGuiInfo(w, h, px, py);
			const tbc::PhysicsEngine* lSteering = avatar1->GetPhysics()->GetEngine(1);
			const float a = lSteering->GetLerpThrottle(0.2f, 0.2f, false) * -1.5f - PIF/2;
			const float ax = -::sin(a) * s * 0.45f;
			const float ay = -::cos(a) * s * 0.45f;
			DrawLine(px+ax, py+ay, px+ax*2.5f, py+ay*2.5f);
			DrawImage(steering_wheel_->GetData(), px, py, s, s, a);
			InfoText(1, L"Left/right", -PIF/2, 0, -10);
		} else {
			const int y = (int)h-o;
			if (throttle_ == 0 && !is_throttling_) {
				DrawMeter((int)(m+lMeterHalfWidth), y, 0, kMeterHeight, 0, 1);
			} else {
				DrawMeter((int)(m+lMeterHalfWidth), y, 0, kMeterHeight, v0, v1);
			}
			InfoText(1, L"Throttle/brake", PIF/2, 0, -14);
			DrawTapIndicator(1, 24, -(int)(throttle_*(kMeterHeight/2-1.3f))-1, 0);
			DrawImage(arrow_->GetData(), m+lMeterHalfWidth+1,	y-m-mw+1,	aw, ah, 0);
			DrawImage(arrow_->GetData(), m+lMeterHalfWidth,		y+m+mw-2,	aw, ah, PIF);

			float px;
			float py;
			const float s = GetSteeringWheelGuiInfo(w, h, px, py);
			const tbc::PhysicsEngine* lSteering = avatar1->GetPhysics()->GetEngine(1);
			const float a = lSteering->GetLerpThrottle(0.2f, 0.2f, false) * -1.5f;
			const float ax = -::sin(a) * s * 0.45f;
			const float ay = -::cos(a) * s * 0.45f;
			DrawLine(px+ax, py+ay, px+ax*2.5f, py+ay*2.5f);
			DrawImage(steering_wheel_->GetData(), px, py, s, s, a);
			InfoText(1, L"Left/right", 0, -10, 0);
		}
	}
	if (game_->GetComputerIndex() != 1 &&
		grenade_->GetLoadState() == cure::kResourceLoadComplete) {
		// Bomb button.
		const int s = std::min((int)(ui_manager_->GetCanvas()->GetHeight() * 0.096f), 56);
		float t = std::min(1.0f, game_->GetLauncherLockPercent());
		const float r = (s+1) * 1.414f;
		Color c = GREEN;
		if (t < 1.0f) {
			const float lMinimumWidth = 0.5f;
			t = Math::Lerp(lMinimumWidth, 1.0f, t);
			c = RED;
		}
		const float r2 = r*0.5f;
		if (game_->GetComputerIndex() == -1) {
			// Dual play = portrait layout.
			DrawRoundedPolygon(w-r2*t-m*2, h/2, r2, c, t, 1.0f, 10);
			DrawImage(grenade_->GetData(), w-r/2-m*2, h/2, (float)s, (float)s, PIF/2);
		} else {
			// Single play = landscape layout.
			DrawRoundedPolygon(w/2, h-r2*t-m*2, r2, c, 1.0f, t, 10);
			DrawImage(grenade_->GetData(), w/2, h-r/2-m*2, (float)s, (float)s, 0);
		}
	}
#endif // touch
}

void App::DrawHealthBar() {
	const float w = (float)ui_manager_->GetCanvas()->GetWidth();
	const float button_width = kButtonWidth;

	// Draw health bar.
	Cutie* cutie = game_->GetCutie();
	if (game_->GetComputerIndex() == -1) {	// Two players.
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		const float h = (float)ui_manager_->GetCanvas()->GetHeight();
		DrawMeter((int)w/2, (int)h/2, PIF, h/2, 0, cutie->GetHealth()*1.0002f, 20, 3, 20);
#else // !touch
		DrawMeter((int)w/4, (int)(button_width*0.7f), -PIF/2, w/3, 0, cutie->GetHealth()*1.0002f, 20, 3, 20);
#endif // touch/!touch
	} else if (game_->GetComputerIndex() != 0) {	// Single player Cutie.
		DrawMeter((int)w/2, (int)(button_width*0.7f), -PIF/2, w/3, 0, cutie->GetHealth()*1.0002f, 20, 3, 20);
	}
}

void App::DrawLauncherIndicators(int computer_index) {
	// Draw launcher indicators.
	if (computer_index != 1 &&	// Computer not running launcher.
		arrow_->GetLoadState() == cure::kResourceLoadComplete) {
		const float w = (float)ui_manager_->GetCanvas()->GetWidth();
		const float h = (float)ui_manager_->GetCanvas()->GetHeight();
		const float m = kButtonMargin;

		// Draw launcher guides, used for steering in touch device.
		float pitch;
		float guide_pitch;
		float yaw;
		float guide_yaw;
		game_->GetLauncher()->GetAngles(game_->GetCutie(), pitch, guide_pitch, yaw, guide_yaw);

		float x;
		float y;
		float dx = 0;
		float dy = 0;
		float ox = 0;
		float oy = -20;
		float _value1 = pitch/-1;
		float _value2 = guide_pitch/-1;
		float lDrawAngle = 0;
		const float aw = (float)arrow_->GetRamData()->GetWidth();
		const float ah = (float)arrow_->GetRamData()->GetHeight();
		const float lBarrelCompassHalfHeight = BARREL_COMPASS_HEIGHT/2 + ah/2-2 + 2;
		const float lBarrelCenterOffset = m + ah-5 + BARREL_COMPASS_HEIGHT/2 + 2;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (computer_index == 0) {	// Single play = landscape.
			x = m+aw/2;
			y = h-lBarrelCenterOffset;
			lDrawAngle = -PIF/2;
			dy = lBarrelCompassHalfHeight;
		} else {	// Dual play = portrait.
			x = w-lBarrelCenterOffset;
			y = h-m-aw/2;
			dx = lBarrelCompassHalfHeight;
			std::swap(ox, oy);
			_value1 = 1 - _value1;
			_value2 = 1 - _value2;
		}
		if (std::find_if(g_finger_move_list.begin(), g_finger_move_list.end(), IsPressing(3)) != g_finger_move_list.end()) {
			if (computer_index == 0) {	// Single play = landscape.
				y -= kTouchOffset;
			} else {	// Dual play = portrait.
				x -= kTouchOffset;
			}
		}
		if (computer_index == 0) {	// Single play = landscape.
			lift_meter_offset_ = y = Math::Lerp(lift_meter_offset_, y, 0.8f);
		} else {	// Dual play = portrait.
			lift_meter_offset_ = x = Math::Lerp(lift_meter_offset_, x, 0.8f);
		}
#else // !touch
		x = m+aw/2 + 30;
		y = h-lBarrelCenterOffset;
		lDrawAngle = -PIF/2;
		if (computer_index == -1) {	// Two players.
			x += w/2;
		}
		dy = lBarrelCompassHalfHeight;
#endif // touch/!touch
		DrawBarrelCompass((int)x, (int)y, lDrawAngle, (int)aw-8, _value1, _value2);
		InfoText(2, L"Up/down compass", lDrawAngle+PIF, ox, oy);
		const float lLiftThrottle = game_->GetLauncher()->GetPhysics()->GetEngine(0)->GetValue();
		const float lCenteredValue = (computer_index == 0)? _value1*2-1 : -(_value1*2-1);
		const int lLiftOffset = (int)((BARREL_COMPASS_HEIGHT-4)*lCenteredValue/2 + 9*lLiftThrottle);
		DrawTapIndicator(3, 24, lLiftOffset, lDrawAngle+PIF/2);
		DrawImage(arrow_->GetData(), x-dx, y-dy, aw, ah, lDrawAngle+PIF/2);
		DrawImage(arrow_->GetData(), x+dx, y+dy, aw, ah, lDrawAngle-PIF/2);

		dx = 0;
		dy = 0;
		ox = -30;
		oy = 0;
		_value1 = yaw/(PIF+0.1f)+0.5f;
		_value2 = (guide_yaw)/(PIF+0.1f)+0.5f;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (computer_index == 0) {	// Single play = landscape.
			x = w-lBarrelCenterOffset;
			y = h-m-aw/2;
			dx = -lBarrelCompassHalfHeight;
		} else {	// Dual play = portrait.
			x = w-m-aw/2;
			y = m+lBarrelCenterOffset;
			dy = lBarrelCompassHalfHeight;
			ox = 0;
			oy = +30;
			_value1 = 1 - _value1;
			_value2 = 1 - _value2;
		}

		if (std::find_if(g_finger_move_list.begin(), g_finger_move_list.end(), IsPressing(4)) != g_finger_move_list.end()) {
			if (computer_index == 0) {	// Single play = landscape.
				y -= kTouchOffset;
			} else {
				x -= kTouchOffset;
			}
		}
		if (computer_index == 0) {	// Single play = landscape.
			yaw_meter_offset_ = y = Math::Lerp(yaw_meter_offset_, y, 0.8f);
		} else {
			yaw_meter_offset_ = x = Math::Lerp(yaw_meter_offset_, x, 0.8f);
		}
#else // !touch
		x = w-lBarrelCenterOffset - 30;
		y = h-m-aw/2;
		dx = -lBarrelCompassHalfHeight;
#endif // touch/!touch
		DrawBarrelCompass((int)x, (int)y, +PIF/2+lDrawAngle, (int)aw-8, _value1, _value2);
		InfoText(2, L"Left/right compass", lDrawAngle+PIF/2, ox, oy);
		const float lRotateThrottle = game_->GetLauncher()->GetPhysics()->GetEngine(1)->GetValue();
		const float lCenteredRotation = (computer_index == 0)? _value1*2-1 : -(_value1*2-1);
		const int lRotateOffset = (int)((BARREL_COMPASS_HEIGHT-4)*lCenteredRotation/2 - 9*lRotateThrottle);
		DrawTapIndicator(4, +24, lRotateOffset, lDrawAngle);
		DrawImage(arrow_->GetData(), x-dx, y-dy, aw, ah, lDrawAngle);
		DrawImage(arrow_->GetData(), x+dx, y+dy, aw, ah, lDrawAngle-PIF);
	}
}

void App::DrawHearts() {
	flip_draw_ = false;

	// Draw hearts.
	if (heart_->GetLoadState() == cure::kResourceLoadComplete &&
		grey_heart_->GetLoadState() == cure::kResourceLoadComplete) {
		const float w = (float)ui_manager_->GetCanvas()->GetWidth();

		ui_manager_->GetPainter()->SetRenderMode(uitbc::Painter::kRmAlphablend);
		ui_manager_->GetPainter()->SetAlphaValue(255);
		const float margin = 8;
		float x = margin;
		float y = kButtonWidth + 8*2 + 3;
		float _angle = 0;
		const int lBalance = -game_->GetHeartBalance();
		static float lHeartBeatTime = 0;
		lHeartBeatTime += 0.5f;
		if (lHeartBeatTime > 4*PIF) {
			lHeartBeatTime -= 4*PIF;
		}
		const float iw = (float)heart_->GetRamData()->GetWidth();
		const float ih = (float)heart_->GetRamData()->GetHeight();
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (game_->GetComputerIndex() == -1) {
			x = w/2 - kButtonWidth/2 - 8 - 3 - ih;
			_angle = -PIF/2;
		}
#endif	// touch L&F
		if (game_->GetComputerIndex() == 1 && !geti_phone_button_) {
			x = margin;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
			x += kButtonWidth/2 - iw/2;
#endif // touch
			y = margin;
		}
		if (game_->GetComputerIndex() == -1) {
			y = margin;
		}
		float lHeartBeatLoopTime = lHeartBeatTime;
		int lHeartIndex = 0;
		for (int i = -kHeartPoints/2; i < +kHeartPoints/2; ++i, lHeartBeatLoopTime += 0.6f) {
			if (game_->GetComputerIndex() != 0) {
				vec3 v(x+iw/2, y+iw/2, _angle);
				if (i < lBalance) {
					const float lHeartBeatFactor = (lHeartBeatLoopTime < 2*PIF) ? 1 : 1+0.15f*sin(lHeartBeatLoopTime);
					const float hw = iw * lHeartBeatFactor;
					const float hh = ih * lHeartBeatFactor;
					v = heart_pos_[lHeartIndex] = Math::Lerp(heart_pos_[lHeartIndex], v, 0.07f);
					++lHeartIndex;
					DrawImage(heart_->GetData(), v.x, v.y, hw, hh, v.z);
				} else {
					DrawImage(grey_heart_->GetData(), v.x, v.y, iw, ih, v.z);
				}
				y += iw+8;
			}
		}
		x = w - margin - iw;
		y = kButtonWidth + 8*2 + 3;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (game_->GetComputerIndex() == -1) {
			x = w/2 + kButtonWidth/2 + margin;
			y = margin;
			_angle = PIF/2;
		} else {
			x = margin;
			y = kButtonWidth + margin*2;
			_angle = 0;
		}
#endif	// touch L&F
		if (game_->GetComputerIndex() == -1) {
			y = margin;
		} else if (game_->GetComputerIndex() == 0 && !geti_phone_button_) {
#ifdef LEPRA_TOUCH_LOOKANDFEEL
			x += kButtonWidth/2 - iw/2;
#endif // touch
			y = margin;
		}
		y += (iw+8) * (kHeartPoints - 1);
		lHeartBeatLoopTime = lHeartBeatTime;
		for (int i = -kHeartPoints/2; i < +kHeartPoints/2; ++i, lHeartBeatLoopTime += 0.6f) {
			if (game_->GetComputerIndex() != 1) {
				vec3 v(x+iw/2, y+iw/2, _angle);
				if (i >= lBalance) {
					const float lHeartBeatFactor = (lHeartBeatLoopTime < 2*PIF) ? 1 : 1+0.15f*sin(lHeartBeatLoopTime);
					const float hw = iw * lHeartBeatFactor;
					const float hh = ih * lHeartBeatFactor;
					v = heart_pos_[lHeartIndex] = Math::Lerp(heart_pos_[lHeartIndex], v, 0.07f);
					++lHeartIndex;
					DrawImage(heart_->GetData(), v.x, v.y, hw, hh, v.z);
				} else {
					DrawImage(grey_heart_->GetData(), v.x, v.y, iw, ih, v.z);
				}
				y -= iw+8;
			}
		}
		//ui_manager_->GetPainter()->SetRenderMode(uitbc::Painter::kRmNormal);
	}
}

void App::DrawKeys() {
#ifndef LEPRA_TOUCH_LOOKANDFEEL
	// Draw keyboard buttons.
	if (game_->GetFlybyMode() == Game::kFlybyInactive && !start_timer_.IsStarted()) {
		start_timer_.Start();
	}
	if (start_timer_.IsStarted() && start_timer_.QueryTimeDiff() < 7.0 &&
		keyboard_button_->GetLoadState() == cure::kResourceLoadComplete) {
		const float w = (float)ui_manager_->GetCanvas()->GetWidth();
		const float h = (float)ui_manager_->GetCanvas()->GetHeight();

		ui_manager_->GetPainter()->SetColor(GRAY);
		const int sw = (int)w;
		const int sh = (int)h;
		const int m = kButtonMargin;
		const int iw = keyboard_button_->GetRamData()->GetWidth();
		const int ih = keyboard_button_->GetRamData()->GetHeight();
		int bx = sw/2;
		const int by = sh/2 - ih/3;
		if (game_->GetComputerIndex() == -1) {
			bx = sw/4;
			ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*3/2-m, by-ih-m);	// w
			ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw/2-m*0, by-ih-m);	// e
			ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*2-m*2, by);	// a
			ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*1-m*1, by);	// s
			ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*0-m*0, by);	// d
			PrintText(L"W", 0, bx-iw-m*3, by-ih/2-m*2);
			PrintText(L"E", 0, bx-m*2, by-ih/2-m*2);
			PrintText(L"A", 0, bx-iw*3/2-m*4, by+ih/2-m*2);
			PrintText(L"S", 0, bx-iw/2-m*3, by+ih/2-m*2);
			PrintText(L"D", 0, bx+iw/2-m*2, by+ih/2-m*2);
			bx = sw*3/4;
		}
		bx += iw*3/2;
		ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*1-m*1, by-ih-m);
		ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*4+m*0, by);
		ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*2-m*2, by);
		ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*1-m*1, by);
		ui_manager_->GetPainter()->DrawImage(keyboard_button_->GetData(), bx-iw*0-m*0, by);
		PrintText(L"Up",	0, bx-iw/2-m*3, by-ih/2-m*2);
		PrintText(L"Ctrl",	0, bx-iw*7/2-m*2, by+ih/2-m*2);
		PrintText(L"L",		0, bx-iw*3/2-m*4, by+ih/2-m*2);
		PrintText(L"Dn",	0, bx-iw/2-m*3, by+ih/2-m*2);
		PrintText(L"R",		0, bx+iw/2-m*2, by+ih/2-m*2);
	}
#endif // Computer
}

void App::DrawInfoTexts() const {
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	if (!is_paused_) {
		const Color c = ui_manager_->GetPainter()->GetColor(0);
		ui_manager_->GetPainter()->SetColor(info_text_color_, 0);

		for (size_t x = 0; x < info_text_array_.size(); ++x) {
			const InfoTextData& data = info_text_array_[x];
			PrintText(data.text_, -data.angle_, (int)data.coord_.x, (int)data.coord_.y);
		}

		ui_manager_->GetPainter()->SetColor(c, 0);
	}
#endif // touch

	ClearInfoTexts();
}

void App::ClearInfoTexts() const {
	info_text_array_.clear();
}

bool App::PreDrawHud() {
	if ((game_->GetFlybyMode() != Game::kFlybyInactive && game_->GetFlybyMode() != Game::kFlybyIntroductionFinishingUp) || dialog_) {
		return false;
	}

	cure::ContextObject* avatar1 = game_->GetP1();
	cure::ContextObject* avatar2 = game_->GetP2();
	if (!avatar1 || !avatar1->IsLoaded() || !avatar2 || !avatar2->IsLoaded()) {
		return false;
	}


	flip_draw_ = game_->IsFlipRenderSide();

	return true;
}



void App::DrawLine(float x1, float y1, float x2, float y2) {
	float a = 0;
	Transpose(x1, y1, a);
	Transpose(x2, y2, a);
	ui_manager_->GetPainter()->DrawLine((int)x1, (int)y1, (int)x2, (int)y2);
}

void App::DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const {
	Transpose (cx, cy, angle);
	cx -= 0.5f;

	pen_x_ = cx;
	pen_y_ = cy;
	const float ca = ::cos(angle);
	const float sa = ::sin(angle);
	const float w2 = w*0.5f;
	const float h2 = h*0.5f;
	const float x = cx - w2*ca - h2*sa;
	const float y = cy - h2*ca + w2*sa;
	const vec2 c[] = { vec2(x, y), vec2(x+w*ca, y-w*sa), vec2(x+w*ca+h*sa, y+h*ca-w*sa), vec2(x+h*sa, y+h*ca) };
	const vec2 t[] = { vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1) };
#define V(z) std::vector<vec2>(z, z+LEPRA_ARRAY_COUNT(z))
	ui_manager_->GetPainter()->DrawImageFan(image_id, V(c), V(t));
}

void App::DrawRoundedPolygon(int x, int y, int radius, const Color& color, float scale_x, float scale_y) const {
	DrawRoundedPolygon((float)x, (float)y, (float)radius, color, scale_x, scale_y, 20);
}

void App::DrawRoundedPolygon(float x, float y, float radius, const Color& color, float scale_x, float scale_y, int corner_radius) const {
	float _angle = 0;
	Transpose(x, y, _angle);

	pen_x_ = (float)x;
	pen_y_ = (float)y;
	const int dx = (int)(radius * scale_x);
	const int dy = (int)(radius * scale_y);
	ui_manager_->GetPainter()->SetColor(color, 0);
	ui_manager_->GetPainter()->SetAlphaValue(color.alpha_);
	ui_manager_->GetPainter()->DrawRoundedRect(PixelRect((int)x-dx, (int)y-dy, (int)x+dx, (int)y+dx), corner_radius, 0xF, true);
	ui_manager_->GetPainter()->SetAlphaValue(255);
}

void App::DrawMeter(int x, int y, float angle, float _size, float min_value, float max_value, int _width, int spacing, int bar_count) const {
	Transpose(x, y, angle);

	pen_x_ = (float)x;
	pen_y_ = (float)y;

	Color start_color = RED;
	Color lEndColor = GREEN;
	const int lBarCount = bar_count;
	const int lBarSpace = spacing;
	const int lBarHeight = (int)(_size/lBarCount) - lBarSpace;
	const int lBarWidth = _width;
	int lMaxValue = (int)(max_value * lBarCount);
	int lMinValue = (int)(min_value * lBarCount);
	if (max_value <= 0) {
		lMaxValue = -1;
	}
	const int x_step = -(int)(::sin(angle)*(lBarHeight+lBarSpace));
	const int y_step = -(int)(::cos(angle)*(lBarHeight+lBarSpace));
	const bool lXIsMain = ::abs(x_step) >= ::abs(y_step);
	x -= (int)(x_step * (lBarCount-1) * 0.5f);
	y -= (int)(y_step * (lBarCount-1) * 0.5f);
	for (int i = 0; i < lBarCount; ++i) {
		const Color c = (i >= lMinValue && i <= lMaxValue)?
			Color(start_color, lEndColor, i/(float)lBarCount) : DARK_GRAY;
		ui_manager_->GetPainter()->SetColor(c);
		if (lXIsMain) {
			ui_manager_->GetPainter()->FillRect(x, y-lBarWidth/2, x+lBarHeight, y+lBarWidth/2);
		} else {
			ui_manager_->GetPainter()->FillRect(x-lBarWidth/2, y, x+lBarWidth/2, y+lBarHeight);
		}
		x += x_step;
		y += y_step;
	}
}

void App::DrawTapIndicator(int tag, int dx, int dy, float angle) const {
	angle = -angle;
	if (flip_draw_) {
		//dx = -dx;
		//dy = -dy;
		angle += PIF;
	}
	vec2 point((float)dx, (float)dy);
	point.RotateAround(vec2(), angle);
	int x = (int)(pen_x_ + point.x);
	int y = (int)(pen_y_ + point.y);
	int x2 = x + (int)(20*::cos(angle));
	int y2 = y + (int)(20*::sin(angle));
	ui_manager_->GetPainter()->SetColor(WHITE);
	FingerMoveList::iterator i = g_finger_move_list.begin();
	for (; i != g_finger_move_list.end(); ++i) {
		if (i->tag_ != tag) {
			continue;
		}
		int x3 = i->last_y_;
		int y3 = ui_manager_->GetCanvas()->GetHeight() - i->last_x_;
		//Transpose(x3, y3, _angle);
		ui_manager_->GetPainter()->DrawLine(x3, y3, x2, y2);
		ui_manager_->GetPainter()->DrawLine(x2, y2, x, y);
	}
}

void App::DrawBarrelCompass(int x, int  y, float angle, int _size, float value1, float value2) const {
	Transpose(x, y, angle);

	pen_x_ = (float)x;
	pen_y_ = (float)y;

	float ca = ::cos(angle);
	ca = (::fabs(ca) < 1e-5)? 0 : ca;
	float sa = ::sin(angle);
	sa = (::fabs(sa) < 1e-5)? 0 : sa;
	const int line_count = kBarrelCompassLineCount;
	const int lLineSpacing = kBarrelCompassLineSpacing;
	const int __width = BARREL_COMPASS_HEIGHT;
	const int w2 = __width/2;
	const int s2 = _size/2;
	const int s3 = _size/3;
	const int s23 = _size*2/3;
	ui_manager_->GetPainter()->SetColor(DARK_GRAY);
	for (int i = 0; i < line_count; ++i) {
		const int __height = (i%3 == 0)? s23 : s3;
		const int x2 = -w2 + i*lLineSpacing;
		const int y2 = s2-__height;
		ui_manager_->GetPainter()->DrawLine((int)(x-x2*ca+s2*sa), (int)(y+y2*ca+x2*sa), (int)(x-x2*ca+y2*sa), (int)(y+s2*ca+x2*sa));
	}
	const int _value1 = (int)((value1-0.5f)*-__width);
	int _value2 = (int)((value2-0.5f)*-__width);
	ui_manager_->GetPainter()->SetColor(RED);
	--_value2;
	int xca = (int)(_value2*ca);
	int ysa = (int)(_value2*sa);
	ui_manager_->GetPainter()->DrawLine(x+xca, y+ysa-(int)(s2*ca), x+xca-(int)(s2*sa), y+ysa);
	xca += (int)Math::Round(2*ca);
	ysa += (int)Math::Round(2*sa);
	ui_manager_->GetPainter()->DrawLine(x+xca, y+ysa-(int)(s2*ca), x+xca-(int)(s2*sa), y+ysa);
	ui_manager_->GetPainter()->SetColor(WHITE);
	ui_manager_->GetPainter()->DrawLine((int)(x+_value1*ca+s2*sa), (int)(y-s2*ca+_value1*sa), (int)(x+_value1*ca-s2*sa), (int)(y+s2*ca+_value1*sa));
}

void App::InfoText(int player, const wstr& info, float angle, float dx, float dy) const {
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	if (flip_draw_) {
		dx = -dx;
		dy = -dy;
		angle += PIF;
	}
#endif // touch

	const double lLastTime = (player == 1)? player1_last_touch_.QueryTimeDiff() : player2_last_touch_.QueryTimeDiff();
	const double lShowDelayTime = (player == 1)? player1_touch_delay_.QueryTimeDiff() : player2_touch_delay_.QueryTimeDiff();
	if (lLastTime < 15) {	// Delay until shown.
		if (lShowDelayTime > 2) {	// Delay after next touch until hidden.
			return;
		}
	} else {
		(player == 1)? player1_touch_delay_.ClearTimeDiff() : player2_touch_delay_.ClearTimeDiff();
	}
	InfoTextData data;
	data.text_ = info;
	data.coord_ = vec2(pen_x_+dx, pen_y_+dy);
	data.angle_ = angle;
	info_text_array_.push_back(data);
}

void App::PrintText(const wstr& text, float angle, int center_x, int center_y) const {
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


float App::GetSteeringWheelGuiInfo(float w, float h, float& x, float& y) const {
	if (game_->GetComputerIndex() == -1) {
		// 2P
		const float s = std::max(128.0f, h * 0.25f);
		x = s*0.15f;
		y = h-s*0.3f;
		return s;
	} else {
		// 1P
		const float s = w * 0.25f;
		x = w-s*0.3f;
		y = h-s*0.15f;
		return s;
	}
}

void App::Layout() {
	PixelRect rect = ui_manager_->GetDesktopWindow()->GetScreenRect();
	rect.left_ = rect.GetCenterX()-5;
	rect.right_ = rect.left_+10;
	player_splitter_->SetPos(rect.left_, rect.top_);
	player_splitter_->SetPreferredSize(rect.GetSize());
	player_splitter_->SetVisible(game_->GetComputerIndex() < 0);
	do_layout_ = false;

	if (!pause_button_) {
		return;
	}
	const int s = 8;
	//const int x = s;
	const int px = pause_button_->GetSize().x;
	const int py = pause_button_->GetSize().y;
	//const int dy = py * 4/3;
	//const int sy = ui_manager_->GetCanvas()->GetHeight() / 20 + 34;
	int tx;
	int ty;
	int tx2;
	int ty2;
	if (game_->GetComputerIndex() == -1) {
		tx = ui_manager_->GetCanvas()->GetWidth()/2 - px/2;
		ty = ui_manager_->GetCanvas()->GetHeight() - s - py;
		tx2 = tx;
		ty2 = s;
	} else {
		tx = ui_manager_->GetCanvas()->GetWidth() - s - px;
		ty = s;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		tx2 = tx;
		ty2 = ty;
#else // Computer L&F
		tx2 = s;
		ty2 = ty;
		if (game_->GetComputerIndex() == 1 && !geti_phone_button_) {
			tx2 = tx;
			ty2 = ty;
		}
#endif // touch L&F / Computer L&F
	}
	/*lazy_button_->SetPos(x, sy);
	hard_button_->SetPos(x, sy+dy);
	original_button_->SetPos(x, sy+dy*2);
	m1PButton_->SetPos(x, sy);
	m2PButton_->SetPos(x, sy+dy);
	next_button_->SetPos(x, sy);
	reset_button_->SetPos(x, sy);
	retry_button_->SetPos(x, sy+dy);*/
	if (geti_phone_button_) {
		geti_phone_button_->SetPos(tx, ty);
	}
	pause_button_->SetPos(tx2, ty2);

	if (dialog_) {
		dialog_->Center();
	}
}



void App::Suspend(bool hard) {
	if (music_player_) {
		music_player_->Pause();
	}
	DoPause();
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
}

void App::Resume(bool hard) {
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
	if (!game_ || game_->GetFlybyMode() != Game::kFlybyInactive || dialog_ || is_paused_) {
		return false;
	}
	UiCure::CppContextObject* avatar1 = game_->GetP1();
	UiCure::CppContextObject* avatar2 = game_->GetP2();
	if (!avatar1 || !avatar1->IsLoaded() || !avatar2 ||!avatar2->IsLoaded()) {
		return false;
	}
	switch (game_->GetComputerIndex()) {
		case 0:	avatar1 = avatar2;	break;
		case 1:	avatar2 = avatar1;	break;
	}
	enum Directive {
		kDirectiveNone,
		kDirectiveUp,
		kDirectiveDown,
		kDirectiveLeft,
		kDirectiveRight,
		kDirectiveFunction,
	};
	Directive __directive = kDirectiveNone;
	UiCure::CppContextObject* avatar = avatar2;
	switch (key_code) {
		case UIKEY(Up):
		case UIKEY(Numpad8):	__directive = kDirectiveUp;					break;
		case UIKEY(W):		__directive = kDirectiveUp;		avatar = avatar1;	break;
		case UIKEY(Down):
		case UIKEY(Numpad2):
		case UIKEY(Numpad5):	__directive = kDirectiveDown;					break;
		case UIKEY(S):		__directive = kDirectiveDown;		avatar = avatar1;	break;
		case UIKEY(Left):
		case UIKEY(Numpad4):	__directive = kDirectiveLeft;					break;
		case UIKEY(A):		__directive = kDirectiveLeft;		avatar = avatar1;	break;
		case UIKEY(Right):
		case UIKEY(Numpad6):	__directive = kDirectiveRight;					break;
		case UIKEY(D):		__directive = kDirectiveRight;		avatar = avatar1;	break;
		case UIKEY(Lctrl):
		case UIKEY(Rctrl):
		case UIKEY(Insert):
		case UIKEY(Numpad0):	__directive = kDirectiveFunction;				break;
		case UIKEY(E):
		case UIKEY(F):		__directive = kDirectiveFunction;	avatar = avatar1;	break;

#ifdef LEPRA_DEBUG
		case UIKEY(0): {
			if (!factor) {
				game_->NextComputerIndex();
				do_layout_ = true;
			}
		} break;
		case UIKEY(Plus): {
			if (!factor) {
				const cure::ObjectPositionalData* position = 0;
				avatar1->UpdateFullPosition(position);
				if (position) {
					cure::ObjectPositionalData* new_placement = (cure::ObjectPositionalData*)position->Clone();
					new_placement->position_.transformation_.GetPosition().x -= 10;
					avatar1->SetFullPosition(*new_placement, 0);
				}
			}
		} break;
		case UIKEY(9): {
			if (!factor) {
				const cure::ObjectPositionalData* position = 0;
				avatar1->UpdateFullPosition(position);
				if (position) {
					cure::ObjectPositionalData* new_placement = (cure::ObjectPositionalData*)position->Clone();
					new_placement->position_.transformation_.GetOrientation().RotateAroundOwnY(PIF*0.4f);
					avatar1->SetFullPosition(*new_placement, 0);
				}
			}
		} break;
		case UIKEY(8): {
			if (!factor) {
				game_->FlipRenderSides();
			}
		} break;
		case UIKEY(7): {
			game_->GetCutie()->DrainHealth(1);
		} break;
		case UIKEY(6): {
			if (!factor) {
				const cure::ObjectPositionalData* position = 0;
				avatar1->UpdateFullPosition(position);
				if (position) {
					cure::ObjectPositionalData* new_placement = (cure::ObjectPositionalData*)position->Clone();
					new_placement->position_.transformation_.GetPosition().x += 30;
					new_placement->position_.transformation_.GetPosition().y += 20;
					new_placement->position_.transformation_.GetPosition().z += 15;
					avatar1->SetFullPosition(*new_placement, 0);
				}
			}
		} break;
		case UIKEY(5): {
			if (!factor) {
				game_->SetComputerIndex((game_->GetComputerIndex() == 0)? 1 : 0);
			}
		} break;
		case UIKEY(4): {
			if (!factor) {
				if (ui_manager_->GetCanvas()->GetOutputRotation() == 0) {
					ui_manager_->GetCanvas()->SetOutputRotation(90);
				} else {
					ui_manager_->GetCanvas()->SetOutputRotation(-90);
				}
			}
		} break;
		case UIKEY(3): {
			if (!factor) {
				delete dialog_;
				dialog_ = 0;
				game_->AddScore(Random::Uniform(500.0, 5000.0), Random::Uniform(500.0, 5000.0));
				EnterHiscore(wstr(), WHITE);
			}
		} break;
#endif // Debug
	}

	if (game_->IsFlipRenderSide()) {
		//std::swap(avatar1, avatar2);
		avatar = (avatar == avatar1)? avatar2 : avatar1;
	}
	switch (__directive) {
		case kDirectiveNone: {
		} break;
		case kDirectiveUp: {
			avatar->SetEnginePower(0, +1*factor);
		} break;
		case kDirectiveDown: {
			if (game_->GetComputerIndex() != 0 && avatar == avatar1) {
				reverse_and_brake_ = factor;
				if (!reverse_and_brake_) {
					avatar->SetEnginePower(0, 0);
					avatar->SetEnginePower(2, 0);
				}
			}
			if (game_->GetComputerIndex() != 1 && avatar == avatar2) {
				avatar->SetEnginePower(0, -1*factor);
			}
		} break;
		case kDirectiveLeft: {
			avatar->SetEnginePower(1, -1*factor);
		} break;
		case kDirectiveRight: {
			avatar->SetEnginePower(1, +1*factor);
		} break;
		case kDirectiveFunction: {
			switch (game_->GetComputerIndex()) {
				case -1: {
					if (avatar == avatar1) {
						avatar1->SetEnginePower(2, +1*factor);	// Break.
					} else {
						game_->Shoot();
					}
				} break;
				case 0: {
					game_->Shoot();
				} break;
				case 1: {
					avatar1->SetEnginePower(2, +1*factor);	// Break.
				} break;
			}
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
	(void)movement;
	int _tag = 0;

#ifdef LEPRA_TOUCH_LOOKANDFEEL
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
		return _tag;
	}

	float x = (float)movement.last_x_;
	float y = (float)movement.last_y_;
	float start_x = (float)movement.start_x_;
	float start_y = (float)movement.start_y_;

	const float w = (float)ui_manager_->GetCanvas()->GetWidth();
	const float h = (float)ui_manager_->GetCanvas()->GetHeight();
	std::swap(x, y);
	std::swap(start_x, start_y);
	/*const float lTapMargin = 28.0f;
	start_x = Math::Clamp(start_x, lTapMargin, w-lTapMargin);
	start_y = Math::Clamp(start_y, lTapMargin, h-lTapMargin);*/
	y = h-y;
	start_y = h-start_y;
	const float single_width = 130;
	const float double_width = 190;
	const float s = double_width / 2;
	enum Directive {
		kDirectiveNone,
		kDirectiveUpDown,
		kDirectiveLeftRight,
		kDirectiveFunction,
	};
	Directive directive = kDirectiveNone;
	bool is_right_controls = false;
	float value = 0;
#define SCALEUP(v)	(v)*2
#define CLAMPUP(v)	Math::Clamp((v)*2, -1.0f, 1.0f)
	if (game_->GetComputerIndex() != -1) {
		// Single play, both Cutie and Launcher.
		if (start_x <= double_width && start_y >= h-double_width) {	// Up/down?
			directive = kDirectiveUpDown;
			value = SCALEUP((start_y-y)/s);
		} else if (start_x >= w-double_width && start_y >= h-double_width) {	// Left/right?
			directive = kDirectiveLeftRight;
			value = SCALEUP((x-start_x)/s);
			if (game_->GetComputerIndex() == 1) {
				// 1P Cutie also gets some "rotate" moves on the steering wheel.
				float px;
				float py;
				GetSteeringWheelGuiInfo(w, h, px, py);
				if (x < px) {
					value += SCALEUP((start_y-y)/s);
				} else {
					value -= SCALEUP((start_y-y)/s);
				}
			}
		} else if (x >= w/2-s && x <= w/2+s && y >= h-double_width) {	// Bomb?
			directive = kDirectiveFunction;
		}
		if (game_->GetComputerIndex() != 1) {
			if (directive == kDirectiveUpDown) {
				value = -value;
			}
			is_right_controls = true;
		}
	} else {
		// Cutie dual play = portrait layout.
		if (start_x <= double_width && start_y <= single_width) {	// P1 up/down?
			directive = kDirectiveUpDown;
			value = SCALEUP((x-start_x)/s);
		} else if (start_x <= double_width && start_y >= h-single_width) {	// P1 left/right?
			directive = kDirectiveLeftRight;
			value = SCALEUP((y-start_y)/s);
			// 2P Cutie also gets some "rotate" moves on the steering wheel.
			float px;
			float py;
			GetSteeringWheelGuiInfo(w, h, px, py);
			if (y < py) {
				value += SCALEUP((x-start_x)/s);
			} else {
				value -= SCALEUP((x-start_x)/s);
			}
		} else if (x <= single_width && y >= h/2-s && y <= h/2+s) {	// Bomb?
			directive = kDirectiveFunction;
		}

		// Launcher in portrait mode.
		if (start_x >= w-double_width && start_y >= h-single_width) {	// P2 up/down?
			is_right_controls = true;
			directive = kDirectiveUpDown;
			value = CLAMPUP((x-start_x)/s);
		} else if (start_x >= w-double_width && start_y <= single_width) {	// P1 left/right?
			is_right_controls = true;
			directive = kDirectiveLeftRight;
			value = CLAMPUP((start_y-y)/s);
		} else if (x >= w-single_width && y >= h/2-s && y <= h/2+s) {	// Bomb?
			is_right_controls = true;
			directive = kDirectiveFunction;
		}
	}

	movement.UpdateDistance();

	{
		UiCure::CppContextObject* avatar = game_->GetP2();
		/*if (game_->IsFlipRenderSide()) {
			avatar = game_->GetP1();
		}*/
		switch (directive) {
			case kDirectiveNone: {
			} break;
			case kDirectiveUpDown: {
				if (game_->IsFlipRenderSide()) {
					value = -value;
				}
				if (game_->IsFlipRenderSide() == is_right_controls) {
					is_throttling_ = true;
					throttle_ = Math::Clamp(base_throttle_+value, -1.0f, +1.0f);
					if (!movement.is_press_) {
						base_throttle_ = throttle_;
						if (movement.moved_distance_ < 20 &&
							movement.timer_.QueryTimeDiff() < 0.3f) {	// Go to neutral if just tap/release.
							is_throttling_ = false;
							base_throttle_ = 0.0f;
							throttle_ = 0.0f;
						}
					}
					player1_last_touch_.ClearTimeDiff();
					_tag = 1;
				} else {
					game_->SetThrottle(avatar, value);
					player2_last_touch_.ClearTimeDiff();
					_tag = 3;
				}
			} break;
			case kDirectiveLeftRight: {
				if (game_->IsFlipRenderSide() == is_right_controls) {
					steering_ = Math::Clamp(base_steering_+value, -1.0f, 1.0f);
					if (!movement.is_press_) {
						base_steering_ = steering_;
						if (movement.moved_distance_ < 20 &&
							movement.timer_.QueryTimeDiff() < 0.3f) {	// Go to neutral if just tap/release.
							base_steering_ = 0.0f;
							steering_ = 0;
						}
					}
					player1_last_touch_.ClearTimeDiff();
					_tag = 2;
				} else {
					avatar->SetEnginePower(1, value);
					player2_last_touch_.ClearTimeDiff();
					_tag = 4;
				}
			} break;
			case kDirectiveFunction: {
				if (game_->IsFlipRenderSide() != is_right_controls) {
					game_->Shoot();
					player2_last_touch_.ClearTimeDiff();
					_tag = 5;
				}
			} break;
		}
	}
	movement.tag_ = _tag;
#endif // touch L&F
	return _tag;
}



void App::MainMenu() {
	// TRICKY: leave these here, since this call comes from >1 place.
	game_over_timer_.Stop();
	game_->EnableScoreCounting(false);
	game_->ResetWinnerIndex();
	game_->SetFlybyMode(Game::kFlybyPause);
	game_->SetHeartBalance(0);
	// TRICKY-END!

	uitbc::Dialog* d = CreateTbcDialog(&App::OnMainMenuAction);
	d->AddButton(1, ICONBTNA("btn_1p.png", L"Single player"), true);
	d->AddButton(2, ICONBTNA("btn_2p.png", L"Two players"), true);
	d->AddButton(3, ICONBTNA("btn_hiscore.png", L"Hiscore"), true);
	d->AddButton(4, ICONBTNA("btn_credits.png", L"Credits"), true);
}

void App::UpdateHiscore(bool error) {
	if (!dialog_) {
		return;
	}
	if (error) {
		uitbc::Label* _text = new uitbc::Label(LIGHT_RED, L"Network problem, try again l8r.");
		_text->SetVericalAlignment(uitbc::Label::kValignTop);
		dialog_->AddChild(_text, 135, 75);
		return;
	}
	str last_hiscore_name;
	v_get(last_hiscore_name, =, variable_scope_, kRtvarHiscoreName, "");
	typedef cure::HiscoreAgent::Entry HiscoreEntry;
	typedef cure::HiscoreAgent::List HiscoreList;
	const HiscoreList& hiscore_list = hiscore_agent_->GetDownloadedList();
	wstr hiscore;
	const int base_place = hiscore_list.offset_;
	const int entry_count = 10;
	const double log_exponent = ::log10((double)(base_place+entry_count)) + 1e-12;
	const int position_digits = (int)::floor(log_exponent) + 1;
	for (int x = 0; x < (int)hiscore_list.entry_list_.size() && x < entry_count; ++x) {
		const int place = x + 1 + base_place;
		const HiscoreEntry& entry = hiscore_list.entry_list_[x];
		const wstr score = wstrutil::Encode(Int2Str(entry.score_));
		wchar_t pointer = ' ';
		wchar_t pointer2 = ' ';
		if (last_hiscore_name == entry.name_) {
			pointer  = '>';
			pointer2 = '<';
		}
		const wstr format_place = wstrutil::Format(L"%i", position_digits);
		// TRICKY: ugly circumvention for string that won't vswprintf()!
		wstr __name = wstrutil::Encode(entry.name_);
		if (__name.size() < 13) {
			__name.append(13-__name.size(), ' ');
		}
		const wstr format1 = L"%c%" + format_place + L"i ";
		const wstr format2 = L" %10s%c\n";
		hiscore += wstrutil::Format(format1.c_str(), pointer, place) +
			__name +
			wstrutil::Format(format2.c_str(), score.c_str(), pointer2);
	}
	if (hiscore.empty()) {
		hiscore = L"No score entered. Yet.";
	}
	uitbc::Label* _text = new uitbc::Label(FGCOLOR_DIALOG, hiscore);
	_text->SetFontId(monospaced_font_id_);
	_text->SetVericalAlignment(uitbc::Label::kValignTop);
	const uitbc::FontManager::FontId previous_font_id = ui_manager_->GetFontManager()->GetActiveFontId();
	ui_manager_->GetFontManager()->SetActiveFont(monospaced_font_id_);
	const int char_width = ui_manager_->GetFontManager()->GetStringWidth(L" ");
	ui_manager_->GetFontManager()->SetActiveFont(previous_font_id);
	dialog_->AddChild(_text, 110 - position_digits/2 * char_width, 75);
}

void App::HiscoreMenu(int direction) {
	// Start downloading the highscore.
	CreateHiscoreAgent();
	const str level_name = kLevels[hiscore_level_index_];
	const str vehicle_name = kVehicles[hiscore_vehicle_index_];
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
	d->SetQueryLabel(wstrutil::Encode("Hiscore " + level_name + "/" + vehicle_name), big_font_id_);
	uitbc::Button* main_menu_button = ICONBTNA("btn_back.png", L"");
	uitbc::Button* prev_level_button = ICONBTN("btn_prev.png", wstrutil::Encode(GET_NAME(hiscore_level_index_-1, kLevels)));
	uitbc::Button* next_level_button = ICONBTN("btn_next.png", wstrutil::Encode(GET_NAME(hiscore_level_index_+1, kLevels)));
	uitbc::Button* prev_avatar_button = ICONBTN("btn_prev.png", wstrutil::Encode(GET_NAME(hiscore_vehicle_index_-1, kVehicles)));
	uitbc::Button* next_avatar_button = ICONBTN("btn_next.png", wstrutil::Encode(GET_NAME(hiscore_vehicle_index_+1, kVehicles)));
	main_menu_button->SetPreferredSize(d->GetPreferredWidth() / 2, d->GetPreferredHeight());
	prev_level_button->SetPreferredSize(57, 57);
	next_level_button->SetPreferredSize(prev_level_button->GetPreferredSize());
	prev_avatar_button->SetPreferredSize(prev_level_button->GetPreferredSize());
	next_avatar_button->SetPreferredSize(prev_level_button->GetPreferredSize());
	d->AddButton(-1, main_menu_button, true);
	d->AddButton(-2, prev_level_button, true);
	d->AddButton(-3, next_level_button, true);
	d->AddButton(-4, prev_avatar_button, true);
	d->AddButton(-5, next_avatar_button, true);
	main_menu_button->SetPos(d->GetPreferredWidth()/4, 0);
	prev_level_button->SetPos(20, d->GetPreferredHeight()/2 - 57-15);
	next_level_button->SetPos(d->GetPreferredWidth()-20-57, d->GetPreferredHeight()/2 - 57-15);
	prev_avatar_button->SetPos(20, d->GetPreferredHeight()/2 +15);
	next_avatar_button->SetPos(d->GetPreferredWidth()-20-57, d->GetPreferredHeight()/2 +15);
	if (!hiscore_agent_) {
		UpdateHiscore(true);
	}
}

void App::EnterHiscore(const wstr& message, const Color& color) {
	game_over_timer_.Stop();
	game_->ResetWinnerIndex();

	uitbc::Dialog* d = CreateTbcDialog(&App::OnEnterHiscoreAction);
	d->SetPreClickTarget(uitbc::Dialog::Action(this, &App::OnPreEnterAction));
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel(wstrutil::Encode("Enter hiscore name ("+Int2Str((int)game_->GetScore())+" points)"), big_font_id_);
	if (!message.empty()) {
		uitbc::Label* _message = new uitbc::Label(color, message);
		const int string_width = ui_manager_->GetPainter()->GetStringWidth(message);
		d->AddChild(_message, d->GetSize().x/2 - string_width/2, 80);
	}
	hiscore_text_field_ = new HiscoreTextField(d, uitbc::TextField::kBorderSunken, 2, WHITE);
	hiscore_text_field_->app_ = this;
	hiscore_text_field_->SetText(wstrutil::Encode(v_slowget(variable_scope_, kRtvarHiscoreName, "")));
	hiscore_text_field_->SetPreferredSize(205, 25, false);
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	d->AddChild(hiscore_text_field_, 70, 97);
#else // Computer
	d->AddChild(hiscore_text_field_, 70, 130);
#endif // touch / computer
	hiscore_text_field_->SetKeyboardFocus();	// TRICKY: focus after adding.
	uitbc::Button* cancel_button = new uitbc::Button(L"");
	Color c = Color(180, 50, 40);
	cancel_button->SetBaseColor(c);
	cancel_button->SetText(L"Cancel", FGCOLOR_DIALOG, CLEAR_COLOR);
	cancel_button->SetRoundedRadius(8);
	cancel_button->SetPreferredSize(300-hiscore_text_field_->GetPreferredWidth()-8, hiscore_text_field_->GetPreferredHeight()+1);
	d->AddButton(-1, cancel_button, true);
	cancel_button->SetPos(hiscore_text_field_->GetPos().x+hiscore_text_field_->GetPreferredWidth()+8, hiscore_text_field_->GetPos().y);
}

void App::SuperReset(bool game_over) {
	game_over_timer_.Stop();

	player1_last_touch_.PopTimeDiff();
	player2_last_touch_.PopTimeDiff();

	is_throttling_ = false;
	throttle_ = 0;
	base_throttle_ = 0;
	steering_ = 0;
	base_steering_ = 0;
	game_->SyncCameraPositions();
	game_->EndSlowmo();
	if (game_over) {
		// Total game over (someone won the match)?
		const int heart_balance = game_->GetHeartBalance();
		if (heart_balance == -kHeartPoints/2 || heart_balance == +kHeartPoints/2) {
			game_->SetComputerIndex(game_->GetComputerIndex());	// Force flip side reset.
			game_->ResetScore();
			MainMenu();
			return;
		}

		// Nope, simply reload the interior. Go another round.
		const int _computer_index = game_->GetComputerIndex();
		switch (_computer_index) {
			case -1:	game_->FlipRenderSides();								break;
			case 0:		game_->SetComputerIndex(1);	game_->SetHeartBalance(-game_->GetHeartBalance());	break;
			case 1:		game_->SetComputerIndex(0);	game_->SetHeartBalance(-game_->GetHeartBalance());	break;
		}
		game_->NextRound();
	} else {
		// Restart level.
		game_->SetHeartBalance(0);
		if (game_->IsFlipRenderSide()) {
			game_->FlipRenderSides();
		}
		game_->SetComputerIndex(game_->GetComputerIndex());
		game_->ResetScore();
	}
	game_->ResetWinnerIndex();
	game_->SetVehicle(game_->GetVehicle());
	game_->ResetLauncher();
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
	hiscore_agent_ = new cure::HiscoreAgent(host, 80, "kill_cutie");
	//hiscore_agent_ = new cure::HiscoreAgent("localhost", 8080, "kill_cutie");
}

void App::Purchase(const str& product_name) {
#ifdef LEPRA_IOS
	[animated_app_ startPurchase:MacLog::Encode(product_name)];
#else // !iOS
	(void)product_name;
	button_delegate_ = uitbc::Dialog::Action(this, &App::OnOk);
	uitbc::MessageDialog* _message = new uitbc::MessageDialog(ui_manager_->GetDesktopWindow(), button_delegate_,
		L"Content purchase not yet implemented on this platform.");
	_message->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	_message->AddButton(+10, L"OK", true);
	_message->AddButton(+33, L"Nooo!", true);
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
		SystemManager::WebBrowseTo("http://pixeldoctrine.com/#KillCutie");
		//SystemManager::EmailTo(
		//	"info@pixeldoctrine.com",
		//	"I want the complete game!",
		//	"Hiya Game Slave Bitches,\n\n"
		//	"I enjoyed the KillCutie Demo [for PC/Mac?] and would like the complete game!\n\n"
		//	"Get a move on,\n"
		//	"Yours Truly");
	}
	MainMenu();
}

void App::OnMainMenuAction(uitbc::Button* button) {
	KillIntroVoice();
	const int previous_computer_index = game_->GetComputerIndex();
	switch (button->GetTag()) {
		case 1: {
			// 1P
			const int _computer_index = ((Random::GetRandomNumber() % 256) >= 128);
			game_->SetComputerIndex(_computer_index);
		} break;
		case 2: {
			// 2P
			game_->SetComputerIndex(-1);
#ifdef LEPRA_TOUCH
			// Dual play requires original canvas orientation.
			ui_manager_->GetCanvas()->SetOutputRotation(0);
#endif // touch
		} break;
		case 3: {
			HiscoreMenu(+1);
			//EnterHiscore("Press enter when you're done", FGCOLOR_DIALOG);
		}
		return;
		case 4: {
			uitbc::Dialog* d = CreateTbcDialog(&App::OnCreditsAction);
			d->SetOffset(PixelCoord(0, -30));
			d->SetQueryLabel(L"Credits", big_font_id_);
			wstr s =	L"Game    Pixel Doctrine\n"
					L"Music   Jonas Kapla\n"
					L"Thanks  ODE, ChibiXM, Ogg/Vorbis, OpenAL, ALUT, GAE,\n"
					L"        libpng, Minizip, zlib, FastDelegate, UTF-8 CPP,\n"
					L"        DMI, freesound, HappyHTTP, Python, py-cgkit\n"
					L"\n"
					L"Idiots kill civilians for real. Visit Avaaz.org if you\n"
					L"too belive media attention eventually can crush tyrants.";
			uitbc::Label* _text = new uitbc::Label(FGCOLOR_DIALOG, s);
			_text->SetFontId(monospaced_font_id_);
			_text->SetVericalAlignment(uitbc::Label::kValignTop);
			d->AddChild(_text, 25, 85);
			uitbc::Button* back_button = new uitbc::CustomButton(L"");
			back_button->SetPreferredSize(d->GetPreferredSize());
			d->AddButton(-1, back_button, true);
			back_button->SetPos(0, 0);
		}
		return;
	}
	// Switched from single play to dual play or vice versa?
	if ((previous_computer_index == -1) != (game_->GetComputerIndex() == -1)) {
		start_timer_.Stop();	// Show keys again on the computer screen.
	}
	uitbc::Dialog* d = CreateTbcDialog(&App::OnLevelAction);
	d->SetQueryLabel(L"Select level", big_font_id_);
	d->AddButton(1, ICONBTN("btn_tutorial.png", L"Tutorial"), true);
	d->AddButton(2, ICONBTN("btn_lvl2.png", wstrutil::Encode(kLevels[0])), true);
	d->AddButton(3, ICONBTN("btn_lvl3.png", wstrutil::Encode(kLevels[1])), true);
	d->AddButton(4, ICONBTN("btn_lvl4.png", wstrutil::Encode(kLevels[2])), true);
	if (is_money_icon_added_ && !v_slowget(variable_scope_, kRtvarContentLevels, false)) {
		AddCostIcon(kLevels[1]);
		AddCostIcon(kLevels[2]);
	}
}

void App::OnEnterHiscoreAction(uitbc::Button* button) {
	if (!button) {
		str last_hiscore_name = strutil::Strip(strutil::Encode(hiscore_text_field_->GetText()), " \t\v\r\n");
		hiscore_text_field_ = 0;
		if (!last_hiscore_name.empty()) {
			v_set(variable_scope_, kRtvarHiscoreName, last_hiscore_name);
#ifdef LEPRA_IOS
			[AnimatedApp hiscore_name_];
#endif // iOS
			const str level_name = kLevels[hiscore_level_index_];
			const str vehicle_name = kVehicles[hiscore_vehicle_index_];
			CreateHiscoreAgent();
			if (!hiscore_agent_->StartUploadingScore(kPlatform, level_name, vehicle_name, last_hiscore_name, (int)Math::Round(game_->GetScore()))) {
				delete hiscore_agent_;
				hiscore_agent_ = 0;
				MainMenu();
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
		game_->ResetWinnerIndex();
		game_->SetVehicle("cutie");
		game_->ResetLauncher();
		game_->SetComputerDifficulty(0);
		delete intro_streamer_;
		intro_streamer_ = ui_manager_->GetSoundManager()->CreateSoundStream("voice_intro.ogg", uilepra::SoundManager::kLoopNone, 0);
		if (intro_streamer_ && intro_streamer_->Playback()) {
			SuperReset(false);
			if (music_player_) {
				music_player_->SetVolume(0.2f);
			}
			return;
		}
		delete intro_streamer_;
		intro_streamer_ = 0;
		log_.Error("Oops! Unable to play tutorial voice-over. Skipping tutorial.");
		// If all else fails, fall thru add keep going. Makes more sense to the user than
		// a tutorial without any instructions.
	}
	uitbc::Dialog* d = CreateTbcDialog(&App::OnVehicleAction);
	d->SetQueryLabel(L"Select vehicle", big_font_id_);
	d->AddButton(1, ICONBTN("btn_cutie.png", wstrutil::Encode(kVehicles[0])), true);
	d->AddButton(2, ICONBTN("btn_hardie.png", wstrutil::Encode(kVehicles[1])), true);
	d->AddButton(3, ICONBTN("btn_speedie.png", wstrutil::Encode(kVehicles[2])), true);
	d->AddButton(4, ICONBTN("btn_sleepie.png", wstrutil::Encode(kVehicles[3])), true);
	if (is_money_icon_added_ && !v_slowget(variable_scope_, kRtvarContentVehicles, false)) {
		AddCostIcon(kVehicles[1]);
		AddCostIcon(kVehicles[2]);
		AddCostIcon(kVehicles[3]);
	}
	if (game_->GetComputerIndex() != -1) {
		d->SetOffset(PixelCoord(0, -40));

		if (!scroll_bar_image_) {
			scroll_bar_image_ = new UiCure::PainterImageResource(ui_manager_, resource_manager_,
				"scrollbar.png", UiCure::PainterImageResource::kReleaseFreeBuffer);
			scroll_bar_image_->Load();
			scroll_bar_image_->PostProcess();
			ui_manager_->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*scroll_bar_image_->GetRamData(),
				scroll_bar_image_->GetUserData(0), uitbc::GUIImageManager::kCentered,
				uitbc::GUIImageManager::kAlphablend, 255);
		}
		uitbc::Button* scroll_button = ICONBTNA("btn_scroll.png", L"");
		scroll_button->SetPreferredSize(PixelCoord(57, 57), false);
		difficulty_slider_ = new uitbc::ScrollBar(uitbc::ScrollBar::kHorizontal,
			scroll_bar_image_->GetUserData(0), 0, 0, scroll_button);
		difficulty_slider_->SetScrollRatio(44, scroll_bar_image_->GetRamData()->GetWidth());
		difficulty_slider_->SetScrollPos((game_->GetComputerDifficulty() < 0)? 0.0 : game_->GetComputerDifficulty());
		difficulty_slider_->SetPreferredSize(scroll_bar_image_->GetRamData()->GetWidth()+15*2, 44);
		d->AddChild(difficulty_slider_);
		const int x = d->GetPreferredWidth()/2 - difficulty_slider_->GetPreferredWidth()/2;
		const int y = d->GetPreferredHeight() - 60;
		difficulty_slider_->SetPos(x, y);

		difficulty_label_ = new uitbc::Label(FGCOLOR_DIALOG, L"");
		d->AddChild(difficulty_label_);
		difficulty_label_->SetPos(x+15, y-3);

		d->UpdateLayout();
	}

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

void App::OnVehicleAction(uitbc::Button* button) {
	difficulty_label_ = 0;
	difficulty_slider_ = 0;

	if (button->GetTag() >= 2 && !v_slowget(variable_scope_, kRtvarContentVehicles, false)) {
		Purchase(kContentVehicles);
		return;
	}

	game_->EnableScoreCounting(true);
	str vehicle = "cutie";
	switch (button->GetTag()) {
		case 1:	vehicle = "cutie";		hiscore_vehicle_index_ = 0;	break;
		case 2:	vehicle = "monster";	hiscore_vehicle_index_ = 1;	break;
		case 3:	vehicle = "corvette";	hiscore_vehicle_index_ = 2;	break;
		case 4:	vehicle = "road_roller";	hiscore_vehicle_index_ = 3;	break;
	}
	SuperReset(false);
	game_->ResetWinnerIndex();
	game_->SetVehicle(vehicle);
	game_->ResetLauncher();
	game_->SetFlybyMode(Game::kFlybyInactive);
	is_paused_ = false;
	playback_voice_instruction_ = true;
}

void App::OnHiscoreAction(uitbc::Button* button) {
	delete hiscore_agent_;
	hiscore_agent_ = 0;
	switch (button->GetTag()) {
		case -1: {
			MainMenu();
		} break;
		case -2: {
			hiscore_level_index_ = GET_NAME_INDEX(hiscore_level_index_-1, kLevels);
			HiscoreMenu(-1);
		} break;
		case -3: {
			hiscore_level_index_ = GET_NAME_INDEX(hiscore_level_index_+1, kLevels);
			HiscoreMenu(+1);
		} break;
		case -4: {
			hiscore_vehicle_index_ = GET_NAME_INDEX(hiscore_vehicle_index_-1, kVehicles);
			HiscoreMenu(-1);
		} break;
		case -5: {
			hiscore_vehicle_index_ = GET_NAME_INDEX(hiscore_vehicle_index_+1, kVehicles);
			HiscoreMenu(+1);
		} break;
	}
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
	const bool is_game_over = (game_->GetHeartBalance() == -kHeartPoints/2 || game_->GetHeartBalance() == +kHeartPoints/2);
	if (is_paused_ || dialog_ || (game_->GetWinnerIndex() >= 0 && is_game_over)) {
		return;
	}
	is_paused_ = true;
	if (intro_streamer_) {
		intro_streamer_->Pause();
	}
	uitbc::Dialog* d = CreateTbcDialog(&App::OnPauseAction);
	d->AddButton(1, ICONBTNA("btn_resume.png", L"Resume"), true);
	if (game_->GetFlybyMode() == Game::kFlybyInactive) {	// Restart not available in tutorial mode.
		d->AddButton(2, ICONBTNA("btn_restart.png", L"Restart"), true);
	}
	d->AddButton(3, ICONBTNA("btn_main_menu.png", L"Main menu"), true);
}

void App::OnPauseClickWithSound(uitbc::Button* button) {
	if (is_paused_ || dialog_) {
		return;
	}
	OnTapSound(button);
	DoPause();
}

void App::OnPauseAction(uitbc::Button* button) {
	player1_last_touch_.PopTimeDiff();
	player2_last_touch_.PopTimeDiff();
	player1_touch_delay_.PopTimeDiff();
	player1_touch_delay_.ReduceTimeDiff(-10);
	player2_touch_delay_.PopTimeDiff();
	player2_touch_delay_.ReduceTimeDiff(-10);

	is_paused_ = false;
	if (intro_streamer_ && !intro_streamer_->IsPlaying()) {
		intro_streamer_->Playback();
	}
	if (button->GetTag() == 2) {
		SuperReset(false);
	} else if (button->GetTag() == 3) {
		MainMenu();
	}
}

void App::OnGetiPhoneClick(uitbc::Button*) {
	SystemManager::WebBrowseTo("http://itunes.apple.com/us/app/kill-cutie/id500583207?ls=1&mt=8");
	delete geti_phone_button_;
	geti_phone_button_ = 0;
	do_layout_ = true;
}

void App::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource) {
	(void)resource;
}

void App::SoundLoadCallback(UiCure::UserSound2dResource* resource) {
	(void)resource;
}

void App::KillIntroVoice() {
	delete intro_streamer_;
	intro_streamer_ = 0;
	if (music_player_) {
		music_player_->SetVolume(0.5f);
	}
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

void App::AddCostIcon(const str& _name) {
	uitbc::Label* label = new uitbc::Label(FGCOLOR_DIALOG, L"");
	label->SetIcon(money_->GetData(), uitbc::Label::kIconCenter);
	//label->SetAdaptive(true);
	uitbc::Button* _button = (uitbc::Button*)dialog_->GetChild(_name);
	_button->GetClientRectComponent()->ReplaceLayer(0, new uitbc::FloatingLayout);
	_button->AddChild(label, 57-14, 57-9);
	dialog_->UpdateLayout();
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
