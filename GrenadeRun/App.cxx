
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
#include "Cutie.h"
#include "Game.h"
#include "Launcher.h"
#ifdef LEPRA_MAC
#include "../Lepra/Include/Posix/MacLog.h"
#endif // iOS



#define UIKEY(name)			UiLepra::InputManager::IN_KBD_##name
#define BUTTON_WIDTH			40
#define BUTTON_MARGIN			2
#define METER_HEIGHT			52
#define BGCOLOR_DIALOG			Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG			Color(170, 170, 170, 255)
#define ICONBTN(i,n)			new UiCure::IconButton(mUiManager, mResourceManager, i, n)
#define ICONBTNA(i,n)			ICONBTN(_T(i), _T(n))
#define HEART_POINTS			4
#define BARREL_COMPASS_LINE_COUNT	16
#define BARREL_COMPASS_LINE_SPACING	3
#define BARREL_COMPASS_HEIGHT		((BARREL_COMPASS_LINE_COUNT-1)*BARREL_COMPASS_LINE_SPACING + 1)
#define TOUCH_OFFSET			92
#define GET_NAME_INDEX(idx, a)		((idx) < 0)? LEPRA_ARRAY_COUNT(a)-1 : (idx)%LEPRA_ARRAY_COUNT(a)
#define GET_NAME(idx, a)		a[GET_NAME_INDEX(idx, a)]
#define CONTENT_LEVELS			"levels"
#define CONTENT_VEHICLES		"vehicles"
#define RTVAR_CONTENT_LEVELS		"Content.Levels"
#define RTVAR_CONTENT_VEHICLES		"Content.Vehicles"
#define RTVAR_HISCORE_NAME		"Hiscore.Name"	// Last entered name.
#define KC_DEV_TESTING			1	// TODO!!!!!!!!!!!!!!!!!!!!!


namespace GrenadeRun
{



FingerMoveList gFingerMoveList;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
const str gPlatform = _T("touch");
#else // Computer.
const str gPlatform = _T("computer");
#endif // Touch / computer.
const str gLevels[] = { _T("Pendulum"), _T("Elevate"), _T("RoboCastle") };
const str gVehicles[] = { _T("Cutie"), _T("Hardie"), _T("Speedie"), _T("Sleepie") };



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
	void DrawMatchStatus();
	void DrawVehicleSteering();
	void DrawHealthBar();
	void DrawLauncherIndicators(int pComputerIndex);
	void DrawHearts();
	void DrawKeys();
	void DrawInfoTexts() const;
	void ClearInfoTexts() const;
	bool PreDrawHud();
	void DrawImage(UiTbc::Painter::ImageID pImageId, float x, float y, float w, float h, float pAngle) const;
	void DrawRoundedPolygon(int x, int y, int pRadius, const Color& pColor, float pScaleX, float pScaleY) const;
	void DrawRoundedPolygon(float x, float y, float pRadius, const Color& pColor, float pScaleX, float pScaleY, int pCornerRadius) const;
	void DrawMeter(int x, int y, float pAngle, float pSize, float pMinValue, float pMaxValue, int pWidth = 40, int pSpacing = 2, int pBarCount = 13) const;
	void DrawTapIndicator(int pTag, int x, int y, float pAngle) const;
	void DrawBarrelCompass(int x, int  y, float pAngle, int pSize, float pValue1, float pValue2) const;
	void InfoText(int pPlayer, const str& pInfo, float pAngle, float dx = 0, float dy = 0) const;
	void PrintText(const str& pText, float pAngle, int pCenterX, int pCenterY) const;
	void Layout();
	void MainMenu();
	void UpdateHiscore(bool pError);
	void HiscoreMenu(int pDirection);
	void EnterHiscore(const str& pMessage, const Color& pColor);
	void SuperReset(bool pGameOver);
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
	void AddCostIcon(const str& pName);
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
	UiLepra::SoundStream* mIntroStreamer;

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
	bool mIsPaused;
	bool mPlaybackVoiceInstruction;
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
	UiCure::UserPainterKeepImageResource* mHeart;
	UiCure::UserPainterKeepImageResource* mGreyHeart;
#ifndef LEPRA_TOUCH_LOOKANDFEEL
	UiCure::UserPainterKeepImageResource* mKeyboardButton;
#endif // Computer
	UiCure::UserPainterKeepImageResource* mArrow;
	UiCure::UserPainterKeepImageResource* mSteeringWheel;
	UiCure::UserPainterKeepImageResource* mGrenade;
	UiCure::UserPainterKeepImageResource* mMoney;
	UiCure::UserPainterKeepImageResource* mScoreHud;
	UiCure::UserPainterKeepImageResource* mBack;
	float mGrenadeSizeFactor;
	mutable Vector3DF mHeartPos[HEART_POINTS];
	UiTbc::RectComponent* mPlayerSplitter;
	float mAngleTime;
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
	UiTbc::FontManager::FontId mMonospacedFontId;
	float mReverseAndBrake;
	UiTbc::Dialog::Action mButtonDelegate;
	UiTbc::Dialog* mDialog;
	mutable StopWatch mStartTimer;
	mutable StopWatch mGameOverTimer;
	UiCure::PainterImageResource* mScrollBarImage;
	UiTbc::Label* mDifficultyLabel;
	UiTbc::ScrollBar* mDifficultySlider;
	int mSlowShadowCount;
	float mBaseThrottle;
	bool mIsThrottling;
	int mThrottleMeterOffset;
	float mLiftMeterOffset;
	float mYawMeterOffset;
	float mSteering;
	float mBaseSteering;
	bool mFlipDraw;
	HiResTimer mBootLogoTimer;
	int mHiscoreLevelIndex;
	int mHiscoreVehicleIndex;
	HiscoreTextField* mHiscoreTextField;
	float mThrottle;
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



LEPRA_RUN_APPLICATION(GrenadeRun::App, UiLepra::UiMain);



namespace GrenadeRun
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mGame(0),
	mLayoutFrameCounter(-10),
	mVariableScope(0),
	mIntroStreamer(0),
	mAverageLoopTime(1.0/(FPS+1)),
	mAverageFastLoopTime(1.0/(FPS+1)),
	mIsLoaded(false),
	mDoLayout(true),
	mIsPaused(false),
	mPlaybackVoiceInstruction(false),
	mPauseButton(0),
	mGetiPhoneButton(0),
	mAngleTime(0),
	mBigFontId(UiTbc::FontManager::INVALID_FONTID),
	mMonospacedFontId(UiTbc::FontManager::INVALID_FONTID),
	mReverseAndBrake(0),
	mDialog(0),
	mScrollBarImage(0),
	mDifficultyLabel(0),
	mDifficultySlider(0),
	mSlowShadowCount(0),
	mBaseThrottle(0),
	mIsThrottling(false),
	mThrottleMeterOffset(0),
	mLiftMeterOffset(0),
	mYawMeterOffset(0),
	mSteering(0),
	mBaseSteering(0),
	mFlipDraw(false),
	mHiscoreLevelIndex(0),
	mHiscoreVehicleIndex(0),
	mHiscoreTextField(0),
	mThrottle(0),
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

#ifdef KC_DEV_TESTING
	CURE_RTVAR_SET(mVariableScope, RTVAR_CONTENT_LEVELS, true);
	CURE_RTVAR_SET(mVariableScope, RTVAR_CONTENT_VEHICLES, true);
	//CURE_RTVAR_SET(mVariableScope, RTVAR_UI_SOUND_VOLUME, 0.0);
#endif // Kill Cutie development testing

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
		mUiManager->GetDisplayManager()->SetCaption(_T("Kill Cutie"));
		mUiManager->GetDisplayManager()->AddResizeObserver(this);
		mUiManager->GetInputManager()->AddKeyCodeInputObserver(this);
#if !defined(LEPRA_TOUCH) && defined(LEPRA_TOUCH_LOOKANDFEEL)
		mIsMouseDown = false;
		mUiManager->GetInputManager()->GetMouse()->AddFunctor(new UiLepra::TInputFunctor<App>(this, &App::OnMouseInput));
#endif // Computer emulating touch
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

