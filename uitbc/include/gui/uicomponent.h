
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Base class of all GUI components.



#pragma once

#include "../../../lepra/include/unordered.h"
#include "../../../lepra/include/graphics2d.h"
#include "../../../lepra/include/hashtable.h"
#include "../../../lepra/include/string.h"
#include "../../../uilepra/include/uiinput.h"
#include "../uitbc.h"
#include "../uipainter.h"
#include "uilayout.h"
#include "uiguiimagemanager.h"



namespace uitbc {



class GUIImageManager;



class Component: public uilepra::TextInputObserver, public uilepra::KeyCodeInputObserver, public uilepra::MouseInputObserver {
public:

	friend class DesktopWindow;

	enum Type {
		kComponent = 0,
		kRectcomponent,
		kBorder,
		kButton,
		kRadiobutton,
		kCaption,
		kLabel,
		kWindow,
		kDesktopwindow,
		kScrollbar,
		kListcontrol,
	};

	enum { // Area
		kAreaOutside = 0,
		kAreaInside,	// Empy client area.
		kAreaChild,
	};

	enum RecurseDir {
		kRecurseUp = 0,
		kRecurseDown,
	};

	Component(Layout* layout);
	virtual ~Component();
	void DeleteAllLayers();

	Component* GetParent();
	Layout* GetLayout(int layer = 0) const;
	void SetParentLayout(Layout* layout);
	Layout* GetParentLayout() const;

	int CreateLayer(Layout* layout);
	void DeleteLayer(int layer);
	void DeleteChildrenInLayer(int layer);
	void ReplaceLayer(int layer, Layout* layout);

	const PixelCoord& GetPos() const;
	PixelCoord GetScreenPos() const;

	const PixelCoord& GetSize() const;
	virtual PixelCoord GetMinSize() const;
	PixelRect GetScreenRect() const;

	void SetPreferredSize(const PixelCoord& size, bool adaptive = true);
	virtual void SetPreferredSize(int width, int height, bool adaptive = true);
	void SetPreferredWidth(int width);
	void SetPreferredHeight(int height);
	virtual PixelCoord GetPreferredSize(bool force_adaptive = false);
	int GetPreferredWidth(bool force_adaptive = false);
	int GetPreferredHeight(bool force_adaptive = false);
	void SetAdaptive(bool adaptive);
	bool IsAdaptive();

	void SetVisible(bool visible);
	bool IsVisible() const;
	bool IsLocalVisible() const;

	// Sets and gets the selected flag. The selected flag isn't actually used
	// within this class, but is stored here to make it easier to implement
	// list- and tree-controls.
	virtual void SetSelected(bool selected);
	bool GetSelected() const;

	virtual void Enable(bool enable);

	// The exact interpretation of param1 and param2 depends on the layout.
	virtual void AddChild(Component* child, int param1 = 0, int param2 = 0, int layer = 0);
	virtual void RemoveChild(Component* child, int layer);
	virtual int GetNumChildren() const;

	void AddTextListener(uilepra::TextInputObserver* listener);
	void RemoveTextListener(uilepra::TextInputObserver* listener);
	void AddKeyListener(uilepra::KeyCodeInputObserver* listener);
	void RemoveKeyListener(uilepra::KeyCodeInputObserver* listener);

	/*
		Coordinate convertions.
	*/

	PixelCoord ClientToWindow(const PixelCoord& coords);
	PixelCoord WindowToClient(const PixelCoord& coords);
	PixelCoord WindowToScreen(const PixelCoord& coords);
	PixelCoord ScreenToWindow(const PixelCoord& coords);
	PixelCoord ClientToScreen(const PixelCoord& coords);
	PixelCoord ScreenToClient(const PixelCoord& coords);

	PixelRect ClientToWindow(const PixelRect& rect);
	PixelRect WindowToClient(const PixelRect& rect);
	PixelRect WindowToScreen(const PixelRect& rect);
	PixelRect ScreenToWindow(const PixelRect& rect);
	PixelRect ClientToScreen(const PixelRect& rect);
	PixelRect ScreenToClient(const PixelRect& rect);

