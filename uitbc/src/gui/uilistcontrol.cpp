/*
	Class:  ListControl
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uidesktopwindow.h"
#include "../../include/gui/uilistcontrol.h"
#include "../../../uilepra/include/uiinput.h"

namespace uitbc {

ListControl::ListControl(unsigned border_style, int border_width, const Color& color, ListLayout::ListType list_type):
	Window(border_style, border_width, color, new GridLayout(2, 2)),
	style_(kSingleSelect),
	h_scroll_bar_(0),
	v_scroll_bar_(0),
	list_rect_(0),
	corner_rect_(0),
	last_selected_(0) {
	Init(list_type);
}

ListControl::ListControl(unsigned border_style, int border_width, Painter::ImageID image_id, ListLayout::ListType list_type):
	Window(border_style, border_width, image_id, new GridLayout(2, 2)),
	style_(kSingleSelect),
	h_scroll_bar_(0),
	v_scroll_bar_(0),
	list_rect_(0),
	corner_rect_(0),
	last_selected_(0) {
	Init(list_type);
}

ListControl::ListControl(const Color& color, ListLayout::ListType list_type):
	Window(color, new GridLayout(2, 2)),
	style_(kSingleSelect),
	h_scroll_bar_(0),
	v_scroll_bar_(0),
	list_rect_(0),
	corner_rect_(0),
	last_selected_(0) {
	Init(list_type);
}

ListControl::ListControl(Painter::ImageID image_id, ListLayout::ListType list_type):
	Window(image_id, new GridLayout(2, 2)),
	style_(kSingleSelect),
	h_scroll_bar_(0),
	v_scroll_bar_(0),
	list_rect_(0),
	corner_rect_(0),
	last_selected_(0) {
	Init(list_type);
}

ListControl::~ListControl() {
}

void ListControl::Init(ListLayout::ListType list_type) {
	h_scroll_bar_ = new ScrollBar(ScrollBar::kHorizontal);
	v_scroll_bar_ = new ScrollBar(ScrollBar::kVertical);
	list_rect_   = new RectComponent(new ListLayout(list_type));
	corner_rect_ = new RectComponent(Color(192, 192, 192));

	h_scroll_bar_->SetPreferredSize(0, 16, false);
	v_scroll_bar_->SetPreferredSize(16, 0, false);
	corner_rect_->SetPreferredSize(16, 16, false);
	corner_rect_->SetMinSize(16, 16);

	h_scroll_bar_->SetOwner(list_rect_);
	v_scroll_bar_->SetOwner(list_rect_);

	PixelCoord vsb_min_size(v_scroll_bar_->GetMinSize());
	PixelCoord hsb_min_size(h_scroll_bar_->GetMinSize());
	PixelCoord c_min_size(corner_rect_->GetMinSize());
	PixelCoord min_size(c_min_size.x + hsb_min_size.x + GetTotalBorderWidth(),
				     c_min_size.y + vsb_min_size.y + GetTotalBorderWidth());
	SetMinSize(min_size);

	h_scroll_bar_->SetVisible(false);
	v_scroll_bar_->SetVisible(false);
	corner_rect_->SetVisible(false);

	Parent::AddChild(list_rect_, 0, 0);
	Parent::AddChild(h_scroll_bar_, 1, 0);
	Parent::AddChild(v_scroll_bar_, 0, 1);
	Parent::AddChild(corner_rect_, 1, 1);

	Parent::Init();
}

void ListControl::SetScrollBars(ScrollBar* h_scroll_bar,
				ScrollBar* v_scroll_bar,
				RectComponent* corner_rect) {
	if (h_scroll_bar != 0) {
		Parent::RemoveChild(h_scroll_bar_, 0);
		delete h_scroll_bar_;
		h_scroll_bar_ = h_scroll_bar;
		h_scroll_bar_->SetOwner(list_rect_);
		Parent::AddChild(h_scroll_bar_, 1, 0);
	}

	if (v_scroll_bar != 0) {
		Parent::RemoveChild(v_scroll_bar_, 0);
		delete v_scroll_bar_;
		v_scroll_bar_ = v_scroll_bar;
		v_scroll_bar_->SetOwner(list_rect_);
		Parent::AddChild(v_scroll_bar_, 0, 1);
	}

	if (corner_rect != 0) {
		Parent::RemoveChild(corner_rect_, 0);
		delete corner_rect_;
		corner_rect_ = corner_rect;
		Parent::AddChild(corner_rect_, 1, 1);

		corner_rect_->SetMinSize(corner_rect_->GetPreferredSize());
	}

	PixelCoord vsb_min_size(v_scroll_bar_->GetMinSize());
	PixelCoord hsb_min_size(h_scroll_bar_->GetMinSize());
	PixelCoord c_min_size(corner_rect_->GetMinSize());
	PixelCoord min_size(c_min_size.x + vsb_min_size.x,
				     c_min_size.y + hsb_min_size.y);
	SetMinSize(min_size);

}

void ListControl::AddChild(Component* child, int param1, int param2, int layer) {
	list_rect_->AddChild(child, param1, param2, layer);

	if (last_selected_ == 0) {
		last_selected_ = child;
	}
}

void ListControl::AddChildAfter(Component* child, Component* after_this, int indentation_level) {
	((ListLayout*)list_rect_->GetLayout())->AddChildAfter(child, after_this, indentation_level);

	child->SetParent(list_rect_);

	if (last_selected_ == 0) {
		last_selected_ = child;
	}
}

void ListControl::AddChildrenAfter(std::list<Component*>& child_list, Component* after_this, int indentation_level) {
	((ListLayout*)list_rect_->GetLayout())->AddChildrenAfter(child_list, after_this, indentation_level);

	std::list<Component*>::iterator iter;
	for (iter = child_list.begin(); iter != child_list.end(); ++iter) {
		(*iter)->SetParent(list_rect_);
	}

	if (last_selected_ == 0 && child_list.empty() == false) {
		last_selected_ = *(--child_list.end());
	}

	SetNeedsRepaint(true);
}

void ListControl::RemoveChild(Component* child, int layer) {
	list_rect_->RemoveChild(child, layer);
}

int ListControl::GetNumChildren() const {
	return list_rect_->GetNumChildren();
}

Component* ListControl::GetFirstSelectedItem() {
	Layout* layout = list_rect_->GetLayout();
	Component* item = layout->GetFirst();

	while (item != 0 && item->GetSelected() == false) {
		item = layout->GetNext();
	}

	return item;
}

Component* ListControl::GetNextSelectedItem() {
	Layout* layout = list_rect_->GetLayout();
	Component* item = layout->GetNext();

	while (item != 0 && item->GetSelected() == false) {
		item = layout->GetNext();
	}

	return item;
}

bool ListControl::OnLButtonDown(int mouse_x, int mouse_y) {
	if (list_rect_->IsOver(mouse_x, mouse_y) == true) {
		ListLayout* layout = (ListLayout*)list_rect_->GetLayout();

		if (layout->IsEmpty() == true) {
			return true;
		}

		Select(layout->Find(layout->GetListType() == ListLayout::kColumn ? mouse_y : mouse_x), mouse_x, mouse_y);
	}

	return Parent::OnLButtonDown(mouse_x, mouse_y);
}

bool ListControl::OnLButtonUp(int mouse_x, int mouse_y) {
	return Parent::OnLButtonUp(mouse_x, mouse_y);
}

bool ListControl::OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y) {
	if (style_ == kMenuSelect) {
		DeselectAll();
		ListLayout* layout = (ListLayout*)list_rect_->GetLayout();
		SetSelected(layout->Find(mouse_y), true);
	}
	return (Parent::OnMouseMove(mouse_x, mouse_y, delta_x, delta_y));
}

bool ListControl::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	Parent::OnKeyDown(key_code);

	ListLayout* layout = (ListLayout*)list_rect_->GetLayout();

	Component* child_to_select = 0;

	if (layout->GetListType() == ListLayout::kColumn) {
		if (key_code == uilepra::InputManager::kInKbdDown) {
			child_to_select = layout->GetNext(last_selected_);
		}
		if (key_code == uilepra::InputManager::kInKbdUp) {
			child_to_select = layout->GetPrev(last_selected_);
		}
	} else { //if (layout->GetListType() == ListLayout::kRow)
		if (key_code == uilepra::InputManager::kInKbdRight) {
			child_to_select = layout->GetNext(last_selected_);
		}
		if (key_code == uilepra::InputManager::kInKbdLeft) {
			child_to_select = layout->GetPrev(last_selected_);
		}
	}

	if (child_to_select != 0) {
		PixelRect rect(child_to_select->GetScreenRect());
		Select(child_to_select, rect.GetCenterX(), rect.GetCenterY());
	}
	return (false);
}

bool ListControl::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	return (Parent::OnKeyUp(key_code));
}

void ListControl::OnItemSelected(Component* /*item*/) {
	// This is a dummy implementation, do nothing.
}

