
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
	static void OnTap(float x, float y);
	//void OnInput(UiLepra::InputElement* pElement);

private:
	Graphics GetGraphics();
	bool Open();
	void Close();
	void UpdateSettings();
	virtual void Init();
	virtual int Run();
	bool Poll();
	void Logic();

	void BeginRender(const Vector3DF& pBackgroundColor);
	void Render(const PixelRect& pArea);
	void Paint();
	void PreparePaint();
	void EndRender();
	void Clear(float pRed, float pGreen, float pBlue, bool pClearDepth = true);
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

	static App* mApp;
#ifdef LEPRA_IOS
	AnimatedApp* mAnimatedApp;
#endif // iOS
	SlimeVolleyball* mGame;

	UiLepra::DisplayManager* mDisplay;
	Canvas* mCanvas;
	UiTbc::Renderer* mRenderer;
	UiTbc::Painter* mPainter;
	UiTbc::FontManager* mFontManager;
	UiTbc::DesktopWindow* mDesktopWindow;
	UiLepra::InputManager* mInput;
	UiLepra::SoundManager* mSound;
	UiLepra::SoundStream* mMusicStreamer;
	str mPathPrefix;
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
	Application(pArgumentList)
{
	mApp = this;
}

App::~App()
{
	//delete (mUiManager);
	//mUiManager = 0;
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

void App::OnTap(float x, float y)
{
	mApp->mGame->MoveTo(y, x);
}

/*void App::OnInput(UiLepra::InputElement* pElement)
{
	if (pElement->GetParentDevice() == mInput->GetMouse() &&
		pElement->GetType() == UiLepra::InputElement::DIGITAL &&
		pElement->GetBooleanValue())
	{
		Event lEvent;
		lEvent.id = 501;
		mGame->handleEvent(lEvent);
	}
}*/

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
	const int lDisplayWidth = 580;
	const int lDisplayHeight = 400;
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
			mRenderer = new UiTbc::OpenGLRenderer(mCanvas);
			mPainter = new UiTbc::OpenGLPainter;
		}
		/*else if (pContext == UiLepra::DisplayManager::DIRECTX_CONTEXT)
		{
			mRenderer = new TBC::Direct3DRenderer(mCanvas);
			mPainter = new TBC::DirectXPainter;
		}*/
	}

	if (lOk)
	{
		mFontManager = UiTbc::FontManager::Create(mDisplay);
		//mFontManager->SetColor(Color(255, 255, 255, 255), 0);
		//mFontManager->SetColor(Color(0, 0, 0, 0), 1);
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
		/*if (mInput->GetMouse())
		{
			class AppInputFunctor: public UiLepra::InputFunctor
			{
			public:
				inline AppInputFunctor(App* pApp):
					mApp(pApp)
				{
				}
			private:
				inline void Call(UiLepra::InputElement* pElement)
				{
					mApp->OnInput(pElement);
				}
				inline UiLepra::InputFunctor* CreateCopy() const
				{
					return new AppInputFunctor(mApp);
				}
				App* mApp;
			};
			mInput->GetMouse()->AddFunctor(new AppInputFunctor(this));
			mInput->ActivateAll();
		}*/
#endif // !iOS
	}
	if (lOk)
	{
		mDesktopWindow = new UiTbc::DesktopWindow(mInput, mPainter, new UiTbc::FloatingLayout(), 0, 0);
		mDesktopWindow->SetIsHollow(true);
		mDesktopWindow->SetPreferredSize(mCanvas->GetWidth(), mCanvas->GetHeight());
		mLazyButton = new UiTbc::Button(Color(0, 192, 0), _T(""));
		mLazyButton->SetText(_T("Lazy"));
		mLazyButton->SetPreferredSize(150, 50);
		mDesktopWindow->AddChild(mLazyButton);
		mLazyButton->SetPos(20, 20);
		mLazyButton->SetOnClick(App, OnSpeedClick);
		mLazyButton->SetVisible(false);
		mHardButton = new UiTbc::Button(Color(192, 192, 0), _T(""));
		mHardButton->SetText(_T("Hard"));
		mHardButton->SetPreferredSize(150, 50);
		mDesktopWindow->AddChild(mHardButton);
		mHardButton->SetPos(20, 100);
		mHardButton->SetOnClick(App, OnSpeedClick);
		mHardButton->SetVisible(false);
		mOriginalButton = new UiTbc::Button(Color(192, 0, 0), _T(""));
		mOriginalButton->SetText(_T("Original"));
		mOriginalButton->SetPreferredSize(150, 50);
		mDesktopWindow->AddChild(mOriginalButton);
		mOriginalButton->SetOnClick(App, OnSpeedClick);
		mOriginalButton->SetPos(20, 180);
		mOriginalButton->SetVisible(false);

		m1PButton = new UiTbc::Button(Color(128, 64, 0), _T(""));
		m1PButton->SetText(_T("1P"));
		m1PButton->SetPreferredSize(150, 50);
		mDesktopWindow->AddChild(m1PButton);
		m1PButton->SetOnClick(App, OnPClick);
		m1PButton->SetPos(20, 70);
		m1PButton->SetVisible(false);
		m2PButton = new UiTbc::Button(Color(128, 170, 160), _T(""));
		m2PButton->SetText(_T("2P"));
		m2PButton->SetPreferredSize(150, 50);
		mDesktopWindow->AddChild(m2PButton);
		m2PButton->SetOnClick(App, OnPClick);
		m2PButton->SetPos(20, 160);
		m2PButton->SetVisible(false);

		mNextButton = new UiTbc::Button(Color(0, 255, 0), _T(""));
		mNextButton->SetText(_T("Next"));
		mNextButton->SetPreferredSize(150, 50);
		mDesktopWindow->AddChild(mNextButton);
		mNextButton->SetOnClick(App, OnFinishedClick);
		mNextButton->SetPos(20, 100);
		mNextButton->SetVisible(false);
		mResetButton = new UiTbc::Button(Color(255, 0, 0), _T(""));
		mResetButton->SetText(_T("Reset"));
		mResetButton->SetPreferredSize(150, 50);
		mDesktopWindow->AddChild(mResetButton);
		mResetButton->SetOnClick(App, OnFinishedClick);
		mResetButton->SetPos(20, 70);
		mResetButton->SetVisible(false);
		mRetryButton = new UiTbc::Button(Color(150, 150, 0), _T(""));
		mRetryButton->SetText(_T("Retry"));
		mRetryButton->SetPreferredSize(150, 50);
		mDesktopWindow->AddChild(mRetryButton);
		mRetryButton->SetOnClick(App, OnFinishedClick);
		mRetryButton->SetPos(20, 160);
		mRetryButton->SetVisible(false);
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
	if (lOk)
	{
		UpdateSettings();
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
	delete (mRenderer);
	mRenderer = 0;
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

void App::UpdateSettings()
{
	mDisplay->SetVSyncEnabled(false);

	bool lEnableLights = false;
	double lAmbientRed = 0;
	double lAmbientGreen = 0;
	double lAmbientBlue = 0;
	bool lEnableTrilinearFiltering = false;
	bool lEnableBilinearFiltering = false;
	bool lEnableMipMapping = false;
	double lFOV = 70.0;
	double lClipNear = 0.1;
	double lClipFar = 500.0;
	//str lShadowsString = _T("Force:Volumes");
	str lShadowsString = _T("Nope");
	if (!mRenderer->IsPixelShadersEnabled())
	{
		// Without pixel shaders the scene becomes darker for some reason. At least on my computer...
		lAmbientRed *= 1.5;
		lAmbientGreen *= 1.5;
		lAmbientBlue *= 1.5;
	}

	mRenderer->SetLightsEnabled(lEnableLights);
	mRenderer->SetAmbientLight((float)lAmbientRed, (float)lAmbientGreen, (float)lAmbientBlue);
	mRenderer->SetTrilinearFilteringEnabled(lEnableTrilinearFiltering);
	mRenderer->SetBilinearFilteringEnabled(lEnableBilinearFiltering);
	mRenderer->SetMipMappingEnabled(lEnableMipMapping);
	mRenderer->SetViewFrustum((float)lFOV, (float)lClipNear, (float)lClipFar);

	UiTbc::Renderer::Shadows lShadowMode = UiTbc::Renderer::NO_SHADOWS;
	UiTbc::Renderer::ShadowHint lShadowType = UiTbc::Renderer::SH_VOLUMES_ONLY;
	bool lForceShadowsOnAll = false;
	if (strutil::StartsWith(lShadowsString, _T("Force:")))
	{
		lShadowsString = lShadowsString.substr(6);
		lForceShadowsOnAll = true;
	}
	if (lShadowsString == _T("Volumes"))
	{
		lShadowMode = UiTbc::Renderer::CAST_SHADOWS;
		lShadowType = UiTbc::Renderer::SH_VOLUMES_ONLY;
	}
	else if (lShadowsString == _T("VolumesAndMaps"))
	{
		lShadowMode = UiTbc::Renderer::CAST_SHADOWS;
		lShadowType = UiTbc::Renderer::SH_VOLUMES_AND_MAPS;
	}
	if (lForceShadowsOnAll)
	{
		lShadowMode = UiTbc::Renderer::FORCE_CAST_SHADOWS;
	}
	mRenderer->SetShadowMode(lShadowMode, lShadowType);
	mRenderer->SetShadowUpdateFrameDelay(60);

	// ----------------------------------------
	// 2D rendering settings.
	UiTbc::Painter::RenderMode lPainterRenderMode = UiTbc::Painter::RM_ALPHABLEND;
	mPainter->SetRenderMode(lPainterRenderMode);

	const bool lSmoothFonts = true;
	mPainter->SetFontSmoothness(lSmoothFonts);

	if (mSound)
	{
		double lSoundRollOff = 0.2;
		mSound->SetRollOffFactor((float)lSoundRollOff);
		double lSoundDoppler = 1.3;
		mSound->SetDopplerFactor((float)lSoundDoppler);
	}
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
		//mUiManager = new UiCure::App(UiCure::GetSettings());
		//lOk = ...;
		lOk = Open();
	}
	if (lOk)
	{
		//lOk = Network::Start();
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
	BeginRender(Vector3DF(0,0,0));
	Render(PixelRect(0, 0, mDisplay->GetWidth(), mDisplay->GetHeight()));
	PreparePaint();
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
		mLazyButton->SetVisible(true);
		mHardButton->SetVisible(true);
		mOriginalButton->SetVisible(true);
	}
}

void App::BeginRender(const Vector3DF& pBackgroundColor)
{
	if (CanRender())
	{
		mRenderer->ResetClippingRect();
		Clear(pBackgroundColor.x, pBackgroundColor.y, pBackgroundColor.z);

		if (mSound)
		{
			float lMasterVolume = 1;
			mSound->SetMasterVolume(lMasterVolume);
		}
	}
	else if (mSound)
	{
		mSound->SetMasterVolume(0);
	}
}

void App::Render(const PixelRect& pArea)
{
	if (CanRender())
	{
		if (pArea.GetWidth() > 0 && pArea.GetHeight() > 0)
		{
			mRenderer->ResetClippingRect();
			//mRenderer->SetClippingRect(pArea);
			mRenderer->SetViewport(pArea);
			mRenderer->RenderScene();
		}
	}
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
		mRenderer->SetAmbientLight(0.1f, 0.1f, 0.1f);
		mPainter->ResetClippingRect();
		mPainter->PrePaint();
	}
}

