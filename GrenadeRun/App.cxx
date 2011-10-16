
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <list>
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/Application.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Path.h"
#include "../Lepra/Include/SystemManager.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../UiCure/Include/UiCppContextObject.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
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
#include "Game.h"



#define UIKEY(name)	UiLepra::InputManager::IN_KBD_##name
#define FPS		20
#define BUTTON_WIDTH	40
#define BUTTON_MARGIN	2



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
	bool Poll();
	void PollTaps();
	void DrawHud() const;
	void DrawRoundedPolygon(float x, float y, float pRadius, float pAngle, int pCorners) const;
	void DrawRoundedPolygon(float x, float y, float pRadius, float pAngle, int pCorners, const Color& pColor) const;
	void DrawCircle(float x, float y, float pRadius) const;
	void DrawCircle(float x, float y, float pRadius, const Color& pColor) const;
	void DrawForceMeter(int x, int y, float pAngle, float pForce, bool pAreEqual) const;
	void GetLauncherAngles(Cure::ContextObject* pAvatar1, Cure::ContextObject* pAvatar2,
		float& pPitch, float& pGuidePitch, float& pYaw, float& pGuideYaw) const;
	void DrawBarrel(float x, float y, float pAngle) const;
	void Layout();

	virtual void Suspend();
	virtual void Resume();

	bool Steer(UiLepra::InputManager::KeyCode pKeyCode, float pFactor);
	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual int PollTap(FingerMovement& pMovement);

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
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("Fixed"));

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
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.7);
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_DOPPLER, 1.0);

#ifndef LEPRA_IOS
	CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.2);
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
		DrawHud();
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

void App::PollTaps()
{
#ifdef LEPRA_IOS
	UiCure::CppContextObject* lAvatar1 = mGame->GetP1();
	UiCure::CppContextObject* lAvatar2 = mGame->GetP2();
	if (!lAvatar1 || !lAvatar1->IsLoaded() || !lAvatar2 ||!lAvatar2->IsLoaded())
	{
		return;
	}
	mGame->SetThrottle(lAvatar1, 0);
	lAvatar1->SetEnginePower(1, 0, 0);
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
#endif // iOS
}

