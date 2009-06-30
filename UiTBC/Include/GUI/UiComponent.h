
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous games

// Base class of all GUI components.



#pragma once

#include <set>
#include "../../../Lepra/Include/Graphics2D.h"
#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/String.h"
#include "../../../UiLepra/Include/UiInput.h"
#include "../UiTBC.h"
#include "../UiPainter.h"
#include "UiLayout.h"
#include "UiGUIImageManager.h"



namespace UiTbc
{



class GUIImageManager;



class Component: public UiLepra::TextInputObserver, public UiLepra::KeyCodeInputObserver, public UiLepra::MouseInputObserver
{
public:

	friend class DesktopWindow;

	// Used in combination with text rendering.
	enum BlendFunc
	{
		NO_BLEND = 0,
		ALPHATEST,
		ALPHABLEND
	};

	enum Type
	{
		COMPONENT = 0,
		RECTCOMPONENT,
		BORDER,
		BUTTON,
		RADIOBUTTON,
		CAPTION,
		LABEL,
		WINDOW,
		DESKTOPWINDOW,
		SCROLLBAR,
		LISTCONTROL,
	};

	enum // Area
	{
		AREA_OUTSIDE = 0,
		AREA_INSIDE,	// Empy client area.
		AREA_CHILD,
	};

	enum RecurseDir
	{
		RECURSE_UP = 0,
		RECURSE_DOWN,
	};

	Component(const Lepra::String& pName, Layout* pLayout);
	virtual ~Component();

	const Lepra::String& GetName();

	Component* GetParent();
	Layout* GetLayout(int pLayer = 0) const;

	int CreateLayer(Layout* pLayout);
	void DeleteLayer(int pLayer);
	void ReplaceLayer(int pLayer, Layout* pLayout);

	const Lepra::PixelCoords& GetPos();
	Lepra::PixelCoords GetScreenPos();
	
	const Lepra::PixelCoords& GetSize();
	virtual Lepra::PixelCoords GetMinSize();
	Lepra::PixelRect GetScreenRect();
	
	void SetPreferredSize(const Lepra::PixelCoords& pSize, bool pAdaptive = true);
	virtual void SetPreferredSize(int pWidth, int pHeight, bool pAdaptive = true);
	void SetPreferredWidth(int pWidth);
	void SetPreferredHeight(int pHeight);
	virtual Lepra::PixelCoords GetPreferredSize(bool pForceAdaptive = false);
	int GetPreferredWidth(bool pForceAdaptive = false);
	int GetPreferredHeight(bool pForceAdaptive = false);
	void SetAdaptive(bool pAdaptive);
	bool IsAdaptive();

	void SetVisible(bool pVisible);
	bool IsVisible();

	// Sets and gets the selected flag. The selected flag isn't actually used
	// within this class, but is stored here to make it easier to implement
	// list- and tree-controls.
	virtual void SetSelected(bool pSelected);
	bool GetSelected() const;

	// The exact interpretation of pParam1 and pParam2 depends on the layout.
	virtual void AddChild(Component* pChild, int pParam1 = 0, int pParam2 = 0, int pLayer = 0);
	virtual void RemoveChild(Component* pChild, int pLayer);
	virtual Component* GetChild(const Lepra::String& pName, int pLayer);
	virtual int GetNumChildren() const;
	
	void AddTextListener(UiLepra::TextInputObserver* pListener);
	void RemoveTextListener(UiLepra::TextInputObserver* pListener);
	void AddKeyListener(UiLepra::KeyCodeInputObserver* pListener);
	void RemoveKeyListener(UiLepra::KeyCodeInputObserver* pListener);

	/*
		Coordinate convertions.
	*/

	Lepra::PixelCoords ClientToWindow(const Lepra::PixelCoords& pCoords);
	Lepra::PixelCoords WindowToClient(const Lepra::PixelCoords& pCoords);
	Lepra::PixelCoords WindowToScreen(const Lepra::PixelCoords& pCoords);
	Lepra::PixelCoords ScreenToWindow(const Lepra::PixelCoords& pCoords);
	Lepra::PixelCoords ClientToScreen(const Lepra::PixelCoords& pCoords);
	Lepra::PixelCoords ScreenToClient(const Lepra::PixelCoords& pCoords);

	Lepra::PixelRect ClientToWindow(const Lepra::PixelRect& pRect);
	Lepra::PixelRect WindowToClient(const Lepra::PixelRect& pRect);
	Lepra::PixelRect WindowToScreen(const Lepra::PixelRect& pRect);
	Lepra::PixelRect ScreenToWindow(const Lepra::PixelRect& pRect);
	Lepra::PixelRect ClientToScreen(const Lepra::PixelRect& pRect);
	Lepra::PixelRect ScreenToClient(const Lepra::PixelRect& pRect);

