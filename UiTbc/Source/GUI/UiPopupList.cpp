/*
	Class:  PopupList
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uipopuplist.h"



namespace uitbc {



PopupList::PopupList(unsigned border_style, int border_width, const Color& color, ListLayout::ListType list_type):
	ListControl(border_style, border_width, color, list_type) {
}

PopupList::PopupList(unsigned border_style, int border_width, Painter::ImageID image_id, ListLayout::ListType list_type):
	ListControl(border_style, border_width, image_id, list_type) {
}

PopupList::PopupList(const Color& color, ListLayout::ListType list_type):
	ListControl(color, list_type) {
}

PopupList::PopupList(Painter::ImageID image_id, ListLayout::ListType list_type):
	ListControl(image_id, list_type) {
}

PopupList::~PopupList() {
}

void PopupList::AddListener(Listener* listener) {
	listeners_.push_back(listener);
	listeners_.unique();
}

void PopupList::RemoveListener(Listener* listener) {
	listeners_.remove(listener);
}

void PopupList::SetKeyboardFocus() {
	Parent::SetKeyboardFocus();
	// Notify all listeners.
	ListenerList::iterator iter;
	for (iter = listeners_.begin(); iter != listeners_.end(); ++iter) {
		if ((*iter)->NotifySetKeyboardFocus(this) == false) {
			// TRICKY: The list control has been deleted and we must return immediately!
			return;
		}
	}
}

void PopupList::ReleaseKeyboardFocus(RecurseDir dir, Component* focused_component) {
	Parent::ReleaseKeyboardFocus(dir, focused_component);
	// Notify all listeners.
	ListenerList::iterator iter;
	for (iter = listeners_.begin(); iter != listeners_.end(); ++iter) {
		if ((*iter)->NotifyReleaseKeyboardFocus(this, focused_component) == false) {
			// TRICKY: The list control has been deleted and we must return immediately!
			return;
		}
	}
}

bool PopupList::OnLButtonDown(int mouse_x, int mouse_y) {
	bool return_value = Parent::OnLButtonDown(mouse_x, mouse_y);

	// Notify all listeners.
	ListenerList::iterator iter;
	for (iter = listeners_.begin(); iter != listeners_.end(); ++iter) {
		if ((*iter)->NotifyLButtonDown(this, mouse_x, mouse_y) == false) {
			// TRICKY: The list control has been deleted and we must return immediately!
			return return_value;
		}
	}

	return return_value;
}

bool PopupList::OnDoubleClick(int mouse_x, int mouse_y) {
	Parent::OnDoubleClick(mouse_x, mouse_y);

	// Notify all listeners.
	ListenerList::iterator iter;
	for (iter = listeners_.begin(); iter != listeners_.end(); ++iter) {
		if ((*iter)->NotifyDoubleClick(this, mouse_x, mouse_y) == false) {
			// TRICKY: The list control has been deleted and we must return immediately!
			break;
		}
	}
	return (false);
}

bool PopupList::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	Parent::OnKeyDown(key_code);

	// Notify all listeners.
	ListenerList::iterator iter;
	for (iter = listeners_.begin(); iter != listeners_.end(); ++iter) {
		if ((*iter)->NotifyKeyDown(this, key_code) == false) {
			// TRICKY: The list control has been deleted and we must return immediately!
			break;
		}
	}
	return (false);
}



}
