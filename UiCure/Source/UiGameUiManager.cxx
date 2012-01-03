
// Author: Jonas Byström
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
	mSound(0),
	mSoundRollOffShadow(0),
	mSoundDopplerShadow(0)
{
}

GameUiManager::~GameUiManager()
{
	Close();
	mVariableScope = 0;
}



bool GameUiManager::Open()
{
	bool lOk = OpenDraw();
	if (lOk)
	{
		lOk = OpenRest();
	}
	return lOk;
}

bool GameUiManager::OpenDraw()
{
	str lRenderTypeString;
	int lDisplayWidth;
	int lDisplayHeight;
	int lDisplayBpp;
	int lDisplayFrequency;
	bool lDisplayFullScreen;
	CURE_RTVAR_GET(lRenderTypeString, =, mVariableScope, RTVAR_UI_DISPLAY_RENDERENGINE, _T("OpenGL"));
	CURE_RTVAR_GET(lDisplayWidth, =, mVariableScope, RTVAR_UI_DISPLAY_WIDTH, 640);
	CURE_RTVAR_GET(lDisplayHeight, =, mVariableScope, RTVAR_UI_DISPLAY_HEIGHT, 480);
	CURE_RTVAR_GET(lDisplayBpp, =, mVariableScope, RTVAR_UI_DISPLAY_BITSPERPIXEL, 0);
	CURE_RTVAR_GET(lDisplayFrequency, =, mVariableScope, RTVAR_UI_DISPLAY_FREQUENCY, 0);
	CURE_RTVAR_GET(lDisplayFullScreen, =, mVariableScope, RTVAR_UI_DISPLAY_FULLSCREEN, false);

	UiLepra::DisplayManager::ContextType lRenderingContext = UiLepra::DisplayManager::OPENGL_CONTEXT;
	if (lRenderTypeString == _T("OpenGL"))
	{
		lRenderingContext = UiLepra::DisplayManager::OPENGL_CONTEXT;
	}
	else if (lRenderTypeString == _T("DirectX"))
	{
		lRenderingContext = UiLepra::DisplayManager::DIRECTX_CONTEXT;
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
		str lDisplayOrientation;
		CURE_RTVAR_GET(lDisplayOrientation, =, mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("AllowUpsideDown"));
		UiLepra::DisplayManager::Orientation lOrientation = UiLepra::DisplayManager::ORIENTATION_ALLOW_UPSIDE_DOWN;
		if (lDisplayOrientation == _T("AllowAny"))
		{
			lOrientation = UiLepra::DisplayManager::ORIENTATION_ALLOW_ANY;
		}
		else if (lDisplayOrientation == _T("AllowUpsideDown"))
		{
			lOrientation = UiLepra::DisplayManager::ORIENTATION_ALLOW_UPSIDE_DOWN;
		}
		else if (lDisplayOrientation == _T("Fixed"))
		{
			lOrientation = UiLepra::DisplayManager::ORIENTATION_FIXED;
		}
		if (lDisplayFullScreen)
		{
			lOk = mDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::FULLSCREEN, lOrientation);
		}
		else
		{
			lOk = mDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::WINDOWED, lOrientation);
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
		mPainter->SetDestCanvas(mCanvas);
	}
	UiLepra::Core::ProcessMessages();
	return lOk;
}

bool GameUiManager::OpenRest()
{
	str lSoundTypeString;
	CURE_RTVAR_GET(lSoundTypeString, =, mVariableScope, RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));
	UiLepra::SoundManager::ContextType lSoundContext = UiLepra::SoundManager::CONTEXT_OPENAL;
	if (lSoundTypeString == _T("OpenAL"))
	{
		lSoundContext = UiLepra::SoundManager::CONTEXT_OPENAL;
	}
	else if (lSoundTypeString == _T("FMOD"))
	{
		lSoundContext = UiLepra::SoundManager::CONTEXT_FMOD;
	}

	bool lOk = true;
	if (lOk)
	{
		mFontManager = UiTbc::FontManager::Create(mDisplay);
		//mFontManager->SetColor(Color(255, 255, 255, 255), 0);
		//mFontManager->SetColor(Color(0, 0, 0, 0), 1);
		mPainter->SetFontManager(mFontManager);

		str lFont;
		CURE_RTVAR_GET(lFont, =, mVariableScope, RTVAR_UI_2D_FONT, _T("Times New Roman"));
		double lFontHeight;
		CURE_RTVAR_GET(lFontHeight, =, mVariableScope, RTVAR_UI_2D_FONTHEIGHT, 14.0);
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
	if (mDisplay)
	{
		mDisplay->RemoveResizeObserver(this);
	}

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
		delete (mDisplay);
		mDisplay = 0;
	}

	// Poll system to let go of old windows.
	UiLepra::Core::ProcessMessages();
	Thread::Sleep(0.05);
	UiLepra::Core::ProcessMessages();
}

