
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <list>
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Application.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/FileOpener.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../UiCure/Include/UiCppContextObject.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiMusicPlayer.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"
#include "../UiLepra/Include/Mac/UiIosInput.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiDisplayManager.h"
#include "../UiLepra/Include/UiInput.h"
#include "../UiLepra/Include/UiOpenGLExtensions.h"	// Included to get the gl-headers.
#include "../UiLepra/Include/UiSoundManager.h"
#include "../UiLepra/Include/UiSoundStream.h"
#include "../UiTBC/Include/GUI/UiButton.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiDialog.h"
#include "../UiTBC/Include/GUI/UiScrollBar.h"
#include "../UiTBC/Include/UiFontManager.h"
#include "Cutie.h"
#include "Game.h"
#include "Launcher.h"



#define UIKEY(name)	UiLepra::InputManager::IN_KBD_##name
#define BUTTON_WIDTH	40
#define BUTTON_MARGIN	2
#define BGCOLOR_DIALOG	Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG	Color(170, 170, 170, 255)
#define ICONBTN(i,n)	new UiCure::IconButton(mUiManager, mResourceManager, _T(i), _T(n))
#define SCORE_POINTS	4



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

private:
	bool Open();
	void Close();
	virtual void Init();
	virtual int Run();
	void DisplayLogo();
	bool Poll();
	void PollTaps();
	void DrawHud();
	void DrawImage(UiTbc::Painter::ImageID pImageId, float x, float y, float w, float h, float pAngle) const;
	void DrawRoundedPolygon(float x, float y, float pRadius, float pAngle, int pCorners) const;
	void DrawRoundedPolygon(float x, float y, float pRadius, float pAngle, int pCorners, const Color& pColor) const;
	void DrawCircle(float x, float y, float pRadius) const;
	void DrawCircle(float x, float y, float pRadius, const Color& pColor) const;
	void DrawForceMeter(int x, int y, float pAngle, float pForce, bool pSidesAreEqual) const;
	void DrawHealthMeter(int x, int y, float pAngle, float pSize, float pHealth) const;
	void DrawBarrelIndicatorGround(float x, float y, float pAAngle, float pBaseLength, float pBaseWidth) const;
	void DrawBarrelIndicator(float x, float y, float pAngle, float pLength, float pBaseWidth, bool pIsArrow) const;
	void InfoText(int pPlayer, const str& pInfo, float pAngle, float dx = 0, float dy = 0) const;
	void DrawInfoTexts() const;
	void PrintText(const str& pText, float pAngle, int pCenterX, int pCenterY) const;
	void Layout();
	void MainMenu();
	void SuperReset(bool pGameOver);

	virtual void Suspend();
	virtual void Resume();

	bool Steer(UiLepra::InputManager::KeyCode pKeyCode, float pFactor);
	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
#if !defined(LEPRA_IOS) && defined(LEPRA_IOS_LOOKANDFEEL)
	void OnMouseInput(UiLepra::InputElement* pElement);
	bool mIsMouseDown;
#endif // Computer emulating iOS
	virtual int PollTap(FingerMovement& pMovement);

	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	void OnAction(UiTbc::Button* pButton);
	void OnMainMenuAction(UiTbc::Button* pButton);
	void OnLevelAction(UiTbc::Button* pButton);
	void OnVehicleAction(UiTbc::Button* pButton);
	void OnPauseClick(UiTbc::Button*);
	void OnPauseAction(UiTbc::Button* pButton);
	void OnGetiPhoneClick(UiTbc::Button*);
	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);

	typedef void (App::*ButtonAction)(UiTbc::Button*);
	UiTbc::Dialog<App>* CreateTbcDialog(ButtonAction pAction);
	static UiTbc::Button* CreateButton(const str& pText, const Color& pColor, UiTbc::Component* pParent);

	StdioConsoleLogListener mConsoleLogger;
	DebuggerLogListener mDebugLogger;

	static App* mApp;
#ifdef LEPRA_IOS
	AnimatedApp* mAnimatedApp;
#endif // iOS
	Game* mGame;

	double mAverageLoopTime;
	HiResTimer mLoopTimer;

	struct InfoTextData
	{
		str mText;
		Vector2DF mCoord;
		float mAngle;
	};

	bool mIsLoaded;
	bool mDoLayout;
	bool mIsPaused;
	Cure::ResourceManager* mResourceManager;
	Cure::RuntimeVariableScope* mVariableScope;
	UiCure::GameUiManager* mUiManager;
	UiCure::MusicPlayer* mMusicPlayer;
	int mLayoutFrameCounter;
	UiTbc::Button* mLazyButton;
	UiTbc::Button* mHardButton;
	UiTbc::Button* mOriginalButton;
	UiTbc::Button* m1PButton;
	UiTbc::Button* m2PButton;
	UiTbc::Button* mNextButton;
	UiTbc::Button* mResetButton;
	UiTbc::Button* mRetryButton;
	UiTbc::Button* mPauseButton;
	UiTbc::Button* mGetiPhoneButton;
	UiCure::UserPainterKeepImageResource* mScoreHeart;
	UiCure::UserPainterKeepImageResource* mScoreGreyHeart;
#ifndef LEPRA_IOS_LOOKANDFEEL
	UiCure::UserPainterKeepImageResource* mKeyboardButton;
#endif // Computer
	UiCure::UserPainterKeepImageResource* mSteeringWheel;
	mutable Vector3DF mScoreHeartPos[SCORE_POINTS];
	UiTbc::RectComponent* mPlayerSplitter;
	float mAngleTime;
	Color mTouchCenterColor;
	Color mTouchSteerColor;
	Color mTouchShootColor;
	Color mInfoTextColor;
	mutable float mPenX;
	mutable float mPenY;
	typedef std::vector<InfoTextData> InfoTextArray;
	mutable InfoTextArray mInfoTextArray;
	mutable HiResTimer mPlayer1LastTouch;
	mutable HiResTimer mPlayer2LastTouch;
	mutable HiResTimer mPlayer1TouchDelay;
	mutable HiResTimer mPlayer2TouchDelay;
	UiTbc::FontManager::FontId mBigFontId;
	float mReverseAndBrake;
	UiTbc::Dialog<App>::Action mButtonDelegate;
	UiTbc::Dialog<App>* mDialog;
	mutable StopWatch mStartTimer;
	mutable StopWatch mGameOverTimer;
	UiCure::PainterImageResource* mScrollBarImage;
	UiTbc::ScrollBar* mDifficultySlider;
	int mSlowShadowCount;
	float mThrottle;
	float mPreviousSteering;
	float mCurrentSteering;

	LOG_CLASS_DECLARE();
};



struct IsPressing
{
	IsPressing(int pTag): mTag(pTag) {}
	bool operator()(const FingerMovement& pTouch) { return (pTouch.mTag == mTag); }
	int mTag;
};



}



LEPRA_RUN_APPLICATION(GrenadeRun::App, UiLepra::UiMain);



namespace GrenadeRun
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mGame(0),
	mLayoutFrameCounter(-10),
	mVariableScope(0),
	mAverageLoopTime(1.0/FPS),
	mIsLoaded(false),
	mDoLayout(true),
	mIsPaused(false),
	mPauseButton(0),
	mGetiPhoneButton(0),
	mAngleTime(0),
	mBigFontId(UiTbc::FontManager::INVALID_FONTID),
	mReverseAndBrake(0),
	mDialog(0),
	mScrollBarImage(0),
	mDifficultySlider(0),
	mSlowShadowCount(0),
	mThrottle(0),
	mPreviousSteering(0),
	mCurrentSteering(0)
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

bool App::Open()
{
#ifdef LEPRA_IOS
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	const int lDisplayWidth = lSize.height;
	const int lDisplayHeight = lSize.width;
#else // !iOS
#ifdef LEPRA_IOS_LOOKANDFEEL
	const int lDisplayWidth = 480;
	const int lDisplayHeight = 320;
#else // !iOS L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
#endif // iOS / Computer L&F
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
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("Fixed"));

	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));

	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ENABLEVSYNC, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_PIXELSHADERS, false);
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
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.7);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_DOPPLER, 1.0);

