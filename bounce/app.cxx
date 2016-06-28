
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
#include "../uitbc/include/gui/uitextarea.h"
#include "../uitbc/include/gui/uitextfield.h"
#include "../uitbc/include/uifontmanager.h"
#include "ball.h"
#include "game.h"
#ifdef LEPRA_MAC
#include "../lepra/include/posix/maclog.h"
#endif // iOS



namespace bounce {



#ifdef LEPRA_TOUCH_LOOKANDFEEL
const str kPlatform = "touch";
#else // Computer.
const str kPlatform = "computer";
#endif // touch / computer.
const str kLevelName = "nothing";
const str kAvatarName = "no-one";
#define BGCOLOR_DIALOG		Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG		Color(255, 255, 255, 255)
#define kRtvarHiscoreName	"Hiscore.Name"  // Last entered name.
#define ICONBTN(i,n)		new UiCure::IconButton(ui_manager_, resource_manager_, i, n)
#define ICONBTNA(i,n)		ICONBTN(i, n)



FingerMoveList g_finger_move_list;



class HiscoreTextField;

class App: public Application {
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

	void SetRacketForce(float lift_factor, const vec3& down);
	virtual void Suspend();
	virtual void Resume();

#if !defined(LEPRA_TOUCH)
	void OnMouseInput(uilepra::InputElement* element);
	bool is_mouse_down_;
#endif // Computer emulating touch device
	virtual int PollTap(FingerMovement& movement);

	typedef void (App::*ButtonAction)(uitbc::Button*);
	void Reset(bool reset_score);
	void MainMenu(bool is_reusume);
	void MainMenu();
	void HiscoreMenu();
	void NumberDialog();
	void EnterHiscore(const wstr& message, const Color& color);
	void CreateHiscoreAgent();
	void UpdateHiscore(bool error);
	void OnAction(uitbc::Button* button);
	void OnTapSound(uitbc::Button* button);
	void OnMainMenuAction(uitbc::Button* button);
	void OnHiscoreMenuAction(uitbc::Button* button);
	void OnWhatsThisClick(uitbc::Button* button);
	void OnGoToMainMenuDialogAction(uitbc::Button* button);
	void OnEnterHiscoreAction(uitbc::Button* button);
	void RearrangeHiScore();
	int GetExecutionCount() const;
	void PrintText(const wstr& text, float angle, int center_x, int center_y) const;
	uitbc::Button* CreateButton(const wstr& text, const Color& color);
	uitbc::Dialog* CreateTbcDialog(ButtonAction action);

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource);
	void SoundLoadCallback(UiCure::UserSound2dResource* resource);

	static wstr Int2Str(int number);

	StdioConsoleLogListener console_logger_;
	DebuggerLogListener debug_logger_;

	static App* app_;
#ifdef LEPRA_TOUCH
	AnimatedApp* animated_app_;
#endif // touch
	Game* game_;

	double average_loop_time_;
	HiResTimer loop_timer_;

	bool is_loaded_;
	cure::ResourceManager* resource_manager_;
	cure::RuntimeVariableScope* variable_scope_;
	UiCure::GameUiManager* ui_manager_;
	UiCure::MusicPlayer* music_player_;
	bool is_pressing_;
	HiscoreTextField* hiscore_text_field_;
	cure::HiscoreAgent* hiscore_agent_;
	int my_hiscore_index_;
	int frame_counter_;
	UiCure::UserSound2dResource* tap_click_;
	uitbc::Dialog::Action button_delegate_;
	uitbc::Dialog* dialog_;
	uitbc::FontManager::FontId big_font_id_;
	uitbc::FontManager::FontId monospaced_font_id_;
	UiCure::UserPainterKeepImageResource* backdrop_;
	bool is_resume_;

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



}



LEPRA_RUN_APPLICATION(bounce::App, uilepra::UiMain);



