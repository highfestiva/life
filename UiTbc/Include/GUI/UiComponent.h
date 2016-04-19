
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Base class of all GUI components.



#pragma once

#include "../../../Lepra/Include/Unordered.h"
#include "../../../Lepra/Include/Graphics2D.h"
#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/String.h"
#include "../../../UiLepra/Include/UiInput.h"
#include "../UiTbc.h"
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

	Component(Layout* pLayout);
	virtual ~Component();
	void DeleteAllLayers();

	Component* GetParent();
	Layout* GetLayout(int pLayer = 0) const;
	void SetParentLayout(Layout* pLayout);
	Layout* GetParentLayout() const;

	int CreateLayer(Layout* pLayout);
	void DeleteLayer(int pLayer);
	void DeleteChildrenInLayer(int pLayer);
	void ReplaceLayer(int pLayer, Layout* pLayout);

	const PixelCoord& GetPos() const;
	PixelCoord GetScreenPos() const;
	
	const PixelCoord& GetSize() const;
	virtual PixelCoord GetMinSize() const;
	PixelRect GetScreenRect() const;
	
	void SetPreferredSize(const PixelCoord& pSize, bool pAdaptive = true);
	virtual void SetPreferredSize(int pWidth, int pHeight, bool pAdaptive = true);
	void SetPreferredWidth(int pWidth);
	void SetPreferredHeight(int pHeight);
	virtual PixelCoord GetPreferredSize(bool pForceAdaptive = false);
	int GetPreferredWidth(bool pForceAdaptive = false);
	int GetPreferredHeight(bool pForceAdaptive = false);
	void SetAdaptive(bool pAdaptive);
	bool IsAdaptive();

	void SetVisible(bool pVisible);
	bool IsVisible() const;
	bool IsLocalVisible() const;

	// Sets and gets the selected flag. The selected flag isn't actually used
	// within this class, but is stored here to make it easier to implement
	// list- and tree-controls.
	virtual void SetSelected(bool pSelected);
	bool GetSelected() const;

	virtual void Enable(bool pEnable);

	// The exact interpretation of pParam1 and pParam2 depends on the layout.
	virtual void AddChild(Component* pChild, int pParam1 = 0, int pParam2 = 0, int pLayer = 0);
	virtual void RemoveChild(Component* pChild, int pLayer);
	virtual int GetNumChildren() const;
	
	void AddTextListener(UiLepra::TextInputObserver* pListener);
	void RemoveTextListener(UiLepra::TextInputObserver* pListener);
	void AddKeyListener(UiLepra::KeyCodeInputObserver* pListener);
	void RemoveKeyListener(UiLepra::KeyCodeInputObserver* pListener);

	/*
		Coordinate convertions.
	*/

	PixelCoord ClientToWindow(const PixelCoord& pCoords);
	PixelCoord WindowToClient(const PixelCoord& pCoords);
	PixelCoord WindowToScreen(const PixelCoord& pCoords);
	PixelCoord ScreenToWindow(const PixelCoord& pCoords);
	PixelCoord ClientToScreen(const PixelCoord& pCoords);
	PixelCoord ScreenToClient(const PixelCoord& pCoords);

	PixelRect ClientToWindow(const PixelRect& pRect);
	PixelRect WindowToClient(const PixelRect& pRect);
	PixelRect WindowToScreen(const PixelRect& pRect);
	PixelRect ScreenToWindow(const PixelRect& pRect);
	PixelRect ClientToScreen(const PixelRect& pRect);
	PixelRect ScreenToClient(const PixelRect& pRect);

	/*
		Virtual functions and events.
	*/

	virtual void Repaint(Painter* pPainter);
	virtual void RepaintBackground(Painter* pPainter);
	virtual void RepaintComponents(Painter* pPainter);
	virtual void RepaintChild(Component* pChild, Painter* pPainter);
	virtual bool IsOver(int pScreenX, int pScreenY);

	virtual bool OnDoubleClick(int pMouseX, int pMouseY);

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnRButtonDown(int pMouseX, int pMouseY);
	virtual bool OnMButtonDown(int pMouseX, int pMouseY);

	virtual bool OnLButtonUp(int pMouseX, int pMouseY);
	virtual bool OnRButtonUp(int pMouseX, int pMouseY);
	virtual bool OnMButtonUp(int pMouseX, int pMouseY);

	virtual bool OnMouseWheel(int pMouseX, int pMouseY, int pChange, bool pDown);
	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY);

	virtual bool OnChar(wchar_t pChar);

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
	void SetPos(const PixelCoord& pPos);
	virtual void SetPos(int x, int y);
	void SetSize(const PixelCoord& pSize);
	virtual void SetSize(int pWidth, int pHeight);
	void SetMinSize(const PixelCoord& pSize);
	virtual void SetMinSize(int pWidth, int pHeight);

	virtual Type GetType() const;

	Component* GetChild(int pScreenX, int pScreenY, int pLevelsDown = 0);

	GUIImageManager* GetImageManager();

	virtual void UpdateLayout();

	virtual void SetParent(Component* pParent);
	bool IsChildOf(Component* pParent);

	enum ComponentState
	{
		STATE_FOCUSABLE	= 1,
		STATE_CLICKABLE,
	};
	typedef std::pair<int, Component*> StateComponent;
	typedef std::list<StateComponent> StateComponentList;
	virtual StateComponentList GetStateList(ComponentState pState);

	virtual void SetMouseFocus();
	virtual void ReleaseMouseFocus(RecurseDir pDir = RECURSE_UP, Component* pFocusedComponent = 0);
	bool HasMouseFocus() const;

	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(RecurseDir pDir = RECURSE_UP, Component* pFocusedComponent = 0);
	bool HasKeyboardFocus() const;

	void RequestRepaint();

	virtual bool IsComplete() const;

	Component* GetChild(const str& pName) const;
	void SetName(const str& pName);
	const str& GetName() const;

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
	void DispatchChar(wchar_t pChar);
	bool IsDispatcher() const;

	void DeleteLayout(int pLayer);

	typedef std::unordered_set<UiLepra::TextInputObserver*, LEPRA_VOIDP_HASHER> TextListenerSet;
	typedef std::unordered_set<UiLepra::KeyCodeInputObserver*, LEPRA_VOIDP_HASHER> KeyListenerSet;
	TextListenerSet mTextListenerSet;
	KeyListenerSet mKeyListenerSet;

	Component* mParent;
	Component* mMouseFocusChild;
	Component* mKeyboardFocusChild;
	Layout** mLayout;
	Layout* mParentLayout;

	PixelCoord mPos;
	PixelCoord mPreferredSize;
	PixelCoord mSize;
	PixelCoord mMinSize;

	bool mNeedsRepaint;
	bool mVisible;
	bool mAdaptivePreferredSize;
	bool mSelected;
	bool mEnabled;

	// Used by DesktopWindow to optimize rendering.
	Painter::ImageID mImageID;
	Canvas mImage;

	int mLayerCount;

	str mName;

	static GUIImageManager* smImageManager;

	logclass();
};



}
