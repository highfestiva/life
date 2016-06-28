
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../lepra/include/application.h"
#include "../lepra/include/loglistener.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "../uilepra/include/mac/uiiosinput.h"
#include "../uilepra/include/uicore.h"
#include "../uilepra/include/uidisplaymanager.h"
#include "../uilepra/include/uilepra.h"
#include "../uilepra/include/uiinput.h"
#include "../uilepra/include/uisoundmanager.h"
#include "../uilepra/include/uisoundstream.h"
#include "../uitbc/include/gui/uibutton.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifloatinglayout.h"
#include "../uitbc/include/uifontmanager.h"
#include "../uitbc/include/uiopenglpainter.h"
#include "../uitbc/include/uiopenglrenderer.h"
#include "slimevolleyball.hpp"



namespace slime {



FingerMoveList g_finger_move_list;



class App: public Application, public uilepra::DisplayResizeObserver, public uilepra::KeyCodeInputObserver {
public:
	typedef Application Parent;

	App(const strutil::strvec& argument_list);
	virtual ~App();

	static bool PollApp();
	static void OnTap(const FingerMovement& move);
	static void OnMouseTap(float x, float y, bool pressed);

private:
	Graphics GetGraphics();
	bool Open();
	void Close();
	virtual void Init();
	virtual int Run();
	bool Poll();
	void Logic();
	void Layout();

	void Paint();
	void PreparePaint();
	void EndRender();
	bool CanRender() const;

	virtual void Suspend(bool hard);
	virtual void Resume(bool hard);

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
	virtual void OnMouseMove(float x, float y, bool pressed);

	void OnResize(int _width, int _height);
	void OnMinimize();
	void OnMaximize(int _width, int _height);

	void OnSpeedClick(uitbc::Button* button);
	void OnPClick(uitbc::Button* button);
	void OnFinishedClick(uitbc::Button* button);
	void OnGetiPhoneClick(uitbc::Button*);

	static uitbc::Button* CreateButton(const str& text, const Color& color, uitbc::DesktopWindow* desktop);

	static App* app_;
#ifdef LEPRA_IOS
	AnimatedApp* animated_app_;
#endif // iOS
	SlimeVolleyball* game_;

	uilepra::DisplayManager* display_;
	Canvas* canvas_;
	uitbc::Painter* painter_;
	uitbc::FontManager* font_manager_;
	uitbc::DesktopWindow* desktop_window_;
	uilepra::InputManager* input_;
	uilepra::SoundManager* sound_;
	uilepra::SoundStream* music_streamer_;
	int layout_frame_counter_;
	uitbc::Button* lazy_button_;
	uitbc::Button* hard_button_;
	uitbc::Button* original_button_;
	uitbc::Button* m1PButton_;
	uitbc::Button* m2PButton_;
	uitbc::Button* next_button_;
	uitbc::Button* reset_button_;
	uitbc::Button* retry_button_;
	uitbc::Button* geti_phone_button_;
	double extra_sleep_;

	logclass();
};



}



LEPRA_RUN_APPLICATION(slime::App, uilepra::UiMain);



