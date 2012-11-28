
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <list>
#include "../Cure/Include/HiscoreAgent.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Application.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/FileOpener.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Network.h"
#include "../Lepra/Include/Obfuxator.h"
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
#include "../UiCure/Include/UiSound.h"
#include "../UiLepra/Include/Mac/UiIosInput.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiDisplayManager.h"
#include "../UiLepra/Include/UiInput.h"
#include "../UiLepra/Include/UiOpenGLExtensions.h"	// Included to get the gl-headers.
#include "../UiLepra/Include/UiSoundManager.h"
#include "../UiLepra/Include/UiSoundStream.h"
#include "../UiTBC/Include/GUI/UiCustomButton.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiMessageDialog.h"
#include "../UiTBC/Include/GUI/UiScrollBar.h"
#include "../UiTBC/Include/GUI/UiTextField.h"
#include "../UiTBC/Include/UiFontManager.h"
#include "Vehicle.h"
#include "Game.h"
#ifdef LEPRA_MAC
#include "../Lepra/Include/Posix/MacLog.h"
#endif // iOS



#define UIKEY(name)			UiLepra::InputManager::IN_KBD_##name
#define BGCOLOR_DIALOG			Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG			Color(170, 170, 170, 255)
#define ICONBTN(i,n)			new UiCure::IconButton(mUiManager, mResourceManager, i, n)
#define ICONBTNA(i,n)			ICONBTN(_T(i), _T(n))
#define CONTENT_LEVELS			"levels"
#define CONTENT_VEHICLES		"vehicles"
#define RTVAR_CONTENT_LEVELS		"Content.Levels"
#define RTVAR_CONTENT_VEHICLES		"Content.Vehicles"
#define RTVAR_HISCORE_NAME		"Hiscore.Name"	// Last entered name.



namespace TireFire
{



FingerMoveList gFingerMoveList;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
const str gPlatform = _T("touch");
#else // Computer.
const str gPlatform = _T("computer");
#endif // Touch / computer.



class HiscoreTextField;

class App: public Application, public UiLepra::DisplayResizeObserver, public UiLepra::KeyCodeInputObserver
{
public:
	typedef Application Parent;

	App(const strutil::strvec& pArgumentList);
	virtual ~App();

	static App* GetApp();

	bool Open();
	void Close();
	virtual void Init();
	virtual int Run();
	void DisplayLogo();
	bool Poll();
	void PollTaps();
	void PrintText(const str& pText, float pAngle, int pCenterX, int pCenterY) const;
	void Layout();
	void MainMenu();
	void UpdateHiscore(bool pError);
	void HiscoreMenu(int pDirection);
	void EnterHiscore(const str& pMessage, const Color& pColor);
	void SuperReset();
	void CreateHiscoreAgent();
	void Purchase(const str& pProductName);
	void SetIsPurchasing(bool pIsPurchasing);

	virtual void Suspend();
	virtual void Resume();

	bool Steer(UiLepra::InputManager::KeyCode pKeyCode, float pFactor);
	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
#if !defined(LEPRA_TOUCH) && defined(LEPRA_TOUCH_LOOKANDFEEL)
	void OnMouseInput(UiLepra::InputElement* pElement);
	bool mIsMouseDown;
#endif // Computer emulating touch device
	virtual int PollTap(FingerMovement& pMovement);

	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	void OnAction(UiTbc::Button* pButton);
	void OnTapSound(UiTbc::Button* pButton);
	void OnOk(UiTbc::Button* pButton);
	void OnMainMenuAction(UiTbc::Button* pButton);
	void OnEnterHiscoreAction(UiTbc::Button* pButton);
	void OnLevelAction(UiTbc::Button* pButton);
	void OnVehicleAction(UiTbc::Button* pButton);
	void OnHiscoreAction(UiTbc::Button* pButton);
	void OnPreHiscoreAction(UiTbc::Button* pButton);
	void OnPreEnterAction(UiTbc::Button* pButton);
	void OnCreditsAction(UiTbc::Button* pButton);
	void DoPause();
	void OnPauseClickWithSound(UiTbc::Button* pButton);
	void OnPauseAction(UiTbc::Button* pButton);
	void OnGetiPhoneClick(UiTbc::Button*);
	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);
	void SoundLoadCallback(UiCure::UserSound2dResource* pResource);

	typedef void (App::*ButtonAction)(UiTbc::Button*);
	UiTbc::Dialog* CreateTbcDialog(ButtonAction pAction);
	static UiTbc::Button* CreateButton(const str& pText, const Color& pColor, UiTbc::Component* pParent);
	void Transpose(int& x, int& y, float& pAngle) const;
	void Transpose(float& x, float& y, float& pAngle) const;
	static str Int2Str(int pNumber);

	StdioConsoleLogListener mConsoleLogger;
	DebuggerLogListener mDebugLogger;

	static App* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch
	Game* mGame;

	str mPathPrefix;

	double mAverageLoopTime;
	double mAverageFastLoopTime;
	HiResTimer mLoopTimer;

	struct InfoTextData
	{
		str mText;
		Vector2DF mCoord;
		float mAngle;
	};

	bool mIsLoaded;
	bool mDoLayout;
	Cure::ResourceManager* mResourceManager;
	Cure::RuntimeVariableScope* mVariableScope;
	UiCure::GameUiManager* mUiManager;
	UiCure::MusicPlayer* mMusicPlayer;
	UiTbc::FontManager::FontId mBigFontId;
	UiTbc::FontManager::FontId mMonospacedFontId;
	float mReverseAndBrake;
	UiTbc::Dialog::Action mButtonDelegate;
	UiTbc::Dialog* mDialog;
	mutable StopWatch mStartTimer;
	mutable StopWatch mGameOverTimer;
	bool mFlipDraw;
	HiResTimer mBootLogoTimer;
	int mHiscoreLevelIndex;
	int mHiscoreVehicleIndex;
	HiscoreTextField* mHiscoreTextField;
	Cure::HiscoreAgent* mHiscoreAgent;
	bool mIsPurchasing;
	bool mIsMoneyIconAdded;
	UiCure::UserSound2dResource* mTapClick;
	int mMyHiscoreIndex;
	unsigned mFrameCounter;

