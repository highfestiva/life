/*
	Class:  StackLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uistacklayout.h"
#include "../../include/gui/uicomponent.h"

namespace uitbc {

StackLayout::StackLayout(int levels) :
	Layout(),
	child_(0),
	num_levels_(levels) {
	if (num_levels_ > 0) {
		child_ = new Component*[num_levels_];
		for (int i = 0; i < num_levels_; i++) {
			child_[i] = 0;
		}
	}
}

StackLayout::~StackLayout() {
	if (num_levels_ > 0) {
		delete[] child_;
	}
}

Layout::Type StackLayout::GetType() const {
	return Layout::kStacklayout;
}

void StackLayout::Add(Component* component, int param1, int /*param2*/) {
	if (param1 >= 0 && param1 < num_levels_) {
		while (param1 < num_levels_ && child_[param1] != 0) {
			param1++;
		}

		if (param1 < num_levels_) {
			child_[param1] = component;
			num_components_++;
		}
	}
}

void StackLayout::Remove(Component* component) {
	for (int i = 0; i < num_levels_; i++) {
		if (child_[i] == component) {
			child_[i] = 0;
			num_components_--;
		}
	}
}

int StackLayout::GetNumComponents() const {
	return num_components_;
}

Component* StackLayout::GetFirst() {
	for (current_level_ = 0;
		current_level_ < num_levels_;
		current_level_++) {
		if (child_[current_level_] != 0) {
			return child_[current_level_];
		}
	}

	return 0;
}

Component* StackLayout::GetNext() {
	for (++current_level_;
		current_level_ >= 0 && current_level_ < num_levels_;
		current_level_++) {
		if (child_[current_level_] != 0) {
			return child_[current_level_];
		}
	}

	return 0;
}

Component* StackLayout::GetLast() {
	for (current_level_ = num_levels_ - 1;
		current_level_ >= 0;
		current_level_--) {
		if (child_[current_level_] != 0) {
			return child_[current_level_];
		}
	}

	return 0;
}

Component* StackLayout::GetPrev() {
	for (--current_level_;
		current_level_ >= 0 && current_level_ < num_levels_;
		current_level_--) {
		if (child_[current_level_] != 0) {
			return child_[current_level_];
		}
	}

	return 0;
}

void StackLayout::UpdateLayout() {
	PixelCoord owner_size(GetOwner()->GetSize());

	for (int i = 0; i < num_levels_; i++) {
		if (child_[i] != 0) {
			child_[i]->SetPos(0, 0);
			child_[i]->SetSize(owner_size);
		}
	}
}

PixelCoord StackLayout::GetPreferredSize(bool force_adaptive) {
	PixelCoord size(0, 0);
	for (int i = 0; i < num_levels_; i++) {
		if (child_[i] != 0) {
			PixelCoord temp(child_[i]->GetPreferredSize(force_adaptive));
			if (temp.x > size.x) {
				size.x = temp.x;
			}
			if (temp.y > size.y) {
				size.y = temp.y;
			}
		}
	}

	return size;
}

PixelCoord StackLayout::GetMinSize() const {
	PixelCoord size(0, 0);
	for (int i = 0; i < num_levels_; i++) {
		if (child_[i] != 0) {
			PixelCoord temp(child_[i]->GetMinSize());
			if (temp.x > size.x) {
				size.x = temp.x;
			}
			if (temp.y > size.y) {
				size.y = temp.y;
			}
		}
	}

	return size;
}

PixelCoord StackLayout::GetContentSize() const {
	PixelCoord size(0, 0);
	for (int i = 0; i < num_levels_; i++) {
		if (child_[i] != 0) {
			PixelCoord temp(child_[i]->GetPreferredSize());
			if (temp.x > size.x) {
				size.x = temp.x;
			}
			if (temp.y > size.y) {
				size.y = temp.y;
			}
		}
	}

	return size;
}

}
