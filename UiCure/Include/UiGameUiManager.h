
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../UiLepra/Include/UiDisplayManager.h"
#include "UiCure.h"



namespace Lepra
{
class Canvas;
}
namespace UiLepra
{
class InputManager;
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



class GraphicsObject;
class SoundManager;



class GameUiManager: public UiLepra::DisplayResizeObserver
{
public:
	GameUiManager(Cure::RuntimeVariableScope* pVariableScope);
	virtual ~GameUiManager();

	bool Open();
	void Close();

	void InputTick();
	void BeginRender();
	void Render(const Lepra::PixelRect& pArea);
	void Paint();
	void EndRender();

	UiLepra::DisplayManager* GetDisplayManager() const;
	UiTbc::Renderer* GetRenderer() const;
	UiTbc::Painter* GetPainter() const;
	UiLepra::InputManager* GetInputManager() const;
	UiTbc::DesktopWindow* GetDesktopWindow() const;
	SoundManager* GetSoundManager() const;

	void SetCameraPosition(float pX, float pY, float pZ);
	void SetCameraOrientation(float pTheta, float pPhi, float pGimbal);
	void SetViewport(int pLeft, int pTop, int pWidth, int pHeight);
	void Clear(float pRed, float pGreen, float pBlue, bool pClearDepth = true);
	void ClearDepth();
	void PrintText(int pX, int pY, const Lepra::String& pText);

	void AssertDesktopLayout(UiTbc::Layout* pLayout);

private:
	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	void UpdateSettings();

	Cure::RuntimeVariableScope* mVariableScope;
	UiLepra::DisplayManager* mDisplay;
	Lepra::Canvas* mCanvas;
	UiTbc::Renderer* mRenderer;
	UiTbc::Painter* mPainter;
	UiTbc::FontManager* mFontManager;
	UiTbc::DesktopWindow* mDesktopWindow;
	UiLepra::InputManager* mInput;
	SoundManager* mSound;

	LOG_CLASS_DECLARE();
};



}
