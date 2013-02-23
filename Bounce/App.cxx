
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
#include "../UiTBC/Include/GUI/UiTextArea.h"
#include "../UiTBC/Include/GUI/UiTextField.h"
#include "../UiTBC/Include/UiFontManager.h"
#include "Ball.h"
#include "Game.h"
#ifdef LEPRA_MAC
#include "../Lepra/Include/Posix/MacLog.h"
#endif // iOS



namespace Bounce
{



#ifdef LEPRA_TOUCH_LOOKANDFEEL
const str gPlatform = _T("touch");
#else // Computer.
const str gPlatform = _T("computer");
#endif // Touch / computer.
const str gLevelName = _T("nothing");
const str gAvatarName = _T("no-one");
#define BGCOLOR_DIALOG		Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG		Color(255, 255, 255, 255)
#define RTVAR_HISCORE_NAME	"Hiscore.Name"  // Last entered name.
#define ICONBTN(i,n)		new UiCure::IconButton(mUiManager, mResourceManager, i, n)
#define ICONBTNA(i,n)		ICONBTN(_T(i), _T(n))



FingerMoveList gFingerMoveList;



class HiscoreTextField;

class App: public Application
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

	void SetRacketForce(float pLiftFactor, const Vector3DF& pDown);
	virtual void Suspend();
	virtual void Resume();

#if !defined(LEPRA_TOUCH)
	void OnMouseInput(UiLepra::InputElement* pElement);
	bool mIsMouseDown;
#endif // Computer emulating touch device
	virtual int PollTap(FingerMovement& pMovement);

	typedef void (App::*ButtonAction)(UiTbc::Button*);
	void Reset(bool pResetScore);
	void MainMenu(bool pIsReusume);
	void MainMenu();
	void HiscoreMenu();
	void NumberDialog();
	void EnterHiscore(const str& pMessage, const Color& pColor);
	void CreateHiscoreAgent();
	void UpdateHiscore(bool pError);
	void OnAction(UiTbc::Button* pButton);
	void OnTapSound(UiTbc::Button* pButton);
	void OnMainMenuAction(UiTbc::Button* pButton);
	void OnHiscoreMenuAction(UiTbc::Button* pButton);
	void OnWhatsThisClick(UiTbc::Button* pButton);
	void OnGoToMainMenuDialogAction(UiTbc::Button* pButton);
	void OnEnterHiscoreAction(UiTbc::Button* pButton);
	void RearrangeHiScore();
	int GetExecutionCount() const;
	void PrintText(const str& pText, float pAngle, int pCenterX, int pCenterY) const;
	UiTbc::Button* CreateButton(const str& pText, const Color& pColor);
	UiTbc::Dialog* CreateTbcDialog(ButtonAction pAction);

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);
	void SoundLoadCallback(UiCure::UserSound2dResource* pResource);

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
	HiResTimer mLoopTimer;

	bool mIsLoaded;
	Cure::ResourceManager* mResourceManager;
	Cure::RuntimeVariableScope* mVariableScope;
	UiCure::GameUiManager* mUiManager;
	UiCure::MusicPlayer* mMusicPlayer;
	bool mIsPressing;
	HiscoreTextField* mHiscoreTextField;
	Cure::HiscoreAgent* mHiscoreAgent;
	int mMyHiscoreIndex;
	int mFrameCounter;
	UiCure::UserSound2dResource* mTapClick;
	UiTbc::Dialog::Action mButtonDelegate;
	UiTbc::Dialog* mDialog;
	UiTbc::FontManager::FontId mBigFontId;
	UiTbc::FontManager::FontId mMonospacedFontId;
	UiCure::UserPainterKeepImageResource* mBackdrop;
	bool mIsResume;

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



}



LEPRA_RUN_APPLICATION(Bounce::App, UiLepra::UiMain);



