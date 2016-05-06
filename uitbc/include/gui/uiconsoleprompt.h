
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/consolecommandmanager.h"
#include "../../../lepra/include/thread.h"
#include "uicomponent.h"



namespace uitbc {



class TextField;



class ConsolePrompt: public lepra::ConsolePrompt, public uilepra::TextInputObserver, public uilepra::KeyCodeInputObserver {
public:
	ConsolePrompt();
	virtual ~ConsolePrompt();

	void SetInputComponent(TextField* input_component);

	bool OnChar(wchar_t c);
	bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
	void SetFocus(bool focus);

protected:
	int WaitChar();
	void ReleaseWaitCharThread();
	void Backspace(size_t count);
	void EraseText(size_t count);
	void PrintPrompt(const str& prompt, const str& input_text, size_t edit_index);

	TextField* parent_;
	bool is_ctrl_pressed_;
	bool is_alt_pressed_;
	bool is_shift_pressed_;
	Lock lock_;
	Semaphore semaphore_;
	std::list<int> buffered_chars_;
};



}
