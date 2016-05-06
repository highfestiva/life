/*
	Class:  ListLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/



#include "pch.h"
#include "../../include/gui/uilistlayout.h"
#include "../../include/gui/uicomponent.h"
#include "../../../lepra/include/listutil.h"



namespace uitbc {



ListLayout::ListLayout(ListType list_type):
	component_tree_(0, (float64)(1 << 30), 1.0),
	content_size_(0,0),
	pos_dx_(0),
	pos_dy_(0),
	list_hw_(0),
	indentation_size_(1),
	list_type_(list_type) {
	iter_ = node_list_.end();
}

ListLayout::~ListLayout() {
}

Layout::Type ListLayout::GetType() const {
	return Layout::kListlayout;
}

void ListLayout::Add(Component* component, int param1, int /*param2*/) {
	float64 half_hw = (float64)GetPreferredHW(component) / 2.0;
	float64 pos    = (float64)list_hw_ + half_hw;
	component_tree_.InsertObject(component, component, pos, half_hw);

	node_list_.push_back(Node(component, param1));
	component_table_.Insert(component, --node_list_.end());

	list_hw_ += GetPreferredHW(component);
}

void ListLayout::AddChildAfter(Component* child, Component* after_this, int indentation_level) {
	ComponentTable::Iterator t_iter = component_table_.Find(after_this);

	if (t_iter != component_table_.End()) {
		NodeList::iterator before_this_iter(*t_iter);
		++before_this_iter;
		NodeList::iterator child_iter = node_list_.insert(before_this_iter, Node(child, indentation_level));
		component_table_.Insert(child, child_iter);

		int hw = GetPreferredHW(child);
		list_hw_ += hw;

		// Get the size and position of the existing child.
		float64 prev_half_hw;
		float64 prev_pos;
		component_tree_.GetObjectSizeAndPos((*(*t_iter)).component_, prev_pos, prev_half_hw);

		// The position and the size of the new child.
		float64 half_hw = (float64)hw / 2.0;
		float64 pos    = prev_pos + prev_half_hw + half_hw;

		float64 new_pos = pos + half_hw;

		// Move all affected objects in the component tree.
		NodeList::iterator iter2(child_iter);
		++iter2;
		for (; iter2 != node_list_.end(); ++iter2) {
			float64 new_hw = GetPreferredHW((*iter2).component_);
			component_tree_.MoveObject((*iter2).component_, new_pos + new_hw / 2.0, new_hw / 2.0);
			new_pos += new_hw;
		}

		// Insert the new child in the component tree.
		component_tree_.InsertObject(child, child, pos, half_hw);
	}
}

void ListLayout::AddChildrenAfter(std::list<Component*>& child_list, Component* after_this, int indentation_level) {
	ComponentTable::Iterator t_iter = component_table_.Find(after_this);

	if (t_iter != component_table_.End()) {
		NodeList::iterator after_this_iter(*t_iter);

		// Get the size and position of the existing child.
		float64 half_hw;
		float64 pos;
		component_tree_.GetObjectSizeAndPos((*after_this_iter).component_, pos, half_hw);
		pos += half_hw;

		NodeList::iterator next(after_this_iter);
		++next;

		std::list<Component*>::iterator cl_iter;
		for (cl_iter = child_list.begin(); cl_iter != child_list.end(); ++cl_iter) {
			Component* _child = *cl_iter;

			NodeList::iterator child_iter = node_list_.insert(++after_this_iter, Node(_child, indentation_level));
			after_this_iter = child_iter;
			component_table_.Insert(_child, child_iter);

			int hw = GetPreferredHW(_child);
			list_hw_ += hw;

			half_hw = (float64)hw / 2.0;

			// Insert the new child in the component tree.
			component_tree_.InsertObject(_child, _child, pos + half_hw, half_hw);

			pos += (float64)hw;
		}


		// Move all affected objects in the component tree.
		for (; next != node_list_.end(); ++next) {
			float64 new_hw = GetPreferredHW((*next).component_);
			component_tree_.MoveObject((*next).component_, pos + new_hw / 2.0, new_hw / 2.0);
			pos += new_hw;
		}
	}
}