	LOG_CLASS_DECLARE();
};

class HiscoreTextField: public UiTbc::TextField
{
public:
	typedef UiTbc::TextField Parent;
	App* mApp;
	HiscoreTextField(Component* pTopParent, unsigned pBorderStyle, int pBorderWidth,
		const Color& pColor, const str& pName):
		Parent(pTopParent, pBorderStyle, pBorderWidth, pColor, pName)
	{
	}
	virtual bool OnChar(tchar pChar)
	{
		bool b = false;
		if (GetText().length() < 13 || pChar < ' ')	// Limit character length.
		{
			b = Parent::OnChar(pChar);
			if (GetText().length() > 13)	// Shouln't happen...
			{
				SetText(GetText().substr(0, 13));
			}
		}
		if (pChar == '\r' || pChar == '\n')
		{
			const str lText = strutil::Strip(GetText(), _T(" \t\v\r\n"));
			if (!lText.empty())
			{
				mApp->mDialog->Dismiss();
				b = true;
			}
		}
		return b;
	}
};



struct IsPressing
{
	IsPressing(int pTag): mTag(pTag) {}
	bool operator()(const FingerMovement& pTouch) { return (pTouch.mTag == mTag); }
	int mTag;
};



}



LEPRA_RUN_APPLICATION(TireFire::App, UiLepra::UiMain);



namespace TireFire
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mGame(0),
	mVariableScope(0),
	mAverageLoopTime(1.0/(FPS+1)),
	mAverageFastLoopTime(1.0/(FPS+1)),
	mIsLoaded(false),
	mDoLayout(true),
	mMusicPlayer(0),
	mBigFontId(UiTbc::FontManager::INVALID_FONTID),
	mMonospacedFontId(UiTbc::FontManager::INVALID_FONTID),
	mReverseAndBrake(0),
	mDialog(0),
	mFlipDraw(false),
	mHiscoreLevelIndex(0),
	mHiscoreVehicleIndex(0),
	mHiscoreTextField(0),
	mHiscoreAgent(0),
	mIsPurchasing(false),
	mIsMoneyIconAdded(false),
	mMyHiscoreIndex(0),
	mFrameCounter(0)
{
	mApp = this;
}

App::~App()
{
	delete mHiscoreAgent;
	mHiscoreAgent = 0;
	mVariableScope = 0;
	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

App* App::GetApp()
{
	return mApp;
}

bool App::Open()
{
#ifdef LEPRA_TOUCH
	CGSize lSize = [UIScreen mainScreen].bounds.size;
	const int lDisplayWidth = lSize.height;
	const int lDisplayHeight = lSize.width;
#else // !Touch
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	const int lDisplayWidth = 480;
	const int lDisplayHeight = 320;
#else // !Touch L&F
	const int lDisplayWidth = 760;
	const int lDisplayHeight = 524;
#endif // Touch / Computer L&F
#endif // Touch/!Touch
	int lDisplayBpp = 0;
	int lDisplayFrequency = 0;
	bool lDisplayFullScreen = false;
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_RENDERENGINE, _T("OpenGL"));
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_WIDTH, lDisplayWidth);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_HEIGHT, lDisplayHeight);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_BITSPERPIXEL, lDisplayBpp);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_FREQUENCY, lDisplayFrequency);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_FULLSCREEN, lDisplayFullScreen);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("AllowUpsideDown"));

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

#ifndef LEPRA_TOUCH_LOOKANDFEEL
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_PIXELSHADERS, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.5);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("Force:Volume"));	
#endif // !Touch

	CURE_RTVAR_SET(mVariableScope, RTVAR_CONTENT_LEVELS, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_CONTENT_VEHICLES, false);
#ifdef LEPRA_IOS
	[AnimatedApp updateContent];
