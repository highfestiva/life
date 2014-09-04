
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiRectComponent.h"
#include <list>
#include "../../../UiLepra/Include/UiInput.h"
#include "UiCleaner.h"
#include "UiGUIImageManager.h"



namespace UiTbc
{



class Cleaner;



class DesktopWindow: public RectComponent
{
	typedef RectComponent Parent;
public:
	enum MouseButtonFlags
	{
		CONSUMED_MOUSE_BUTTON1 = 1<<0,
		CONSUMED_MOUSE_BUTTON2 = 1<<1,
		CONSUMED_MOUSE_BUTTON3 = 1<<2,
		CONSUMED_MOUSE_BUTTON  = (CONSUMED_MOUSE_BUTTON1 | CONSUMED_MOUSE_BUTTON2 | CONSUMED_MOUSE_BUTTON3),
	};

	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		Layout* pLayout = 0,
		const tchar* pImageDefinitionFile = 0,
		const tchar* pArchive = 0);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		const Color& pColor,
		Layout* pLayout = 0,
		const tchar* pImageDefinitionFile = 0,
		const tchar* pArchive = 0);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		const Color& pTopLeftColor,
		const Color& pTopRightColor,
		const Color& pBottomRightColor,
		const Color& pBottomLeftColor,
		Layout* pLayout = 0,
		const tchar* pImageDefinitionFile = 0,
		const tchar* pArchive = 0);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		Painter::ImageID pImageID,
		Layout* pLayout = 0,
		const tchar* pImageDefinitionFile = 0,
		const tchar* pArchive = 0);

	virtual ~DesktopWindow();

	UiLepra::InputManager* GetInputManager() const;

	void SetMouseEnabled(bool pEnabled);
	void SetKeyboardEnabled(bool pEnabled);
	
	void SetUpdateLayout(bool pUpdateLayout);

	// Note: If the component is a child component of one of the DesktopWindow's
	// children, you have to make sure that it removes itself from the subscriber
	// list before it is deleted. Otherwise you have added a crash bug to the code.
	void AddIdleSubscriber(Component* pComponent);
	void RemoveIdleSubscriber(Component* pComponent);

	void ActivateKeyboard();
	void DeactivateKeyboard();

	// Wait for destruction of the DesktopWindow, in which case all cleaners will 
	// be deleted. This is a special solution to cope with the problem of cleaning up
	// among static variables stored in various GUI component classes.
	void AddCleaner(Cleaner* pCleaner);

	virtual void AddChild(Component* pChild, int pParam1 = 0, int pParam2 = 0, int pLayer = 0);

	// The painter sent to Repaint() will be ignored, since the painter is already
	// given through the constructor.
	virtual void Repaint(Painter* pPainter = 0);
	virtual void RepaintChild(Component* pChild, Painter* pPainter);

	virtual bool OnChar(tchar pChar);
	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnDoubleClick();
	MouseButtonFlags GetMouseButtonFlags() const;

	virtual void DoSetSize(int pWidth, int pHeight);

	virtual Type GetType() const;

	// This function will put the component in the delete queue
	// which is processed once for every call to Repaint().
	void PostDeleteComponent(Component* pComponent, int pLayer);

	// Returns the painter given in the constructor.
	Painter* GetPainter();

protected:
private:
	typedef std::list<Component*> ComponentList;

	void OnButton1(UiLepra::InputElement* pElement);
	void OnButton2(UiLepra::InputElement* pElement);
	void OnButton3(UiLepra::InputElement* pElement);
	void GetCursorPosition(int& pMouseX, int& pMouseY) const;
	void DispatchMouseMove(int pMouseX, int pMouseY);

	void Init(const tchar* pImageDefinitionFile, const tchar* pArchive);
	void ClampMouse(int& x, int& y) const;

	void PurgeDeleted();

	UiLepra::InputManager* mInputManager;

	bool mMouseEnabled;
	bool mKeyboardEnabled;
	bool mRenderRect;
	bool mUpdateLayout;

	ComponentList mIdleSubscribers;
	std::list<Cleaner*> mCleanerList;
	ComponentList mDeleteQueue;

	PixelRect mMouseArea;

	int mMouseX;
	int mMouseY;
	int mMousePrevX;
	int mMousePrevY;
	int mMouseButtonFlags;

	Painter* mPainter;

	GUIImageManager mImageManager;

	logclass();
};



}