namespace Bounce
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mGame(0),
	mVariableScope(0),
	mAverageLoopTime(1.0/(FPS+1)),
	mIsLoaded(false),
	mIsPressing(false),
	mHiscoreTextField(0),
	mHiscoreAgent(0),
	mMyHiscoreIndex(0),
	mFrameCounter(0),
	mDialog(0),
	mBigFontId(UiTbc::FontManager::INVALID_FONTID),
	mMonospacedFontId(UiTbc::FontManager::INVALID_FONTID),
	mBackdrop(0),
	mIsResume(false)
{
	mApp = this;
}

App::~App()
{
	mDialog = 0;
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
	const int lDisplayWidth = lSize.width;
	const int lDisplayHeight = lSize.height;
#else // !Touch
	const int lDisplayWidth = 320;
	const int lDisplayHeight = 480;
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
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("Fixed"));

	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));

	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ENABLEVSYNC, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLECLEAR, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_PIXELSHADERS, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLELIGHTS, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, false);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_FOV, 60.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_CLIPNEAR, 0.01);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_CLIPFAR, 2.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("None"));
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTRED, 0.5);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTGREEN, 0.5);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_AMBIENTBLUE, 0.5);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_2D_FONTHEIGHT, 18.0);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.7);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_DOPPLER, 1.0);

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
	}
	if (lOk)
	{
#ifdef LEPRA_TOUCH
		mUiManager->GetCanvas()->SetOutputRotation(0);
#endif // Touch
		lOk = mUiManager->OpenRest();
	}
	if (lOk)
	{
		mUiManager->GetDisplayManager()->SetCaption(_T("Bounce"));
#if !defined(LEPRA_TOUCH)
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
		mMusicPlayer = new UiCure::MusicPlayer(mUiManager->GetSoundManager());
		mMusicPlayer->SetVolume(0.5f);
		mMusicPlayer->SetSongPauseTime(9, 15);
		/*mMusicPlayer->AddSong(_T("ButterflyRide.xm"));
		mMusicPlayer->AddSong(_T("BehindTheFace.xm"));
		mMusicPlayer->AddSong(_T("BrittiskBensin.xm"));
		mMusicPlayer->AddSong(_T("DontYouWantMe'97.xm"));
		mMusicPlayer->AddSong(_T("CloseEncounters.xm"));*/
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
	if (lOk)
	{
		mTapClick = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		mTapClick->Load(mResourceManager, _T("tap.wav"),
			UiCure::UserSound2dResource::TypeLoadCallback(this, &App::SoundLoadCallback));
	}
	if (lOk)
	{
		mBackdrop = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_NONE);
		mBackdrop->Load(mResourceManager, _T("backdrop.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
	}
	if (lOk)
	{
		MainMenu(false);
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
	mPathPrefix = SystemManager::GetDataDirectory(mArgumentVector[0]);
}


int App::Run()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();
	Network::Start();

	const str lLogName = Path::JoinPath(SystemManager::GetIoDirectory(_T("Bounce")), _T("log"), _T("txt"));
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
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_FASTALGO, true);
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_PARALLEL, false);	// Let's do it same on all platforms, so we can render stuff from physics data.
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_MICROSTEPS, 3);
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
	}
	if (lOk)
	{
		mGame->Cure::GameTicker::GetTimeManager()->Tick();
		mGame->Cure::GameTicker::GetTimeManager()->Clear(1);

		lOk = mResourceManager->InitDefault();
	}
	if (lOk)
	{
		mLoopTimer.EnableShadowCounter(true);
	}
	if (lOk)
	{
		mGame->Cure::GameManager::GetPhysicsManager()->SetGravity(Vector3DF(0, 0, -9.82f));
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
		HiResTimer::StepCounterShadow();
	}
	if (lOk)
	{
		const double lInstantLoopTime = mLoopTimer.QueryTimeDiff();
		if (++mFrameCounter > 2)
		{
			// Adjust frame rate, or it will be hopelessly high... on most reasonable platforms.
			mAverageLoopTime = Lepra::Math::Lerp(mAverageLoopTime, lInstantLoopTime, 0.05);
		}
		const double lDelayTime = 1.0/FPS - mAverageLoopTime;
		if (lDelayTime > 0)
		{
			Thread::Sleep(lDelayTime-0.001);
			UiLepra::Core::ProcessMessages();
		}
		HiResTimer::StepCounterShadow();	// TRICKY: after sleep we must manually step the counter shadow.
		mLoopTimer.PopTimeDiff();

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
							HiscoreMenu();
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
						MainMenu(false);	// Well... assume some super-shitty state...
					}
					break;
				}
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
	if (lOk)
	{
		if (mBackdrop->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			if (mBackdrop->GetRamData()->GetBuffer() && mGame->GetScore() > 1600)
			{
				RearrangeHiScore();
				mBackdrop->GetRamData()->SetBuffer(0);
			}

			mUiManager->PreparePaint(true);
			const Canvas* lCanvas = mUiManager->GetCanvas();
			mUiManager->GetPainter()->DrawImage(mBackdrop->GetData(), PixelRect(0, 0, lCanvas->GetWidth(), lCanvas->GetHeight()));
		}
	}
	if (lOk)
	{
		mUiManager->InputTick();
		PollTaps();
	}
	bool lIsPaused = (mDialog != 0 || mHiscoreAgent != 0);
	if (lOk && !lIsPaused)
	{
		if (!mGame->MoveRacket())
		{
			if (mGame->GetScore() >= 1000)
			{
				EnterHiscore(_T("Enter your name for the hiscore list"), LIGHT_GRAY);
			}
			else
			{
				MainMenu(false);
			}
		}
	}
	if (lOk && !lIsPaused)
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
		mGame->Paint();
		mUiManager->GetPainter()->SetColor(OFF_BLACK, 0);

		if (!lIsPaused)
		{
			const str lScore = _T("Score: ") + Int2Str((int)Math::Round(mGame->GetScore()));
			PrintText(lScore, 0, 160, mUiManager->GetCanvas()->GetHeight() - 20);
		}

		if (mHiscoreAgent)
		{
			const str lInfo = _T("Speaking to score server");
			PrintText(lInfo, 0,
				mUiManager->GetCanvas()->GetWidth()/2,
				mUiManager->GetCanvas()->GetHeight() - mUiManager->GetPainter()->GetFontHeight());
		}
	}
	if (lOk && !lIsPaused)
	{
		mGame->PreEndTick();
		mGame->WaitPhysicsTick();
		lOk = mGame->EndTick();
	}
	if (lOk && !lIsPaused)
	{
		lOk = mGame->Tick();
	}
	mResourceManager->Tick();
	mUiManager->EndRender(1.0f/FPS);

	if (mMusicPlayer)
	{
		mMusicPlayer->Update();
	}

	return lOk;
}

