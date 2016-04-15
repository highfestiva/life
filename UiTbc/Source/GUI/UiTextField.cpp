
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/GUI/UiTextField.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../Include/GUI/UiPopupList.h"
#include "../../Include/GUI/UiFloatingLayout.h"
#include "../../../UiLepra/Include/UiInput.h"



namespace UiTbc
{



TextField::TextField(Component* pTopParent):
	Window(),
	mIsReadOnly(false),
	mPasswordCharacter(0),
	mTextX(0),
	mMarkerID(Painter::INVALID_IMAGEID),
	mMarkerPos(0),
	mMarkerVisible(false),
	mMarkerVisibleTime(0.5),
	mMarkerInvisibleTime(0.5),
	mUpdateMarkerPosOnNextRepaint(false),
	mClickX(0),
	mTopParent(pTopParent),
	mDesktopWindow(0),
	mListLayer(0),
	mListControl(0),
	mDeleteListControl(false)
{
	Init();
}

TextField::TextField(Component* pTopParent, unsigned pBorderStyle, int pBorderWidth, const Color& pColor):
	Window(pBorderStyle, pBorderWidth, pColor),
	mIsReadOnly(false),
	mPasswordCharacter(0),
	mTextX(0),
	mMarkerID(Painter::INVALID_IMAGEID),
	mMarkerPos(0),
	mMarkerVisible(false),
	mMarkerVisibleTime(0.5),
	mMarkerInvisibleTime(0.5),
	mUpdateMarkerPosOnNextRepaint(false),
	mClickX(0),
	mTopParent(pTopParent),
	mDesktopWindow(0),
	mListLayer(0),
	mListControl(0),
	mDeleteListControl(false)
{
	Init();
}

TextField::TextField(Component* pTopParent, unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID) :
	Window(pBorderStyle, pBorderWidth, pImageID),
	mIsReadOnly(false),
	mPasswordCharacter(0),
	mTextX(0),
	mMarkerID(Painter::INVALID_IMAGEID),
	mMarkerPos(0),
	mMarkerVisible(false),
	mMarkerVisibleTime(0.5),
	mMarkerInvisibleTime(0.5),
	mUpdateMarkerPosOnNextRepaint(false),
	mClickX(0),
	mTopParent(pTopParent),
	mDesktopWindow(0),
	mListLayer(0),
	mListControl(0),
	mDeleteListControl(false)
{
	Init();
}

TextField::TextField(Component* pTopParent, const Color& pColor) :
	Window(pColor),
	mIsReadOnly(false),
	mPasswordCharacter(0),
	mTextX(0),
	mMarkerID(Painter::INVALID_IMAGEID),
	mMarkerPos(0),
	mMarkerVisible(false),
	mMarkerVisibleTime(0.5),
	mMarkerInvisibleTime(0.5),
	mUpdateMarkerPosOnNextRepaint(false),
	mClickX(0),
	mTopParent(pTopParent),
	mDesktopWindow(0),
	mListLayer(0),
	mListControl(0),
	mDeleteListControl(false)
{
	Init();
}

TextField::TextField(Component* pTopParent, Painter::ImageID pImageID):
	Window(pImageID),
	mIsReadOnly(false),
	mPasswordCharacter(0),
	mTextX(0),
	mMarkerID(Painter::INVALID_IMAGEID),
	mMarkerPos(0),
	mMarkerVisible(false),
	mMarkerVisibleTime(0.5),
	mMarkerInvisibleTime(0.5),
	mUpdateMarkerPosOnNextRepaint(false),
	mClickX(0),
	mTopParent(pTopParent),
	mDesktopWindow(0),
	mListLayer(0),
	mListControl(0),
	mDeleteListControl(false)
{
	Init();
}

TextField::~TextField()
{
	if (mDesktopWindow != 0)
	{
		// We are still registered as a subscriber...
		ReleaseKeyboardFocus();
	}

	// The list control is deleted when the top parent is deleted.
}

Component* TextField::GetTopParent() const
{
	return mTopParent;
}

void TextField::SetTopParent(Component* pTopParent)
{
	mTopParent = pTopParent;
}

PopupList* TextField::CreatePopupList()
{
	return 0;
}

void TextField::SpawnPopupList()
{
	if (mListControl == 0)
	{
		if (mTopParent != 0)
		{
			mListControl = CreatePopupList();
		}
	
		if (mListControl != 0)
		{
			PixelRect lScreenRect(GetScreenRect());
			PixelRect lClientScreenRect(mTopParent->GetScreenRect());

			PixelRect lRect(mTopParent->ScreenToWindow(GetScreenRect()));

			mListControl->UpdateLayout();
			int lHeight = (int)std::min(mListControl->GetPreferredHeight(), std::min(mListControl->GetContentSize().y, lClientScreenRect.mBottom - lScreenRect.mBottom));

			mListControl->SetPreferredSize(GetSize().x, lHeight);

			if (mListLayer == 0)
			{
				mListLayer = mTopParent->CreateLayer(new FloatingLayout());
			}
			mTopParent->AddChild(mListControl, 0, 0, mListLayer);
			mTopParent->RequestRepaint();

			mListControl->SetPos(lRect.mLeft, lRect.mBottom);

			// Select the first item in the list.
			//mListControl->SetKeyboardFocus();
			mListControl->AddListener(this);
			mListControl->SetItemSelected(0, true);
			mListControl->RequestRepaint();
			Parent::SetKeyboardFocus();
		}
	}
}

void TextField::DestroyPopupList()
{
	// The list is destroyed with the layer.
	mTopParent->DeleteLayer(mListLayer);
	mTopParent->RequestRepaint();
	mListLayer = 0;
	mListControl = 0;
}

PopupList* TextField::GetPopupList() const
{
	return mListControl;
}

void TextField::SetIsReadOnly(bool pIsReadOnly)
{
	mIsReadOnly = pIsReadOnly;
}

void TextField::SetPasswordCharacter(char pCharacter)
{
	mPasswordCharacter = pCharacter;
}

wstr TextField::GetVisibleText() const
{
	wstr lText;
	if (mPasswordCharacter)
	{
		lText.assign(mText.length(), mPasswordCharacter);
	}
	else
	{
		lText = mText;
	}
	return (lText);
}

void TextField::SetText(const wstr& pText)
{
	mText = pText;
	mTextX = 0;
	SetMarkerPosition(pText.length());
	SetNeedsRepaint(true);
}

const wstr& TextField::GetText() const
{
	return mText;
}

void TextField::SetMarker(Painter::ImageID pImageID)
{
	mMarkerID = pImageID;
	SetNeedsRepaint(true);
}

void TextField::SetMarkerBlinkRate(float64 pVisibleTime, float64 pInvisibleTime)
{
	mMarkerVisibleTime = pVisibleTime;
	mMarkerInvisibleTime = pInvisibleTime;
}

void TextField::SetMarkerPosition(size_t pIndex)
{
	if (pIndex <= mText.size())
	{
		mMarkerPos = pIndex;
		SetNeedsRepaint(true);
	}
}

bool TextField::OnChar(wchar_t pChar)
{
	Parent::OnChar(pChar);
	if (mIsReadOnly)
	{
		return (false);	// RAII simplifies here.
	}

	bool lRegenerateList = false;

	if (pChar == '\b')
	{
		if (mMarkerPos > 0)
		{
			SetMarkerPosition(mMarkerPos-1);
			mText.erase(mMarkerPos, 1);
			SetNeedsRepaint(true);
			lRegenerateList = true;
		}
	}
	else if (pChar != '\r' && pChar != '\n' && pChar != 27)
	{
		mText.insert(mMarkerPos, 1, pChar);
		SetMarkerPosition(mMarkerPos+1);
		SetNeedsRepaint(true);
		lRegenerateList = true;
	}

	if (lRegenerateList == true && mListControl != 0)
	{
		DestroyPopupList();
		SpawnPopupList();
	}
	return (false);
}

bool TextField::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	Parent::OnKeyDown(pKeyCode);
	if (mIsReadOnly)
	{
		return (false);	// RAII simplifies here.
	}