namespace bounce {



App::App(const strutil::strvec& argument_list):
	Application(argument_list),
	game_(0),
	variable_scope_(0),
	average_loop_time_(1.0/(kFps+1)),
	is_loaded_(false),
	is_pressing_(false),
	hiscore_text_field_(0),
	hiscore_agent_(0),
	my_hiscore_index_(0),
	frame_counter_(0),
	dialog_(0),
	big_font_id_(uitbc::FontManager::kInvalidFontid),
	monospaced_font_id_(uitbc::FontManager::kInvalidFontid),
	backdrop_(0),
	is_resume_(false) {
	app_ = this;
}

App::~App() {
	dialog_ = 0;
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
	const int display_width = __size.width;
	const int display_height = __size.height;
	bool display_full_screen = true;
#else // !touch
	const int display_width = 320;
	const int display_height = 480;
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
	v_set(variable_scope_, kRtvarUiDisplayOrientation, "Fixed");

	v_set(variable_scope_, kRtvarUiSoundEngine, "OpenAL");

	v_set(variable_scope_, kRtvarUiDisplayEnablevsync, false);
	v_set(variable_scope_, kRtvarUi3DEnableclear, false);
	v_set(variable_scope_, kRtvarUi3DPixelshaders, false);
	v_set(variable_scope_, kRtvarUi3DEnablelights, true);
	v_set(variable_scope_, kRtvarUi3DEnabletrilinearfiltering, false);
	v_set(variable_scope_, kRtvarUi3DEnablebilinearfiltering, false);
	v_set(variable_scope_, kRtvarUi3DEnablemipmapping, false);
	v_set(variable_scope_, kRtvarUi3DFov, 60.0);
	v_set(variable_scope_, kRtvarUi3DClipnear, 0.01);
	v_set(variable_scope_, kRtvarUi3DClipfar, 2.0);
	v_set(variable_scope_, kRtvarUi3DShadows, "None");
	v_set(variable_scope_, kRtvarUi3DAmbientred, 0.5);
	v_set(variable_scope_, kRtvarUi3DAmbientgreen, 0.5);
	v_set(variable_scope_, kRtvarUi3DAmbientblue, 0.5);
	v_set(variable_scope_, kRtvarUi2DFontheight, 18.0);
	v_set(variable_scope_, kRtvarUiSoundRolloff, 0.7);
	v_set(variable_scope_, kRtvarUiSoundDoppler, 1.0);

	ui_manager_ = new UiCure::GameUiManager(variable_scope_, 0);
	bool ok = ui_manager_->OpenDraw();
	if (ok) {
		uilepra::Core::ProcessMessages();
		ui_manager_->GetPainter()->ResetClippingRect();
		ui_manager_->GetPainter()->Clear(BLACK);
		DisplayLogo();
	}
	if (ok) {
#ifdef LEPRA_TOUCH
		ui_manager_->GetCanvas()->SetOutputRotation(0);
#endif // touch
		ok = ui_manager_->OpenRest();
	}
	if (ok) {
		ui_manager_->GetDisplayManager()->SetCaption("Bounce");
#if !defined(LEPRA_TOUCH)
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
		music_player_ = new UiCure::MusicPlayer(ui_manager_->GetSoundManager());
		music_player_->SetVolume(0.5f);
		music_player_->SetSongPauseTime(9, 15);
		/*music_player_->AddSong("ButterflyRide.xm");
		music_player_->AddSong("BehindTheFace.xm");
		music_player_->AddSong("BrittiskBensin.xm");
		music_player_->AddSong("DontYouWantMe'97.xm");
		music_player_->AddSong("CloseEncounters.xm");*/
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
		tap_click_ = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
		tap_click_->Load(resource_manager_, "tap.wav",
			UiCure::UserSound2dResource::TypeLoadCallback(this, &App::SoundLoadCallback));
	}
	if (ok) {
		backdrop_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseNone);
		backdrop_->Load(resource_manager_, "backdrop.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
	}
	if (ok) {
		MainMenu(false);
	}

	uilepra::Core::ProcessMessages();
	return (ok);
}

void App::Close() {
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
}


int App::Run() {
	uilepra::Init();
	uitbc::Init();
	UiCure::Init();
	Network::Start();

	const str log_name = Path::JoinPath(SystemManager::GetIoDirectory("Bounce"), "log", "txt");
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
		v_set(variable_scope_, kRtvarPhysicsFastalgo, true);
		v_set(variable_scope_, kRtvarPhysicsParallel, false);	// Let's do it same on all platforms, so we can render stuff from physics data.
		v_set(variable_scope_, kRtvarPhysicsMicrosteps, 3);
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
	}
	if (ok) {
		game_->cure::GameTicker::GetTimeManager()->Tick();
		game_->cure::GameTicker::GetTimeManager()->Clear(1);

		ok = resource_manager_->InitDefault();
	}
	if (ok) {
		loop_timer_.EnableShadowCounter(true);
	}
	if (ok) {
		game_->cure::GameManager::GetPhysicsManager()->SetGravity(vec3(0, 0, -9.82f));
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
		const double instant_loop_time = loop_timer_.QueryTimeDiff();
		if (++frame_counter_ > 2) {
			// Adjust frame rate, or it will be hopelessly high... on most reasonable platforms.
			average_loop_time_ = lepra::Math::Lerp(average_loop_time_, instant_loop_time, 0.05);
		}
		const double delay_time = 1.0/kFps - average_loop_time_;
		if (delay_time > 0) {
			Thread::Sleep(delay_time-0.001);
			uilepra::Core::ProcessMessages();
		}
		HiResTimer::StepCounterShadow();	// TRICKY: after sleep we must manually step the counter shadow.
		loop_timer_.PopTimeDiff();

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
							HiscoreMenu();
						} else {
							EnterHiscore(L"Please retry; score server obstipated", LIGHT_RED);
						}
					} break;
					default: {
						delete hiscore_agent_;
						hiscore_agent_ = 0;
						log_.Error("Oops! Completed hiscore communication, but something went wrong.");
						deb_assert(false);
						MainMenu(false);	// Well... assume some super-shitty state...
					} break;
				}
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
	if (ok) {
		if (backdrop_->GetLoadState() == cure::kResourceLoadComplete) {
			if (backdrop_->GetRamData()->GetBuffer() && game_->GetScore() > 1600) {
				RearrangeHiScore();
				backdrop_->GetRamData()->SetBuffer(0);
			}

			ui_manager_->PreparePaint(true);
			const Canvas* canvas = ui_manager_->GetCanvas();
			ui_manager_->GetPainter()->DrawImage(backdrop_->GetData(), PixelRect(0, 0, canvas->GetWidth(), canvas->GetHeight()));
		}
	}
	if (ok) {
		ui_manager_->InputTick();
		PollTaps();
	}
	bool is_paused = (dialog_ != 0 || hiscore_agent_ != 0);
	if (ok && !is_paused) {
		if (!game_->MoveRacket()) {
			if (game_->GetScore() >= 1000) {
				EnterHiscore(L"Enter your name for the hiscore list", LIGHT_GRAY);
			} else {
				MainMenu(false);
			}
		}
	}
	if (ok && !is_paused) {
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
		ui_manager_->Paint(false);
		game_->Paint();
		ui_manager_->GetPainter()->SetColor(OFF_BLACK, 0);

		if (!is_paused) {
			const wstr score = L"Score: " + Int2Str((int)Math::Round(game_->GetScore()));
			PrintText(score, 0, 160, ui_manager_->GetCanvas()->GetHeight() - 20);
		}

		if (hiscore_agent_) {
			const wstr info = L"Speaking to score server";
			PrintText(info, 0,
				ui_manager_->GetCanvas()->GetWidth()/2,
				ui_manager_->GetCanvas()->GetHeight() - ui_manager_->GetPainter()->GetFontHeight());
		}
	}
	if (ok && !is_paused) {
		game_->PreEndTick();
		game_->WaitPhysicsTick();
		ok = game_->EndTick();
	}
	if (ok && !is_paused) {
		ok = game_->Tick();
	}
	resource_manager_->Tick();
	ui_manager_->EndRender(1.0f/kFps);

	if (music_player_) {
		music_player_->Update();
	}

	return ok;
}

