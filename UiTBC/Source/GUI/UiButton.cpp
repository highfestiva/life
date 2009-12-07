
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Alexander Hugestrand



#include "../../Include/GUI/UiButton.h"



namespace UiTbc
{



Button::Button(const Lepra::String& pName) :
	Window(pName),
	mIconID(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_CENTER),
	mText(_T("")),
	mTextColor(0, 0, 0),
	mTextBackgColor(255, 255, 255),
	mTextBlendFunc(NO_BLEND),
	mTextAlpha(128),
	mPressed(false),
	mState(RELEASED),
	mOnPressedFunctor(0),
	mOnReleasedFunctor(0),
	mOnClickedFunctor(0),
	mOnUnclickedFunctor(0),
	mOnButtonDraggedFunctor(0)
{
	InitializeHoover();
}

Button::Button(const Lepra::Color& pColor, const Lepra::String& pName):
	Window(pColor, pName),
	mIconID(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_CENTER),
	mText(_T("")),
	mTextColor(0, 0, 0),
	mTextBackgColor(255, 255, 255),
	mTextBlendFunc(NO_BLEND),
	mTextAlpha(128),
	mPressed(false),
	mState(RELEASED),
	mOnPressedFunctor(0),
	mOnReleasedFunctor(0),
	mOnClickedFunctor(0),
	mOnUnclickedFunctor(0),
	mOnButtonDraggedFunctor(0)
{
	InitializeHoover();
}

Button::Button(BorderComponent::BorderShadeFunc pShadeFunc, int pBorderWidth, const Lepra::Color& pColor,
	const Lepra::String& pName):
	Window((pShadeFunc == BorderComponent::LINEAR ? Parent::BORDER_LINEARSHADING : 0), pBorderWidth, pColor, pName),
	mIconID(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_CENTER),
	mText(_T("")),
	mTextColor(0, 0, 0),
	mTextBackgColor(255, 255, 255),
	mTextBlendFunc(NO_BLEND),
	mTextAlpha(128),
	mPressed(false),
	mState(RELEASED),
	mOnPressedFunctor(0),
	mOnReleasedFunctor(0),
	mOnClickedFunctor(0),
	mOnUnclickedFunctor(0),
	mOnButtonDraggedFunctor(0)
{
	InitializeHoover();
}

Button::Button(Painter::ImageID pReleasedImageID, Painter::ImageID pPressedImageID,
	Painter::ImageID pReleasedActiveImageID,	// Mouse over.
	Painter::ImageID pPressedActiveImageID, Painter::ImageID pReleasingImageID,
	Painter::ImageID pPressingImageID, const Lepra::String& pName) :
	Window(pReleasedImageID, pName),
	mReleasedImageID(pReleasedImageID),
	mPressedImageID(pPressedImageID),
	mReleasedActiveImageID(pReleasedActiveImageID),
	mPressedActiveImageID(pPressedActiveImageID),
	mReleasingImageID(pReleasingImageID),
	mPressingImageID(pPressingImageID),
	mIconID(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_CENTER),
	mText(_T("")),
	mTextColor(0, 0, 0),
	mTextBackgColor(255, 255, 255),
	mTextBlendFunc(NO_BLEND),
	mTextAlpha(128),
	mPressed(false),
	mState(RELEASED),
	mOnPressedFunctor(0),
	mOnReleasedFunctor(0),
	mOnClickedFunctor(0),
	mOnUnclickedFunctor(0),
	mOnButtonDraggedFunctor(0)
{
	InitializeHoover();
}

Button::~Button()
{
	if (mOnPressedFunctor != 0)
	{
		delete mOnPressedFunctor;
	}

	if (mOnReleasedFunctor != 0)
	{
		delete mOnReleasedFunctor;
	}

	if (mOnClickedFunctor != 0)
	{
		delete mOnClickedFunctor;
	}

	if (mOnUnclickedFunctor != 0)
	{
		delete mOnUnclickedFunctor;
	}

	if (mOnButtonDraggedFunctor != 0)
	{
		delete mOnButtonDraggedFunctor;
	}
}

void Button::InitializeHoover()
{
	mHooverColor = GetColor() * 1.2f;
	mPressColor = GetColor() * 0.95f;
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
			if (mOnReleasedFunctor != 0)
			{
				mOnReleasedFunctor->Call(this);
			}
			break;
		case RELEASING:
			break;
		case PRESSED:
		case PRESSED_HOOVER:
			break;
		case PRESSING:
			if (mOnPressedFunctor != 0)
			{
				mOnPressedFunctor->Call(this);
			}
			break;
		}
	}
}

Component::StateComponentList Button::GetStateList(ComponentState pState) const
{
	StateComponentList lList;
	if (pState == STATE_CLICKABLE)
	{
		lList.push_back(StateComponent(0, (Component*)this));
	}
	return (lList);
}

