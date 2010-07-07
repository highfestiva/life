
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Thread.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../UiLepra/Include/UiCore.h"
#include "../../UiLepra/Include/UiInput.h"
#include "../../UiLepra/Include/UiSoundManager.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../../UiTBC/Include/UiFontManager.h"
#include "../../UiTBC/Include/UiOpenGLPainter.h"
#include "../../UiTBC/Include/UiOpenGLRenderer.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



GameUiManager::GameUiManager(Cure::RuntimeVariableScope* pVariableScope):
	mVariableScope(pVariableScope),
	mDisplay(0),
	mCanvas(0),
	mRenderer(0),
	mPainter(0),
	mFontManager(0),
	mDesktopWindow(0),
	mInput(0),
	mSound(0)
{
}

GameUiManager::~GameUiManager()
{
	Close();
	mVariableScope = 0;
}



bool GameUiManager::Open()
{
	str lRenderTypeString = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_RENDERENGINE, _T("OpenGL"));
	int lDisplayWidth = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_DISPLAY_WIDTH, 640);
	int lDisplayHeight = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_DISPLAY_HEIGHT, 480);
	int lDisplayBpp = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_DISPLAY_BITSPERPIXEL, 0);
	int lDisplayFrequency = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_DISPLAY_FREQUENCY, 0);
	bool lDisplayFullScreen = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_DISPLAY_FULLSCREEN, false);

	UiLepra::DisplayManager::ContextType lRenderingContext = UiLepra::DisplayManager::OPENGL_CONTEXT;
	if (lRenderTypeString == _T("OpenGL"))
	{
		lRenderingContext = UiLepra::DisplayManager::OPENGL_CONTEXT;
	}
	else if (lRenderTypeString == _T("DirectX"))
	{
		lRenderingContext = UiLepra::DisplayManager::DIRECTX_CONTEXT;
	}

	str lSoundTypeString = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));
	UiLepra::SoundManager::ContextType lSoundContext = UiLepra::SoundManager::CONTEXT_OPENAL;
	if (lSoundTypeString == _T("OpenAL"))
	{
		lSoundContext = UiLepra::SoundManager::CONTEXT_OPENAL;
	}
	else if (lSoundTypeString == _T("FMOD"))
	{
		lSoundContext = UiLepra::SoundManager::CONTEXT_FMOD;
	}

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
			mLog.Warning(lError);
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

		const str lFont = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_2D_FONT, _T("Times New Roman"));
		const double lFontHeight = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_2D_FONTHEIGHT, 14.0);
		UiTbc::FontManager::FontId lFontId = mFontManager->QueryAddFont(lFont, lFontHeight);
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
		mInput = UiLepra::InputManager::CreateInputManager(mDisplay);
		mInput->ActivateAll();
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

void GameUiManager::Close()
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



void GameUiManager::InputTick()
{
	UiLepra::Core::ProcessMessages();
	if (mDisplay->IsVisible())
	{
		mInput->PollEvents();
	}
}

void GameUiManager::BeginRender()
{
	if (mDisplay->IsVisible())
	{
		mRenderer->ResetClippingRect();
		if (CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_ENABLECLEAR, true))
		{
			float r = (float)CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_CLEARRED, 0.75);
			float g = (float)CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_CLEARGREEN, 0.80);
			float b = (float)CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_CLEARBLUE, 0.85);
			Clear(r, g, b);
		}
		else
		{
			ClearDepth();
		}
	}
}

void GameUiManager::Render(const PixelRect& pArea)
{
	if (mDisplay->IsVisible())
	{
		if (pArea.GetWidth() > 0 && pArea.GetHeight() > 0)
		{
			mRenderer->ResetClippingRect();
			mRenderer->SetClippingRect(pArea);
			mRenderer->SetViewport(pArea);
			mRenderer->RenderScene();
		}
	}
}

void GameUiManager::Paint()
{
	if (mDisplay->IsVisible())
	{
		mCanvas->SetBuffer(0);
		mPainter->SetDestCanvas(mCanvas);
		PreparePaint();
		mDesktopWindow->Repaint(mPainter);
	}
}