#ifndef LEPRA_IOS_LOOKANDFEEL
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_PIXELSHADERS, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.5);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("Force:Volume"));	
#endif // !iOS

	mUiManager = new UiCure::GameUiManager(mVariableScope);
	bool lOk = mUiManager->OpenDraw();
	if (lOk)
	{
		DisplayLogo();
	}
	if (lOk)
	{
		lOk = mUiManager->OpenRest();
	}
	if (lOk)
	{
#ifdef LEPRA_IOS
		mUiManager->GetCanvas()->SetOutputRotation(90);
#endif // iOS
		mUiManager->GetDisplayManager()->SetCaption(_T("Kill Cutie"));
		mUiManager->GetDisplayManager()->AddResizeObserver(this);
		mUiManager->GetInputManager()->AddKeyCodeInputObserver(this);
#if !defined(LEPRA_IOS) && defined(LEPRA_IOS_LOOKANDFEEL)
		mIsMouseDown = false;
		mUiManager->GetInputManager()->GetMouse()->AddFunctor(new UiLepra::TInputFunctor<App>(this, &App::OnMouseInput));
#endif // Computer emulating iOS
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

		mPlayerSplitter = new UiTbc::RectComponent(BLACK, _T("Splitter"));
		lDesktopWindow->AddChild(mPlayerSplitter);

		mPauseButton = ICONBTN("btn_pause.png", "");
		mPauseButton->SetBaseColor(BGCOLOR_DIALOG);
		mPauseButton->SetRoundedStyle(10);
		lDesktopWindow->AddChild(mPauseButton);
		mPauseButton->SetPreferredSize(PixelCoord(44, 44), false);
		mPauseButton->SetSize(mPauseButton->GetPreferredSize());
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(App, OnPauseClick);

#ifndef LEPRA_IOS_LOOKANDFEEL
		mGetiPhoneButton = ICONBTN("btn_iphone.png", "");
		lDesktopWindow->AddChild(mGetiPhoneButton);
		mGetiPhoneButton->SetVisible(true);
		mGetiPhoneButton->SetOnClick(App, OnGetiPhoneClick);
#endif // iOS L&F
	}
	if (lOk)
	{
		mScoreHeart = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mScoreHeart->Load(mResourceManager, _T("heart.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		mScoreGreyHeart = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mScoreGreyHeart->Load(mResourceManager, _T("grey_heart.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
#ifndef LEPRA_IOS_LOOKANDFEEL
		mKeyboardButton = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mKeyboardButton->Load(mResourceManager, _T("btn_key.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
#endif // Computer
		mSteeringWheel = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mSteeringWheel->Load(mResourceManager, _T("steering_wheel.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
	}
	if (lOk)
	{
		UiTbc::FontManager::FontId lDefaultFontId = mUiManager->GetFontManager()->GetActiveFontId();
		mBigFontId = mUiManager->GetFontManager()->QueryAddFont(_T("Helvetica"), 24);
		mUiManager->GetFontManager()->SetActiveFont(lDefaultFontId);
	}
	if (lOk)
	{
		mMusicPlayer = new UiCure::MusicPlayer(mUiManager->GetSoundManager());
		mMusicPlayer->SetVolume(0.5f);
		mMusicPlayer->SetSongPauseTime(6, 18);
		mMusicPlayer->AddSong(_T("ButterflyRide.xm"));
		mMusicPlayer->AddSong(_T("BehindTheFace.xm"));
		mMusicPlayer->AddSong(_T("BrittiskBensin.xm"));
		mMusicPlayer->AddSong(_T("DontYouWantMe'97.xm"));
		mMusicPlayer->AddSong(_T("CloseEncounters.xm"));
		mMusicPlayer->Shuffle();
		lOk = mMusicPlayer->Playback();
	}
	if (lOk)
	{
		struct ResourceOpener: public FileOpener
		{
			Cure::ResourceManager* mResourceManager;

			ResourceOpener(Cure::ResourceManager* pResourceManager):
				mResourceManager(pResourceManager)
			{
			}

			virtual File* Open(const str& pFilename)
			{
				return mResourceManager->QueryFile(pFilename);
			}
		};
		mUiManager->GetSoundManager()->SetFileOpener(new ResourceOpener(mResourceManager));
	}

	UiLepra::Core::ProcessMessages();
	return (lOk);
}

void App::Close()
{
	delete mScrollBarImage;
	mScrollBarImage = 0;

	mUiManager->GetInputManager()->RemoveKeyCodeInputObserver(this);
	mUiManager->GetDisplayManager()->RemoveResizeObserver(this);

	// Poll system to let go of old windows.
	UiLepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	UiLepra::Core::ProcessMessages();

	delete mMusicPlayer;
	mMusicPlayer = 0;

	delete mGame;
	mGame = 0;

	mUiManager->DeleteDesktopWindow();

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
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_PARALLEL, false);	// Let's do it same on all platforms, so we can render stuff from physics data.
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_MICROSTEPS, 2);
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_FPS, FPS);
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_ISFIXEDFPS, true);
		//CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLELIGHTS, false);
	}
	if (lOk)
	{
		const str lPathPrefix = SystemManager::GetDataDirectory(mArgumentVector[0]);
		mResourceManager = new Cure::ResourceManager(1, lPathPrefix);
	}
#ifndef LEPRA_IOS
	HiResTimer lStartupTimer;
#endif // Computer
	if (lOk)
	{
		lOk = Open();
	}
	if (lOk)
	{
		mGame = new Game(mUiManager, mVariableScope, mResourceManager);
		mGame->SetComputerDifficulty(0.5f);
		mGame->SetComputerIndex(1);
		lOk = mGame->SetLevelName(_T("level_2"));
	}
	if (lOk)
	{
		mGame->Cure::GameTicker::GetTimeManager()->Tick();
		mGame->Cure::GameTicker::GetTimeManager()->Clear(1);
		MainMenu();
		lOk = mResourceManager->InitDefault();
	}
	mLoopTimer.PopTimeDiff();
#ifndef LEPRA_IOS
	while (2.0 - lStartupTimer.QueryTimeDiff() > 0)
	{
		if (SystemManager::GetQuitRequest())
		{
			break;
		}
		Thread::Sleep(0.1);
		UiLepra::Core::ProcessMessages();
	}
	bool lQuit = (SystemManager::GetQuitRequest() != 0);
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

void App::DisplayLogo()
{
	UiCure::PainterImageResource* lLogo = new UiCure::PainterImageResource(mUiManager, mResourceManager, _T("logo.png"), UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
	if (lLogo->Load())
	{
		if (lLogo->PostProcess() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			//mUiManager->BeginRender(Vector3DF(0, 1, 0));
			mUiManager->PreparePaint(true);
			const Canvas* lCanvas = mUiManager->GetCanvas();
			const Canvas* lImage = lLogo->GetRamData();
			mUiManager->GetPainter()->DrawImage(lLogo->GetUserData(0), lCanvas->GetWidth()/2 - lImage->GetWidth()/2, lCanvas->GetHeight()/2 - lImage->GetHeight()/2);
			mUiManager->GetDisplayManager()->UpdateScreen();
		}
	}
	delete lLogo;
}

bool App::Poll()
{
	bool lOk = true;
	if (lOk)
	{
		// Adjust frame rate, or it will be hopelessly high... on most reasonable platforms.
		mAverageLoopTime = Lepra::Math::Lerp(mAverageLoopTime, mLoopTimer.QueryTimeDiff(), 0.05);
		const double lDelayTime = 1.0/FPS - mAverageLoopTime;
		Thread::Sleep(lDelayTime);
		mLoopTimer.PopTimeDiff();
#ifndef LEPRA_IOS_LOOKANDFEEL
		if (mAverageLoopTime > 1.0/(FPS-2))
		{
			if (++mSlowShadowCount > 20)
			{
				// Shadows is not such a good idea on this computer.
				CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("None"));
			}
		}
#endif // !iOS
	}
	if (lOk)
	{
		// Take care of the "brake and reverse" steering.
		if (mGame->GetCutie() && mGame->GetCutie()->IsLoaded())
		{
			const bool lIsMovingForward = (mGame->GetCutie()->GetForwardSpeed() > 2.0f);
			if (mReverseAndBrake)
			{
				mGame->GetCutie()->SetEnginePower(0, lIsMovingForward? 0 : -1*mReverseAndBrake, 0);	// Reverse.
				mGame->GetCutie()->SetEnginePower(2, lIsMovingForward? +1*mReverseAndBrake : 0, 0);	// Brake.
			}
		}
	}
	if (lOk)
	{
		mAngleTime += 1.0f/FPS/10;
		mAngleTime -= (mAngleTime > 2*PIF)? 2*PIF : 0;
		lOk = (SystemManager::GetQuitRequest() == 0);
	}
	if (!mIsLoaded && mResourceManager->IsLoading())
	{
		mResourceManager->Tick();
		return lOk;
	}
	mResourceManager->ForceFreeCache();
	mIsLoaded = true;
	if (lOk)
	{
		const bool lShowPause = !mIsPaused && (mGame->GetFlybyMode() == Game::FLYBY_INACTIVE);
		mPauseButton->SetVisible(lShowPause);
	}
	if (lOk && mDoLayout)
	{
		Layout();
	}
	if (lOk)
	{
		if (mDifficultySlider)
		{
			mGame->SetComputerDifficulty((float)mDifficultySlider->GetScrollPos());
		}
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
		PollTaps();
	}
	const bool lTRICKY_IsLoopPaused = mIsPaused;
	if (lOk && !lTRICKY_IsLoopPaused)
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
		if (mDialog)
		{
			mUiManager->PreparePaint(false);
		}
		else
		{
			mUiManager->Paint(false);
		}
		mTouchCenterColor = Color(102, 120, 190)*(1.1f+::sin(mAngleTime*40)*0.2f);
		mTouchSteerColor = Color(102, 120, 190)*(1.0f+::sin(mAngleTime*41)*0.3f);
		mTouchShootColor = Color(170, 38, 45)*(1.1f+::sin(mAngleTime*37)*0.2f);
		mInfoTextColor = Color(127, 127, 127)*(1+::sin(mAngleTime*27)*0.9f);
		DrawHud();

#ifdef LEPRA_DEBUG
		mUiManager->GetPainter()->SetColor(WHITE);
		mUiManager->GetPainter()->PrintText(strutil::Format(_T("%.1f"), 1.0/mAverageLoopTime), 10, 10);
#endif // Debug

		mGame->Paint();

		if (mDialog)
		{
			mUiManager->Paint(false);
		}
	}
	if (lOk && !lTRICKY_IsLoopPaused)
	{
		lOk = mGame->EndTick();
	}
	if (lOk && !lTRICKY_IsLoopPaused)
	{
		lOk = mGame->Tick();
	}
	mResourceManager->Tick();
	mUiManager->EndRender();

	if (mMusicPlayer)
	{
		mMusicPlayer->Update();
	}

	if (mGameOverTimer.IsStarted())
	{
		if (mGameOverTimer.QueryTimeDiff() > 20.0 ||
			(mGame->GetComputerIndex() == 1 && mGame->GetCutie()->GetHealth() <= 0 && mGameOverTimer.QueryTimeDiff() > 5.0) ||
			(mGame->GetComputerIndex() == 0 && mGameOverTimer.QueryTimeDiff() > 10.0))
		{
			SuperReset(true);
		}
	}

	return lOk;
}

