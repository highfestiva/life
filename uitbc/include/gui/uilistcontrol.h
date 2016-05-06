/*
	Class:  ListControl
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	The list control will store components in, and render them as, a list.
	The components should preferrably be Labels, but can be of any type.

	The list control assumes that each component has a static preferred size.
	If you change the size of one component, you have to rebuild the entire
	list. The list won't work properly otherwise.
*/

#pragma once

#include "uiwindow.h"
#include "uigridlayout.h"
#include "uilistlayout.h"
#include "uiscrollbar.h"
#include "uirectcomponent.h"
#include "uilabel.h"
#include <list>
#include "../../../lepra/include/hashtable.h"



namespace uitbc {



class ListControl: public Window {
	typedef Window Parent;
public:

	enum Style {
		kSingleSelect = 0,	// Select at most one (by clicking).
		kMultiSelect,		// Select zero or more.
		kMenuSelect,	// Like single select, but selects by hovering with the cursor.
	};

	ListControl(unsigned border_style,
		    int border_width,
		    const Color& color,
		    ListLayout::ListType list_type = ListLayout::kColumn);
	ListControl(unsigned border_style,
		    int border_width,
		    Painter::ImageID image_id,
		    ListLayout::ListType list_type = ListLayout::kColumn);
	ListControl(const Color& color, ListLayout::ListType list_type = ListLayout::kColumn);
	ListControl(Painter::ImageID image_id, ListLayout::ListType list_type = ListLayout::kColumn);

	virtual ~ListControl();

	inline Style GetStyle();
	inline void SetStyle(Style style);

	void SetScrollBars(ScrollBar* h_scroll_bar, ScrollBar* v_scroll_bar, RectComponent* corner_rect);

	virtual void AddChild(Component* child, int param1 = 0, int param2 = 0, int layer = 0);
	virtual void RemoveChild(Component* child, int layer);
	virtual int GetNumChildren() const;

	void AddChildAfter(Component* child, Component* after_this, int indentation_level);
	void AddChildrenAfter(std::list<Component*>& child_list, Component* after_this, int indentation_level);

	void GetScrollOffsets(int& horizontal_offset, int& vertical_offset) const;
	void SetScrollOffsets(int horizontal_offset, int vertical_offset);

	void SetItemSelected(int item_index, bool selected);

	Component* GetFirstSelectedItem();
	Component* GetNextSelectedItem();

	virtual bool OnLButtonDown(int mouse_x, int mouse_y);
	virtual bool OnLButtonUp(int mouse_x, int mouse_y);

	virtual bool OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y);

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);

	// Override this if implementing a popup menu.
	virtual void OnItemSelected(Component* item);

	virtual void DoSetMinSize(int width, int height);

	virtual void UpdateLayout();

	inline virtual Type GetType() const;

	inline PixelCoord GetContentSize();

protected:
	void Init(ListLayout::ListType list_type);
private:

	typedef std::list<Component*> ComponentList;

	void Select(Component* child, int selected_x, int selected_y);
	void SetSelected(Component* child, bool selected);
	void DeselectAll();

	void UpdateScrollPos();

	void ScrollToChild(Component* child);

	ComponentList selected_list_;

	Style style_;

	ScrollBar* h_scroll_bar_;
	ScrollBar* v_scroll_bar_;

	RectComponent* list_rect_;
	// The square in the bottom right corner where the scroll bars meet.
	RectComponent* corner_rect_;

	Component* last_selected_;
};

ListControl::Style ListControl::GetStyle() {
	return style_;
}

void ListControl::SetStyle(Style style) {
	style_ = style;
}

Component::Type ListControl::GetType() const {
	return Component::kListcontrol;
}

PixelCoord ListControl::GetContentSize() {
	return ((ListLayout*)list_rect_->GetLayout())->GetContentSize();
}



}
