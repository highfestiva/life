/*
	Class:  CheckButton
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiCheckButton.h"

namespace UiTbc
{

Painter::ImageID CheckButton::smCheckIconID = Painter::INVALID_IMAGEID;
Painter* CheckButton::smPrevPainter = 0;

const uint8 CheckButton::smIconCheck[] =
{
	0  , 0  , 0  , 0  , 0  , 0  , 255, 0  ,
	0  , 0  , 0  , 0  , 0  , 255, 255, 0  ,
	255, 0  , 0  , 0  , 255, 255, 255, 0  ,
	255, 255, 0  , 255, 255, 255, 0  , 0  ,
	255, 255, 255, 255, 255, 0  , 0  , 0  ,
	0  , 255, 255, 255, 0  , 0  , 0  , 0  ,
	0  , 0  , 255, 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
};


CheckButton::CheckButton(const Color& pBodyColor, const str& pName) :
	Button(pName),
	mUserDefinedGfx(false),
	mUpdateSize(true),
	mBodyColor(pBodyColor),
	mLightColor(pBodyColor + Color(255 - pBodyColor.mRed, 255 - pBodyColor.mGreen, 255 - pBodyColor.mBlue) / 2 + 8),
	mDarkColor1(pBodyColor / 3),
	mDarkColor2(pBodyColor * (2.0f / 3.0f)),
	mReleasedID(Painter::INVALID_IMAGEID),
	mReleasingID(Painter::INVALID_IMAGEID),
	mPressedID(Painter::INVALID_IMAGEID),
	mPressingID(Painter::INVALID_IMAGEID)
{
}

CheckButton::CheckButton(Painter::ImageID pReleasedImageID,
			 Painter::ImageID pPressedImageID,
			 Painter::ImageID pReleasedActiveImageID,
			 Painter::ImageID pPressedActiveImageID,
			 Painter::ImageID pReleasingImageID,
			 Painter::ImageID pPressingImageID,
			 const str& pName) :
	Button(pName),
	mUserDefinedGfx(true),
	mUpdateSize(true),
	mBodyColor(0, 0, 0),
	mLightColor(0, 0, 0),
	mDarkColor1(0, 0, 0),
	mDarkColor2(0, 0, 0),
	mReleasedID(pReleasedImageID),
	mReleasedActiveID(pReleasedActiveImageID),
	mReleasingID(pReleasingImageID),
	mPressedID(pPressedImageID),
	mPressedActiveID(pPressedActiveImageID),
	mPressingID(pPressingImageID)
{
}

CheckButton::~CheckButton()
{
}

void CheckButton::Repaint(Painter* pPainter)
{
	const int lTextDisplacement = 4;

	GUIImageManager* lIMan = GetImageManager();

	if (mUpdateSize == true)
	{
		PixelCoords lSize(0, 0);

		if (mUserDefinedGfx == true)
		{
			switch(GetState())
			{
			case RELEASED:
				lSize = lIMan->GetImageSize(mReleasedID);
				break;
			case RELEASED_HOOVER:
				lSize = lIMan->GetImageSize(mReleasedActiveID);
				break;
			case RELEASING:
				lSize = lIMan->GetImageSize(mReleasingID);
				break;
			case PRESSED:
				lSize = lIMan->GetImageSize(mPressedID);
				break;
			case PRESSED_HOOVER:
				lSize = lIMan->GetImageSize(mPressedActiveID);
				break;
			case PRESSING:
				lSize = lIMan->GetImageSize(mPressingID);
				break;
			}
		}
		else
		{
			lSize.x = 13;
			lSize.y = 13;
		}

		lSize.x += pPainter->GetStringWidth(Button::GetText().c_str()) + lTextDisplacement;

		if (pPainter->GetFontHeight() > lSize.y)
		{
			lSize.y = pPainter->GetFontHeight();
		}

		Component::SetPreferredSize(lSize);

		mUpdateSize = false;
	}

	// Skip the default rendering...
	Window::Repaint(pPainter);

	pPainter->PushAttrib(Painter::ATTR_ALL);
	PixelRect lRect(Window::GetClientRect());
	pPainter->ReduceClippingRect(lRect);

	pPainter->SetRenderMode(Painter::RM_NORMAL);

	// First draw the checkbox, either as user defined images or as a standard checkbox.
	if (mUserDefinedGfx == true)
	{
		Painter::ImageID lImageID = Painter::INVALID_IMAGEID;
		switch(GetState())
		{
		case RELEASED:
			lImageID = mReleasedID;
			break;
		case RELEASED_HOOVER:
			lImageID = mReleasedActiveID;
			break;
		case RELEASING:
			lImageID = mReleasingID;
			break;
		case PRESSED:
			lImageID = mPressedID;
			break;
		case PRESSED_HOOVER:
			lImageID = mPressedActiveID;
			break;
		case PRESSING:
			lImageID = mPressingID;
			break;
		}

		if (lImageID != Painter::INVALID_IMAGEID)
		{
			PixelCoords lImageSize(lIMan->GetImageSize(lImageID));
			lIMan->DrawImage(lImageID, 
					   lRect.mLeft, 
					   lRect.mTop + (lRect.GetHeight() - lImageSize.y) / 2);
		}
	}
	else
	{
		if (smCheckIconID == Painter::INVALID_IMAGEID || smPrevPainter != pPainter)
		{
			smPrevPainter = pPainter;
			Canvas lCanvas(8, 8, Canvas::BITDEPTH_8_BIT);
			lCanvas.SetBuffer((void*)smIconCheck);
			smCheckIconID = pPainter->AddImage(&lCanvas, &lCanvas);
		}

		Color lBody(255, 255, 255);

		if (GetState() == RELEASING || GetState() == PRESSING)
		{
			lBody = mBodyColor;
		}

		PixelRect lButtonRect(0, 0, 13, 13);
		lButtonRect.Offset(lRect.mLeft,
				    lRect.mTop + (lRect.GetHeight() - lButtonRect.GetHeight()) / 2);

		pPainter->SetColor(lBody);
		pPainter->SetAlphaValue(lBody.mAlpha);
		pPainter->FillRect(lButtonRect);

		PixelRect lInnerRect(lButtonRect);

		pPainter->SetColor(mLightColor, 0);
		pPainter->SetColor(mDarkColor2, 1);
		pPainter->Draw3DRect(lInnerRect, 1, true);

		lInnerRect.Shrink(1);
		pPainter->SetColor(mBodyColor, 0);
		pPainter->SetColor(mDarkColor1, 1);
		pPainter->Draw3DRect(lInnerRect, 1, true);

		if (GetPressed() == true)
		{
			pPainter->SetRenderMode(Painter::RM_ALPHATEST);
			pPainter->SetColor(Color(0, 0, 0));
			pPainter->DrawAlphaImage(smCheckIconID,
						   lButtonRect.mLeft + 3,
						   lButtonRect.mTop + 3);
		}

		Button::PrintText(pPainter, lButtonRect.mRight + lTextDisplacement, lButtonRect.mBottom - pPainter->GetFontHeight() - 2);
	}

	pPainter->PopAttrib();
}

bool CheckButton::IsOver(int pScreenX, int pScreenY)
{
	return Window::GetClientRect().IsInside(pScreenX, pScreenY);
}

bool CheckButton::OnLButtonUp(int pMouseX, int pMouseY)
{
	if (IsOver(pMouseX, pMouseY) == true)
	{
		switch(GetState())
		{
		case RELEASING:
			SetPressed(false);
			break;
		case PRESSING:
			SetPressed(true);
			break;
		default:
			break;
		}
		if (Button::GetOnUnclickedFunctor() != 0)
		{
			Button::GetOnUnclickedFunctor()->Call(this);
		}
	}
	else
	{
		// Go back to previous state.
		switch(GetState())
		{
		case RELEASING:
			SetState(PRESSED);
			break;
		case PRESSING:
			SetState(RELEASED);
			break;
		default:
			break;
		}
	}

	ReleaseMouseFocus();
	return true;
}

void CheckButton::OnTextChanged()
{
	mUpdateSize = true;
}

} // End namespace.