void ListLayout::Remove(Component* component) {
	ComponentTable::Iterator t_iter = component_table_.Find(component);
	if (t_iter != component_table_.End()) {
		NodeList::iterator iter = (*t_iter);
		node_list_.erase(iter);
		component_tree_.RemoveObject(component);
		component_table_.Remove(t_iter);

		// We need to loop through the entire list and update the layout.

		list_hw_ = 0;
		int i = 0;

		for (iter = node_list_.begin(); iter != node_list_.end(); ++iter) {
			Node& node = *iter;
			int hw = GetPreferredHW(node.component_);

			float64 half_hw = (float64)hw / 2.0;
			float64 pos    = (float64)list_hw_ + half_hw;
			component_tree_.MoveObject(node.component_, pos, half_hw);
			list_hw_ += hw;
			i++;
		}
	}
}

int ListLayout::GetNumComponents() const {
	return (int)node_list_.size();
}

Component* ListLayout::Find(int screen_xy) {
	PixelCoord owner_pos(GetOwner()->GetScreenPos());
	float64 pos = 0;

	if (list_type_ == kColumn) {
		pos = (float64)(screen_xy - owner_pos.y - pos_dy_);
	} else { // if (list_type_ == kRow)
		pos = (float64)(screen_xy - owner_pos.x - pos_dx_);
	}

	ComponentTree::ObjectList __list;
	component_tree_.GetObjects(__list, pos, 0.5);

	if (__list.empty() == true) {
		return 0;
	}

	if (__list.size() == 1) {
		return __list.front();
	}

	// The list shouldn't contain more than one component.
	// But if it does, somehow, let's find the correct one.
	ComponentTree::ObjectList::iterator iter;
	for (iter = __list.begin(); iter != __list.end(); ++iter) {
		Component* comp = *iter;
		PixelRect rect(comp->GetScreenRect());

		if ((list_type_ == kColumn && rect.IsInside(rect.GetCenterX(), screen_xy) == true) ||
		    (list_type_ == kRow && rect.IsInside(screen_xy, rect.GetCenterY()) == true)) {
			return comp;
		}
	}

	return 0;
}

void ListLayout::Find(ComponentList& components, int screen_x_y1, int screen_x_y2) {
	PixelCoord owner_pos(GetOwner()->GetScreenPos());

	if (screen_x_y1 > screen_x_y2) {
		int temp = screen_x_y1;
		screen_x_y1 = screen_x_y2;
		screen_x_y2 = temp;
	}

	float64 upper = 0;
	float64 lower = 0;

	if (list_type_ == kColumn) {
		upper = (float64)(screen_x_y1 - owner_pos.y - pos_dy_);
		lower = (float64)(screen_x_y2 - owner_pos.y - pos_dy_);
	} else { // if (list_type_ == kRow)
		upper = (float64)(screen_x_y1 - owner_pos.x - pos_dx_);
		lower = (float64)(screen_x_y2 - owner_pos.x - pos_dx_);
	}

	component_tree_.GetObjects(components, (lower + upper) * 0.5, (lower - upper) * 0.5);
}

Component* ListLayout::FindIndex(int index) {
	Component* _component = 0;

	if (index >= 0 && index < (int)node_list_.size()) {
		_component = (*ListUtil::FindByIndex(node_list_, index)).component_;
	}

	return _component;
}

Component* ListLayout::GetFirst() {
	if (node_list_.empty() == true) {
		return 0;
	}

	iter_ = node_list_.begin();
	return (*iter_).component_;
}

Component* ListLayout::GetNext() {
	++iter_;
	if (iter_ == node_list_.end()) {
		return 0;
	}

	return (*iter_).component_;
}

Component* ListLayout::GetLast() {
	if (node_list_.empty() == true) {
		return 0;
	}

	iter_ = --node_list_.end();
	return (*iter_).component_;
}

Component* ListLayout::GetPrev() {
	--iter_;
	if (iter_ == node_list_.end()) {
		return 0;
	}

	return (*iter_).component_;
}