	// TODO: Implement popup menu and marker control
	bool lResetMarker = false;

	const DesktopWindow* lDesktopWindow = (DesktopWindow*)GetParentOfType(Component::DESKTOPWINDOW);
	UiLepra::InputManager* lInputManager = lDesktopWindow->GetInputManager();

	bool lCtrlDown = lInputManager->ReadKey(UiLepra::InputManager::IN_KBD_LCTRL) ||
		lInputManager->ReadKey(UiLepra::InputManager::IN_KBD_RCTRL);
	const wstr lDelimitors = L" \t";


	switch(pKeyCode)
	{
		case UiLepra::InputManager::IN_KBD_LEFT:
		{
			if (mMarkerPos > 0)
			{
				if (lCtrlDown == true)
				{
					mMarkerPos = wstrutil::FindPreviousWord(GetVisibleText(), lDelimitors, mMarkerPos);
				}
				else
				{
					mMarkerPos--;
				}

				lResetMarker = true;
			}
		}
		break;
		case UiLepra::InputManager::IN_KBD_RIGHT:
		{
			if (mMarkerPos < mText.length())
			{
				if (lCtrlDown == true)
				{
					size_t lIndex = GetVisibleText().find_first_not_of(lDelimitors, mMarkerPos);
					lIndex = GetVisibleText().find_first_of(lDelimitors, lIndex);
					if (lIndex == wstr::npos)
					{
						// We have reached the end.
						mMarkerPos = mText.length();
					}
					else
					{
						mMarkerPos = lIndex;
					}
				}
				else
				{
					mMarkerPos++;
				}
				lResetMarker = true;
			}
		}
		break;
		case UiLepra::InputManager::IN_KBD_HOME:
		{
			mMarkerPos = 0;
			lResetMarker = true;
		}
		break;
		case UiLepra::InputManager::IN_KBD_END:
		{
			mMarkerPos = mText.length();
			lResetMarker = true;
		}
		break;
		case UiLepra::InputManager::IN_KBD_DOWN:
		{
			if (mListControl == 0)
			{
				SpawnPopupList();
			}
			else
			{
				mListControl->OnKeyDown(pKeyCode);
				Parent::SetKeyboardFocus();
			}
		}
		break;
		case UiLepra::InputManager::IN_KBD_UP:
		{
			if (mListControl != 0)
			{
				mListControl->OnKeyDown(pKeyCode);
				Parent::SetKeyboardFocus();
			}
		}
		break;
		case UiLepra::InputManager::IN_KBD_DEL:
		{
			if (mMarkerPos < mText.length())
			{
				mText.erase(mMarkerPos, 1);
				lResetMarker = true;
			}
		}
		 break;
		case UiLepra::InputManager::IN_KBD_ESC:
		{
			if (mListControl != 0)
			{
				DestroyPopupList();
				Parent::SetKeyboardFocus();
			}
		}
		break;
		default: break;
	}

