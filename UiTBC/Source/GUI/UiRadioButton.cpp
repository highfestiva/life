/*
	Class:  RadioButton
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiRadioButton.h"

namespace UiTbc
{

Painter::ImageID RadioButton::smIconRadioCheckID    = Painter::INVALID_IMAGEID;
Painter::ImageID RadioButton::smRadioButtonInnerID  = Painter::INVALID_IMAGEID;
Painter::ImageID RadioButton::smRadioButtonLight1ID = Painter::INVALID_IMAGEID;
Painter::ImageID RadioButton::smRadioButtonLight2ID = Painter::INVALID_IMAGEID;
Painter::ImageID RadioButton::smRadioButtonDark1ID  = Painter::INVALID_IMAGEID;
Painter::ImageID RadioButton::smRadioButtonDark2ID  = Painter::INVALID_IMAGEID;
Painter* RadioButton::smPrevPainter = 0;

const uint8 RadioButton::smIconRadioCheck[] =
{
	0  , 255, 255, 0  ,
	255, 255, 255, 255,
	255, 255, 255, 255,
	0  , 255, 255, 0  ,
};

/*
	The different parts of the icon that makes a radio button.
	They are split up like this because of the different colors
	of each part. They will be combined into one picture.
*/

const uint8 RadioButton::smRadioButtonInner[] =
{   0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 255, 255, 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 255, 255, 255, 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 255, 255, 255, 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 255, 255, 255, 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 255, 255, 255, 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 255, 255, 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
};

const uint8 RadioButton::smRadioButtonLight1Col[] =
{   0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 255, 0  , 0  , 0  , 0  , 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
};

const uint8 RadioButton::smRadioButtonLight2Col[] =
{   0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 255, 0  , 0  , 0  , 0  , 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
};

const uint8 RadioButton::smRadioButtonDark1Col[] =
{   0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 255, 0  , 0  , 0  , 0  , 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
};

const uint8 RadioButton::smRadioButtonDark2Col[] =
{   0  , 0  , 0  , 0  , 255, 255, 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 255, 255, 0  , 0  , 0  , 0  , 255, 255, 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 255, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
    0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 
};


RadioButton::RadioButton(const Color& pBodyColor, const str& pName):
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

