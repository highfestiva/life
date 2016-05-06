/*
	Class:  TextField
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This class implements the functionality of a text field with some extra
	features. It also handles optional popup-lists which can be used to select
	one item from a set (and then set the contents of the text field to match
	that item).


	This class is far from done... TODO:

	1. Implement text selection.
	2. Implement cut, copy & paste.
	3. Implement undo.
	4. Optimize rendering performance. It is far too slow.
*/



#pragma once

#include "../../../lepra/include/timer.h"
#include "uipopuplist.h"
#include "uitextcomponent.h"
#include "uiwindow.h"



namespace uitbc {



class DesktopWindow;



class TextField: public Window, public PopupList::Listener, public TextComponent {
	typedef Window Parent;
public:
	// top_parent points to the window that contains this textfield.
	// If this pointer is null the popup list feature will be disabled.
	// The popup list will be a child of this parent, in an upper layer.
	TextField(Component* top_parent);
	TextField(Component* top_parent, unsigned border_style, int border_width, const Color& color);
	TextField(Component* top_parent, unsigned border_style, int border_width, Painter::ImageID image_id);
	TextField(Component* top_parent, const Color& color);
	TextField(Component* top_parent, Painter::ImageID image_id);
	virtual ~TextField();

	Component* GetTopParent() const;
	void SetTopParent(Component* top_parent);

	void SetIsReadOnly(bool is_read_only);
	void SetPasswordCharacter(char character);

	wstr GetVisibleText() const;
	void SetText(const wstr& text);
	const wstr& GetText() const;

	void SetMarker(Painter::ImageID image_id);
	void SetMarkerBlinkRate(float64 visible_time, float64 invisible_time);
	void SetMarkerPosition(size_t index);
	size_t GetMarkerPosition() const;

	virtual void Repaint(Painter* painter);

	virtual bool OnChar(wchar_t c);
	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual void OnIdle();

	virtual bool OnLButtonDown(int mouse_x, int mouse_y);
	virtual bool OnLButtonUp(int mouse_x, int mouse_y);

	virtual bool OnMouseMove(int mouse_x, int mouse_y, int mouse_dx, int mouse_dy);

	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(RecurseDir dir = kRecurseUp, Component* focused_component = 0);

	// Override these and remove the functionality.
	virtual void SetCaption(Caption* caption);
	virtual void AddChild(Component* child, int param1 = 0, int param2 = 0, int layer = 0);

	virtual void DoSetPos(int x, int y);
	virtual void DoSetSize(int width, int height);

	// From PopupList::Listener.
	virtual bool NotifySetKeyboardFocus(PopupList* list);
	virtual bool NotifyReleaseKeyboardFocus(PopupList* list, Component* focused_component);
	virtual bool NotifyKeyDown(PopupList* list, uilepra::InputManager::KeyCode key_code);
	virtual bool NotifyLButtonDown(PopupList* list, int mouse_x, int mouse_y);
	virtual bool NotifyDoubleClick(PopupList* list, int mouse_x, int mouse_y);
protected:
	// Default behaviour is to return a null pointer, in which case
	// the popup list feature is disabled.
	virtual PopupList* CreatePopupList();

	void SpawnPopupList();
	void DestroyPopupList();

	PopupList* GetPopupList() const;

	virtual StateComponentList GetStateList(ComponentState state);

	void UpdateMarkerPos(Painter* painter);
	void SetMarkerPos(size_t pos);

	virtual void SetKeyboardFocus(Component* child);
	void SetupMarkerBlink();

	void ForceRepaint();

private:
	wstr text_;
	bool is_read_only_;
	char password_character_;
	int text_x_;

	Painter::ImageID marker_id_;
	size_t marker_pos_;
	bool marker_visible_;
	Timer marker_timer_;
	float64 marker_visible_time_;
	float64 marker_invisible_time_;

	bool update_marker_pos_on_next_repaint_;
	int click_x_;

	Component* top_parent_;
	DesktopWindow* desktop_window_;

	// The layer in top_parent_ where the popup list is.
	int list_layer_;
	PopupList* list_control_;
	bool delete_list_control_;
};



}
