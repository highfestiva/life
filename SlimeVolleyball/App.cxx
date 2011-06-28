
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Lepra/Include/Application.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiDisplayManager.h"
#include "../UiLepra/Include/UiLepra.h"
#include "../UiLepra/Include/UiInput.h"
#include "../UiLepra/Include/UiSoundManager.h"
#include "../UiLepra/Include/UiSoundStream.h"
#include "../UiTBC/Include/GUI/UiButton.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../UiTBC/Include/UiFontManager.h"
#include "../UiTBC/Include/UiOpenGLPainter.h"
#include "../UiTBC/Include/UiOpenGLRenderer.h"
#include "SlimeVolleyball.hpp"



namespace Slime
{



class App: public Application, public UiLepra::DisplayResizeObserver, public UiLepra::KeyCodeInputObserver
{
public:
	typedef Application Parent;

	App(const strutil::strvec& pArgumentList);
	virtual ~App();

	static bool PollApp();
	static void OnTap(float x, float y, bool pIsLowest);

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

	virtual void Suspend();
	virtual void Resume();

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);

	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	void OnSpeedClick(UiTbc::Button* pButton);
	void OnPClick(UiTbc::Button* pButton);
	void OnFinishedClick(UiTbc::Button* pButton);

	static UiTbc::Button* CreateButton(const str& pText, const Color& pColor, UiTbc::DesktopWindow* pDesktop);

	static App* mApp;
#ifdef LEPRA_IOS
	AnimatedApp* mAnimatedApp;
#endif // iOS
	SlimeVolleyball* mGame;

	UiLepra::DisplayManager* mDisplay;
	Canvas* mCanvas;
	UiTbc::Painter* mPainter;
	UiTbc::FontManager* mFontManager;
	UiTbc::DesktopWindow* mDesktopWindow;
	UiLepra::InputManager* mInput;
	UiLepra::SoundManager* mSound;
	UiLepra::SoundStream* mMusicStreamer;
	str mPathPrefix;
	int mLayoutFrameCounter;
	UiTbc::Button* mLazyButton;
	UiTbc::Button* mHardButton;
	UiTbc::Button* mOriginalButton;
	UiTbc::Button* m1PButton;
	UiTbc::Button* m2PButton;
	UiTbc::Button* mNextButton;
	UiTbc::Button* mResetButton;
	UiTbc::Button* mRetryButton;

	LOG_CLASS_DECLARE();
};



}



LEPRA_RUN_APPLICATION(Slime::App, UiLepra::UiMain);



namespace Slime
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mLayoutFrameCounter(-10)
{
	mApp = this;
}

App::~App()
{
	UiLepra::Shutdown();
}

bool App::PollApp()
{
	if (!mApp->Poll())
	{
		return false;
	}
	return (SystemManager::GetQuitRequest() == 0);
}

void App::OnTap(float x, float y, bool pIsLowest)
{
	mApp->mGame->MoveTo(y, x, pIsLowest);
}

Graphics App::GetGraphics()
{
#ifdef LEPRA_IOS
	const int w = mDisplay->GetHeight();
	const int h = mDisplay->GetWidth();
#else // !iOS
	const int w = mDisplay->GetWidth();
	const int h = mDisplay->GetHeight();
#endif // iOS/!iOS
	return Graphics(w, h, mPainter);
}