void Button::Repaint(Painter* pPainter)
{
	Parent::Repaint(pPainter);

	GUIImageManager* lIMan = GetImageManager();

	pPainter->PushAttrib(Painter::ATTR_ALL);
	Lepra::PixelRect lRect(Parent::GetClientRect());
	pPainter->ReduceClippingRect(lRect);

	int lOffset = GetPressed() ? 1 : 0;
	int lTextX = lRect.mLeft;

	if (mIconID != Painter::INVALID_IMAGEID)
	{
		Lepra::PixelCoords lImageSize(lIMan->GetImageSize(mIconID));

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
	switch(mTextBlendFunc)
	{
	case NO_BLEND:
		pPainter->SetRenderMode(Painter::RM_NORMAL);
		break;
	case ALPHATEST:
		pPainter->SetRenderMode(Painter::RM_ALPHATEST);
		break;
	case ALPHABLEND:
		pPainter->SetRenderMode(Painter::RM_ALPHABLEND);
		break;
	default:
		pPainter->SetRenderMode(Painter::RM_NORMAL);
		break;
	}

	pPainter->SetAlphaValue(mTextAlpha);

	pPainter->SetColor(mTextColor, 0);
	pPainter->SetColor(mTextBackgColor, 1);
	pPainter->PrintText(mText.c_str(), x, y);
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
		lCallFunctor == true && mOnUnclickedFunctor != 0)
	{
		mOnUnclickedFunctor->Call(this);
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

		if (mOnButtonDraggedFunctor != 0)
		{
			mOnButtonDraggedFunctor->Call(this, pDeltaX, pDeltaY);
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
	return Component::OnMouseMove(pMouseX, pMouseY, pDeltaX, pDeltaY);
}

bool Button::Click(bool pDepress)
{
	bool lClicked = false;
	if (mOnClickedFunctor)
	{
		mOnClickedFunctor->Call(this);
		lClicked = true;
	}
	if (pDepress && mOnUnclickedFunctor)
	{
		mOnUnclickedFunctor->Call(this);
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

void Button::SetText(const Lepra::String& pText, 
		     const Lepra::Color& pTextColor,
		     const Lepra::Color& pBackgColor,
		     BlendFunc pBlendFunc,
		     Lepra::uint8 pAlphaTreshold)
{
	mText           = pText;
	mTextColor      = pTextColor;
	mTextBackgColor = pBackgColor;
	mTextBlendFunc  = pBlendFunc;
	mTextAlpha    = pAlphaTreshold;

	OnTextChanged();
}

const Lepra::String& Button::GetText()
{
	return mText;
}

Component::Type Button::GetType()
{
	return Component::BUTTON;
}

void Button::OnTextChanged()
{
}

void Button::SetOnPressedFunctor(const ButtonFunctor& pOnPressedFunctor)
{
	if (mOnPressedFunctor != 0)
	{
		delete mOnPressedFunctor;
	}

	mOnPressedFunctor = pOnPressedFunctor.CreateCopy();
}

void Button::SetOnReleasedFunctor(const ButtonFunctor& pOnReleasedFunctor)
{
	if (mOnReleasedFunctor != 0)
	{
		delete mOnReleasedFunctor;
	}

	mOnReleasedFunctor = pOnReleasedFunctor.CreateCopy();
}

void Button::SetOnClickedFunctor(const ButtonFunctor& pOnClickedFunctor)
{
	if (mOnClickedFunctor != 0)
	{
		delete mOnClickedFunctor;
	}

	mOnClickedFunctor = pOnClickedFunctor.CreateCopy();
}

void Button::SetOnUnclickedFunctor(const ButtonFunctor& pOnUnclickedFunctor)
{
	if (mOnUnclickedFunctor != 0)
	{
		delete mOnUnclickedFunctor;
	}

	mOnUnclickedFunctor = pOnUnclickedFunctor.CreateCopy();
}

void Button::SetOnButtonDraggedFunctor(const ButtonDraggedFunctor& pOnButtonDraggedFunctor)
{
	if (mOnButtonDraggedFunctor != 0)
	{
		delete mOnButtonDraggedFunctor;
	}

	mOnButtonDraggedFunctor = pOnButtonDraggedFunctor.CreateCopy();
}

ButtonFunctor* Button::GetOnPressedFunctor()
{
	return mOnPressedFunctor;
}

ButtonFunctor* Button::GetOnReleasedFunctor()
{
	return mOnReleasedFunctor;
}

ButtonFunctor* Button::GetOnClickedFunctor()
{
	return mOnClickedFunctor;
}

ButtonFunctor* Button::GetOnUnclickedFunctor()
{
	return mOnUnclickedFunctor;
}

ButtonDraggedFunctor* Button::GetOnButtonDraggedFunctor()
{
	return mOnButtonDraggedFunctor;
}



}
