/*
	Class:  FloatingLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uifloatinglayout.h"
#include "../../include/gui/uiwindow.h"

namespace uitbc {

FloatingLayout::FloatingLayout() :
	Layout(),
	x_(kPosoffset),
	y_(kPosoffset),
	pos_count_(0) {
}

FloatingLayout::~FloatingLayout() {
}

Layout::Type FloatingLayout::GetType() const {
	return Layout::kFloatinglayout;
}

void FloatingLayout::Add(Component* component, int param1, int param2) {
	if (child_list_.empty() == false) {
		Component* child = child_list_.back();
		if (child->GetType() == Component::kWindow) {
			((Window*)child)->SetActive(false);
		}
	}

	child_list_.push_back(component);
	//child_list_.unique();

	if (component->GetType() == Component::kWindow) {
		((Window*)component)->SetActive(true);
	}

	if (param1 != 0 || param2 != 0) {
		component->SetPos(param1, param2);
	} else {
		component->SetPos(x_, y_);
		x_ += kPosoffset;
		y_ += kPosoffset;
		pos_count_++;

		if (pos_count_ >= 10) {
			x_ = kPosoffset;
			y_ = kPosoffset;
			pos_count_ = 0;
		}
	}
}

void FloatingLayout::Remove(Component* component) {
	child_list_.remove(component);

	if (!child_list_.empty()) {
		Component* comp = *(--child_list_.end());
		if (comp->GetType() == Component::kWindow) {
			((Window*)comp)->SetActive(true);
		}
	}
}

int FloatingLayout::GetNumComponents() const {
	return (int)child_list_.size();
}

Component* FloatingLayout::GetFirst() {
	iter_ = child_list_.begin();
	if (iter_ != child_list_.end()) {
		return *iter_;
	}
	return 0;
}

Component* FloatingLayout::GetNext() {
	if (iter_ != child_list_.end()) {
		++iter_;
		if (iter_ != child_list_.end()) {
			return *iter_;
		}
	}
	return 0;
}

Component* FloatingLayout::GetLast() {
	if(child_list_.empty()) {
		iter_ = child_list_.begin();
		return 0;
	} else {
		iter_ = --child_list_.end();
		return *iter_;
	}
}

Component* FloatingLayout::GetPrev() {
	if (iter_ != child_list_.begin()) {
		return *(--iter_);
	}

	return 0;
}

void FloatingLayout::UpdateLayout() {
	ComponentList::iterator iter;

	for (iter = child_list_.begin(); iter != child_list_.end(); ++iter) {
		Component* child = *iter;
		if (child->GetSize() != child->GetPreferredSize()) {
			child->SetSize(child->GetPreferredSize());
		}
	}
}

PixelCoord FloatingLayout::GetPreferredSize(bool /*force_adaptive*/) {
	return PixelCoord(0, 0);
}

PixelCoord FloatingLayout::GetMinSize() const {
	return PixelCoord(0, 0);
}

PixelCoord FloatingLayout::GetContentSize() const {
	return PixelCoord(0, 0);
}

void FloatingLayout::MoveToTop(Component* component) {
	ComponentList::iterator x = child_list_.begin();
	for (; x != child_list_.end(); ++x) {
		if (*x == component) {
			break;
		}
	}
	if (x != child_list_.end()) {
		Component* last = *(--child_list_.end());
		if (last->GetType() == Component::kWindow &&
		   last != component) {
			((Window*)last)->SetActive(false);
		}

		child_list_.erase(x);
		child_list_.push_back(component);
	}
}

}