void ListControl::Select(Component* child, int selected_x, int selected_y) {
	if (child != 0) {
		ListLayout* layout = (ListLayout*)list_rect_->GetLayout();

		if (style_ == kMultiSelect) {
			const DesktopWindow* desktop_window = (DesktopWindow*)GetParentOfType(kDesktopwindow);
			uilepra::InputManager* input_manager = desktop_window->GetInputManager();
			bool ctrl  = input_manager->ReadKey(uilepra::InputManager::kInKbdLCtrl) || input_manager->ReadKey(uilepra::InputManager::kInKbdRCtrl);
			bool shift = input_manager->ReadKey(uilepra::InputManager::kInKbdLShift) || input_manager->ReadKey(uilepra::InputManager::kInKbdRShift);

			if (ctrl == false && shift == false) {
				DeselectAll();
			}

			if (shift == true) {
				if (last_selected_ == 0) {
					last_selected_ = layout->GetFirst();
				}

				ListLayout::ComponentList __list;
				PixelRect rect(last_selected_->GetScreenRect());

				if (layout->GetListType() == ListLayout::kColumn) {
					layout->Find(__list, rect.GetCenterY(), selected_y);
				} else {
					layout->Find(__list, rect.GetCenterX(), selected_x);
				}

				ListLayout::ComponentList::iterator iter;

				if (child->GetSelected() == false) {
					for (iter = __list.begin(); iter != __list.end(); ++iter) {
						SetSelected(*iter, true);
					}
				} else {
					for (iter = __list.begin(); iter != __list.end(); ++iter) {
						SetSelected(*iter, false);
					}
				}
			} else {
				if (child->GetSelected() == true) {
					SetSelected(child, false);
				} else {
					SetSelected(child, true);
					OnItemSelected(child);
				}
			}
		} else {
			DeselectAll();
			SetSelected(child, true);
			OnItemSelected(child);
		}

		last_selected_ = child;
		child->SetKeyboardFocus();

		ScrollToChild(last_selected_);
	}
}