void GameUiManager::PreparePaint()
{
	mPainter->ResetClippingRect();
	mPainter->PrePaint();
}

void GameUiManager::EndRender()
{
	if (mDisplay->IsVisible())
	{
		UpdateSettings();
		mDisplay->UpdateScreen();
	}
}



UiLepra::DisplayManager* GameUiManager::GetDisplayManager() const
{
	return (mDisplay);
}

UiTbc::Renderer* GameUiManager::GetRenderer() const
{
	return (mRenderer);
}

UiTbc::Painter* GameUiManager::GetPainter() const
{
	return (mPainter);
}

UiTbc::FontManager* GameUiManager::GetFontManager() const
{
	return (mFontManager);
}

UiLepra::InputManager* GameUiManager::GetInputManager() const
{
	return (mInput);
}

UiTbc::DesktopWindow* GameUiManager::GetDesktopWindow() const
{
	return (mDesktopWindow);
}

UiLepra::SoundManager* GameUiManager::GetSoundManager() const
{
	return (mSound);
}



void GameUiManager::SetCameraPosition(const TransformationF& pTransform)
{
	mRenderer->SetCameraTransformation(pTransform);
}

void GameUiManager::SetMicrophonePosition(const TransformationF& pTransform, const Vector3DF& pVelocity)
{
	Vector3DF lUp = pTransform.GetOrientation() * Vector3DF(0,0,1);
	Vector3DF lForward = pTransform.GetOrientation() * Vector3DF(0,1,0);
	mSound->SetListenerPosition(pTransform.GetPosition(), pVelocity, lUp, lForward);
}

void GameUiManager::SetViewport(int pLeft, int pTop, int lDisplayWidth, int lDisplayHeight)
{
	if(lDisplayWidth < 0 || lDisplayHeight < 0)
	{
		return;
	}
	mRenderer->SetViewport(PixelRect(pLeft, pTop, pLeft+lDisplayWidth, pTop+lDisplayHeight));
	mRenderer->SetClippingRect(PixelRect(pLeft, pTop, pLeft+lDisplayWidth, pTop+lDisplayHeight));
}

void GameUiManager::Clear(float pRed, float pGreen, float pBlue, bool pClearDepth)
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

void GameUiManager::ClearDepth()
{
	mRenderer->Clear(UiTbc::Renderer::CLEAR_DEPTHBUFFER);
}

void GameUiManager::PrintText(int pX, int pY, const str& pText)
{
	//mPainter->ResetClippingRect();
	mPainter->SetColor(Color(255, 255, 255, 255), 0);
	mPainter->SetColor(Color(0, 0, 0, 0), 1);
	mPainter->PrintText(pText, pX, pY);
}



PixelCoord GameUiManager::GetMouseDisplayPosition() const
{
	return (PixelCoord((int)((GetInputManager()->GetCursorX() + 1) * 0.5 * GetDisplayManager()->GetWidth()),
		(int)((GetInputManager()->GetCursorY() + 1) * 0.5 * GetDisplayManager()->GetHeight())));
}



void GameUiManager::AssertDesktopLayout(UiTbc::Layout* pLayout)
{
	if (GetDesktopWindow()->GetLayout(0)->GetType() != pLayout->GetType())
	{
		GetDesktopWindow()->ReplaceLayer(0, pLayout);
	}
	else
	{
		delete (pLayout);
	}
}



void GameUiManager::OnResize(int pWidth, int pHeight)
{
	if (mCanvas)
	{
		mCanvas->Reset(pWidth, pHeight, mCanvas->GetBitDepth());
		mDesktopWindow->SetPreferredSize(mCanvas->GetWidth(), mCanvas->GetHeight());
		mDesktopWindow->SetSize(mCanvas->GetWidth(), mCanvas->GetHeight());
		mInput->Refresh();
	}
}

void GameUiManager::OnMinimize()
{
}

