/*
	Class:  ListControl
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../Include/GUI/UiListControl.h"
#include "../../../UiLepra/Include/UiInput.h"

namespace UiTbc
{

ListControl::ListControl(unsigned pBorderStyle, int pBorderWidth, const Color& pColor, 
			 ListLayout::ListType pListType, const str& pName):
	Window(pBorderStyle, pBorderWidth, pColor, pName, new GridLayout(2, 2)),
	mStyle(SINGLE_SELECT),
	mHScrollBar(0),
	mVScrollBar(0),
	mListRect(0),
	mCornerRect(0),
	mLastSelected(0)
{
	Init(pListType);
}

ListControl::ListControl(unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID,
			 ListLayout::ListType pListType, const str& pName):
	Window(pBorderStyle, pBorderWidth, pImageID, pName, new GridLayout(2, 2)),
	mStyle(SINGLE_SELECT),
	mHScrollBar(0),
	mVScrollBar(0),
	mListRect(0),
	mCornerRect(0),
	mLastSelected(0)
{
	Init(pListType);
}

ListControl::ListControl(const Color& pColor, ListLayout::ListType pListType, const str& pName):
	Window(pColor, pName, new GridLayout(2, 2)),
	mStyle(SINGLE_SELECT),
	mHScrollBar(0),
	mVScrollBar(0),
	mListRect(0),
	mCornerRect(0),
	mLastSelected(0)
{
	Init(pListType);
}

ListControl::ListControl(Painter::ImageID pImageID, ListLayout::ListType pListType, const str& pName):
	Window(pImageID, pName, new GridLayout(2, 2)),
	mStyle(SINGLE_SELECT),
	mHScrollBar(0),
	mVScrollBar(0),
	mListRect(0),
	mCornerRect(0),
	mLastSelected(0)
{
	Init(pListType);
}

ListControl::~ListControl()
{
}

void ListControl::Init(ListLayout::ListType pListType)
{
	mHScrollBar = new ScrollBar(ScrollBar::HORIZONTAL);
	mVScrollBar = new ScrollBar(ScrollBar::VERTICAL);
	mListRect   = new RectComponent(_T("ListRect"), new ListLayout(pListType));
	mCornerRect = new RectComponent(Color(192, 192, 192), _T("CornerRect"));

	mHScrollBar->SetPreferredSize(0, 16, false);
	mVScrollBar->SetPreferredSize(16, 0, false);
	mCornerRect->SetPreferredSize(16, 16, false);
	mCornerRect->SetMinSize(16, 16);

	mHScrollBar->SetOwner(mListRect);
	mVScrollBar->SetOwner(mListRect);

	PixelCoord lVSBMinSize(mVScrollBar->GetMinSize());
	PixelCoord lHSBMinSize(mHScrollBar->GetMinSize());
	PixelCoord lCMinSize(mCornerRect->GetMinSize());
	PixelCoord lMinSize(lCMinSize.x + lHSBMinSize.x + GetTotalBorderWidth(),
				     lCMinSize.y + lVSBMinSize.y + GetTotalBorderWidth());
	SetMinSize(lMinSize);

	mHScrollBar->SetVisible(false);
	mVScrollBar->SetVisible(false);
	mCornerRect->SetVisible(false);

	Parent::AddChild(mListRect, 0, 0);
	Parent::AddChild(mHScrollBar, 1, 0);
	Parent::AddChild(mVScrollBar, 0, 1);
	Parent::AddChild(mCornerRect, 1, 1);
}

void ListControl::SetScrollBars(ScrollBar* pHScrollBar,
				ScrollBar* pVScrollBar,
				RectComponent* pCornerRect)
{
	if (pHScrollBar != 0)
	{
		Parent::RemoveChild(mHScrollBar, 0);
		delete mHScrollBar;
		mHScrollBar = pHScrollBar;
		mHScrollBar->SetOwner(mListRect);
		Parent::AddChild(mHScrollBar, 1, 0);
	}

	if (pVScrollBar != 0)
	{
		Parent::RemoveChild(mVScrollBar, 0);
		delete mVScrollBar;
		mVScrollBar = pVScrollBar;
		mVScrollBar->SetOwner(mListRect);
		Parent::AddChild(mVScrollBar, 0, 1);
	}

	if (pCornerRect != 0)
	{
		Parent::RemoveChild(mCornerRect, 0);
		delete mCornerRect;
		mCornerRect = pCornerRect;
		Parent::AddChild(mCornerRect, 1, 1);

		mCornerRect->SetMinSize(mCornerRect->GetPreferredSize());
	}

	PixelCoord lVSBMinSize(mVScrollBar->GetMinSize());
	PixelCoord lHSBMinSize(mHScrollBar->GetMinSize());
	PixelCoord lCMinSize(mCornerRect->GetMinSize());
	PixelCoord lMinSize(lCMinSize.x + lVSBMinSize.x, 
				     lCMinSize.y + lHSBMinSize.y);
	SetMinSize(lMinSize);

}

void ListControl::AddChild(Component* pChild, int pParam1, int pParam2)
{
	mListRect->AddChild(pChild, pParam1, pParam2);

	if (mLastSelected == 0)
	{
		mLastSelected = pChild;
	}
}

void ListControl::AddChildAfter(Component* pChild, Component* pAfterThis, int pIndentationLevel)
{
	((ListLayout*)mListRect->GetLayout())->AddChildAfter(pChild, pAfterThis, pIndentationLevel);

	pChild->SetParent(mListRect);

	if (mLastSelected == 0)
	{
		mLastSelected = pChild;
	}
}

void ListControl::AddChildrenAfter(std::list<Component*>& pChildList, Component* pAfterThis, int pIndentationLevel)
{
	((ListLayout*)mListRect->GetLayout())->AddChildrenAfter(pChildList, pAfterThis, pIndentationLevel);

	std::list<Component*>::iterator lIter;
	for (lIter = pChildList.begin(); lIter != pChildList.end(); ++lIter)
	{
		(*lIter)->SetParent(mListRect);
	}

	if (mLastSelected == 0 && pChildList.empty() == false)
	{
		mLastSelected = *(--pChildList.end());
	}

	SetNeedsRepaint(true);
}

void ListControl::RemoveChild(Component* pChild, int pLayer)
{
	mListRect->RemoveChild(pChild, pLayer);
}

Component* ListControl::GetChild(const str& pName, int pLayer)
{
	return mListRect->GetChild(pName, pLayer);
}

int ListControl::GetNumChildren() const
{
	return mListRect->GetNumChildren();
}

Component* ListControl::GetFirstSelectedItem()
{
	Layout* lLayout = mListRect->GetLayout();
	Component* lItem = lLayout->GetFirst();

	while (lItem != 0 && lItem->GetSelected() == false)
	{
		lItem = lLayout->GetNext();
	}

	return lItem;
}

Component* ListControl::GetNextSelectedItem()
{
	Layout* lLayout = mListRect->GetLayout();
	Component* lItem = lLayout->GetNext();

	while (lItem != 0 && lItem->GetSelected() == false)
	{
		lItem = lLayout->GetNext();
	}

	return lItem;
}

bool ListControl::OnLButtonDown(int pMouseX, int pMouseY)
{
	if (mListRect->IsOver(pMouseX, pMouseY) == true)
	{
		ListLayout* lLayout = (ListLayout*)mListRect->GetLayout();

		if (lLayout->IsEmpty() == true)
		{
			return true;
		}

		Select(lLayout->Find(lLayout->GetListType() == ListLayout::COLUMN ? pMouseY : pMouseX), pMouseX, pMouseY);
	}

	return Parent::OnLButtonDown(pMouseX, pMouseY);
}

bool ListControl::OnLButtonUp(int pMouseX, int pMouseY)
{
	return Parent::OnLButtonUp(pMouseX, pMouseY);
}

bool ListControl::OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY)
{
	if (mStyle == MENU_SELECT)
	{
		DeselectAll();
		ListLayout* lLayout = (ListLayout*)mListRect->GetLayout();
		SetSelected(lLayout->Find(pMouseY), true);
	}
	return (Parent::OnMouseMove(pMouseX, pMouseY, pDeltaX, pDeltaY));
}

bool ListControl::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	Parent::OnKeyDown(pKeyCode);

	ListLayout* lLayout = (ListLayout*)mListRect->GetLayout();

	Component* lChildToSelect = 0;

	if (lLayout->GetListType() == ListLayout::COLUMN)
	{
		if (pKeyCode == UiLepra::InputManager::IN_KBD_DOWN)
		{
			lChildToSelect = lLayout->GetNext(mLastSelected);
		}
		if (pKeyCode == UiLepra::InputManager::IN_KBD_UP)
		{
			lChildToSelect = lLayout->GetPrev(mLastSelected);
		}
	}
	else //if (lLayout->GetListType() == ListLayout::ROW)
	{
		if (pKeyCode == UiLepra::InputManager::IN_KBD_RIGHT)
		{
			lChildToSelect = lLayout->GetNext(mLastSelected);
		}
		if (pKeyCode == UiLepra::InputManager::IN_KBD_LEFT)
		{
			lChildToSelect = lLayout->GetPrev(mLastSelected);
		}
	}

	if (lChildToSelect != 0)
	{
		PixelRect lRect(lChildToSelect->GetScreenRect());
		Select(lChildToSelect, lRect.GetCenterX(), lRect.GetCenterY());
	}
	return (false);
}

bool ListControl::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	return (Parent::OnKeyUp(pKeyCode));
}

void ListControl::OnItemSelected(Component* /*pItem*/)
{
	// This is a dummy implementation, do nothing.
}