void ListControl::SetSelected(Component* child, bool selected) {
	if (selected == true) {
		if (child != 0 && child->GetSelected() == false) {
			child->SetSelected(true);
			selected_list_.push_back(child);
		}
	} else {
		if (child != 0 && child->GetSelected() == true) {
			child->SetSelected(false);
			selected_list_.remove(child);
		}
	}
}

void ListControl::SetItemSelected(int item_index, bool selected) {
	if (item_index >= 0 && item_index < GetNumChildren()) {
		Component* _child = ((ListLayout*)list_rect_->GetLayout())->FindIndex(item_index);
		SetSelected(_child, selected);
	}
}


void ListControl::DeselectAll() {
	ComponentList::iterator iter;

	for (iter = selected_list_.begin(); iter != selected_list_.end(); ++iter) {
		(*iter)->SetSelected(false);
	}

	selected_list_.clear();
}

void ListControl::UpdateScrollPos() {
	int h_offset;
	int v_offset;

	GetScrollOffsets(h_offset, v_offset);

	ListLayout* layout = (ListLayout*)list_rect_->GetLayout();
	layout->SetPosOffset(h_offset, v_offset);

	SetNeedsRepaint(true);
}

void ListControl::GetScrollOffsets(int& horizontal_offset, int& vertical_offset) const {
	ListLayout* layout = (ListLayout*)list_rect_->GetLayout();
	PixelCoord size_diff = list_rect_->GetSize() - layout->GetContentSize();

	horizontal_offset = (int)(h_scroll_bar_->GetScrollPos() * size_diff.x);
	vertical_offset = (int)(v_scroll_bar_->GetScrollPos() * size_diff.y);

	if (horizontal_offset > 0) {
		horizontal_offset = 0;
	}

	if (vertical_offset > 0) {
		vertical_offset = 0;
	}
}