	if (lResetMarker == true)
	{
		mMarkerTimer.ClearTimeDiff();
		mMarkerVisible = true;
		SetNeedsRepaint(true);
	}
	return (false);
}

void TextField::OnIdle()
{
	Parent::OnIdle();

	mMarkerTimer.UpdateTimer();

	bool lOldState = mMarkerVisible;
	bool lLoop = true;

	// Loop and consume the time difference in case the program has freezed
	// for a while.
	while (lLoop == true)
	{
		lLoop = false;

		if (mMarkerVisible == true && mMarkerTimer.GetTimeDiff() > mMarkerVisibleTime)
		{
			mMarkerVisible = false;
			mMarkerTimer.ReduceTimeDiff(mMarkerVisibleTime);
			lLoop = true;
		}

		if (mMarkerVisible == false && mMarkerTimer.GetTimeDiff() > mMarkerInvisibleTime)
		{
			mMarkerVisible = true;
			mMarkerTimer.ReduceTimeDiff(mMarkerInvisibleTime);
			lLoop = true;
		}
	}

	if (lOldState != mMarkerVisible)
	{
		SetNeedsRepaint(true);
	}
}

bool TextField::OnLButtonDown(int pMouseX, int pMouseY)
{
	SetKeyboardFocus();

	// The coordinate relative to the start of the text string's x-coordinate.
	mUpdateMarkerPosOnNextRepaint = true;
	mClickX = pMouseX;

	return Parent::OnLButtonDown(pMouseX, pMouseY);
}