#endif // iOS

	mUiManager = new UiCure::GameUiManager(mVariableScope);
	bool lOk = mUiManager->OpenDraw();
	if (lOk)
	{
#ifdef LEPRA_TOUCH
		mUiManager->GetCanvas()->SetOutputRotation(90);
#endif // Touch
		UiLepra::Core::ProcessMessages();
		mUiManager->GetPainter()->ResetClippingRect();
		mUiManager->GetPainter()->Clear(BLACK);
		DisplayLogo();
		mBootLogoTimer.PopTimeDiff();
	}
	if (lOk)
	{
		lOk = mUiManager->OpenRest();
	}
	if (lOk)
	{
		mUiManager->GetDisplayManager()->SetCaption(_T("Tire Fire"));
		mUiManager->GetDisplayManager()->AddResizeObserver(this);
		mUiManager->GetInputManager()->AddKeyCodeInputObserver(this);
#if !defined(LEPRA_TOUCH) && defined(LEPRA_TOUCH_LOOKANDFEEL)
		mIsMouseDown = false;
		mUiManager->GetInputManager()->GetMouse()->AddFunctor(new UiLepra::TInputFunctor<App>(this, &App::OnMouseInput));
#endif // Computer emulating touch
	}
	if (lOk)
	{
		UiTbc::FontManager::FontId lDefaultFontId = mUiManager->GetFontManager()->GetActiveFontId();
		mBigFontId = mUiManager->GetFontManager()->QueryAddFont(_T("Helvetica"), 24);
		mMonospacedFontId = mUiManager->GetFontManager()->QueryAddFont(_T("Courier New"), 14);
		mUiManager->GetFontManager()->SetActiveFont(lDefaultFontId);
	}
	if (lOk)
	{
		/*mMusicPlayer = new UiCure::MusicPlayer(mUiManager->GetSoundManager());
		mMusicPlayer->SetVolume(0.5f);
		mMusicPlayer->SetSongPauseTime(9, 15);
		mMusicPlayer->AddSong(_T("ButterflyRide.xm"));
		mMusicPlayer->AddSong(_T("BehindTheFace.xm"));
		mMusicPlayer->AddSong(_T("BrittiskBensin.xm"));
		mMusicPlayer->AddSong(_T("DontYouWantMe'97.xm"));
		mMusicPlayer->AddSong(_T("CloseEncounters.xm"));
		mMusicPlayer->Shuffle();
		lOk = mMusicPlayer->Playback();*/
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
	if (lOk)
	{
		mTapClick = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		mTapClick->Load(mResourceManager, _T("tap.wav"),
			UiCure::UserSound2dResource::TypeLoadCallback(this, &App::SoundLoadCallback));
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
	assert(Int2Str(-123) == _T("-123"));
	assert(Int2Str(-1234) == _T("-1,234"));
	assert(Int2Str(-12345) == _T("-12,345"));
	assert(Int2Str(-123456) == _T("-123,456"));
	assert(Int2Str(-1234567) == _T("-1,234,567"));
	assert(Int2Str(+123) == _T("123"));
	assert(Int2Str(+1234) == _T("1,234"));
	assert(Int2Str(+12345) == _T("12,345"));
	assert(Int2Str(+123456) == _T("123,456"));
	assert(Int2Str(+1234567) == _T("1,234,567"));
	mPathPrefix = SystemManager::GetDataDirectory(mArgumentVector[0]);
}


int App::Run()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();
	Network::Start();

	const str lLogName = Path::JoinPath(SystemManager::GetIoDirectory(_T("TireFire")), _T("log"), _T("txt"));
	FileLogListener lFileLogger(lLogName);
	{
		LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(&mConsoleLogger, &mDebugLogger, &lFileLogger, 0, 0);
		const std::vector<Log*> lLogArray = LogType::GetLogs();
		std::vector<Log*>::const_iterator x = lLogArray.begin();
		for (; x != lLogArray.end(); ++x)
		{
			(*x)->SetLevelThreashold(LEVEL_INFO);
		}
	}

	bool lOk = true;
	if (lOk)
	{
		mVariableScope = UiCure::GetSettings();
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_PARALLEL, false);	// Let's do it same on all platforms, so we can render stuff from physics data.
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_MICROSTEPS, 15);
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_FPS, FPS);
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_ISFIXEDFPS, true);
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
		lOk = mGame->SetLevelName(_T("level_1"));
	}
	if (lOk)
	{
		mGame->Cure::GameTicker::GetTimeManager()->Tick();
		mGame->Cure::GameTicker::GetTimeManager()->Clear(1);
		lOk = mResourceManager->InitDefault();
	}
	if (lOk)
	{
		mBootLogoTimer.EnableShadowCounter(true);
		mLoopTimer.EnableShadowCounter(true);
		mGameOverTimer.EnableShadowCounter(true);
		mStartTimer.EnableShadowCounter(true);
	}
	mLoopTimer.PopTimeDiff();
#ifndef LEPRA_IOS
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
		mLoopTimer.StepCounterShadow();
	}
	if (lOk)
	{
		if (2.0 - mBootLogoTimer.QueryTimeDiff() > 0)
		{
			Thread::Sleep(0.1);
			UiLepra::Core::ProcessMessages();
			mResourceManager->Tick();
			return true;
		}
	}
	if (lOk)
	{
		const double lInstantLoopTime = mLoopTimer.QueryTimeDiff();
		if (++mFrameCounter > 2)
		{
			// Adjust frame rate, or it will be hopelessly high... on most reasonable platforms.
			mAverageLoopTime = Lepra::Math::Lerp(mAverageLoopTime, lInstantLoopTime, 0.05);
			mAverageFastLoopTime = Lepra::Math::Lerp(mAverageFastLoopTime, lInstantLoopTime, 0.7);
		}
		const double lDelayTime = 1.0/FPS - mAverageLoopTime;
		if (lDelayTime > 0)
		{
			Thread::Sleep(lDelayTime-0.001);
			UiLepra::Core::ProcessMessages();
			mLoopTimer.StepCounterShadow();	// TRICKY: after sleep we must manually step the counter shadow.
		}
		mLoopTimer.PopTimeDiff();
		if (lInstantLoopTime >= 1.0/FPS && mAverageFastLoopTime > 1.0/FPS)
		{
			// This should be a temporary slow-down, due to something like rendering of
			// lots of transparent OpenGL triangles.
			int lFrameRate = (int)(1.0/mAverageFastLoopTime);
			if (lFrameRate < 10)
			{
				lFrameRate = 10;
			}
			CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_FPS, lFrameRate);
		}
		else
		{
			CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_FPS, FPS);
			mAverageFastLoopTime = lInstantLoopTime;	// Immediately drop back sliding average to current value when FPS is normal again.
		}

	}
	if (lOk)
	{
		// Download any pending hiscore request.
		if (mHiscoreAgent)
		{
			Cure::ResourceLoadState lLoadState = mHiscoreAgent->Poll();
			if (lLoadState != Cure::RESOURCE_LOAD_IN_PROGRESS)
			{
				switch (mHiscoreAgent->GetAction())
				{
					case Cure::HiscoreAgent::ACTION_DOWNLOAD_LIST:
					{
						UpdateHiscore(lLoadState != Cure::RESOURCE_LOAD_COMPLETE);
						delete mHiscoreAgent;
						mHiscoreAgent = 0;
					}
					break;
					case Cure::HiscoreAgent::ACTION_UPLOAD_SCORE:
					{
						mMyHiscoreIndex = mHiscoreAgent->GetUploadedPlace();
						delete mHiscoreAgent;
						mHiscoreAgent = 0;
						if (lLoadState == Cure::RESOURCE_LOAD_COMPLETE)
						{
							HiscoreMenu(+1);
						}
						else
						{
							EnterHiscore(_T("Please retry; score server obstipated"), LIGHT_RED);
						}
					}
					break;
					default:
					{
						delete mHiscoreAgent;
						mHiscoreAgent = 0;
						mLog.AError("Oops! Completed hiscore communication, but something went wrong.");
						assert(false);
						MainMenu();	// Well... assume some super-shitty state...
					}
					break;
				}
			}
		}
	}
	if (lOk)
	{
		// Take care of the "brake and reverse" steering.
		if (mGame->GetVehicle() && mGame->GetVehicle()->IsLoaded())
		{
			const bool lIsMovingForward = (mGame->GetVehicle()->GetForwardSpeed() > 3.0f*SCALE_FACTOR);
			if (mReverseAndBrake)
			{
				mGame->GetVehicle()->SetEnginePower(0, lIsMovingForward? 0 : -1*mReverseAndBrake, 0);	// Reverse.
				mGame->GetVehicle()->SetEnginePower(2, lIsMovingForward? +1*mReverseAndBrake : 0, 0);	// Brake.
			}
		}
	}
	if (lOk)
	{
		lOk = (SystemManager::GetQuitRequest() == 0);
	}
	if (!mIsLoaded && mResourceManager->IsLoading())
	{
		mResourceManager->Tick();
		return lOk;
	}
	mIsLoaded = true;
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
		PollTaps();
	}
	if (lOk)
	{
		if (mGame->GetFlybyMode() == Game::FLYBY_INACTIVE)
		{
			mGame->AddScore(+3);
		}
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

		if (mGame->GetFlybyMode() == Game::FLYBY_INACTIVE &&
			mGame->IsScoreCountingEnabled())
		{
			mUiManager->GetPainter()->SetColor(WHITE);
			const str lScore = _T("Score: ") + Int2Str((int)mGame->GetScore());
			int sy = 8 + mUiManager->GetPainter()->GetFontHeight()/2;
			const int sx = mUiManager->GetCanvas()->GetWidth()/2;
			PrintText(lScore, 0, sx, sy);
		}

		mGame->Paint();

		if (mDialog)
		{
			mUiManager->Paint(false);
		}

		if (mIsPurchasing || mHiscoreAgent)
		{
			mUiManager->GetPainter()->SetColor(WHITE, 0);
			const str lInfo = mIsPurchasing? _T("Communicating with App Store...") : _T("Speaking to score server");
			PrintText(lInfo, 0, 
				mUiManager->GetCanvas()->GetWidth()/2,
				mUiManager->GetCanvas()->GetHeight() - mUiManager->GetPainter()->GetFontHeight());
		}
	}
	if (lOk)
	{
		mGame->PreEndTick();
		mGame->WaitPhysicsTick();
		lOk = mGame->EndTick();
	}
	if (lOk)
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
		if (mGameOverTimer.QueryTimeDiff() > 11.0)
		{
			if (mGame->GetScore() >= 1000.0)		// Negative score isn't any good - at least be positive.
			{
				EnterHiscore(str(), FGCOLOR_DIALOG);
			}
			else
			{
				// Score wasn't high enough, not allowed in on hiscore list.
				SuperReset();
			}
		}
	}

	return lOk;
}

