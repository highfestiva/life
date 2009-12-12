/*
	Class:  ListControl
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	The list control will store components in, and render them as, a list.
	The components should preferrably be Labels, but can be of any type.
	
	The list control assumes that each component has a static preferred size.
	If you change the size of one component, you have to rebuild the entire
	list. The list won't work properly otherwise.
*/

#pragma once

#include "UiWindow.h"
#include "UiGridLayout.h"
#include "UiListLayout.h"
#include "UiScrollBar.h"
#include "UiRectComponent.h"
#include "UiLabel.h"
#include <list>
#include "../../../Lepra/Include/HashTable.h"



namespace UiTbc
{



class ListControl: public Window
{
	typedef Window Parent;
public:

	enum Style
	{
		SINGLE_SELECT = 0,	// Select at most one (by clicking).
		MULTI_SELECT,		// Select zero or more.
		MENU_SELECT,	// Like single select, but selects by hovering with the cursor.
	};

	ListControl(unsigned pBorderStyle,
		    int pBorderWidth,
		    const Lepra::Color& pColor,
		    ListLayout::ListType pListType = ListLayout::COLUMN,
		    const Lepra::String& pName = _T("ListControl"));
	ListControl(unsigned pBorderStyle,
		    int pBorderWidth,
		    Painter::ImageID pImageID,
		    ListLayout::ListType pListType = ListLayout::COLUMN,
		    const Lepra::String& pName = _T("ListControl"));
	ListControl(const Lepra::Color& pColor,
		    ListLayout::ListType pListType = ListLayout::COLUMN,
		    const Lepra::String& pName = _T("ListControl"));
	ListControl(Painter::ImageID pImageID,
		    ListLayout::ListType pListType = ListLayout::COLUMN,
		    const Lepra::String& pName = _T("ListControl"));

	virtual ~ListControl();

	inline Style GetStyle();
	inline void SetStyle(Style pStyle);

	void SetScrollBars(ScrollBar* pHScrollBar, ScrollBar* pVScrollBar, RectComponent* pCornerRect);

	virtual void AddChild(Component* pChild, int pParam1 = 0, int pParam2 = 0);
	virtual void RemoveChild(Component* pChild, int pLayer);
	virtual Component* GetChild(const Lepra::String& pName, int pLayer);
	virtual int GetNumChildren() const;

	void AddChildAfter(Component* pChild, Component* pAfterThis, int pIndentationLevel);
	void AddChildrenAfter(std::list<Component*>& pChildList, Component* pAfterThis, int pIndentationLevel);

	void GetScrollOffsets(int& pHorizontalOffset, int& pVerticalOffset) const;
	void SetScrollOffsets(int pHorizontalOffset, int pVerticalOffset);

	void SetItemSelected(int pItemIndex, bool pSelected);

	Component* GetFirstSelectedItem();
	Component* GetNextSelectedItem();

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY);

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);

	// Override this if implementing a popup menu.
	virtual void OnItemSelected(Component* pItem);

	virtual void DoSetMinSize(int pWidth, int pHeight);

	virtual void UpdateLayout();

	inline virtual Type GetType();

	inline Lepra::PixelCoords GetContentSize();

protected:
	void Init(ListLayout::ListType pListType);
private:

	typedef std::list<Component*> ComponentList;

	void Select(Component* pChild, int pSelectedX, int pSelectedY);
	void SetSelected(Component* pChild, bool pSelected);
	void DeselectAll();

	void UpdateScrollPos();
	
	void ScrollToChild(Component* pChild);

	ComponentList mSelectedList;

	Style mStyle;

	ScrollBar* mHScrollBar;
	ScrollBar* mVScrollBar;

	RectComponent* mListRect;
	// The square in the bottom right corner where the scroll bars meet.
	RectComponent* mCornerRect;

	Component* mLastSelected;
};

ListControl::Style ListControl::GetStyle()
{
	return mStyle;
}

void ListControl::SetStyle(Style pStyle)
{
	mStyle = pStyle;
}

Component::Type ListControl::GetType()
{
	return Component::LISTCONTROL;
}

Lepra::PixelCoords ListControl::GetContentSize()
{
	return ((ListLayout*)mListRect->GetLayout())->GetContentSize();
}



}
