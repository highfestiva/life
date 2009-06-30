
// Author: Alexander Hugestrand
// Copyright (c) 2002-2006, Righteous Games



#pragma once

#include "UiRectComponent.h"
#include "UiGUIImageManager.h"
#include "UiCleaner.h"
#include "../UiSoftwarePainter.h"
#include "../../../UiLepra/Include/UiInput.h"
#include <list>



namespace UiTbc
{



class MouseTheme;
class Cleaner;



class DesktopWindow: public RectComponent
{
	typedef RectComponent Parent;
public:

	// The render mode defines what algorithm the GUI will use when rendered.
	// RM_EVERY_FRAME is the default mode, and repaints EVERYTHING EVERY FRAME.
	// This mode is recommended if you are using resizable windows, or if the
	// rendering is performed by a software painter.
	// RM_OPTIMIZE_STATIC will prerender all top level components as static images. 
	// The images are updated when the component's appearance change. This mode 
	// requires static components (not resizable), since the image's size is determined 
	// the first time the component is rendered. Top level components are those
	// who's first parent is the DesktopWindow. A hardware renderer is recommended,
	// since the rendering is perfomed using alpha blending.
	enum RenderMode
	{
		RM_EVERY_FRAME = 0,
		RM_OPTIMIZE_STATIC,
	};

	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		Layout* pLayout = 0,
		const Lepra::tchar* pImageDefinitionFile = 0,
		const Lepra::tchar* pArchive = 0,
		RenderMode pRenderMode = RM_EVERY_FRAME);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		const Lepra::Color& pColor,
		Layout* pLayout = 0,
		const Lepra::tchar* pImageDefinitionFile = 0,
		const Lepra::tchar* pArchive = 0,
		RenderMode pRenderMode = RM_EVERY_FRAME);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		const Lepra::Color& pTopLeftColor,
		const Lepra::Color& pTopRightColor,
		const Lepra::Color& pBottomRightColor,
		const Lepra::Color& pBottomLeftColor,
		Layout* pLayout = 0,
		const Lepra::tchar* pImageDefinitionFile = 0,
		const Lepra::tchar* pArchive = 0,
		RenderMode pRenderMode = RM_EVERY_FRAME);
	DesktopWindow(UiLepra::InputManager* pInputManager,
		Painter* pPainter,
		Painter::ImageID pImageID,
		Layout* pLayout = 0,
		const Lepra::tchar* pImageDefinitionFile = 0,
		const Lepra::tchar* pArchive = 0,
		RenderMode pRenderMode = RM_EVERY_FRAME);

	virtual ~DesktopWindow();

	UiLepra::InputManager* GetInputManager() const;

	inline MouseTheme* GetMouseTheme() const;
	void SetMouseTheme(MouseTheme* pMouseTheme);

	void SetMouseEnabled(bool pEnabled);
	inline void SetKeyboardEnabled(bool pEnabled);
	
	inline void SetUpdateLayout(bool pUpdateLayout);

	// Note: If the component is a child component of one of the DesktopWindow's
	// children, you have to make sure that it removes itself from the subscriber
	// list before it is deleted. Otherwise you have added a crash bug to the code.
	inline void AddIdleSubscriber(Component* pComponent);
	inline void RemoveIdleSubscriber(Component* pComponent);

	// Wait for destruction of the DesktopWindow, in which case all cleaners will 
	// be deleted. This is a special solution to cope with the problem of cleaning up
	// among static variables stored in various GUI component classes.
	inline void AddCleaner(Cleaner* pCleaner);

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

	inline virtual Component::Type GetType();

	// This function will put the component in the delete queue
	// which is processed once for every call to Repaint().
	void DeleteComponent(Component* pComponent, int pLayer);

	// Returns the painter given in the constructor.
	Painter* GetPainter();

	// Returns the painter used internally drawing the windows.
	// If RenderMode is set to RM_EVERY_FRAME this will be the same
	// painter as the one returned by GetPainter().
	Painter* GetInternalPainter();

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

	MouseTheme* mMouseTheme;
	ComponentList mIdleSubscribers;
	std::list<Cleaner*> mCleanerList;
	ComponentList mDeleteQueue;

	Lepra::PixelRect mMouseArea;

	int mMouseX;
	int mMouseY;
	int mMousePrevX;
	int mMousePrevY;

	RenderMode mRenderMode;
	SoftwarePainter mPainter;
	Painter* mUserPainter;

	GUIImageManager mImageManager;

	LOG_CLASS_DECLARE();
};

MouseTheme* DesktopWindow::GetMouseTheme() const
{
	return mMouseTheme;
}

void DesktopWindow::SetKeyboardEnabled(bool pEnabled)
{
	mKeyboardEnabled = pEnabled;
}

void DesktopWindow::SetUpdateLayout(bool pUpdateLayout)
{
	mUpdateLayout = pUpdateLayout;
}

void DesktopWindow::AddIdleSubscriber(Component* pComponent)
{
	mIdleSubscribers.push_back(pComponent);
	mIdleSubscribers.unique();
}

void DesktopWindow::RemoveIdleSubscriber(Component* pComponent)
{
	mIdleSubscribers.remove(pComponent);
}

void DesktopWindow::AddCleaner(Cleaner* pCleaner)
{
	mCleanerList.push_back(pCleaner);
	mCleanerList.unique();
}

Component::Type DesktopWindow::GetType()
{
	return Component::DESKTOPWINDOW;
}



}