void App::PollTaps()
{
	mIsPressing = true;
	FingerMoveList::iterator x = gFingerMoveList.begin();
	for (; x != gFingerMoveList.end();)
	{
		x->mTag = PollTap(*x);
		if (x->mTag > 0)
		{
			++x;
		}
		else
		{
			gFingerMoveList.erase(x++);
		}
	}

#ifndef LEPRA_TOUCH
	// Test code.
	if (!gFingerMoveList.empty() && !gFingerMoveList.back().mIsPress)
	{
		gFingerMoveList.clear();
	}

	static float lLastX = mUiManager->GetInputManager()->GetCursorX();
	static float lLastY = mUiManager->GetInputManager()->GetCursorY();
	const float dx = mUiManager->GetInputManager()->GetCursorX() - lLastX;
	const float dy = mUiManager->GetInputManager()->GetCursorY() - lLastY;
	lLastX += dx;
	lLastY += dy;
	Vector3DF lGravity(dx*6, 0, dy*6 - 1);
	SetRacketForce(lGravity.GetLength() - 1, lGravity);

	if (mUiManager->GetInputManager()->GetMouse()->GetButton(1)->GetBooleanValue())
	{
		TBC::ChunkyPhysics* lStructure = mGame->GetBall()->GetPhysics();
		const int lBoneCount = lStructure->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			TBC::PhysicsManager::BodyID lBodyId = lStructure->GetBoneGeometry(x)->GetBodyId();
			if (lBodyId != TBC::INVALID_BODY)
			{
				mGame->Cure::GameManager::GetPhysicsManager()->AddForce(lBodyId, Vector3DF(1, 0, 0));
			}
		}
	}
