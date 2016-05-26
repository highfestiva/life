
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uiconsoleprompt.h"
#include "../../include/gui/uitextfield.h"



namespace uitbc {



ConsolePrompt::ConsolePrompt():
	parent_(0),
	is_ctrl_pressed_(false),
	is_alt_pressed_(false),
	is_shift_pressed_(false) {
}

ConsolePrompt::~ConsolePrompt() {
	SetInputComponent(0);
	semaphore_.Signal();
}



void ConsolePrompt::SetInputComponent(TextField* input_component) {
	if (parent_) {
		parent_->RemoveTextListener(this);
		parent_->RemoveKeyListener(this);
	}
	parent_ = input_component;
	if (parent_) {
		parent_->AddTextListener(this);
		parent_->AddKeyListener(this);

		parent_->SetIsReadOnly(true);
	}
}



bool ConsolePrompt::OnChar(wchar_t c) {
	{
		ScopeLock lock(&lock_);
		wchar_t _c = c;
		if (_c == 27) {
			_c = kConKeyEscape;
		}
		buffered_chars_.push_back(_c);
	}
	semaphore_.Signal();
	return (false);
}

bool ConsolePrompt::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	if (key_code == uilepra::InputManager::kInKbdLCtrl || key_code == uilepra::InputManager::kInKbdRCtrl) {
		is_ctrl_pressed_ = true;
	} else if (key_code == uilepra::InputManager::kInKbdLAlt || key_code == uilepra::InputManager::kInKbdRAlt) {
		is_alt_pressed_ = true;
	} else if (key_code == uilepra::InputManager::kInKbdLShift || key_code == uilepra::InputManager::kInKbdRShift) {
		is_shift_pressed_ = true;
	}

	{
		ScopeLock lock(&lock_);
		switch(key_code) {
			case uilepra::InputManager::kInKbdLeft: {
				if (is_ctrl_pressed_) {
					buffered_chars_.push_back(kConKeyCtrlLeft);
				} else {
					buffered_chars_.push_back(kConKeyLeft);
				}
			} break;
			case uilepra::InputManager::kInKbdRight: {
				if (is_ctrl_pressed_) {
					buffered_chars_.push_back(kConKeyCtrlRight);
				} else {
					buffered_chars_.push_back(kConKeyRight);
				}
			} break;
			case uilepra::InputManager::kInKbdHome:		buffered_chars_.push_back(kConKeyHome);		break;
			case uilepra::InputManager::kInKbdEnd:		buffered_chars_.push_back(kConKeyEnd);		break;
			case uilepra::InputManager::kInKbdDown:		buffered_chars_.push_back(kConKeyDown);		break;
			case uilepra::InputManager::kInKbdUp:		buffered_chars_.push_back(kConKeyUp);		break;
			case uilepra::InputManager::kInKbdDel:		buffered_chars_.push_back(kConKeyDelete);	break;
			case uilepra::InputManager::kInKbdPgUp:		buffered_chars_.push_back(kConKeyPageUp);	break;
			case uilepra::InputManager::kInKbdPgDown:	buffered_chars_.push_back(kConKeyPageDown);	break;
			default:					return (false);	// TRICKY: RAII!
		}
	}
	semaphore_.Signal();
	return (false);
}

bool ConsolePrompt::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	if (key_code == uilepra::InputManager::kInKbdLCtrl || key_code == uilepra::InputManager::kInKbdRCtrl) {
		is_ctrl_pressed_ = false;
	} else if (key_code == uilepra::InputManager::kInKbdLAlt || key_code == uilepra::InputManager::kInKbdRAlt) {
		is_alt_pressed_ = false;
	} else if (key_code == uilepra::InputManager::kInKbdLShift || key_code == uilepra::InputManager::kInKbdRShift) {
		is_shift_pressed_ = false;
	}
	return (false);
}

void ConsolePrompt::SetFocus(bool focus) {
	if (parent_) {
		if (focus) {
			parent_->SetKeyboardFocus();
		} else {
			parent_->ReleaseKeyboardFocus();
		}
	}
}

int ConsolePrompt::WaitChar() {
	semaphore_.Wait();
	int _c = -100;
	{
		ScopeLock lock(&lock_);
		if (!buffered_chars_.empty()) {
			_c = buffered_chars_.front();
			buffered_chars_.pop_front();
		}
	}
	return (_c);
}

void ConsolePrompt::ReleaseWaitCharThread() {
	semaphore_.Signal();
}

void ConsolePrompt::Backspace(size_t) {
	// Handled by PrintPrompt() in this implementation.
}

void ConsolePrompt::EraseText(size_t) {
	// Handled by PrintPrompt() in this implementation.
}

void ConsolePrompt::PrintPrompt(const str& prompt, const str& input_text, size_t edit_index) {
	if (parent_) {
		parent_->SetText(wstrutil::Encode(prompt+input_text));
		parent_->SetMarkerPosition(prompt.length()+edit_index);
	}
}



}