bool TextField::OnLButtonUp(int pMouseX, int pMouseY)
{
	return Parent::OnLButtonUp(pMouseX, pMouseY);
}

bool TextField::OnMouseMove(int, int, int, int)
{
	// TODO: set mouse cursor: text.
	return true;
}

void TextField::SetKeyboardFocus()
{
	Parent::SetKeyboardFocus();
	SetupMarkerBlink();
}

void TextField::SetKeyboardFocus(Component* pChild)
{
	Parent::SetKeyboardFocus(pChild);
	SetupMarkerBlink();
}

void TextField::SetupMarkerBlink()
{
	DesktopWindow* lDesktopWin = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
	if (lDesktopWin != 0)
	{
		lDesktopWin->AddIdleSubscriber(this);
		lDesktopWin->ActivateKeyboard();
		mMarkerTimer.PopTimeDiff();
		mMarkerVisible = true;
		SetNeedsRepaint(true);

		// Store the pointer to the desktop window in case the
		// TextField is deleted before the keyboard focus is released.
		mDesktopWindow = lDesktopWin;
	}
}

void TextField::ReleaseKeyboardFocus(RecurseDir pDir, Component* pFocusedComponent)
{
	Parent::ReleaseKeyboardFocus(pDir, pFocusedComponent);

	if (mDesktopWindow != 0)
	{
		mDesktopWindow->RemoveIdleSubscriber(this);
		mDesktopWindow->DeactivateKeyboard();
		if (mMarkerVisible == true)
		{
			SetNeedsRepaint(true);
		}
		mMarkerVisible = false;

		mDesktopWindow = 0;
	}
}

void TextField::SetCaption(Caption*)
{
	// Do nothing. TextField can't have a caption.
}

void TextField::AddChild(Component*, int, int, int)
{
	// Do nothing. TextField can't have children.
}

void TextField::DoSetPos(int x, int y)
{
	Parent::DoSetPos(x, y);
	if (mListControl != 0)
	{
		PixelRect lRect(mTopParent->ScreenToWindow(GetScreenRect()));
		mListControl->SetPos(lRect.mLeft, lRect.mBottom);
	}
}

void TextField::DoSetSize(int pWidth, int pHeight)
{
	Parent::DoSetSize(pWidth, pHeight);
	if (mListControl != 0)
	{
		mListControl->SetPreferredWidth(pWidth);
	}
}