#endif // Computer
}



void App::SetRacketForce(float pLiftFactor, const Vector3DF& pDown)
{
	if (mGame)
	{
		mGame->SetRacketForce(pLiftFactor, pDown);
	}
}

void App::Suspend()
{
	MainMenu(true);
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
	mLoopTimer.StepCounterShadow();
	mLoopTimer.PopTimeDiff();
	if (mMusicPlayer)
	{
		mMusicPlayer->Stop();
		mMusicPlayer->Playback();
	}
}


#if !defined(LEPRA_TOUCH)
void App::OnMouseInput(UiLepra::InputElement* pElement)
{
	if (pElement->GetType() == UiLepra::InputElement::DIGITAL)
	{
		mIsMouseDown = pElement->GetBooleanValue();
	}
	if (mIsMouseDown)
	{
		if (gFingerMoveList.empty())
		{
			gFingerMoveList.push_back(FingerMovement(0, 0));
		}
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
	const int lScreenWidth = mUiManager->GetCanvas()->GetWidth();
        mUiManager->GetInputManager()->SetMousePosition(lScreenWidth - pMovement.mLastX, pMovement.mLastY);
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

	mIsPressing |= pMovement.mIsPress;
	return pMovement.mIsPress? 1 : -1;
}



void App::Reset(bool pResetScore)
{
	if (pResetScore)
	{
		mGame->ResetScore();
	}
	mGame->SetRacketForce(0, Vector3DF(0,0,-1));
}

void App::MainMenu(bool pIsResume)
{
	if (!mDialog)
	{
		mIsResume = pIsResume;
		MainMenu();
	}
}

void App::MainMenu()
{
	UiTbc::Dialog* d = CreateTbcDialog(&App::OnMainMenuAction);
	if (!d)
	{
		return;
	}
	const str lPlayText = mIsResume? _T("Resume") : _T("Play");
	d->AddButton(1, CreateButton(lPlayText, Color(40, 210, 40)));
	d->AddButton(2, CreateButton(_T("High score"), Color(50, 90, 210)));
	d->AddButton(3, CreateButton(_T("A number"), Color(210, 50, 40)));
}

void App::HiscoreMenu()
{
	// Start downloading the highscore.
	CreateHiscoreAgent();
	const int lOffset = std::max(0, mMyHiscoreIndex-5);
	if (!mHiscoreAgent->StartDownloadingList(gPlatform, gLevelName, gAvatarName, lOffset, 10))
	{
		delete mHiscoreAgent;
		mHiscoreAgent = 0;
	}
	mMyHiscoreIndex = 0;

	UiTbc::Dialog* d = CreateTbcDialog(&App::OnHiscoreMenuAction);
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel(_T("High Score List"), mBigFontId);
	UiTbc::Button* lMainMenuButton = ICONBTNA("btn_back.png", "");
	lMainMenuButton->SetPreferredSize(d->GetPreferredSize());
	d->AddButton(-1, lMainMenuButton);
	lMainMenuButton->SetPos(0, 0);
	if (!mHiscoreAgent)
	{
		UpdateHiscore(true);
	}
}

void App::NumberDialog()
{
	UiTbc::Dialog* d = CreateTbcDialog(&App::OnGoToMainMenuDialogAction);
	d->SetOffset(PixelCoord(0, -30));
	const str lExecutionCount = Int2Str(GetExecutionCount());
	d->SetQueryLabel(lExecutionCount, mBigFontId);
	UiTbc::Button* lWhatsThis = CreateButton(_T("What's this?"), Color(210, 50, 40));
	d->AddButton(1, lWhatsThis);
	lWhatsThis->SetOnClick(App, OnWhatsThisClick);
	d->AddButton(2, CreateButton(_T("Main menu"), Color(40, 210, 40)));
}

void App::EnterHiscore(const str& pMessage, const Color& pColor)
{
	UiTbc::Dialog* d = CreateTbcDialog(&App::OnEnterHiscoreAction);
	d->SetOffset(PixelCoord(0, -30));
	d->SetQueryLabel(_T("Wow - ")+Int2Str((int)mGame->GetScore())+_T(" points"), mBigFontId);
	if (!pMessage.empty())
	{
		UiTbc::Label* lMessage = new UiTbc::Label;
		lMessage->SetText(pMessage, pColor, CLEAR_COLOR);
		const int lStringWidth = mUiManager->GetPainter()->GetStringWidth(pMessage);
		d->AddChild(lMessage, d->GetSize().x/2 - lStringWidth/2, 90);
	}
	mHiscoreTextField = new HiscoreTextField(d, UiTbc::TextField::BORDER_SUNKEN, 2, WHITE, _T("hiscore"));
	mHiscoreTextField->mApp = this;
	mHiscoreTextField->SetText(CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_HISCORE_NAME, _T("")));
	mHiscoreTextField->SetPreferredSize(140, 25, false);
	d->AddChild(mHiscoreTextField, 28, 107);
	mHiscoreTextField->SetKeyboardFocus();	// TRICKY: focus after adding.
	UiTbc::Button* lCancelButton = new UiTbc::Button(_T("cancel"));
	Color c = Color(210, 50, 40);
	lCancelButton->SetBaseColor(c);
	lCancelButton->SetText(_T("Cancel"), FGCOLOR_DIALOG, CLEAR_COLOR);
	lCancelButton->SetRoundedStyle(8);
	lCancelButton->SetPreferredSize(d->GetSize().x - mHiscoreTextField->GetPos().x*2 - mHiscoreTextField->GetPreferredWidth()-8, mHiscoreTextField->GetPreferredHeight()+1);
	d->AddButton(-1, lCancelButton);
	lCancelButton->SetPos(mHiscoreTextField->GetPos().x+mHiscoreTextField->GetPreferredWidth()+8, mHiscoreTextField->GetPos().y);
}