void App::PollTaps()
{
	if (mIsPurchasing)
	{
		gFingerMoveList.clear();
		return;
	}

#ifdef LEPRA_TOUCH_LOOKANDFEEL
	UiCure::CppContextObject* lVehicle = mGame->GetVehicle();
	if (!lVehicle || !lVehicle->IsLoaded())
	{
		return;
	}
	FingerMoveList::iterator x = gFingerMoveList.begin();
	float lThrottle = 0;
	for (; x != gFingerMoveList.end();)
	{
		x->mTag = PollTap(*x);
		if (x->mTag > 0)
		{
			++x;
			lThrottle = 1;
		}
		else
		{
			gFingerMoveList.erase(x++);
		}
	}
	if (lVehicle->GetPhysics()->GetEngineCount() >= 3)
	{
		mGame->SetThrottle(lThrottle);
	}
#ifndef LEPRA_TOUCH
	// Test code.
	if (!gFingerMoveList.empty() && !gFingerMoveList.back().mIsPress)
	{
		gFingerMoveList.clear();
	}
#endif // Computer
#endif // Touch L&F
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
	DoPause();
#ifdef LEPRA_IOS
	[mAnimatedApp stopTick];
#endif // iOS
}

void App::Resume()
{
#ifdef LEPRA_IOS
	[mAnimatedApp startTick];
#endif // iOS
	mLoopTimer.StepCounterShadow();
	mLoopTimer.PopTimeDiff();
	if (mMusicPlayer)
	{
		mMusicPlayer->Stop();
		mMusicPlayer->Playback();
	}
}


