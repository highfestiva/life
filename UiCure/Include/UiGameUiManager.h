
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Lepra/Include/Transformation.h"
#include "../../UiLepra/Include/UiDisplayManager.h"
#include "UiCure.h"



namespace Lepra
{
class Canvas;
}
namespace UiLepra
{
class InputManager;
class SoundManager;
}
namespace UiTbc
{
class DesktopWindow;
class FontManager;
class Layout;
class Painter;
class Renderer;
}



namespace UiCure
{



class GameUiManager: public UiLepra::DisplayResizeObserver
{
public:
	GameUiManager(Cure::RuntimeVariableScope* pVariableScope);
	virtual ~GameUiManager();

	bool Open();
	void Close();

	void InputTick();
	void BeginRender();
	void Render(const PixelRect& pArea);
	void Paint();
	void PreparePaint();
	void EndRender();

	UiLepra::DisplayManager* GetDisplayManager() const;
	UiTbc::Renderer* GetRenderer() const;
	UiTbc::Painter* GetPainter() const;
	UiTbc::FontManager* GetFontManager() const;
	UiLepra::InputManager* GetInputManager() const;
	UiTbc::DesktopWindow* GetDesktopWindow() const;
	UiLepra::SoundManager* GetSoundManager() const;

	void SetCameraPosition(const TransformationF& pTransform);
	void SetMicrophonePosition(const TransformationF& pTransform, const Vector3DF& pVelocity);
	void SetViewport(int pLeft, int pTop, int pWidth, int pHeight);
	void Clear(float pRed, float pGreen, float pBlue, bool pClearDepth = true);
	void ClearDepth();
	void PrintText(int pX, int pY, const str& pText);

	PixelCoord GetMouseDisplayPosition() const;

	void AssertDesktopLayout(UiTbc::Layout* pLayout);

private:
	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	void UpdateSettings();

	Cure::RuntimeVariableScope* mVariableScope;
	UiLepra::DisplayManager* mDisplay;
	Canvas* mCanvas;
	UiTbc::Renderer* mRenderer;
	UiTbc::Painter* mPainter;
	UiTbc::FontManager* mFontManager;
	UiTbc::DesktopWindow* mDesktopWindow;
	UiLepra::InputManager* mInput;
	UiLepra::SoundManager* mSound;
	double mSoundRollOffShadow;	// Optimization.
	double mSoundDopplerShadow;	// Optimization.

	LOG_CLASS_DECLARE();
};



}