void App::PollTaps() {
	is_pressing_ = true;
	FingerMoveList::iterator x = g_finger_move_list.begin();
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

	static float last_x = ui_manager_->GetInputManager()->GetCursorX();
	static float last_y = ui_manager_->GetInputManager()->GetCursorY();
	const float dx = ui_manager_->GetInputManager()->GetCursorX() - last_x;
	const float dy = ui_manager_->GetInputManager()->GetCursorY() - last_y;
	last_x += dx;
	last_y += dy;
	vec3 gravity(dx*6, 0, dy*6 - 1);
	SetRacketForce(gravity.GetLength() - 1, gravity);

	if (ui_manager_->GetInputManager()->GetMouse()->GetButton(1)->GetBooleanValue()) {
		tbc::ChunkyPhysics* structure = game_->GetBall()->GetPhysics();
		const int bone_count = structure->GetBoneCount();
		for (int y = 0; y < bone_count; ++y) {
			tbc::PhysicsManager::BodyID body_id = structure->GetBoneGeometry(y)->GetBodyId();
			if (body_id != tbc::INVALID_BODY) {
				game_->cure::GameManager::GetPhysicsManager()->AddForce(body_id, vec3(1, 0, 0));
			}
		}
	}
#endif // Computer
}



void App::SetRacketForce(float lift_factor, const vec3& down) {
	if (game_) {
		game_->SetRacketForce(lift_factor, down);
	}
}

