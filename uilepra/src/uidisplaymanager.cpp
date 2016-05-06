
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uidisplaymanager.h"
#include "../../lepra/include/lepraassert.h"



namespace uilepra {



DisplayManager::DisplayManager():
	screen_mode_(kWindowed),
	orientation_(kOrientationAllowAny),
	enumerated_display_mode_(0),
	enumerated_display_mode_count_(0) {
}

DisplayManager::~DisplayManager() {
	delete[] enumerated_display_mode_;
	enumerated_display_mode_ = 0;
	enumerated_display_mode_count_ = 0;

	RemoveResizeObserver(this);
	deb_assert(resize_observer_set_.empty());
}



void DisplayManager::SetOrientation(Orientation orientation) {
	orientation_ = orientation;
}

void DisplayManager::AddResizeObserver(DisplayResizeObserver* resize_observer) {
	resize_observer_set_.insert(resize_observer);
}

void DisplayManager::RemoveResizeObserver(DisplayResizeObserver* resize_observer) {
	resize_observer_set_.erase(resize_observer);
}

void DisplayManager::DispatchResize(int width, int height) {
	OnResize(width, height);

	ResizeObserverSet::iterator x = resize_observer_set_.begin();
	for (; x != resize_observer_set_.end(); ++x) {
		(*x)->OnResize(width, height);
	}
}

void DisplayManager::DispatchMinimize() {
	OnMinimize();

	ResizeObserverSet::iterator x = resize_observer_set_.begin();
	for (; x != resize_observer_set_.end(); ++x) {
		(*x)->OnMinimize();
	}
}

void DisplayManager::DispatchMaximize(int width, int height) {
	OnMaximize(width, height);

	ResizeObserverSet::iterator x = resize_observer_set_.begin();
	for (; x != resize_observer_set_.end(); ++x) {
		(*x)->OnMaximize(width, height);
	}
}



int DisplayManager::GetNumDisplayModes() {
	return enumerated_display_mode_count_;
}

int DisplayManager::GetNumDisplayModes(int bit_depth) {
	int count = 0;

	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].bit_depth_ == bit_depth) {
			count++;
		}
	}

	return count;
}

int DisplayManager::GetNumDisplayModes(int width, int height) {
	int count = 0;

	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].width_ == width &&
		   enumerated_display_mode_[i].height_ == height) {
			count++;
		}
	}

	return count;
}

int DisplayManager::GetNumDisplayModes(int width, int height, int bit_depth) {
	int count = 0;

	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].width_ == width &&
		   enumerated_display_mode_[i].height_ == height &&
		   enumerated_display_mode_[i].bit_depth_ == bit_depth) {
			count++;
		}
	}

	return count;
}

bool DisplayManager::GetDisplayMode(DisplayMode& display_mode, int mode) {
	bool ok = false;
	if (mode >= 0 && mode < enumerated_display_mode_count_) {
		display_mode.width_ = enumerated_display_mode_[mode].width_;
		display_mode.height_ = enumerated_display_mode_[mode].height_;
		display_mode.bit_depth_ = enumerated_display_mode_[mode].bit_depth_;
		display_mode.refresh_rate_ = enumerated_display_mode_[mode].refresh_rate_;

		ok = true;
	}
	return ok;
}

bool DisplayManager::GetDisplayMode(DisplayMode& display_mode, int mode, int bit_depth) {
	int count = 0;
	bool ok = false;
	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].bit_depth_ == bit_depth) {
			if (count == mode) {
				display_mode.width_ = enumerated_display_mode_[i].width_;
				display_mode.height_ = enumerated_display_mode_[i].height_;
				display_mode.bit_depth_ = enumerated_display_mode_[i].bit_depth_;
				display_mode.refresh_rate_ = enumerated_display_mode_[i].refresh_rate_;

				ok = true;
				break;
			}

			count++;
		}
	}
	return ok;
}

bool DisplayManager::GetDisplayMode(DisplayMode& display_mode, int mode, int width, int height) {
	int count = 0;
	bool ok = false;
	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].width_ == width &&
		   enumerated_display_mode_[i].height_ == height) {
			if (count == mode) {
				display_mode.width_ = enumerated_display_mode_[i].width_;
				display_mode.height_ = enumerated_display_mode_[i].height_;
				display_mode.bit_depth_ = enumerated_display_mode_[i].bit_depth_;
				display_mode.refresh_rate_ = enumerated_display_mode_[i].refresh_rate_;

				ok = true;
				break;
			}

			count++;
		}
	}
	return ok;
}