bool App::Steer(UiLepra::InputManager::KeyCode pKeyCode, float pFactor)
{
	if (!mGame || mGame->GetFlybyMode() != Game::FLYBY_INACTIVE || mDialog)
	{
		return false;
	}
	UiCure::CppContextObject* lVehicle = mGame->GetVehicle();
	if (!lVehicle || !lVehicle->IsLoaded())
	{
		return false;
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
	switch (pKeyCode)
	{
		case UIKEY(UP):
		case UIKEY(NUMPAD_8):	lDirective = DIRECTIVE_UP;		break;
		case UIKEY(W):		lDirective = DIRECTIVE_UP;		break;
		case UIKEY(DOWN):
		case UIKEY(NUMPAD_2):
		case UIKEY(NUMPAD_5):	lDirective = DIRECTIVE_DOWN;		break;
		case UIKEY(S):		lDirective = DIRECTIVE_DOWN;		break;
		case UIKEY(LEFT):
		case UIKEY(NUMPAD_4):	lDirective = DIRECTIVE_LEFT;		break;
		case UIKEY(A):		lDirective = DIRECTIVE_LEFT;		break;
		case UIKEY(RIGHT):
		case UIKEY(NUMPAD_6):	lDirective = DIRECTIVE_RIGHT;		break;
		case UIKEY(D):		lDirective = DIRECTIVE_RIGHT;		break;
		case UIKEY(LCTRL):
		case UIKEY(RCTRL):
		case UIKEY(INSERT):
		case UIKEY(NUMPAD_0):	lDirective = DIRECTIVE_FUNCTION;	break;
		case UIKEY(E):
		case UIKEY(F):		lDirective = DIRECTIVE_FUNCTION;	break;

#ifdef LEPRA_DEBUG
		case UIKEY(0):
		{
			if (!pFactor)
			{
				mDoLayout = true;
			}
		}
		break;
		case UIKEY(PLUS):
		{
			if (!pFactor)
			{
				const Cure::ObjectPositionalData* lPosition = 0;
				lVehicle->UpdateFullPosition(lPosition);
				if (lPosition)
				{
					Cure::ObjectPositionalData* lNewPlacement = (Cure::ObjectPositionalData*)lPosition->Clone();
					lNewPlacement->mPosition.mTransformation.GetPosition().x -= 10;
					lVehicle->SetFullPosition(*lNewPlacement);
				}
			}
		}
		break;
		case UIKEY(9):
		{
			if (!pFactor)
			{
				const Cure::ObjectPositionalData* lPosition = 0;
				lVehicle->UpdateFullPosition(lPosition);
				if (lPosition)
				{
					Cure::ObjectPositionalData* lNewPlacement = (Cure::ObjectPositionalData*)lPosition->Clone();
					lNewPlacement->mPosition.mTransformation.GetOrientation().RotateAroundOwnY(PIF*0.4f);
					lVehicle->SetFullPosition(*lNewPlacement);
				}
			}
		}
		break;
		case UIKEY(8):
		{
			if (!pFactor)
			{
				mGame->FlipRenderSides();
			}
		}
		break;
		case UIKEY(7):
		{
			mGame->GetVehicle()->DrainHealth(1);
		}
		break;
		case UIKEY(6):
		{
			if (!pFactor)
			{
				const Cure::ObjectPositionalData* lPosition = 0;
				lVehicle->UpdateFullPosition(lPosition);
				if (lPosition)
				{
					Cure::ObjectPositionalData* lNewPlacement = (Cure::ObjectPositionalData*)lPosition->Clone();
					lNewPlacement->mPosition.mTransformation.GetPosition().x += 30;
					lNewPlacement->mPosition.mTransformation.GetPosition().y += 20;
					lNewPlacement->mPosition.mTransformation.GetPosition().z += 15;
					lVehicle->SetFullPosition(*lNewPlacement);
				}
			}
		}
		break;
		case UIKEY(4):
		{
			if (!pFactor)
			{
				if (mUiManager->GetCanvas()->GetOutputRotation() == 0)
				{
					mUiManager->GetCanvas()->SetOutputRotation(180);
				}
				else
				{
					mUiManager->GetCanvas()->SetOutputRotation(0);
				}
			}
		}
		break;
		case UIKEY(3):
		{
			if (!pFactor)
			{
				delete mDialog;
				mDialog = 0;
				mGame->AddScore(Random::Uniform(500, 5000));
				EnterHiscore(str(), WHITE);
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
			lVehicle->SetEnginePower(0, +1*pFactor, 0);
		}
		break;
		case DIRECTIVE_DOWN:
		{
			mReverseAndBrake = pFactor;
			if (!mReverseAndBrake)
			{
				lVehicle->SetEnginePower(0, 0, 0);
				lVehicle->SetEnginePower(2, 0, 0);
			}
		}
		break;
		case DIRECTIVE_LEFT:
		{
			lVehicle->SetEnginePower(1, -1*pFactor, 0);
		}
		break;
		case DIRECTIVE_RIGHT:
		{
			lVehicle->SetEnginePower(1, +1*pFactor, 0);
		}
		break;
		case DIRECTIVE_FUNCTION:
		{
			//mGame->Shoot();
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

#if !defined(LEPRA_TOUCH) && defined(LEPRA_TOUCH_LOOKANDFEEL)
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
		lMovement.Update(lMovement.mLastX, lMovement.mLastY, x, y);
	}
	else
	{
		FingerMovement& lMovement = gFingerMoveList.back();
		lMovement.mIsPress = false;
	}
}
#endif // Computer emulating touch device.

int App::PollTap(FingerMovement& pMovement)
{
#ifdef LEPRA_TOUCH
	mUiManager->GetInputManager()->SetMousePosition(pMovement.mLastY, pMovement.mLastX);
	if (pMovement.mIsPress)
	{
		mUiManager->GetInputManager()->GetMouse()->GetButton(0)->SetValue(1);
	}
	else
	{
		// If releasing, we click-release to make sure we don't miss anything.
		mUiManager->GetInputManager()->GetMouse()->GetButton(0)->SetValue(1);
		mUiManager->GetInputManager()->GetMouse()->GetButton(0)->SetValue(0);
	}
	mUiManager->GetInputManager()->GetMouse()->GetAxis(0)->SetValue(pMovement.mLastX);
	mUiManager->GetInputManager()->GetMouse()->GetAxis(1)->SetValue(pMovement.mLastY);
#endif // Touch

	if (!mGame || mGame->GetFlybyMode() != Game::FLYBY_INACTIVE)
	{
		return 0;
	}

	return pMovement.mIsPress? 1 : 0;
}



void App::MainMenu()
{
	// TRICKY: leave these here, since this call comes from >1 place.
	mGameOverTimer.Stop();
	mGame->EnableScoreCounting(false);
	mGame->SetFlybyMode(Game::FLYBY_PAUSE);
	// TRICKY-END!

	UiTbc::Dialog* d = CreateTbcDialog(&App::OnMainMenuAction);
	d->AddButton(1, ICONBTNA("btn_1p.png", "Single player"));
	d->AddButton(2, ICONBTNA("btn_2p.png", "Two players"));
	d->AddButton(3, ICONBTNA("btn_hiscore.png", "Hiscore"));
	d->AddButton(4, ICONBTNA("btn_credits.png", "Credits"));
}

void App::UpdateHiscore(bool pError)
{
	if (!mDialog)
	{
		return;
	}
	if (pError)
	{
		UiTbc::Label* lText = new UiTbc::Label;
		lText->SetVericalAlignment(UiTbc::Label::VALIGN_TOP);
		lText->SetText(_T("Network problem, try again l8r."), LIGHT_RED, CLEAR_COLOR);
		mDialog->AddChild(lText, 135, 75);
		return;
	}
	str lLastHiscoreName;
	CURE_RTVAR_GET(lLastHiscoreName, =, mVariableScope, RTVAR_HISCORE_NAME, _T(""));
	typedef Cure::HiscoreAgent::Entry HiscoreEntry;
	typedef Cure::HiscoreAgent::List HiscoreList;
	const HiscoreList& lHiscoreList = mHiscoreAgent->GetDownloadedList();
	str lHiscore;
	const int lBasePlace = lHiscoreList.mOffset;
	const int lEntryCount = 10;
	const double lLogExponent = ::log10((double)(lBasePlace+lEntryCount)) + 1e-12;
	const int lPositionDigits = (int)::floor(lLogExponent) + 1;
	for (int x = 0; x < (int)lHiscoreList.mEntryList.size() && x < lEntryCount; ++x)
	{
		const int lPlace = x + 1 + lBasePlace;
		const HiscoreEntry& lEntry = lHiscoreList.mEntryList[x];
		const str lScore = Int2Str(lEntry.mScore);
		char lPointer = ' ';
		char lPointer2 = ' ';
		if (lLastHiscoreName == lEntry.mName)
		{
			lPointer  = '>';
			lPointer2 = '<';
		}
		const str lFormatPlace = strutil::Format(_T("%i"), lPositionDigits);
		// TRICKY: ugly circumvention for string that won't vswprintf()!
		str lName = lEntry.mName;
		if (lName.size() < 13)
		{
			lName.append(13-lName.size(), ' ');
		}
		const str lFormat1 = _T("%c%") + lFormatPlace + _T("i ");
		const str lFormat2 = _T(" %10s%c\n");
		lHiscore += strutil::Format(lFormat1.c_str(), lPointer, lPlace) +
			lName +
			strutil::Format(lFormat2.c_str(), lScore.c_str(), lPointer2);
	}
	if (lHiscore.empty())
	{
		lHiscore = _T("No score entered. Yet.");
	}
	UiTbc::Label* lText = new UiTbc::Label;
	lText->SetFontId(mMonospacedFontId);
	lText->SetVericalAlignment(UiTbc::Label::VALIGN_TOP);
	lText->SetText(lHiscore, FGCOLOR_DIALOG, CLEAR_COLOR);
	const UiTbc::FontManager::FontId lPreviousFontId = mUiManager->GetFontManager()->GetActiveFontId();
	mUiManager->GetFontManager()->SetActiveFont(mMonospacedFontId);
	const int lCharWidth = mUiManager->GetFontManager()->GetStringWidth(_T(" "));
	mUiManager->GetFontManager()->SetActiveFont(lPreviousFontId);
	mDialog->AddChild(lText, 110 - lPositionDigits/2 * lCharWidth, 75);
}

void App::HiscoreMenu(int pDirection)
{
	// Start downloading the highscore.
	CreateHiscoreAgent();
	const str lLevelName = _T("any");
	const str lVehicleName = _T("monster_01");
	const int lOffset = std::max(0, mMyHiscoreIndex-5);
	if (!mHiscoreAgent->StartDownloadingList(gPlatform, lLevelName, lVehicleName, lOffset, 10))
	{
		delete mHiscoreAgent;
		mHiscoreAgent = 0;
	}
	mMyHiscoreIndex = 0;

	UiTbc::Dialog* d = CreateTbcDialog(&App::OnHiscoreAction);
	d->SetPreClickTarget(UiTbc::Dialog::Action(this, &App::OnPreHiscoreAction));
	d->SetDirection(pDirection, true);
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel(_T("Hiscore ") + lLevelName + _T("/") + lVehicleName, mBigFontId);
	UiTbc::Button* lMainMenuButton = ICONBTNA("btn_back.png", "");
	lMainMenuButton->SetPreferredSize(d->GetPreferredWidth() / 2, d->GetPreferredHeight());
	d->AddButton(-1, lMainMenuButton);
	lMainMenuButton->SetPos(d->GetPreferredWidth()/4, 0);
	if (!mHiscoreAgent)
	{
		UpdateHiscore(true);
	}
}

void App::EnterHiscore(const str& pMessage, const Color& pColor)
{
	mGameOverTimer.Stop();

	UiTbc::Dialog* d = CreateTbcDialog(&App::OnEnterHiscoreAction);
	d->SetPreClickTarget(UiTbc::Dialog::Action(this, &App::OnPreEnterAction));
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel(_T("Enter hiscore name (")+Int2Str((int)mGame->GetScore())+_T(" points)"), mBigFontId);
	if (!pMessage.empty())
	{
		UiTbc::Label* lMessage = new UiTbc::Label;
		lMessage->SetText(pMessage, pColor, CLEAR_COLOR);
		const int lStringWidth = mUiManager->GetPainter()->GetStringWidth(pMessage);
		d->AddChild(lMessage, d->GetSize().x/2 - lStringWidth/2, 80);
	}
	mHiscoreTextField = new HiscoreTextField(d, UiTbc::TextField::BORDER_SUNKEN, 2, WHITE, _T("hiscore"));
	mHiscoreTextField->mApp = this;
	mHiscoreTextField->SetText(CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_HISCORE_NAME, _T("")));
	mHiscoreTextField->SetPreferredSize(205, 25, false);
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	d->AddChild(mHiscoreTextField, 70, 97);
#else // Computer
	d->AddChild(mHiscoreTextField, 70, 130);
#endif // Touch / computer
	mHiscoreTextField->SetKeyboardFocus();	// TRICKY: focus after adding.
	UiTbc::Button* lCancelButton = new UiTbc::Button(_T("cancel"));
	Color c = Color(180, 50, 40);
	lCancelButton->SetBaseColor(c);
	lCancelButton->SetText(_T("Cancel"), FGCOLOR_DIALOG, CLEAR_COLOR);
	lCancelButton->SetRoundedStyle(8);
	lCancelButton->SetPreferredSize(300-mHiscoreTextField->GetPreferredWidth()-8, mHiscoreTextField->GetPreferredHeight()+1);
	d->AddButton(-1, lCancelButton);
	lCancelButton->SetPos(mHiscoreTextField->GetPos().x+mHiscoreTextField->GetPreferredWidth()+8, mHiscoreTextField->GetPos().y);
}

void App::SuperReset()
{
	mGameOverTimer.Stop();

	mGame->EndSlowmo();

	mGame->ResetScore();

	mGame->SetVehicleName(mGame->GetVehicleName());
	mResourceManager->Tick();
	strutil::strvec lResourceTypes;
	lResourceTypes.push_back(_T("RenderImg"));
	lResourceTypes.push_back(_T("Geometry"));
	lResourceTypes.push_back(_T("GeometryRef"));
	lResourceTypes.push_back(_T("Physics"));
	lResourceTypes.push_back(_T("PhysicsShared"));
	lResourceTypes.push_back(_T("RamImg"));
	mResourceManager->ForceFreeCache(lResourceTypes);
	mResourceManager->ForceFreeCache(lResourceTypes);	// Call again to release any dependent resources.

	//mIsLoaded = false;
	mDoLayout = true;
}

void App::CreateHiscoreAgent()
{
	delete mHiscoreAgent;
	const str lHost = _O("7y=196h5+;/,9p.5&92r:/;*(,509p;/1", "gamehiscore.pixeldoctrine.com");
	mHiscoreAgent = new Cure::HiscoreAgent(lHost, 80, _T("tire_fire"));
	//mHiscoreAgent = new Cure::HiscoreAgent(_T("localhost"), 8080, _T("tire_fire"));
}

void App::Purchase(const str& pProductName)
{
#ifdef LEPRA_IOS
	[mAnimatedApp startPurchase:MacLog::Encode(pProductName)];
#else // !iOS
	(void)pProductName;
	mButtonDelegate = UiTbc::Dialog::Action(this, &App::OnOk);
	UiTbc::MessageDialog* lMessage = new UiTbc::MessageDialog(mUiManager->GetDesktopWindow(), mButtonDelegate,
		_T("Content purchase not yet implemented on this platform."));
	lMessage->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	lMessage->AddButton(+10, _T("OK"));
	lMessage->AddButton(+33, _T("Nooo!"));
	mDialog = lMessage;
#endif // iOS
}

void App::SetIsPurchasing(bool pIsPurchasing)
{
	mIsPurchasing = pIsPurchasing;
	if (!mIsPurchasing && !mDialog)
	{
		MainMenu();
	}
}

void App::OnResize(int /*pWidth*/, int /*pHeight*/)
{
	mDoLayout = true;
	mBootLogoTimer.ReduceTimeDiff(-10);
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
	UiTbc::Dialog* d = mDialog;
	mButtonDelegate(pButton);
	if (mDialog == d)	// No news? Just drop it.
	{
		mButtonDelegate.clear();
		mDialog = 0;
	}
}

void App::OnTapSound(UiTbc::Button*)
{
	if (mTapClick->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->Play(mTapClick->GetData(), 1, (float)Random::Uniform(0.7, 1.4));
	}
}

void App::OnOk(UiTbc::Button* pButton)
{
	if (pButton->GetTag() == 33)
	{
		SystemManager::EmailTo(
			_T("info@pixeldoctrine.com"),
			_T("I want the complete game!"),
			_T("Hiya Game Slave Bitches,\n\n")
			_T("I enjoyed the TireFire Demo [for PC/Mac?] and would like the complete game!\n\n")
			_T("Get a move on,\n")
			_T("Yours Truly"));
	}
	MainMenu();
}

void App::OnMainMenuAction(UiTbc::Button* pButton)
{
	switch (pButton->GetTag())
	{
		case 1:
		{
			// 1P
		}
		break;
		case 2:
		{
			// 2P
		}
		break;
		case 3:
		{
			HiscoreMenu(+1);
			//EnterHiscore(_T("Press enter when you're done"), FGCOLOR_DIALOG);
		}
		return;
		case 4:
		{
			UiTbc::Dialog* d = CreateTbcDialog(&App::OnCreditsAction);
			d->SetOffset(PixelCoord(0, -30));
			d->SetQueryLabel(_T("Credits"), mBigFontId);
			UiTbc::Label* lText = new UiTbc::Label;
			lText->SetFontId(mMonospacedFontId);
			lText->SetVericalAlignment(UiTbc::Label::VALIGN_TOP);
			lText->SetText(
					_T("Game    Pixel Doctrine\n")
					_T("Music   Jonas Kapla\n")
					_T("Thanks  ODE, STLport, ChibiXM, Ogg/Vorbis, OpenAL, ALUT\n")
					_T("        libpng, Minizip, zlib, FastDelegate, UTF-8 CPP,\n")
					_T("        DMI, freesound, HappyHTTP, GAE, Python, py-cgkit\n")
					_T("\n")
					_T("Idiots kill civilians for real. Visit Avaaz.org if you\n")
					_T("too belive media attention eventually can crush tyrants."),
					FGCOLOR_DIALOG, CLEAR_COLOR);
			d->AddChild(lText, 25, 85);
			UiTbc::Button* lBackButton = new UiTbc::CustomButton(_T("back"));
			lBackButton->SetPreferredSize(d->GetPreferredSize());
			d->AddButton(-1, lBackButton);
			lBackButton->SetPos(0, 0);
		}
		return;
	}
	/*UiTbc::Dialog* d = CreateTbcDialog(&App::OnLevelAction);
	d->SetQueryLabel(_T("Select level"), mBigFontId);
	d->AddButton(1, ICONBTN(_T("btn_tutorial.png"), _T("Tutorial")));
	d->AddButton(2, ICONBTN(_T("btn_lvl2.png"), gLevels[0]));
	d->AddButton(3, ICONBTN(_T("btn_lvl3.png"), gLevels[1]));
	d->AddButton(4, ICONBTN(_T("btn_lvl4.png"), gLevels[2]));
	if (mIsMoneyIconAdded && !CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_CONTENT_LEVELS, false))
	{
		AddCostIcon(gLevels[1]);
		AddCostIcon(gLevels[2]);
	}*/
}

void App::OnEnterHiscoreAction(UiTbc::Button* pButton)
{
	if (!pButton)
	{
		str lLastHiscoreName = strutil::Strip(mHiscoreTextField->GetText(), _T(" \t\v\r\n"));
		mHiscoreTextField = 0;
		if (!lLastHiscoreName.empty())
		{
			CURE_RTVAR_SET(mVariableScope, RTVAR_HISCORE_NAME, lLastHiscoreName);
#ifdef LEPRA_IOS
			[AnimatedApp storeHiscoreName];
#endif // iOS
			const str lLevelName = _T("any");
			const str lVehicleName = _T("any");
			CreateHiscoreAgent();
			if (!mHiscoreAgent->StartUploadingScore(gPlatform, lLevelName, lVehicleName, lLastHiscoreName, (int)Math::Round(mGame->GetScore())))
			{
				delete mHiscoreAgent;
				mHiscoreAgent = 0;
			}
		}
		else
		{
			MainMenu();
		}
	}
	else if (pButton->GetTag() == -1)
	{
		MainMenu();
	}
}
	
void App::OnLevelAction(UiTbc::Button* pButton)
{
	if (pButton->GetTag() >= 3 && !CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_CONTENT_LEVELS, false))
	{
		Purchase(_T(CONTENT_LEVELS));
		return;
	}

	str lLevel = _T("level_2");
	switch (pButton->GetTag())
	{
		case 1:	lLevel = _T("level_2");			mHiscoreLevelIndex = 0;	break;
		case 2:	lLevel = _T("level_2");			mHiscoreLevelIndex = 0;	break;
		case 3:	lLevel = _T("level_elevate");		mHiscoreLevelIndex = 1;	break;
		case 4:	lLevel = _T("level_balls_castle");	mHiscoreLevelIndex = 2;	break;
	}
	if (mGame->GetLevelName() != lLevel)
	{
		mGame->SetLevelName(lLevel);
	}
	if (pButton->GetTag() == 1)
	{
		// Tutorial.
		mGameOverTimer.Stop();
		mGame->EnableScoreCounting(true);
		mGame->SetFlybyMode(Game::FLYBY_INTRODUCTION);
		mGame->SetVehicleName(_T("monster_01"));
		// If all else fails, fall thru add keep going. Makes more sense to the user than
		// a tutorial without any instructions.
	}
	if (mIsMoneyIconAdded && !CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_CONTENT_VEHICLES, false))
	{
		/*AddCostIcon(gVehicles[1]);
		AddCostIcon(gVehicles[2]);
		AddCostIcon(gVehicles[3]);*/
	}
	/*if (mGame->GetComputerIndex() != -1)
	{
		d->SetOffset(PixelCoord(0, -40));
		d->UpdateLayout();
	}*/

	strutil::strvec lResourceTypes;
	lResourceTypes.push_back(_T("RenderImg"));
	lResourceTypes.push_back(_T("Geometry"));
	lResourceTypes.push_back(_T("GeometryRef"));
	lResourceTypes.push_back(_T("Physics"));
	lResourceTypes.push_back(_T("PhysicsShared"));
	lResourceTypes.push_back(_T("RamImg"));
	mResourceManager->ForceFreeCache(lResourceTypes);
	mResourceManager->ForceFreeCache(lResourceTypes);	// Call again to release any dependent resources.
}