void App::DrawHud() const
{
	const float lButtonWidth = BUTTON_WIDTH;	// TODO: fix for Retina.
	const float lButtonRadius = lButtonWidth/2;
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();
	const float m = BUTTON_MARGIN;
	const float m2 = m*2;

	// Left player.
	DrawCircle(m+lButtonWidth,			m+lButtonRadius,	lButtonRadius);	// Up/down.
	DrawRoundedPolygon(m2+lButtonWidth*1.6f,	m+lButtonRadius,	lButtonRadius*0.6f,	+PIF/2,	3);
	DrawRoundedPolygon(lButtonWidth*0.4f,		m+lButtonRadius,	lButtonRadius*0.6f,	-PIF/2,	3);
	DrawCircle(m+lButtonRadius,			h-m-lButtonWidth,	lButtonRadius);	// Left/right.
	DrawRoundedPolygon(m+lButtonRadius,		h-m2-lButtonWidth*1.6f,	lButtonRadius*0.6f,	0,	3);
	DrawRoundedPolygon(m+lButtonRadius,		h-lButtonWidth*0.4f,	lButtonRadius*0.6f,	PIF,	3);
	// Right player.
	DrawCircle(w-m-lButtonWidth,			h-m-lButtonRadius,	lButtonRadius);	// Up/down.
	DrawRoundedPolygon(w-m2-lButtonWidth*1.6f,	h-m-lButtonRadius,	lButtonRadius*0.6f,	-PIF/2,	3);
	DrawRoundedPolygon(w-lButtonWidth*0.4f,		h-m-lButtonRadius,	lButtonRadius*0.6f,	+PIF/2,	3);
	DrawCircle(w-m-lButtonRadius,			m+lButtonWidth,		lButtonRadius);	// Left/right.
	DrawRoundedPolygon(w-m-lButtonRadius,		m2+lButtonWidth*1.6f,	lButtonRadius*0.6f,	PIF,	3);
	DrawRoundedPolygon(w-m-lButtonRadius,		lButtonWidth*0.4f,	lButtonRadius*0.6f,	0,	3);
	// Bomb button.
	bool lIsLocked = mGame->IsLauncherLocked();
	Color c = lIsLocked? Color(50, 52, 51) : Color(190, 48, 55);
	DrawRoundedPolygon(w-m-lButtonRadius,	h/2,			lButtonRadius,	-PIF/2,	6, c);

	// Draw touch force meters, to give a visual indication of steering.
	Cure::ContextObject* lAvatar1 = mGame->GetP1();
	Cure::ContextObject* lAvatar2 = mGame->GetP2();
	if (!lAvatar1 || !lAvatar1->IsLoaded() || !lAvatar2 || !lAvatar2->IsLoaded())
	{
		return;
	}

	float lForce;
	const TBC::PhysicsEngine* lGas;
	const TBC::PhysicsEngine* lBrakes;
	const TBC::PhysicsEngine* lTurn;
	{
		lGas = lAvatar1->GetPhysics()->GetEngine(0);
		lBrakes = lAvatar1->GetPhysics()->GetEngine(2);
		lForce = lGas->GetValue() - lBrakes->GetValue();
		if (lForce != 0 || std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(1)) != gFingerMoveList.end())
		{
			DrawForceMeter((int)(m2+lButtonWidth*4), (int)(m+lButtonRadius), -PIF/2, lForce, false);
		}
		lTurn = lAvatar1->GetPhysics()->GetEngine(1);
		lForce = lTurn->GetValue();
		if (lForce != 0 || std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(2)) != gFingerMoveList.end())
		{
			DrawForceMeter((int)(m2+lButtonWidth*4), (int)(h-m-lButtonWidth), PIF, lForce, true);
		}
	}
	{
		lGas = lAvatar2->GetPhysics()->GetEngine(0);
		lForce = lGas->GetValue();
		if (lForce != 0 || std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(3)) != gFingerMoveList.end())
		{
			DrawForceMeter((int)(w-m2-lButtonWidth*4), (int)(h-m*1.5f-lButtonRadius), -PIF/2, lForce, true);
		}
		lTurn = lAvatar2->GetPhysics()->GetEngine(1);
		lForce = lTurn->GetValue();
		if (lForce != 0 || std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(4)) != gFingerMoveList.end())
		{
			DrawForceMeter((int)(w-m2-lButtonWidth*4), (int)(m*1.5f+lButtonWidth), 0, lForce, true);
		}
	}

	// Draw launcher guides
	const float lDrawAngle = 0;
	float lPitch;
	float lGuidePitch;
	float lYaw;
	float lGuideYaw;
	GetLauncherAngles(lAvatar1, lAvatar2, lPitch, lGuidePitch, lYaw, lGuideYaw);
	{
		mUiManager->GetPainter()->SetColor(Color(100, 30, 30), 0);
		DrawBarrel(w-m*1.5f-lButtonWidth/2, h-m2-lButtonWidth-8, lGuidePitch+lDrawAngle);
		mUiManager->GetPainter()->SetColor(Color(140, 140, 140), 0);
		DrawBarrel(w-m*1.5f-lButtonWidth/2, h-m2-lButtonWidth-8, lPitch+lDrawAngle);
	}
	{
		mUiManager->GetPainter()->SetColor(Color(100, 30, 30), 0);
		DrawBarrel(w-m2-lButtonWidth-8, m*1.5f+lButtonWidth, lGuideYaw+lDrawAngle);
		mUiManager->GetPainter()->SetColor(Color(140, 140, 140), 0);
		DrawBarrel(w-m2-lButtonWidth-8, m*1.5f+lButtonWidth, lYaw+lDrawAngle);
	}
}

void App::DrawRoundedPolygon(float x, float y, float pRadius, float pAngle, int pCorners) const
{
	DrawRoundedPolygon(x, y, pRadius, pAngle, pCorners, Color(Color(57, 60, 190)));
}

void App::DrawRoundedPolygon(float x, float y, float pRadius, float pAngle, int pCorners, const Color& pColor) const
{
	pRadius -= 2;
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
	mUiManager->GetPainter()->DrawFan(lCoords, true);
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
	DrawCircle(x, y, pRadius, Color(Color(57, 60, 190)));
}