Component* ListLayout::GetNext(Component* current) {
	Component* next = 0;
	ComponentTable::Iterator iter = component_table_.Find(current);

	if (iter != component_table_.End()) {
		NodeList::iterator list_iter = (*iter);
		++list_iter;

		if (list_iter != node_list_.end()) {
			next = (*list_iter).component_;
		}
	}

	return next;
}

Component* ListLayout::GetPrev(Component* current) {
	Component* prev = 0;
	ComponentTable::Iterator iter = component_table_.Find(current);

	if (iter != component_table_.End()) {
		NodeList::iterator list_iter = (*iter);
		--list_iter;

		if (list_iter != node_list_.end()) {
			prev = (*list_iter).component_;
		}
	}

	return prev;
}

void ListLayout::UpdateLayout() {
	content_size_.x = 0;
	content_size_.y = 0;

	PixelCoord owner_size(GetOwner()->GetSize());
	NodeList::iterator iter;

	if (list_type_ == kColumn) {
		for (iter = node_list_.begin(); iter != node_list_.end(); ++iter) {
			Node& node = *iter;
			PixelCoord __size(node.component_->GetPreferredWidth(true), node.component_->GetPreferredHeight());

			__size.x += node.indentation_level_ * indentation_size_;

			if (__size.x < owner_size.x) {
				__size.x = owner_size.x;
			}

			if (content_size_.x < __size.x) {
				content_size_.x = __size.x;
			}

			content_size_.y += __size.y;
		}

		int y = pos_dy_;
		for (iter = node_list_.begin(); iter != node_list_.end(); ++iter) {
			Node& node = *iter;
			node.component_->SetPos(pos_dx_ + node.indentation_level_ * indentation_size_, y);
			PixelCoord __size(node.component_->GetPreferredSize());

			__size.x = content_size_.x;
			node.component_->SetSize(__size);

			y += __size.y;
		}
	} else { // if (list_type_ == kRow)
		for (iter = node_list_.begin(); iter != node_list_.end(); ++iter) {
			Node& node = *iter;
			PixelCoord __size(node.component_->GetPreferredWidth(), node.component_->GetPreferredHeight(true));

			__size.y += node.indentation_level_ * indentation_size_;

			if (__size.y < owner_size.y) {
				__size.y = owner_size.y;
			}

			if (content_size_.y < __size.y) {
				content_size_.y = __size.y;
			}

			content_size_.x += __size.x;
		}

		int x = pos_dx_;
		for (iter = node_list_.begin(); iter != node_list_.end(); ++iter) {
			Node& node = *iter;
			node.component_->SetPos(x, pos_dy_ + node.indentation_level_ * indentation_size_);
			PixelCoord __size(node.component_->GetPreferredSize());

			__size.y = content_size_.y;
			node.component_->SetSize(__size);

			x += __size.x;
		}
	}
}

PixelCoord ListLayout::GetPreferredSize(bool /*force_adaptive*/) {
	return PixelCoord(0, 0);
}

PixelCoord ListLayout::GetMinSize() const {
	return PixelCoord(0, 0);
}

PixelCoord ListLayout::GetContentSize() const {
	return content_size_;
}

int ListLayout::GetPreferredHW(Component* component) {
	return list_type_ == kColumn ? component->GetPreferredHeight() : component->GetPreferredWidth();
}

ListLayout::ListType ListLayout::GetListType() const {
	return list_type_;
}

void ListLayout::SetPosOffset(int dx, int dy) {
	pos_dx_ = dx;
	pos_dy_ = dy;
}

int ListLayout::GetPosDX() const {
	return pos_dx_;
}

int ListLayout::GetPosDY() const {
	return pos_dy_;
}

float64 ListLayout::GetAverageComponentHW() const {
	float64 hw = 0;
	if (list_type_ == kColumn) {
		hw = (float64)content_size_.y / (float64)node_list_.size();
	} else { // if (list_type_ == kRow)
		hw = (float64)content_size_.x / (float64)node_list_.size();
	}

	return hw;
}

bool ListLayout::IsEmpty() const {
	return node_list_.empty();
}

void ListLayout::SetIndentationSize(int indentation_size) {
	indentation_size_ = indentation_size;
}

int ListLayout::GetIndentationSize() const {
	return indentation_size_;
}



}