	/*
		Virtual functions and events.
	*/

	virtual void Repaint(Painter* pPainter);
	virtual void RepaintChild(Component* pChild, Painter* pPainter);
	virtual bool IsOver(int pScreenX, int pScreenY);
	virtual int GetArea(int pScreenX, int pScreenY);

	virtual bool OnDoubleClick(int pMouseX, int pMouseY);

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnRButtonDown(int pMouseX, int pMouseY);
	virtual bool OnMButtonDown(int pMouseX, int pMouseY);

	virtual bool OnLButtonUp(int pMouseX, int pMouseY);
	virtual bool OnRButtonUp(int pMouseX, int pMouseY);
	virtual bool OnMButtonUp(int pMouseX, int pMouseY);

	virtual bool OnMouseWheel(int pMouseX, int pMouseY, int pChange, bool pDown);
	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY);

	virtual bool OnChar(Lepra::tchar pChar);

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);

	virtual bool OnDoubleClick();

	// Called once the component has been connected to the DesktopWindow.
	// Use this to initialize things that can't be initialized otherwise.
	virtual void OnConnectedToDesktopWindow();

	// Called every frame only if the component has registered itself as a subscriber
	// of this event at the DesktopWindow.
	virtual void OnIdle(){}

	// The following are functions that should be handled with care (thus, you need to know
	// what you are doing). For instance: Don't set the size by calling SetSize()... Call 
	// SetPreferredSize() instead. SetSize() is used by the various layout classes in order
	// to set the definite size of the window, just before it's rendered.
	void SetPos(const Lepra::PixelCoords& pPos);
	virtual void SetPos(int x, int y);
	void SetSize(const Lepra::PixelCoords& pSize);
	virtual void SetSize(int pWidth, int pHeight);
	void SetMinSize(const Lepra::PixelCoords& pSize);
	virtual void SetMinSize(int pWidth, int pHeight);

	virtual Type GetType();

	Component* GetChild(int pScreenX, int pScreenY, int pLevelsDown = 0);

	GUIImageManager* GetImageManager();

	virtual void UpdateLayout();

	virtual void SetParent(Component* pParent);
	bool IsChildOf(Component* pParent);

	virtual void SetMouseFocus();
	virtual void ReleaseMouseFocus(RecurseDir pDir = RECURSE_UP, Component* pFocusedComponent = 0);
	bool HasMouseFocus() const;

	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(RecurseDir pDir = RECURSE_UP, Component* pFocusedComponent = 0);
	bool HasKeyboardFocus() const;

	void RequestRepaint();

protected:
	virtual void DoSetPos(int x, int y);
	virtual void DoSetSize(int pWidth, int pHeight);
	virtual void DoSetMinSize(int pWidth, int pHeight);

	bool NeedsRepaint();
	// Should really be protected, but ScrollBar needs access to it.
	void SetNeedsRepaint(bool pNeedsRepaint);
	
	// Some helper functions...
	Component* GetParentOfType(Type pType);
	Component* GetTopParent();

	void SetImageManager(GUIImageManager* pImageManager);

	virtual void SetKeyboardFocus(Component* pChild);
	virtual void SetMouseFocus(Component* pChild);
	void DispatchChar(Lepra::tchar pChar);
	bool IsDispatcher() const;

	void DeleteLayout(int pLayer);

	enum ComponentState
	{
		STATE_FOCUSABLE	= 1,
		STATE_CLICKABLE,
	};
	typedef std::pair<int, Component*> StateComponent;
	typedef std::list<StateComponent> StateComponentList;
	virtual StateComponentList GetStateList(ComponentState pState) const;

private:
	typedef std::set<UiLepra::TextInputObserver*> TextListenerSet;
	typedef std::set<UiLepra::KeyCodeInputObserver*> KeyListenerSet;
	TextListenerSet mTextListenerSet;
	KeyListenerSet mKeyListenerSet;

	Lepra::String mName;
	Component* mParent;
	Component* mMouseFocusChild;
	Component* mKeyboardFocusChild;
	Layout** mLayout;

	Lepra::PixelCoords mPos;
	Lepra::PixelCoords mPreferredSize;
	Lepra::PixelCoords mSize;
	Lepra::PixelCoords mMinSize;

	bool mNeedsRepaint;
	bool mVisible;
	bool mAdaptivePreferredSize;
	bool mSelected;

	// Used by DesktopWindow to optimize rendering.
	Painter::ImageID mImageID;
	Lepra::Canvas mImage;

	int mLayerCount;

	static GUIImageManager* smImageManager;

	LOG_CLASS_DECLARE();
};



}