bool App::Open()
{
#ifdef LEPRA_IOS
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	const int lDisplayWidth = lSize.width;
	const int lDisplayHeight = lSize.height;
#else // !iOS
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
#endif // iOS/!iOS
	int lDisplayBpp = 0;
	int lDisplayFrequency = 0;
	bool lDisplayFullScreen = false;
	UiLepra::DisplayManager::ContextType lRenderingContext = UiLepra::DisplayManager::OPENGL_CONTEXT;
	const UiLepra::SoundManager::ContextType lSoundContext = UiLepra::SoundManager::CONTEXT_OPENAL;

	// Initialize UI based on settings parameters.
	bool lOk = true;
	mDisplay = UiLepra::DisplayManager::CreateDisplayManager(lRenderingContext);
	UiLepra::DisplayMode lDisplayMode;
	if (lDisplayBpp > 0 && lDisplayFrequency > 0)
	{
		lOk = mDisplay->FindDisplayMode(lDisplayMode, lDisplayWidth, lDisplayHeight, lDisplayBpp, lDisplayFrequency);
	}
	else if (lDisplayBpp > 0)
	{
		lOk = mDisplay->FindDisplayMode(lDisplayMode, lDisplayWidth, lDisplayHeight, lDisplayBpp);
	}
	else
	{
		lOk = mDisplay->FindDisplayMode(lDisplayMode, lDisplayWidth, lDisplayHeight);
	}
	if (!lOk)
	{
		str lError(strutil::Format(_T("Unsupported resolution %ux%u."), lDisplayWidth, lDisplayHeight));
		if (lDisplayFullScreen)
		{
			mLog.Error(lError);
		}
		else
		{
			lOk = true;	// Go ahead - running in a window is OK.
			lDisplayMode.mWidth = lDisplayWidth;
			lDisplayMode.mHeight = lDisplayHeight;
			lDisplayMode.mBitDepth = lDisplayBpp;
			lDisplayMode.mRefreshRate = lDisplayFrequency;
		}
	}
	if (lOk)
	{
		if (lDisplayFullScreen)
		{
			lOk = mDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::FULLSCREEN);
		}
		else
		{
			lOk = mDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::WINDOWED);
		}
	}
	if (lOk)
	{
		mDisplay->SetCaption(_T("Slime Volleyball"));
		mDisplay->AddResizeObserver(this);

		mCanvas = new Canvas(lDisplayMode.mWidth, lDisplayMode.mHeight, Canvas::IntToBitDepth(lDisplayMode.mBitDepth));
	}
	if (lOk)
	{
		if (lRenderingContext == UiLepra::DisplayManager::OPENGL_CONTEXT)
		{
			mPainter = new UiTbc::OpenGLPainter;
		}
	}

	if (lOk)
	{
		mFontManager = UiTbc::FontManager::Create(mDisplay);
		mPainter->SetFontManager(mFontManager);

		UiTbc::FontManager::FontId lFontId = UiTbc::FontManager::INVALID_FONTID;
		const double lFontHeight = lDisplayHeight / 24.0;
		const tchar* lFontNames[] =
		{
			_T("Times New Roman"),
			_T("Arial"),
			_T("Courier New"),
			_T("Verdana"),
			_T("Helvetica"),
			0
		};
		for (int x = 0; lFontNames[x] && lFontId == UiTbc::FontManager::INVALID_FONTID; ++x)
		{
			lFontId = mFontManager->QueryAddFont(lFontNames[x], lFontHeight);
		}
	}
	if (lOk)
	{
		mCanvas->SetBuffer(0);
		mPainter->SetDestCanvas(mCanvas);
	}
	if (lOk)
	{
		mInput = 0;
#ifndef LEPRA_IOS
		mInput = UiLepra::InputManager::CreateInputManager(mDisplay);
		mInput->ActivateAll();
		mInput->AddKeyCodeInputObserver(this);
#endif // !iOS
	}
	if (lOk)
	{
		mDesktopWindow = new UiTbc::DesktopWindow(mInput, mPainter, new UiTbc::FloatingLayout(), 0, 0);
		mDesktopWindow->SetIsHollow(true);
		mDesktopWindow->SetPreferredSize(mCanvas->GetWidth(), mCanvas->GetHeight());
		mLazyButton = CreateButton(_T("Zzzzz"), Color(50, 150, 0), mDesktopWindow);
		mLazyButton->SetOnClick(App, OnSpeedClick);
		mHardButton = CreateButton(_T("n00b"), Color(150, 150, 0), mDesktopWindow);
		mHardButton->SetOnClick(App, OnSpeedClick);
		mOriginalButton = CreateButton(_T("Original"), Color(150, 50, 0), mDesktopWindow);
		mOriginalButton->SetOnClick(App, OnSpeedClick);

		m1PButton = CreateButton(_T("1P"), Color(128, 64, 0), mDesktopWindow);
		m1PButton->SetOnClick(App, OnPClick);
		m2PButton = CreateButton(_T("2P"), Color(128, 170, 160), mDesktopWindow);
		m2PButton->SetOnClick(App, OnPClick);

		mNextButton = CreateButton(_T("Next"), Color(0, 192, 0), mDesktopWindow);
		mNextButton->SetOnClick(App, OnFinishedClick);
		mResetButton = CreateButton(_T("Quit"), Color(192, 0, 0), mDesktopWindow);
		mResetButton->SetOnClick(App, OnFinishedClick);
		mRetryButton = CreateButton(_T("Retry"), Color(150, 150, 0), mDesktopWindow);
		mRetryButton->SetOnClick(App, OnFinishedClick);

		Layout();
	}
	if (lOk)
	{
		mSound = UiLepra::SoundManager::CreateSoundManager(lSoundContext);
	}
	if (lOk)
	{
		mMusicStreamer = mSound->CreateSoundStream(mPathPrefix+_T("Tingaliin.ogg"), UiLepra::SoundManager::LOOP_FORWARD, 0);
		if (!mMusicStreamer || !mMusicStreamer->Playback())
		{
			mLog.Errorf(_T("Unable to play beautiful muzak!"));
		}
	}

	UiLepra::Core::ProcessMessages();
	return (lOk);
}