void App::PollTaps()
{
#ifdef LEPRA_IOS_LOOKANDFEEL
	UiCure::CppContextObject* lAvatar1 = mGame->GetP1();
	UiCure::CppContextObject* lAvatar2 = mGame->GetP2();
	if (!lAvatar1 || !lAvatar1->IsLoaded() || !lAvatar2 ||!lAvatar2->IsLoaded())
	{
		return;
	}
	if (lAvatar1->GetPhysics()->GetEngineCount() >= 3)
	{
		mGame->SetThrottle(lAvatar1, mThrottle);
		mPreviousSteering = Math::Clamp(mPreviousSteering, -1.0f, 1.0f);
		lAvatar1->SetEnginePower(1, mPreviousSteering+mCurrentSteering, 0);
	}
	mGame->SetThrottle(lAvatar2, 0);
	lAvatar2->SetEnginePower(1, 0, 0);
	FingerMoveList::iterator x = gFingerMoveList.begin();
	for (; x != gFingerMoveList.end();)
	{
		if (PollTap(*x) > 0)
		{
			++x;
		}
		else
		{
			gFingerMoveList.erase(x++);
		}
	}
#ifndef LEPRA_IOS
	// Test code.
	if (!gFingerMoveList.empty() && !gFingerMoveList.back().mIsPress)
	{
		gFingerMoveList.clear();
	}
#endif // Computer
#endif // iOS L&F
}

void App::DrawHud()
{
	if (mGame->GetFlybyMode() != Game::FLYBY_INACTIVE)
	{
		return;
	}

	Cure::ContextObject* lAvatar1 = mGame->GetP1();
	Cure::ContextObject* lAvatar2 = mGame->GetP2();
	if (!lAvatar1 || !lAvatar1->IsLoaded() || !lAvatar2 || !lAvatar2->IsLoaded())
	{
		return;
	}

	const float lButtonWidth = BUTTON_WIDTH;	// TODO: fix for Retina.
	const float lButtonRadius = lButtonWidth/2;
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();
	const float m = BUTTON_MARGIN;

	const int lWinner = mGame->GetWinnerIndex();
	if (lWinner >= 0)
	{
#ifdef LEPRA_IOS_LOOKANDFEEL
		const float lAngle = (mGame->GetComputerIndex() != 1)? PIF/2 : 0;
#else // Computer.
		const float lAngle = 0;
#endif // iOS / computer
		UiTbc::FontManager::FontId lFontId = mUiManager->GetFontManager()->GetActiveFontId();
		mUiManager->GetFontManager()->SetActiveFont(mBigFontId);
		const bool lGameOver = (mGame->GetScoreBalance() == -SCORE_POINTS/2 || mGame->GetScoreBalance() == +SCORE_POINTS/2);
		const str lWon = lGameOver? _T("You rule!") : _T("Won heart");
		const str lLost = lGameOver? _T("Defeat!") : _T("Lost heart");
		const float lBackgroundSize = 110;
		if (mGame->GetComputerIndex() < 0)
		{
			str lText1;
			str lText2;
			Color lColor1;
			Color lColor2;
			if (lWinner == 0)
			{
				lText1 = lWon;
				lText2 = lLost;
				lColor1 = LIGHT_GREEN;
				lColor2 = LIGHT_RED;
			}
			else if (lWinner == 1)
			{
				lText1 = lLost;
				lText2 = lWon;
				lColor1 = LIGHT_RED;
				lColor2 = LIGHT_GREEN;
			}
			const int x1 = (int)(w*1/4);
			const int x2 = (int)(w*3/4);
			const int y  = (int)(h/2);
			mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
			DrawRoundedPolygon((float)x1, (float)y, lBackgroundSize, PIF/4, 4, BGCOLOR_DIALOG);
			DrawRoundedPolygon((float)x2, (float)y, lBackgroundSize, PIF/4, 4, BGCOLOR_DIALOG);
			//mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_NORMAL);
			mUiManager->GetPainter()->SetColor(lColor1, 0);
			PrintText(lText1, +lAngle, x1, y);
			mUiManager->GetPainter()->SetColor(lColor2, 0);
			PrintText(lText2, -lAngle, x2, y);
		}
		else
		{
			const int x = (int)(w/2);
			const int y  = (int)(h/2);
			mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
			DrawRoundedPolygon((float)x, (float)y, lBackgroundSize, PIF/4, 4, BGCOLOR_DIALOG);
			//mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_NORMAL);
#ifdef LEPRA_IOS_LOOKANDFEEL
				const float a = (mGame->GetComputerIndex() == 0)? -lAngle : 0;
#else // Computer L&F
				const float a = 0;
#endif // iOS / Computer L&F
			if (lWinner != mGame->GetComputerIndex())
			{
				mUiManager->GetPainter()->SetColor(LIGHT_GREEN, 0);
				PrintText(lWon, a, x, y);
			}
			else
			{
				mUiManager->GetPainter()->SetColor(LIGHT_RED, 0);
				PrintText(lLost, a, x, y);
			}
		}
		mUiManager->GetFontManager()->SetActiveFont(lFontId);

		mGameOverTimer.TryStart();
	}

#ifdef LEPRA_IOS_LOOKANDFEEL
	if (mGame->GetComputerIndex() != 0 &&
		mSteeringWheel->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
		lAvatar1->GetPhysics()->GetEngineCount() >= 3)
	{
		// Left player.
		if (mGame->GetComputerIndex() < 0)	// 2P?
		{
			DrawCircle(m+lButtonWidth,			m+lButtonRadius,	lButtonRadius-2);	// Up/down.
			InfoText(1, _T("Throttle/brake"), 0, 14, 0);
			DrawRoundedPolygon(m*2+lButtonWidth*1.6f,	m+lButtonRadius,	lButtonRadius*0.5f,	+PIF/2,	3);
			DrawRoundedPolygon(lButtonWidth*0.4f,		m+lButtonRadius,	lButtonRadius*0.5f,	-PIF/2,	3);

			const float s = h * 0.25f;
			const TBC::PhysicsEngine* lGas = lAvatar1->GetPhysics()->GetEngine(1);
			const float a = lGas->GetLerpThrottle(0.2f, 0.2f) * -1.5f - PIF/2;
			DrawImage(mSteeringWheel->GetData(), s*0.15f, h-s*0.3f, s, s, a);
			InfoText(1, _T("Left/right"), -PIF/2, 0, -10);
		}
		else
		{
			DrawCircle(m+lButtonRadius,			h-m-lButtonWidth,		lButtonRadius-2);	// Up/down.
			InfoText(1, _T("Throttle/brake"), PIF/2, 0, -14);
			DrawRoundedPolygon(m+lButtonRadius,		h-m*2-lButtonWidth*1.6f,	lButtonRadius*0.5f,	0,	3);
			DrawRoundedPolygon(m+lButtonRadius,		h-lButtonWidth*0.4f,		lButtonRadius*0.5f,	+PIF,	3);

			const float s = w * 0.25f;
			const TBC::PhysicsEngine* lGas = lAvatar1->GetPhysics()->GetEngine(1);
			const float a = lGas->GetLerpThrottle(0.2f, 0.2f) * -1.5f;
			DrawImage(mSteeringWheel->GetData(), w-s*0.3f, h-s*0.15f, s, s, a);
			InfoText(1, _T("Left/right"), 0, -10, 0);
		}
	}
	if (mGame->GetComputerIndex() != 1)
	{
		// Right player.
		DrawCircle(w-m-lButtonWidth,			h-m-lButtonRadius,	lButtonRadius-2);	// Up/down.
		InfoText(2, _T("Up/down"), PIF);
		DrawRoundedPolygon(w-m*2-lButtonWidth*1.6f,	h-m-lButtonRadius,	lButtonRadius*0.5f,	-PIF/2,	3);
		DrawRoundedPolygon(w-lButtonWidth*0.4f,		h-m-lButtonRadius,	lButtonRadius*0.5f,	+PIF/2,	3);
		DrawCircle(w-m-lButtonRadius,			m+lButtonWidth,		lButtonRadius-2);	// Left/right.
		InfoText(2, _T("Left/right"), PIF/2);
		DrawRoundedPolygon(w-m-lButtonRadius,		m*2+lButtonWidth*1.6f,	lButtonRadius*0.5f,	PIF,	3);
		DrawRoundedPolygon(w-m-lButtonRadius,		lButtonWidth*0.4f,	lButtonRadius*0.5f,	0,	3);
		// Bomb button.
		bool lIsLocked = mGame->IsLauncherLocked();
		Color c = lIsLocked? Color(10, 10, 10) : mTouchShootColor;
		DrawRoundedPolygon(w-m-lButtonRadius,		h/2,			lButtonRadius,	-PIF/2,	6, c);
		InfoText(2, _T("BOOOM!"), PIF/2);
	}
#endif // iOS

	// Draw touch force meters, to give a visual indication of steering.

	Cutie* lCutie = (Cutie*)lAvatar1;
	if (mGame->GetComputerIndex() < 0)	// Two players.
	{
#ifdef LEPRA_IOS_LOOKANDFEEL
		DrawHealthMeter((int)w/2, (int)h/2, PIF, h/2, lCutie->GetHealth());
#else // !iOS
		DrawHealthMeter((int)w/4, (int)(lButtonWidth*0.7f), -PIF/2, w/3, lCutie->GetHealth());
#endif // iOS/!iOS
	}
	else if (mGame->GetComputerIndex() != 0)	// Single player Cutie.
	{
		DrawHealthMeter((int)w/2, (int)(lButtonWidth*0.7f), -PIF/2, w/3, lCutie->GetHealth());
	}

#ifdef LEPRA_IOS_LOOKANDFEEL
	float lForce;
	const TBC::PhysicsEngine* lGas;
	const TBC::PhysicsEngine* lBrakes;
	const TBC::PhysicsEngine* lTurn;
	if (lAvatar1->GetPhysics()->GetEngineCount() >= 3)
	{
		if (mGame->GetComputerIndex() != 0)
		{
			lGas = lAvatar1->GetPhysics()->GetEngine(0);
			lBrakes = lAvatar1->GetPhysics()->GetEngine(2);
			lForce = lGas->GetValue() - lBrakes->GetValue();
			if (lForce != 0 || std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(1)) != gFingerMoveList.end())
			{
				if (mGame->GetComputerIndex() < 0)	// 2P?
				{
					DrawForceMeter((int)(m*2+lButtonWidth*4), (int)(m+lButtonRadius), -PIF/2, lForce, false);
					InfoText(1, _T("Acceleration"), 0);
				}
				else
				{
					DrawForceMeter((int)(m+lButtonRadius), (int)(h-m*2-lButtonWidth*4), 0, lForce, false);
					InfoText(1, _T("Acceleration"), +PIF/2);
				}
			}
			/*lTurn = lAvatar1->GetPhysics()->GetEngine(1);
			lForce = lTurn->GetValue();
			if (lForce != 0 || std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(2)) != gFingerMoveList.end())
			{
				if (mGame->GetComputerIndex() < 0)	// 2P?
				{
					DrawForceMeter((int)(m*2+lButtonWidth*4), (int)(h-m-lButtonWidth), PIF, lForce, true);
					InfoText(1, _T("Steering wheel"), -PIF/2, 0, -20);
				}
				else
				{
					DrawForceMeter((int)(w-m-lButtonWidth), (int)(h-m*2-lButtonWidth*4), -PIF/2, lForce, true);
					InfoText(1, _T("Steering wheel"), 0, -20, 0);
				}
			}*/
		}
	}
	if (mGame->GetComputerIndex() != 1 && lAvatar2->GetPhysics()->GetEngineCount() >= 2)
	{
		lGas = lAvatar2->GetPhysics()->GetEngine(0);
		lForce = lGas->GetValue();
		if (lForce != 0 || std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(3)) != gFingerMoveList.end())
		{
			DrawForceMeter((int)(w-m*2-lButtonWidth*4), (int)(h-m*1.5f-lButtonRadius), -PIF/2, lForce, true);
			InfoText(2, _T("Lift power"), PIF);
		}
		lTurn = lAvatar2->GetPhysics()->GetEngine(1);
		lForce = lTurn->GetValue();
		if (lForce != 0 || std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(4)) != gFingerMoveList.end())
		{
			DrawForceMeter((int)(w-m*2-lButtonWidth*4), (int)(m*1.5f+lButtonWidth), 0, lForce, true);
			InfoText(2, _T("Turn power"), PIF/2);
		}
	}