	/*
		Virtual functions and events.
	*/

	virtual void Repaint(Painter* painter);
	virtual void RepaintBackground(Painter* painter);
	virtual void RepaintComponents(Painter* painter);
	virtual void RepaintChild(Component* child, Painter* painter);
	virtual bool IsOver(int screen_x, int screen_y);

	virtual bool OnDoubleClick(int mouse_x, int mouse_y);

	virtual bool OnLButtonDown(int mouse_x, int mouse_y);
	virtual bool OnRButtonDown(int mouse_x, int mouse_y);
	virtual bool OnMButtonDown(int mouse_x, int mouse_y);

	virtual bool OnLButtonUp(int mouse_x, int mouse_y);
	virtual bool OnRButtonUp(int mouse_x, int mouse_y);
	virtual bool OnMButtonUp(int mouse_x, int mouse_y);

	virtual bool OnMouseWheel(int mouse_x, int mouse_y, int change, bool down);
	virtual bool OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y);

	virtual bool OnChar(wchar_t c);

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);

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
	void SetPos(const PixelCoord& pos);
	virtual void SetPos(int x, int y);
	void SetSize(const PixelCoord& size);
	virtual void SetSize(int width, int height);
	void SetMinSize(const PixelCoord& size);
	virtual void SetMinSize(int width, int height);

	virtual Type GetType() const;

	Component* GetChild(int screen_x, int screen_y, int levels_down = 0);

	GUIImageManager* GetImageManager();

	virtual void UpdateLayout();

	virtual void SetParent(Component* parent);
	bool IsChildOf(Component* parent);

	enum ComponentState {
		kStateFocusable	= 1,
		kStateClickable,
	};
	typedef std::pair<int, Component*> StateComponent;
	typedef std::list<StateComponent> StateComponentList;
	virtual StateComponentList GetStateList(ComponentState state);

	virtual void SetMouseFocus();
	virtual void ReleaseMouseFocus(RecurseDir dir = kRecurseUp, Component* focused_component = 0);
	bool HasMouseFocus() const;

	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(RecurseDir dir = kRecurseUp, Component* focused_component = 0);
	bool HasKeyboardFocus() const;

	void RequestRepaint();

	virtual bool IsComplete() const;

	Component* GetChild(const str& name) const;
	void SetName(const str& name);
	const str& GetName() const;

protected:
	virtual void DoSetPos(int x, int y);
	virtual void DoSetSize(int width, int height);
	virtual void DoSetMinSize(int width, int height);

	bool NeedsRepaint();
	// Should really be protected, but ScrollBar needs access to it.
	void SetNeedsRepaint(bool needs_repaint);

	// Some helper functions...
	Component* GetParentOfType(Type type);
	Component* GetTopParent();

	void SetImageManager(GUIImageManager* image_manager);

	virtual void SetKeyboardFocus(Component* child);
	virtual void SetMouseFocus(Component* child);
	void DispatchChar(wchar_t c);
	bool IsDispatcher() const;

	void DeleteLayout(int layer);

	typedef std::unordered_set<uilepra::TextInputObserver*, LEPRA_VOIDP_HASHER> TextListenerSet;
	typedef std::unordered_set<uilepra::KeyCodeInputObserver*, LEPRA_VOIDP_HASHER> KeyListenerSet;
	TextListenerSet text_listener_set_;
	KeyListenerSet key_listener_set_;

	Component* parent_;
	Component* mouse_focus_child_;
	Component* keyboard_focus_child_;
	Layout** layout_;
	Layout* parent_layout_;

	PixelCoord pos_;
	PixelCoord preferred_size_;
	PixelCoord size_;
	PixelCoord min_size_;

	bool needs_repaint_;
	bool visible_;
	bool adaptive_preferred_size_;
	bool selected_;
	bool enabled_;

	// Used by DesktopWindow to optimize rendering.
	Painter::ImageID image_id_;
	Canvas image_;

	int layer_count_;

	str name_;

	static GUIImageManager* image_manager_;

	logclass();
};



}
