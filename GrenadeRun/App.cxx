
// Author: Jonas Byström
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

	static App* mApp;
#ifdef LEPRA_IOS
	AnimatedApp* mAnimatedApp;
#endif // iOS
	Game* mGame;

	double mAverageLoopTime;
	HiResTimer mLoopTimer;
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
	mAverageLoopTime(1.0/FPS)
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
	mApp->OnMouseMove(y, x, pPressed);
}

bool App::Open()
{
#ifdef LEPRA_IOS
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	const int lDisplayWidth = lSize.height;
	const int lDisplayHeight = lSize.width;
#else // !iOS
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
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
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_FOV, 60.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_CLIPNEAR, 1.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_CLIPFAR, 3000.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("None"));
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTRED, 0.8);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTGREEN, 0.8);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTBLUE, 0.8);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.2);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_DOPPLER, 1.3);

	mUiManager = new UiCure::GameUiManager(mVariableScope);
	bool lOk = mUiManager->Open();
	if (lOk)
	{
#ifdef LEPRA_IOS
		mUiManager->GetCanvas()->SetOutputRotation(90);
#endif // iOS
		mUiManager->GetInputManager()->AddKeyCodeInputObserver(this);
	}
	if (lOk)
	{
		UiTbc::DesktopWindow* lDesktopWindow = mUiManager->GetDesktopWindow();
		mLazyButton = CreateButton(_T("Slower"), Color(50, 150, 0), lDesktopWindow);
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
		mRetryButton->SetOnClick(App, OnFinishedClick);

		mGetiPhoneButton = 0;
/*#ifndef LEPRA_IOS
		mGetiPhoneButton = CreateButton(_T("4 iPhone!"), Color(45, 45, 45), lDesktopWindow);
		mGetiPhoneButton->SetVisible(true);
		mGetiPhoneButton->SetOnClick(App, OnGetiPhoneClick);
#endif // iOS*/

		Layout();
	}
	if (lOk)
	{
		const str lPathPrefix = SystemManager::GetDataDirectory(mArgumentVector[0]);
		mMusicStreamer = 0;
		/*mMusicStreamer = mUiManager->GetSoundManager()->CreateSoundStream(lPathPrefix+_T("Oiit.ogg"), UiLepra::SoundManager::LOOP_FORWARD, 0);
		if (!mMusicStreamer || !mMusicStreamer->Playback())
		{
			mLog.Errorf(_T("Unable to play beautiful muzak!"));
		}*/
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

	StdioConsoleLogListener lConsoleLogger;
	DebuggerLogListener lDebugLogger;
	const str lLogName = Path::JoinPath(SystemManager::GetIoDirectory(_T("KillCutie")), _T("log"), _T("txt"));
	FileLogListener lFileLogger(lLogName);
	{
		LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(&lConsoleLogger, &lDebugLogger, &lFileLogger, 0, 0);
		/*const std::vector<Log*> lLogArray = LogType::GetLogs();
		std::vector<Log*>::const_iterator x = lLogArray.begin();
		for (; x != lLogArray.end(); ++x)
		{
			(*x)->SetLevelThreashold(Log::LEVEL_LOWEST_TYPE);
		}*/
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
	if (lOk)
	{
		lOk = mUiManager->CanRender();
	}
	if (lOk)
	{
		lOk = mGame->Render();
	}
	if (lOk)
	{
		mUiManager->Paint();
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

void App::Layout()
{
	if (!mLazyButton)
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
	}
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
	if (!lAvatar1)
	{
		return false;
	}
	if (!lAvatar2)
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
#endif // iOS
}



void App::OnResize(int /*pWidth*/, int /*pHeight*/)
{
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