#endif // iOS

	// Draw launcher guides
	float lDrawAngle = 0;
	float lPitch;
	float lGuidePitch;
	float lYaw;
	float lGuideYaw;
	mGame->GetLauncher()->GetAngles(mGame->GetCutie(), lPitch, lGuidePitch, lYaw, lGuideYaw);
	if (mGame->GetComputerIndex() != 1)	// Computer not running launcher.
	{
		float x;
		float y;
#ifdef LEPRA_IOS_LOOKANDFEEL
		x = w-m*1.5f-lButtonWidth/2;
		y = h-m*2-lButtonWidth-8;
#else // !iOS
		if (mGame->GetComputerIndex() < 0)	// Two players.
		{
			lDrawAngle = -PIF/2;
			x = w/2+m+lButtonWidth;
			y = h-m*2;
		}
		else	// Single player launcher.
		{
			lDrawAngle = -PIF/2;
			x = +m+lButtonWidth;
			y = h-m*2;
		}
#endif // iOS/!iOS
		mUiManager->GetPainter()->SetColor(Color(60, 40, 20), 0);
		DrawBarrelIndicatorGround(x, y, lDrawAngle, lButtonWidth*0.4f, 8);
		mUiManager->GetPainter()->SetColor(Color(150, 20, 20), 0);
		DrawBarrelIndicator(x, y, lGuidePitch+lDrawAngle, 1.1f, 3.0f, true);
		mUiManager->GetPainter()->SetColor(Color(220, 210, 200), 0);
		DrawBarrelIndicator(x, y, lGuidePitch+lDrawAngle, 0.9f, 1.4f, true);
		mUiManager->GetPainter()->SetColor(Color(140, 140, 140), 0);
		DrawBarrelIndicator(x, y, lPitch+lDrawAngle, 1, 1, false);
		InfoText(2, _T("Up/down compass"), PIF, -20-lButtonRadius, -lButtonRadius/2);
	}

	if (mGame->GetComputerIndex() != 1)	// Computer not running launcher.
	{
		float x;
		float y;
#ifdef LEPRA_IOS_LOOKANDFEEL
		x = w-m*2-lButtonWidth-8;
		y = m*1.5f+lButtonWidth;
#else // !iOS
		lDrawAngle = -PIF/2;
		x = w-m-lButtonWidth;
		y = h-m*2;
#endif // iOS/!iOS
		mUiManager->GetPainter()->SetColor(Color(150, 20, 20), 0);
		DrawBarrelIndicator(x, y, lGuideYaw+lDrawAngle, 1.1f, 3.0f, true);
		mUiManager->GetPainter()->SetColor(Color(220, 210, 200), 0);
		DrawBarrelIndicator(x, y, lGuideYaw+lDrawAngle, 0.9f, 1.4f, true);
		mUiManager->GetPainter()->SetColor(Color(140, 140, 140), 0);
		DrawBarrelIndicator(x, y, lYaw+lDrawAngle, 1, 1, false);
		InfoText(2, _T("Left/right compass"), PIF/2, -lButtonRadius, 30);
	}

	if (mScoreHeart->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
		mScoreGreyHeart->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
		mUiManager->GetPainter()->SetAlphaValue(255);
		const float lMargin = 8;
		float x = lMargin;
		float y = BUTTON_WIDTH + 8*2 + 3;
		float lAngle = 0;
		const int lBalance = -mGame->GetScoreBalance();
		static float lHeartBeatTime = 0;
		lHeartBeatTime += 0.5f;
		if (lHeartBeatTime > 4*PIF)
		{
			lHeartBeatTime -= 4*PIF;
		}
		const float lHeartBeatFactor = (lHeartBeatTime < 2*PIF) ? 1 : 1+0.15f*sin(lHeartBeatTime);
		const float iw = (float)mScoreHeart->GetRamData()->GetWidth();
		const float ih = (float)mScoreHeart->GetRamData()->GetHeight();
		const float hw = iw * lHeartBeatFactor;
		const float hh = ih * lHeartBeatFactor;
#ifdef LEPRA_IOS_LOOKANDFEEL
		if (mGame->GetComputerIndex() == -1)
		{
			x = w/2 - BUTTON_WIDTH/2 - 8 - 3 - ih;
			lAngle = -PIF/2;
		}
#endif	// iOS L&F
		if (mGame->GetComputerIndex() == -1)
		{
			y = lMargin;
		}
		int lHeartIndex = 0;
		for (int i = -SCORE_POINTS/2; i < +SCORE_POINTS/2; ++i)
		{
			if (mGame->GetComputerIndex() != 0)
			{
				Vector3DF v(x+iw/2, y+iw/2, lAngle);
				if (i < lBalance)
				{
					v = mScoreHeartPos[lHeartIndex] = Math::Lerp(mScoreHeartPos[lHeartIndex], v, 0.07f);
					++lHeartIndex;
					DrawImage(mScoreHeart->GetData(), v.x, v.y, hw, hh, v.z);
				}
				else
				{
					DrawImage(mScoreGreyHeart->GetData(), v.x, v.y, iw, ih, v.z);
				}
				y += iw+8;
			}
		}
		x = w - lMargin - iw;
		y = BUTTON_WIDTH + 8*2 + 3;
#ifdef LEPRA_IOS_LOOKANDFEEL
		if (mGame->GetComputerIndex() == -1)
		{
			x = w/2 + BUTTON_WIDTH/2 + lMargin;
			y = lMargin;
		}
		else
		{
			x = lMargin;
			y = BUTTON_WIDTH + lMargin*2;
		}
		lAngle = PIF/2;
#endif	// iOS L&F
		if (mGame->GetComputerIndex() == -1)
		{
			y = lMargin;
		}
		y += (iw+8) * (SCORE_POINTS - 1);
		for (int i = -SCORE_POINTS/2; i < +SCORE_POINTS/2; ++i)
		{
			if (mGame->GetComputerIndex() != 1)
			{
				Vector3DF v(x+iw/2, y+iw/2, lAngle);
				if (i >= lBalance)
				{
					v = mScoreHeartPos[lHeartIndex] = Math::Lerp(mScoreHeartPos[lHeartIndex], v, 0.07f);
					++lHeartIndex;
					DrawImage(mScoreHeart->GetData(), v.x, v.y, hw, hh, v.z);
				}
				else
				{
					DrawImage(mScoreGreyHeart->GetData(), v.x, v.y, iw, ih, v.z);
				}
				y -= iw+8;
			}
		}
		//mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_NORMAL);
	}

