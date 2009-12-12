
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../../Lepra/Include/ConsoleCommandManager.h"
#include "../../../Lepra/Include/Thread.h"
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

protected:
	bool OnChar(Lepra::tchar pChar);
	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	void SetFocus(bool pFocus);
	int WaitChar();
	void ReleaseWaitCharThread();
	void Backspace(size_t pCount);
	void EraseText(size_t pCount);
	void PrintPrompt(const Lepra::String& pPrompt, const Lepra::String& pInputText, size_t pEditIndex);

	TextField* mParent;
	bool mIsCtrlPressed;
	bool mIsAltPressed;
	bool mIsShiftPressed;
	Lepra::Lock mLock;
	Lepra::Semaphore mSemaphore;
	std::list<int> mBufferedChars;
};



}