void App::Suspend() {
	MainMenu(true);
	if (music_player_) {
		music_player_->Pause();
	}
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


#if !defined(LEPRA_TOUCH)
void App::OnMouseInput(uilepra::InputElement* element) {
	if (element->GetType() == uilepra::InputElement::kDigital) {
		is_mouse_down_ = element->GetBooleanValue();
	}
	if (is_mouse_down_) {
		if (g_finger_move_list.empty()) {
			g_finger_move_list.push_back(FingerMovement(0, 0));
		}
	} else {
		FingerMovement& _movement = g_finger_move_list.back();
		_movement.is_press_ = false;
	}
}
#endif // Computer emulating touch device.

int App::PollTap(FingerMovement& movement) {
#ifdef LEPRA_TOUCH
	const int screen_width = ui_manager_->GetCanvas()->GetWidth();
        ui_manager_->GetInputManager()->SetMousePosition(screen_width - movement.last_x_, movement.last_y_);
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

	is_pressing_ |= movement.is_press_;
	return movement.is_press_? 1 : -1;
}



void App::Reset(bool reset_score) {
	if (reset_score) {
		game_->ResetScore();
	}
	game_->SetRacketForce(0, vec3(0,0,-1));
}

void App::MainMenu(bool is_resume) {
	if (!dialog_) {
		is_resume_ = is_resume;
		MainMenu();
	}
}

void App::MainMenu() {
	uitbc::Dialog* d = CreateTbcDialog(&App::OnMainMenuAction);
	if (!d) {
		return;
	}
	const wstr play_text = is_resume_? L"Resume" : L"Play";
	d->AddButton(1, CreateButton(play_text, Color(40, 210, 40)), true);
	d->AddButton(2, CreateButton(L"High score", Color(50, 90, 210)), true);
	d->AddButton(3, CreateButton(L"A number", Color(210, 50, 40)), true);
}

void App::HiscoreMenu() {
	// Start downloading the highscore.
	CreateHiscoreAgent();
	const int offset = std::max(0, my_hiscore_index_-5);
	if (!hiscore_agent_->StartDownloadingList(kPlatform, kLevelName, kAvatarName, offset, 10)) {
		delete hiscore_agent_;
		hiscore_agent_ = 0;
	}
	my_hiscore_index_ = 0;

	uitbc::Dialog* d = CreateTbcDialog(&App::OnHiscoreMenuAction);
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel(L"High Score List", big_font_id_);
	uitbc::Button* main_menu_button = ICONBTNA("btn_back.png", L"");
	main_menu_button->SetPreferredSize(d->GetPreferredSize());
	d->AddButton(-1, main_menu_button, true);
	main_menu_button->SetPos(0, 0);
	if (!hiscore_agent_) {
		UpdateHiscore(true);
	}
}

void App::NumberDialog() {
	uitbc::Dialog* d = CreateTbcDialog(&App::OnGoToMainMenuDialogAction);
	d->SetOffset(PixelCoord(0, -30));
	const wstr execution_count = Int2Str(GetExecutionCount());
	d->SetQueryLabel(execution_count, big_font_id_);
	uitbc::Button* whats_this = CreateButton(L"What's this?", Color(210, 50, 40));
	d->AddButton(1, whats_this, true);
	whats_this->SetOnClick(App, OnWhatsThisClick);
	d->AddButton(2, CreateButton(L"Main menu", Color(40, 210, 40)), true);
}

void App::EnterHiscore(const wstr& message, const Color& color) {
	uitbc::Dialog* d = CreateTbcDialog(&App::OnEnterHiscoreAction);
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel(L"Wow - "+Int2Str((int)game_->GetScore())+L" points", big_font_id_);
	if (!message.empty()) {
		uitbc::Label* _message = new uitbc::Label(color, message);
		const int string_width = ui_manager_->GetPainter()->GetStringWidth(message);
		d->AddChild(_message, d->GetSize().x/2 - string_width/2, 90);
	}
	hiscore_text_field_ = new HiscoreTextField(d, uitbc::TextField::kBorderSunken, 2, WHITE);
	hiscore_text_field_->app_ = this;
	hiscore_text_field_->SetText(wstrutil::Encode(v_slowget(variable_scope_, kRtvarHiscoreName, "")));
	hiscore_text_field_->SetPreferredSize(140, 25, false);
	d->AddChild(hiscore_text_field_, 28, 107);
	hiscore_text_field_->SetKeyboardFocus();	// TRICKY: focus after adding.
	uitbc::Button* cancel_button = new uitbc::Button(L"cancel");
	Color c = Color(210, 50, 40);
	cancel_button->SetBaseColor(c);
	cancel_button->SetText(L"Cancel", FGCOLOR_DIALOG, CLEAR_COLOR);
	cancel_button->SetRoundedRadius(8);
	cancel_button->SetPreferredSize(d->GetSize().x - hiscore_text_field_->GetPos().x*2 - hiscore_text_field_->GetPreferredWidth()-8, hiscore_text_field_->GetPreferredHeight()+1);
	d->AddButton(-1, cancel_button, true);
	cancel_button->SetPos(hiscore_text_field_->GetPos().x+hiscore_text_field_->GetPreferredWidth()+8, hiscore_text_field_->GetPos().y);
}

void App::CreateHiscoreAgent() {
	delete hiscore_agent_;
	const str host = _O("7y=196h5+;/,9p.5&92r:/;*(,509p;/1", "gamehiscore.pixeldoctrine.com");
	hiscore_agent_ = new cure::HiscoreAgent(host, 80, "bounce_master");
	//hiscore_agent_ = new cure::HiscoreAgent("localhost", 8080, "bounce_master");
}

void App::UpdateHiscore(bool error) {
	if (!dialog_) {
		return;
	}
	if (error) {
		uitbc::Label* _text = new uitbc::Label(LIGHT_RED, L"Network problem, try again l8r.");
		_text->SetVericalAlignment(uitbc::Label::kValignTop);
		dialog_->AddChild(_text, 45, 30);
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
		const wstr score = Int2Str(entry.score_);
		wchar_t pointer = ' ';
		wchar_t pointer2 = ' ';
		if (last_hiscore_name == entry.name_) {
			pointer  = '>';
			pointer2 = '<';
		}
		const wstr format_place = wstrutil::Format(L"%i", position_digits);
		// TRICKY: ugly circumvention for string that won't vswprintf()!
		wstr _name = wstrutil::Encode(entry.name_);
		if (_name.size() < 13) {
			_name.append(13-_name.size(), ' ');
		}
		const wstr format1 = L"%c%" + format_place + L"i ";
		const wstr format2 = L" %10s%c\n";
		hiscore += wstrutil::Format(format1.c_str(), pointer, place) +
			_name +
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
	dialog_->AddChild(_text, 50 - position_digits/2 * char_width, 95);
}

void App::OnAction(uitbc::Button* button) {
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

void App::OnMainMenuAction(uitbc::Button* button) {
	dialog_ = 0;
	switch (button->GetTag()) {
		case 1: {
			Reset(!is_resume_);
		} break;
		case 2: {
			HiscoreMenu();
		} break;
		case 3: {
			NumberDialog();
		} break;
	}
}

void App::OnHiscoreMenuAction(uitbc::Button* /*button*/) {
	dialog_ = 0;
	MainMenu();
}

void App::OnWhatsThisClick(uitbc::Button* button) {
	const int y = button->GetPos().y - 5;
	dialog_->RemoveChild(button, 0);
	uitbc::TextArea* label = new uitbc::TextArea(CLEAR_COLOR);
	label->SetPreferredSize(280, 60);
	label->SetFontColor(LIGHT_GRAY);
	label->AddText(L"This is the number of people executed\nin a certain ping-pong nation since you\ninstalled this app.");
	dialog_->AddChild(label);
	label->SetPos(20, y);
}

void App::OnGoToMainMenuDialogAction(uitbc::Button*) {
	dialog_ = 0;
	MainMenu();
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
			CreateHiscoreAgent();
			if (!hiscore_agent_->StartUploadingScore(kPlatform, kLevelName, kAvatarName, last_hiscore_name, (int)Math::Round(game_->GetScore()))) {
				delete hiscore_agent_;
				hiscore_agent_ = 0;
			}
		} else {
			MainMenu();
		}
	} else {
		dialog_ = 0;
		MainMenu();
	}
}

struct MyHash {
public:
        size_t operator() (const PixelCoord& __r) const {
                return __r.y*10000 + __r.x;
        }
};

#define REPLC(c, n)	\
{	\
	Color ic = c->GetPixelColor(n.x, n.y);	\
	std::swap(ic.red_, ic.green_);	\
	c->SetPixelColor(n.x, n.y, ic);	\
}

#define GETC ejw3

static bool GETC(const Canvas* c, const PixelCoord& p) {
	Color ic = c->GetPixelColor(p.x, p.y);
	return (ic.green_ > ic.red_ + ic.blue_);
	//return (ic.red_ > ic.green_ + ic.blue_);
}

#define UniquePut(q, v)	\
	if (!q.Exists(v)) q.PushBack(v, v);

void App::RearrangeHiScore() {
	Canvas* c = backdrop_->GetRamData();
	PixelCoord p(150, 150);
	typedef OrderedMap<PixelCoord, PixelCoord, MyHash> piq;
	piq __piq;
	__piq.PushBack(p, p);
	while (!__piq.IsEmpty()) {
		PixelCoord n;
		__piq.PopFront(n, n);
		REPLC(c, n);
		p = n + PixelCoord(-1, 0);
		if (GETC(c, p))	UniquePut(__piq, p);
		p = n + PixelCoord(+1, 0);
		if (GETC(c, p))	UniquePut(__piq, p);
		p = n + PixelCoord(0, -1);
		if (GETC(c, p))	UniquePut(__piq, p);
		p = n + PixelCoord(0, +1);
		if (GETC(c, p))	UniquePut(__piq, p);
	}
	ui_manager_->GetPainter()->RemoveImage(backdrop_->GetData());
	UiCure::PainterImageResource* r = (UiCure::PainterImageResource*)backdrop_->GetConstResource();
	r->SetOptimizedData(uitbc::Painter::kInvalidImageid);
	r->PostProcess();
}

int App::GetExecutionCount() const {
#ifdef LEPRA_IOS
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	const double start_time = [defaults doubleForKey:@"FirstStartTime"];
#else // !iOS
	const double start_time = 0;
#endif // iOS / !iOS
	const double current_time = HiResTimer::GetSystemCounter() * HiResTimer::GetPeriod();
	const double diff = current_time - start_time;
	const double executions = 5000 * diff / 365 / 24 / 60 / 60;
	return (int)Math::Round(executions) + 1;
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

uitbc::Button* App::CreateButton(const wstr& text, const Color& color) {
	uitbc::Button* _button = new uitbc::Button(color, text);
	_button->SetText(text);
	_button->SetPreferredSize(200, 48);
	_button->SetRoundedRadius(10);
	_button->UpdateLayout();
	return _button;
}

uitbc::Dialog* App::CreateTbcDialog(ButtonAction action) {
	if (dialog_) {
		return 0;
	}
	button_delegate_ = uitbc::Dialog::Action(this, action);
	uitbc::Dialog* d = new uitbc::Dialog(ui_manager_->GetDesktopWindow(), uitbc::Dialog::Action(this, &App::OnAction));
	d->SetPreClickTarget(uitbc::Dialog::Action(this, &App::OnTapSound));
	d->SetSize(280, 320);
	d->SetPreferredSize(280, 320);
	d->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	dialog_ = d;
	return d;
}



void App::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource) {
	(void)resource;
}

void App::SoundLoadCallback(UiCure::UserSound2dResource* resource) {
	(void)resource;
}



wstr App::Int2Str(int number) {
	wstr s = wstrutil::IntToString(number, 10);
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