#ifndef LEPRA_IOS_LOOKANDFEEL
	if (mGame->GetFlybyMode() == Game::FLYBY_INACTIVE && !mStartTimer.IsStarted())
	{
		mStartTimer.Start();
	}
	if (mStartTimer.IsStarted() && mStartTimer.QueryTimeDiff() < 7.0 &&
		mKeyboardButton->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetPainter()->SetColor(GRAY);
		const int sw = (int)w;
		const int sh = (int)h;
		const int m = BUTTON_MARGIN;
		const int iw = mKeyboardButton->GetRamData()->GetWidth();
		const int ih = mKeyboardButton->GetRamData()->GetHeight();
		int bx = sw/2;
		const int by = sh/2 - ih/3;
		if (mGame->GetComputerIndex() == -1)
		{
			bx = sw/4;
			mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*3/2-m, by-ih-m);	// w
			mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw/2-m*0, by-ih-m);	// e
			mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*2-m*2, by);	// a
			mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*1-m*1, by);	// s
			mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*0-m*0, by);	// d
			PrintText(_T("W"), 0, bx-iw-m*3, by-ih/2-m*2);
			PrintText(_T("E"), 0, bx-m*2, by-ih/2-m*2);
			PrintText(_T("A"), 0, bx-iw*3/2-m*4, by+ih/2-m*2);
			PrintText(_T("S"), 0, bx-iw/2-m*3, by+ih/2-m*2);
			PrintText(_T("D"), 0, bx+iw/2-m*2, by+ih/2-m*2);
			bx = sw*3/4;
		}
		bx += iw*3/2;
		mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*1-m*1, by-ih-m);
		mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*4+m*0, by);
		mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*2-m*2, by);
		mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*1-m*1, by);
		mUiManager->GetPainter()->DrawImage(mKeyboardButton->GetData(), bx-iw*0-m*0, by);
		PrintText(_T("Up"),	0, bx-iw/2-m*3, by-ih/2-m*2);
		PrintText(_T("Ctrl"),	0, bx-iw*7/2-m*2, by+ih/2-m*2);
		PrintText(_T("L"),	0, bx-iw*3/2-m*4, by+ih/2-m*2);
		PrintText(_T("Dn"),	0, bx-iw/2-m*3, by+ih/2-m*2);
		PrintText(_T("R"),	0, bx+iw/2-m*2, by+ih/2-m*2);
	}
#endif // Computer

	DrawInfoTexts();
}

void App::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
{
	mPenX = cx;
	mPenY = cy;
	const float ca = ::cos(pAngle);
	const float sa = ::sin(pAngle);
	const float w2 = w*0.5f;
	const float h2 = h*0.5f;
	const float x = cx - w2*ca - h2*sa;
	const float y = cy - h2*ca + w2*sa;
	const Vector2DF c[] = { Vector2DF(x, y), Vector2DF(x+w*ca, y-w*sa), Vector2DF(x+w*ca+h*sa, y+h*ca-w*sa), Vector2DF(x+h*sa, y+h*ca) };
	const Vector2DF t[] = { Vector2DF(0, 0), Vector2DF(1, 0), Vector2DF(1, 1), Vector2DF(0, 1) };
#define V(z) std::vector<Vector2DF>(z, z+LEPRA_ARRAY_COUNT(z))
	mUiManager->GetPainter()->DrawImageFan(pImageId, V(c), V(t));
}

void App::DrawRoundedPolygon(float x, float y, float pRadius, float pAngle, int pCorners) const
{
	DrawRoundedPolygon(x, y, pRadius, pAngle, pCorners, mTouchSteerColor);
}

void App::DrawRoundedPolygon(float x, float y, float pRadius, float pAngle, int pCorners, const Color& pColor) const
{
	mPenX = x;
	mPenY = y;
	//pRadius -= 2;
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
	mUiManager->GetPainter()->SetColor(pColor, 0);
	mUiManager->GetPainter()->SetAlphaValue(pColor.mAlpha);
	mUiManager->GetPainter()->DrawFan(lCoords, true);
	mUiManager->GetPainter()->SetAlphaValue(255);
	/*const Vector2DF lCenter(x, y);
	for (size_t i = 0; i < lCoords.size()-1; ++i)
	{
		const Vector2DF c = lCoords[i+1]-lCenter;
		lCoords[i] = c*44.0f/40 + lCenter;
	}
	lCoords.pop_back();
	::glLineWidth(2);
	mUiManager->GetPainter()->SetColor(Color(170, 180, 190), 0);
	mUiManager->GetPainter()->DrawFan(lCoords, false);*/
}

void App::DrawCircle(float x, float y, float pRadius) const
{
	DrawCircle(x, y, pRadius, mTouchCenterColor);
}

void App::DrawCircle(float x, float y, float pRadius, const Color& pColor) const
{
	mPenX = x;
	mPenY = y;
	std::vector<Vector2DF> lCoords;
	lCoords.push_back(Vector2DF(x, y));
	for (float lAngle = 0; lAngle < 2*PIF; lAngle += 2*PIF/16)
	{
		lCoords.push_back(Vector2DF(x+pRadius*::sin(lAngle), y-pRadius*::cos(lAngle)));
	}
	lCoords.push_back(lCoords[1]);
	mUiManager->GetPainter()->SetColor(pColor, 0);
	mUiManager->GetPainter()->DrawFan(lCoords, true);
}

void App::DrawForceMeter(int x, int y, float pAngle, float pForce, bool pSidesAreEqual) const
{
	mPenX = (float)x;
	mPenY = (float)y;

	Color lColor = YELLOW;
	Color lTargetColor = RED;
	if (pForce < 0)
	{
		pForce = -pForce;
		pAngle += PIF;
		if (!pSidesAreEqual)
		{
			lTargetColor = BLUE;
		}
	}
	const int lBarCount = 5;
	const int lBarHeight = BUTTON_WIDTH/lBarCount/2;
	const int lBarWidth = BUTTON_WIDTH;
	const float lForceStep = 1.0f/lBarCount - MathTraits<float>::FullEps();
	float lCurrentForce = 0;
	const int lXStep = -(int)(::sin(pAngle)*lBarHeight*2);
	const int lYStep = -(int)(::cos(pAngle)*lBarHeight*2);
	const bool lXIsMain = ::abs(lXStep) >= ::abs(lYStep);
	const int lStartCount = -lBarCount+1;
	x += lXStep * lStartCount;
	y += lYStep * lStartCount;
	for (int i = lStartCount; i < lBarCount; ++i)
	{
		const Color c = (i >= 0 && lCurrentForce <= pForce)? Color(lColor, lTargetColor, lCurrentForce) : DARK_GRAY;
		mUiManager->GetPainter()->SetColor(c);
		if (lXIsMain)
		{
			mUiManager->GetPainter()->FillRect(x, y-lBarWidth/2, x+lBarHeight, y+lBarWidth/2);
		}
		else
		{
			mUiManager->GetPainter()->FillRect(x-lBarWidth/2, y, x+lBarWidth/2, y+lBarHeight);
		}
		x += lXStep;
		y += lYStep;
		if (i >= 0)
		{
			lCurrentForce += lForceStep;
		}
	}
}

void App::DrawHealthMeter(int x, int y, float pAngle, float pSize, float pHealth) const
{
	Color lStartColor = RED;
	Color lEndColor = GREEN;
	const int lBarCount = 19;
	const int lBarSpace = 3;
	const int lBarHeight = (int)(pSize/lBarCount) - lBarSpace;
	const int lBarWidth = BUTTON_WIDTH;
	const float lHealthStep = 1.0f/lBarCount - 0.0001f*lBarCount;
	float lCurrentHealth = 0;
	const int lXStep = -(int)(::sin(pAngle)*(lBarHeight+lBarSpace));
	const int lYStep = -(int)(::cos(pAngle)*(lBarHeight+lBarSpace));
	const bool lXIsMain = ::abs(lXStep) >= ::abs(lYStep);
	x -= (int)(lXStep * lBarCount*0.5f);
	y -= (int)(lYStep * lBarCount*0.5f);
	for (int i = 0; i < lBarCount; ++i)
	{
		const Color c = (lCurrentHealth < pHealth)? Color(lStartColor, lEndColor, lCurrentHealth) : DARK_GRAY;
		mUiManager->GetPainter()->SetColor(c);
		if (lXIsMain)
		{
			mUiManager->GetPainter()->FillRect(x, y-lBarWidth/2, x+lBarHeight, y+lBarWidth/2);
		}
		else
		{
			mUiManager->GetPainter()->FillRect(x-lBarWidth/2, y, x+lBarWidth/2, y+lBarHeight);
		}
		x += lXStep;
		y += lYStep;
		lCurrentHealth += lHealthStep;
	}
}

