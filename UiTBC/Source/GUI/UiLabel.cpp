/*
	Class:  Label
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/GUI/UiLabel.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../Include/UiPainter.h"

namespace UiTbc
{

Label::Label(bool pSelectable, const Lepra::String& pName) :
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
	SetFont(Painter::INVALID_FONTID, Lepra::Color(0, 0, 0), Parent::NO_BLEND, 128);
}

Label::Label(const Lepra::Color& pColor, const Lepra::String& pName):
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
	SetFont(Painter::INVALID_FONTID, Lepra::Color(0, 0, 0), Parent::NO_BLEND, 128);
}

Label::Label(const Lepra::Color& pTopLeftColor,
	     const Lepra::Color& pTopRightColor,
	     const Lepra::Color& pBottomRightColor,
	     const Lepra::Color& pBottomLeftColor,
	     const Lepra::String& pName) :
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
	SetFont(Painter::INVALID_FONTID, Lepra::Color(0, 0, 0), Parent::NO_BLEND, 128);
}

Label::Label(Painter::ImageID pImageID, const Lepra::String& pName):
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
	SetFont(Painter::INVALID_FONTID, Lepra::Color(0, 0, 0), Parent::NO_BLEND, 128);
}

Label::Label(const Lepra::Color& pColor, const Lepra::Color& pSelectedColor, const Lepra::String& pName):
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
	SetFont(Painter::INVALID_FONTID, Lepra::Color(0, 0, 0), Parent::NO_BLEND, 128);

	mColor[0] = pColor;
	mSelectedColor[0] = pSelectedColor;
}

Label::Label(const Lepra::Color& pTopLeftColor,
		const Lepra::Color& pTopRightColor,
		const Lepra::Color& pBottomRightColor,
		const Lepra::Color& pBottomLeftColor,
		const Lepra::Color& pSelectedTopLeftColor,
		const Lepra::Color& pSelectedTopRightColor,
		const Lepra::Color& pSelectedBottomRightColor,
		const Lepra::Color& pSelectedBottomLeftColor,
		const Lepra::String& pName) :
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
	SetFont(Painter::INVALID_FONTID, Lepra::Color(0, 0, 0), Parent::NO_BLEND, 128);

	mColor[0] = pTopLeftColor;
	mColor[1] = pTopRightColor;
	mColor[2] = pBottomRightColor;
	mColor[3] = pBottomLeftColor;
	mSelectedColor[0] = pSelectedTopLeftColor;
	mSelectedColor[1] = pSelectedTopRightColor;
	mSelectedColor[2] = pSelectedBottomRightColor;
	mSelectedColor[3] = pSelectedBottomLeftColor;
}

Label::Label(Painter::ImageID pImageID, Painter::ImageID pSelectedImageID, Painter::ImageID pLCLImageID, Painter::ImageID pLCLSelectedImageID, const Lepra::String& pName) :
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
	SetFont(Painter::INVALID_FONTID, Lepra::Color(0, 0, 0), Parent::NO_BLEND, 128);
}


Label::~Label()
{
}

void Label::Repaint(Painter* pPainter)
{
	pPainter->SetActiveFont(GetFontId());

	if (mTextWidth < 0 || mTextHeight < 0)
	{
		mTextWidth  = pPainter->GetStringWidth(_T(" ")+mText);
		mTextHeight = pPainter->GetLineHeight();
	}

	GUIImageManager* lIMan = GetImageManager();

	RectComponent::Repaint(pPainter);

	pPainter->PushAttrib(Painter::ATTR_ALL);

	Lepra::PixelCoords lPos(GetScreenPos());
	Lepra::PixelRect lRect(lPos, lPos + GetSize());
	pPainter->ReduceClippingRect(lRect);

	int lFontHeight = pPainter->GetLineHeight();
	int lTextX = lRect.mLeft;
	int lTextY = lRect.mTop + (lRect.GetHeight() - lFontHeight) / 2;

	if (mIconID != Painter::INVALID_IMAGEID)
	{
		Lepra::PixelCoords lIconSize(lIMan->GetImageSize(mIconID));

		int lIconY = lTextY + lFontHeight - lIconSize.y + 1;
			//lRect.mTop + (lRect.GetHeight() - lIconSize.y) / 2;

		lIMan->DrawImage(mIconID, lRect.mLeft, lIconY);
		lTextX = lRect.mLeft + lIconSize.x;
	}

	switch(GetTextBlendFunc())
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

	pPainter->SetAlphaValue(GetTextAlpha());

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
		pPainter->SetColor(Lepra::WHITE);
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

Lepra::PixelCoords Label::GetPreferredSize(bool pForceAdaptive)
{
	GUIImageManager* lIMan = GetImageManager();

	Lepra::PixelCoords lIconSize(0, 0);
	if (mIconID != Painter::INVALID_IMAGEID)
	{
		lIconSize = lIMan->GetImageSize(mIconID);
	}

	Lepra::PixelCoords lSize(RectComponent::GetPreferredSize());

	if (pForceAdaptive == true || IsAdaptive() == true)
	{
		int lContentSizeX = lIconSize.x + mTextWidth;
		int lContentSizeY = max(lIconSize.y, mTextHeight);
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


void Label::SetText(const Lepra::String& pText,
			Painter::FontID pFontID,
			const Lepra::Color& pTextColor,
			const Lepra::Color& pBackgColor,
			BlendFunc pBlendFunc,
			Lepra::uint8 pAlphaTreshold,
			Painter* pPainter)
{
	SetFont(pFontID, pTextColor, pBlendFunc, pAlphaTreshold);
	mText                   = pText;
	mTextBackgColor         = pBackgColor;
	mSelectedTextColor      = pTextColor;
	mSelectedTextBackgColor = pBackgColor;

	if (pPainter != 0)
	{
		Painter::FontID lFontID = pPainter->GetCurrentFont();
		pPainter->SetActiveFont(GetFontId());
		mTextWidth = pPainter->GetStringWidth(_T(" ")+mText);
		mTextHeight = pPainter->GetLineHeight();
		pPainter->SetActiveFont(lFontID);
	}
	else
	{
		mTextWidth = -1;
		mTextHeight = -1;
	}
}

void Label::SetText(const Lepra::String& pText,
			Painter::FontID pFontID,
			const Lepra::Color& pTextColor,
			const Lepra::Color& pBackgColor,
			const Lepra::Color& pSelectedTextColor,
			const Lepra::Color& pSelectedBackgColor,
			BlendFunc pBlendFunc,
			Lepra::uint8 pAlphaTreshold,
			Painter* pPainter)
{
	SetFont(pFontID, pTextColor, pBlendFunc, pAlphaTreshold);
	mText                   = pText;
	mTextBackgColor         = pBackgColor;
	mSelectedTextColor      = pSelectedTextColor;
	mSelectedTextBackgColor = pSelectedBackgColor;

	if (pPainter != 0)
	{
		Painter::FontID lFontID = pPainter->GetCurrentFont();
		pPainter->SetActiveFont(GetFontId());
		mTextWidth = pPainter->GetStringWidth(_T(" ")+mText);
		mTextHeight = pPainter->GetLineHeight();
		pPainter->SetActiveFont(lFontID);
	}
	else
	{
		// We can't initialize the text dimensions. This can result in wrong
		// layouts and disturbing "corrections" when the user interacts with
		// the GUI. A seconds attempt to fix this is made below in
		// OnConnectedToDesktopWindow().
		mTextWidth = -1;
		mTextHeight = -1;
	}
}

void Label::OnConnectedToDesktopWindow()
{
	// If we have uninitialized text dimensions, try to initialize again.
	if(mTextWidth < 0 || mTextHeight < 0)
	{
		DesktopWindow* lDesktopWindow = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);

		// Hopefully there is a Painter...
		Painter* lPainter = lDesktopWindow->GetPainter();
		if (lPainter != 0)
		{
			// Yes! We can initialize the text dimensions! =)
			Painter::FontID lFontID = lPainter->GetCurrentFont();
			lPainter->SetActiveFont(GetFontId());
			mTextWidth = lPainter->GetStringWidth(_T(" ")+mText);
			mTextHeight = lPainter->GetLineHeight();
			lPainter->SetActiveFont(lFontID);
		}
	}
}

const Lepra::String& Label::GetText() const
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



}