void GameUiManager::DeleteDesktopWindow()
{
	delete (mDesktopWindow);
	mDesktopWindow = 0;
}



bool GameUiManager::CanRender() const
{
	return mDisplay->IsVisible();
}



void GameUiManager::InputTick()
{
	UiLepra::Core::ProcessMessages();
	if (CanRender())
	{
		mInput->PollEvents();
	}
}

void GameUiManager::BeginRender(const Vector3DF& pBackgroundColor)
{
	if (CanRender())
	{
		mRenderer->ResetClippingRect();
		bool lEnableClear;
		CURE_RTVAR_GET(lEnableClear, =, mVariableScope, RTVAR_UI_3D_ENABLECLEAR, true);
		if (lEnableClear)
		{
			Clear(pBackgroundColor.x, pBackgroundColor.y, pBackgroundColor.z);
		}
		else
		{
			ClearDepth();
		}

		float lMasterVolume;
		CURE_RTVAR_GET(lMasterVolume, =(float), mVariableScope, RTVAR_UI_SOUND_VOLUME, 1.0);
		mSound->SetMasterVolume(lMasterVolume);
	}
	else
	{
		mSound->SetMasterVolume(0);
	}
}

void GameUiManager::Render(const PixelRect& pArea)
{
	if (CanRender())
	{
		if (pArea.GetWidth() > 0 && pArea.GetHeight() > 0)
		{
			mRenderer->SetClippingRect(pArea);
			mRenderer->SetViewport(pArea);
			mRenderer->RenderScene();
		}
	}
}

void GameUiManager::Paint(bool pClearDepthBuffer)
{
	if (CanRender())
	{
		mCanvas->SetBuffer(0);
		mPainter->SetDestCanvas(mCanvas);
		float r, g, b;
		mRenderer->GetAmbientLight(r, g, b);
		mRenderer->SetAmbientLight(0.1f, 0.1f, 0.1f);
		PreparePaint(pClearDepthBuffer);
		mDesktopWindow->Repaint(mPainter);
		mRenderer->SetAmbientLight(r, g, b);
	}
}

void GameUiManager::PreparePaint(bool pClearDepthBuffer)
{
	mPainter->ResetClippingRect();
	mPainter->PrePaint(pClearDepthBuffer);
}

void GameUiManager::EndRender()
{
	if (CanRender())
	{
		UpdateSettings();
		mDisplay->UpdateScreen();
	}
}



Cure::RuntimeVariableScope* GameUiManager::GetVariableScope() const
{
	return mVariableScope;
}

UiLepra::DisplayManager* GameUiManager::GetDisplayManager() const
{
	return (mDisplay);
}

