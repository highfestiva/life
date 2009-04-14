/*
	Class:  PopupList
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/GUI/UiPopupList.h"



namespace UiTbc
{



PopupList::PopupList(unsigned pBorderStyle, int pBorderWidth, const Lepra::Color& pColor, 
			 ListLayout::ListType pListType, const Lepra::String& pName):
	ListControl(pBorderStyle, pBorderWidth, pColor, pListType, pName)
{
}

PopupList::PopupList(unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID,
			 ListLayout::ListType pListType, const Lepra::String& pName):
	ListControl(pBorderStyle, pBorderWidth, pImageID, pListType, pName)
{
}

PopupList::PopupList(const Lepra::Color& pColor, ListLayout::ListType pListType, const Lepra::String& pName):
	ListControl(pColor, pListType, pName)
{
}

PopupList::PopupList(Painter::ImageID pImageID, ListLayout::ListType pListType, const Lepra::String& pName):
	ListControl(pImageID, pListType, pName)
{
}

PopupList::~PopupList()
{
}

void PopupList::AddListener(Listener* pListener)
{
	mListeners.push_back(pListener);
	mListeners.unique();
}

void PopupList::RemoveListener(Listener* pListener)
{
	mListeners.remove(pListener);
}

void PopupList::SetKeyboardFocus()
{
	Parent::SetKeyboardFocus();
	// Notify all listeners.
	ListenerList::iterator lIter;
	for (lIter = mListeners.begin(); lIter != mListeners.end(); ++lIter)
	{
		if ((*lIter)->NotifySetKeyboardFocus(this) == false)
		{
			// TRICKY: The list control has been deleted and we must return immediately!
			return;
		}
	}
}

void PopupList::ReleaseKeyboardFocus(RecurseDir pDir, Component* pFocusedComponent)
{
	Parent::ReleaseKeyboardFocus(pDir, pFocusedComponent);
	// Notify all listeners.
	ListenerList::iterator lIter;
	for (lIter = mListeners.begin(); lIter != mListeners.end(); ++lIter)
	{
		if ((*lIter)->NotifyReleaseKeyboardFocus(this, pFocusedComponent) == false)
		{
			// TRICKY: The list control has been deleted and we must return immediately!
			return;
		}
	}
}

bool PopupList::OnLButtonDown(int pMouseX, int pMouseY)
{
	bool lReturnValue = Parent::OnLButtonDown(pMouseX, pMouseY);

	// Notify all listeners.
	ListenerList::iterator lIter;
	for (lIter = mListeners.begin(); lIter != mListeners.end(); ++lIter)
	{
		if ((*lIter)->NotifyLButtonDown(this, pMouseX, pMouseY) == false)
		{
			// TRICKY: The list control has been deleted and we must return immediately!
			return lReturnValue;
		}
	}

	return lReturnValue;
}

bool PopupList::OnDoubleClick(int pMouseX, int pMouseY)
{
	Parent::OnDoubleClick(pMouseX, pMouseY);

	// Notify all listeners.
	ListenerList::iterator lIter;
	for (lIter = mListeners.begin(); lIter != mListeners.end(); ++lIter)
	{
		if ((*lIter)->NotifyDoubleClick(this, pMouseX, pMouseY) == false)
		{
			// TRICKY: The list control has been deleted and we must return immediately!
			break;
		}
	}
	return (false);
}

bool PopupList::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	Parent::OnKeyDown(pKeyCode);

	// Notify all listeners.
	ListenerList::iterator lIter;
	for (lIter = mListeners.begin(); lIter != mListeners.end(); ++lIter)
	{
		if ((*lIter)->NotifyKeyDown(this, pKeyCode) == false)
		{
			// TRICKY: The list control has been deleted and we must return immediately!
			break;
		}
	}
	return (false);
}



}
