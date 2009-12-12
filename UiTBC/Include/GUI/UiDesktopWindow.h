
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



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
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		Layout* pLayout = 0,
		const Lepra::tchar* pImageDefinitionFile = 0,
		const Lepra::tchar* pArchive = 0);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		const Lepra::Color& pColor,
		Layout* pLayout = 0,
		const Lepra::tchar* pImageDefinitionFile = 0,
		const Lepra::tchar* pArchive = 0);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		const Lepra::Color& pTopLeftColor,
		const Lepra::Color& pTopRightColor,
		const Lepra::Color& pBottomRightColor,
		const Lepra::Color& pBottomLeftColor,
		Layout* pLayout = 0,
		const Lepra::tchar* pImageDefinitionFile = 0,
		const Lepra::tchar* pArchive = 0);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		Painter::ImageID pImageID,
		Layout* pLayout = 0,
		const Lepra::tchar* pImageDefinitionFile = 0,
		const Lepra::tchar* pArchive = 0);

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

	// Wait for destruction of the DesktopWindow, in which case all cleaners will 
	// be deleted. This is a special solution to cope with the problem of cleaning up
	// among static variables stored in various GUI component classes.
	void AddCleaner(Cleaner* pCleaner);

	virtual void AddChild(Component* pChild, int pParam1 = 0, int pParam2 = 0, int pLayer = 0);

	// The painter sent to Repaint() will be ignored, since the painter is already
	// given through the constructor.
	virtual void Repaint(Painter* pPainter = 0);
	virtual void RepaintChild(Component* pChild, Painter* pPainter);

	virtual bool OnChar(Lepra::tchar pChar);
	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnDoubleClick();

	virtual void DoSetSize(int pWidth, int pHeight);

	virtual Component::Type GetType();

	// This function will put the component in the delete queue
	// which is processed once for every call to Repaint().
	void DeleteComponent(Component* pComponent, int pLayer);

	// Returns the painter given in the constructor.
	Painter* GetPainter();

protected:
private:
	typedef std::list<Component*> ComponentList;

	void OnButton1(UiLepra::InputElement* pElement);
	void OnButton2(UiLepra::InputElement* pElement);
	void OnButton3(UiLepra::InputElement* pElement);

	void Init(const Lepra::tchar* pImageDefinitionFile, const Lepra::tchar* pArchive);
	void ClampMouse(int& x, int& y);

	void PurgeDeleted();

	UiLepra::InputManager* mInputManager;

	bool mMouseEnabled;
	bool mKeyboardEnabled;
	bool mRenderRect;
	bool mUpdateLayout;

	ComponentList mIdleSubscribers;
	std::list<Cleaner*> mCleanerList;
	ComponentList mDeleteQueue;

	Lepra::PixelRect mMouseArea;

	int mMouseX;
	int mMouseY;
	int mMousePrevX;
	int mMousePrevY;

	Painter* mPainter;

	GUIImageManager mImageManager;

	LOG_CLASS_DECLARE();
};



}
