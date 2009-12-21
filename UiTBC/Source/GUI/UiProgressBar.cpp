/*
	Class:  ProgressBar
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiProgressBar.h"
#include <math.h>

namespace UiTbc
{
ProgressBar::ProgressBar(Style pStyle,
						 const Color& pBodyColor,
						 const Color& pProgressColorLeft,
						 const Color& pProgressColorRight,
						 const str& pName) :
	Window(BORDER_SUNKEN | BORDER_LINEARSHADING, 2, pBodyColor, pName),
	mStyle(pStyle),
	mBoxSize(16),
	mMax(1),
	mPos(0),
	mUserDefinedGfx(false),
	mProgressColorLeft(pProgressColorLeft),
	mProgressColorRight(pProgressColorRight),
	mProgressImageID(Painter::INVALID_IMAGEID)
{
}

ProgressBar::ProgressBar(Style pStyle,
						 Painter::ImageID pBackgroundImageID,
						 Painter::ImageID pProgressImageID,
						 const str& pName) :
	Window(pBackgroundImageID, pName),
	mStyle(pStyle),
	mBoxSize(16),
	mMax(1),
	mPos(0),
	mUserDefinedGfx(true),
	mProgressColorLeft(0, 0, 0),
	mProgressColorRight(0, 0, 0),
	mProgressImageID(pProgressImageID)
{
}

void ProgressBar::SetProgressMax(int pMax)
{
	mMax = pMax;
	SetNeedsRepaint(true);
}

int ProgressBar::GetProgressMax()
{
	return mMax;
}

void ProgressBar::SetProgressPos(int pPos)
{
	mPos = pPos;
	SetNeedsRepaint(true);
}

int ProgressBar::GetProgressPos()
{
	return mPos;
}

int ProgressBar::Step()
{
	mPos++;
	mPos = mPos < 0 ? 0 : (mPos > mMax ? mMax : mPos);
	SetNeedsRepaint(true);
	return mPos;
}

int ProgressBar::Step(int pSteps)
{
	mPos += pSteps;
	mPos = mPos < 0 ? 0 : (mPos > mMax ? mMax : mPos);
	SetNeedsRepaint(true);
	return mPos;
}

void ProgressBar::Repaint(Painter* pPainter)
{
	GUIImageManager* lIMan = GetImageManager();

	Parent::Repaint(pPainter);
	
	pPainter->PushAttrib(Painter::ATTR_ALL);

	PixelRect lRect = GetClientRect();
	pPainter->SetClippingRect(lRect);

	int lProgress = (int)floor(((float64)mPos / (float64)mMax) * (float64)lRect.GetWidth() + 0.5);
	int lNumBoxes = lProgress / (mBoxSize + 1);
	if (lProgress % (mBoxSize + 1) > 0)
	{
		lNumBoxes++;
	}

	if (mUserDefinedGfx == true)
	{
		if (mStyle == STYLE_NORMAL)
		{
			lRect.mRight = lRect.mLeft + lProgress + 1;
			lIMan->DrawImage(mProgressImageID, lRect);
		}
		else
		{
			PixelRect lBoxRect(lRect.mLeft, lRect.mTop, lRect.mLeft + (mBoxSize + 1) * lNumBoxes, lRect.mBottom);
			lIMan->DrawImage(mProgressImageID, lBoxRect);
		}
	}
	else
	{
		if (mStyle == STYLE_NORMAL)
		{
			Color lRight(mProgressColorLeft, mProgressColorRight, mPos/(float)mMax);
			lRect.mRight = lRect.mLeft + lProgress;
			pPainter->SetColor(mProgressColorLeft, 0);
			pPainter->SetColor(lRight, 1);
			pPainter->SetColor(lRight, 2);
			pPainter->SetColor(mProgressColorLeft, 3);
			pPainter->FillShadedRect(lRect);
		}
		else
		{
			for (int i = 0; i < lNumBoxes; i++)
			{
				int x = i * (mBoxSize + 1);
				pPainter->SetColor(Color(mProgressColorLeft, mProgressColorRight, (x + (mBoxSize + 1) * 0.5f) / lRect.GetWidth()));
				pPainter->SetAlphaValue(mProgressColorLeft.mAlpha);
				pPainter->FillRect(x + lRect.mLeft, lRect.mTop, x + lRect.mLeft + mBoxSize, lRect.mBottom);
			}
		}
	}

	pPainter->PopAttrib();
}

}
