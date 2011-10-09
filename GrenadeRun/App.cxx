
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include <list>
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/Application.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiCure/Include/UiCppContextObject.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"
#include "../UiLepra/Include/Mac/UiIosInput.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiDisplayManager.h"
#include "../UiLepra/Include/UiInput.h"
#include "../UiLepra/Include/UiSoundManager.h"
#include "../UiLepra/Include/UiSoundStream.h"
#include "../UiTBC/Include/GUI/UiButton.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "Game.h"



#define UIKEY(name)	UiLepra::InputManager::IN_KBD_##name
#define FPS		20
#define BUTTON_WIDTH	28
#define BUTTON_MARGIN	8



namespace GrenadeRun
{



FingerMoveList gFingerMoveList;



class App: public Application, public UiLepra::DisplayResizeObserver, public UiLepra::KeyCodeInputObserver
{
public:
	typedef Application Parent;

	App(const strutil::strvec& pArgumentList);
	virtual ~App();

	static bool PollApp();
	static void OnTap(const FingerMovement& pMove);
	static void OnMouseTap(float x, float y, bool pPressed);

private:
	bool Open();
	void Close();
	virtual void Init();
	virtual int Run();
	bool Poll();
	void DrawButtons() const;
	void DrawButton(float x, float y, float pRadius, float pAngle, int pCorners) const;
	void Layout();

	virtual void Suspend();
	virtual void Resume();

	bool Steer(UiLepra::InputManager::KeyCode pKeyCode, float pFactor);
	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnMouseMove(float x, float y, bool pPressed);

	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	void OnSpeedClick(UiTbc::Button* pButton);
	void OnPClick(UiTbc::Button* pButton);
	void OnFinishedClick(UiTbc::Button* pButton);
	void OnGetiPhoneClick(UiTbc::Button*);

	static UiTbc::Button* CreateButton(const str& pText, const Color& pColor, UiTbc::DesktopWindow* pDesktop);

	StdioConsoleLogListener mConsoleLogger;
	DebuggerLogListener mDebugLogger;

	static App* mApp;
#ifdef LEPRA_IOS
	AnimatedApp* mAnimatedApp;
#endif // iOS
	Game* mGame;

	double mAverageLoopTime;
	HiResTimer mLoopTimer;
	bool mDoLayout;
	Cure::ResourceManager* mResourceManager;
	Cure::RuntimeVariableScope* mVariableScope;
	UiCure::GameUiManager* mUiManager;
	UiLepra::SoundStream* mMusicStreamer;
	int mLayoutFrameCounter;
	UiTbc::Button* mLazyButton;
	UiTbc::Button* mHardButton;
	UiTbc::Button* mOriginalButton;
	UiTbc::Button* m1PButton;
	UiTbc::Button* m2PButton;
	UiTbc::Button* mNextButton;
	UiTbc::Button* mResetButton;
	UiTbc::Button* mRetryButton;
	UiTbc::Button* mGetiPhoneButton;
	UiTbc::RectComponent* mPlayerSplitter;

	LOG_CLASS_DECLARE();
};



}



LEPRA_RUN_APPLICATION(GrenadeRun::App, UiLepra::UiMain);



namespace GrenadeRun
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mLayoutFrameCounter(-10),
	mVariableScope(0),
	mAverageLoopTime(1.0/FPS),
	mDoLayout(true)
{
	mApp = this;
}

App::~App()
{
	mVariableScope = 0;
	UiCure::Shutdown();
	UiTbc::Shutdown();
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

void App::OnTap(const FingerMovement& pMove)
{
	pMove;
	//mApp->mGame->MoveTo(pMove);
}

void App::OnMouseTap(float x, float y, bool pPressed)
{
	mApp->OnMouseMove(x, y, pPressed);
}

bool App::Open()
{
#ifdef LEPRA_IOS
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	const int lDisplayWidth = lSize.height;
	const int lDisplayHeight = lSize.width;
#else // !iOS
//	const int lDisplayWidth = 760;
//	const int lDisplayHeight = 524;
	const int lDisplayWidth = 485;
	const int lDisplayHeight = 340;
#endif // iOS/!iOS
	int lDisplayBpp = 0;
	int lDisplayFrequency = 0;
	bool lDisplayFullScreen = false;
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_RENDERENGINE, _T("OpenGL"));
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_WIDTH, lDisplayWidth);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_HEIGHT, lDisplayHeight);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_BITSPERPIXEL, lDisplayBpp);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_FREQUENCY, lDisplayFrequency);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_FULLSCREEN, lDisplayFullScreen);

	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));

	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ENABLEVSYNC, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLELIGHTS, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_FOV, 60.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_CLIPNEAR, 1.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_CLIPFAR, 1000.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("None"));
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTRED, 0.5);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTGREEN, 0.5);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTBLUE, 0.5);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.2);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_DOPPLER, 1.3);