void App::DrawBarrelIndicatorGround(float x, float y, float pAngle, float pBaseLength, float pBaseWidth) const
{
	const float sa = ::sin(pAngle);
	const float ca = ::cos(pAngle);
	x += 3*ca - 3*sa;
	y -= 2*ca + 2*sa;
	std::vector<Vector2DF> lCoords;
	lCoords.push_back(Vector2DF(x-ca*pBaseWidth+sa*pBaseLength, y+ca*pBaseLength-sa*pBaseWidth));
	lCoords.push_back(Vector2DF(x-ca*pBaseWidth-sa*pBaseLength, y-ca*pBaseLength-sa*pBaseWidth));
	lCoords.push_back(Vector2DF(x+ca*pBaseWidth-sa*pBaseLength, y-ca*pBaseLength+sa*pBaseWidth));
	lCoords.push_back(Vector2DF(x+ca*pBaseWidth+sa*pBaseLength, y+ca*pBaseLength+sa*pBaseWidth));
	lCoords.push_back(lCoords[0]);
	mUiManager->GetPainter()->DrawFan(lCoords, true);
}

void App::DrawBarrelIndicator(float x, float y, float pAngle, float pLength, float pBaseWidth, bool pIsArrow) const
{
	mPenX = x;
	mPenY = y;

	const float lIndicatorLength = BUTTON_WIDTH * pLength;
	const float ca = ::cos(pAngle);
	const float sa = ::sin(pAngle);
	const float lWidth = BUTTON_WIDTH/10/2 * pBaseWidth;
	std::vector<Vector2DF> lCoords;
	lCoords.push_back(Vector2DF(x-sa*lWidth, y-ca*lWidth));
	lCoords.push_back(Vector2DF(x+sa*lWidth, y+ca*lWidth));
	lCoords.push_back(Vector2DF(x+sa*lWidth-lIndicatorLength*ca, y+ca*lWidth+lIndicatorLength*sa));
	if (pIsArrow)
	{
		lCoords.push_back(Vector2DF(x-(lIndicatorLength+pBaseWidth*1.4f)*ca, y+(lIndicatorLength+pBaseWidth*1.4f)*sa));
	}
	lCoords.push_back(Vector2DF(x-sa*lWidth-lIndicatorLength*ca, y-ca*lWidth+lIndicatorLength*sa));
	mUiManager->GetPainter()->DrawFan(lCoords, true);
}

void App::InfoText(int pPlayer, const str& pInfo, const float pAngle, float dx, float dy) const
{
	const double lLastTime = (pPlayer == 1)? mPlayer1LastTouch.QueryTimeDiff() : mPlayer2LastTouch.QueryTimeDiff();
	const double lShowDelayTime = (pPlayer == 1)? mPlayer1TouchDelay.QueryTimeDiff() : mPlayer2TouchDelay.QueryTimeDiff();
	if (lLastTime < 20)	// Delay until shown.
	{
		if (lShowDelayTime > 3)	// Delay after next touch until hidden.
		{
			return;
		}
	}
	else
	{
		(pPlayer == 1)? mPlayer1TouchDelay.ClearTimeDiff() : mPlayer2TouchDelay.ClearTimeDiff();
	}
	InfoTextData lData;
	lData.mText = pInfo;
	lData.mCoord = Vector2DF(mPenX+dx, mPenY+dy);
	lData.mAngle = pAngle;
	mInfoTextArray.push_back(lData);
}

void App::DrawInfoTexts() const
{
#ifdef LEPRA_IOS_LOOKANDFEEL
	const Color c = mUiManager->GetPainter()->GetColor(0);
	mUiManager->GetPainter()->SetColor(mInfoTextColor, 0);

	for (size_t x = 0; x < mInfoTextArray.size(); ++x)
	{
		const InfoTextData& lData = mInfoTextArray[x];
		PrintText(lData.mText, -lData.mAngle, (int)lData.mCoord.x, (int)lData.mCoord.y);
	}

	mUiManager->GetPainter()->SetColor(c, 0);
#endif // iOS

	mInfoTextArray.clear();
}

void App::PrintText(const str& pText, float pAngle, int pCenterX, int pCenterY) const
{
	if (pAngle)
	{
		::glMatrixMode(GL_PROJECTION);
		::glPushMatrix();
		::glRotatef(pAngle*180/PIF, 0, 0, 1);
	}
	const int cx = (int)(pCenterX*cos(pAngle) + pCenterY*sin(pAngle));
	const int cy = (int)(pCenterY*cos(pAngle) - pCenterX*sin(pAngle));
	const int w = mUiManager->GetPainter()->GetStringWidth(pText);
	const int h = mUiManager->GetPainter()->GetFontHeight();
	mUiManager->GetPainter()->PrintText(pText, cx-w/2, cy-h/2);
	if (pAngle)
	{
		::glPopMatrix();
		::glMatrixMode(GL_MODELVIEW);
	}
}


void App::Layout()
{
	PixelRect lRect = mUiManager->GetDesktopWindow()->GetScreenRect();
	lRect.mLeft = lRect.GetCenterX()-5;
	lRect.mRight = lRect.mLeft+10;
	mPlayerSplitter->SetPos(lRect.mLeft, lRect.mTop);
	mPlayerSplitter->SetPreferredSize(lRect.GetSize());
	mPlayerSplitter->SetVisible(mGame->GetComputerIndex() < 0);
	mDoLayout = false;

	if (!mPauseButton)
	{
		return;
	}
	const int s = 8;
	//const int x = s;
	const int px = mPauseButton->GetSize().x;
	const int py = mPauseButton->GetSize().y;
	//const int dy = py * 4/3;
	//const int sy = mUiManager->GetCanvas()->GetHeight() / 20 + 34;
	int tx;
	int ty;
	int tx2;
	int ty2;
	if (mGame->GetComputerIndex() < 0)
	{
		tx = mUiManager->GetCanvas()->GetWidth()/2 - px/2;
		ty = mUiManager->GetCanvas()->GetHeight() - s - py;
		tx2 = tx;
		ty2 = s;
	}
	else
	{
		tx = mUiManager->GetCanvas()->GetWidth() - s - px;
		ty = s;
		tx2 = s;
		ty2 = ty;
	}
	/*mLazyButton->SetPos(x, sy);
	mHardButton->SetPos(x, sy+dy);
	mOriginalButton->SetPos(x, sy+dy*2);
	m1PButton->SetPos(x, sy);
	m2PButton->SetPos(x, sy+dy);
	mNextButton->SetPos(x, sy);
	mResetButton->SetPos(x, sy);
	mRetryButton->SetPos(x, sy+dy);*/
	if (mGetiPhoneButton)
	{
		mGetiPhoneButton->SetPos(tx, ty);
	}
	mPauseButton->SetPos(tx2, ty2);

	if (mDialog)
	{
		mDialog->Center();
	}
}



void App::Suspend()
{
	if (mMusicPlayer)
	{
		mMusicPlayer->Pause();
	}
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
#endif // iOS
}

void App::Resume()
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	if (mMusicPlayer)
	{
		mMusicPlayer->Stop();
		mMusicPlayer->Playback();
	}
}


