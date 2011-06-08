
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Lepra/Include/Application.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiLepra/Include/UiDisplayManager.h"
#include "../UiLepra/Include/UiLepra.h"
#include "../UiLepra/Include/UiInput.h"
#include "../UiLepra/Include/UiSoundManager.h"
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

private:
	bool Open();
	void Close();
	void UpdateSettings();
	virtual void Init();
	virtual int Run();
	bool Poll();

	void BeginRender(const Vector3DF& pBackgroundColor);
	void Render(const PixelRect& pArea);
	void Paint();
	void PreparePaint();
	void EndRender();
	void Clear(float pRed, float pGreen, float pBlue, bool pClearDepth = true);
	bool CanRender() const;

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);

	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	SlimeVolleyball* mGame;

	UiLepra::DisplayManager* mDisplay;
	Canvas* mCanvas;
	UiTbc::Renderer* mRenderer;
	UiTbc::Painter* mPainter;
	UiTbc::FontManager* mFontManager;
	UiTbc::DesktopWindow* mDesktopWindow;
	UiLepra::InputManager* mInput;
	UiLepra::SoundManager* mSound;

	LOG_CLASS_DECLARE();
};



}



LEPRA_RUN_APPLICATION(Slime::App, UiLepra::UiMain);



namespace Slime
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList)
{
}

App::~App()
{
	//delete (mUiManager);
	//mUiManager = 0;
	UiLepra::Shutdown();
}

bool App::Open()
{
	const int lDisplayWidth = 480;
	const int lDisplayHeight = 320;
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
		mDisplay->SetCaption("Slime Volleyball");
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
		const double lFontHeight = lDisplayHeight / 21.0;
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
#ifndef LEPRA_IOS
		mInput = UiLepra::InputManager::CreateInputManager(mDisplay);
		//mInput->ActivateAll();
		mInput->AddKeyCodeInputObserver(this);
#endif // !iOS
	}
	if (lOk)
	{
		mDesktopWindow = new UiTbc::DesktopWindow(mInput, mPainter, new UiTbc::FloatingLayout(), 0, 0);
		mDesktopWindow->SetIsHollow(true);
		mDesktopWindow->SetPreferredSize(mCanvas->GetWidth(), mCanvas->GetHeight());
	}
	if (lOk)
	{
		mSound = UiLepra::SoundManager::CreateSoundManager(lSoundContext);
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

	if (mDisplay)
	{
		mDisplay->RemoveResizeObserver(this);
		delete (mDisplay);
		mDisplay = 0;
	}

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
	str lShadowsString = _T("Force:Volumes");
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
		lOk = mGame->init(Graphics(mDisplay, mPainter));
	}
	bool lQuit = false;
	while (lOk && !lQuit)
	{
		lOk = Poll();
		lQuit = (SystemManager::GetQuitRequest() != 0);
	}
	Close();
	return lQuit? 0 : 1;
}

bool App::Poll()
{
	UiLepra::Core::ProcessMessages();
	BeginRender(Vector3DF(0,0,0));
	Render(PixelRect(0,0,100,100));
	Graphics g(mDisplay, mPainter);
	Paint();
	mGame->paint(g);
	mGame->run();
	EndRender();
	return true;
}

void App::BeginRender(const Vector3DF& pBackgroundColor)
{
	if (CanRender())
	{
		mRenderer->ResetClippingRect();
		Clear(pBackgroundColor.x, pBackgroundColor.y, pBackgroundColor.z);

		float lMasterVolume = 1;
		mSound->SetMasterVolume(lMasterVolume);
	}
	else
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
		mCanvas->SetBuffer(0);
		mPainter->SetDestCanvas(mCanvas);
		float r, g, b;
		mRenderer->GetAmbientLight(r, g, b);
		mRenderer->SetAmbientLight(0.1f, 0.1f, 0.1f);
		PreparePaint();
		mDesktopWindow->Repaint(mPainter);
		mRenderer->SetAmbientLight(r, g, b);
	}
}

void App::PreparePaint()
{
	mPainter->ResetClippingRect();
	mPainter->PrePaint();
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


bool App::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	Event lEvent;
	switch (pKeyCode)
	{
		case UiLepra::InputManager::IN_KBD_SPACE:
			lEvent.id = 501;
			mGame->handleEvent(lEvent);
			break;
		default:
			lEvent.id = 401;
			lEvent.key = pKeyCode;
			mGame->handleEvent(lEvent);
			break;
	}
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



LOG_CLASS_DEFINE(GAME, App);



}