namespace slime {



App::App(const strutil::strvec& argument_list):
	Application(argument_list),
	layout_frame_counter_(-10),
	extra_sleep_(0) {
	app_ = this;
}

App::~App() {
	uilepra::Shutdown();
}

bool App::PollApp() {
	if (!app_->Poll()) {
		return false;
	}
	return (SystemManager::GetQuitRequest() == 0);
}

void App::OnTap(const FingerMovement& move) {
	app_->game_->MoveTo(move);
}

void App::OnMouseTap(float x, float y, bool pressed) {
	app_->OnMouseMove(y, x, pressed);
}

Graphics App::GetGraphics() {
	const int w = display_->GetWidth();
	const int h = display_->GetHeight();
	return Graphics(w, h, painter_);
}

bool App::Open() {
#ifdef LEPRA_IOS
	CGSize __size = [UIScreen mainScreen].bounds.size;
	const int display_width = __size.height;
	const int display_height = __size.width;
#else // !iOS
	const int display_width = 760;
	const int display_height = 524;
#endif // iOS/!iOS
	int display_bpp = 0;
	int display_frequency = 0;
	bool display_full_screen = false;
	uilepra::DisplayManager::ContextType rendering_context = uilepra::DisplayManager::kOpenglContext;
	const uilepra::SoundManager::ContextType sound_context = uilepra::SoundManager::kContextOpenal;

	// Initialize kUi based on settings parameters.
	bool ok = true;
	display_ = uilepra::DisplayManager::CreateDisplayManager(rendering_context);
	uilepra::DisplayMode display_mode;
	if (display_bpp > 0 && display_frequency > 0) {
		ok = display_->FindDisplayMode(display_mode, display_width, display_height, display_bpp, display_frequency);
	} else if (display_bpp > 0) {
		ok = display_->FindDisplayMode(display_mode, display_width, display_height, display_bpp);
	} else {
		ok = display_->FindDisplayMode(display_mode, display_width, display_height);
	}
	if (!ok) {
		str error(strutil::Format("Unsupported resolution %ux%u.", display_width, display_height));
		if (display_full_screen) {
			log_.Error(error);
		} else {
			ok = true;	// Go ahead - running in a window is OK.
			display_mode.width_ = display_width;
			display_mode.height_ = display_height;
			display_mode.bit_depth_ = display_bpp;
			display_mode.refresh_rate_ = display_frequency;
		}
	}
	if (ok) {
		if (display_full_screen) {
			ok = display_->OpenScreen(display_mode, uilepra::DisplayManager::kFullscreen, uilepra::DisplayManager::kOrientationAllowUpsideDown);
		} else {
			ok = display_->OpenScreen(display_mode, uilepra::DisplayManager::kWindowed, uilepra::DisplayManager::kOrientationAllowUpsideDown);
		}
	}
	if (ok) {
		display_->SetCaption("Slime Volleyball");
		display_->SetOrientation(uilepra::DisplayManager::kOrientationAllowUpsideDown);
		display_->AddResizeObserver(this);

		canvas_ = new Canvas(display_mode.width_, display_mode.height_, Canvas::IntToBitDepth(display_mode.bit_depth_));
	}
	if (ok) {
		if (rendering_context == uilepra::DisplayManager::kOpenglContext) {
			painter_ = new uitbc::OpenGLPainter;
		}
	}

	if (ok) {
		font_manager_ = uitbc::FontManager::Create(display_);
		painter_->SetFontManager(font_manager_);

		uitbc::FontManager::FontId font_id = uitbc::FontManager::kInvalidFontid;
		const double font_height = display_height / 24.0;
		const char* font_names[] =
		{
			"Times New Roman",
			"Arial",
			"Courier New",
			"Verdana",
			"Helvetica",
			"Courier New",
			"Segoe UI",
			"Open Sans",
			"Liberation Sans",
			"Liberation Serif",
			"Nimbus",
			"Cantarell",
			"Bookman",
			"Gothic",
			"Sans",
			"Serif",
			"",
			0
		};
		for (int x = 0; font_names[x] && font_id == uitbc::FontManager::kInvalidFontid; ++x) {
			font_id = font_manager_->QueryAddFont(font_names[x], font_height);
		}
	}
	if (ok) {
		canvas_->SetBuffer(0);
		painter_->SetDestCanvas(canvas_);
	}
	if (ok) {
		input_ = uilepra::InputManager::CreateInputManager(display_);
		input_->ActivateAll(false);
		input_->AddKeyCodeInputObserver(this);
		input_->SetCursorVisible(true);
	}
	if (ok) {
		desktop_window_ = new uitbc::DesktopWindow(input_, painter_, new uitbc::FloatingLayout(), 0, 0);
		desktop_window_->SetIsHollow(true);
		desktop_window_->SetPreferredSize(canvas_->GetWidth(), canvas_->GetHeight());
		lazy_button_ = CreateButton("Slower", Color(50, 150, 0), desktop_window_);
		lazy_button_->SetOnClick(App, OnSpeedClick);
		hard_button_ = CreateButton("Slow", Color(192, 192, 0), desktop_window_);
		hard_button_->SetOnClick(App, OnSpeedClick);
		original_button_ = CreateButton("Original", Color(210, 0, 0), desktop_window_);
		original_button_->SetOnClick(App, OnSpeedClick);

		m1PButton_ = CreateButton("1P", Color(128, 64, 0), desktop_window_);
		m1PButton_->SetOnClick(App, OnPClick);
		m2PButton_ = CreateButton("2P", Color(128, 0, 128), desktop_window_);
		m2PButton_->SetOnClick(App, OnPClick);

		next_button_ = CreateButton("Next", Color(50, 150, 0), desktop_window_);
		next_button_->SetOnClick(App, OnFinishedClick);
		reset_button_ = CreateButton("Menu", Color(210, 0, 0), desktop_window_);
		reset_button_->SetOnClick(App, OnFinishedClick);
		retry_button_ = CreateButton("Rematch", Color(192, 192, 0), desktop_window_);
		retry_button_->SetOnClick(App, OnFinishedClick);

#ifdef LEPRA_TOUCH
		geti_phone_button_ = 0;
#else // !iOS
		geti_phone_button_ = CreateButton("4 iPhone!", Color(45, 45, 45), desktop_window_);
		geti_phone_button_->SetOnClick(App, OnGetiPhoneClick);
#endif // iOS / !iOS

		Layout();
	}
	if (ok) {
		sound_ = uilepra::SoundManager::CreateSoundManager(sound_context);
	}
	if (ok) {
		music_streamer_ = 0;
		music_streamer_ = sound_->CreateSoundStream("tingaliin.ogg", uilepra::SoundManager::kLoopForward, 0);
		if (!music_streamer_ || !music_streamer_->Playback()) {
			log_.Errorf("Unable to play beautiful muzak!");
		}
	}

	uilepra::Core::ProcessMessages();
	return (ok);
}

void App::Close() {
	// Poll system to let go of old windows.
	uilepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	uilepra::Core::ProcessMessages();

	delete music_streamer_;
	music_streamer_ = 0;

	delete (sound_);
	sound_ = 0;

	delete (desktop_window_);
	desktop_window_ = 0;

	delete (input_);
	input_ = 0;

	delete (font_manager_);
	font_manager_ = 0;
	delete (painter_);
	painter_ = 0;
	delete (canvas_);
	canvas_ = 0;
	display_->RemoveResizeObserver(this);
	delete (display_);
	display_ = 0;

	// Poll system to let go of old windows.
	uilepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	uilepra::Core::ProcessMessages();
}

void App::Init() {
}


int App::Run() {
	uilepra::Init();

	StdioConsoleLogListener console_logger;
	DebuggerLogListener debug_logger;
	LogType::GetLogger(LogType::kRoot)->SetupBasicListeners(&console_logger, &debug_logger, 0, 0, 0);

	bool ok = true;
	if (ok) {
		ok = Open();
	}
	if (ok) {
		game_ = new SlimeVolleyball;
		ok = game_->init(GetGraphics());
	}
#ifndef LEPRA_IOS
	bool quit = false;
	while (ok && !quit) {
		ok = Poll();
		quit = (SystemManager::GetQuitRequest() != 0);
	}
	Close();
	return quit? 0 : 1;
#else // iOS
	animated_app_ = [[AnimatedApp alloc] init:canvas_];
	return 0;
#endif // !iOS/iOS
}

bool App::Poll() {
	if (extra_sleep_ > 0.001) {
		Thread::Sleep(extra_sleep_);
		extra_sleep_ *= 0.94;
	} else {
		extra_sleep_ = 0;
	}


	uilepra::Core::ProcessMessages();
	if (display_->IsVisible()) {
		PreparePaint();
		++layout_frame_counter_;
		if (layout_frame_counter_ < 0 || layout_frame_counter_ > 220) {
			layout_frame_counter_ = 0;
			Layout();
		}
		game_->paint(GetGraphics());
		Paint();
		game_->run();
		Logic();
		EndRender();
	}

	input_->PollEvents();
	input_->Refresh();

	if (music_streamer_ && music_streamer_->Update()) {
		if(!music_streamer_->IsPlaying()) {
			music_streamer_->Pause();
			music_streamer_->Playback();
		}
	}
	return true;
}

void App::Logic() {
	if (game_->fInPlay) {
		return;
	}

	if (game_->player_count_ == 1) {
		if (!game_->bGameOver && !next_button_->IsVisible()) {
			next_button_->SetVisible(true);
		} else if (game_->bGameOver && !reset_button_->IsVisible()) {
			reset_button_->SetVisible(true);
			if (game_->canContinue()) {
				retry_button_->SetVisible(true);
			}
		}
		return;
	}

	if (!lazy_button_->IsVisible() && !m1PButton_->IsVisible()) {
		game_->ShowTitle();
		lazy_button_->SetVisible(true);
		hard_button_->SetVisible(true);
		original_button_->SetVisible(true);
	}
}

void App::Layout() {
	if (!lazy_button_) {
		return;
	}
	const int s = 20;
	const int x = s;
	const int px = lazy_button_->GetSize().x;
	const int py = lazy_button_->GetSize().y;
	const int dy = py * 4/3;
	const int sy = canvas_->GetHeight() / 20 + 34;
	const int tx = canvas_->GetWidth() - s - px;
	const int ty = canvas_->GetHeight() - s - py;
	lazy_button_->SetPos(x, sy);
	hard_button_->SetPos(x, sy+dy);
	original_button_->SetPos(x, sy+dy*2);
	m1PButton_->SetPos(x, sy);
	m2PButton_->SetPos(x, sy+dy);
	next_button_->SetPos(x, sy);
	reset_button_->SetPos(x, sy);
	retry_button_->SetPos(x, sy+dy);
	if (geti_phone_button_) {
		geti_phone_button_->SetPos(tx, ty);
	}
}



void App::Paint() {
	if (CanRender()) {
		desktop_window_->Repaint(painter_);
	}
}

void App::PreparePaint() {
	if (CanRender()) {
		canvas_->SetBuffer(0);
		painter_->SetDestCanvas(canvas_);
		//painter_->ResetClippingRect();
		painter_->Clear(game_->SKY_COL);
		painter_->PrePaint(false);
	}
}

void App::EndRender() {
	if (CanRender()) {
		display_->UpdateScreen();
	}
}

bool App::CanRender() const {
	return display_->IsVisible();
}



void App::Suspend(bool hard) {
	(void)hard;
	if (game_->fInPlay) {
		extra_sleep_ = 0.2;
	}
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
}

void App::Resume(bool hard) {
	(void)hard;
#ifdef LEPRA_IOS
	[animated_app_ tick_];
#endif // iOS
	if (music_streamer_) {
		music_streamer_->Stop();
		music_streamer_->Playback();
	}
}


bool App::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	Event event;
	event.id = 401;
	event.key = key_code;
	game_->handleEvent(event);
	return false;
}

