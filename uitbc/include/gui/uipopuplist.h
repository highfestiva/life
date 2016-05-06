/*
	Class:  PopupList
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This component extends the ListControl with the functionality required to work
	as a popup menu.
*/

#pragma once

#include "uilistcontrol.h"
#include "../../../uilepra/include/uiinput.h"

namespace uitbc {

class PopupList: public ListControl {
	typedef ListControl Parent;
public:
	class Listener {
	public:
		// Both functions returns false if the list control has been deleted.
		virtual bool NotifySetKeyboardFocus(PopupList* _list) = 0;
		virtual bool NotifyReleaseKeyboardFocus(PopupList* _list, Component* focused_component) = 0;
		virtual bool NotifyKeyDown(PopupList* _list, uilepra::InputManager::KeyCode key_code) = 0;
		virtual bool NotifyLButtonDown(PopupList* _list, int mouse_x, int mouse_y) = 0;
		virtual bool NotifyDoubleClick(PopupList* _list, int mouse_x, int mouse_y) = 0;
	};

	PopupList(unsigned border_style,
		  int border_width,
		  const Color& color,
		  ListLayout::ListType list_type = ListLayout::kColumn);
	PopupList(unsigned border_style,
		  int border_width,
		  Painter::ImageID image_id,
		  ListLayout::ListType list_type = ListLayout::kColumn);
	PopupList(const Color& color, ListLayout::ListType list_type = ListLayout::kColumn);
	PopupList(Painter::ImageID image_id, ListLayout::ListType list_type = ListLayout::kColumn);

	virtual ~PopupList();

	void AddListener(Listener* listener);
	void RemoveListener(Listener* listener);

	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(RecurseDir dir = kRecurseUp, Component* focused_component = 0);

	bool OnLButtonDown(int mouse_x, int mouse_y);
	bool OnDoubleClick(int mouse_x, int mouse_y);

	bool OnKeyDown(uilepra::InputManager::KeyCode key_code);

protected:
private:
	typedef std::list<Listener*> ListenerList;

	ListenerList listeners_;
};

}