#ifndef LEPRA_IOS
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("Force:Volume"));	
#endif // !iOS

	mUiManager = new UiCure::GameUiManager(mVariableScope);
	bool lOk = mUiManager->Open();
	if (lOk)
	{
#ifdef LEPRA_IOS
		mUiManager->GetCanvas()->SetOutputRotation(90);
#endif // iOS
		mUiManager->GetDisplayManager()->SetCaption(_T("Kill Cutie"));
		mUiManager->GetDisplayManager()->AddResizeObserver(this);
		mUiManager->GetInputManager()->AddKeyCodeInputObserver(this);
	}
	if (lOk)
	{
		UiTbc::DesktopWindow* lDesktopWindow = mUiManager->GetDesktopWindow();
		/*mLazyButton = CreateButton(_T("Slower"), Color(50, 150, 0), lDesktopWindow);
		mLazyButton->SetOnClick(App, OnSpeedClick);
		mHardButton = CreateButton(_T("Slow"), Color(192, 192, 0), lDesktopWindow);
		mHardButton->SetOnClick(App, OnSpeedClick);
		mOriginalButton = CreateButton(_T("Original"), Color(210, 0, 0), lDesktopWindow);
		mOriginalButton->SetOnClick(App, OnSpeedClick);

		m1PButton = CreateButton(_T("1P"), Color(128, 64, 0), lDesktopWindow);
		m1PButton->SetOnClick(App, OnPClick);
		m2PButton = CreateButton(_T("2P"), Color(128, 0, 128), lDesktopWindow);
		m2PButton->SetOnClick(App, OnPClick);

		mNextButton = CreateButton(_T("Next"), Color(50, 150, 0), lDesktopWindow);
		mNextButton->SetOnClick(App, OnFinishedClick);
		mResetButton = CreateButton(_T("Menu"), Color(210, 0, 0), lDesktopWindow);
		mResetButton->SetOnClick(App, OnFinishedClick);
		mRetryButton = CreateButton(_T("Rematch"), Color(192, 192, 0), lDesktopWindow);
		mRetryButton->SetOnClick(App, OnFinishedClick);*/

		mGetiPhoneButton = 0;
/*#ifndef LEPRA_IOS
		mGetiPhoneButton = CreateButton(_T("4 iPhone!"), Color(45, 45, 45), lDesktopWindow);
		mGetiPhoneButton->SetVisible(true);
		mGetiPhoneButton->SetOnClick(App, OnGetiPhoneClick);
#endif // iOS*/

		mPlayerSplitter = new UiTbc::RectComponent(BLACK, _T("Splitter"));
		lDesktopWindow->AddChild(mPlayerSplitter);
	}
	if (lOk)
	{
		const str lPathPrefix = SystemManager::GetDataDirectory(mArgumentVector[0]);
		mMusicStreamer = 0;
		//mMusicStreamer = mUiManager->GetSoundManager()->CreateSoundStream(lPathPrefix+_T("Oiit.ogg"), UiLepra::SoundManager::LOOP_FORWARD, 0);
		if (!mMusicStreamer || !mMusicStreamer->Playback())
		{
			mLog.Errorf(_T("Unable to play beautiful muzak!"));
		}
		else
		{
			mMusicStreamer->SetVolume(0.5f);
		}
	}

	UiLepra::Core::ProcessMessages();
	return (lOk);
}

void App::Close()
{
	mUiManager->GetInputManager()->RemoveKeyCodeInputObserver(this);
	mUiManager->GetDisplayManager()->RemoveResizeObserver(this);

	// Poll system to let go of old windows.
	UiLepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	UiLepra::Core::ProcessMessages();

	delete mMusicStreamer;
	mMusicStreamer = 0;

	delete mGame;
	mGame = 0;

	delete mResourceManager;	// Resource manager lives long enough for all volontary resources to disappear.
	mResourceManager = 0;

	delete mUiManager;
	mUiManager = 0;

	// Poll system to let go of old windows.
	UiLepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	UiLepra::Core::ProcessMessages();
}

void App::Init()
{
}


