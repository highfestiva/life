
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../Lepra/Include/Transformation.h"
#include "../../UiLepra/Include/UiDisplayManager.h"
#include "../../UiTbc/Include/UiFontManager.h"
#include "UiCure.h"



namespace Lepra
{
class Canvas;
}
namespace UiLepra
{
namespace Touch
{
class DragManager;
}
class InputManager;
class SoundManager;
}
namespace UiTbc
{
class DesktopWindow;
class Layout;
class Painter;
class Renderer;
}



namespace UiCure
{



class GameUiManager: public UiLepra::DisplayResizeObserver
{
public:
	GameUiManager(Cure::RuntimeVariableScope* pVariableScope, UiLepra::Touch::DragManager* pDragManager);
	virtual ~GameUiManager();

	bool Open();
	bool OpenDraw();	// Only opens the renderer+painter.
	bool OpenRest();	// Opens rest; fonts, GUI, input, sound, etc.
	void Close();
	void DeleteDesktopWindow();

	bool CanRender() const;

	void InputTick();
	void EndInputTick();
	void BeginRender(const Vector3DF& pBackgroundColor);
	void Render(const PixelRect& pArea);
	void Paint(bool pClearDepthBuffer);
	void PreparePaint(bool pClearDepthBuffer);
	void EndRender(float pFrameTime);

	Cure::RuntimeVariableScope* GetVariableScope() const;
	UiLepra::DisplayManager* GetDisplayManager() const;
	Canvas* GetCanvas() const;
	UiTbc::Renderer* GetRenderer() const;
	UiTbc::Painter* GetPainter() const;
	UiTbc::FontManager* GetFontManager() const;
	UiLepra::InputManager* GetInputManager() const;
	UiLepra::Touch::DragManager* GetDragManager() const;
	UiTbc::DesktopWindow* GetDesktopWindow() const;
	UiLepra::SoundManager* GetSoundManager() const;

	Vector3DF GetAccelerometer() const;
	void SetCameraPosition(const TransformationF& pTransform);
	void SetMicrophonePosition(const TransformationF& pTransform, const Vector3DF& pVelocity);
	void SetViewport(int pLeft, int pTop, int pWidth, int pHeight);
	void Clear(float pRed, float pGreen, float pBlue, bool pClearDepth = true);
	void ClearDepth();

	void SetScaleFont(float pScale);
	void SetMasterFont();
	void PrintText(int pX, int pY, const str& pText);

	PixelCoord GetMouseDisplayPosition() const;

	void AssertDesktopLayout(UiTbc::Layout* pLayout, int pLayer);

	void UpdateSettings();

private:
	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	Cure::RuntimeVariableScope* mVariableScope;
	UiLepra::DisplayManager* mDisplay;
	Canvas* mCanvas;
	UiTbc::Renderer* mRenderer;
	UiTbc::Painter* mPainter;
	UiTbc::FontManager* mFontManager;
	UiTbc::DesktopWindow* mDesktopWindow;
	UiLepra::InputManager* mInput;
	UiLepra::SoundManager* mSound;
	UiLepra::Touch::DragManager* mDragManager;
	double mSoundRollOffShadow;	// Optimization.
	double mSoundDopplerShadow;	// Optimization.
	UiTbc::FontManager::FontId mCurrentFontId;

	LOG_CLASS_DECLARE();
};



}