Canvas* GameUiManager::GetCanvas() const
{
	return mCanvas;
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



void GameUiManager::AssertDesktopLayout(UiTbc::Layout* pLayout, int pLayer)
{
	if (GetDesktopWindow()->GetLayout(pLayer)->GetType() != pLayout->GetType())
	{
		GetDesktopWindow()->ReplaceLayer(pLayer, pLayout);
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
	bool lEnableVSync;
	CURE_RTVAR_GET(lEnableVSync, =, mVariableScope, RTVAR_UI_DISPLAY_ENABLEVSYNC, true);
	mDisplay->SetVSyncEnabled(lEnableVSync);

	// ----------------------------------------

	// 3D rendering settings.
	bool lEnableLights;
	double lAmbientRed;
	double lAmbientGreen;
	double lAmbientBlue;
	bool lEnableTrilinearFiltering;
	bool lEnableBilinearFiltering;
	bool lEnableMipMapping;
	bool lEnablePixelShaders;
	double lFOV;
	double lClipNear;
	double lClipFar;
	str lShadowsString;
	CURE_RTVAR_GET(lEnableLights, =, mVariableScope, RTVAR_UI_3D_ENABLELIGHTS, true);
	CURE_RTVAR_GET(lAmbientRed, =, mVariableScope, RTVAR_UI_3D_AMBIENTRED, 0.1);
	CURE_RTVAR_GET(lAmbientGreen, =, mVariableScope, RTVAR_UI_3D_AMBIENTGREEN, 0.1);
	CURE_RTVAR_GET(lAmbientBlue, =, mVariableScope, RTVAR_UI_3D_AMBIENTBLUE, 0.1);
	CURE_RTVAR_TRYGET(lAmbientRed, *=, mVariableScope, RTVAR_UI_3D_AMBIENTREDFACTOR, 1.0);
	CURE_RTVAR_TRYGET(lAmbientGreen, *=, mVariableScope, RTVAR_UI_3D_AMBIENTGREENFACTOR, 1.0);
	CURE_RTVAR_TRYGET(lAmbientBlue, *=, mVariableScope, RTVAR_UI_3D_AMBIENTBLUEFACTOR, 1.0);
	/*if (!mRenderer->IsPixelShadersEnabled())
	{
		// Without pixel shader highlights the scene becomes darker. At least on my computer...
		lAmbientRed *= 1.2;
		lAmbientGreen *= 1.2;
		lAmbientBlue *= 1.2;
	}*/

	CURE_RTVAR_GET(lEnableTrilinearFiltering, =, mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	CURE_RTVAR_GET(lEnableBilinearFiltering, =, mVariableScope, RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	CURE_RTVAR_GET(lEnableMipMapping, =, mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	CURE_RTVAR_GET(lEnablePixelShaders, =, mVariableScope, RTVAR_UI_3D_PIXELSHADERS, true);
	CURE_RTVAR_GET(lFOV, =, mVariableScope, RTVAR_UI_3D_FOV, 45.0);
	CURE_RTVAR_GET(lClipNear, =, mVariableScope, RTVAR_UI_3D_CLIPNEAR, 0.1);
	CURE_RTVAR_GET(lClipFar, =, mVariableScope, RTVAR_UI_3D_CLIPFAR, 3000.0);
	CURE_RTVAR_GET(lShadowsString, =, mVariableScope, RTVAR_UI_3D_SHADOWS, _T("Volumes"));

	mRenderer->SetLightsEnabled(lEnableLights);
	mRenderer->SetAmbientLight((float)lAmbientRed, (float)lAmbientGreen, (float)lAmbientBlue);
	mRenderer->SetTrilinearFilteringEnabled(lEnableTrilinearFiltering);
	mRenderer->SetBilinearFilteringEnabled(lEnableBilinearFiltering);
	mRenderer->SetMipMappingEnabled(lEnableMipMapping);
	mRenderer->EnablePixelShaders(lEnablePixelShaders);
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
	str lPaintModeString;
	CURE_RTVAR_GET(lPaintModeString, =, mVariableScope, RTVAR_UI_2D_PAINTMODE, _T("AlphaBlend"));
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

	const bool lSmoothFonts = true;
	mPainter->SetFontSmoothness(lSmoothFonts);

	if (mSound)
	{
		double lSoundRollOff;
		CURE_RTVAR_GET(lSoundRollOff, =, mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.2);
		if (lSoundRollOff != mSoundRollOffShadow)
		{
			mSoundRollOffShadow = lSoundRollOff;
			mSound->SetRollOffFactor((float)lSoundRollOff);
		}
		double lSoundDoppler;
		CURE_RTVAR_GET(lSoundDoppler, =, mVariableScope, RTVAR_UI_SOUND_DOPPLER, 1.3);
		if (lSoundDoppler != mSoundDopplerShadow)
		{
			mSoundDopplerShadow = lSoundDoppler;
			mSound->SetDopplerFactor((float)lSoundDoppler);
		}
	}
}



LOG_CLASS_DEFINE(GAME, GameUiManager);



}
