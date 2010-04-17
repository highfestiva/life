
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiButton.h"



namespace UiTbc
{



Button::Button(const str& pName) :
	Window(pName),
	mOnPress(0),
	mOnRelease(0),
	mOnClick(0),
	mOnDrag(0),
	mIconID(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_CENTER),
	mText(_T("")),
	mTextColor(OFF_BLACK),
	mTextBackgColor(255, 255, 255),
	mPressed(false),
	mState(RELEASED),
	mExtraData(0)
{
	InitializeHoover();
}

Button::Button(const Color& pColor, const str& pName):
	Window(pColor, pName),
	mOnPress(0),
	mOnRelease(0),
	mOnClick(0),
	mOnDrag(0),
	mIconID(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_CENTER),
	mText(_T("")),
	mTextColor(OFF_BLACK),
	mTextBackgColor(255, 255, 255),
	mPressed(false),
	mState(RELEASED),
	mExtraData(0)
{
	InitializeHoover();
}

Button::Button(BorderComponent::BorderShadeFunc pShadeFunc, int pBorderWidth, const Color& pColor,
	const str& pName):
	Window((pShadeFunc == BorderComponent::LINEAR ? Parent::BORDER_LINEARSHADING : 0), pBorderWidth, pColor, pName),
	mOnPress(0),
	mOnRelease(0),
	mOnClick(0),
	mOnDrag(0),
	mIconID(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_CENTER),
	mText(_T("")),
	mTextColor(OFF_BLACK),
	mTextBackgColor(255, 255, 255),
	mPressed(false),
	mState(RELEASED),
	mExtraData(0)
{
	InitializeHoover();
}

Button::Button(Painter::ImageID pReleasedImageID, Painter::ImageID pPressedImageID,
	Painter::ImageID pReleasedActiveImageID,	// Mouse over.
	Painter::ImageID pPressedActiveImageID, Painter::ImageID pReleasingImageID,
	Painter::ImageID pPressingImageID, const str& pName):
	Window(pReleasedImageID, pName),
	mOnPress(0),
	mOnRelease(0),
	mOnClick(0),
	mOnDrag(0),
	mReleasedImageID(pReleasedImageID),
	mPressedImageID(pPressedImageID),
	mReleasedActiveImageID(pReleasedActiveImageID),
	mPressedActiveImageID(pPressedActiveImageID),
	mReleasingImageID(pReleasingImageID),
	mPressingImageID(pPressingImageID),
	mIconID(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_CENTER),
	mText(_T("")),
	mTextColor(OFF_BLACK),
	mTextBackgColor(255, 255, 255),
	mPressed(false),
	mState(RELEASED),
	mExtraData(0)
{
	InitializeHoover();
}

Button::~Button()
{
	delete mOnPress;
	delete mOnRelease;
	delete mOnClick;
	delete mOnDrag;
}

void Button::InitializeHoover()
{
	mHooverColor = GetColor() * 1.2f;
	mPressColor = GetColor() * 0.95f;
	GetClientRectComponent()->SetIsHollow(false);
}

void Button::SetPressed(bool pPressed)
{
	mPressed = pPressed;
	SetState(mPressed? PRESSED : HasMouseFocus()? RELEASED : RELEASED_HOOVER);
}

void Button::SetState(State pState)
{
	bool lChangedState = false;
	if (mState != pState)
	{
		SetNeedsRepaint(true);
		lChangedState = true;
	}

	mState = pState;

	if (mImageButton == true)
	{
		switch(mState)
		{
		case RELEASED:
			Parent::SetBackgroundImage(mReleasedImageID);
			break;
		case RELEASED_HOOVER:
			Parent::SetBackgroundImage(mReleasedActiveImageID);
			break;
		case RELEASING:
			Parent::SetBackgroundImage(mReleasingImageID);
			break;
		case PRESSED:
			Parent::SetBackgroundImage(mPressedImageID);
			break;
		case PRESSED_HOOVER:
			Parent::SetBackgroundImage(mPressedActiveImageID);
			break;
		case PRESSING:
			Parent::SetBackgroundImage(mPressingImageID);
			break;
		}
	}
	else
	{
		if (mState == PRESSED || mState == PRESSING || mState == PRESSED_HOOVER)
		{
			unsigned lStyle = Parent::GetBorderStyle() | Parent::BORDER_SUNKEN;
			Parent::SetBorder(lStyle, Parent::GetBorderWidth());
			Parent::SetColor(mPressColor);
		}
		else if (mState == RELEASED_HOOVER)
		{
			Parent::SetColor(mHooverColor);
		}
		else
		{
			unsigned lStyle = (Parent::GetBorderStyle() & (~Parent::BORDER_SUNKEN));
			Parent::SetBorder(lStyle, Parent::GetBorderWidth());
			Parent::SetColor(GetColor());
		}
	}

	if (lChangedState == true)
	{
		switch(mState)
		{
		case RELEASED:
		case RELEASED_HOOVER:
			if (mOnRelease != 0)
			{
				(*mOnRelease)(this);
			}
			break;
		case RELEASING:
			break;
		case PRESSED:
		case PRESSED_HOOVER:
			break;
		case PRESSING:
			if (mOnPress != 0)
			{
				(*mOnPress)(this);
			}
			break;
		}
	}
}

Button::StateComponentList Button::GetStateList(ComponentState pState)
{
	StateComponentList lList;
	if (pState == STATE_CLICKABLE)
	{
		lList.push_back(StateComponent(0, this));
	}
	return (lList);
}

void Button::Repaint(Painter* pPainter)
{
	Parent::Repaint(pPainter);

	GUIImageManager* lIMan = GetImageManager();

	pPainter->PushAttrib(Painter::ATTR_ALL);
	PixelRect lRect(GetClientRect());
	pPainter->ReduceClippingRect(lRect);

	int lOffset = GetPressed() ? 1 : 0;
	int lTextX = lRect.mLeft;

	if (mIconID != Painter::INVALID_IMAGEID)
	{
		PixelCoord lImageSize(lIMan->GetImageSize(mIconID));

		switch(mIconAlignment)
		{
		case ICON_LEFT:
			lIMan->DrawImage(mIconID, lRect.mLeft + lOffset, lRect.mTop + (lRect.GetHeight() - lImageSize.y) / 2 + lOffset);
			lTextX = lRect.mLeft + lImageSize.x;
			break;
		case ICON_CENTER:
			lIMan->DrawImage(mIconID,
					   lRect.mLeft + (lRect.GetWidth()  - lImageSize.x) / 2 + lOffset,
					   lRect.mTop  + (lRect.GetHeight() - lImageSize.y) / 2 + lOffset);
			break;
		case ICON_RIGHT:
			lIMan->DrawImage(mIconID,
					   lRect.mRight - lImageSize.x + lOffset,
					   lRect.mTop + (lRect.GetHeight() - lImageSize.y) / 2 + lOffset);
			break;
		}
	}

	if (mIconID != Painter::INVALID_IMAGEID)
	{
		if (mIconAlignment != ICON_CENTER)
		{
			PrintText(pPainter, lTextX + lOffset, lRect.mTop + (lRect.GetHeight() - pPainter->GetFontHeight()) / 2);
		}
	}
	else
	{
		PrintText(pPainter, 
			  lRect.mLeft + (lRect.GetWidth() - pPainter->GetStringWidth(mText.c_str())) / 2 + lOffset, 
			  lRect.mTop + (lRect.GetHeight() - pPainter->GetFontHeight()) / 2 + lOffset);
	}

	pPainter->PopAttrib();
}

void Button::PrintText(Painter* pPainter, int x, int y)
{
	pPainter->SetColor(mTextColor, 0);
	pPainter->SetColor(mTextBackgColor, 1);
	pPainter->PrintText(mText.c_str(), x, y);
}

void Button::SetExtraData(void* pData)
{
	mExtraData = pData;
}

void* Button::GetExtraData() const
{
	return (mExtraData);
}

bool Button::OnLButtonDown(int pMouseX, int pMouseY)
{
	if (IsOver(pMouseX, pMouseY) == true)
	{
		switch(mState)
		{
		case RELEASED:
		case RELEASED_HOOVER:
			SetState(PRESSING);
			break;
		case PRESSED:
		case PRESSED_HOOVER:
			SetState(RELEASING);
			break;
		default: break;
		}

		SetMouseFocus();

		Click(false);
	}

	return true;
}

bool Button::OnLButtonUp(int pMouseX, int pMouseY)
{
	bool lCallFunctor = HasMouseFocus();

	SetPressed(false);
	ReleaseMouseFocus();

	if (IsOver(pMouseX, pMouseY) == true &&
		lCallFunctor == true && mOnClick != 0)
	{
		(*mOnClick)(this);
	}

	return true;
}

bool Button::OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY)
{
	if (HasMouseFocus() == true)
	{
		switch(mState)
		{
		case RELEASED:
		case RELEASED_HOOVER:
			if (IsOver(pMouseX, pMouseY) == true)
				SetState(PRESSING);
			else
				SetState(RELEASED);
			break;
		case RELEASING:
			if (IsOver(pMouseX, pMouseY) == false)
				SetState(PRESSED);
			break;
		case PRESSED:
		case PRESSED_HOOVER:
			if (IsOver(pMouseX, pMouseY) == true)
				SetState(RELEASING);
			else
				SetState(PRESSED);
			break;
		case PRESSING:
			if (IsOver(pMouseX, pMouseY) == false)
				SetState(RELEASED);
			break;
		}

		if (mOnDrag != 0)
		{
			(*mOnDrag)(this, pDeltaX, pDeltaY);
		}
	}
	else
	{
		switch(mState)
		{
		case RELEASED:
		case RELEASED_HOOVER:
		case RELEASING:
			if (IsOver(pMouseX, pMouseY) == true)
				SetState(RELEASED_HOOVER);
			else
				SetState(RELEASED);
			break;
		case PRESSED:
		case PRESSED_HOOVER:
		case PRESSING:
			if (IsOver(pMouseX, pMouseY) == true)
				SetState(PRESSED_HOOVER);
			else
				SetState(PRESSED);
			break;
		}
	}
	return Parent::OnMouseMove(pMouseX, pMouseY, pDeltaX, pDeltaY);
}