void App::Close()
{
	// Poll system to let go of old windows.
	UiLepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	UiLepra::Core::ProcessMessages();

	delete mMusicStreamer;
	mMusicStreamer = 0;

	delete (mSound);
	mSound = 0;

	delete (mDesktopWindow);
	mDesktopWindow = 0;

	delete (mInput);
	mInput = 0;

	delete (mFontManager);
	mFontManager = 0;
	delete (mPainter);
	mPainter = 0;
	delete (mCanvas);
	mCanvas = 0;
	mDisplay->RemoveResizeObserver(this);
	delete (mDisplay);
	mDisplay = 0;

	// Poll system to let go of old windows.
	UiLepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	UiLepra::Core::ProcessMessages();
}

void App::Init()
{
	mPathPrefix = SystemManager::GetDataDirectory(mArgumentVector[0]);
}


int App::Run()
{
	UiLepra::Init();

	StdioConsoleLogListener lConsoleLogger;
	DebuggerLogListener lDebugLogger;
	LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(&lConsoleLogger, &lDebugLogger, 0, 0, 0);

	bool lOk = true;
	if (lOk)
	{
		lOk = Open();
	}
	if (lOk)
	{
		mGame = new SlimeVolleyball;
		lOk = mGame->init(GetGraphics());
	}
#ifndef LEPRA_IOS
	bool lQuit = false;
	while (lOk && !lQuit)
	{
		lOk = Poll();
		lQuit = (SystemManager::GetQuitRequest() != 0);
	}
	Close();
	return lQuit? 0 : 1;
#else // iOS
	mAnimatedApp = [AnimatedApp alloc];
	return 0;
#endif // !iOS/iOS
}

bool App::Poll()
{
	UiLepra::Core::ProcessMessages();
	PreparePaint();
	++mLayoutFrameCounter;
	if (mLayoutFrameCounter < 0 || mLayoutFrameCounter > 220)
	{
		mLayoutFrameCounter = 0;
		Layout();
	}
	mGame->paint(GetGraphics());
	Paint();
	mGame->run();
	Logic();
	EndRender();

#ifndef LEPRA_IOS
	mInput->PollEvents();
	mInput->Refresh();
#endif // !iOS

	if (mMusicStreamer && mMusicStreamer->Update())
	{
		if(!mMusicStreamer->IsPlaying())
		{
			mMusicStreamer->Playback();
		}
	}
	return true;
}

void App::Logic()
{
	if (mGame->fInPlay)
	{
		return;
	}

	if (mGame->mPlayerCount == 1)
	{
		if (!mGame->bGameOver && !mNextButton->IsVisible())
		{
			mNextButton->SetVisible(true);
		}
		else if (mGame->bGameOver && !mResetButton->IsVisible())
		{
			mResetButton->SetVisible(true);
			if (mGame->canContinue())
			{
				mRetryButton->SetVisible(true);
			}
		}
		return;
	}

	if (!mLazyButton->IsVisible() && !m1PButton->IsVisible())
	{
		mGame->ShowTitle();
		mLazyButton->SetVisible(true);
		mHardButton->SetVisible(true);
		mOriginalButton->SetVisible(true);
	}
}

