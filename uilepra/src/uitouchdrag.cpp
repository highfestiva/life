
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uitouchdrag.h"
#include "../../lepra/include/thread.h"
#include "../include/uitouchstick.h"



namespace uilepra {
namespace touch {



Drag::Drag(int x, int y, bool is_press, int button_mask):
	start_(x, y),
	last_(x, y),
	is_press_(is_press),
	is_new_(true),
	button_mask_(button_mask),
	flags_(0),
	extra_(0) {
}

void Drag::Update(const PixelCoord& coord, bool is_press, int button_mask) {
	is_new_ = false;
	last_ = coord;
	is_press_ = is_press;
	button_mask_ = button_mask;
}

int Drag::GetDiamondDistanceTo(const PixelCoord& coord) const {
	return std::abs(last_.x-coord.x) + std::abs(last_.y-coord.y);
}



DragManager::DragManager():
	lock_(new Lock),
	mouse_last_pressed_(false),
	max_drag_diamond_distance_(106) {
}

DragManager::~DragManager() {
}

void DragManager::SetMaxDragDistance(int max_drag_distance) {
	max_drag_diamond_distance_ = (int)(max_drag_distance*1.2f);
}

void DragManager::UpdateDrag(const PixelCoord& previous, const PixelCoord& location, bool is_pressed, int button_mask) {
	ScopeLock lock(lock_);
	int closest_diamond_distance = 1000000;
	DragList::iterator i = drag_list_.begin();
	DragList::iterator best_drag = i;
	for (; i != drag_list_.end(); ++i) {
		int d = i->GetDiamondDistanceTo(previous);
		if (d < closest_diamond_distance) {
			closest_diamond_distance = d;
			best_drag = i;
		}
	}
	if (best_drag != drag_list_.end() && (!is_pressed || closest_diamond_distance <= max_drag_diamond_distance_)) {	// If releasing we must do this.
		best_drag->Update(location, is_pressed, button_mask);
		return;
	}
	drag_list_.push_back(Drag(location.x, location.y, is_pressed, button_mask));
}

void DragManager::UpdateDragByMouse(const InputManager* input_manager) {
	if (!input_manager->GetMouse()) {
		return;
	}

	ScopeLock lock(lock_);
	PixelCoord mouse;
	input_manager->GetMousePosition(mouse.x, mouse.y);
	int _button_mask = 0;
	const int c = input_manager->GetMouse()->GetNumDigitalElements();
	for (int x = 0; x < c; ++x) {
		_button_mask |= ((input_manager->GetMouse()->GetButton(x)->GetBooleanValue()? 1:0) << x);
	}
	bool _is_pressed = !!_button_mask;
	if (_is_pressed || mouse_last_pressed_) {
		UpdateDrag(last_mouse_, mouse, _is_pressed, _button_mask);
	}
	last_mouse_ = mouse;
	mouse_last_pressed_ = _is_pressed;
}

void DragManager::UpdateMouseByDrag(InputManager* input_manager) {
	ScopeLock lock(lock_);
	DragList::iterator i = drag_list_.begin();
	for (; i != drag_list_.end(); ++i) {
		input_manager->SetMousePosition(i->last_.x, i->last_.y);
		int _button_mask = i->button_mask_;
		const int c = input_manager->GetMouse()->GetNumDigitalElements();
		for (int x = 0; x < c; ++x) {
			input_manager->GetMouse()->GetButton(x)->SetValue((_button_mask&(1<<x))? 1 : 0);
		}
	}
}

bool DragManager::UpdateTouchsticks(InputManager* input_manager) const {
	ScopeLock lock(lock_);
	bool did_use_stick = false;
	DragList::const_iterator i = drag_list_.begin();
	for (; i != drag_list_.end(); ++i) {
		Drag& drag = (Drag&)*i;
		TouchstickInputDevice* touchstick = TouchstickInputDevice::GetByCoordinate(input_manager, drag.start_);
		if (!touchstick) {
			touchstick = TouchstickInputDevice::GetByCoordinate(input_manager, drag.last_);
		}
		if (touchstick) {
			touchstick->SetTap(drag.last_, drag.is_press_);
			drag.flags_ |= kDraggingStick;
			did_use_stick = true;
		} else {
			drag.flags_ &= ~kDraggingStick;
		}
	}
	return did_use_stick;
}

void DragManager::SetDraggingUi(bool is_ui) {
	ScopeLock lock(lock_);
	DragList::iterator i = drag_list_.begin();
	for (; i != drag_list_.end(); ++i) {
		if (is_ui) {
			i->flags_ |= kDraggingUi;
		} else {
			i->flags_ &= ~kDraggingUi;
		}
	}
}

void DragManager::DropReleasedDrags() {
	ScopeLock lock(lock_);
	DragList::iterator i = drag_list_.begin();
	for (; i != drag_list_.end();) {
		if (!i->is_press_) {
			i = drag_list_.erase(i);
		} else {
			++i;
		}
	}
}

void DragManager::ClearDrags(InputManager* input_manager) {
	ScopeLock lock(lock_);
	drag_list_.clear();
	if (input_manager->GetMouse()) {
		const int c = input_manager->GetMouse()->GetNumDigitalElements();
		for (int x = 0; x < c; ++x) {
			input_manager->GetMouse()->GetButton(x)->SetValue(0);
		}
	}
}

DragManager::DragList DragManager::GetDragList() {
	ScopeLock lock(lock_);
	return drag_list_;
}

void DragManager::SetDragList(const DragList& drag_list) {
	ScopeLock lock(lock_);
	drag_list_ = drag_list;
}



loginstance(kUiInput, DragManager);



}
}