void TextField::Repaint(Painter* pPainter)
{
	ActivateFont(pPainter);

	Parent::Repaint(pPainter);

	if (mUpdateMarkerPosOnNextRepaint == true)
	{
		UpdateMarkerPos(pPainter);
		mUpdateMarkerPosOnNextRepaint = false;
	}

	int lTextHeight = pPainter->GetLineHeight();

	pPainter->PushAttrib(Painter::ATTR_ALL);

	const PixelRect lRect(GetClientRect());
#ifndef LEPRA_TOUCH
	pPainter->ReduceClippingRect(lRect);
#endif // !Touch

	int lMarkerX  = mTextX + mHorizontalMargin + pPainter->GetStringWidth(GetVisibleText().substr(0, mMarkerPos));
	int lMarkerWidth = 1;

	if (mMarkerID != Painter::INVALID_IMAGEID)
	{
		lMarkerWidth = GetImageManager()->GetImageSize(mMarkerID).x;
	}

	int lMaxX = lRect.GetWidth() - lMarkerWidth;
	if (lMarkerX > lMaxX)
	{
		mTextX += lMaxX - lMarkerX;
		lMarkerX = lMaxX;
	}
	if (lMarkerX < 0)
	{
		mTextX += -lMarkerX;
		lMarkerX = 0;
	}

	int lTextX = lRect.mLeft + mTextX + mHorizontalMargin;
	int lTextY = lRect.mTop + (lRect.GetHeight() - lTextHeight) / 2;

	PrintTextDeactivate(pPainter, GetVisibleText(), lTextX, lTextY);

	if (mMarkerVisible == true)
	{
		//pPainter->PushAttrib(Painter::ATTR_RENDERMODE);
		//pPainter->SetRenderMode(Painter::RM_XOR);
		if (mMarkerID != Painter::INVALID_IMAGEID)
		{
			GetImageManager()->DrawImage(mMarkerID, lRect.mLeft + lMarkerX, lRect.mTop);
		}
		else
		{
			const int lMarkerY = lRect.mTop + (lRect.GetHeight() - lTextHeight) / 2;
			pPainter->DrawLine(lRect.mLeft + lMarkerX, lMarkerY, lRect.mLeft + lMarkerX, lMarkerY + lTextHeight);
		}
		//pPainter->PopAttrib();
	}

	pPainter->PopAttrib();
}

Component::StateComponentList TextField::GetStateList(ComponentState pState)
{
	StateComponentList lList;
	if (pState == STATE_FOCUSABLE)
	{
		int lState = 0;
		if (IsDispatcher())
		{
			lState = 2;
		}
		else if (HasKeyboardFocus())
		{
			lState = 1;
		}
		lList.push_back(StateComponent(lState, (Component*)this));
	}
	return (lList);
}

void TextField::UpdateMarkerPos(Painter* pPainter)
{
	PixelRect lRect(GetClientRect());
	int lTextX = (mClickX - lRect.mLeft) - mTextX - mHorizontalMargin;

	// Search for the correct marker position using binary search.
	size_t lMin = 0;
	size_t lMax = mText.length();

	int lLeft = 0;
	int lRight = 0;

	while (lMax - lMin > 1)
	{
		size_t lMid = (lMin + lMax) / 2;
		int lStringWidth = pPainter->GetStringWidth(GetVisibleText().substr(0, lMid));

		if (lStringWidth < lTextX)
		{
			lMin = lMid;
			lLeft = lTextX - lStringWidth;
		}
		else if (lStringWidth >= lTextX)
		{
			lMax = lMid;
			lRight = lStringWidth - lTextX;
		}
	}

	if (lLeft < lRight)
	{
		mMarkerPos = lMin;
	}
	else
	{
		mMarkerPos = lMax;
	}

	mMarkerTimer.ClearTimeDiff();
	mMarkerVisible = true;
}

void TextField::SetMarkerPos(size_t pPos)
{
	if (pPos <= mText.length())
	{
		mMarkerPos = pPos;
		mMarkerTimer.ClearTimeDiff();
		mMarkerVisible = true;
		SetNeedsRepaint(true);
	}
}

void TextField::ForceRepaint()
{
	SetNeedsRepaint(true);
}

bool TextField::NotifySetKeyboardFocus(PopupList*)
{
	Parent::SetKeyboardFocus();
	return true;
}

bool TextField::NotifyReleaseKeyboardFocus(PopupList*, Component*)
{
	return true;
}

bool TextField::NotifyKeyDown(PopupList*, UiLepra::InputManager::KeyCode)
{
	return true;
}

bool TextField::NotifyLButtonDown(PopupList*, int, int)
{
	Parent::SetKeyboardFocus();
	return true;	
}

bool TextField::NotifyDoubleClick(PopupList*, int, int)
{
	// Let the subclass take care of this.
	return true;
}

} // End namespace.