void App::CreateHiscoreAgent()
{
	delete mHiscoreAgent;
	const str lHost = _O("7y=196h5+;/,9p.5&92r:/;*(,509p;/1", "gamehiscore.pixeldoctrine.com");
	mHiscoreAgent = new Cure::HiscoreAgent(lHost, 80, _T("bounce_master"));
	//mHiscoreAgent = new Cure::HiscoreAgent(_T("localhost"), 8080, _T("bounce_master"));
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
		mDialog->AddChild(lText, 45, 30);
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
	mDialog->AddChild(lText, 50 - lPositionDigits/2 * lCharWidth, 95);
}

void App::OnAction(UiTbc::Button* pButton)
{
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

void App::OnMainMenuAction(UiTbc::Button* pButton)
{
	mDialog = 0;
	switch (pButton->GetTag())
	{
		case 1:
		{
			Reset(!mIsResume);
		}
		break;
		case 2:
		{
			HiscoreMenu();
		}
		break;
		case 3:
		{
			NumberDialog();
		}
		break;
	}
}

void App::OnHiscoreMenuAction(UiTbc::Button* /*pButton*/)
{
	mDialog = 0;
	MainMenu();
}

void App::OnWhatsThisClick(UiTbc::Button* pButton)
{
	const int y = pButton->GetPos().y - 5;
	mDialog->RemoveChild(pButton, 0);
	UiTbc::TextArea* lLabel = new UiTbc::TextArea(CLEAR_COLOR);
	lLabel->SetPreferredSize(280, 60);
	lLabel->SetFontColor(LIGHT_GRAY);
	lLabel->AddText(_T("This is the number of people executed\nin a certain ping-pong nation since you\ninstalled this app."));
	mDialog->AddChild(lLabel);
	lLabel->SetPos(20, y);
}

void App::OnGoToMainMenuDialogAction(UiTbc::Button*)
{
	mDialog = 0;
	MainMenu();
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
			CreateHiscoreAgent();
			if (!mHiscoreAgent->StartUploadingScore(gPlatform, gLevelName, gAvatarName, lLastHiscoreName, (int)Math::Round(mGame->GetScore())))
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
	else
	{
		mDialog = 0;
		MainMenu();
	}
}

struct MyHash
{
public:
        size_t operator() (const PixelCoord& __r) const
        {
                return __r.y*10000 + __r.x;
        }
};

#define REPLC(c, n)	\
{	\
	Color ic = c->GetPixelColor(n.x, n.y);	\
	std::swap(ic.mRed, ic.mGreen);	\
	c->SetPixelColor(n.x, n.y, ic);	\
}

#define GETC ejw3

static bool GETC(const Canvas* c, const PixelCoord& p)
{
	Color ic = c->GetPixelColor(p.x, p.y);
	return (ic.mGreen > ic.mRed + ic.mBlue);
	//return (ic.mRed > ic.mGreen + ic.mBlue);
}

#define UniquePut(q, v)	\
	if (!q.Exists(v)) q.PushBack(v, v);

void App::RearrangeHiScore()
{
	Canvas* c = mBackdrop->GetRamData();
	PixelCoord p(150, 150);
	typedef OrderedMap<PixelCoord, PixelCoord, MyHash> piq;
	piq lPiq;
	lPiq.PushBack(p, p);
	while (!lPiq.IsEmpty())
	{
		PixelCoord n;
		lPiq.PopFront(n, n);
		REPLC(c, n);
		p = n + PixelCoord(-1, 0);
		if (GETC(c, p))	UniquePut(lPiq, p);
		p = n + PixelCoord(+1, 0);
		if (GETC(c, p))	UniquePut(lPiq, p);
		p = n + PixelCoord(0, -1);
		if (GETC(c, p))	UniquePut(lPiq, p);
		p = n + PixelCoord(0, +1);
		if (GETC(c, p))	UniquePut(lPiq, p);
	}
	mUiManager->GetPainter()->RemoveImage(mBackdrop->GetData());
	UiCure::PainterImageResource* r = (UiCure::PainterImageResource*)mBackdrop->GetConstResource();
	r->SetOptimizedData(UiTbc::Painter::INVALID_IMAGEID);
	r->PostProcess();
}

int App::GetExecutionCount() const
{
#ifdef LEPRA_IOS
	NSUserDefaults* lDefaults = [NSUserDefaults standardUserDefaults];
	const double lStartTime = [lDefaults doubleForKey:@"FirstStartTime"];
#else // !iOS
	const double lStartTime = 0;
#endif // iOS / !iOS
	const double lCurrentTime = HiResTimer::GetSystemCounter() * HiResTimer::GetPeriod();
	const double lDiff = lCurrentTime - lStartTime;
	const double lExecutions = 5000 * lDiff / 365 / 24 / 60 / 60;
	return (int)Math::Round(lExecutions) + 1;
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

UiTbc::Button* App::CreateButton(const str& pText, const Color& pColor)
{
	UiTbc::Button* lButton = new UiTbc::Button(pColor, pText);
	lButton->SetText(pText);
	lButton->SetPreferredSize(200, 48);
	lButton->SetRoundedStyle(10);
	lButton->UpdateLayout();
	return lButton;
}

UiTbc::Dialog* App::CreateTbcDialog(ButtonAction pAction)
{
	if (mDialog)
	{
		return 0;
	}
	mButtonDelegate = UiTbc::Dialog::Action(this, pAction);
	UiTbc::Dialog* d = new UiTbc::Dialog(mUiManager->GetDesktopWindow(), UiTbc::Dialog::Action(this, &App::OnAction));
	d->SetPreClickTarget(UiTbc::Dialog::Action(this, &App::OnTapSound));
	d->SetSize(280, 320);
	d->SetPreferredSize(280, 320);
	d->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	mDialog = d;
	return d;
}



void App::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	(void)pResource;
}

void App::SoundLoadCallback(UiCure::UserSound2dResource* pResource)
{
	(void)pResource;
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