bool Button::Click(bool pDepress)
{
	bool lClicked = false;
	if (pDepress && mOnClick)
	{
		(*mOnClick)(this);
		lClicked = true;
	}
	return (lClicked);
}

bool Button::GetPressed()
{
	return mPressed;
}

Button::State Button::GetState()
{
	return mState;
}

void Button::SetIcon(Painter::ImageID pIconID,
		     IconAlignment pAlignment)
{
	mIconID        = pIconID;
	mIconAlignment = pAlignment;
	SetNeedsRepaint(true);
}

void Button::SetText(const str& pText, 
		     const Color& pTextColor,
		     const Color& pBackgColor)
{
	assert(mTextColor != BLACK);
	mText           = pText;
	mTextColor      = pTextColor;
	mTextBackgColor = pBackgColor;

	OnTextChanged();
}

const str& Button::GetText()
{
	return mText;
}

Button::Type Button::GetType() const
{
	return BUTTON;
}

void Button::OnTextChanged()
{
}

void Button::SetOnPressDelegate(const Delegate& pOnPress)
{
	delete mOnPress;
	mOnPress = new Delegate(pOnPress);
}

void Button::SetOnReleaseDelegate(const Delegate& pOnRelease)
{
	delete mOnRelease;
	mOnRelease = new Delegate(pOnRelease);
}

void Button::SetOnClickDelegate(const Delegate& pOnClick)
{
	delete mOnClick;
	mOnClick = new Delegate(pOnClick);
}

void Button::SetOnDragDelegate(const DelegateXY& pOnDrag)
{
	delete mOnDrag;
	mOnDrag = new DelegateXY(pOnDrag);
}



}
