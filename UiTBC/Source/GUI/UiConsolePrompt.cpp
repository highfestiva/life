
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Include/GUI/UiConsolePrompt.h"
#include "../../Include/GUI/UiTextField.h"



namespace UiTbc
{



ConsolePrompt::ConsolePrompt():
	mParent(0),
	mIsCtrlPressed(false),
	mIsAltPressed(false),
	mIsShiftPressed(false)
{
}

ConsolePrompt::~ConsolePrompt()
{
	SetInputComponent(0);
	mSemaphore.Signal();
}



void ConsolePrompt::SetInputComponent(TextField* pInputComponent)
{
	if (mParent)
	{
		mParent->RemoveTextListener(this);
		mParent->RemoveKeyListener(this);
	}
	mParent = pInputComponent;
	if (mParent)
	{
		mParent->AddTextListener(this);
		mParent->AddKeyListener(this);

		mParent->SetIsReadOnly(true);
	}
}



bool ConsolePrompt::OnChar(Lepra::tchar pChar)
{
	{
		Lepra::ScopeLock lLock(&mLock);
		int lChar = (int)(Lepra::utchar)pChar;
		if (lChar == 27)
		{
			lChar = CON_KEY_ESCAPE;
		}
		mBufferedChars.push_back(lChar);
	}
	mSemaphore.Signal();
	return (false);
}

bool ConsolePrompt::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	if (pKeyCode == UiLepra::InputManager::IN_KBD_LCTRL || pKeyCode == UiLepra::InputManager::IN_KBD_RCTRL)
	{
		mIsCtrlPressed = true;
	}
	else if (pKeyCode == UiLepra::InputManager::IN_KBD_LALT || pKeyCode == UiLepra::InputManager::IN_KBD_RALT)
	{
		mIsAltPressed = true;
	}
	else if (pKeyCode == UiLepra::InputManager::IN_KBD_LSHIFT || pKeyCode == UiLepra::InputManager::IN_KBD_RSHIFT)
	{
		mIsShiftPressed = true;
	}

	{
		Lepra::ScopeLock lLock(&mLock);
		switch(pKeyCode)
		{
			case UiLepra::InputManager::IN_KBD_LEFT:
			{
				if (mIsCtrlPressed)
				{
					mBufferedChars.push_back(CON_KEY_CTRL_LEFT);
				}
				else
				{
					mBufferedChars.push_back(CON_KEY_LEFT);
				}
			}
			break;
			case UiLepra::InputManager::IN_KBD_RIGHT:
			{
				if (mIsCtrlPressed)
				{
					mBufferedChars.push_back(CON_KEY_CTRL_RIGHT);
				}
				else
				{
					mBufferedChars.push_back(CON_KEY_RIGHT);
				}
			}
			break;
			case UiLepra::InputManager::IN_KBD_HOME:	mBufferedChars.push_back(CON_KEY_HOME);		break;
			case UiLepra::InputManager::IN_KBD_END:		mBufferedChars.push_back(CON_KEY_END);		break;
			case UiLepra::InputManager::IN_KBD_DOWN:	mBufferedChars.push_back(CON_KEY_DOWN);		break;
			case UiLepra::InputManager::IN_KBD_UP:		mBufferedChars.push_back(CON_KEY_UP);		break;
			case UiLepra::InputManager::IN_KBD_DEL:		mBufferedChars.push_back(CON_KEY_DELETE);	break;
			case UiLepra::InputManager::IN_KBD_PGUP:	mBufferedChars.push_back(CON_KEY_PAGE_UP);	break;
			case UiLepra::InputManager::IN_KBD_PGDOWN:	mBufferedChars.push_back(CON_KEY_PAGE_DOWN);	break;
			default:					return (false);	// TRICKY: RAII!
		}
	}
	mSemaphore.Signal();
	return (false);
}

bool ConsolePrompt::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	if (pKeyCode == UiLepra::InputManager::IN_KBD_LCTRL || pKeyCode == UiLepra::InputManager::IN_KBD_RCTRL)
	{
		mIsCtrlPressed = false;
	}
	else if (pKeyCode == UiLepra::InputManager::IN_KBD_LALT || pKeyCode == UiLepra::InputManager::IN_KBD_RALT)
	{
		mIsAltPressed = false;
	}
	else if (pKeyCode == UiLepra::InputManager::IN_KBD_LSHIFT || pKeyCode == UiLepra::InputManager::IN_KBD_RSHIFT)
	{
		mIsShiftPressed = false;
	}
	return (false);
}

void ConsolePrompt::SetFocus(bool pFocus)
{
	if (mParent)
	{
		if (pFocus)
		{
			mParent->SetKeyboardFocus();
		}
		else
		{
			mParent->ReleaseKeyboardFocus();
		}
	}
}

int ConsolePrompt::WaitChar()
{
	mSemaphore.Wait();
	int lChar = -100;
	{
		Lepra::ScopeLock lLock(&mLock);
		if (!mBufferedChars.empty())
		{
			lChar = mBufferedChars.back();
			mBufferedChars.pop_back();
		}
	}
	return (lChar);
}

void ConsolePrompt::ReleaseWaitCharThread()
{
	mSemaphore.Signal();
}

void ConsolePrompt::Backspace(size_t)
{
	// Handled by PrintPrompt() in this implementation.
}

void ConsolePrompt::EraseText(size_t)
{
	// Handled by PrintPrompt() in this implementation.
}

void ConsolePrompt::PrintPrompt(const Lepra::String& pPrompt, const Lepra::String& pInputText, size_t pEditIndex)
{
	if (mParent)
	{
		mParent->SetText(pPrompt+pInputText);
		mParent->SetMarkerPosition(pPrompt.length()+pEditIndex);
	}
}



}