void App::EndRender()
{
	if (CanRender())
	{
		UpdateSettings();
		mDisplay->UpdateScreen();
	}
}

void App::Clear(float pRed, float pGreen, float pBlue, bool pClearDepth)
{
	//mDisplay->Activate();

	Color lColor;
	lColor.Set(pRed, pGreen, pBlue, 1.0f);
	mRenderer->SetClearColor(lColor);
	unsigned lClearFlags = UiTbc::Renderer::CLEAR_COLORBUFFER;
	if (pClearDepth)
	{
		lClearFlags |= UiTbc::Renderer::CLEAR_DEPTHBUFFER;
	}
	mRenderer->Clear(lClearFlags);
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
		mGame->mSpeed = -6;
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
		Event lEvent;
		lEvent.id = 501;
		mGame->handleEvent(lEvent);
	}
	else if (pButton == mRetryButton)
	{
		Event lEvent;
		lEvent.id = 401;
		lEvent.key = 'c';
		mGame->handleEvent(lEvent);
	}
	else if (pButton == mResetButton)
	{
		mGame->resetGame();
	}
	mNextButton->SetVisible(false);
	mResetButton->SetVisible(false);
	mRetryButton->SetVisible(false);
}



App* App::mApp = 0;
LOG_CLASS_DEFINE(GAME, App);



}