void App::Layout()
{
	if (!mLazyButton)
	{
		return;
	}
	const int x = 20;
	const int dy = mLazyButton->GetSize().y * 4/3;
	const int sy = mCanvas->GetHeight() / 20 + 34;
	mLazyButton->SetPos(x, sy);
	mHardButton->SetPos(x, sy+dy);
	mOriginalButton->SetPos(x, sy+dy*2);
	m1PButton->SetPos(x, sy);
	m2PButton->SetPos(x, sy+dy);
	mNextButton->SetPos(x, sy);
	mResetButton->SetPos(x, sy);
	mRetryButton->SetPos(x, sy+dy);
}



void App::Paint()
{
	if (CanRender())
	{
		mDesktopWindow->Repaint(mPainter);
	}
}

void App::PreparePaint()
{
	if (CanRender())
	{
		mCanvas->SetBuffer(0);
		mPainter->SetDestCanvas(mCanvas);
		mPainter->ResetClippingRect();
		mPainter->Clear(mGame->SKY_COL);
		mPainter->PrePaint();
	}
}

void App::EndRender()
{
	if (CanRender())
	{
		mDisplay->UpdateScreen();
	}
}

bool App::CanRender() const
{
	return mDisplay->IsVisible();
}



void App::Suspend()
{
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
#endif // iOS
}

void App::Resume()
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
}


bool App::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	Event lEvent;
	lEvent.id = 401;
	lEvent.key = pKeyCode;
	mGame->handleEvent(lEvent);
	return false;
}

bool App::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	Event lEvent;
	lEvent.id = 402;
	lEvent.key = pKeyCode;
	mGame->handleEvent(lEvent);
	return false;
}



void App::OnResize(int pWidth, int pHeight)
{
	if (mCanvas)
	{
		mCanvas->Reset(pWidth, pHeight, mCanvas->GetBitDepth());
		mDesktopWindow->SetPreferredSize(mCanvas->GetWidth(), mCanvas->GetHeight());
		mDesktopWindow->SetSize(mCanvas->GetWidth(), mCanvas->GetHeight());
		mInput->Refresh();
	}
	Layout();
}

void App::OnMinimize()
{
}

void App::OnMaximize(int pWidth, int pHeight)
{
	OnResize(pWidth, pHeight);
}

void App::OnSpeedClick(UiTbc::Button* pButton)
{
	if (pButton == mLazyButton)
	{
		mGame->mSpeed = -15;
	}
	else if (pButton == mHardButton)
	{
		mGame->mSpeed = -5;
	}
	else if (pButton == mOriginalButton)
	{
		mGame->mSpeed = 0;
	}
	mLazyButton->SetVisible(false);
	mHardButton->SetVisible(false);
	mOriginalButton->SetVisible(false);
	m1PButton->SetVisible(true);
	m2PButton->SetVisible(true);
}

void App::OnPClick(UiTbc::Button* pButton)
{
	mGame->mPlayerCount = 1;
	if (pButton == m2PButton)
	{
		mGame->mPlayerCount = 2;
	}
	m1PButton->SetVisible(false);
	m2PButton->SetVisible(false);

	mGame->resetGame();
}

void App::OnFinishedClick(UiTbc::Button* pButton)
{
	if (pButton == mNextButton)
	{
		mGame->nextGameLevel();
	}
	else if (pButton == mRetryButton)
	{
		mGame->retryGame();
	}
	else if (pButton == mResetButton)
	{
		mGame->mPlayerCount = 2;	// TRICKY: quit == 2P game over.
	}
	mNextButton->SetVisible(false);
	mResetButton->SetVisible(false);
	mRetryButton->SetVisible(false);
}

UiTbc::Button* App::CreateButton(const str& pText, const Color& pColor, UiTbc::DesktopWindow* pDesktop)
{
	UiTbc::Button* lButton = new UiTbc::Button(UiTbc::BorderComponent::LINEAR, 6, pColor, _T(""));
	lButton->SetText(pText);
	lButton->SetPreferredSize(pDesktop->GetSize().x/6, pDesktop->GetSize().y/9);
	pDesktop->AddChild(lButton);
	lButton->SetVisible(false);
	lButton->UpdateLayout();
	return lButton;
}



App* App::mApp = 0;
LOG_CLASS_DEFINE(GAME, App);



}