bool App::Steer(UiLepra::InputManager::KeyCode pKeyCode, float pFactor)
{
	if (!mGame || mGame->GetFlybyMode() != Game::FLYBY_INACTIVE)
	{
		return false;
	}
	UiCure::CppContextObject* lAvatar1 = mGame->GetP1();
	UiCure::CppContextObject* lAvatar2 = mGame->GetP2();
	if (!lAvatar1 || !lAvatar1->IsLoaded() || !lAvatar2 ||!lAvatar2->IsLoaded())
	{
		return false;
	}
	switch (mGame->GetComputerIndex())
	{
		case 0:	lAvatar1 = lAvatar2;	break;
		case 1:	lAvatar2 = lAvatar1;	break;
	}
	enum Directive
	{
		DIRECTIVE_NONE,
		DIRECTIVE_UP,
		DIRECTIVE_DOWN,
		DIRECTIVE_LEFT,
		DIRECTIVE_RIGHT,
		DIRECTIVE_FUNCTION,
	};
	Directive lDirective = DIRECTIVE_NONE;
	UiCure::CppContextObject* lAvatar = lAvatar2;
	switch (pKeyCode)
	{
		case UIKEY(UP):
		case UIKEY(NUMPAD_8):	lDirective = DIRECTIVE_UP;					break;
		case UIKEY(W):		lDirective = DIRECTIVE_UP;		lAvatar = lAvatar1;	break;
		case UIKEY(DOWN):
		case UIKEY(NUMPAD_2):
		case UIKEY(NUMPAD_5):	lDirective = DIRECTIVE_DOWN;					break;
		case UIKEY(S):		lDirective = DIRECTIVE_DOWN;		lAvatar = lAvatar1;	break;
		case UIKEY(LEFT):
		case UIKEY(NUMPAD_4):	lDirective = DIRECTIVE_LEFT;					break;
		case UIKEY(A):		lDirective = DIRECTIVE_LEFT;		lAvatar = lAvatar1;	break;
		case UIKEY(RIGHT):
		case UIKEY(NUMPAD_6):	lDirective = DIRECTIVE_RIGHT;					break;
		case UIKEY(D):		lDirective = DIRECTIVE_RIGHT;		lAvatar = lAvatar1;	break;
		case UIKEY(LCTRL):
		case UIKEY(RCTRL):
		case UIKEY(INSERT):
		case UIKEY(NUMPAD_0):	lDirective = DIRECTIVE_FUNCTION;				break;
		case UIKEY(E):
		case UIKEY(F):		lDirective = DIRECTIVE_FUNCTION;	lAvatar = lAvatar1;	break;

#ifdef LEPRA_DEBUG
		case UIKEY(0):
		{
			if (!pFactor)
			{
				mGame->NextComputerIndex();
				mDoLayout = true;
			}
		}
		break;
		case UIKEY(PLUS):
		{
			if (!pFactor)
			{
				const Cure::ObjectPositionalData* lPosition = 0;
				lAvatar1->UpdateFullPosition(lPosition);
				if (lPosition)
				{
					Cure::ObjectPositionalData* lNewPlacement = (Cure::ObjectPositionalData*)lPosition->Clone();
					lNewPlacement->mPosition.mTransformation.GetPosition().x -= 10;
					lAvatar1->SetFullPosition(*lNewPlacement);
				}
			}
		}
		break;
		case UIKEY(9):
		{
			if (!pFactor)
			{
				const Cure::ObjectPositionalData* lPosition = 0;
				lAvatar1->UpdateFullPosition(lPosition);
				if (lPosition)
				{
					Cure::ObjectPositionalData* lNewPlacement = (Cure::ObjectPositionalData*)lPosition->Clone();
					lNewPlacement->mPosition.mTransformation.GetOrientation().RotateAroundOwnY(PIF*0.4f);
					lAvatar1->SetFullPosition(*lNewPlacement);
				}
			}
		}
		break;
#endif // Debug
	}

	switch (lDirective)
	{
		case DIRECTIVE_NONE:
		{
		}
		break;
		case DIRECTIVE_UP:
		{
			lAvatar->SetEnginePower(0, +1*pFactor, 0);
		}
		break;
		case DIRECTIVE_DOWN:
		{
			if (mGame->GetComputerIndex() != 0 && lAvatar == lAvatar1)
			{
				mReverseAndBrake = pFactor;
				if (!mReverseAndBrake)
				{
					lAvatar->SetEnginePower(0, 0, 0);
					lAvatar->SetEnginePower(2, 0, 0);
				}
			}
			if (mGame->GetComputerIndex() != 1 && lAvatar == lAvatar2)
			{
				lAvatar->SetEnginePower(0, -1*pFactor, 0);
			}
		}
		break;
		case DIRECTIVE_LEFT:
		{
			lAvatar->SetEnginePower(1, -1*pFactor, 0);
		}
		break;
		case DIRECTIVE_RIGHT:
		{
			lAvatar->SetEnginePower(1, +1*pFactor, 0);
		}
		break;
		case DIRECTIVE_FUNCTION:
		{
			switch (mGame->GetComputerIndex())
			{
				case -1:
				{
					if (lAvatar == lAvatar1)
					{
						lAvatar1->SetEnginePower(2, +1*pFactor, 0);	// Break.
					}
					else
					{
						mGame->Shoot();
					}
				}
				break;
				case 0:
				{
					mGame->Shoot();
				}
				break;
				case 1:
				{
					lAvatar1->SetEnginePower(2, +1*pFactor, 0);	// Break.
				}
				break;
			}
		}
		break;
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

#if !defined(LEPRA_IOS) && defined(LEPRA_IOS_LOOKANDFEEL)
void App::OnMouseInput(UiLepra::InputElement* pElement)
{
	if (pElement->GetType() == UiLepra::InputElement::DIGITAL)
	{
		mIsMouseDown = pElement->GetBooleanValue();
	}
	UiLepra::InputManager* lInput = mUiManager->GetInputManager();
	if (mIsMouseDown)
	{
		const int y = (int)((1+lInput->GetCursorX())*mUiManager->GetCanvas()->GetWidth()/2);
		const int x = (int)((1-lInput->GetCursorY())*mUiManager->GetCanvas()->GetHeight()/2);
		if (gFingerMoveList.empty())
		{
			gFingerMoveList.push_back(FingerMovement(x, y));
		}
		FingerMovement& lMovement = gFingerMoveList.back();
		lMovement.mLastX = x;
		lMovement.mLastY = y;
	}
	else
	{
		FingerMovement& lMovement = gFingerMoveList.back();
		lMovement.mIsPress = false;
	}
}
#endif // Computer emulating iOS

int App::PollTap(FingerMovement& pMovement)
{
	(void)pMovement;
	int lTag = 0;

#ifdef LEPRA_IOS_LOOKANDFEEL
#ifdef LEPRA_IOS
	mUiManager->GetInputManager()->SetMousePosition(pMovement.mLastY, pMovement.mLastX);
	mUiManager->GetInputManager()->GetMouse()->GetButton(0)->SetValue(pMovement.mIsPress? 1 : 0);
	mUiManager->GetInputManager()->GetMouse()->GetAxis(0)->SetValue(pMovement.mLastX);
	mUiManager->GetInputManager()->GetMouse()->GetAxis(1)->SetValue(pMovement.mLastY);
#endif // iOS

	if (!mGame || mGame->GetFlybyMode() != Game::FLYBY_INACTIVE)
	{
		return lTag;
	}

	float x = (float)pMovement.mLastX;
	float y = (float)pMovement.mLastY;
	float lStartX = (float)pMovement.mStartX;
	float lStartY = (float)pMovement.mStartY;

	//UiCure::CppContextObject* lAvatar1 = mGame->GetP1();
	UiCure::CppContextObject* lAvatar2 = mGame->GetP2();
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();
	std::swap(x, y);
	std::swap(lStartX, lStartY);
	/*const float lTapMargin = 28.0f;
	lStartX = Math::Clamp(lStartX, lTapMargin, w-lTapMargin);
	lStartY = Math::Clamp(lStartY, lTapMargin, h-lTapMargin);*/
	y = h-y;
	lStartY = h-lStartY;
	const float m = BUTTON_MARGIN;
	const float lSingleWidth = (m*2 + BUTTON_WIDTH) * 1.5f;
	const float lDoubleWidth = (m*3 + BUTTON_WIDTH*2) * 1.5f;
	const float s = lDoubleWidth / 2;
#define CLAMPUP(v)	Math::Clamp((v)*2, -1.0f, 1.0f)
	if (mGame->GetComputerIndex() != 0)
	{
		if (mGame->GetComputerIndex() != 1)	// Dual play = portrait layout.
		{
			if (lStartX <= lDoubleWidth && lStartY <= lSingleWidth)	// P1 up/down?
			{
				mThrottle = CLAMPUP((x-lStartX)/s);
				mPlayer1LastTouch.ClearTimeDiff();
				lTag = 1;
			}
			else if (lStartX <= lDoubleWidth && lStartY >= h-lDoubleWidth)	// P1 left/right?
			{
				mCurrentSteering = CLAMPUP((y-lStartY)/s);
				if (!pMovement.mIsPress)
				{
					mPreviousSteering += mCurrentSteering;
					if (::fabs(mCurrentSteering) < 0.03f)	// Go to neutral if just tap/release.
					{
						mPreviousSteering = 0;
					}
					mCurrentSteering = 0;
				}
				mPlayer1LastTouch.ClearTimeDiff();
				lTag = 2;
			}
		}
		else	// Cutie vs. computer = landscape layout.
		{
			if (lStartX <= lSingleWidth && lStartY >= h-lDoubleWidth)	// P1 up/down?
			{
				mThrottle = CLAMPUP((lStartY-y)/s);
				mPlayer1LastTouch.ClearTimeDiff();
				lTag = 1;
			}
			else if (lStartX >= h-lDoubleWidth && lStartY >= h-lDoubleWidth)	// P1 left/right?
			{
				mCurrentSteering = CLAMPUP((x-lStartX)/s);
				if (!pMovement.mIsPress)
				{
					mPreviousSteering += mCurrentSteering;
					if (::fabs(mCurrentSteering) < 0.03f)	// Go to neutral if just tap/release.
					{
						mPreviousSteering = 0;
					}
					mCurrentSteering = 0;
				}
				mPlayer1LastTouch.ClearTimeDiff();
				lTag = 2;
			}
		}
	}
	if (mGame->GetComputerIndex() != 1)
	{
		// Launcher always in portrait mode.
		if (lStartX >= w-lDoubleWidth && lStartY >= h-lSingleWidth)	// P2 up/down?
		{
			mGame->SetThrottle(lAvatar2, CLAMPUP((x-lStartX)/s));
			mPlayer2LastTouch.ClearTimeDiff();
			lTag = 3;
		}
		else if (lStartX >= w-lSingleWidth && lStartY <= lDoubleWidth)	// P1 left/right?
		{
			lAvatar2->SetEnginePower(1, CLAMPUP((lStartY-y)/s), 0);
			mPlayer2LastTouch.ClearTimeDiff();
			lTag = 4;
		}
		else if (x >= w-lSingleWidth && y >= h/2-s && y <= h/2+s)	// Bomb?
		{
			mGame->Shoot();
			mPlayer2LastTouch.ClearTimeDiff();
			lTag = 5;
		}
	}
	pMovement.mTag = lTag;
#endif // iOS L&F
	return lTag;
}



void App::MainMenu()
{
	// TRICKY: leave these here, since this call comes from >1 place.
	mGameOverTimer.Stop();
	mGame->ResetWinnerIndex();
	mGame->SetFlybyMode(Game::FLYBY_SYSTEM_PAUSE);
	mGame->SetScoreBalance(0);
	// TRICKY-END!

	mPauseButton->SetVisible(false);
	UiTbc::Dialog<App>* d = CreateTbcDialog(&App::OnMainMenuAction);
	d->AddButton(1, ICONBTN("btn_1p.png", "Single player"));
	d->AddButton(2, ICONBTN("btn_2p.png", "Two players"));
}

void App::SuperReset(bool pGameOver)
{
	mGameOverTimer.Stop();

	mPlayer1LastTouch.PopTimeDiff();
	mPlayer2LastTouch.PopTimeDiff();

	mThrottle = 0;
	mPreviousSteering = 0;
	mCurrentSteering = 0;
	mGame->SyncCameraPositions();
	if (pGameOver)
	{
		// Total game over (someone won the match)?
		const int lScoreBalance = mGame->GetScoreBalance();
		if (lScoreBalance == -SCORE_POINTS/2 || lScoreBalance == +SCORE_POINTS/2)
		{
			MainMenu();
			return;
		}
		mGame->SetScoreBalance(-lScoreBalance);

		// Nope, simply reload the interior.
		const int lComputerIndex = mGame->GetComputerIndex();
		switch (lComputerIndex)
		{
			case 0:		mGame->SetComputerIndex(1);	break;
			case 1:		mGame->SetComputerIndex(0);	break;
		}
	}
	else
	{
		// Restart level.
		mGame->SetScoreBalance(0);
	}
	mGame->ResetWinnerIndex();
	mGame->SetVehicle(mGame->GetVehicle());
	mGame->ResetLauncher();
	mResourceManager->Tick();
	mResourceManager->ForceFreeCache();

	mIsLoaded = false;
	mDoLayout = true;
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

void App::OnAction(UiTbc::Button* pButton)
{
	mDoLayout = true;
	UiTbc::Dialog<App>* d = mDialog;
	mButtonDelegate(pButton);
	if (mDialog == d)	// No news? Just drop it.
	{
		mButtonDelegate.clear();
		mDialog = 0;
	}
}

void App::OnMainMenuAction(UiTbc::Button* pButton)
{
	const int lPreviousComputerIndex = mGame->GetComputerIndex();
	if (pButton->GetTag() == 1)
	{
		// 1P
		const int lComputerIndex = Random::GetRandomNumber()&1;
		mGame->SetComputerIndex(lComputerIndex);
	}
	else
	{
		// 2P
		mGame->SetComputerIndex(-1);
	}
	// Switched from single play to dual play or vice versa?
	if ((lPreviousComputerIndex == -1) != (mGame->GetComputerIndex() == -1))
	{
		mStartTimer.Stop();	// Show keys again in.
	}
	UiTbc::Dialog<App>* d = CreateTbcDialog(&App::OnLevelAction);
	d->QueryLabel(_T("Select level"), mBigFontId);
	d->AddButton(1, ICONBTN("btn_lvl2.png", "Tutorial"));
	d->AddButton(2, ICONBTN("btn_lvl2.png", "Pendulum"));
	d->AddButton(3, ICONBTN("btn_lvl3.png", "Elevate"));
	d->AddButton(4, ICONBTN("btn_lvl4.png", "RoboCastle"));
}

void App::OnLevelAction(UiTbc::Button* pButton)
{
	str lLevel = _T("level_2");
	switch (pButton->GetTag())
	{
		case 1:	lLevel = _T("level_2");	break;
		case 2:	lLevel = _T("level_2");	break;
		case 3:	lLevel = _T("level_3");	break;
		case 4:	lLevel = _T("level_4");	break;
	}
	if (mGame->GetLevelName() != lLevel)
	{
		mGame->SetLevelName(lLevel);
	}
	if (pButton->GetTag() == 1)
	{
		// Tutorial.
		mGameOverTimer.Stop();
		mGame->SetFlybyMode(Game::FLYBY_INTRODUCTION);
		mGame->ResetWinnerIndex();
		mGame->SetVehicle(mGame->GetVehicle());
		mGame->ResetLauncher();
		return;
	}
	UiTbc::Dialog<App>* d = CreateTbcDialog(&App::OnVehicleAction);
	d->QueryLabel(_T("Select vehicle"), mBigFontId);
	d->AddButton(1, _T("Cutie"));
	d->AddButton(2, _T("Hardie"));
	d->AddButton(3, _T("Speedie"));
	d->AddButton(4, _T("Sleepie"));
	if (mGame->GetComputerIndex() != -1)
	{
		d->SetOffset(PixelCoord(0, -40));

		if (!mScrollBarImage)
		{
			mScrollBarImage = new UiCure::PainterImageResource(mUiManager, mResourceManager,
				_T("scrollbar.png"), UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
			mScrollBarImage->Load();
			mScrollBarImage->PostProcess();
			mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*mScrollBarImage->GetRamData(),
				mScrollBarImage->GetUserData(0), UiTbc::GUIImageManager::CENTERED,
				UiTbc::GUIImageManager::ALPHABLEND, 255);
		}
		UiTbc::Button* lScrollButton = ICONBTN("btn_scroll.png", "");
		lScrollButton->SetPreferredSize(PixelCoord(44, 44), false);
		mDifficultySlider = new UiTbc::ScrollBar(UiTbc::ScrollBar::HORIZONTAL,
			mScrollBarImage->GetUserData(0), 0, 0, lScrollButton);
		mDifficultySlider->SetScrollRatio(44, mScrollBarImage->GetRamData()->GetWidth());
		mDifficultySlider->SetScrollPos((mGame->GetComputerDifficulty() < 0)? 0.5 : mGame->GetComputerDifficulty());
		mDifficultySlider->SetPreferredSize(mScrollBarImage->GetRamData()->GetWidth()+15*2, 44);
		d->AddChild(mDifficultySlider);
		const int x = d->GetPreferredWidth()/2 - mDifficultySlider->GetPreferredWidth()/2;
		const int y = d->GetPreferredHeight() - 60;
		mDifficultySlider->SetPos(x, y);

		UiTbc::Label* lLabel = new UiTbc::Label();
		lLabel->SetText(_T("Opponent difficulty"), FGCOLOR_DIALOG, CLEAR_COLOR);
		d->AddChild(lLabel);
		lLabel->SetPos(x+15, y-3);

		d->UpdateLayout();
	}
}

void App::OnVehicleAction(UiTbc::Button* pButton)
{
	str lVehicle = _T("cutie");
	switch (pButton->GetTag())
	{
		case 1:	lVehicle = _T("cutie");		break;
		case 2:	lVehicle = _T("monster");	break;
		case 3:	lVehicle = _T("corvette");	break;
		case 4:	lVehicle = _T("road_roller");	break;
	}
	mDifficultySlider = 0;
	SuperReset(false);
	mGame->ResetWinnerIndex();
	mGame->SetVehicle(lVehicle);
	mGame->ResetLauncher();
	mGame->SetFlybyMode(Game::FLYBY_INACTIVE);
}

void App::OnPauseClick(UiTbc::Button*)
{
	mIsPaused = true;
	UiTbc::Dialog<App>* d = CreateTbcDialog(&App::OnPauseAction);
	d->AddButton(1, _T("Resume"));
	d->AddButton(2, _T("Restart"));
	d->AddButton(3, _T("Main menu"));
}

void App::OnPauseAction(UiTbc::Button* pButton)
{
	mIsPaused = false;
	if (pButton->GetTag() == 2)
	{
		SuperReset(false);
	}
	else if (pButton->GetTag() == 3)
	{
		MainMenu();
	}
}

void App::OnGetiPhoneClick(UiTbc::Button*)
{
	SystemManager::WebBrowseTo(_T("http://itunes.apple.com/us/app/slimeball/id447966821?mt=8&ls=1"));
	delete mGetiPhoneButton;
	mGetiPhoneButton = 0;
}

void App::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	(void)pResource;
}

UiTbc::Dialog<App>* App::CreateTbcDialog(ButtonAction pAction)
{
	mButtonDelegate = UiTbc::Dialog<App>::Action(this, pAction);
	UiTbc::Dialog<App>* d = new UiTbc::Dialog<App>(mUiManager->GetDesktopWindow(), UiTbc::Dialog<App>::Action(this, &App::OnAction));
	d->SetSize(440, 280);
	d->SetPreferredSize(440, 280);
	d->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	mDialog = d;
	return d;
}

UiTbc::Button* App::CreateButton(const str& pText, const Color& pColor, UiTbc::Component* pParent)
{
	UiTbc::Button* lButton = new UiTbc::Button(UiTbc::BorderComponent::LINEAR, 6, pColor, _T(""));
	lButton->SetText(pText);
	lButton->SetPreferredSize(44, 44);
	pParent->AddChild(lButton);
	lButton->SetVisible(false);
	lButton->UpdateLayout();
	return lButton;
}



App* App::mApp = 0;
LOG_CLASS_DEFINE(GAME, App);



}
