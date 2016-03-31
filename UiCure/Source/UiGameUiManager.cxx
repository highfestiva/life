
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/LepraTarget.h"
#include "../../Lepra/Include/Thread.h"
#include "../../Tbc/Include/PhysicsManager.h"
#include "../../UiLepra/Include/UiCore.h"
#include "../../UiLepra/Include/UiTouchDrag.h"
#include "../../UiLepra/Include/UiInput.h"
#include "../../UiLepra/Include/UiSoundManager.h"
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../UiTbc/Include/GUI/UiFloatingLayout.h"
#include "../../UiTbc/Include/UiOpenGLPainter.h"
#include "../../UiTbc/Include/UiOpenGLRenderer.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



GameUiManager::GameUiManager(Cure::RuntimeVariableScope* pVariableScope, UiLepra::Touch::DragManager* pDragManager):
	mVariableScope(pVariableScope),
	mDisplay(0),
	mCanvas(0),
	mRenderer(0),
	mPainter(0),
	mFontManager(0),
	mDesktopWindow(0),
	mInput(0),
	mDragManager(pDragManager),
	mSound(0),
	mSoundRollOffShadow(0),
	mSoundDopplerShadow(0),
	mCurrentFontId(UiTbc::FontManager::INVALID_FONTID)
{
}

GameUiManager::~GameUiManager()
{
	Close();
	mVariableScope = 0;
	mDragManager = 0;
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
	v_get(lRenderTypeString, =, mVariableScope, RTVAR_UI_DISPLAY_RENDERENGINE, _T("OpenGL"));
	v_get(lDisplayWidth, =, mVariableScope, RTVAR_UI_DISPLAY_WIDTH, 640);
	v_get(lDisplayHeight, =, mVariableScope, RTVAR_UI_DISPLAY_HEIGHT, 480);
	v_get(lDisplayBpp, =, mVariableScope, RTVAR_UI_DISPLAY_BITSPERPIXEL, 0);
	v_get(lDisplayFrequency, =, mVariableScope, RTVAR_UI_DISPLAY_FREQUENCY, 0);
	v_get(lDisplayFullScreen, =, mVariableScope, RTVAR_UI_DISPLAY_FULLSCREEN, false);

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
	lDisplayMode.mWidth = lDisplayWidth;
	lDisplayMode.mHeight = lDisplayHeight;
	lDisplayMode.mRefreshRate = 0;
	lDisplayMode.mBitDepth = 0;
#ifndef LEPRA_TOUCH
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
		if (lDisplayFullScreen)
		{
			mLog.Errorf(_T("Unsupported resolution %ux%u."), lDisplayWidth, lDisplayHeight);
		}
		else
		{
			mLog.Infof(_T("Note that resolution %ux%u only supported in windowed mode."), lDisplayWidth, lDisplayHeight);
			lOk = true;	// Go ahead - running in a window is OK.
			lDisplayMode.mWidth = lDisplayWidth;
			lDisplayMode.mHeight = lDisplayHeight;
			lDisplayMode.mBitDepth = lDisplayBpp;
			lDisplayMode.mRefreshRate = lDisplayFrequency;
		}
	}
#endif // Touch device.
	if (lOk)
	{
		if (lDisplayFullScreen)
		{
			lOk = mDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::FULLSCREEN, UiLepra::DisplayManager::ORIENTATION_ALLOW_UPSIDE_DOWN);
		}
		else
		{
			lOk = mDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::WINDOWED, UiLepra::DisplayManager::ORIENTATION_ALLOW_UPSIDE_DOWN);
		}
	}
	if (lOk)
	{
		mDisplay->AddResizeObserver(this);
		const double lPhysicalScreenSize = mDisplay->GetPhysicalScreenSize();
		v_override(mVariableScope, RTVAR_UI_DISPLAY_PHYSICALSIZE, lPhysicalScreenSize);

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
			mRenderer = new Tbc::Direct3DRenderer(mCanvas);
			mPainter = new Tbc::DirectXPainter;
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
	v_get(lSoundTypeString, =, mVariableScope, RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));
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
		v_get(lFont, =, mVariableScope, RTVAR_UI_2D_FONT, _T("Times New Roman"));
		double lFontHeight;
		v_get(lFontHeight, =, mVariableScope, RTVAR_UI_2D_FONTHEIGHT, 14.0);
		int lFontFlags;
		v_get(lFontFlags, =, mVariableScope, RTVAR_UI_2D_FONTFLAGS, 0);
		UiTbc::FontManager::FontId lFontId = mFontManager->QueryAddFont(lFont, lFontHeight, lFontFlags);
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
			lFontId = mFontManager->QueryAddFont(lFontNames[x], lFontHeight, lFontFlags);
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
		// Cut sound some slack. Sometimes OpenAL crashes during startup.
		for (int x = 0; x < 3; ++x)
		{
			UiLepra::Core::ProcessMessages();
			Thread::Sleep(0.001f);
		}
		UpdateSettings();
	}

	UiLepra::Core::ProcessMessages();
	return lOk;
}