void App::DrawCircle(float x, float y, float pRadius, const Color& pColor) const
{
	pRadius -= 2;
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

void App::DrawForceMeter(int x, int y, float pAngle, float pForce, bool pAreEqual) const
{
	Color lColor = YELLOW;
	Color lTargetColor = RED;
	if (pForce < 0)
	{
		pForce = -pForce;
		pAngle += PIF;
		if (!pAreEqual)
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

void App::GetLauncherAngles(Cure::ContextObject* pAvatar1, Cure::ContextObject* pAvatar2,
	float& pPitch, float& pGuidePitch, float& pYaw, float& pGuideYaw) const
{
	const Vector3DF lDelta = pAvatar1->GetPosition() - pAvatar2->GetPosition();
	const Vector2DF lYawVector(lDelta.x, lDelta.y);
	pGuideYaw = lYawVector.GetAngle(Vector2DF(0, 1));
	if (lDelta.x > 0)
	{
		pGuideYaw = -pGuideYaw;
	}

	float lRoll;
	pAvatar2->GetOrientation().GetEulerAngles(pYaw, pPitch, lRoll);

	const float h = lDelta.z;
	const float v = mGame->GetMuzzleVelocity();
	const float vup = v * ::cos(pPitch);
	// g*t^2/2 - vup*t + h = 0
	//
	// Quaderatic formula:
	// ax^2 + bx + c = 0
	// =>
	//     -b +- sqrt(b^2 - 4ac)
	// x = ---------------------
	//             2a
	const float a = 9.82f/2;
	const float b = -vup;
	const float c = h;
	const float b2 = b*b;
	const float _4ac = 4*a*c;
	if (b2 < _4ac)	// Does not compute.
	{
		pGuidePitch = -PIF/4;
	}
	else
	{
		const float t = (-b + sqrt(b2 - _4ac)) / (2*a);
		const float vfwd = lYawVector.GetLength() / t;
		pGuidePitch = -::atan(vfwd/vup);
		pGuidePitch += (pGuidePitch-pPitch);	// Homebrew.
	}
}

void App::DrawBarrel(float x, float y, float pAngle) const
{
	const float lIndicatorLength = BUTTON_WIDTH;
	const float ca = ::cos(pAngle);
	const float sa = ::sin(pAngle);
	const float lWidth = BUTTON_WIDTH/10/2;
	std::vector<Vector2DF> lCoords;
	lCoords.push_back(Vector2DF(x-sa*lWidth, y-ca*lWidth));
	lCoords.push_back(Vector2DF(x+sa*lWidth, y+ca*lWidth));
	lCoords.push_back(lCoords[1] + Vector2DF(-lIndicatorLength*ca, lIndicatorLength*sa));
	lCoords.push_back(lCoords[0] + Vector2DF(-lIndicatorLength*ca, lIndicatorLength*sa));
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

int App::PollTap(FingerMovement& pMovement)
{
	pMovement;
	int lTag = 0;
#ifdef LEPRA_IOS
	float x = pMovement.mLastX;
	float y = pMovement.mLastY;
	float lStartX = pMovement.mStartX;
	float lStartY = pMovement.mStartY;
	((UiLepra::IosInputManager*)mUiManager->GetInputManager())->SetMousePosition(x, y);
	((UiLepra::IosInputElement*)mUiManager->GetInputManager()->GetMouse()->GetButton(0))->SetValue(pMovement.mIsPress? 1 : 0);
	((UiLepra::IosInputElement*)mUiManager->GetInputManager()->GetMouse()->GetAxis(0))->SetValue(x);
	((UiLepra::IosInputElement*)mUiManager->GetInputManager()->GetMouse()->GetAxis(1))->SetValue(y);

	UiCure::CppContextObject* lAvatar1 = mGame->GetP1();
	UiCure::CppContextObject* lAvatar2 = mGame->GetP2();
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();
	std::swap(x, y);
	std::swap(lStartX, lStartY);
	y = h-y;
	lStartY = h-lStartY;
	const float m = BUTTON_MARGIN;
	const float lSingleWidth = (m*2 + BUTTON_WIDTH) * 1.5f;
	const float lDoubleWidth = (m*3 + BUTTON_WIDTH*2) * 1.5f;
	const float s = lDoubleWidth / 2;
#define CLAMPUP(v)	Math::Clamp((v)*2, -1.0f, 1.0f)
	if (lStartX <= lDoubleWidth && lStartY <= lSingleWidth)	// P1 up/down?
	{
		mGame->SetThrottle(lAvatar1, CLAMPUP((x-lStartX)/s));
		lTag = 1;
	}
	else if (lStartX <= lSingleWidth && lStartY >= h-lDoubleWidth)	// P1 left/right?
	{
		lAvatar1->SetEnginePower(1, CLAMPUP((y-lStartY)/s), 0);
		lTag = 2;
	}
	else if (lStartX >= w-lDoubleWidth && lStartY >= h-lSingleWidth)	// P2 up/down?
	{
		mGame->SetThrottle(lAvatar2, CLAMPUP((x-lStartX)/s));
		lTag = 3;
	}
	else if (lStartX >= w-lSingleWidth && lStartY <= lDoubleWidth)	// P1 left/right?
	{
		lAvatar2->SetEnginePower(1, CLAMPUP((lStartY-y)/s), 0);
		lTag = 4;
	}
	else if (x >= w-lSingleWidth && y >= h/2-s && y <= h/2+s)	// Bomb?
	{
		mGame->Shoot();
		lTag = 5;
	}
	pMovement.mTag = lTag;
#endif // iOS
	return lTag;
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