int App::Run()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

	const str lLogName = Path::JoinPath(SystemManager::GetIoDirectory(_T("KillCutie")), _T("log"), _T("txt"));
	FileLogListener lFileLogger(lLogName);
	{
		LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(&mConsoleLogger, &mDebugLogger, &lFileLogger, 0, 0);
		const std::vector<Log*> lLogArray = LogType::GetLogs();
		std::vector<Log*>::const_iterator x = lLogArray.begin();
		for (; x != lLogArray.end(); ++x)
		{
			(*x)->SetLevelThreashold(Log::LEVEL_INFO);
		}
	}
	bool lOk = true;
	if (lOk)
	{
		mVariableScope = UiCure::GetSettings();
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_PARALLEL, false);	// Let's do it same on all platforms.
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_MICROSTEPS, 3);
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_FPS, FPS);
		//CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLELIGHTS, false);
	}
	if (lOk)
	{
		const str lPathPrefix = SystemManager::GetDataDirectory(mArgumentVector[0]);
		mResourceManager = new Cure::ResourceManager(1, lPathPrefix);
	}
	if (lOk)
	{
		lOk = Open();
	}
	if (lOk)
	{
		mGame = new Game(mUiManager, mVariableScope, mResourceManager);
		lOk = mGame->Initialize();
	}
	if (lOk)
	{
		lOk = mResourceManager->InitDefault();
	}
#ifndef LEPRA_IOS
	bool lQuit = false;
	while (!lQuit)
	{
		lQuit = !Poll();
	}
	Close();
	return 0;
#else // iOS
	mAnimatedApp = [[AnimatedApp alloc] init:mUiManager->GetCanvas()];
	return 0;
#endif // !iOS/iOS
}

bool App::Poll()
{
	bool lOk = true;
	if (lOk)
	{
		mAverageLoopTime = Lepra::Math::Lerp(mAverageLoopTime, mLoopTimer.QueryTimeDiff(), 0.05);
		Thread::Sleep(1.0/FPS - mAverageLoopTime);
		mLoopTimer.PopTimeDiff();
		lOk = (SystemManager::GetQuitRequest() == 0);
	}
	if (lOk && mDoLayout)
	{
		Layout();
	}
	if (lOk)
	{
		float r, g, b;
		CURE_RTVAR_GET(r, =(float), mVariableScope, RTVAR_UI_3D_CLEARRED, 0.75);
		CURE_RTVAR_GET(g, =(float), mVariableScope, RTVAR_UI_3D_CLEARGREEN, 0.80);
		CURE_RTVAR_GET(b, =(float), mVariableScope, RTVAR_UI_3D_CLEARBLUE, 0.85);
		Vector3DF lColor(r, g, b);
		mUiManager->BeginRender(lColor);
	}
	if (lOk)
	{
		mUiManager->InputTick();
	}
	if (lOk)
	{
		mGame->BeginTick();
	}
	bool lRender = false;
	if (lOk)
	{
		lRender = mUiManager->CanRender();
	}
	if (lOk && lRender)
	{
		lOk = mGame->Render();
	}
	if (lOk && lRender)
	{
		mUiManager->Paint(false);
		DrawButtons();
	}
	if (lOk)
	{
		lOk = mGame->EndTick();
	}
	if (lOk)
	{
		lOk = mGame->Tick();
	}
	mResourceManager->Tick();
	mUiManager->EndRender();

	if (mMusicStreamer && mMusicStreamer->Update())
	{
		if(!mMusicStreamer->IsPlaying())
		{
			mMusicStreamer->Pause();
			mMusicStreamer->Playback();
		}
	}
	return lOk;
}

void App::DrawButtons() const
{
	const float lButtonWidth = BUTTON_WIDTH;	// TODO: fix for Retina.
	const float lButtonRadius = lButtonWidth/2;
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();
	const float m = BUTTON_MARGIN;
	const float m2 = m*2;

	mUiManager->GetPainter()->SetColor(Color(64, 64, 255), 0);
	// Left player.
	DrawButton(m2+lButtonWidth*1.5f,	m+lButtonRadius,		lButtonRadius, +PIF/2,	3);	// Up.
	DrawButton(m+lButtonRadius,		m+lButtonRadius,		lButtonRadius, -PIF/2,	3);	// Down.
	DrawButton(m+lButtonRadius,		h-m2-lButtonWidth*1.5f,		lButtonRadius, 0,	3);	// Left.
	DrawButton(m+lButtonRadius,		h-m-lButtonRadius,		lButtonRadius, PIF,	3);	// Right.
	// Right player.
	DrawButton(w-m2-lButtonWidth*1.5f,	h-m-lButtonRadius,		lButtonRadius, -PIF/2,	3);	// Up.
	DrawButton(w-m-lButtonRadius,		h-m-lButtonRadius,		lButtonRadius, +PIF/2,	3);	// Down.
	DrawButton(w-m-lButtonRadius,		m2+lButtonWidth*1.5f,		lButtonRadius, PIF,	3);	// Left.
	DrawButton(w-m-lButtonRadius,		m+lButtonRadius,		lButtonRadius, 0,	3);	// Right.
	// Bomb button.
	DrawButton(w-m-lButtonRadius,		h/2,				lButtonRadius,	PIF/4,	4);	// Square.
}