bool App::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	Event event;
	event.id = 402;
	event.key = key_code;
	game_->handleEvent(event);
	return false;
}

void App::OnMouseMove(float x, float y, bool pressed) {
	(void)x;
	(void)y;
	(void)pressed;
#ifdef LEPRA_IOS
	((uilepra::IosInputManager*)input_)->SetMousePosition(x, y);
	((uilepra::IosInputElement*)input_->GetMouse()->GetButton(0))->SetValue(pressed? 1 : 0);
	((uilepra::IosInputElement*)input_->GetMouse()->GetAxis(0))->SetValue(x);
	((uilepra::IosInputElement*)input_->GetMouse()->GetAxis(1))->SetValue(y);
#endif // iOS
}



void App::OnResize(int _width, int _height) {
	if (canvas_) {
		canvas_->Reset(_width, _height, canvas_->GetBitDepth());
		desktop_window_->SetPreferredSize(canvas_->GetWidth(), canvas_->GetHeight());
		desktop_window_->SetSize(canvas_->GetWidth(), canvas_->GetHeight());
		input_->Refresh();
	}
	Layout();
}

void App::OnMinimize() {
}

void App::OnMaximize(int _width, int _height) {
	OnResize(_width, _height);
}

void App::OnSpeedClick(uitbc::Button* button) {
	if (button == lazy_button_) {
		game_->speed_ = -15;
	} else if (button == hard_button_) {
		game_->speed_ = -5;
	} else if (button == original_button_) {
		game_->speed_ = 0;
	}
	lazy_button_->SetVisible(false);
	hard_button_->SetVisible(false);
	original_button_->SetVisible(false);
	m1PButton_->SetVisible(true);
	m2PButton_->SetVisible(true);
}

