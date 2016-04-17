/*
	Class:  PopupList
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This component extends the ListControl with the functionality required to work
	as a popup menu.
*/

#pragma once

#include "UiListControl.h"
#include "../../../UiLepra/Include/UiInput.h"

namespace UiTbc
{

class PopupList: public ListControl
{
	typedef ListControl Parent;
public:
	class Listener
	{
	public:
		// Both functions returns false if the list control has been deleted.
		virtual bool NotifySetKeyboardFocus(PopupList* pList) = 0;
		virtual bool NotifyReleaseKeyboardFocus(PopupList* pList, Component* pFocusedComponent) = 0;
		virtual bool NotifyKeyDown(PopupList* pList, UiLepra::InputManager::KeyCode pKeyCode) = 0;
		virtual bool NotifyLButtonDown(PopupList* pList, int pMouseX, int pMouseY) = 0;
		virtual bool NotifyDoubleClick(PopupList* pList, int pMouseX, int pMouseY) = 0;
	};

	PopupList(unsigned pBorderStyle,
		  int pBorderWidth,
		  const Color& pColor,
		  ListLayout::ListType pListType = ListLayout::COLUMN);
	PopupList(unsigned pBorderStyle,
		  int pBorderWidth,
		  Painter::ImageID pImageID,
		  ListLayout::ListType pListType = ListLayout::COLUMN);
	PopupList(const Color& pColor, ListLayout::ListType pListType = ListLayout::COLUMN);
	PopupList(Painter::ImageID pImageID, ListLayout::ListType pListType = ListLayout::COLUMN);

	virtual ~PopupList();

	void AddListener(Listener* pListener);
	void RemoveListener(Listener* pListener);

	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(RecurseDir pDir = RECURSE_UP, Component* pFocusedComponent = 0);

	bool OnLButtonDown(int pMouseX, int pMouseY);
	bool OnDoubleClick(int pMouseX, int pMouseY);

	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);

protected:
private:
	typedef std::list<Listener*> ListenerList;

	ListenerList mListeners;
};

}
