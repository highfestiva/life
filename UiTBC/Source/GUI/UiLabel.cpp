
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiLabel.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../Include/UiPainter.h"



namespace UiTbc
{



Label::Label(bool pSelectable, const str& pName):
	RectComponent(pName),
	mIconID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mTextBackgColor(255, 255, 255),
	mTextWidth(-1),
	mTextHeight(-1),
	mSelectable(pSelectable),
	mImageID(Painter::INVALID_IMAGEID),
	mSelectedImageID(Painter::INVALID_IMAGEID),
	mLCLImageID(Painter::INVALID_IMAGEID),
	mLCLSelectedImageID(Painter::INVALID_IMAGEID)
{
	SetFontColor(OFF_BLACK);
}

Label::Label(const Color& pColor, const str& pName):
	RectComponent(pColor, pName),
	mIconID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mTextBackgColor(255, 255, 255),
	mTextWidth(-1),
	mTextHeight(-1),
	mSelectable(false),
	mImageID(Painter::INVALID_IMAGEID),
	mSelectedImageID(Painter::INVALID_IMAGEID),
	mLCLImageID(Painter::INVALID_IMAGEID),
	mLCLSelectedImageID(Painter::INVALID_IMAGEID)
{
	SetFontColor(OFF_BLACK);
}

Label::Label(const Color& pTopLeftColor,
	     const Color& pTopRightColor,
	     const Color& pBottomRightColor,
	     const Color& pBottomLeftColor,
	     const str& pName) :
	RectComponent(pTopLeftColor, pTopRightColor, pBottomRightColor, pBottomLeftColor, pName),
	mIconID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mTextBackgColor(255, 255, 255),
	mTextWidth(-1),
	mTextHeight(-1),
	mSelectable(false),
	mImageID(Painter::INVALID_IMAGEID),
	mSelectedImageID(Painter::INVALID_IMAGEID),
	mLCLImageID(Painter::INVALID_IMAGEID),
	mLCLSelectedImageID(Painter::INVALID_IMAGEID)
{
	SetFontColor(OFF_BLACK);
}

Label::Label(Painter::ImageID pImageID, const str& pName):
	RectComponent(pImageID, pName),
	mIconID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mTextBackgColor(255, 255, 255),
	mTextWidth(-1),
	mTextHeight(-1),
	mSelectable(false),
	mImageID(Painter::INVALID_IMAGEID),
	mSelectedImageID(Painter::INVALID_IMAGEID),
	mLCLImageID(Painter::INVALID_IMAGEID),
	mLCLSelectedImageID(Painter::INVALID_IMAGEID)
{
	SetFontColor(OFF_BLACK);
}

Label::Label(const Color& pColor, const Color& pSelectedColor, const str& pName):
	RectComponent(pColor, pName),
	mIconID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mTextBackgColor(255, 255, 255),
	mTextWidth(-1),
	mTextHeight(-1),
	mSelectable(true),
	mImageID(Painter::INVALID_IMAGEID),
	mSelectedImageID(Painter::INVALID_IMAGEID),
	mLCLImageID(Painter::INVALID_IMAGEID),
	mLCLSelectedImageID(Painter::INVALID_IMAGEID)
{
	SetFontColor(OFF_BLACK);

	mColor[0] = pColor;
	mSelectedColor[0] = pSelectedColor;
}

Label::Label(const Color& pTopLeftColor,
		const Color& pTopRightColor,
		const Color& pBottomRightColor,
		const Color& pBottomLeftColor,
		const Color& pSelectedTopLeftColor,
		const Color& pSelectedTopRightColor,
		const Color& pSelectedBottomRightColor,
		const Color& pSelectedBottomLeftColor,
		const str& pName) :
	RectComponent(pTopLeftColor, pTopRightColor, pBottomRightColor, pBottomLeftColor, pName),
	mIconID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mTextBackgColor(255, 255, 255),
	mTextWidth(-1),
	mTextHeight(-1),
	mSelectable(true),
	mImageID(Painter::INVALID_IMAGEID),
	mSelectedImageID(Painter::INVALID_IMAGEID),
	mLCLImageID(Painter::INVALID_IMAGEID),
	mLCLSelectedImageID(Painter::INVALID_IMAGEID)
{
	SetFontColor(OFF_BLACK);

	mColor[0] = pTopLeftColor;
	mColor[1] = pTopRightColor;
	mColor[2] = pBottomRightColor;
	mColor[3] = pBottomLeftColor;
	mSelectedColor[0] = pSelectedTopLeftColor;
	mSelectedColor[1] = pSelectedTopRightColor;
	mSelectedColor[2] = pSelectedBottomRightColor;
	mSelectedColor[3] = pSelectedBottomLeftColor;
}

Label::Label(Painter::ImageID pImageID, Painter::ImageID pSelectedImageID, Painter::ImageID pLCLImageID, Painter::ImageID pLCLSelectedImageID, const str& pName) :
	RectComponent(pImageID, pName),
	mIconID(Painter::INVALID_IMAGEID),
	mText(_T("")),
	mTextBackgColor(255, 255, 255),
	mTextWidth(-1),
	mTextHeight(-1),
	mSelectable(true),
	mImageID(pImageID),
	mSelectedImageID(pSelectedImageID),
	mLCLImageID(pLCLImageID),
	mLCLSelectedImageID(pLCLSelectedImageID)
{
	SetFontColor(OFF_BLACK);
}


Label::~Label()
{
}

void Label::Repaint(Painter* pPainter)
{
	SetNeedsRepaint(false);

	const int lTextWidth  = pPainter->GetStringWidth(_T(" ")+mText);
	const int lTextHeight = pPainter->GetLineHeight() * (std::count(mText.begin(), mText.end(), _T('\n')) + 1);
	if (mTextWidth != lTextWidth || mTextHeight != lTextHeight)
	{
		mTextWidth = lTextWidth;
		mTextHeight = lTextHeight;
		GetParent()->UpdateLayout();
	}

	GUIImageManager* lIMan = GetImageManager();

	RectComponent::Repaint(pPainter);

	pPainter->PushAttrib(Painter::ATTR_ALL);

	PixelCoord lPos(GetScreenPos());
	PixelRect lRect(lPos, lPos + GetSize());
	pPainter->ReduceClippingRect(lRect);

	int lTextX = lRect.mLeft;
	int lTextY = lRect.mTop + (lRect.GetHeight() - mTextHeight) / 2;

	if (mIconID != Painter::INVALID_IMAGEID)
	{
		PixelCoord lIconSize(lIMan->GetImageSize(mIconID));

		int lIconY = lTextY + mTextHeight - lIconSize.y + 1;
			//lRect.mTop + (lRect.GetHeight() - lIconSize.y) / 2;

		lIMan->DrawImage(mIconID, lRect.mLeft, lIconY);
		lTextX = lRect.mLeft + lIconSize.x;
	}

	if (GetSelected() == true)
	{
		pPainter->SetColor(mSelectedTextColor, 0);
		pPainter->SetColor(mSelectedTextBackgColor, 1);
	}
	else
	{
		pPainter->SetColor(GetTextColor(), 0);
		pPainter->SetColor(mTextBackgColor, 1);
	}

	pPainter->PrintText(_T(" ")+mText, lTextX, lTextY);

	// Finally, draw the dotted rectangle.
	if (HasKeyboardFocus() == true  && 
		((GetSelected() == false && (mImageID == Painter::INVALID_IMAGEID || mLCLImageID == Painter::INVALID_IMAGEID)) ||
		(GetSelected() == true && (mSelectedImageID == Painter::INVALID_IMAGEID || mLCLSelectedImageID == Painter::INVALID_IMAGEID))))
	{
		pPainter->SetColor(WHITE);
		pPainter->SetRenderMode(Painter::RM_XOR);

		lRect.mRight--;
		lRect.mBottom--;

		// Draw top and bottom line.
		for (int x = lRect.mLeft; x < lRect.mRight; x++)
		{
			if (((x ^ lRect.mTop) & 1) != 0)
			{
				pPainter->DrawPixel(x, lRect.mTop);
			}

			if (((x ^ lRect.mBottom) & 1) != 0)
			{
				pPainter->DrawPixel(x, lRect.mBottom);
			}
		}

		// Draw left and right lines.
		for (int y = lRect.mTop + 1; y < lRect.mBottom; y++)
		{
			if (((y ^ lRect.mLeft) & 1) != 0)
			{
				pPainter->DrawPixel(lRect.mLeft, y);
			}

			if (((y ^ lRect.mRight) & 1) != 0)
			{
				pPainter->DrawPixel(lRect.mRight, y);
			}
		}
	}

	pPainter->PopAttrib();
}

PixelCoord Label::GetPreferredSize(bool pForceAdaptive)
{
	GUIImageManager* lIMan = GetImageManager();

	PixelCoord lIconSize(0, 0);
	if (mIconID != Painter::INVALID_IMAGEID)
	{
		lIconSize = lIMan->GetImageSize(mIconID);
	}

	PixelCoord lSize(RectComponent::GetPreferredSize());

	if (pForceAdaptive == true || IsAdaptive() == true)
	{
		int lContentSizeX = lIconSize.x + mTextWidth;
		int lContentSizeY = std::max(lIconSize.y, mTextHeight);
		if (lContentSizeX > lSize.x)
		{
			lSize.x = lContentSizeX;
		}

		if (lContentSizeY > lSize.y)
		{
			lSize.y = lContentSizeY;
		}
	}

	return lSize;
}


void Label::SetText(const str& pText,
			const Color& pTextColor,
			const Color& pBackgColor,
			Painter*)
{
	SetFontColor(pTextColor);
	mText                   = pText;
	mTextBackgColor         = pBackgColor;
	mSelectedTextColor      = pTextColor;
	mSelectedTextBackgColor = pBackgColor;
}

void Label::SetText(const str& pText,
			const Color& pTextColor,
			const Color& pBackgColor,
			const Color& pSelectedTextColor,
			const Color& pSelectedBackgColor,
			Painter*)
{
	SetFontColor(pTextColor);
	mText                   = pText;
	mTextBackgColor         = pBackgColor;
	mSelectedTextColor      = pSelectedTextColor;
	mSelectedTextBackgColor = pSelectedBackgColor;
}

void Label::OnConnectedToDesktopWindow()
{
}

const str& Label::GetText() const
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

void Label::SetKeyboardFocus()
{
	bool lUpdate = !HasKeyboardFocus();

	Parent::SetKeyboardFocus();

	if (lUpdate)
	{
		UpdateBackground();
	}
}

void Label::ReleaseKeyboardFocus(Parent::RecurseDir pDir, Component* pFocusedComponent)
{
	bool lUpdate = HasKeyboardFocus();

	Parent::ReleaseKeyboardFocus(pDir, pFocusedComponent);

	if (lUpdate)
	{
		UpdateBackground();
	}
}

void Label::UpdateBackground()
{
	if (RectComponent::GetImage() != Painter::INVALID_IMAGEID)
	{
		if (GetSelected() == true)
		{
			if (HasKeyboardFocus() == true && mLCLSelectedImageID != Painter::INVALID_IMAGEID)
			{
				RectComponent::SetImage(mLCLSelectedImageID);
			}
			else
			{
				RectComponent::SetImage(mSelectedImageID);
			}
		}
		else
		{
			if (HasKeyboardFocus() == true && mLCLImageID != Painter::INVALID_IMAGEID)
			{
				RectComponent::SetImage(mLCLImageID);
			}
			else
			{
				RectComponent::SetImage(mImageID);
			}
		}
	}
	else if(RectComponent::IsShaded() == true)
	{
		if (GetSelected() == true)
		{
			RectComponent::SetColor(mSelectedColor[0], 
						mSelectedColor[1], 
						mSelectedColor[2], 
						mSelectedColor[3]);
		}
		else
		{
			RectComponent::SetColor(mColor[0], 
						mColor[1], 
						mColor[2], 
						mColor[3]);
		}
	}
	else if(RectComponent::IsHollow() == false)
	{
		if (GetSelected() == true)
		{
			RectComponent::SetColor(mSelectedColor[0]);
		}
		else
		{
			RectComponent::SetColor(mColor[0]);
		}
	}

	SetNeedsRepaint(true);
}

void Label::ForceRepaint()
{
	SetNeedsRepaint(true);
}

void Label::SetIcon(Painter::ImageID pIconID)
{
	mIconID = pIconID;
}

Component::Type Label::GetType() const
{
	return Component::LABEL;
}



}