void ListControl::Select(Component* pChild, int pSelectedX, int pSelectedY)
{
	if (pChild != 0)
	{
		ListLayout* lLayout = (ListLayout*)mListRect->GetLayout();

		if (mStyle == MULTI_SELECT)
		{
			const DesktopWindow* lDesktopWindow = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
			UiLepra::InputManager* lInputManager = lDesktopWindow->GetInputManager();
			bool lCTRL  = lInputManager->ReadKey(UiLepra::InputManager::IN_KBD_LCTRL) || lInputManager->ReadKey(UiLepra::InputManager::IN_KBD_RCTRL);
			bool lShift = lInputManager->ReadKey(UiLepra::InputManager::IN_KBD_LSHIFT) || lInputManager->ReadKey(UiLepra::InputManager::IN_KBD_RSHIFT);

			if (lCTRL == false && lShift == false)
			{
				DeselectAll();
			}

			if (lShift == true)
			{
				if (mLastSelected == 0)
				{
					mLastSelected = lLayout->GetFirst();
				}

				ListLayout::ComponentList lList;
				PixelRect lRect(mLastSelected->GetScreenRect());

				if (lLayout->GetListType() == ListLayout::COLUMN)
				{
					lLayout->Find(lList, lRect.GetCenterY(), pSelectedY);
				}
				else
				{
					lLayout->Find(lList, lRect.GetCenterX(), pSelectedX);
				}

				ListLayout::ComponentList::iterator lIter;

				if (pChild->GetSelected() == false)
				{
					for (lIter = lList.begin(); lIter != lList.end(); ++lIter)
					{
						SetSelected(*lIter, true);
					}
				}
				else
				{
					for (lIter = lList.begin(); lIter != lList.end(); ++lIter)
					{
						SetSelected(*lIter, false);
					}
				}
			}
			else
			{
				if (pChild->GetSelected() == true)
				{
					SetSelected(pChild, false);
				}
				else
				{
					SetSelected(pChild, true);
					OnItemSelected(pChild);
				}
			}
		}
		else
		{
			DeselectAll();
			SetSelected(pChild, true);
			OnItemSelected(pChild);
		}

		mLastSelected = pChild;
		pChild->SetKeyboardFocus();

		ScrollToChild(mLastSelected);
	}
}