void ListControl::SetScrollOffsets(int horizontal_offset, int vertical_offset) {
	ListLayout* layout = (ListLayout*)list_rect_->GetLayout();
	PixelCoord size_diff = list_rect_->GetSize() - layout->GetContentSize();

	h_scroll_bar_->SetScrollPos((float64)horizontal_offset / (float64)size_diff.x);
	v_scroll_bar_->SetScrollPos((float64)vertical_offset / (float64)size_diff.y);

	layout->SetPosOffset(horizontal_offset, vertical_offset);
}

void ListControl::ScrollToChild(Component* child) {
	if (child != 0) {
		ListLayout* layout = (ListLayout*)list_rect_->GetLayout();

		PixelRect rect(child->GetScreenRect());
		PixelRect client_rect(list_rect_->GetScreenRect());
		PixelCoord size_diff = layout->GetContentSize() -
			PixelCoord(client_rect.GetWidth(), client_rect.GetHeight());

		if (layout->GetListType() == ListLayout::kColumn) {
			// Check if we need to scroll down.
			if (rect.bottom_ > client_rect.bottom_) {
				float64 dy = (float64)(rect.bottom_ - client_rect.bottom_) / (float64)size_diff.y;
				v_scroll_bar_->SetScrollPos(v_scroll_bar_->GetScrollPos() + dy);
			}

			// Check if we need to scroll up.
			if (rect.top_ < client_rect.top_) {
				float64 dy = (float64)(rect.top_ - client_rect.top_) / (float64)size_diff.y;
				v_scroll_bar_->SetScrollPos(v_scroll_bar_->GetScrollPos() + dy);
			}
		} else {
			// Check if we need to scroll right.
			if (rect.right_ > client_rect.right_) {
				float64 dx = (float64)(rect.right_ - client_rect.right_) / (float64)size_diff.x;
				h_scroll_bar_->SetScrollPos(h_scroll_bar_->GetScrollPos() + dx);
			}

			// Check if we need to scroll up.
			if (rect.left_ < client_rect.left_) {
				float64 dx = (float64)(rect.left_ - client_rect.left_) / (float64)size_diff.x;
				h_scroll_bar_->SetScrollPos(h_scroll_bar_->GetScrollPos() + dx);
			}
		}
	}
}

void ListControl::DoSetMinSize(int width, int height) {
	Parent::DoSetMinSize(width, height);
}

void ListControl::UpdateLayout() {
	Parent::UpdateLayout();

	float64 average = ((ListLayout*)list_rect_->GetLayout())->GetAverageComponentHW();

	bool changed;

	do {
		changed = false;

		PixelCoord content_size(list_rect_->GetLayout()->GetContentSize());
		PixelCoord size(list_rect_->GetSize());

		h_scroll_bar_->SetScrollRatio((float64)size.x / average, (float64)content_size.x / average);
		v_scroll_bar_->SetScrollRatio((float64)size.y / average, (float64)content_size.y / average);

		if (content_size.x > size.x) {
			changed = changed || !h_scroll_bar_->IsVisible();
			h_scroll_bar_->SetVisible(true);
		} else {
			changed = changed || h_scroll_bar_->IsVisible();
			h_scroll_bar_->SetVisible(false);
			h_scroll_bar_->SetScrollPos(0);
		}

		if (content_size.y > size.y) {
			changed = changed || !v_scroll_bar_->IsVisible();
			v_scroll_bar_->SetVisible(true);
		} else {
			changed = changed || v_scroll_bar_->IsVisible();
			v_scroll_bar_->SetVisible(false);
			v_scroll_bar_->SetScrollPos(0);
		}

		if (v_scroll_bar_->IsVisible() == true &&
		   h_scroll_bar_->IsVisible() == true) {
			corner_rect_->SetVisible(true);
		} else {
			corner_rect_->SetVisible(false);
		}

		// Update the layout one more time.
		Parent::UpdateLayout();
	} while(changed == true);

	UpdateScrollPos();
	Parent::UpdateLayout();
}



}
