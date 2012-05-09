
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
#include "Chain.h"
#include "Game.h"
#ifdef LEPRA_MAC
#include "../Lepra/Include/Posix/MacLog.h"
#endif // iOS



namespace Magnetic
{



#define RTVAR_HISCORE_NAME	"Hiscore.Name"  // Last entered name.



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

	virtual void Suspend();
	virtual void Resume();
	void SetGravity(const Vector3DF& pGravity);

#if !defined(LEPRA_TOUCH)
	void OnMouseInput(UiLepra::InputElement* pElement);
	bool mIsMouseDown;
#endif // Computer emulating touch device
	virtual int PollTap(FingerMovement& pMovement);

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
	Vector3DF mGravity;

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
				//mApp->mDialog->Dismiss();
				b = true;
			}
		}
		return b;
	}
};



}



LEPRA_RUN_APPLICATION(Magnetic::App, UiLepra::UiMain);



namespace Magnetic
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mGame(0),
	mVariableScope(0),
	mAverageLoopTime(1.0/(FPS+1)),
	mAverageFastLoopTime(1.0/(FPS+1)),
	mIsLoaded(false),
	mIsPressing(false),
	mHiscoreTextField(0),
	mHiscoreAgent(0),
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
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("AllowAny"));

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
		lOk = mUiManager->OpenRest();
	}
	if (lOk)
	{
		mUiManager->GetDisplayManager()->SetCaption(_T("Magnetic"));
#if !defined(LEPRA_TOUCH)
		mIsMouseDown = false;
		mUiManager->GetInputManager()->GetMouse()->AddFunctor(new UiLepra::TInputFunctor<App>(this, &App::OnMouseInput));
#endif // Computer emulating touch
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

	const str lLogName = Path::JoinPath(SystemManager::GetIoDirectory(_T("Magnetic")), _T("log"), _T("txt"));
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
		CURE_RTVAR_SET(mVariableScope, RTVAR_PHYSICS_FASTALGO, false);
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
		SetGravity(Vector3DF(-2, 0, -9));
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
						//UpdateHiscore(lLoadState != Cure::RESOURCE_LOAD_COMPLETE);
						delete mHiscoreAgent;
						mHiscoreAgent = 0;
					}
					break;
					case Cure::HiscoreAgent::ACTION_UPLOAD_SCORE:
					{
						mMyHiscoreIndex = mHiscoreAgent->GetUploadedPlace();
						delete mHiscoreAgent;
						mHiscoreAgent = 0;
						/*if (lLoadState == Cure::RESOURCE_LOAD_COMPLETE)
						{
							HiscoreMenu(+1);
						}
						else
						{
							EnterHiscore(_T("Please retry; score server obstipated"), LIGHT_RED);
						}*/
					}
					break;
					default:
					{
						delete mHiscoreAgent;
						mHiscoreAgent = 0;
						mLog.AError("Oops! Completed hiscore communication, but something went wrong.");
						assert(false);
						//MainMenu();	// Well... assume some super-shitty state...
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
		mGame->GetPhysicsManager()->SetGravity(mGravity);
	}
	if (lOk)
	{
		const float r = 0.35f;
		const float g = 0.30f;
		const float b = 0.25f;
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

		if (mHiscoreAgent)
		{
			mUiManager->GetPainter()->SetColor(WHITE, 0);
			const str lInfo = _T("Speaking to score server");
			//PrintText(lInfo, 0,
			//	mUiManager->GetCanvas()->GetWidth()/2,
			//	mUiManager->GetCanvas()->GetHeight() - mUiManager->GetPainter()->GetFontHeight());
		}
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

	const float lZAngle = -mUiManager->GetInputManager()->GetCursorX();
	const float lXAngle = -mUiManager->GetInputManager()->GetCursorY();
	QuaternionF lRotation;
	lRotation.RotateAroundOwnZ(lZAngle*PIF);
	lRotation.RotateAroundOwnX(lXAngle*PIF);
	Vector3DF lGravity = lRotation * Vector3DF(0, 9.82f, 0);

	/*if (!mUiManager->GetInputManager()->GetMouse()->GetButton(0)->GetBooleanValue())
	{
		lGravity.y = 0;
	}*/
	if (mUiManager->GetInputManager()->GetMouse()->GetButton(1)->GetBooleanValue())
	{
		TBC::ChunkyPhysics* lStructure = mGame->GetChain()->GetPhysics();
		const int lBoneCount = lStructure->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			TBC::PhysicsManager::BodyID lBodyId = lStructure->GetBoneGeometry(x)->GetBodyId();
			if (lBodyId != TBC::INVALID_BODY)
			{
				mGame->GetPhysicsManager()->AddForce(lBodyId, Vector3DF(1, 0, 0));
			}
		}
	}
	SetGravity(lGravity);
#endif // Computer
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
	mLoopTimer.StepCounterShadow();
	mLoopTimer.PopTimeDiff();
	if (mMusicPlayer)
	{
		mMusicPlayer->Stop();
		mMusicPlayer->Playback();
	}
}


void App::SetGravity(const Vector3DF& pGravity)
{
	mGravity = pGravity;
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
	mIsPressing |= pMovement.mIsPress;
	return pMovement.mIsPress? 1 : -1;
}



App* App::mApp = 0;
LOG_CLASS_DEFINE(GAME, App);



}