void ListControl::SetSelected(Component* pChild, bool pSelected)
{
	if (pSelected == true)
	{
		if (pChild != 0 && pChild->GetSelected() == false)
		{
			pChild->SetSelected(true);
			mSelectedList.push_back(pChild);
		}
	}
	else
	{
		if (pChild != 0 && pChild->GetSelected() == true)
		{
			pChild->SetSelected(false);
			mSelectedList.remove(pChild);
		}
	}
}

void ListControl::SetItemSelected(int pItemIndex, bool pSelected)
{
	if (pItemIndex >= 0 && pItemIndex < GetNumChildren())
	{
		Component* lChild = ((ListLayout*)mListRect->GetLayout())->FindIndex(pItemIndex);
		SetSelected(lChild, pSelected);
	}
}


void ListControl::DeselectAll()
{
	ComponentList::iterator lIter;
	
	for (lIter = mSelectedList.begin(); lIter != mSelectedList.end(); ++lIter)
	{
		(*lIter)->SetSelected(false);
	}

	mSelectedList.clear();
}

void ListControl::UpdateScrollPos()
{
	int lHOffset;
	int lVOffset;

	GetScrollOffsets(lHOffset, lVOffset);

	ListLayout* lLayout = (ListLayout*)mListRect->GetLayout();
	lLayout->SetPosOffset(lHOffset, lVOffset);

	SetNeedsRepaint(true);
}

void ListControl::GetScrollOffsets(int& pHorizontalOffset, int& pVerticalOffset) const
{
	ListLayout* lLayout = (ListLayout*)mListRect->GetLayout();
	PixelCoord lSizeDiff = mListRect->GetSize() - lLayout->GetContentSize();

	pHorizontalOffset = (int)(mHScrollBar->GetScrollPos() * lSizeDiff.x);
	pVerticalOffset = (int)(mVScrollBar->GetScrollPos() * lSizeDiff.y);

	if (pHorizontalOffset > 0)
	{
		pHorizontalOffset = 0;
	}

	if (pVerticalOffset > 0)
	{
		pVerticalOffset = 0;
	}
}

