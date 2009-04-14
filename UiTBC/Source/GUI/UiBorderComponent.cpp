/*
	Class:  BorderComponent
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/GUI/UiBorderComponent.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../Include/UiMouseTheme.h"

#include <math.h>

namespace UiTbc
{

BorderComponent::BorderComponent(BorderPart pPart, const Lepra::Color& pColor, BorderShadeFunc pShadeFunc,
	const Lepra::String& pName):
	RectComponent(pColor, pName),
	mImageBorder(false),
	mResizable(false),
	mResizing(false),
	mPart(pPart),
	mShading(pShadeFunc),
	mBodyColor(pColor)
{
	Set(pColor, pShadeFunc);
}

BorderComponent::BorderComponent(BorderPart pPart, Painter::ImageID pImageID, const Lepra::String& pName):
	RectComponent(pImageID, pName),
	mImageBorder(true),
	mResizable(false),
	mResizing(false),
	mPart(pPart),
	mShading(LINEAR)
{
	GUIImageManager* lIMan = GetImageManager();
	Lepra::PixelCoords lImageSize(lIMan->GetImageSize(pImageID));

	switch(mPart)
	{
	case TOPLEFT_CORNER:
	case TOPRIGHT_CORNER:
	case BOTTOMRIGHT_CORNER:
	case BOTTOMLEFT_CORNER:
		SetPreferredSize(lImageSize.x, lImageSize.y);
		break;
	case TOP_BORDER:
	case BOTTOM_BORDER:
		SetPreferredSize(0, lImageSize.y);
		break;
	case LEFT_BORDER:
	case RIGHT_BORDER:
		SetPreferredSize(lImageSize.x, 0);
		break;
	}
}

BorderComponent::~BorderComponent()
{
}

void BorderComponent::SetSunken(bool pSunken)
{
	CalcColors();
	if (pSunken == true)
	{
		Lepra::Color lTemp(mLightColor);
		mLightColor = mDarkColor;
		mDarkColor = lTemp;
	}
}

void BorderComponent::Set(const Lepra::Color& pColor, BorderShadeFunc pShadeFunc)
{
	mBodyColor = pColor;
	mShading = pShadeFunc;

	CalcColors();
	RectComponent::SetColor(pColor);

	mImageBorder = false;
}

void BorderComponent::Set(Painter::ImageID pImageID)
{
	RectComponent::SetImage(pImageID);
	mImageBorder = true;
}

void BorderComponent::CalcColors()
{
	mLightColor = mBodyColor + Lepra::Color(
		(Lepra::uint8)((256-(int)mBodyColor.mRed)/2),
		(Lepra::uint8)((256-(int)mBodyColor.mGreen)/2),
		(Lepra::uint8)((256-(int)mBodyColor.mBlue)/2),
		mBodyColor.mAlpha) + 8;
	mDarkColor = mBodyColor / 3.0f;
}

void BorderComponent::Repaint(Painter* pPainter)
{
	if (mImageBorder == true)
	{
		Parent::Repaint(pPainter);
	}
	else
	{
		Lepra::PixelRect lRect(GetScreenPos(), GetScreenPos() + GetSize());

		pPainter->PushAttrib(Painter::ATTR_ALL);
		pPainter->SetAlphaValue(mBodyColor.mAlpha);

		switch(mPart)
		{
			case TOPLEFT_CORNER:
			{
				Lepra::PixelCoords lTopLeft(lRect.mLeft, lRect.mTop);
				Lepra::PixelCoords lTopRight(lRect.mRight, lRect.mTop);
				Lepra::PixelCoords lBottomLeft(lRect.mLeft, lRect.mBottom);
				Lepra::PixelCoords lBottomRight(lRect.mRight, lRect.mBottom);

				if (mShading == LINEAR)
				{
					pPainter->SetColor(mLightColor, 0);
					pPainter->SetColor(mLightColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->FillShadedTriangle(lTopLeft, lTopRight, lBottomRight);
					pPainter->FillShadedTriangle(lBottomLeft, lTopLeft, lBottomRight);
				}
				else
				{
					Lepra::PixelCoords lMiddle(lRect.GetCenterX(), lRect.GetCenterY());
					Lepra::PixelCoords lMiddleBottom(lRect.GetCenterX(), lRect.mBottom);
					Lepra::PixelCoords lMiddleRight(lRect.mRight, lRect.GetCenterY());

					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mLightColor, 2);

					pPainter->FillShadedTriangle(lBottomLeft, lTopLeft, lMiddle);
					pPainter->FillShadedTriangle(lTopLeft, lTopRight, lMiddle);

					pPainter->SetColor(mLightColor, 1);

					pPainter->FillShadedTriangle(lBottomLeft, lMiddle, lMiddleBottom);
					pPainter->FillShadedTriangle(lTopRight, lMiddleRight, lMiddle);
					pPainter->FillShadedTriangle(lBottomRight, lMiddle, lMiddleRight);
					pPainter->FillShadedTriangle(lBottomRight, lMiddleBottom, lMiddle);
				}
			}
			break;
			case TOPRIGHT_CORNER:
			{
				Lepra::PixelCoords lTopLeft(lRect.mLeft, lRect.mTop);
				Lepra::PixelCoords lTopRight(lRect.mRight, lRect.mTop);
				Lepra::PixelCoords lBottomLeft(lRect.mLeft, lRect.mBottom);
				Lepra::PixelCoords lBottomRight(lRect.mRight, lRect.mBottom);

				if (mShading == LINEAR)
				{
					pPainter->SetColor(mLightColor, 0);
					pPainter->SetColor(mLightColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->FillShadedTriangle(lTopLeft, lTopRight, lBottomLeft);

					pPainter->SetColor(mDarkColor, 0);
					pPainter->SetColor(mDarkColor, 1);
					pPainter->FillShadedTriangle(lTopRight, lBottomRight, lBottomLeft);
				}
				else
				{
					Lepra::PixelCoords lMiddle(lRect.GetCenterX(), lRect.GetCenterY());
					Lepra::PixelCoords lMiddleBottom(lRect.GetCenterX(), lRect.mBottom);
					Lepra::PixelCoords lMiddleLeft(lRect.mLeft, lRect.GetCenterY());

					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mLightColor, 2);

					pPainter->FillShadedTriangle(lTopLeft, lTopRight, lMiddle);

					pPainter->SetColor(mDarkColor, 2);
					pPainter->FillShadedTriangle(lTopRight, lBottomRight, lMiddle);

					pPainter->SetColor(mDarkColor, 1);

					pPainter->FillShadedTriangle(lBottomRight, lMiddleBottom, lMiddle);
					pPainter->FillShadedTriangle(lBottomLeft, lMiddle, lMiddleBottom);

					pPainter->SetColor(mLightColor, 1);
					pPainter->SetColor(mLightColor, 2);
					pPainter->FillShadedTriangle(lBottomLeft, lMiddleLeft, lMiddle);
					pPainter->FillShadedTriangle(lTopLeft, lMiddle, lMiddleLeft);
				}
			}
			break;
			case BOTTOMRIGHT_CORNER:
			{
				Lepra::PixelCoords lTopLeft(lRect.mLeft, lRect.mTop);
				Lepra::PixelCoords lTopRight(lRect.mRight, lRect.mTop);
				Lepra::PixelCoords lBottomLeft(lRect.mLeft, lRect.mBottom);
				Lepra::PixelCoords lBottomRight(lRect.mRight, lRect.mBottom);

				if (mShading == LINEAR)
				{
					pPainter->SetColor(mDarkColor, 0);
					pPainter->SetColor(mDarkColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->FillShadedTriangle(lBottomRight, lBottomLeft, lTopLeft);
					pPainter->FillShadedTriangle(lTopRight, lBottomRight, lTopLeft);
				}
				else
				{
					Lepra::PixelCoords lMiddle(lRect.GetCenterX(), lRect.GetCenterY());
					Lepra::PixelCoords lMiddleTop(lRect.GetCenterX(), lRect.mTop);
					Lepra::PixelCoords lMiddleLeft(lRect.mLeft, lRect.GetCenterY());

					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mDarkColor, 2);

					pPainter->FillShadedTriangle(lBottomRight, lBottomLeft, lMiddle);
					pPainter->FillShadedTriangle(lTopRight, lBottomRight, lMiddle);

					pPainter->SetColor(mDarkColor, 1);

					pPainter->FillShadedTriangle(lBottomLeft, lMiddleLeft, lMiddle);
					pPainter->FillShadedTriangle(lTopLeft, lMiddle, lMiddleLeft);
					pPainter->FillShadedTriangle(lTopLeft, lMiddleTop, lMiddle);
					pPainter->FillShadedTriangle(lTopRight, lMiddle, lMiddleTop);
				}
			}
			break;
			case BOTTOMLEFT_CORNER:
			{
				Lepra::PixelCoords lTopLeft(lRect.mLeft, lRect.mTop);
				Lepra::PixelCoords lTopRight(lRect.mRight, lRect.mTop);
				Lepra::PixelCoords lBottomLeft(lRect.mLeft, lRect.mBottom);
				Lepra::PixelCoords lBottomRight(lRect.mRight, lRect.mBottom);

				if (mShading == LINEAR)
				{
					pPainter->SetColor(mLightColor, 0);
					pPainter->SetColor(mLightColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->FillShadedTriangle(lBottomLeft, lTopLeft, lTopRight);

					pPainter->SetColor(mDarkColor, 0);
					pPainter->SetColor(mDarkColor, 1);
					pPainter->FillShadedTriangle(lBottomRight, lBottomLeft, lTopRight);
				}
				else
				{
					Lepra::PixelCoords lMiddle(lRect.GetCenterX(), lRect.GetCenterY());
					Lepra::PixelCoords lMiddleTop(lRect.GetCenterX(), lRect.mTop);
					Lepra::PixelCoords lMiddleRight(lRect.mRight, lRect.GetCenterY());

					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mLightColor, 2);

					pPainter->FillShadedTriangle(lBottomLeft, lTopLeft, lMiddle);

					pPainter->SetColor(mDarkColor, 2);
					pPainter->FillShadedTriangle(lBottomRight, lBottomLeft, lMiddle);

					pPainter->SetColor(mDarkColor, 1);

					pPainter->FillShadedTriangle(lBottomRight, lMiddle, lMiddleRight);
					pPainter->FillShadedTriangle(lTopRight, lMiddleRight, lMiddle);

					pPainter->SetColor(mLightColor, 1);
					pPainter->SetColor(mLightColor, 2);
					pPainter->FillShadedTriangle(lTopLeft, lMiddleTop, lMiddle);
					pPainter->FillShadedTriangle(lTopRight, lMiddle, lMiddleTop);
				}
			}
			break;
			case TOP_BORDER:
			{
				if (mShading == LINEAR)
				{
					pPainter->SetColor(mLightColor, 0);
					pPainter->SetColor(mLightColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->SetColor(mBodyColor, 3);
					pPainter->FillShadedRect(lRect);
				}
				else
				{
					int lMiddleY = lRect.GetCenterY();
					Lepra::PixelRect lTopRect(lRect.mLeft, lRect.mTop, lRect.mRight, lMiddleY);
					Lepra::PixelRect lBottomRect(lRect.mLeft, lMiddleY, lRect.mRight, lRect.mBottom);

					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mLightColor, 2);
					pPainter->SetColor(mLightColor, 3);
					pPainter->FillShadedRect(lTopRect);

					pPainter->SetColor(mLightColor, 0);
					pPainter->SetColor(mLightColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->SetColor(mBodyColor, 3);
					pPainter->FillShadedRect(lBottomRect);
				}
			}
			break;
			case BOTTOM_BORDER:
			{
				if (mShading == LINEAR)
				{
					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mDarkColor, 2);
					pPainter->SetColor(mDarkColor, 3);
					pPainter->FillShadedRect(lRect);
				}
				else
				{
					int lMiddleY = lRect.GetCenterY();
					Lepra::PixelRect lTopRect(lRect.mLeft, lRect.mTop, lRect.mRight, lMiddleY);
					Lepra::PixelRect lBottomRect(lRect.mLeft, lMiddleY, lRect.mRight, lRect.mBottom);

					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mDarkColor, 2);
					pPainter->SetColor(mDarkColor, 3);
					pPainter->FillShadedRect(lTopRect);

					pPainter->SetColor(mDarkColor, 0);
					pPainter->SetColor(mDarkColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->SetColor(mBodyColor, 3);
					pPainter->FillShadedRect(lBottomRect);
				}
			}
			break;
			case LEFT_BORDER:
			{
				if (mShading == LINEAR)
				{
					pPainter->SetColor(mLightColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->SetColor(mLightColor, 3);
					pPainter->FillShadedRect(lRect);
				}
				else
				{
					int lMiddleX = lRect.GetCenterX();
					Lepra::PixelRect lLeftRect(lRect.mLeft, lRect.mTop, lMiddleX, lRect.mBottom);
					Lepra::PixelRect lRightRect(lMiddleX, lRect.mTop, lRect.mRight, lRect.mBottom);

					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mLightColor, 1);
					pPainter->SetColor(mLightColor, 2);
					pPainter->SetColor(mBodyColor, 3);
					pPainter->FillShadedRect(lLeftRect);

					pPainter->SetColor(mLightColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->SetColor(mLightColor, 3);
					pPainter->FillShadedRect(lRightRect);
				}
			}
			break;
			case RIGHT_BORDER:
			{
				if (mShading == LINEAR)
				{
					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mDarkColor, 1);
					pPainter->SetColor(mDarkColor, 2);
					pPainter->SetColor(mBodyColor, 3);
					pPainter->FillShadedRect(lRect);
				}
				else
				{
					int lMiddleX = lRect.GetCenterX();
					Lepra::PixelRect lLeftRect(lRect.mLeft, lRect.mTop, lMiddleX, lRect.mBottom);
					Lepra::PixelRect lRightRect(lMiddleX, lRect.mTop, lRect.mRight, lRect.mBottom);

					pPainter->SetColor(mBodyColor, 0);
					pPainter->SetColor(mDarkColor, 1);
					pPainter->SetColor(mDarkColor, 2);
					pPainter->SetColor(mBodyColor, 3);
					pPainter->FillShadedRect(lLeftRect);

					pPainter->SetColor(mDarkColor, 0);
					pPainter->SetColor(mBodyColor, 1);
					pPainter->SetColor(mBodyColor, 2);
					pPainter->SetColor(mDarkColor, 3);
					pPainter->FillShadedRect(lRightRect);
				}
			}
			break;
		}

		pPainter->PopAttrib();

		SetNeedsRepaint(false);
	}
}

bool BorderComponent::OnLButtonDown(int pMouseX, int pMouseY)
{
	if (mResizable == true && IsOver(pMouseX, pMouseY) == true)
	{
		mResizing = true;
		SetMouseFocus();
		return true;
	}
	else
	{
		return Parent::OnLButtonDown(pMouseX, pMouseY);
	}
}

bool BorderComponent::OnLButtonUp(int pMouseX, int pMouseY)
{
	if (mResizing == true)
	{
		mResizing = false;
		ReleaseMouseFocus();

		Component* lDWin = GetParentOfType(DESKTOPWINDOW);
		if (lDWin != 0)
		{
			// Call OnMouseMove() to update the mouse cursor icon.
			lDWin->OnMouseMove(pMouseX, pMouseY, 0, 0);
		}
	}

	return Parent::OnLButtonUp(pMouseX, pMouseY);
}

bool BorderComponent::OnMouseMove(int /*pMouseX*/, int /*pMouseY*/, int pMouseDX, int pMouseDY)
{
	if (mResizing == true)
	{
		Component* lWin = GetParentOfType(WINDOW);
		Lepra::PixelCoords lSize(lWin->GetSize());
		Lepra::PixelCoords lMinSize(lWin->GetMinSize());
		Lepra::PixelCoords lNewSize(lSize);
		Lepra::PixelCoords lMove(0, 0);

		if (lWin != 0)
		{
			switch(mPart)
			{
			case TOPLEFT_CORNER:
				lNewSize += Lepra::PixelCoords(-pMouseDX, -pMouseDY);
				lMove.x = pMouseDX;
				lMove.y = pMouseDY;
				break;
			case BOTTOMRIGHT_CORNER:
				lNewSize += Lepra::PixelCoords(pMouseDX, pMouseDY);
				break;
			case BOTTOMLEFT_CORNER:
				lNewSize += Lepra::PixelCoords(-pMouseDX, pMouseDY);
				lMove.x = pMouseDX;
				break;
			case TOPRIGHT_CORNER:
				lNewSize += Lepra::PixelCoords(pMouseDX, -pMouseDY);
				lMove.y = pMouseDY;
				break;
			case TOP_BORDER:
				lNewSize += Lepra::PixelCoords(0, -pMouseDY);
				lMove.y = pMouseDY;
				break;
			case BOTTOM_BORDER:
				lNewSize += Lepra::PixelCoords(0, pMouseDY);
				break;
			case LEFT_BORDER:
				lNewSize += Lepra::PixelCoords(-pMouseDX, 0);
				lMove.x = pMouseDX;
				break;
			case RIGHT_BORDER:
				lNewSize += Lepra::PixelCoords(pMouseDX, 0);
				break;
			default:
				break;
			};

			if (lNewSize.x < lMinSize.x)
			{
				if (lMove.x > 0)
				{
					lMove.x -= (lMinSize.x - lNewSize.x);
				}

				lNewSize.x = lMinSize.x;
			}

			if (lNewSize.y < lMinSize.y)
			{
				if (lMove.y > 0)
				{
					lMove.y -= (lMinSize.y - lNewSize.y);
				}

				lNewSize.y = lMinSize.y;
			}

			lWin->SetPos(lWin->GetPos() + lMove);
			lWin->SetPreferredSize(lNewSize, false);
		}

		return true;
	}
	else if(mResizable == true)
	{
		DesktopWindow* lDWin = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
		MouseTheme* lMTheme = 0;

		if (lDWin != 0)
		{
			lMTheme = lDWin->GetMouseTheme();
		}

		if (lMTheme != 0)
		{
			switch(mPart)
			{
			case TOPLEFT_CORNER:
			case BOTTOMRIGHT_CORNER:
				lMTheme->LoadDiagonal1ResizeCursor();
				break;
			case BOTTOMLEFT_CORNER:
			case TOPRIGHT_CORNER:
				lMTheme->LoadDiagonal2ResizeCursor();
				break;
			case TOP_BORDER:
			case BOTTOM_BORDER:
				lMTheme->LoadVResizeCursor();
				break;
			case LEFT_BORDER:
			case RIGHT_BORDER:
				lMTheme->LoadHResizeCursor();
				break;
			default:
				lMTheme->LoadArrowCursor();
			};
		}

		return true;
	}
	else
	{
		return false;
	}
}

} // End namespace.