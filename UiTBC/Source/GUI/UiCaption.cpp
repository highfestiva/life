/*
	Class:  Caption
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiCaption.h"
#include "../../Include/GUI/UiCenterLayout.h"
#include "../../Include/GUI/UiStackLayout.h"

namespace UiTbc
{

Caption::Caption(const Color& pActiveColor, const Color& pInactiveColor, int pHeight) :
	RectComponent(pActiveColor, _T("Caption"), new StackLayout(1)),
	mLabel(0),
	mCaptionRect(0),
	mButtonRect(0),
	mLeftImageRect(0),
	mCenterImageRect(0),
	mRightImageRect(0),
	mLeftButton(0),
	mRightButton(0),
	mMiddleButton(0),
	mActiveTLColor(pActiveColor),
	mActiveTRColor(pActiveColor),
	mActiveBLColor(pActiveColor),
	mActiveBRColor(pActiveColor),
	mInactiveTLColor(pInactiveColor),
	mInactiveTRColor(pInactiveColor),
	mInactiveBLColor(pInactiveColor),
	mInactiveBRColor(pInactiveColor),
	mActiveLeftImageID(Painter::INVALID_IMAGEID),
	mActiveRightImageID(Painter::INVALID_IMAGEID),
	mActiveCenterImageID(Painter::INVALID_IMAGEID),
	mInactiveLeftImageID(Painter::INVALID_IMAGEID),
	mInactiveRightImageID(Painter::INVALID_IMAGEID),
	mInactiveCenterImageID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mActiveTextBackgColor(255, 255, 255),
	mInactiveTextColor(0, 0, 0),
	mInactiveTextBackgColor(255, 255, 255),
	mMovingWindow(false),
	mActive(true),
	mStyle(SINGLECOLOR_STYLE)
{
	SetFontColor(OFF_BLACK);

	SetMinSize(0, pHeight);
	SetPreferredSize(PixelCoord(0, pHeight));

	mLabel = new Label(false, _T("Label"));
	mLabel->SetPreferredHeight(pHeight);

	RectComponent* lTopLayer = new RectComponent(_T("TopLayer"), new GridLayout(1, 2));
	lTopLayer->SetPreferredSize(0, 0);
	AddChild(lTopLayer);

	mCaptionRect = new RectComponent(_T("CaptionRect"), new GridLayout(1, 1));
	mCaptionRect->AddChild(mLabel);
	lTopLayer->AddChild(mCaptionRect);

	RectComponent* lButtonRect = new RectComponent(_T("ButtonArea"), new CenterLayout);
	lButtonRect->SetPreferredSize(0, 0);
	lTopLayer->AddChild(lButtonRect);

	mButtonRect = new RectComponent(Color(255, 255, 255), _T("ButtonRect"), new GridLayout(1, 3));
	mButtonRect->SetPreferredSize(1, 0);
	lButtonRect->AddChild(mButtonRect);
}

Caption::Caption(const Color& pActiveTopLeftColor, const Color& pActiveTopRightColor,
	const Color& pActiveBottomLeftColor, const Color& pActiveBottomRightColor,
	const Color& pInactiveTopLeftColor, const Color& pInactiveTopRightColor,
	const Color& pInactiveBottomLeftColor, const Color& pInactiveBottomRightColor,
	int pHeight) :
	RectComponent(pActiveTopLeftColor, pActiveTopRightColor, pActiveBottomRightColor, pActiveBottomLeftColor, _T("Caption"), new StackLayout(1)),
	mLabel(0),
	mCaptionRect(0),
	mButtonRect(0),
	mLeftImageRect(0),
	mCenterImageRect(0),
	mRightImageRect(0),
	mLeftButton(0),
	mRightButton(0),
	mMiddleButton(0),
	mActiveTLColor(pActiveTopLeftColor),
	mActiveTRColor(pActiveTopRightColor),
	mActiveBLColor(pActiveBottomLeftColor),
	mActiveBRColor(pActiveBottomRightColor),
	mInactiveTLColor(pInactiveTopLeftColor),
	mInactiveTRColor(pInactiveTopRightColor),
	mInactiveBLColor(pInactiveBottomLeftColor),
	mInactiveBRColor(pInactiveBottomRightColor),
	mActiveLeftImageID(Painter::INVALID_IMAGEID),
	mActiveRightImageID(Painter::INVALID_IMAGEID),
	mActiveCenterImageID(Painter::INVALID_IMAGEID),
	mInactiveLeftImageID(Painter::INVALID_IMAGEID),
	mInactiveRightImageID(Painter::INVALID_IMAGEID),
	mInactiveCenterImageID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mActiveTextBackgColor(255, 255, 255),
	mInactiveTextColor(0, 0, 0),
	mInactiveTextBackgColor(255, 255, 255),
	mMovingWindow(false),
	mActive(true),
	mStyle(MULTICOLOR_STYLE)
{
	SetFontColor(OFF_BLACK);

	SetMinSize(0, pHeight);
	SetPreferredSize(PixelCoord(0, pHeight));

	mLabel = new Label(false, _T("Label"));
	mLabel->SetPreferredHeight(pHeight);

	RectComponent* lTopLayer = new RectComponent(_T("TopLayer"), new GridLayout(1, 2));
	AddChild(lTopLayer);

	mCaptionRect = new RectComponent(_T("CaptionRect"), new GridLayout(1, 1));
	mCaptionRect->AddChild(mLabel);
	lTopLayer->AddChild(mCaptionRect);

	RectComponent* lButtonRect = new RectComponent(_T("ButtonArea"), new CenterLayout);
	lButtonRect->SetPreferredSize(0, 0);
	lTopLayer->AddChild(lButtonRect);

	mButtonRect = new RectComponent(Color(255, 255, 255), _T("ButtonRect"), new GridLayout(1, 3));
	mButtonRect->SetPreferredSize(1, 0);
	lButtonRect->AddChild(mButtonRect);
}

Caption::Caption(Painter::ImageID pActiveLeftImageID, Painter::ImageID pActiveRightImageID,
	Painter::ImageID pActiveCenterImageID, Painter::ImageID pInactiveLeftImageID,
	Painter::ImageID pInactiveRightImageID, Painter::ImageID pInactiveCenterImageID,
	int pHeight) :
	RectComponent(_T("Caption"), new StackLayout(2)),
	mLabel(0),
	mCaptionRect(0),
	mButtonRect(0),
	mLeftImageRect(0),
	mCenterImageRect(0),
	mRightImageRect(0),
	mLeftButton(0),
	mRightButton(0),
	mMiddleButton(0),
	mActiveTLColor(0, 0, 0),
	mActiveTRColor(0, 0, 0),
	mActiveBLColor(0, 0, 0),
	mActiveBRColor(0, 0, 0),
	mInactiveTLColor(0, 0, 0),
	mInactiveTRColor(0, 0, 0),
	mInactiveBLColor(0, 0, 0),
	mInactiveBRColor(0, 0, 0),
	mActiveLeftImageID(pActiveLeftImageID),
	mActiveRightImageID(pActiveRightImageID),
	mActiveCenterImageID(pActiveCenterImageID),
	mInactiveLeftImageID(pInactiveLeftImageID),
	mInactiveRightImageID(pInactiveRightImageID),
	mInactiveCenterImageID(pInactiveCenterImageID),
	mText(_T("")),
	mActiveTextBackgColor(255, 255, 255),
	mInactiveTextColor(0, 0, 0),
	mInactiveTextBackgColor(255, 255, 255),
	mMovingWindow(false),
	mActive(true),
	mStyle(IMAGE_STYLE)
{
	SetFontColor(OFF_BLACK);

	GUIImageManager* lIMan = GetImageManager();

	PixelCoord lLeftImageSize(lIMan->GetImageSize(pActiveLeftImageID));
	PixelCoord lRightImageSize(lIMan->GetImageSize(pActiveRightImageID));
	PixelCoord lCenterImageSize(lIMan->GetImageSize(pActiveCenterImageID));

	SetMinSize(lLeftImageSize.x + lRightImageSize.x, pHeight);
	SetPreferredSize(0, pHeight);

	mLabel = new Label(false, _T("Label"));
	mLabel->SetPreferredHeight(pHeight);

	RectComponent* lBottomLayer = new RectComponent(_T("BottomLayer"), new GridLayout(1, 3));
	lBottomLayer->SetPreferredSize(0, 0);
	AddChild(lBottomLayer);

	RectComponent* lTopLayer = new RectComponent(_T("TopLayer"), new GridLayout(1, 2));
	lTopLayer->SetPreferredSize(0, 0);
	AddChild(lTopLayer);

	mCaptionRect = new RectComponent(_T("CaptionRect"), new GridLayout(1, 1));
	mCaptionRect->AddChild(mLabel);
	lTopLayer->AddChild(mCaptionRect);

	RectComponent* lButtonRect = new RectComponent(_T("ButtonArea"), new CenterLayout);
	lButtonRect->SetPreferredSize(0, 0);
	lTopLayer->AddChild(lButtonRect);

	mButtonRect = new RectComponent(_T("ButtonRect"), new GridLayout(1, 3));
	mButtonRect->SetPreferredSize(1, 0);
	lButtonRect->AddChild(mButtonRect);

	mLeftImageRect = new RectComponent(pActiveLeftImageID, _T("CaptionRectLeft"));
	mLeftImageRect->SetPreferredSize(lLeftImageSize);
	lBottomLayer->AddChild(mLeftImageRect, 0, 0);

	mCenterImageRect = new RectComponent(pActiveCenterImageID, _T("CaptionRectCenter"));
	mCenterImageRect->SetPreferredSize(0, lCenterImageSize.y, false);
	lBottomLayer->AddChild(mCenterImageRect, 0, 1);

	mRightImageRect = new RectComponent(pActiveRightImageID, _T("CaptionRectRight"));
	mRightImageRect->SetPreferredSize(lRightImageSize);
	lBottomLayer->AddChild(mRightImageRect, 0, 2);
}

Caption::~Caption()
{
}

void Caption::InitCaption()
{
}

Button* Caption::SetLeftButton(Button* pButton)
{
	return SetButton(mLeftButton, pButton);
}

Button* Caption::SetRightButton(Button* pButton)
{
	return SetButton(mRightButton, pButton);
}

Button* Caption::SetMiddleButton(Button* pButton)
{
	return SetButton(mMiddleButton, pButton);
}

Button* Caption::SetButton(Button*& pMemberButton, Button* pNewButton)
{
	Button* lOld = pMemberButton;

	if (pMemberButton != 0)
	{
		mButtonRect->RemoveChild(pMemberButton, 0);
		if (mButtonRect->GetNumChildren() == 0)
		{
			mButtonRect->SetPreferredSize(1, 0, true);
		}
	}

	pMemberButton = pNewButton;

	if (pMemberButton != 0)
	{
		mButtonRect->AddChild(pMemberButton, 0, 1);
		mButtonRect->SetPreferredSize(0, 0, true);
		const int lGapPixels = 4;
		mButtonRect->GetParent()->SetPreferredWidth(mButtonRect->GetMinSize().x +
			mButtonRect->GetNumChildren() * lGapPixels);
	}

	return lOld;
}

void Caption::ForceRepaint()
{
	SetNeedsRepaint(true);
}

void Caption::SetActive(bool pActive)
{
	if (mActive == pActive)
	{
		return;	// TRICKY: RAII simplifies.
	}

	mActive = pActive;

	GUIImageManager* lIMan = GetImageManager();

	switch(mStyle)
	{
	case SINGLECOLOR_STYLE:
		if (mActive == true)
		{
			RectComponent::SetColor(mActiveTLColor);
		}
		else
		{
			RectComponent::SetColor(mInactiveTLColor);
		}
		break;
	case MULTICOLOR_STYLE:
		if (mActive == true)
		{
			RectComponent::SetColor(mActiveTLColor, mActiveTRColor, mActiveBRColor, mActiveBLColor);
		}
		else
		{
			RectComponent::SetColor(mInactiveTLColor, mInactiveTRColor, mInactiveBRColor, mInactiveBLColor);
		}
		break;
	case IMAGE_STYLE:
	{
		if (mActive == true)
		{
			mLeftImageRect->SetImage(mActiveLeftImageID);
			mCenterImageRect->SetImage(mActiveCenterImageID);
			mRightImageRect->SetImage(mActiveRightImageID);
		}
		else
		{
			mLeftImageRect->SetImage(mInactiveLeftImageID);
			mCenterImageRect->SetImage(mInactiveCenterImageID);
			mRightImageRect->SetImage(mInactiveRightImageID);
		}

		PixelCoord lLeftImageSize(lIMan->GetImageSize(mActiveLeftImageID));
		PixelCoord lRightImageSize(lIMan->GetImageSize(mActiveRightImageID));
		PixelCoord lCenterImageSize(lIMan->GetImageSize(mActiveCenterImageID));

		mLeftImageRect->SetPreferredSize(lLeftImageSize);
		mCenterImageRect->SetPreferredSize(0, lCenterImageSize.y);
		mRightImageRect->SetPreferredSize(lRightImageSize);
	}
		break;
	}

	if (mActive == true)
	{
		mLabel->SetText(mText, GetTextColor(), mActiveTextBackgColor);
	}
	else
	{
		mLabel->SetText(mText, mInactiveTextColor, mInactiveTextBackgColor);
	}
}

bool Caption::OnLButtonDown(int pMouseX, int pMouseY)
{
	Component::OnLButtonDown(pMouseX, pMouseY);
	Component* lChild = GetChild(pMouseX, pMouseY);

	while (lChild != 0 &&
		  lChild != mLeftButton && 
		  lChild != mRightButton && 
		  lChild != mMiddleButton)
	{
		lChild = lChild->GetChild(pMouseX, pMouseY);
	}

	if (lChild == 0)
	{
		if (IsOver(pMouseX, pMouseY) == true)
		{
			mMovingWindow = true;
			SetMouseFocus();
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Caption::OnLButtonUp(int pMouseX, int pMouseY)
{
	mMovingWindow = false;
	ReleaseMouseFocus();
	return Component::OnLButtonUp(pMouseX, pMouseY);
}

bool Caption::OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY)
{
	if (mMovingWindow == true)
	{
		Window* lParentWindow = (Window*)Component::GetParentOfType(WINDOW);

		if (lParentWindow != 0)
		{
			lParentWindow->SetPos(lParentWindow->GetPos() + PixelCoord(pDeltaX, pDeltaY));
		}

		return true;
	}
	else
	{
		return Component::OnMouseMove(pMouseX, pMouseY, pDeltaX, pDeltaY);
	}
}

void Caption::SetIcon(Painter::ImageID pIconID)
{
	mLabel->SetIcon(pIconID);
}

void Caption::SetText(const str& pText,
					  const Color& pActiveTextColor,
					  const Color& pActiveBackgColor,
					  const Color& pInactiveTextColor,
					  const Color& pInactiveBackgColor)
{
	mText                   = pText;
	mActiveTextBackgColor   = pActiveBackgColor;
	mInactiveTextColor      = pInactiveTextColor;
	mInactiveTextBackgColor = pInactiveBackgColor;
	SetFontColor(pActiveTextColor);

	mLabel->SetText(mText, GetTextColor(), mActiveTextBackgColor);
}

bool Caption::Check(unsigned pFlags, unsigned pFlag)
{
	return ((pFlags & pFlag) != 0);
}

Component::Type Caption::GetType() const
{
	return Component::CAPTION;
}

bool Caption::GetActive() const
{
	return mActive;
}



}