void GameUiManager::OnMaximize(int pWidth, int pHeight)
{
	OnResize(pWidth, pHeight);
}



void GameUiManager::UpdateSettings()
{
	// Display.
	const bool lEnableVSync = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_DISPLAY_ENABLEVSYNC, true);
	mDisplay->SetVSyncEnabled(lEnableVSync);

	// ----------------------------------------

	// 3D rendering settings.
	const bool lEnableLights = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_ENABLELIGHTS, true);
	const double lAmbientRed = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_AMBIENTRED, 0.1);
	const double lAmbientGreen = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_AMBIENTGREEN, 0.1);
	const double lAmbientBlue = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_AMBIENTBLUE, 0.1);
	const bool lEnableTrilinearFiltering = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	const bool lEnableBilinearFiltering = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	const bool lEnableMipMapping = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	const double lFOV = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_FOV, 45.0);
	const double lClipNear = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_CLIPNEAR, 0.1);
	const double lClipFar = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_CLIPFAR, 1000.0);
	const str lShadowsString = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_3D_SHADOWS, _T("VolumesOnly"));

	mRenderer->SetLightsEnabled(lEnableLights);
	mRenderer->SetAmbientLight((float)lAmbientRed, (float)lAmbientGreen, (float)lAmbientBlue);
	mRenderer->SetTrilinearFilteringEnabled(lEnableTrilinearFiltering);
	mRenderer->SetBilinearFilteringEnabled(lEnableBilinearFiltering);
	mRenderer->SetMipMappingEnabled(lEnableMipMapping);
	mRenderer->SetViewFrustum((float)lFOV, (float)lClipNear, (float)lClipFar);

	bool lUseShadows = false;
	UiTbc::Renderer::ShadowHint lShadowType = UiTbc::Renderer::SH_VOLUMES_ONLY;
	if (lShadowsString == _T("VolumesOnly"))
	{
		lUseShadows = true;
		lShadowType = UiTbc::Renderer::SH_VOLUMES_ONLY;
	}
	else if (lShadowsString == _T("VolumesAndMaps"))
	{
		lUseShadows = true;
		lShadowType = UiTbc::Renderer::SH_VOLUMES_AND_MAPS;
	}
	mRenderer->SetShadowsEnabled(lUseShadows, lShadowType);

	// ----------------------------------------
	// 2D rendering settings.
	const str lPaintModeString = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_2D_PAINTMODE, _T("AlphaBlend"));
	UiTbc::Painter::RenderMode lPainterRenderMode = UiTbc::Painter::RM_ALPHABLEND;
	if (lPaintModeString == _T("Add"))
	{
		lPainterRenderMode = UiTbc::Painter::RM_ADD;
	}
	else if (lPaintModeString == _T("AlphaBlend"))
	{
		lPainterRenderMode = UiTbc::Painter::RM_ALPHABLEND;
	}
	else if (lPaintModeString == _T("AlphaTest"))
	{
		lPainterRenderMode = UiTbc::Painter::RM_ALPHATEST;
	}
	else if (lPaintModeString == _T("Normal"))
	{
		lPainterRenderMode = UiTbc::Painter::RM_NORMAL;
	}
	else if (lPaintModeString == _T("Xor"))
	{
		lPainterRenderMode = UiTbc::Painter::RM_XOR;
	}
	mPainter->SetRenderMode(lPainterRenderMode);

	const bool lSmoothFonts = true;//CURE_RTVAR_GET(mVariableScope, RTVAR_UI_2D_SMOOTHFONTS, true);
	mPainter->SetFontSmoothness(lSmoothFonts);

	if (mSound)
	{
		const double lSoundRollOff = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.3);
		const double lSoundDoppler = CURE_RTVAR_GET(mVariableScope, RTVAR_UI_SOUND_DOPPLER, 1.3);
		mSound->SetRollOffFactor((float)lSoundRollOff);
		mSound->SetDopplerFactor((float)lSoundDoppler);
	}
}



LOG_CLASS_DEFINE(GAME, GameUiManager);



}