void GameUiManager::Close()
{
	if (mDisplay)
	{
		mDisplay->RemoveResizeObserver(this);
	}

	// Poll system to let go of old windows.
	UiLepra::Core::ProcessMessages();
	Thread::Sleep(0.001);
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

	delete (mDisplay);
	mDisplay = 0;

	// Poll system to let go of old windows.
	UiLepra::Core::ProcessMessages();
	Thread::Sleep(0.001);
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
	if (CanRender())
	{
		mInput->PreProcessEvents();
	}
	UiLepra::Core::ProcessMessages();
	if (CanRender())
	{
		mInput->PollEvents();
	}

	if (mDragManager)
	{
		const float lDragLengthInInches = 0.5f;
		const int lDragPixels = (int)(GetCanvas()->GetWidth() * lDragLengthInInches / GetDisplayManager()->GetPhysicalScreenSize());
		mDragManager->SetMaxDragDistance(lDragPixels);
#if defined(LEPRA_TOUCH)
		mDragManager->UpdateMouseByDrag(GetInputManager());
#else // Not a touch device.
		bool lEmulateTouch;
		v_get(lEmulateTouch, =, mVariableScope, RTVAR_CTRL_EMULATETOUCH, false);
		if (lEmulateTouch)
		{
			mDragManager->UpdateDragByMouse(GetInputManager());
		}
#endif // Touch device / Not a touch device.
		mDragManager->UpdateTouchsticks(GetInputManager());
		// Check if the actual tap/click has been consumed by any button or such.
		if (mDesktopWindow && mDesktopWindow->GetMouseButtonFlags()&UiTbc::DesktopWindow::CONSUMED_MOUSE_BUTTON)
		{
			mDragManager->SetDraggingUi(true);
		}
	}
}

void GameUiManager::EndInputTick()
{
	if (mDragManager)
	{
		mDragManager->DropReleasedDrags();
	}
}