void App::OnPClick(uitbc::Button* button) {
	game_->player_count_ = 1;
	if (button == m2PButton_) {
		game_->player_count_ = 2;
	}
	m1PButton_->SetVisible(false);
	m2PButton_->SetVisible(false);

	game_->resetGame();
}

void App::OnFinishedClick(uitbc::Button* button) {
	if (button == next_button_) {
		game_->nextGameLevel();
	} else if (button == retry_button_) {
		game_->retryGame();
	} else if (button == reset_button_) {
		game_->player_count_ = 2;	// TRICKY: quit == 2P game over.
	}
	next_button_->SetVisible(false);
	reset_button_->SetVisible(false);
	retry_button_->SetVisible(false);
}

 void App::OnGetiPhoneClick(uitbc::Button*) {
	SystemManager::WebBrowseTo("http://itunes.apple.com/us/app/slimeball/id447966821?mt=8&ls=1");
	delete geti_phone_button_;
	geti_phone_button_ = 0;
}

uitbc::Button* App::CreateButton(const str& text, const Color& color, uitbc::DesktopWindow* desktop) {
	uitbc::Button* _button = new uitbc::Button(uitbc::BorderComponent::kLinear, 6, color, L"");
	_button->SetText(wstrutil::Encode(text));
	const int h = std::max(desktop->GetSize().y/9, 44);
	_button->SetPreferredSize(desktop->GetSize().x/5, h);
	desktop->AddChild(_button);
	_button->SetVisible(false);
	_button->UpdateLayout();
	return _button;
}



App* App::app_ = 0;
loginstance(kGame, App);



}