void App::OnHiscoreAction(UiTbc::Button* /*pButton*/)
{
	delete mHiscoreAgent;
	mHiscoreAgent = 0;
	MainMenu();
}

void App::OnPreHiscoreAction(UiTbc::Button* pButton)
{
	OnTapSound(pButton);
	switch (pButton->GetTag())
	{
		case -1:
		{
			mDialog->SetDirection(+1, false);
		}
		break;
		case -2:
		case -4:
		{
			mDialog->SetDirection(-1, false);
		}
		break;
		case -3:
		case -5:
		{
			mDialog->SetDirection(+1, false);
		}
		break;
	}
}

void App::OnPreEnterAction(UiTbc::Button* pButton)
{
	OnTapSound(pButton);
}

void App::OnCreditsAction(UiTbc::Button* /*pButton*/)
{
	MainMenu();
}

void App::DoPause()
{
	if (mDialog)
	{
		return;
	}
	UiTbc::Dialog* d = CreateTbcDialog(&App::OnPauseAction);
	d->AddButton(1, ICONBTNA("btn_resume.png", "Resume"));
	if (mGame->GetFlybyMode() == Game::FLYBY_INACTIVE)	// Restart not available in tutorial mode.
	{
		d->AddButton(2, ICONBTNA("btn_restart.png", "Restart"));
	}
	d->AddButton(3, ICONBTNA("btn_main_menu.png", "Main menu"));
}

