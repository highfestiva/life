
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiScrollBar.h"
#include "../../Include/GUI/UiGridLayout.h"
#include "../../Include/GUI/UiButton.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../../Lepra/Include/Log.h"
#include <math.h>



namespace UiTbc
{



uint8 ScrollBar::smIconArrowLeft[] = 

{	0  , 0  ,0  , 0  , 0  , 255, 0  , 0  ,
	0  , 0  ,0  , 0  , 255, 255, 0  , 0  ,
	0  , 0  ,0  , 255, 255, 255, 0  , 0  ,
	0  , 0  ,255, 255, 255, 255, 0  , 0  ,
	0  , 0  ,0  , 255, 255, 255, 0  , 0  ,
	0  , 0  ,0  , 0  , 255, 255, 0  , 0  ,
	0  , 0  ,0  , 0  , 0  , 255, 0  , 0  ,
	0  , 0  ,0  , 0  , 0  , 0  , 0  , 0  ,
};

uint8 ScrollBar::smIconArrowRight[] = 

{	 0  , 0  ,255, 0  , 0  , 0  , 0  , 0  ,
	 0  , 0  ,255, 255, 0  , 0  , 0  , 0  ,
	 0  , 0  ,255, 255, 255, 0  , 0  , 0  ,
	 0  , 0  ,255, 255, 255, 255, 0  , 0  ,
	 0  , 0  ,255, 255, 255, 0  , 0  , 0  ,
	 0  , 0  ,255, 255, 0  , 0  , 0  , 0  ,
	 0  , 0  ,255, 0  , 0  , 0  , 0  , 0  ,
	 0  , 0  ,0  , 0  , 0  , 0  , 0  , 0  ,
};

uint8 ScrollBar::smIconArrowUp[] = 

{	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 255, 0  , 0  , 0  , 0  ,
	0  , 0  , 255, 255, 255, 0  , 0  , 0  ,
	0  , 255, 255, 255, 255, 255, 0  , 0  ,
	255, 255, 255, 255, 255, 255, 255, 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
};

uint8 ScrollBar::smIconArrowDown[] = 

{	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	255, 255, 255, 255, 255, 255, 255, 0  ,
	0  , 255, 255, 255, 255, 255, 0  , 0  ,
	0  , 0  , 255, 255, 255, 0  , 0  , 0  ,
	0  , 0  , 0  , 255, 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
};

Painter::ImageID ScrollBar::smIconLeftID  = Painter::INVALID_IMAGEID;
Painter::ImageID ScrollBar::smIconRightID = Painter::INVALID_IMAGEID;
Painter::ImageID ScrollBar::smIconUpID    = Painter::INVALID_IMAGEID;
Painter::ImageID ScrollBar::smIconDownID  = Painter::INVALID_IMAGEID;

Painter* ScrollBar::smPrevPainter = 0;
ScrollBar::ScrollbarCleaner* ScrollBar::smCleaner = 0;

ScrollBar::ScrollBar(Style pStyle, int pSize,  int pButtonSize, const Color& pBodyColor,
	const Color& pBackgColor, BorderComponent::BorderShadeFunc pBorderShadeFunc,
	int pBorderWidth, const str& pName):
	RectComponent(pBackgColor, pName, CreateLayout(pStyle)),
	mStyle(pStyle),
	mUserDefinedGfx(false),
	mBackgImageID(Painter::INVALID_IMAGEID),
	mTLButton(0),
	mBRButton(0),
	mScrollerButton(0),
	mTLRect(0),
	mBRRect(0),
	mBodyColor(pBodyColor),
	mVisible(1.0),
	mMax(1.0),
	mPos(0.0),
	mScrollSpeed(0),
	mFirstDelay(0.5),
	mDelay(0.08),
	mFirstDelayDone(false),
	mSize(pSize),
	mButtonSize(pButtonSize),
	mScrollerSize(0),
	mBorderShadeFunc(pBorderShadeFunc),
	mBorderWidth(pBorderWidth),
	mOwner(0)
{
	InitPreferredSize();

	LoadIcons();
	LoadButtons();
	CheckAndSetSize();
	InitPreferredSize();

	DoLayout();
	UpdateLayout();
}

ScrollBar::ScrollBar(Style pStyle, Painter::ImageID pBackgImageID, Button* pTopLeftButton,
	Button* pBottomRightButton, Button* pScrollerButton, const str& pName):
	RectComponent(pBackgImageID, pName, CreateLayout(pStyle)),
	mStyle(pStyle),
	mUserDefinedGfx(true),
	mBackgImageID(pBackgImageID),
	mTLButton(pTopLeftButton),
	mBRButton(pBottomRightButton),
	mScrollerButton(pScrollerButton),
	mTLRect(0),
	mBRRect(0),
	mBodyColor(0, 0, 0),
	mVisible(1.0),
	mMax(1.0),
	mPos(0.0),
	mScrollSpeed(0),
	mFirstDelay(0.5),
	mDelay(0.08),
	mFirstDelayDone(false),
	mSize(0),
	mButtonSize(0),
	mScrollerSize(0),
	mBorderShadeFunc(BorderComponent::LINEAR),
	mBorderWidth(3),
	mOwner(0)
{
	CheckAndSetSize();
	InitPreferredSize();

	LoadIcons();
	LoadButtons();
	CheckAndSetSize();
	InitPreferredSize();

	DoLayout();
	UpdateLayout();
}

GridLayout* ScrollBar::CreateLayout(Style pStyle)
{
	if (pStyle == HORIZONTAL)
	{
		return new GridLayout(1, 5);
	}
	else
	{
		return new GridLayout(5, 1);
	}
}



void ScrollBar::Repaint(Painter* pPainter)
{
	DoLayout();
	Parent::Repaint(pPainter);
}

void ScrollBar::SetScrollRatio(float64 pVisible, float64 pMax)
{
	if (pMax > 0.0)
	{
		mMax = pMax;
		mVisible = pVisible < 0 ? 0 : (pVisible > mMax ? mMax : pVisible);

		if (mStyle == HORIZONTAL)
		{
			if (mScrollerSize == 0)
			{
				SetMinSize(2 * mButtonSize + (int)((float64)mBorderWidth * 2.0), mSize);
			}
			else
			{
				SetMinSize(2 * mButtonSize + mScrollerSize, mSize);
			}
		}
		else
		{
			if (mScrollerSize == 0)
			{
				SetMinSize(mSize, 2 * mButtonSize + (int)((float64)mBorderWidth * 2.0));
			}
			else
			{
				SetMinSize(mSize, 2 * mButtonSize + mScrollerSize);
			}
		}
	}

	SetNeedsRepaint(true);
}

float64 ScrollBar::GetScrollRatioVisible() const
{
	return mVisible;
}

float64 ScrollBar::GetScrollRatioMax() const
{
	return mMax;
}

void ScrollBar::SetScrollPos(float64 pPos)
{
	mPos = pPos < 0 ? 0 : (pPos > 1 ? 1 : pPos);
	SetNeedsRepaint(true);
	if (mOwner != 0)
	{
		mOwner->UpdateLayout();
	}
}

void ScrollBar::DoSetSize(int pWidth, int pHeight)
{
	Parent::DoSetSize(pWidth, pHeight);
	DoLayout();
}

void ScrollBar::LoadIcons()
{
	if (mStyle == HORIZONTAL)
	{
		AddImage(smIconLeftID, smIconArrowLeft, 8);
		AddImage(smIconRightID, smIconArrowRight, 8);
	}
	else
	{
		AddImage(smIconUpID, smIconArrowUp, 8);
		AddImage(smIconDownID, smIconArrowDown, 8);
	}
}

void ScrollBar::AddImage(Painter::ImageID& pImageID, uint8 pImage[], int pDim)
{
	GUIImageManager* lIMan = GetImageManager();

	if (smPrevPainter != lIMan->GetPainter())
	{
		smIconLeftID  = Painter::INVALID_IMAGEID;
		smIconRightID = Painter::INVALID_IMAGEID;
		smIconUpID    = Painter::INVALID_IMAGEID;
		smIconDownID  = Painter::INVALID_IMAGEID;
		smPrevPainter = lIMan->GetPainter();
	}

	if (pImageID == Painter::INVALID_IMAGEID)
	{
		Canvas lCanvas(pDim, pDim, Canvas::BITDEPTH_8_BIT);
		lCanvas.SetBuffer(pImage);
		Canvas lAlpha(lCanvas, true);
		lCanvas.ConvertTo32BitWithAlpha(lAlpha);
		pImageID = lIMan->AddImage(lCanvas, GUIImageManager::CENTERED, GUIImageManager::ALPHATEST, 128);
	}
}

void ScrollBar::LoadButtons()
{
	if (mTLButton == 0)
	{
		if (mStyle == HORIZONTAL)
		{
			mTLButton = new Button(mBorderShadeFunc, mBorderWidth, mBodyColor, _T("LeftButton"));
			mTLButton->SetIcon(smIconLeftID, Button::ICON_CENTER);
		}
		else
		{
			mTLButton = new Button(mBorderShadeFunc, mBorderWidth, mBodyColor, _T("UpButton"));
			mTLButton->SetIcon(smIconUpID, Button::ICON_CENTER);
		}
	}

	if (mStyle == HORIZONTAL)
		AddChild(mTLButton, 0, 0);
	else
		AddChild(mTLButton, 0, 0);

	if (mBRButton == 0)
	{
		if (mStyle == HORIZONTAL)
		{
			mBRButton = new Button(mBorderShadeFunc, mBorderWidth, mBodyColor, _T("RightButton"));
			mBRButton->SetIcon(smIconRightID, Button::ICON_CENTER);
		}
		else
		{
			mBRButton = new Button(mBorderShadeFunc, mBorderWidth, mBodyColor, _T("DownButton"));
			mBRButton->SetIcon(smIconDownID, Button::ICON_CENTER);
		}
	}

	if (mStyle == HORIZONTAL)
		AddChild(mBRButton, 0, 4);
	else
		AddChild(mBRButton, 4, 0);

	if (mScrollerButton == 0)
	{
		if (mStyle == HORIZONTAL)
			mScrollerButton = new Button(mBorderShadeFunc, mBorderWidth, mBodyColor, _T("HScroller"));
		else
			mScrollerButton = new Button(mBorderShadeFunc, mBorderWidth, mBodyColor, _T("VScroller"));
	}

	if (mStyle == HORIZONTAL)
		AddChild(mScrollerButton, 0, 2);
	else
		AddChild(mScrollerButton, 2, 0);

	// Finally add the two dummy rects.
	mTLRect = new RectComponent(_T("TLDummyRect"));
	mBRRect = new RectComponent(_T("BRDummyRect"));
	
	if (mStyle == HORIZONTAL)
	{
		AddChild(mTLRect, 0, 1);
		AddChild(mBRRect, 0, 3);

		SetMinSize(2 * mButtonSize + mBorderWidth * 2, mSize);
	}
	else
	{
		AddChild(mTLRect, 1, 0);
		AddChild(mBRRect, 3, 0);

		SetMinSize(mSize, 2 * mButtonSize + mBorderWidth * 2);
	}

	mTLButton->SetOnPress(ScrollBar, OnScrollTL);
	mBRButton->SetOnPress(ScrollBar, OnScrollBR);
	mTLButton->SetOnRelease(ScrollBar, OnStopScroll);
	mBRButton->SetOnRelease(ScrollBar, OnStopScroll);

	mScrollerButton->SetOnPress(ScrollBar, OnScrollerDown);
	mScrollerButton->SetOnDrag(ScrollBar, OnScrollerDragged);
}

void ScrollBar::CheckButtonSize(Button* pButton)
{
	if (pButton != 0)
	{
		if (mStyle == HORIZONTAL)
		{
			if (pButton->GetPreferredHeight() > mSize)
			{
				mSize = pButton->GetPreferredHeight();
			}

			if (pButton->GetPreferredWidth() > mButtonSize)
			{
				mButtonSize = pButton->GetPreferredWidth();
			}
		}
		else
		{
			if (pButton->GetPreferredWidth() > mSize)
			{
				mSize = pButton->GetPreferredWidth();
			}

			if (pButton->GetPreferredHeight() > mButtonSize)
			{
				mButtonSize = pButton->GetPreferredHeight();
			}
		}
	}
}

void ScrollBar::CheckAndSetSize()
{
	CheckButtonSize(mTLButton);
	CheckButtonSize(mBRButton);

	if (mScrollerButton != 0)
	{
		if (mStyle == HORIZONTAL)
		{
			if (mScrollerButton->GetPreferredHeight() > mSize)
			{
				mSize = mScrollerButton->GetPreferredHeight();
			}
		}
		else
		{
			if (mScrollerButton->GetPreferredWidth() > mSize)
			{
				mSize = mScrollerButton->GetPreferredWidth();
			}
		}
	}

	if (mSize == 0)
	{
		mSize = 16;
	}
	if (mButtonSize == 0)
	{
		mButtonSize = 16;
	}
}

void ScrollBar::InitPreferredSize()
{
	if (mStyle == HORIZONTAL)
	{
		if (mTLButton != 0)
		{
			mTLButton->SetPreferredSize(mButtonSize, mSize);
		}

		if (mBRButton != 0)
		{
			mBRButton->SetPreferredSize(mButtonSize, mSize);
		}

		if (mScrollerButton != 0)
		{
			mTLButton->SetPreferredSize(GetSize().x - 2 * mButtonSize, mSize);
		}
	}
	else
	{
		if (mTLButton != 0)
		{
			mTLButton->SetPreferredSize(mSize, mButtonSize);
		}

		if (mBRButton != 0)
		{
			mBRButton->SetPreferredSize(mSize, mButtonSize);
		}

		if (mScrollerButton != 0)
		{
			mTLButton->SetPreferredSize(mSize, GetSize().y - 2 * mButtonSize);
		}
	}

	if (mTLRect != 0)
	{
		mTLRect->SetPreferredSize(0, 0);
	}
	
	if (mBRRect != 0)
	{
		mBRRect->SetPreferredSize(0, 0);
	}
}

void ScrollBar::DoLayout()
{
	PixelRect lRect(Parent::GetScreenRect());
	float64 lRatio = mVisible / mMax;

	PixelCoord lMinSize(GetMinSize());

	if (mStyle == HORIZONTAL)
	{
		/* TODO: complete intention.
		if (lRect.GetWidth() < lMinSize.x)
		{
			int stophere = 0;
		}*/

		int lWidth = lRect.GetWidth() - mButtonSize * 2;
		int lScrollerWidth = (int)floor(((float64)lWidth * lRatio));

		if (mScrollerSize != 0)
		{
			lScrollerWidth = mScrollerSize;
		}
		else if(lScrollerWidth < mScrollerButton->GetMinSize().x)
		{
			lScrollerWidth = mScrollerButton->GetMinSize().x;
		}

		int lRest = lWidth - lScrollerWidth;
		int lScrollPos = (int)floor((float64)lRest * mPos);

		mScrollerButton->SetPreferredSize(lScrollerWidth, mSize);

		mTLRect->SetPreferredSize(lScrollPos, mSize);
		if (lScrollPos == 0)
		{
			mBRRect->SetPreferredSize(lRest - lScrollPos, mSize);
		}
		else
		{
			mBRRect->SetPreferredSize(0, mSize);
		}

		mTLButton->SetPreferredSize(mButtonSize, mSize);
		mBRButton->SetPreferredSize(mButtonSize, mSize);
	}
	else
	{
		int lHeight = lRect.GetHeight() - mButtonSize * 2;
		int lScrollerHeight = (int)floor(((float64)lHeight * lRatio));

		if (mScrollerSize != 0)
		{
			lScrollerHeight = mScrollerSize;
		}
		else if(lScrollerHeight < mScrollerButton->GetMinSize().y)
		{
			lScrollerHeight = mScrollerButton->GetMinSize().y;
		}

		int lRest = lHeight - lScrollerHeight;
		int lScrollPos = (int)floor((float64)lRest * mPos);

		mScrollerButton->SetPreferredSize(mSize, lScrollerHeight);
		mTLRect->SetPreferredSize(mSize, lScrollPos);
		mBRRect->SetPreferredSize(mSize, lRest - lScrollPos);

		mTLButton->SetPreferredSize(mSize, mButtonSize);
		mBRButton->SetPreferredSize(mSize, mButtonSize);
	}
}

void ScrollBar::OnScrollTL(Button* /*pButton*/)
{
	mScrollSpeed = -1.0 / (mMax - mVisible);
	SetScrollPos(GetScrollPos() + mScrollSpeed);
	mTimer.UpdateTimer();
	mTimer.ClearTimeDiff();
	mFirstDelayDone = false;

	DesktopWindow* lDWin = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
	lDWin->AddIdleSubscriber(this);
}

void ScrollBar::OnScrollBR(Button* /*pButton*/)
{
	mScrollSpeed = 1.0 / (mMax - mVisible);
	SetScrollPos(GetScrollPos() + mScrollSpeed);
	mTimer.UpdateTimer();
	mTimer.ClearTimeDiff();
	mFirstDelayDone = false;
	DesktopWindow* lDWin = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
	lDWin->AddIdleSubscriber(this);
}

void ScrollBar::OnStopScroll(Button* /*pButton*/)
{
	mScrollSpeed = 0;
	mFirstDelayDone = false;
	DesktopWindow* lDWin = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
	lDWin->RemoveIdleSubscriber(this);
}

void ScrollBar::OnScrollerDown(Button* pButton)
{
	if (mUserDefinedGfx == false)
	{
		unsigned lStyle = pButton->GetBorderStyle();
		lStyle &= ~Window::BORDER_SUNKEN;
		pButton->SetBorder(lStyle, pButton->GetBorderWidth());
	}
}

bool ScrollBar::OnScrollerDragged(Button* pButton, int pDeltaX, int pDeltaY)
{
	float64 lDeltaPos = 0;

	PixelRect lRect(GetScreenRect());
	PixelCoord lScrollerSize(mScrollerButton->GetSize());

	if (mStyle == HORIZONTAL)
	{
		lDeltaPos = (float64)pDeltaX / (float64)(lRect.GetWidth() - (lScrollerSize.x + 2 * mButtonSize));
	}
	else
	{
		lDeltaPos = (float64)pDeltaY / (float64)(lRect.GetHeight() - (lScrollerSize.y + 2 * mButtonSize));
	}
	SetScrollPos(GetScrollPos() + lDeltaPos);

	if (mUserDefinedGfx == false)
	{
		unsigned lStyle = pButton->GetBorderStyle();
		lStyle &= ~Window::BORDER_SUNKEN;
		pButton->SetBorder(lStyle, pButton->GetBorderWidth());
	}
	return (false);
}

void ScrollBar::OnConnectedToDesktopWindow()
{
	if(smCleaner == 0)
	{
		DesktopWindow* lDesktopWindow = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
		if(lDesktopWindow != 0)
		{
			smCleaner = new ScrollbarCleaner();
			lDesktopWindow->AddCleaner(smCleaner);
		}
	}
}

void ScrollBar::OnIdle()
{
	if (mScrollSpeed != 0)
	{
		mTimer.UpdateTimer();
		float64 lDelay = mFirstDelay;

		if (mFirstDelayDone == true)
		{
			lDelay = mDelay;
		}

		while (mTimer.GetTimeDiffF() > lDelay)
		{
			SetScrollPos(GetScrollPos() + mScrollSpeed);
			mTimer.ReduceTimeDiff((uint64)(1000.0 * lDelay));
			mFirstDelayDone = true;
			lDelay = mDelay;
		}

		SetNeedsRepaint(true);
	}
}



}
