/*
	Class:  RectComponent
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiRectComponent.h"
#include "../../Include/GUI/UiDesktopWindow.h"

namespace UiTbc
{

RectComponent::RectComponent(const str& pName, Layout* pLayout) :
	Component(pName, pLayout),
	mShaded(false),
	mHollow(true),
	mBehaveSolid(false),
	mImageID(Painter::INVALID_IMAGEID)
{
}

RectComponent::RectComponent(const Color& pColor, const str& pName, Layout* pLayout) :
	Component(pName, pLayout),
	mShaded(false),
	mHollow(false),
	mBehaveSolid(false),
	mImageID(Painter::INVALID_IMAGEID)
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
			     Layout* pLayout) :
	Component(pName, pLayout),
	mShaded(true),
	mHollow(false),
	mBehaveSolid(false),
	mImageID(Painter::INVALID_IMAGEID)
{
	mColor[0] = pTopLeftColor;
	mColor[1] = pTopRightColor;
	mColor[2] = pBottomRightColor;
	mColor[3] = pBottomLeftColor;
}

RectComponent::RectComponent(Painter::ImageID pImageID, const str& pName, Layout* pLayout) :
	Component(pName, pLayout),
	mShaded(false),
	mHollow(false),
	mBehaveSolid(false),
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

void RectComponent::Repaint(Painter* pPainter)
{
	pPainter->PushAttrib(Painter::ATTR_ALL);

	GUIImageManager* lIMan = GetImageManager();

	PixelCoord lPos(GetScreenPos());
	PixelCoord lSize(GetSize());
	PixelRect lRect(lPos, lPos + lSize);

	PixelRect lClippingRect(lRect);
	lClippingRect.mRight++;
	lClippingRect.mBottom++;
	pPainter->ReduceClippingRect(lClippingRect);

	if (mHollow == false)
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
				pPainter->FillRect(lRect);
			}
		}
		else
		{
			lIMan->DrawImage(mImageID, lRect);
		}
	}

	Component::Repaint(pPainter);

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



}