void ListControl::SetScrollOffsets(int pHorizontalOffset, int pVerticalOffset)
{
	ListLayout* lLayout = (ListLayout*)mListRect->GetLayout();
	PixelCoord lSizeDiff = mListRect->GetSize() - lLayout->GetContentSize();

	mHScrollBar->SetScrollPos((float64)pHorizontalOffset / (float64)lSizeDiff.x);
	mVScrollBar->SetScrollPos((float64)pVerticalOffset / (float64)lSizeDiff.y);

	lLayout->SetPosOffset(pHorizontalOffset, pVerticalOffset);
}

void ListControl::ScrollToChild(Component* pChild)
{
	if (pChild != 0)
	{
		ListLayout* lLayout = (ListLayout*)mListRect->GetLayout();

		PixelRect lRect(pChild->GetScreenRect());
		PixelRect lClientRect(mListRect->GetScreenRect());
		PixelCoord lSizeDiff = lLayout->GetContentSize() - 
			PixelCoord(lClientRect.GetWidth(), lClientRect.GetHeight());

		if (lLayout->GetListType() == ListLayout::COLUMN)
		{
			// Check if we need to scroll down.
			if (lRect.mBottom > lClientRect.mBottom)
			{
				float64 lDY = (float64)(lRect.mBottom - lClientRect.mBottom) / (float64)lSizeDiff.y;
				mVScrollBar->SetScrollPos(mVScrollBar->GetScrollPos() + lDY);
			}

			// Check if we need to scroll up.
			if (lRect.mTop < lClientRect.mTop)
			{
				float64 lDY = (float64)(lRect.mTop - lClientRect.mTop) / (float64)lSizeDiff.y;
				mVScrollBar->SetScrollPos(mVScrollBar->GetScrollPos() + lDY);
			}
		}
		else
		{
			// Check if we need to scroll right.
			if (lRect.mRight > lClientRect.mRight)
			{
				float64 lDX = (float64)(lRect.mRight - lClientRect.mRight) / (float64)lSizeDiff.x;
				mHScrollBar->SetScrollPos(mHScrollBar->GetScrollPos() + lDX);
			}

			// Check if we need to scroll up.
			if (lRect.mLeft < lClientRect.mLeft)
			{
				float64 lDX = (float64)(lRect.mLeft - lClientRect.mLeft) / (float64)lSizeDiff.x;
				mHScrollBar->SetScrollPos(mHScrollBar->GetScrollPos() + lDX);
			}
		}
	}
}

void ListControl::DoSetMinSize(int pWidth, int pHeight)
{
	Parent::DoSetMinSize(pWidth, pHeight);
}

void ListControl::UpdateLayout()
{
	Parent::UpdateLayout();

	float64 lAverage = ((ListLayout*)mListRect->GetLayout())->GetAverageComponentHW();

	bool lChanged;

	do
	{
		lChanged = false;

		PixelCoord lContentSize(mListRect->GetLayout()->GetContentSize());
		PixelCoord lSize(mListRect->GetSize());
		
		mHScrollBar->SetScrollRatio((float64)lSize.x / lAverage, (float64)lContentSize.x / lAverage);
		mVScrollBar->SetScrollRatio((float64)lSize.y / lAverage, (float64)lContentSize.y / lAverage);

		if (lContentSize.x > lSize.x)
		{
			lChanged = lChanged || !mHScrollBar->IsVisible();
			mHScrollBar->SetVisible(true);
		}
		else
		{
			lChanged = lChanged || mHScrollBar->IsVisible();
			mHScrollBar->SetVisible(false);
			mHScrollBar->SetScrollPos(0);
		}

		if (lContentSize.y > lSize.y)
		{
			lChanged = lChanged || !mVScrollBar->IsVisible();
			mVScrollBar->SetVisible(true);
		}
		else
		{
			lChanged = lChanged || mVScrollBar->IsVisible();
			mVScrollBar->SetVisible(false);
			mVScrollBar->SetScrollPos(0);
		}

		if (mVScrollBar->IsVisible() == true &&
		   mHScrollBar->IsVisible() == true)
		{
			mCornerRect->SetVisible(true);
		}
		else
		{
			mCornerRect->SetVisible(false);
		}

		// Update the layout one more time.
		Parent::UpdateLayout();
	}
	while(lChanged == true);

	UpdateScrollPos();
	Parent::UpdateLayout();
}



}