RadioButton::RadioButton(Painter::ImageID pReleasedImageID,
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

RadioButton::~RadioButton()
{
}

void RadioButton::Repaint(Painter* pPainter)
{
	const int lTextDisplacement = 4;

	GUIImageManager* lIMan = GetImageManager();

	if (mUpdateSize == true)
	{
		PixelCoord lRect(0, 0);

		if (mUserDefinedGfx == true)
		{
			switch(GetState())
			{
			case RELEASED:
				lRect = lIMan->GetImageSize(mReleasedID);
				break;
			case RELEASED_HOOVER:
				lRect = lIMan->GetImageSize(mReleasedActiveID);
				break;
			case RELEASING:
				lRect = lIMan->GetImageSize(mReleasingID);
				break;
			case PRESSED:
				lRect = lIMan->GetImageSize(mPressedID);
				break;
			case PRESSED_HOOVER:
				lRect = lIMan->GetImageSize(mPressedActiveID);
				break;
			case PRESSING:
				lRect = lIMan->GetImageSize(mPressingID);
				break;
			}
		}
		else
		{
			lRect.x = 13;
			lRect.y = 13;
		}

		lRect.x += pPainter->GetStringWidth(Button::GetText()) + lTextDisplacement;

		if (pPainter->GetFontHeight() > lRect.y)
		{
			lRect.y = pPainter->GetFontHeight();
		}

		Component::SetPreferredSize(lRect);

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
			PixelCoord lImageSize(lIMan->GetImageSize(lImageID));
			lIMan->DrawImage(lImageID, 
					   lRect.mLeft, 
					   lRect.mTop + (lRect.GetHeight() - lImageSize.y) / 2);
		}
	}
	else
	{
		const int lButtonWidth = 12;

		pPainter->SetRenderMode(Painter::RM_ALPHATEST);

		AddImageToPainter(smIconRadioCheckID, smIconRadioCheck, 4, pPainter);
		AddImageToPainter(smRadioButtonInnerID, smRadioButtonInner, 16, pPainter);
		AddImageToPainter(smRadioButtonLight1ID, smRadioButtonLight1Col, 16, pPainter);
		AddImageToPainter(smRadioButtonLight2ID, smRadioButtonLight2Col, 16, pPainter);
		AddImageToPainter(smRadioButtonDark1ID, smRadioButtonDark1Col, 16, pPainter);
		AddImageToPainter(smRadioButtonDark2ID, smRadioButtonDark2Col, 16, pPainter);

		Color lBody(255, 255, 255);

		if (GetState() == RELEASING || GetState() == PRESSING)
		{
			lBody = mBodyColor;
		}

		PixelCoord lButtonPos(lRect.mLeft, lRect.mTop + (lRect.GetHeight() - lButtonWidth) / 2);

		pPainter->SetColor(lBody);
		pPainter->DrawAlphaImage(smRadioButtonInnerID, lButtonPos);

		pPainter->SetColor(mBodyColor);
		pPainter->DrawAlphaImage(smRadioButtonLight1ID, lButtonPos);

		pPainter->SetColor(mLightColor);
		pPainter->DrawAlphaImage(smRadioButtonLight2ID, lButtonPos);

		pPainter->SetColor(mDarkColor1);
		pPainter->DrawAlphaImage(smRadioButtonDark1ID, lButtonPos);

		pPainter->SetColor(mDarkColor2);
		pPainter->DrawAlphaImage(smRadioButtonDark2ID, lButtonPos);

		if (GetPressed() == true)
		{
			pPainter->SetRenderMode(Painter::RM_ALPHATEST);
			pPainter->SetColor(Color(0, 0, 0));
			pPainter->DrawAlphaImage(smIconRadioCheckID,
						   lButtonPos.x + 4,
						   lButtonPos.y + 4);
		}

		Button::PrintText(pPainter, lButtonPos.x + lButtonWidth + lTextDisplacement, (lButtonPos.y + lButtonWidth) - pPainter->GetFontHeight() - 2);
	}

	pPainter->PopAttrib();
}

void RadioButton::AddImageToPainter(Painter::ImageID& pImageID, const uint8 pImage[], int pDim, Painter* pPainter)
{
	if (smPrevPainter != pPainter)
	{
		// The painter has changed. All image ids are now invalid.
		smIconRadioCheckID    = Painter::INVALID_IMAGEID;
		smRadioButtonInnerID  = Painter::INVALID_IMAGEID;
		smRadioButtonLight1ID = Painter::INVALID_IMAGEID;
		smRadioButtonLight2ID = Painter::INVALID_IMAGEID;
		smRadioButtonDark1ID  = Painter::INVALID_IMAGEID;
		smRadioButtonDark2ID  = Painter::INVALID_IMAGEID;

		smPrevPainter = pPainter;
	}

	if (pImageID == Painter::INVALID_IMAGEID)
	{
		Canvas lCanvas(pDim, pDim, Canvas::BITDEPTH_8_BIT);
		lCanvas.SetBuffer((void*)pImage);
		pImageID = pPainter->AddImage(&lCanvas, &lCanvas);
	}
}

bool RadioButton::IsOver(int pScreenX, int pScreenY)
{
	return Window::GetClientRect().IsInside(pScreenX, pScreenY);
}

bool RadioButton::OnLButtonUp(int pMouseX, int pMouseY)
{
	if (IsOver(pMouseX, pMouseY) == true)
	{
		Component* lParent = GetParent();
		Layout* lLayout = lParent->GetLayout();

		// Update all the other radio buttons.
		Component* lChild = lLayout->GetFirst();
		while (lChild != 0)
		{
			if (lChild != this && lChild->GetType() == Component::RADIOBUTTON)
			{
				RadioButton* lButton = (RadioButton*)lChild;
				lButton->SetPressed(false);
			}

			lChild = lLayout->GetNext();
		}

		SetPressed(true);
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

void RadioButton::OnTextChanged()
{
	mUpdateSize = true;
}

} // End namespace.