void App::DrawButton(float x, float y, float pRadius, float pAngle, int pCorners) const
{
	const float lRoundRadius = pRadius * 0.96f;
	const float lRoundAngle = PIF/16;
	std::vector<Vector2DF> lCoords;
	lCoords.push_back(Vector2DF(x, y));
	for (int i = 0; i < pCorners; ++i)
	{
		lCoords.push_back(Vector2DF(x+lRoundRadius*::sin(pAngle-lRoundAngle), y-lRoundRadius*::cos(pAngle-lRoundAngle)));
		lCoords.push_back(Vector2DF(x+pRadius*::sin(pAngle), y-pRadius*::cos(pAngle)));
		lCoords.push_back(Vector2DF(x+lRoundRadius*::sin(pAngle+lRoundAngle), y-lRoundRadius*::cos(pAngle+lRoundAngle)));
		pAngle += 2*PIF/pCorners;
	}
	lCoords.push_back(lCoords[1]);
	mUiManager->GetPainter()->DrawFan(lCoords, true);
}

void App::Layout()
{
	PixelRect lRect = mUiManager->GetDesktopWindow()->GetScreenRect();
	lRect.mLeft = lRect.GetCenterX()-5;
	lRect.mRight = lRect.mLeft+10;
	mPlayerSplitter->SetPos(lRect.mLeft, lRect.mTop);
	mPlayerSplitter->SetPreferredSize(lRect.GetSize());
	mDoLayout = false;

	/*if (!mLazyButton)
	{
		return;
	}
	const int s = 20;
	const int x = s;
	const int px = mLazyButton->GetSize().x;
	const int py = mLazyButton->GetSize().y;
	const int dy = py * 4/3;
	const int sy = mUiManager->GetCanvas()->GetHeight() / 20 + 34;
	const int tx = mUiManager->GetCanvas()->GetWidth() - s - px;
	const int ty = mUiManager->GetCanvas()->GetHeight() - s - py;
	mLazyButton->SetPos(x, sy);
	mHardButton->SetPos(x, sy+dy);
	mOriginalButton->SetPos(x, sy+dy*2);
	m1PButton->SetPos(x, sy);
	m2PButton->SetPos(x, sy+dy);
	mNextButton->SetPos(x, sy);
	mResetButton->SetPos(x, sy);
	mRetryButton->SetPos(x, sy+dy);
	if (mGetiPhoneButton)
	{
		mGetiPhoneButton->SetPos(tx, ty);
	}*/
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
	if (mMusicStreamer)
	{
		mMusicStreamer->Stop();
		mMusicStreamer->Playback();
	}
}


bool App::Steer(UiLepra::InputManager::KeyCode pKeyCode, float pFactor)
{
	if (!mGame)
	{
		return false;
	}
	UiCure::CppContextObject* lAvatar1 = mGame->GetP1();
	UiCure::CppContextObject* lAvatar2 = mGame->GetP2();
	if (!lAvatar1 || !lAvatar1->IsLoaded())
	{
		return false;
	}
	if (!lAvatar2 ||!lAvatar2->IsLoaded())
	{
		lAvatar2 = lAvatar1;
	}
	if (pKeyCode == UIKEY(SPACE) && pFactor > 0)
	{
		mGame->Shoot();
	}
	switch (pKeyCode)
	{
		case UIKEY(W):		lAvatar2->SetEnginePower(0, +1*pFactor, 0);	break;
		case UIKEY(S):		lAvatar2->SetEnginePower(0, -1*pFactor, 0);	break;
		case UIKEY(A):		lAvatar2->SetEnginePower(1, -1*pFactor, 0);	break;
		case UIKEY(D):		lAvatar2->SetEnginePower(1, +1*pFactor, 0);	break;
		case UIKEY(UP):		lAvatar1->SetEnginePower(0, +1*pFactor, 0);	break;
		case UIKEY(DOWN):	lAvatar1->SetEnginePower(0, -1*pFactor, 0);	break;
		case UIKEY(LEFT):	lAvatar1->SetEnginePower(1, -1*pFactor, 0);	break;
		case UIKEY(RIGHT):	lAvatar1->SetEnginePower(1, +1*pFactor, 0);	break;
		case UIKEY(NUMPAD_0):	lAvatar1->SetEnginePower(2, +1*pFactor, 0);	break;
	}
	return false;
}

