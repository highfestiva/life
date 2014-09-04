
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../Lepra/Include/ConsoleCommandManager.h"
#include "../../../Lepra/Include/Lock.h"
#include "UiComponent.h"



namespace UiTbc
{



class TextField;



class ConsolePrompt: public Lepra::ConsolePrompt, public UiLepra::TextInputObserver, public UiLepra::KeyCodeInputObserver
{
public:
	ConsolePrompt();
	virtual ~ConsolePrompt();

	void SetInputComponent(TextField* pInputComponent);

	bool OnChar(tchar pChar);
	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	void SetFocus(bool pFocus);

protected:
	int WaitChar();
	void ReleaseWaitCharThread();
	void Backspace(size_t pCount);
	void EraseText(size_t pCount);
	void PrintPrompt(const str& pPrompt, const str& pInputText, size_t pEditIndex);

	TextField* mParent;
	bool mIsCtrlPressed;
	bool mIsAltPressed;
	bool mIsShiftPressed;
	Lock mLock;
	Semaphore mSemaphore;
	std::list<int> mBufferedChars;
};



}