bool DisplayManager::GetDisplayMode(DisplayMode& display_mode, int mode, int width, int height, int bit_depth) {
	int count = 0;
	bool ok = false;
	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].width_ == width &&
		   enumerated_display_mode_[i].height_ == height &&
		   enumerated_display_mode_[i].bit_depth_ == bit_depth) {
			if (count == mode) {
				display_mode.width_ = enumerated_display_mode_[i].width_;
				display_mode.height_ = enumerated_display_mode_[i].height_;
				display_mode.bit_depth_ = enumerated_display_mode_[i].bit_depth_;
				display_mode.refresh_rate_ = enumerated_display_mode_[i].refresh_rate_;

				ok = true;
				break;
			}

			count++;
		}
	}
	return ok;
}

bool DisplayManager::FindDisplayMode(DisplayMode& display_mode, int width, int height) {
	bool mode_found = false;

	display_mode.width_ = 0;
	display_mode.height_ = 0;
	display_mode.bit_depth_ = 0;
	display_mode.refresh_rate_ = 0;

	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].width_ == width &&
		   enumerated_display_mode_[i].height_ == height) {
			if (enumerated_display_mode_[i].bit_depth_ == display_mode.bit_depth_ &&
			   enumerated_display_mode_[i].refresh_rate_ >= display_mode.refresh_rate_) {
				display_mode.width_ = enumerated_display_mode_[i].width_;
				display_mode.height_ = enumerated_display_mode_[i].height_;
				display_mode.bit_depth_ = enumerated_display_mode_[i].bit_depth_;
				display_mode.refresh_rate_ = enumerated_display_mode_[i].refresh_rate_;

				mode_found = true;
			} else if(enumerated_display_mode_[i].bit_depth_ > display_mode.bit_depth_) {
				display_mode.width_ = enumerated_display_mode_[i].width_;
				display_mode.height_ = enumerated_display_mode_[i].height_;
				display_mode.bit_depth_ = enumerated_display_mode_[i].bit_depth_;
				display_mode.refresh_rate_ = enumerated_display_mode_[i].refresh_rate_;

				mode_found = true;
			}
		}
	}

	return mode_found;
}

bool DisplayManager::FindDisplayMode(DisplayMode& display_mode, int width, int height, int bit_depth) {
	bool mode_found = false;

	display_mode.width_ = 0;
	display_mode.height_ = 0;
	display_mode.bit_depth_ = 0;
	display_mode.refresh_rate_ = 0;

	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].width_ == width &&
		   enumerated_display_mode_[i].height_ == height &&
		   enumerated_display_mode_[i].bit_depth_ == bit_depth) {
			if (enumerated_display_mode_[i].refresh_rate_ > display_mode.refresh_rate_) {
				display_mode.width_ = enumerated_display_mode_[i].width_;
				display_mode.height_ = enumerated_display_mode_[i].height_;
				display_mode.bit_depth_ = enumerated_display_mode_[i].bit_depth_;
				display_mode.refresh_rate_ = enumerated_display_mode_[i].refresh_rate_;

				mode_found = true;
			}
		}
	}

	return mode_found;
}

bool DisplayManager::FindDisplayMode(DisplayMode& display_mode, int width, int height, int bit_depth, int refresh_rate) {
	display_mode.width_ = 0;
	display_mode.height_ = 0;
	display_mode.bit_depth_ = 0;
	display_mode.refresh_rate_ = 0;

	bool ok = false;
	for (int i = 0; i < enumerated_display_mode_count_; i++) {
		if (enumerated_display_mode_[i].width_ == width &&
		   enumerated_display_mode_[i].height_ == height &&
		   enumerated_display_mode_[i].bit_depth_ == bit_depth &&
		   enumerated_display_mode_[i].refresh_rate_ == refresh_rate) {
			display_mode.width_ = enumerated_display_mode_[i].width_;
			display_mode.height_ = enumerated_display_mode_[i].height_;
			display_mode.bit_depth_ = enumerated_display_mode_[i].bit_depth_;
			display_mode.refresh_rate_ = enumerated_display_mode_[i].refresh_rate_;

			ok = true;
			break;
		}
	}

	return ok;
}



void DisplayManager::GetScreenCanvas(Canvas& canvas) {
	canvas.Reset(GetWidth(), GetHeight(), Canvas::IntToBitDepth(GetBitDepth()));
}

void DisplayManager::GetScreenCanvas(Canvas* canvas) {
	GetScreenCanvas(*canvas);
}



loginstance(kUiGfx, DisplayManager);



}
