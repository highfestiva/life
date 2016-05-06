/*
	Class:  CenterLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uicenterlayout.h"
#include "../../include/gui/uicomponent.h"

namespace uitbc {

CenterLayout::CenterLayout() :
	child_(0) {
}

CenterLayout::~CenterLayout() {
}

Layout::Type CenterLayout::GetType() const {
	return Layout::kCenterlayout;
}

void CenterLayout::Add(Component* component, int /*param1*/, int /*param2*/) {
	if (child_ == 0) {
		child_ = component;
	} else {
		deb_assert(false);
	}
}

void CenterLayout::Remove(Component* component) {
	if (child_ == component) {
		child_ = 0;
	}
}

int CenterLayout::GetNumComponents() const {
	return (child_ == 0) ? 0 : 1;
}

Component* CenterLayout::GetFirst() {
	return child_;
}

Component* CenterLayout::GetNext() {
	return 0;
}

Component* CenterLayout::GetLast() {
	return child_;
}

Component* CenterLayout::GetPrev() {
	return 0;
}

void CenterLayout::UpdateLayout() {
	if (child_ != 0) {
		PixelCoord owner_size(GetOwner()->GetSize());
		PixelCoord child_size(child_->GetPreferredSize());

		if (child_size.x > owner_size.x) {
			child_size.x = owner_size.x;
		}
		if (child_size.y > owner_size.y) {
			child_size.y = owner_size.y;
		}

		child_->SetPos((owner_size.x - child_size.x) / 2,
						 (owner_size.y - child_size.y) / 2);
		child_->SetSize(child_size);
	}
}

PixelCoord CenterLayout::GetPreferredSize(bool force_adaptive) {
	if (child_ != 0) {
		return child_->GetPreferredSize(force_adaptive);
	} else {
		return PixelCoord(0, 0);
	}
}

PixelCoord CenterLayout::GetMinSize() const {
	if (child_ != 0) {
		return child_->GetMinSize();
	} else {
		return PixelCoord(0, 0);
	}
}

PixelCoord CenterLayout::GetContentSize() const {
	if (child_ != 0) {
		return child_->GetPreferredSize();
	} else {
		return PixelCoord(0, 0);
	}
}

}
