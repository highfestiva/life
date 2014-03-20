
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/GUI/UiRectComponent.h"
#include "../../Include/GUI/UiDesktopWindow.h"



namespace UiTbc
{



RectComponent::RectComponent(const str& pName, Layout* pLayout):
	Component(pName, pLayout),
	mShaded(false),
	mHollow(true),
	mBehaveSolid(false),
	mImageID(Painter::INVALID_IMAGEID),
	mCornerRadius(0),
	mCornerRadiusMask(0xF)
{
}

RectComponent::RectComponent(const Color& pColor, const str& pName, Layout* pLayout):
	Component(pName, pLayout),
	mShaded(false),
	mHollow(false),
	mBehaveSolid(false),
	mImageID(Painter::INVALID_IMAGEID),
	mCornerRadius(0),
	mCornerRadiusMask(0xF)
{
	mColor[0] = pColor;
	mColor[1].Set(0, 0, 0, 0);
	mColor[2].Set(0, 0, 0, 0);
	mColor[3].Set(0, 0, 0, 0);
}

RectComponent::RectComponent(const Color& pTopLeftColor,
			     const Color& pTopRightColor,
			     const Color& pBottomRightColor,
			     const Color& pBottomLeftColor,
			     const str& pName,
			     Layout* pLayout):
	Component(pName, pLayout),
	mShaded(true),
	mHollow(false),
	mBehaveSolid(false),
	mCornerRadius(0),
	mCornerRadiusMask(0xF),
	mImageID(Painter::INVALID_IMAGEID)
{
	mColor[0] = pTopLeftColor;
	mColor[1] = pTopRightColor;
	mColor[2] = pBottomRightColor;
	mColor[3] = pBottomLeftColor;
}

RectComponent::RectComponent(Painter::ImageID pImageID, const str& pName, Layout* pLayout):
	Component(pName, pLayout),
	mShaded(false),
	mHollow(false),
	mBehaveSolid(false),
	mCornerRadius(0),
	mCornerRadiusMask(0xF),
	mImageID(pImageID)
{
	mColor[0].Set(0, 0, 0, 0);
	mColor[1].Set(0, 0, 0, 0);
	mColor[2].Set(0, 0, 0, 0);
	mColor[3].Set(0, 0, 0, 0);
}

RectComponent::~RectComponent()
{
}

void RectComponent::RepaintBackground(Painter* pPainter)
{
	pPainter->PushAttrib(Painter::ATTR_ALL);

	GUIImageManager* lIMan = GetImageManager();

	PixelCoord lPos(GetScreenPos());
	PixelCoord lSize(GetSize());
	PixelRect lRect(lPos, lPos + lSize);

	PixelRect lClippingRect(lRect);
	lClippingRect.mRight++;
	lClippingRect.mBottom++;
	//pPainter->ReduceClippingRect(lClippingRect);

	if (mHollow == false)
	{
		if (mCornerRadius == 0)
		{
			if (mImageID == Painter::INVALID_IMAGEID)
			{
				pPainter->SetColor(mColor[0], 0);
				pPainter->SetAlphaValue(mColor[0].mAlpha);
				if (mShaded == true)
				{
					pPainter->SetColor(mColor[1], 1);
					pPainter->SetColor(mColor[2], 2);
					pPainter->SetColor(mColor[3], 3);
					pPainter->FillShadedRect(lRect);
				}
				else
				{
					if (mColor[0].mAlpha != 0)
					{
						pPainter->FillRect(lRect);
					}
				}
			}
			else
			{
				lIMan->DrawImage(mImageID, lRect);
			}
		}
		else	// Draw with rounded corners.
		{
			pPainter->SetColor(mColor[0], 0);
			pPainter->SetAlphaValue(mColor[0].mAlpha);
			PixelRect lRect(GetScreenPos(), GetScreenPos() + GetSize());
			pPainter->DrawRoundedRect(lRect, mCornerRadius, mCornerRadiusMask, true);
		}
	}

	pPainter->PopAttrib();
}

bool RectComponent::IsOver(int pScreenX, int pScreenY)
{
	if (mBehaveSolid == true)
	{
		return Component::IsOver(pScreenX, pScreenY);
	}

	if (Component::IsOver(pScreenX, pScreenY) == false)
	{
		return false;
	}

	if (mImageID == Painter::INVALID_IMAGEID)
	{
		if (mHollow == true)
		{
			Component* lChild = Component::GetChild(pScreenX, pScreenY);
			if (lChild != 0)
			{
				return lChild->IsOver(pScreenX, pScreenY);
			}
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		PixelCoord lPos(GetScreenPos());
		PixelCoord lSize(GetSize());
		PixelRect lRect(lPos, lPos + lSize);

		GUIImageManager* lIMan = GetImageManager();

		return lIMan->IsOverImage(mImageID, pScreenX, pScreenY, lRect);
	}
}



void RectComponent::SetIsHollow(bool pIsHollow)
{
	mHollow = pIsHollow;
}



void RectComponent::SetImage(Painter::ImageID pImageID)
{
	mImageID = pImageID;
	SetNeedsRepaint(true);
}

void RectComponent::SetColor(const Color& pColor)
{
	mShaded = false;
	SetNeedsRepaint(mColor[0] != pColor);
	mColor[0] = pColor;
	if (mColor[0].mAlpha == 0)
	{
		mHollow = true;
	}
}

void RectComponent::SetColor(const Color& pTopLeftColor, const Color& pTopRightColor, const Color& pBottomRightColor, const Color& pBottomLeftColor)
{
	mShaded = true;
	SetNeedsRepaint(mColor[0] != pTopLeftColor || mColor[1] != pTopRightColor || 
			mColor[2] != pBottomRightColor || mColor[3] != pBottomLeftColor);
	mColor[0] = pTopLeftColor;
	mColor[1] = pTopRightColor;
	mColor[2] = pBottomRightColor;
	mColor[3] = pBottomLeftColor;
	if (mColor[0].mAlpha == 0)
	{
		mHollow = true;
	}
}

Painter::ImageID RectComponent::GetImage()
{
	return mImageID;
}

Color RectComponent::GetColor()
{
	return mColor[0];
}

void RectComponent::GetColor(Color& pTopLeftColor,
			     Color& pTopRightColor,
			     Color& pBottomRightColor,
			     Color& pBottomLeftColor)
{
	pTopLeftColor     = mColor[0];
	pTopRightColor    = mColor[1];
	pBottomRightColor = mColor[2];
	pBottomLeftColor  = mColor[3];
}

Component::Type RectComponent::GetType() const
{
	return Component::RECTCOMPONENT;
}

bool RectComponent::IsHollow()
{
	return mHollow;
}

bool RectComponent::IsShaded()
{
	return mShaded;
}

void RectComponent::SetBehaveSolid(bool pBehaveSolid)
{
	mBehaveSolid = pBehaveSolid;
}

bool RectComponent::GetBehaveSolid() const
{
	return mBehaveSolid;
}



int RectComponent::GetCornerRadius() const
{
	return mCornerRadius;
}

void RectComponent::SetCornerRadius(int pRadius)
{
	mCornerRadius = pRadius;
}

void RectComponent::SetCornerRadiusMask(int pMask)
{
	mCornerRadiusMask = pMask;
}



}