bool App::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	return Steer(pKeyCode, 1);
}

bool App::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	return Steer(pKeyCode, 0);
}

void App::OnMouseMove(float x, float y, bool pPressed)
{
	x;
	y;
	pPressed;
#ifdef LEPRA_IOS
	((UiLepra::IosInputManager*)mUiManager->GetInputManager())->SetMousePosition(x, y);
	((UiLepra::IosInputElement*)mUiManager->GetInputManager()->GetMouse()->GetButton(0))->SetValue(pPressed? 1 : 0);
	((UiLepra::IosInputElement*)mUiManager->GetInputManager()->GetMouse()->GetAxis(0))->SetValue(x);
	((UiLepra::IosInputElement*)mUiManager->GetInputManager()->GetMouse()->GetAxis(1))->SetValue(y);

	UiCure::CppContextObject* lAvatar1 = mGame->GetP1();
	UiCure::CppContextObject* lAvatar2 = mGame->GetP2();
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();
	std::swap(x, y);
	y = h-y;
	const float m = BUTTON_MARGIN;
	const float lSingleWidth = m*2 + BUTTON_WIDTH;
	const float lDoubleWidth = m*3 + BUTTON_WIDTH*2;
	const float s = lDoubleWidth / 2;
	if (x <= lDoubleWidth && y <= lSingleWidth)	// P1 up/down?
	{
		mGame->SetThrottle(lAvatar1, (x-s)/s);
	}
	else if (x <= lSingleWidth && y >= h-lDoubleWidth)	// P1 left/right?
	{
		lAvatar1->SetEnginePower(1, (y-(h-s))/s, 0);
	}
	else if (x >= w-lDoubleWidth && y >= h-lSingleWidth)	// P2 up/down?
	{
		mGame->SetThrottle(lAvatar2, (x-(w-s))/s);
	}
	else if (x >= w-lSingleWidth && y <= lDoubleWidth)	// P1 left/right?
	{
		lAvatar2->SetEnginePower(1, (s-y)/s, 0);
	}
	else if (x >= w-lSingleWidth && y >= h/2-s && y <= h/2+s)	// Bomb?
	{
		mGame->Shoot();
	}
#endif // iOS
}



void App::OnResize(int /*pWidth*/, int /*pHeight*/)
{
	mDoLayout = true;
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
		//mGame->mSpeed = -15;
	}
	else if (pButton == mHardButton)
	{
		//mGame->mSpeed = -5;
	}
	else if (pButton == mOriginalButton)
	{
		//mGame->mSpeed = 0;
	}
	mLazyButton->SetVisible(false);
	mHardButton->SetVisible(false);
	mOriginalButton->SetVisible(false);
	m1PButton->SetVisible(true);
	m2PButton->SetVisible(true);
}

void App::OnPClick(UiTbc::Button* pButton)
{
	//mGame->mPlayerCount = 1;
	if (pButton == m2PButton)
	{
		//mGame->mPlayerCount = 2;
	}
	m1PButton->SetVisible(false);
	m2PButton->SetVisible(false);

	//mGame->resetGame();
}

void App::OnFinishedClick(UiTbc::Button* pButton)
{
	if (pButton == mNextButton)
	{
		//mGame->nextGameLevel();
	}
	else if (pButton == mRetryButton)
	{
		//mGame->retryGame();
	}
	else if (pButton == mResetButton)
	{
		//mGame->mPlayerCount = 2;	// TRICKY: quit == 2P game over.
	}
	mNextButton->SetVisible(false);
	mResetButton->SetVisible(false);
	mRetryButton->SetVisible(false);
}

 void App::OnGetiPhoneClick(UiTbc::Button*)
{
	SystemManager::WebBrowseTo(_T("http://itunes.apple.com/us/app/slimeball/id447966821?mt=8&ls=1"));
	delete mGetiPhoneButton;
	mGetiPhoneButton = 0;
}

UiTbc::Button* App::CreateButton(const str& pText, const Color& pColor, UiTbc::DesktopWindow* pDesktop)
{
	UiTbc::Button* lButton = new UiTbc::Button(UiTbc::BorderComponent::LINEAR, 6, pColor, _T(""));
	lButton->SetText(pText);
	const int h = std::max(pDesktop->GetSize().y/9, 44);
	lButton->SetPreferredSize(pDesktop->GetSize().x/5, h);
	pDesktop->AddChild(lButton);
	lButton->SetVisible(false);
	lButton->UpdateLayout();
	return lButton;
}



App* App::mApp = 0;
LOG_CLASS_DEFINE(GAME, App);



}