		mPauseButton = ICONBTNA("btn_pause.png", "");
		mPauseButton->SetBaseColor(BGCOLOR_DIALOG);
		mPauseButton->SetRoundedStyle(10);
		lDesktopWindow->AddChild(mPauseButton);
		mPauseButton->SetPreferredSize(PixelCoord(44, 44), false);
		mPauseButton->SetSize(mPauseButton->GetPreferredSize());
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(App, OnPauseClickWithSound);

#ifndef LEPRA_TOUCH_LOOKANDFEEL
		mGetiPhoneButton = ICONBTNA("btn_iphone.png", "");
		lDesktopWindow->AddChild(mGetiPhoneButton);
		mGetiPhoneButton->SetVisible(true);
		mGetiPhoneButton->SetOnClick(App, OnGetiPhoneClick);
#endif // Touch L&F
	}
	if (lOk)
	{
		mHeart = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mHeart->Load(mResourceManager, _T("heart.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		mGreyHeart = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mGreyHeart->Load(mResourceManager, _T("grey_heart.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
#ifndef LEPRA_TOUCH_LOOKANDFEEL
		mKeyboardButton = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mKeyboardButton->Load(mResourceManager, _T("btn_key.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
#endif // Computer
		mArrow = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mArrow->Load(mResourceManager, _T("arrow.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		mSteeringWheel = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mSteeringWheel->Load(mResourceManager, _T("steering_wheel.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		mGrenade = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mGrenade->Load(mResourceManager, _T("grenade.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		mMoney = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mMoney->Load(mResourceManager, _T("money.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		mScoreHud = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mScoreHud->Load(mResourceManager, _T("score_hud.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &App::PainterImageLoadCallback));
		mGrenadeSizeFactor = 1.0f;
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
	if (lOk)
	{
		lOk = Open();
	}
	if (lOk)
	{
		mGame = new Game(mUiManager, mVariableScope, mResourceManager);
#ifndef KC_DEV_TESTING
		mGame->SetComputerDifficulty(0.0f);
		lOk = mGame->SetLevelName(_T("level_2"));
#else // Dev testing
		mGame->SetComputerDifficulty(1.0f);
		mGame->SetComputerIndex(1);
		lOk = mGame->SetLevelName(_T("level_balls_castle"));
#endif // Dev testing / !dev testing
	}
	if (lOk)
	{
		mGame->Cure::GameTicker::GetTimeManager()->Tick();
		mGame->Cure::GameTicker::GetTimeManager()->Clear(1);

#ifndef KC_DEV_TESTING
		MainMenu();
#else // Dev testing
		UiTbc::Button* lButton = new UiTbc::Button(_T("Apa"));
		lButton->SetTag(4);
		OnLevelAction(lButton);
#endif // Dev testing / !dev testing

		lOk = mResourceManager->InitDefault();
	}
	if (lOk)
	{
		mBootLogoTimer.EnableShadowCounter(true);
		mLoopTimer.EnableShadowCounter(true);
		mPlayer1LastTouch.EnableShadowCounter(true);
		mPlayer2LastTouch.EnableShadowCounter(true);
		mPlayer1TouchDelay.EnableShadowCounter(true);
		mPlayer2TouchDelay.EnableShadowCounter(true);
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
#ifndef LEPRA_TOUCH_LOOKANDFEEL
		if (mAverageLoopTime > 1.0/FPS)
		{
			if (++mSlowShadowCount > 20)
			{
				// Shadows is not such a good idea on this system.
				CURE_RTVAR_SET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("None"));
				mSlowShadowCount = 0;
			}
		}
#endif // !Touch L&F
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
		if (mGame->GetCutie() && mGame->GetCutie()->IsLoaded())
		{
			const bool lIsMovingForward = (mGame->GetCutie()->GetForwardSpeed() > 3.0f*SCALE_FACTOR);
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
	mIsLoaded = true;
	if (lOk)
	{
		const bool lShowPause = !mIsPaused && !mDialog &&
			(mGame->GetFlybyMode() == Game::FLYBY_INACTIVE || mGame->GetFlybyMode() == Game::FLYBY_INTRODUCTION || mGame->GetFlybyMode() == Game::FLYBY_INTRODUCTION_FINISHING_UP);
		mPauseButton->SetVisible(lShowPause);
	}
	if (lOk)
	{
		if (!mIsPaused && mGame->GetFlybyMode() != Game::FLYBY_INACTIVE)
		{
			mGame->TickFlyby();
		}
	}
	if (lOk && mDoLayout)
	{
		Layout();
	}
	if (lOk)
	{
		if (mDifficultyLabel && mDifficultySlider)
		{
			str s = _T("Opponent difficulty (");
			const float lDifficulty = (float)mDifficultySlider->GetScrollPos();
			if (lDifficulty < 0.1f)
			{
				s += _T("wanker)");
			}
			else if (lDifficulty < 0.3f)
			{
				s += _T("silly)");
			}
			else if (lDifficulty < 0.7f)
			{
				s += _T("alright)");
			}
			else if (lDifficulty < 0.9f)
			{
				s += _T("tough)");
			}
			else
			{
				s += _T("bigot)");
			}
			mDifficultyLabel->SetText(s, FGCOLOR_DIALOG, CLEAR_COLOR);
			mGame->SetComputerDifficulty(lDifficulty);
		}
	}
	if (lOk)
	{
		// Set orientation restriction.
		if (mGame->GetComputerIndex() == -1)
		{
			CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("Fixed"));
		}
		else
		{
			CURE_RTVAR_SET(mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("AllowUpsideDown"));
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
		if (mGame->GetFlybyMode() == Game::FLYBY_INACTIVE)
		{
			mGame->AddScore(+3, 0);
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
		mInfoTextColor = Color(255, (uint8)(127*(1+::sin(mAngleTime*27)*0.9f)), 255);
		if (PreDrawHud())
		{
			if (mGame->GetFlybyMode() == Game::FLYBY_INTRODUCTION_FINISHING_UP)
			{
				if (mGame->GetComputerIndex() == -1)
				{
					DrawLauncherIndicators(0);
					DrawLauncherIndicators(1);
				}
				else
				{
					DrawLauncherIndicators(0);	// Computer is vehicle == we play launcher!
				}
				DrawHearts();
				mPlayer1LastTouch.ClearTimeDiff();
				mPlayer2LastTouch.ClearTimeDiff();
				mPlayer1TouchDelay.ClearTimeDiff();
				mPlayer2TouchDelay.ClearTimeDiff();
			}
			else
			{
				DrawMatchStatus();
				DrawVehicleSteering();
				DrawHealthBar();
				DrawLauncherIndicators(mGame->GetComputerIndex());
				DrawHearts();
				DrawKeys();
				DrawInfoTexts();
			}
		}
		ClearInfoTexts();

		if (mGame->GetFlybyMode() == Game::FLYBY_INACTIVE &&
			(mGame->IsScoreCountingEnabled() || mGame->GetWinnerIndex() >= 0) &&
			mGame->GetComputerIndex() != -1)
		{
			if (mScoreHud->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
			{
				const float x = mUiManager->GetCanvas()->GetWidth() / 2.0f;
				float y = mScoreHud->GetRamData()->GetHeight()/2.0f - 5;
				float a = PIF;
				if (mGame->GetComputerIndex() == 1)
				{
					y = (float)mUiManager->GetCanvas()->GetHeight() - y;
					a = 0;
				}
				DrawImage(mScoreHud->GetData(), x, y, (float)mScoreHud->GetRamData()->GetWidth(),
					(float)mScoreHud->GetRamData()->GetHeight(), a);
			}
			mUiManager->GetPainter()->SetColor(WHITE);
			const str lScore = _T("Score: ") + Int2Str((int)mGame->GetScore());
			int sy = 8 + mUiManager->GetPainter()->GetFontHeight()/2;
			if (mGame->GetComputerIndex() == 1)
			{
				sy = mUiManager->GetCanvas()->GetHeight() - sy;
			}
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
				mUiManager->GetCanvas()->GetHeight() - mUiManager->GetPainter()->GetFontHeight() - 3);
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

	if (mMoney->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE && !mIsMoneyIconAdded)
	{
		mIsMoneyIconAdded = true;
		mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*mMoney->GetRamData(), mMoney->GetData(),
			UiTbc::GUIImageManager::CENTERED, UiTbc::GUIImageManager::ALPHABLEND, 255);
	}

	if (mPlaybackVoiceInstruction)
	{
		mPlaybackVoiceInstruction = false;
		str lInstructionFile;
		if (mGame->GetComputerIndex() == 0)
		{
			lInstructionFile = _T("voice_shoot.wav");
		}
		else if (mGame->GetComputerIndex() == 1)
		{
			lInstructionFile = _T("voice_drive.wav");
		}
		if (!lInstructionFile.empty())
		{
			mGame->AddContextObject(new UiCure::Sound(mGame->GetResourceManager(), lInstructionFile, mUiManager), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		}
	}

	if (mIntroStreamer && mIntroStreamer->Update())
	{
		if (!mIsPaused && !mIntroStreamer->IsPlaying())
		{
			delete mIntroStreamer;
			mIntroStreamer = 0;
			if (mMusicPlayer)
			{
				mMusicPlayer->SetVolume(0.5f);
			}

		}
	}

	if (mGameOverTimer.IsStarted() && !mIsPaused)
	{
		if (mGameOverTimer.QueryTimeDiff() > 11.0 ||
			(mGame->GetComputerIndex() == 1 && mGame->GetCutie()->GetHealth() <= 0 && mGameOverTimer.QueryTimeDiff() > 7.0) ||
			(mGame->GetComputerIndex() == 0 && mGameOverTimer.QueryTimeDiff() > 9.0))
		{
			const int lHeartBalance = mGame->GetHeartBalance();
			if (lHeartBalance == -HEART_POINTS/2 || lHeartBalance == +HEART_POINTS/2)	// Somebody won.
			{
				if (mGame->GetComputerIndex() != -1 &&	// Computer in the game.
					mGame->GetComputerIndex() != mGame->GetWinnerIndex() &&	// Computer didn't win = user won over computer.
					mGame->GetScore() >= 1000.0)		// Negative score isn't any good - at least be positive.
				{
#ifdef LEPRA_TOUCH_LOOKANDFEEL
					EnterHiscore(str(), FGCOLOR_DIALOG);
#else // Computer
					EnterHiscore(_T("Press enter when you're done"), FGCOLOR_DIALOG);
#endif // Touch/computer
				}
				else
				{
					// Score wasn't high enough, not allowed in on hiscore list.
					SuperReset(true);
				}
			}
			else
			{
				// Game's not over, next round!
				SuperReset(true);
				mPlaybackVoiceInstruction = true;
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
	UiCure::CppContextObject* lAvatar1 = mGame->GetP1();
	UiCure::CppContextObject* lAvatar2 = mGame->GetP2();
	if (!lAvatar1 || !lAvatar1->IsLoaded() || !lAvatar2 ||!lAvatar2->IsLoaded())
	{
		return;
	}
	if (lAvatar1->GetPhysics()->GetEngineCount() >= 3)
	{
		mGame->SetThrottle(lAvatar1, mThrottle);
		mSteering = Math::Clamp(mSteering, -1.0f, 1.0f);
		lAvatar1->SetEnginePower(1, mSteering, 0);
	}
	mGame->SetThrottle(lAvatar2, 0);
	lAvatar2->SetEnginePower(1, 0, 0);
	FingerMoveList::iterator x = gFingerMoveList.begin();
	mIsThrottling = false;
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
#endif // Computer
#endif // Touch L&F
}

void App::DrawMatchStatus()
{
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();

#ifdef LEPRA_TOUCH_LOOKANDFEEL
	const float lAngle = (mGame->GetComputerIndex() != 1)? PIF/2 : 0;
#else // Computer.
	const float lAngle = 0;
#endif // Touch / computer.
	const int lWinner = mGame->GetWinnerIndex();
	if (lWinner >= 0)
	{
		const int lSmallFontHeight = mUiManager->GetFontManager()->GetFontHeight();
		UiTbc::FontManager::FontId lFontId = mUiManager->GetFontManager()->GetActiveFontId();
		mUiManager->GetFontManager()->SetActiveFont(mBigFontId);
		const bool lIsGameOver = (mGame->GetHeartBalance() == -HEART_POINTS/2 || mGame->GetHeartBalance() == +HEART_POINTS/2);
		if (lIsGameOver)
		{
			mGame->EnableScoreCounting(false);
		}
		const str lWon = lIsGameOver? _T("You rule!") : _T("Won heart");
		const str lLost = lIsGameOver? _T("Defeat!") : _T("Lost heart");
		const int lBackgroundSize = 100;
		if (mGame->GetComputerIndex() == -1)
		{
			str lText1;
			str lText2;
			Color lColor1;
			Color lColor2;
			if (!!lWinner == mGame->IsFlipRenderSide())
			{
				lText1 = lWon;
				lText2 = lLost;
				lColor1 = LIGHT_GREEN;
				lColor2 = LIGHT_RED;
			}
			else
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
			DrawRoundedPolygon(x1, y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f);
			DrawRoundedPolygon(x2, y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f);
			//mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_NORMAL);
			mUiManager->GetPainter()->SetColor(lColor1, 0);
			PrintText(lText1, +lAngle, x1, y);
			mUiManager->GetPainter()->SetColor(lColor2, 0);
			PrintText(lText2, -lAngle, x2, y);
		}
		else
		{
			const int x = (int)(w/2);
			int y  = (int)(h/2);
			mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
			DrawRoundedPolygon(x, y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f);
			//mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_NORMAL);
			const int lBigFontHeight = mUiManager->GetFontManager()->GetFontHeight();
			const bool lShowHealth = (mGame->GetComputerIndex() == 0 && mGame->GetRoundIndex()+1 >= 2 && !lIsGameOver);
			if (lShowHealth)
			{
				y -= lSmallFontHeight + 8;
			}
			const float a = 0;
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
			if (lShowHealth)
			{
				mUiManager->GetFontManager()->SetActiveFont(lFontId);
				y += lBigFontHeight+8;
				mUiManager->GetPainter()->SetColor(FGCOLOR_DIALOG, 0);
				const str lRound = strutil::Format(_T("Upcoming round %i"), (mGame->GetRoundIndex()+1)/2+1);
				PrintText(lRound, a, x, y);
				const float lInitialHealth = mGame->GetVehicleStartHealth(mGame->GetRoundIndex()+1);
				const str lHealth = strutil::Format(_T("You start with %i%% health"), (int)Math::Round(100*lInitialHealth));
				y += lSmallFontHeight+2;
				PrintText(lHealth, a, x, y);
			}
		}
		mUiManager->GetFontManager()->SetActiveFont(lFontId);

		mGameOverTimer.TryStart();
	}
	else
	{
		float lRealTimeRatio;
		CURE_RTVAR_GET(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
		if (lRealTimeRatio < 0.95f)
		{
			const int lFrameMod = mGame->Cure::GameTicker::GetTimeManager()->GetCurrentPhysicsFrame() % 20;
			if (lFrameMod > 12)
			{
				return;
			}
			const float lBackgroundSize = 20;
			const str lText = _T("S");
			mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
			if (mGame->GetComputerIndex() == -1)
			{
				const int x1 = (int)(w*1/4);
				const int x2 = (int)(w*3/4);
				const int y  = (int)(h/2);
				DrawRoundedPolygon((float)x1, (float)y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f, 10);
				DrawRoundedPolygon((float)x2, (float)y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f, 10);
				mUiManager->GetPainter()->SetColor(FGCOLOR_DIALOG, 0);
				PrintText(lText, +lAngle, x1, y);
				mUiManager->GetPainter()->SetColor(FGCOLOR_DIALOG, 0);
				PrintText(lText, -lAngle, x2, y);
			}
			else
			{
				const int x = (int)(w/2);
				const int y = (int)(h*2/5);
				DrawRoundedPolygon((float)x, (float)y, lBackgroundSize, BGCOLOR_DIALOG, 1.0f, 1.0f, 10);
				mUiManager->GetPainter()->SetColor(FGCOLOR_DIALOG, 0);
				PrintText(lText, 0, x, y);
			}
		}
	}
}

void App::DrawVehicleSteering()
{
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();
	const float m = BUTTON_MARGIN;
	const UiCure::CppContextObject* lAvatar1 = mGame->GetP1();

	// Draw throttle and steering indicators on iOS.
	if (mGame->GetComputerIndex() != 0 &&
		mArrow->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
		mSteeringWheel->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
		lAvatar1->GetPhysics()->GetEngineCount() >= 3)
	{
		float v0 = 0.5f;
		float v1 = mThrottle*0.5f+0.5f;
		if (mThrottle < 0)
		{
			std::swap(v0, v1);
		}
		const float lMeterHalfWidth = 52/2;
		const float aw = (float)mArrow->GetRamData()->GetWidth();
		const float ah = (float)mArrow->GetRamData()->GetHeight();
		const float mw = METER_HEIGHT*0.5f + (ah-5)*0.5f;
		int o = (int)(m + METER_HEIGHT*0.5f + ah-5);
		if (mIsThrottling)
		{
			o += TOUCH_OFFSET;
		}
		mThrottleMeterOffset = o = Math::Lerp(mThrottleMeterOffset, o, 0.8f);

		// Left player.
		if (mGame->GetComputerIndex() == -1)	// 2P?
		{
			const int x = o;
			if (mThrottle == 0 && !mIsThrottling)
			{
				DrawMeter(x, (int)(m+lMeterHalfWidth), -PIF/2, METER_HEIGHT, 0, 1);
			}
			else
			{
				DrawMeter(x, (int)(m+lMeterHalfWidth), -PIF/2, METER_HEIGHT, v0, v1);
			}
			InfoText(1, _T("Throttle/brake"), 0, 14, 0);
			DrawTapIndicator(1, 24, -(int)(mThrottle*(METER_HEIGHT/2-1.3f))-1, -PIF/2);
			DrawImage(mArrow->GetData(), x+m+mw-2,		m+lMeterHalfWidth, aw, ah, -PIF/2);
			DrawImage(mArrow->GetData(), x-m-mw+1.5f,	m+lMeterHalfWidth, aw, ah, +PIF/2);

			const float s = std::max(128.0f, h * 0.25f);
			const TBC::PhysicsEngine* lSteering = lAvatar1->GetPhysics()->GetEngine(1);
			const float a = lSteering->GetLerpThrottle(0.2f, 0.2f, false) * -1.5f - PIF/2;
			DrawImage(mSteeringWheel->GetData(), s*0.15f, h-s*0.3f, s, s, a);
			InfoText(1, _T("Left/right"), -PIF/2, 0, -10);
		}
		else
		{
			const int y = (int)h-o;
			if (mThrottle == 0 && !mIsThrottling)
			{
				DrawMeter((int)(m+lMeterHalfWidth), y, 0, METER_HEIGHT, 0, 1);
			}
			else
			{
				DrawMeter((int)(m+lMeterHalfWidth), y, 0, METER_HEIGHT, v0, v1);
			}
			InfoText(1, _T("Throttle/brake"), PIF/2, 0, -14);
			DrawTapIndicator(1, 24, -(int)(mThrottle*(METER_HEIGHT/2-1.3f))-1, 0);
			DrawImage(mArrow->GetData(), m+lMeterHalfWidth+1,	y-m-mw+1,	aw, ah, 0);
			DrawImage(mArrow->GetData(), m+lMeterHalfWidth,		y+m+mw-2,	aw, ah, PIF);

			const float s = w * 0.25f;
			const TBC::PhysicsEngine* lSteering = lAvatar1->GetPhysics()->GetEngine(1);
			const float a = lSteering->GetLerpThrottle(0.2f, 0.2f, false) * -1.5f;
			DrawImage(mSteeringWheel->GetData(), w-s*0.3f, h-s*0.15f, s, s, a);
			InfoText(1, _T("Left/right"), 0, -10, 0);
		}
	}
	if (mGame->GetComputerIndex() != 1 &&
		mGrenade->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		// Bomb button.
		const int s = std::min((int)(mUiManager->GetCanvas()->GetHeight() * 0.096f), 56);
		float t = std::min(1.0f, mGame->GetLauncherLockPercent());
		const float r = (s+1) * 1.414f;
		Color c = GREEN;
		if (t < 1.0f)
		{
			const float lMinimumWidth = 0.5f;
			t = Math::Lerp(lMinimumWidth, 1.0f, t);
			c = RED;
		}
		const float r2 = r*0.5f;
		if (mGame->GetComputerIndex() == -1)
		{
			// Dual play = portrait layout.
			DrawRoundedPolygon(w-r2*t-m*2, h/2, r2, c, t, 1.0f, 10);
			DrawImage(mGrenade->GetData(), w-r/2-m*2, h/2, (float)s, (float)s, PIF/2);
		}
		else
		{
			// Single play = landscape layout.
			DrawRoundedPolygon(w/2, h-r2*t-m*2, r2, c, 1.0f, t, 10);
			DrawImage(mGrenade->GetData(), w/2, h-r/2-m*2, (float)s, (float)s, 0);
		}
	}
#endif // Touch
}

void App::DrawHealthBar()
{
	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float lButtonWidth = BUTTON_WIDTH;

	// Draw health bar.
	Cutie* lCutie = mGame->GetCutie();
	if (mGame->GetComputerIndex() == -1)	// Two players.
	{
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		const float h = (float)mUiManager->GetCanvas()->GetHeight();
		DrawMeter((int)w/2, (int)h/2, PIF, h/2, 0, lCutie->GetHealth()*1.0002f, 20, 3, 20);
#else // !Touch
		DrawMeter((int)w/4, (int)(lButtonWidth*0.7f), -PIF/2, w/3, 0, lCutie->GetHealth()*1.0002f, 20, 3, 20);
#endif // Touch/!Touch
	}
	else if (mGame->GetComputerIndex() != 0)	// Single player Cutie.
	{
		DrawMeter((int)w/2, (int)(lButtonWidth*0.7f), -PIF/2, w/3, 0, lCutie->GetHealth()*1.0002f, 20, 3, 20);
	}
}

void App::DrawLauncherIndicators(int pComputerIndex)
{
	// Draw launcher indicators.
	if (pComputerIndex != 1 &&	// Computer not running launcher.
		mArrow->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		const float w = (float)mUiManager->GetCanvas()->GetWidth();
		const float h = (float)mUiManager->GetCanvas()->GetHeight();
		const float m = BUTTON_MARGIN;

		// Draw launcher guides, used for steering in touch device.
		float lPitch;
		float lGuidePitch;
		float lYaw;
		float lGuideYaw;
		mGame->GetLauncher()->GetAngles(mGame->GetCutie(), lPitch, lGuidePitch, lYaw, lGuideYaw);

		float x;
		float y;
		float dx = 0;
		float dy = 0;
		float ox = 0;
		float oy = -20;
		float lValue1 = lPitch/-1;
		float lValue2 = lGuidePitch/-1;
		float lDrawAngle = 0;
		const float aw = (float)mArrow->GetRamData()->GetWidth();
		const float ah = (float)mArrow->GetRamData()->GetHeight();
		const float lBarrelCompassHalfHeight = BARREL_COMPASS_HEIGHT/2 + ah/2-2 + 2;
		const float lBarrelCenterOffset = m + ah-5 + BARREL_COMPASS_HEIGHT/2 + 2;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (pComputerIndex == 0)	// Single play = landscape.
		{
			x = m+aw/2;
			y = h-lBarrelCenterOffset;
			lDrawAngle = -PIF/2;
			dy = lBarrelCompassHalfHeight;
		}
		else	// Dual play = portrait.
		{
			x = w-lBarrelCenterOffset;
			y = h-m-aw/2;
			dx = lBarrelCompassHalfHeight;
			std::swap(ox, oy);
			lValue1 = 1 - lValue1;
			lValue2 = 1 - lValue2;
		}
		if (std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(3)) != gFingerMoveList.end())
		{
			if (pComputerIndex == 0)	// Single play = landscape.
			{
				y -= TOUCH_OFFSET;
			}
			else	// Dual play = portrait.
			{
				x -= TOUCH_OFFSET;
			}
		}
		if (pComputerIndex == 0)	// Single play = landscape.
		{
			mLiftMeterOffset = y = Math::Lerp(mLiftMeterOffset, y, 0.8f);
		}
		else	// Dual play = portrait.
		{
			mLiftMeterOffset = x = Math::Lerp(mLiftMeterOffset, x, 0.8f);
		}
#else // !Touch
		x = m+aw/2 + 30;
		y = h-lBarrelCenterOffset;
		lDrawAngle = -PIF/2;
		if (pComputerIndex == -1)	// Two players.
		{
			x += w/2;
		}
		dy = lBarrelCompassHalfHeight;
#endif // Touch/!touch
		DrawBarrelCompass((int)x, (int)y, lDrawAngle, (int)aw-8, lValue1, lValue2);
		InfoText(2, _T("Up/down compass"), lDrawAngle+PIF, ox, oy);
		const float lLiftThrottle = mGame->GetLauncher()->GetPhysics()->GetEngine(0)->GetValue();
		const float lCenteredValue = (pComputerIndex == 0)? lValue1*2-1 : -(lValue1*2-1);
		const int lLiftOffset = (int)((BARREL_COMPASS_HEIGHT-4)*lCenteredValue/2 + 9*lLiftThrottle);
		DrawTapIndicator(3, 24, lLiftOffset, lDrawAngle+PIF/2);
		DrawImage(mArrow->GetData(), x-dx, y-dy, aw, ah, lDrawAngle+PIF/2);
		DrawImage(mArrow->GetData(), x+dx, y+dy, aw, ah, lDrawAngle-PIF/2);

		dx = 0;
		dy = 0;
		ox = -30;
		oy = 0;
		lValue1 = lYaw/(PIF+0.1f)+0.5f;
		lValue2 = (lGuideYaw)/(PIF+0.1f)+0.5f;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (pComputerIndex == 0)	// Single play = landscape.
		{
			x = w-lBarrelCenterOffset;
			y = h-m-aw/2;
			dx = -lBarrelCompassHalfHeight;
		}
		else	// Dual play = portrait.
		{
			x = w-m-aw/2;
			y = m+lBarrelCenterOffset;
			dy = lBarrelCompassHalfHeight;
			ox = 0;
			oy = +30;
			lValue1 = 1 - lValue1;
			lValue2 = 1 - lValue2;
		}

		if (std::find_if(gFingerMoveList.begin(), gFingerMoveList.end(), IsPressing(4)) != gFingerMoveList.end())
		{
			if (pComputerIndex == 0)	// Single play = landscape.
			{
				y -= TOUCH_OFFSET;
			}
			else
			{
				x -= TOUCH_OFFSET;
			}
		}
		if (pComputerIndex == 0)	// Single play = landscape.
		{
			mYawMeterOffset = y = Math::Lerp(mYawMeterOffset, y, 0.8f);
		}
		else
		{
			mYawMeterOffset = x = Math::Lerp(mYawMeterOffset, x, 0.8f);
		}
#else // !Touch
		x = w-lBarrelCenterOffset - 30;
		y = h-m-aw/2;
		dx = -lBarrelCompassHalfHeight;
#endif // Touch/!Touch
		DrawBarrelCompass((int)x, (int)y, +PIF/2+lDrawAngle, (int)aw-8, lValue1, lValue2);
		InfoText(2, _T("Left/right compass"), lDrawAngle+PIF/2, ox, oy);
		const float lRotateThrottle = mGame->GetLauncher()->GetPhysics()->GetEngine(1)->GetValue();
		const float lCenteredRotation = (pComputerIndex == 0)? lValue1*2-1 : -(lValue1*2-1);
		const int lRotateOffset = (int)((BARREL_COMPASS_HEIGHT-4)*lCenteredRotation/2 - 9*lRotateThrottle);
		DrawTapIndicator(4, +24, lRotateOffset, lDrawAngle);
		DrawImage(mArrow->GetData(), x-dx, y-dy, aw, ah, lDrawAngle);
		DrawImage(mArrow->GetData(), x+dx, y+dy, aw, ah, lDrawAngle-PIF);
	}
}

void App::DrawHearts()
{
	mFlipDraw = false;

	// Draw hearts.
	if (mHeart->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
		mGreyHeart->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		const float w = (float)mUiManager->GetCanvas()->GetWidth();

		mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
		mUiManager->GetPainter()->SetAlphaValue(255);
		const float lMargin = 8;
		float x = lMargin;
		float y = BUTTON_WIDTH + 8*2 + 3;
		float lAngle = 0;
		const int lBalance = -mGame->GetHeartBalance();
		static float lHeartBeatTime = 0;
		lHeartBeatTime += 0.5f;
		if (lHeartBeatTime > 4*PIF)
		{
			lHeartBeatTime -= 4*PIF;
		}
		const float iw = (float)mHeart->GetRamData()->GetWidth();
		const float ih = (float)mHeart->GetRamData()->GetHeight();
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (mGame->GetComputerIndex() == -1)
		{
			x = w/2 - BUTTON_WIDTH/2 - 8 - 3 - ih;
			lAngle = -PIF/2;
		}
#endif	// Touch L&F
		if (mGame->GetComputerIndex() == 1 && !mGetiPhoneButton)
		{
			x = lMargin;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
			x += BUTTON_WIDTH/2 - iw/2;
#endif // Touch
			y = lMargin;
		}
		if (mGame->GetComputerIndex() == -1)
		{
			y = lMargin;
		}
		float lHeartBeatLoopTime = lHeartBeatTime;
		int lHeartIndex = 0;
		for (int i = -HEART_POINTS/2; i < +HEART_POINTS/2; ++i, lHeartBeatLoopTime += 0.6f)
		{
			if (mGame->GetComputerIndex() != 0)
			{
				Vector3DF v(x+iw/2, y+iw/2, lAngle);
				if (i < lBalance)
				{
					const float lHeartBeatFactor = (lHeartBeatLoopTime < 2*PIF) ? 1 : 1+0.15f*sin(lHeartBeatLoopTime);
					const float hw = iw * lHeartBeatFactor;
					const float hh = ih * lHeartBeatFactor;
					v = mHeartPos[lHeartIndex] = Math::Lerp(mHeartPos[lHeartIndex], v, 0.07f);
					++lHeartIndex;
					DrawImage(mHeart->GetData(), v.x, v.y, hw, hh, v.z);
				}
				else
				{
					DrawImage(mGreyHeart->GetData(), v.x, v.y, iw, ih, v.z);
				}
				y += iw+8;
			}
		}
		x = w - lMargin - iw;
		y = BUTTON_WIDTH + 8*2 + 3;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (mGame->GetComputerIndex() == -1)
		{
			x = w/2 + BUTTON_WIDTH/2 + lMargin;
			y = lMargin;
			lAngle = PIF/2;
		}
		else
		{
			x = lMargin;
			y = BUTTON_WIDTH + lMargin*2;
			lAngle = 0;
		}
#endif	// Touch L&F
		if (mGame->GetComputerIndex() == -1)
		{
			y = lMargin;
		}
		else if (mGame->GetComputerIndex() == 0 && !mGetiPhoneButton)
		{
#ifdef LEPRA_TOUCH_LOOKANDFEEL
			x += BUTTON_WIDTH/2 - iw/2;
#endif // Touch
			y = lMargin;
		}
		y += (iw+8) * (HEART_POINTS - 1);
		lHeartBeatLoopTime = lHeartBeatTime;
		for (int i = -HEART_POINTS/2; i < +HEART_POINTS/2; ++i, lHeartBeatLoopTime += 0.6f)
		{
			if (mGame->GetComputerIndex() != 1)
			{
				Vector3DF v(x+iw/2, y+iw/2, lAngle);
				if (i >= lBalance)
				{
					const float lHeartBeatFactor = (lHeartBeatLoopTime < 2*PIF) ? 1 : 1+0.15f*sin(lHeartBeatLoopTime);
					const float hw = iw * lHeartBeatFactor;
					const float hh = ih * lHeartBeatFactor;
					v = mHeartPos[lHeartIndex] = Math::Lerp(mHeartPos[lHeartIndex], v, 0.07f);
					++lHeartIndex;
					DrawImage(mHeart->GetData(), v.x, v.y, hw, hh, v.z);
				}
				else
				{
					DrawImage(mGreyHeart->GetData(), v.x, v.y, iw, ih, v.z);
				}
				y -= iw+8;
			}
		}
		//mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_NORMAL);
	}
}

void App::DrawKeys()
{
#ifndef LEPRA_TOUCH_LOOKANDFEEL
	// Draw keyboard buttons.
	if (mGame->GetFlybyMode() == Game::FLYBY_INACTIVE && !mStartTimer.IsStarted())
	{
		mStartTimer.Start();
	}
	if (mStartTimer.IsStarted() && mStartTimer.QueryTimeDiff() < 7.0 &&
		mKeyboardButton->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		const float w = (float)mUiManager->GetCanvas()->GetWidth();
		const float h = (float)mUiManager->GetCanvas()->GetHeight();

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
}

void App::DrawInfoTexts() const
{
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	if (!mIsPaused)
	{
		const Color c = mUiManager->GetPainter()->GetColor(0);
		mUiManager->GetPainter()->SetColor(mInfoTextColor, 0);

		for (size_t x = 0; x < mInfoTextArray.size(); ++x)
		{
			const InfoTextData& lData = mInfoTextArray[x];
			PrintText(lData.mText, -lData.mAngle, (int)lData.mCoord.x, (int)lData.mCoord.y);
		}

		mUiManager->GetPainter()->SetColor(c, 0);
	}
#endif // Touch

	ClearInfoTexts();
}

void App::ClearInfoTexts() const
{
	mInfoTextArray.clear();
}

bool App::PreDrawHud()
{
	if ((mGame->GetFlybyMode() != Game::FLYBY_INACTIVE && mGame->GetFlybyMode() != Game::FLYBY_INTRODUCTION_FINISHING_UP) || mDialog)
	{
		return false;
	}

	Cure::ContextObject* lAvatar1 = mGame->GetP1();
	Cure::ContextObject* lAvatar2 = mGame->GetP2();
	if (!lAvatar1 || !lAvatar1->IsLoaded() || !lAvatar2 || !lAvatar2->IsLoaded())
	{
		return false;
	}


	mFlipDraw = mGame->IsFlipRenderSide();

	return true;
}



void App::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
{
	Transpose (cx, cy, pAngle);
	cx -= 0.5f;

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

void App::DrawRoundedPolygon(int x, int y, int pRadius, const Color& pColor, float pScaleX, float pScaleY) const
{
	DrawRoundedPolygon((float)x, (float)y, (float)pRadius, pColor, pScaleX, pScaleY, 20);
}

void App::DrawRoundedPolygon(float x, float y, float pRadius, const Color& pColor, float pScaleX, float pScaleY, int pCornerRadius) const
{
	float lAngle = 0;
	Transpose(x, y, lAngle);

	mPenX = (float)x;
	mPenY = (float)y;
	const float dx = pRadius * pScaleX;
	const float dy = pRadius * pScaleY;
	std::vector<Vector2DF> lCoords;
	// Start in center.
	lCoords.push_back(Vector2DF(x, y));
	UiTbc::RectComponent::AddRadius(lCoords, (int)Math::Round(x-dx)+pCornerRadius, (int)Math::Round(y-dy)+pCornerRadius, pCornerRadius, +PIF/2, 0);
	UiTbc::RectComponent::AddRadius(lCoords, (int)Math::Round(x+dx)-pCornerRadius, (int)Math::Round(y-dy)+pCornerRadius, pCornerRadius, 0,      -PIF/2);
	UiTbc::RectComponent::AddRadius(lCoords, (int)Math::Round(x+dx)-pCornerRadius, (int)Math::Round(y+dy)-pCornerRadius, pCornerRadius, -PIF/2, -PIF);
	UiTbc::RectComponent::AddRadius(lCoords, (int)Math::Round(x-dx)+pCornerRadius, (int)Math::Round(y+dy)-pCornerRadius, pCornerRadius, +PIF,   +PIF/2);
	// Back to start.
	lCoords.push_back(lCoords[1]);
	mUiManager->GetPainter()->SetColor(pColor, 0);
	mUiManager->GetPainter()->SetAlphaValue(pColor.mAlpha);
	mUiManager->GetPainter()->DrawFan(lCoords, true);
	mUiManager->GetPainter()->SetAlphaValue(255);
}

void App::DrawMeter(int x, int y, float pAngle, float pSize, float pMinValue, float pMaxValue, int pWidth, int pSpacing, int pBarCount) const
{
	Transpose(x, y, pAngle);

	mPenX = (float)x;
	mPenY = (float)y;

	Color lStartColor = RED;
	Color lEndColor = GREEN;
	const int lBarCount = pBarCount;
	const int lBarSpace = pSpacing;
	const int lBarHeight = (int)(pSize/lBarCount) - lBarSpace;
	const int lBarWidth = pWidth;
	int lMaxValue = (int)(pMaxValue * lBarCount);
	int lMinValue = (int)(pMinValue * lBarCount);
	if (pMaxValue <= 0)
	{
		lMaxValue = -1;
	}
	const int lXStep = -(int)(::sin(pAngle)*(lBarHeight+lBarSpace));
	const int lYStep = -(int)(::cos(pAngle)*(lBarHeight+lBarSpace));
	const bool lXIsMain = ::abs(lXStep) >= ::abs(lYStep);
	x -= (int)(lXStep * (lBarCount-1) * 0.5f);
	y -= (int)(lYStep * (lBarCount-1) * 0.5f);
	for (int i = 0; i < lBarCount; ++i)
	{
		const Color c = (i >= lMinValue && i <= lMaxValue)?
			Color(lStartColor, lEndColor, i/(float)lBarCount) : DARK_GRAY;
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
	}
}

void App::DrawTapIndicator(int pTag, int dx, int dy, float pAngle) const
{
	pAngle = -pAngle;
	if (mFlipDraw)
	{
		//dx = -dx;
		//dy = -dy;
		pAngle += PIF;
	}
	Vector2DF lPoint((float)dx, (float)dy);
	lPoint.RotateAround(Vector2DF(), pAngle);
	int x = (int)(mPenX + lPoint.x);
	int y = (int)(mPenY + lPoint.y);
	int x2 = x + (int)(20*::cos(pAngle));
	int y2 = y + (int)(20*::sin(pAngle));
	mUiManager->GetPainter()->SetColor(WHITE);
	FingerMoveList::iterator i = gFingerMoveList.begin();
	for (; i != gFingerMoveList.end(); ++i)
	{
		if (i->mTag != pTag)
		{
			continue;
		}
		int x3 = i->mLastY;
		int y3 = mUiManager->GetCanvas()->GetHeight() - i->mLastX;
		//Transpose(x3, y3, lAngle);
		mUiManager->GetPainter()->DrawLine(x3, y3, x2, y2);
		mUiManager->GetPainter()->DrawLine(x2, y2, x, y);
	}
}

void App::DrawBarrelCompass(int x, int  y, float pAngle, int pSize, float pValue1, float pValue2) const
{
	Transpose(x, y, pAngle);

	mPenX = (float)x;
	mPenY = (float)y;

	float ca = ::cos(pAngle);
	ca = (::fabs(ca) < 1e-5)? 0 : ca;
	float sa = ::sin(pAngle);
	sa = (::fabs(sa) < 1e-5)? 0 : sa;
	const int lLineCount = BARREL_COMPASS_LINE_COUNT;
	const int lLineSpacing = BARREL_COMPASS_LINE_SPACING;
	const int lWidth = BARREL_COMPASS_HEIGHT;
	const int w2 = lWidth/2;
	const int s2 = pSize/2;
	const int s3 = pSize/3;
	const int s23 = pSize*2/3;
	mUiManager->GetPainter()->SetColor(DARK_GRAY);
	for (int i = 0; i < lLineCount; ++i)
	{
		const int lHeight = (i%3 == 0)? s23 : s3;
		const int x2 = -w2 + i*lLineSpacing;
		const int y2 = s2-lHeight;
		mUiManager->GetPainter()->DrawLine((int)(x-x2*ca+s2*sa), (int)(y+y2*ca+x2*sa), (int)(x-x2*ca+y2*sa), (int)(y+s2*ca+x2*sa));
	}
	const int lValue1 = (int)((pValue1-0.5f)*-lWidth);
	int lValue2 = (int)((pValue2-0.5f)*-lWidth);
	mUiManager->GetPainter()->SetColor(RED);
	--lValue2;
	int xca = (int)(lValue2*ca);
	int ysa = (int)(lValue2*sa);
	mUiManager->GetPainter()->DrawLine(x+xca, y+ysa-(int)(s2*ca), x+xca-(int)(s2*sa), y+ysa);
	xca += (int)Math::Round(2*ca);
	ysa += (int)Math::Round(2*sa);
	mUiManager->GetPainter()->DrawLine(x+xca, y+ysa-(int)(s2*ca), x+xca-(int)(s2*sa), y+ysa);
	mUiManager->GetPainter()->SetColor(WHITE);
	mUiManager->GetPainter()->DrawLine((int)(x+lValue1*ca+s2*sa), (int)(y-s2*ca+lValue1*sa), (int)(x+lValue1*ca-s2*sa), (int)(y+s2*ca+lValue1*sa));
}

void App::InfoText(int pPlayer, const str& pInfo, float pAngle, float dx, float dy) const
{
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	if (mFlipDraw)
	{
		dx = -dx;
		dy = -dy;
		pAngle += PIF;
	}
#endif // Touch

	const double lLastTime = (pPlayer == 1)? mPlayer1LastTouch.QueryTimeDiff() : mPlayer2LastTouch.QueryTimeDiff();
	const double lShowDelayTime = (pPlayer == 1)? mPlayer1TouchDelay.QueryTimeDiff() : mPlayer2TouchDelay.QueryTimeDiff();
	if (lLastTime < 15)	// Delay until shown.
	{
		if (lShowDelayTime > 2)	// Delay after next touch until hidden.
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
	if (mGame->GetComputerIndex() == -1)
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
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		tx2 = tx;
		ty2 = ty;
#else // Computer L&F
		tx2 = s;
		ty2 = ty;
		if (mGame->GetComputerIndex() == 1 && !mGetiPhoneButton)
		{
			tx2 = tx;
			ty2 = ty;
		}
#endif // Touch L&F / Computer L&F
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
	if (!mGame || mGame->GetFlybyMode() != Game::FLYBY_INACTIVE || mDialog || mIsPaused)
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
			mGame->GetCutie()->DrainHealth(1);
		}
		break;
		case UIKEY(6):
		{
			if (!pFactor)
			{
				const Cure::ObjectPositionalData* lPosition = 0;
				lAvatar1->UpdateFullPosition(lPosition);
				if (lPosition)
				{
					Cure::ObjectPositionalData* lNewPlacement = (Cure::ObjectPositionalData*)lPosition->Clone();
					lNewPlacement->mPosition.mTransformation.GetPosition().x += 30;
					lNewPlacement->mPosition.mTransformation.GetPosition().y += 20;
					lNewPlacement->mPosition.mTransformation.GetPosition().z += 15;
					lAvatar1->SetFullPosition(*lNewPlacement);
				}
			}
		}
		break;
		case UIKEY(5):
		{
			if (!pFactor)
			{
				mGame->SetComputerIndex((mGame->GetComputerIndex() == 0)? 1 : 0);
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
				mGame->AddScore(Random::Uniform(500, 5000), Random::Uniform(500, 5000));
				EnterHiscore(str(), WHITE);
			}
		}
		break;
#endif // Debug
	}

	if (mGame->IsFlipRenderSide())
	{
		//std::swap(lAvatar1, lAvatar2);
		lAvatar = (lAvatar == lAvatar1)? lAvatar2 : lAvatar1;
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
	(void)pMovement;
	int lTag = 0;

#ifdef LEPRA_TOUCH_LOOKANDFEEL
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
		return lTag;
	}

	float x = (float)pMovement.mLastX;
	float y = (float)pMovement.mLastY;
	float lStartX = (float)pMovement.mStartX;
	float lStartY = (float)pMovement.mStartY;

	const float w = (float)mUiManager->GetCanvas()->GetWidth();
	const float h = (float)mUiManager->GetCanvas()->GetHeight();
	std::swap(x, y);
	std::swap(lStartX, lStartY);
	/*const float lTapMargin = 28.0f;
	lStartX = Math::Clamp(lStartX, lTapMargin, w-lTapMargin);
	lStartY = Math::Clamp(lStartY, lTapMargin, h-lTapMargin);*/
	y = h-y;
	lStartY = h-lStartY;
	const float lSingleWidth = 130;
	const float lDoubleWidth = 190;
	const float s = lDoubleWidth / 2;
	enum Directive
	{
		DIRECTIVE_NONE,
		DIRECTIVE_UP_DOWN,
		DIRECTIVE_LEFT_RIGHT,
		DIRECTIVE_FUNCTION,
	};
	Directive directive = DIRECTIVE_NONE;
	bool lIsRightControls = false;
	float lValue = 0;
#define SCALEUP(v)	(v)*2
#define CLAMPUP(v)	Math::Clamp((v)*2, -1.0f, 1.0f)
	if (mGame->GetComputerIndex() != -1)
	{
		// Single play, both Cutie and Launcher.
		if (lStartX <= lDoubleWidth && lStartY >= h-lDoubleWidth)	// Up/down?
		{
			directive = DIRECTIVE_UP_DOWN;
			lValue = SCALEUP((lStartY-y)/s);
		}
		else if (lStartX >= w-lDoubleWidth && lStartY >= h-lDoubleWidth)	// Left/right?
		{
			directive = DIRECTIVE_LEFT_RIGHT;
			lValue = SCALEUP((x-lStartX)/s);
		}
		else if (x >= w/2-s && x <= w/2+s && y >= h-lDoubleWidth)	// Bomb?
		{
			directive = DIRECTIVE_FUNCTION;
		}
		if (mGame->GetComputerIndex() != 1)
		{
			if (directive == DIRECTIVE_UP_DOWN)
			{
				lValue = -lValue;
			}
			lIsRightControls = true;
		}
	}
	else
	{
		// Cutie dual play = portrait layout.
		if (lStartX <= lDoubleWidth && lStartY <= lSingleWidth)	// P1 up/down?
		{
			directive = DIRECTIVE_UP_DOWN;
			lValue = SCALEUP((x-lStartX)/s);
		}
		else if (lStartX <= lDoubleWidth && lStartY >= h-lSingleWidth)	// P1 left/right?
		{
			directive = DIRECTIVE_LEFT_RIGHT;
			lValue = SCALEUP((y-lStartY)/s);
		}
		else if (x <= lSingleWidth && y >= h/2-s && y <= h/2+s)	// Bomb?
		{
			directive = DIRECTIVE_FUNCTION;
		}

		// Launcher in portrait mode.
		if (lStartX >= w-lDoubleWidth && lStartY >= h-lSingleWidth)	// P2 up/down?
		{
			lIsRightControls = true;
			directive = DIRECTIVE_UP_DOWN;
			lValue = CLAMPUP((x-lStartX)/s);
		}
		else if (lStartX >= w-lDoubleWidth && lStartY <= lSingleWidth)	// P1 left/right?
		{
			lIsRightControls = true;
			directive = DIRECTIVE_LEFT_RIGHT;
			lValue = CLAMPUP((lStartY-y)/s);
		}
		else if (x >= w-lSingleWidth && y >= h/2-s && y <= h/2+s)	// Bomb?
		{
			lIsRightControls = true;
			directive = DIRECTIVE_FUNCTION;
		}
	}

	pMovement.UpdateDistance();

	{
		UiCure::CppContextObject* lAvatar = mGame->GetP2();
		/*if (mGame->IsFlipRenderSide())
		{
			lAvatar = mGame->GetP1();
		}*/
		switch (directive)
		{
			case DIRECTIVE_NONE:
			{
			}
			break;
			case DIRECTIVE_UP_DOWN:
			{
				if (mGame->IsFlipRenderSide())
				{
					lValue = -lValue;
				}
				if (mGame->IsFlipRenderSide() == lIsRightControls)
				{
					mIsThrottling = true;
					mThrottle = Math::Clamp(mBaseThrottle+lValue, -1.0f, +1.0f);
					if (!pMovement.mIsPress)
					{
						mBaseThrottle = mThrottle;
						if (pMovement.mMovedDistance < 20 &&
							pMovement.mTimer.QueryTimeDiff() < 0.3f)	// Go to neutral if just tap/release.
						{
							mIsThrottling = false;
							mBaseThrottle = 0.0f;
							mThrottle = 0.0f;
						}
					}
					mPlayer1LastTouch.ClearTimeDiff();
					lTag = 1;
				}
				else
				{
					mGame->SetThrottle(lAvatar, lValue);
					mPlayer2LastTouch.ClearTimeDiff();
					lTag = 3;
				}
			}
			break;
			case DIRECTIVE_LEFT_RIGHT:
			{
				if (mGame->IsFlipRenderSide() == lIsRightControls)
				{
					mSteering = Math::Clamp(mBaseSteering+lValue, -1.0f, 1.0f);
					if (!pMovement.mIsPress)
					{
						mBaseSteering = mSteering;
						if (pMovement.mMovedDistance < 20 &&
							pMovement.mTimer.QueryTimeDiff() < 0.3f)	// Go to neutral if just tap/release.
						{
							mBaseSteering = 0.0f;
							mSteering = 0;
						}
					}
					mPlayer1LastTouch.ClearTimeDiff();
					lTag = 2;
				}
				else
				{
					lAvatar->SetEnginePower(1, lValue, 0);
					mPlayer2LastTouch.ClearTimeDiff();
					lTag = 4;
				}
			}
			break;
			case DIRECTIVE_FUNCTION:
			{
				if (mGame->IsFlipRenderSide() != lIsRightControls)
				{
					mGame->Shoot();
					mPlayer2LastTouch.ClearTimeDiff();
					lTag = 5;
				}
			}
			break;
		}
	}
	pMovement.mTag = lTag;
#endif // Touch L&F
	return lTag;
}



void App::MainMenu()
{
	// TRICKY: leave these here, since this call comes from >1 place.
	mGameOverTimer.Stop();
	mGame->EnableScoreCounting(false);
	mGame->ResetWinnerIndex();
	mGame->SetFlybyMode(Game::FLYBY_PAUSE);
	mGame->SetHeartBalance(0);
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
	const str lLevelName = gLevels[mHiscoreLevelIndex];
	const str lVehicleName = gVehicles[mHiscoreVehicleIndex];
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
	UiTbc::Button* lPrevLevelButton = ICONBTN(_T("btn_prev.png"), GET_NAME(mHiscoreLevelIndex-1, gLevels));
	UiTbc::Button* lNextLevelButton = ICONBTN(_T("btn_next.png"), GET_NAME(mHiscoreLevelIndex+1, gLevels));
	UiTbc::Button* lPrevAvatarButton = ICONBTN(_T("btn_prev.png"), GET_NAME(mHiscoreVehicleIndex-1, gVehicles));
	UiTbc::Button* lNextAvatarButton = ICONBTN(_T("btn_next.png"), GET_NAME(mHiscoreVehicleIndex+1, gVehicles));
	lMainMenuButton->SetPreferredSize(d->GetPreferredWidth() / 2, d->GetPreferredHeight());
	lPrevLevelButton->SetPreferredSize(57, 57);
	lNextLevelButton->SetPreferredSize(lPrevLevelButton->GetPreferredSize());
	lPrevAvatarButton->SetPreferredSize(lPrevLevelButton->GetPreferredSize());
	lNextAvatarButton->SetPreferredSize(lPrevLevelButton->GetPreferredSize());
	d->AddButton(-1, lMainMenuButton);
	d->AddButton(-2, lPrevLevelButton);
	d->AddButton(-3, lNextLevelButton);
	d->AddButton(-4, lPrevAvatarButton);
	d->AddButton(-5, lNextAvatarButton);
	lMainMenuButton->SetPos(d->GetPreferredWidth()/4, 0);
	lPrevLevelButton->SetPos(20, d->GetPreferredHeight()/2 - 57-15);
	lNextLevelButton->SetPos(d->GetPreferredWidth()-20-57, d->GetPreferredHeight()/2 - 57-15);
	lPrevAvatarButton->SetPos(20, d->GetPreferredHeight()/2 +15);
	lNextAvatarButton->SetPos(d->GetPreferredWidth()-20-57, d->GetPreferredHeight()/2 +15);
	if (!mHiscoreAgent)
	{
		UpdateHiscore(true);
	}
}

void App::EnterHiscore(const str& pMessage, const Color& pColor)
{
	mGameOverTimer.Stop();
	mGame->ResetWinnerIndex();

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

void App::SuperReset(bool pGameOver)
{
	mGameOverTimer.Stop();

	mPlayer1LastTouch.PopTimeDiff();
	mPlayer2LastTouch.PopTimeDiff();

	mIsThrottling = false;
	mThrottle = 0;
	mBaseThrottle = 0;
	mSteering = 0;
	mBaseSteering = 0;
	mGame->SyncCameraPositions();
	mGame->EndSlowmo();
	if (pGameOver)
	{
		// Total game over (someone won the match)?
		const int lHeartBalance = mGame->GetHeartBalance();
		if (lHeartBalance == -HEART_POINTS/2 || lHeartBalance == +HEART_POINTS/2)
		{
			mGame->SetComputerIndex(mGame->GetComputerIndex());	// Force flip side reset.
			mGame->ResetScore();
			MainMenu();
			return;
		}

		// Nope, simply reload the interior. Go another round.
		const int lComputerIndex = mGame->GetComputerIndex();
		switch (lComputerIndex)
		{
			case -1:	mGame->FlipRenderSides();								break;
			case 0:		mGame->SetComputerIndex(1);	mGame->SetHeartBalance(-mGame->GetHeartBalance());	break;
			case 1:		mGame->SetComputerIndex(0);	mGame->SetHeartBalance(-mGame->GetHeartBalance());	break;
		}
		mGame->NextRound();
	}
	else
	{
		// Restart level.
		mGame->SetHeartBalance(0);
		if (mGame->IsFlipRenderSide())
		{
			mGame->FlipRenderSides();
		}
		mGame->SetComputerIndex(mGame->GetComputerIndex());
		mGame->ResetScore();
	}
	mGame->ResetWinnerIndex();
	mGame->SetVehicle(mGame->GetVehicle());
	mGame->ResetLauncher();
	mResourceManager->Tick();
	strutil::strvec lResourceTypes;
	lResourceTypes.push_back(_T("RenderImg"));
	lResourceTypes.push_back(_T("Geometry"));
	lResourceTypes.push_back(_T("GeometryRef"));
	lResourceTypes.push_back(_T("Physics"));
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
	mHiscoreAgent = new Cure::HiscoreAgent(lHost, 80, _T("kill_cutie"));
	//mHiscoreAgent = new Cure::HiscoreAgent(_T("localhost"), 8080, _T("kill_cutie"));
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

void App::OnOk(UiTbc::Button*)
{
	MainMenu();
}

void App::OnMainMenuAction(UiTbc::Button* pButton)
{
	const int lPreviousComputerIndex = mGame->GetComputerIndex();
	switch (pButton->GetTag())
	{
		case 1:
		{
			// 1P
			const int lComputerIndex = ((Random::GetRandomNumber() % 256) >= 128);
			mGame->SetComputerIndex(lComputerIndex);
		}
		break;
		case 2:
		{
			// 2P
			mGame->SetComputerIndex(-1);
#ifdef LEPRA_TOUCH
			// Dual play requires original canvas orientation.
			mUiManager->GetCanvas()->SetOutputRotation(90);
#endif // Touch
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
	// Switched from single play to dual play or vice versa?
	if ((lPreviousComputerIndex == -1) != (mGame->GetComputerIndex() == -1))
	{
		mStartTimer.Stop();	// Show keys again on the computer screen.
	}
	UiTbc::Dialog* d = CreateTbcDialog(&App::OnLevelAction);
	d->SetQueryLabel(_T("Select level"), mBigFontId);
	d->AddButton(1, ICONBTN(_T("btn_tutorial.png"), _T("Tutorial")));
	d->AddButton(2, ICONBTN(_T("btn_lvl2.png"), gLevels[0]));
	d->AddButton(3, ICONBTN(_T("btn_lvl3.png"), gLevels[1]));
	d->AddButton(4, ICONBTN(_T("btn_lvl4.png"), gLevels[2]));
	if (mIsMoneyIconAdded && !CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_CONTENT_LEVELS, false))
	{
		AddCostIcon(gLevels[1]);
		AddCostIcon(gLevels[2]);
	}
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
			const str lLevelName = gLevels[mHiscoreLevelIndex];
			const str lVehicleName = gVehicles[mHiscoreVehicleIndex];
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
		mGame->ResetWinnerIndex();
		mGame->SetVehicle(_T("cutie"));
		mGame->ResetLauncher();
		mGame->SetComputerDifficulty(0);
		delete mIntroStreamer;
		mIntroStreamer = mUiManager->GetSoundManager()->CreateSoundStream(mPathPrefix+_T("voice_intro.ogg"), UiLepra::SoundManager::LOOP_NONE, 0);
		if (mIntroStreamer && mIntroStreamer->Playback())
		{
			if (mMusicPlayer)
			{
				mMusicPlayer->SetVolume(0.2f);
			}
			return;
		}
		delete mIntroStreamer;
		mIntroStreamer = 0;
		mLog.AError("Oops! Unable to play tutorial voice-over. Skipping tutorial.");
		// If all else fails, fall thru add keep going. Makes more sense to the user than
		// a tutorial without any instructions.
	}
	UiTbc::Dialog* d = CreateTbcDialog(&App::OnVehicleAction);
	d->SetQueryLabel(_T("Select vehicle"), mBigFontId);
	d->AddButton(1, ICONBTN(_T("btn_cutie.png"), gVehicles[0]));
	d->AddButton(2, ICONBTN(_T("btn_hardie.png"), gVehicles[1]));
	d->AddButton(3, ICONBTN(_T("btn_speedie.png"), gVehicles[2]));
	d->AddButton(4, ICONBTN(_T("btn_sleepie.png"), gVehicles[3]));
	if (mIsMoneyIconAdded && !CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_CONTENT_VEHICLES, false))
	{
		AddCostIcon(gVehicles[1]);
		AddCostIcon(gVehicles[2]);
		AddCostIcon(gVehicles[3]);
	}
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
		UiTbc::Button* lScrollButton = ICONBTNA("btn_scroll.png", "");
		lScrollButton->SetPreferredSize(PixelCoord(57, 57), false);
		mDifficultySlider = new UiTbc::ScrollBar(UiTbc::ScrollBar::HORIZONTAL,
			mScrollBarImage->GetUserData(0), 0, 0, lScrollButton);
		mDifficultySlider->SetScrollRatio(44, mScrollBarImage->GetRamData()->GetWidth());
		mDifficultySlider->SetScrollPos((mGame->GetComputerDifficulty() < 0)? 0.0 : mGame->GetComputerDifficulty());
		mDifficultySlider->SetPreferredSize(mScrollBarImage->GetRamData()->GetWidth()+15*2, 44);
		d->AddChild(mDifficultySlider);
		const int x = d->GetPreferredWidth()/2 - mDifficultySlider->GetPreferredWidth()/2;
		const int y = d->GetPreferredHeight() - 60;
		mDifficultySlider->SetPos(x, y);

		mDifficultyLabel = new UiTbc::Label();
		d->AddChild(mDifficultyLabel);
		mDifficultyLabel->SetPos(x+15, y-3);

		d->UpdateLayout();
	}

	strutil::strvec lResourceTypes;
	lResourceTypes.push_back(_T("RenderImg"));
	lResourceTypes.push_back(_T("Geometry"));
	lResourceTypes.push_back(_T("GeometryRef"));
	lResourceTypes.push_back(_T("Physics"));
	lResourceTypes.push_back(_T("RamImg"));
	mResourceManager->ForceFreeCache(lResourceTypes);
	mResourceManager->ForceFreeCache(lResourceTypes);	// Call again to release any dependent resources.
}

void App::OnVehicleAction(UiTbc::Button* pButton)
{
	mDifficultyLabel = 0;
	mDifficultySlider = 0;

	if (pButton->GetTag() >= 2 && !CURE_RTVAR_SLOW_GET(mVariableScope, RTVAR_CONTENT_VEHICLES, false))
	{
		Purchase(_T(CONTENT_VEHICLES));
		return;
	}

	mGame->EnableScoreCounting(true);
	str lVehicle = _T("cutie");
	switch (pButton->GetTag())
	{
		case 1:	lVehicle = _T("cutie");		mHiscoreVehicleIndex = 0;	break;
		case 2:	lVehicle = _T("monster");	mHiscoreVehicleIndex = 1;	break;
		case 3:	lVehicle = _T("corvette");	mHiscoreVehicleIndex = 2;	break;
		case 4:	lVehicle = _T("road_roller");	mHiscoreVehicleIndex = 3;	break;
	}
	SuperReset(false);
	mGame->ResetWinnerIndex();
	mGame->SetVehicle(lVehicle);
	mGame->ResetLauncher();
	mGame->SetFlybyMode(Game::FLYBY_INACTIVE);
	mIsPaused = false;
	mPlaybackVoiceInstruction = true;
}

void App::OnHiscoreAction(UiTbc::Button* pButton)
{
	delete mHiscoreAgent;
	mHiscoreAgent = 0;
	switch (pButton->GetTag())
	{
		case -1:
		{
			MainMenu();
		}
		break;
		case -2:
		{
			mHiscoreLevelIndex = GET_NAME_INDEX(mHiscoreLevelIndex-1, gLevels);
			HiscoreMenu(-1);
		}
		break;
		case -3:
		{
			mHiscoreLevelIndex = GET_NAME_INDEX(mHiscoreLevelIndex+1, gLevels);
			HiscoreMenu(+1);
		}
		break;
		case -4:
		{
			mHiscoreVehicleIndex = GET_NAME_INDEX(mHiscoreVehicleIndex-1, gVehicles);
			HiscoreMenu(-1);
		}
		break;
		case -5:
		{
			mHiscoreVehicleIndex = GET_NAME_INDEX(mHiscoreVehicleIndex+1, gVehicles);
			HiscoreMenu(+1);
		}
		break;
	}
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
	const bool lIsGameOver = (mGame->GetHeartBalance() == -HEART_POINTS/2 || mGame->GetHeartBalance() == +HEART_POINTS/2);
	if (mIsPaused || mDialog || (mGame->GetWinnerIndex() >= 0 && lIsGameOver))
	{
		return;
	}
	mIsPaused = true;
	if (mIntroStreamer)
	{
		mIntroStreamer->Pause();
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
	if (mIsPaused || mDialog)
	{
		return;
	}
	OnTapSound(pButton);
	DoPause();
}

void App::OnPauseAction(UiTbc::Button* pButton)
{
	mPlayer1LastTouch.PopTimeDiff();
	mPlayer2LastTouch.PopTimeDiff();
	mPlayer1TouchDelay.PopTimeDiff();
	mPlayer1TouchDelay.ReduceTimeDiff(-10);
	mPlayer2TouchDelay.PopTimeDiff();
	mPlayer2TouchDelay.ReduceTimeDiff(-10);

	mIsPaused = false;
	if (mIntroStreamer && !mIntroStreamer->IsPlaying())
	{
		mIntroStreamer->Playback();
	}
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
	mDoLayout = true;
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

void App::AddCostIcon(const str& pName)
{
	UiTbc::Label* lLabel = new UiTbc::Label;
	lLabel->SetIcon(mMoney->GetData());
	//lLabel->SetAdaptive(true);
	UiTbc::Button* lButton = (UiTbc::Button*)mDialog->GetChild(pName, 0);
	lButton->GetClientRectComponent()->ReplaceLayer(0, new UiTbc::FloatingLayout);
	lButton->AddChild(lLabel, 57-14, 57-9);
	mDialog->UpdateLayout();
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
