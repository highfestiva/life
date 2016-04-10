
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/GUI/UiLabel.h"
#include <algorithm>
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../Include/UiPainter.h"



namespace UiTbc
{



Label::Label(const Color& pColor, const wstr& pText):
	Parent(pColor),
	mIconId(Painter::INVALID_IMAGEID),
	mIconAlignment(ICON_RIGHT),
	mText(pText),
	mTextWidth(0),
	mTextHeight(0),
	mSelectable(false)
{
	SetFontColor(pColor);
	SetIsHollow(true);
}

Label::~Label()
{
}

void Label::SetIcon(Painter::ImageID pIconId, IconAlignment pAlignment)
{
	mIconId = pIconId;
	mIconAlignment = pAlignment;
}

void Label::SetText(const wstr& pText)
{
	mText = pText;
}

const wstr& Label::GetText() const
{
	return mText;
}

void Label::SetSelected(bool pSelected)
{
	if (mSelectable == true && pSelected != GetSelected())
	{
		Parent::SetSelected(pSelected);
		UpdateBackground();
	}
}

void Label::Repaint(Painter* pPainter)
{
	SetNeedsRepaint(false);

	ActivateFont(pPainter);
	const int lTextWidth  = pPainter->GetStringWidth(mText);
	const int lTextHeight = pPainter->GetLineHeight() * (std::count(mText.begin(), mText.end(), _T('\n')) + 1);
	if (mTextWidth != lTextWidth || mTextHeight != lTextHeight)
	{
		mTextWidth = lTextWidth;
		mTextHeight = lTextHeight;
		GetParent()->UpdateLayout();
	}

	Parent::Repaint(pPainter);

	GUIImageManager* lIMan = GetImageManager();

	PixelCoord lPos(GetScreenPos());
	PixelRect lRect(lPos, lPos + GetSize());
/*#ifndef LEPRA_TOUCH
	pPainter->ReduceClippingRect(lRect);
#endif // !Touch*/

	int lTextY = 0;
	switch (GetVAlign())
	{
		case VALIGN_TOP:	lTextY = lRect.mTop;						break;
		case VALIGN_CENTER:	lTextY = lRect.mTop + (lRect.GetHeight() - mTextHeight) / 2;	break;
		case VALIGN_BOTTOM:	lTextY = lRect.mBottom - mTextHeight;				break;
	}

	int lTextX = lRect.mLeft + mHorizontalMargin;
	switch (mIconAlignment)
	{
		case ICON_CENTER:
			lTextX = lRect.GetCenterX() - pPainter->GetStringWidth(mText)/2;
		break;
		case ICON_LEFT:
			if (mIconId == Painter::INVALID_IMAGEID)
			{
				// No icon, but left-aligned indication means text should be right-aligned.
				lTextX = lRect.mRight - pPainter->GetStringWidth(mText);
			}
		break;
	}

	if (mIconId != Painter::INVALID_IMAGEID)
	{
		PixelCoord lImageSize(lIMan->GetImageSize(mIconId));

		int x = 0;
		int y = 0;
		switch (mIconAlignment)
		{
			case ICON_LEFT:
				x = lRect.mLeft;
				y = lRect.mTop + (lRect.GetHeight() - lImageSize.y) / 2;
				lTextX = lRect.mLeft + lImageSize.x + mHorizontalMargin;
			break;
			case ICON_CENTER:
				x = lRect.mLeft + (lRect.GetWidth()  - lImageSize.x) / 2;
				if (!mText.empty())
				{
					y = lRect.mTop;
				}
				else
				{
					y = lRect.GetCenterY() - lImageSize.y/2;
				}
			break;
			case ICON_RIGHT:
				x = lRect.mRight - lImageSize.x - mHorizontalMargin;
				y = lRect.mTop + (lRect.GetHeight() - lImageSize.y) / 2;
			break;
		}
		lIMan->DrawImage(mIconId, x, y);
	}

	RepaintComponents(pPainter);

	pPainter->SetColor(GetTextColor(), 0);
	pPainter->PrintText(mText, lTextX, lTextY);

	DeactivateFont(pPainter);
}

void Label::ForceRepaint()
{
	SetNeedsRepaint(true);
}

PixelCoord Label::GetPreferredSize(bool pForceAdaptive)
{
	GUIImageManager* lIMan = GetImageManager();

	PixelCoord lIconSize(0, 0);
	if (mIconId != Painter::INVALID_IMAGEID)
	{
		lIconSize = lIMan->GetImageSize(mIconId);
	}

	PixelCoord lSize(Parent::GetPreferredSize());

	if (pForceAdaptive == true || IsAdaptive() == true)
	{
		lSize.x = lIconSize.x + mTextWidth;
		lSize.y = std::max(lIconSize.y, mTextHeight);
	}

	return lSize;
}

Component::Type Label::GetType() const
{
	return Component::LABEL;
}

void Label::UpdateBackground()
{
}



}