void App::OnPauseClickWithSound(UiTbc::Button* pButton)
{
	if (mDialog)
	{
		return;
	}
	OnTapSound(pButton);
	DoPause();
}

void App::OnPauseAction(UiTbc::Button* pButton)
{
	if (pButton->GetTag() == 2)
	{
		SuperReset();
	}
	else if (pButton->GetTag() == 3)
	{
		MainMenu();
	}
}

void App::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	(void)pResource;
}

void App::SoundLoadCallback(UiCure::UserSound2dResource* pResource)
{
	(void)pResource;
}

UiTbc::Dialog* App::CreateTbcDialog(ButtonAction pAction)
{
	mButtonDelegate = UiTbc::Dialog::Action(this, pAction);
	UiTbc::Dialog* d = new UiTbc::Dialog(mUiManager->GetDesktopWindow(), UiTbc::Dialog::Action(this, &App::OnAction));
	d->SetPreClickTarget(UiTbc::Dialog::Action(this, &App::OnTapSound));
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

void App::Transpose(int& x, int& y, float& pAngle) const
{
	float fx = (float)x;
	float fy = (float)y;
	Transpose(fx, fy, pAngle);
	x = (int)fx;
	y = (int)fy;
}

void App::Transpose(float& x, float& y, float& pAngle) const
{
	if (mFlipDraw)
	{
		const int w = mUiManager->GetCanvas()->GetWidth();
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		x = w - x;
		y = mUiManager->GetCanvas()->GetHeight() - y;
		pAngle += PIF;
#else // Computer
		(void)y;
		(void)pAngle;
		const int w2 = w/2;
		if (x < w2)
		{
			x += w2;
		}
		else
		{
			x -= w2;
		}
#endif // Touch / Computer
	}
}

str App::Int2Str(int pNumber)
{
	str s = strutil::IntToString(pNumber, 10);
	size_t l = s.length();
	if (pNumber < 0)
	{
		--l;
	}
	for (size_t y = 3; y < l; y += 4)
	{
		s.insert(s.length()-y, 1, ',');
		++l;
	}
	return s;
}



App* App::mApp = 0;
LOG_CLASS_DEFINE(GAME, App);



}