void GameUiManager::BeginRender(const vec3& pBackgroundColor)
{
	if (CanRender())
	{
		mRenderer->ResetClippingRect();
		bool lEnableClear;
		v_get(lEnableClear, =, mVariableScope, RTVAR_UI_3D_ENABLECLEAR, true);
		if (lEnableClear)
		{
			Clear(pBackgroundColor.x, pBackgroundColor.y, pBackgroundColor.z);
		}
		else
		{
			ClearDepth();
		}

		float lMasterVolume;
		v_get(lMasterVolume, =(float), mVariableScope, RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
		mSound->SetMasterVolume(lMasterVolume);
		float lMusicVolume;
		v_get(lMusicVolume, =(float), mVariableScope, RTVAR_UI_SOUND_MUSICVOLUME, 1.0);
		mSound->SetMusicVolume(lMusicVolume);
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

void GameUiManager::EndRender(float pFrameTime)
{
	if (CanRender())
	{
		UpdateSettings();
		mDisplay->UpdateScreen();

		mRenderer->Tick(pFrameTime);
	}
}



Cure::RuntimeVariableScope* GameUiManager::GetVariableScope() const
{
	return mVariableScope;
}

void GameUiManager::SetVariableScope(Cure::RuntimeVariableScope* pVariableScope)
{
	mVariableScope = pVariableScope;
}

UiLepra::DisplayManager* GameUiManager::GetDisplayManager() const
{
	return mDisplay;
}

Canvas* GameUiManager::GetCanvas() const
{
	return mCanvas;
}

UiTbc::Renderer* GameUiManager::GetRenderer() const
{
	return mRenderer;
}

UiTbc::Painter* GameUiManager::GetPainter() const
{
	return mPainter;
}

UiTbc::FontManager* GameUiManager::GetFontManager() const
{
	return mFontManager;
}

UiLepra::InputManager* GameUiManager::GetInputManager() const
{
	return mInput;
}

UiLepra::Touch::DragManager* GameUiManager::GetDragManager() const
{
	return mDragManager;
}

UiTbc::DesktopWindow* GameUiManager::GetDesktopWindow() const
{
	return mDesktopWindow;
}

UiLepra::SoundManager* GameUiManager::GetSoundManager() const
{
	return mSound;
}



vec3 GameUiManager::GetAccelerometer() const
{
	float x, y, z;
	v_get(x, =(float), GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_X,  0.0);
	v_get(y, =(float), GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_Y,  0.0);
	v_get(z, =(float), GetVariableScope(), RTVAR_CTRL_ACCELEROMETER_Z, -1.0);
	return vec3(x,y,z);
}

void GameUiManager::SetCameraPosition(const xform& pTransform)
{
	mRenderer->SetCameraTransformation(pTransform);
}

void GameUiManager::SetMicrophonePosition(const xform& pTransform, const vec3& pVelocity)
{
	vec3 lUp = pTransform.GetOrientation() * vec3(0,0,1);
	vec3 lForward = pTransform.GetOrientation() * vec3(0,1,0);
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



UiTbc::FontManager::FontId GameUiManager::SetScaleFont(float pScale)
{
	SetMasterFont();
	mCurrentFontId = GetFontManager()->GetActiveFontId();
	str lFont;
	v_get(lFont, =, mVariableScope, RTVAR_UI_2D_FONT, _T("Times New Roman"));
	double lFontHeight;
	if (pScale > 0)
	{
		v_get(lFontHeight, =, mVariableScope, RTVAR_UI_2D_FONTHEIGHT, 14.0);
		lFontHeight *= pScale;
	}
	else
	{
		lFontHeight = -pScale;
	}
	int lFontFlags;
	v_get(lFontFlags, =, mVariableScope, RTVAR_UI_2D_FONTFLAGS, 0);
	return mFontManager->QueryAddFont(lFont, lFontHeight, lFontFlags);
}

void GameUiManager::SetMasterFont()
{
	if (mCurrentFontId != UiTbc::FontManager::INVALID_FONTID)
	{
		mFontManager->SetActiveFont(mCurrentFontId);
		mCurrentFontId = UiTbc::FontManager::INVALID_FONTID;
	}
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



void GameUiManager::UpdateSettings()
{
	// Display.
	bool lEnableVSync;
	v_get(lEnableVSync, =, mVariableScope, RTVAR_UI_DISPLAY_ENABLEVSYNC, true);
	mDisplay->SetVSyncEnabled(lEnableVSync);

	str lDisplayOrientation;
	v_get(lDisplayOrientation, =, mVariableScope, RTVAR_UI_DISPLAY_ORIENTATION, _T("AllowUpsideDown"));
	UiLepra::DisplayManager::Orientation lOrientation = UiLepra::DisplayManager::ORIENTATION_ALLOW_UPSIDE_DOWN;
	if (lDisplayOrientation == _T("AllowAny"))
	{
		lOrientation = UiLepra::DisplayManager::ORIENTATION_ALLOW_ANY;
	}
	else if (lDisplayOrientation == _T("AllowUpsideDown"))
	{
		lOrientation = UiLepra::DisplayManager::ORIENTATION_ALLOW_UPSIDE_DOWN;
	}
	else if (lDisplayOrientation == _T("AllowAnyInternal"))
	{
		lOrientation = UiLepra::DisplayManager::ORIENTATION_ALLOW_ANY_INTERNAL;
	}
	else if (lDisplayOrientation == _T("AllowUpsideDownInternal"))
	{
		lOrientation = UiLepra::DisplayManager::ORIENTATION_ALLOW_UPSIDE_DOWN_INTERNAL;
	}
	else if (lDisplayOrientation == _T("Fixed"))
	{
		lOrientation = UiLepra::DisplayManager::ORIENTATION_FIXED;
	}
	mDisplay->SetOrientation(lOrientation);

	// ----------------------------------------

	// 3D rendering settings.
	bool lEnableLights;
	double lAmbientRed;
	double lAmbientGreen;
	double lAmbientBlue;
	bool lEnableTrilinearFiltering;
	bool lEnableBilinearFiltering;
	bool lEnableMipMapping;
	bool lEnableTexturing;
	bool lEnablePixelShaders;
	double lFogNear;
	double lFogFar;
	double lFogDensity;
	double lFogExponent;
	double lFOV;
	double lClipNear;
	double lClipFar;
	str lShadowsString;
	double lShadowDeviation;
	v_get(lEnableLights, =, mVariableScope, RTVAR_UI_3D_ENABLELIGHTS, true);
	v_get(lAmbientRed, =, mVariableScope, RTVAR_UI_3D_AMBIENTRED, 0.1);
	v_get(lAmbientGreen, =, mVariableScope, RTVAR_UI_3D_AMBIENTGREEN, 0.1);
	v_get(lAmbientBlue, =, mVariableScope, RTVAR_UI_3D_AMBIENTBLUE, 0.1);
	v_tryget(lAmbientRed, *=, mVariableScope, RTVAR_UI_3D_AMBIENTREDFACTOR, 1.0);
	v_tryget(lAmbientGreen, *=, mVariableScope, RTVAR_UI_3D_AMBIENTGREENFACTOR, 1.0);
	v_tryget(lAmbientBlue, *=, mVariableScope, RTVAR_UI_3D_AMBIENTBLUEFACTOR, 1.0);
	/*if (!mRenderer->IsPixelShadersEnabled())
	{
		// Without pixel shader highlights the scene becomes darker. At least on my computer...
		lAmbientRed *= 1.2;
		lAmbientGreen *= 1.2;
		lAmbientBlue *= 1.2;
	}*/

	v_get(lEnableTrilinearFiltering, =, mVariableScope, RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	v_get(lEnableBilinearFiltering, =, mVariableScope, RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	v_get(lEnableMipMapping, =, mVariableScope, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	v_get(lEnableTexturing, =, mVariableScope, RTVAR_UI_3D_ENABLETEXTURING, false);
	v_get(lEnablePixelShaders, =, mVariableScope, RTVAR_UI_3D_PIXELSHADERS, true);
	v_get(lFogNear, =, mVariableScope, RTVAR_UI_3D_FOGNEAR, 0.0);
	v_get(lFogFar, =, mVariableScope, RTVAR_UI_3D_FOGFAR, 0.0);
	v_get(lFogDensity, =, mVariableScope, RTVAR_UI_3D_FOGDENSITY, 1.0);
	v_get(lFogExponent, =, mVariableScope, RTVAR_UI_3D_FOGEXPONENT, 0.0);
	v_get(lFOV, =, mVariableScope, RTVAR_UI_3D_FOV, 45.0);
	v_get(lClipNear, =, mVariableScope, RTVAR_UI_3D_CLIPNEAR, 0.1);
	v_get(lClipFar, =, mVariableScope, RTVAR_UI_3D_CLIPFAR, 3000.0);
	v_get(lShadowsString, =, mVariableScope, RTVAR_UI_3D_SHADOWS, _T("Volumes"));
	v_get(lShadowDeviation, =, mVariableScope, RTVAR_UI_3D_SHADOWDEVIATION, 0.0);

	mRenderer->SetLightsEnabled(lEnableLights);
	mRenderer->SetAmbientLight((float)lAmbientRed, (float)lAmbientGreen, (float)lAmbientBlue);
	mRenderer->SetTrilinearFilteringEnabled(lEnableTrilinearFiltering);
	mRenderer->SetBilinearFilteringEnabled(lEnableBilinearFiltering);
	mRenderer->SetMipMappingEnabled(lEnableMipMapping);
	mRenderer->SetTexturingEnabled(lEnableTexturing);
	mRenderer->EnablePixelShaders(lEnablePixelShaders);
	mRenderer->SetFog((float)lFogNear, (float)lFogFar, (float)lFogDensity, (float)lFogExponent);
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
	Tbc::GeometryBase::SetDefaultBigOrientationThreshold((float)lShadowDeviation);

	// ----------------------------------------
	// 2D rendering settings.
	str lPaintModeString;
	v_get(lPaintModeString, =, mVariableScope, RTVAR_UI_2D_PAINTMODE, _T("AlphaBlend"));
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

	if (mFontManager)
	{
		str lFont;
		v_get(lFont, =, mVariableScope, RTVAR_UI_2D_FONT, _T("Times New Roman"));
		double lFontHeight;
		v_get(lFontHeight, =, mVariableScope, RTVAR_UI_2D_FONTHEIGHT, 14.0);
		int lFontFlags;
		v_get(lFontFlags, =, mVariableScope, RTVAR_UI_2D_FONTFLAGS, 0);
		mFontManager->QueryAddFont(lFont, lFontHeight, lFontFlags);
	}

	// --------------
	// Sound settings
	if (mSound)
	{
		double lSoundRollOff;
		v_get(lSoundRollOff, =, mVariableScope, RTVAR_UI_SOUND_ROLLOFF, 0.2);
		if (lSoundRollOff != mSoundRollOffShadow)
		{
			mSoundRollOffShadow = lSoundRollOff;
			mSound->SetRollOffFactor((float)lSoundRollOff);
		}
		double lSoundDoppler;
		v_get(lSoundDoppler, =, mVariableScope, RTVAR_UI_SOUND_DOPPLER, 1.3);
		if (lSoundDoppler != mSoundDopplerShadow)
		{
			mSoundDopplerShadow = lSoundDoppler;
			mSound->SetDopplerFactor((float)lSoundDoppler);
		}
	}
}


void GameUiManager::OnResize(int pWidth, int pHeight)
{
	if (mCanvas)
	{
		mCanvas->Reset(pWidth, pHeight, mCanvas->GetBitDepth());
		if (mDesktopWindow)
		{
			mDesktopWindow->SetPreferredSize(mCanvas->GetWidth(), mCanvas->GetHeight());
			mDesktopWindow->SetSize(mCanvas->GetWidth(), mCanvas->GetHeight());
		}
	}
	if (mInput)
	{
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



loginstance(GAME, GameUiManager);



}
