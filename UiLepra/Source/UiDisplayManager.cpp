
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiDisplayManager.h"
#include <assert.h>



namespace UiLepra
{



DisplayManager::DisplayManager()
{
	AddResizeObserver(this);
}

DisplayManager::~DisplayManager()
{
	RemoveResizeObserver(this);
	assert(mResizeObserverSet.empty());
}



void DisplayManager::AddResizeObserver(DisplayResizeObserver* pResizeObserver)
{
	mResizeObserverSet.insert(pResizeObserver);
}

void DisplayManager::RemoveResizeObserver(DisplayResizeObserver* pResizeObserver)
{
	mResizeObserverSet.erase(pResizeObserver);
}

void DisplayManager::DispatchResize(int pWidth, int pHeight)
{
	ResizeObserverSet::iterator x = mResizeObserverSet.begin();
	for (; x != mResizeObserverSet.end(); ++x)
	{
		(*x)->OnResize(pWidth, pHeight);
	}
}

void DisplayManager::DispatchMinimize()
{
	ResizeObserverSet::iterator x = mResizeObserverSet.begin();
	for (; x != mResizeObserverSet.end(); ++x)
	{
		(*x)->OnMinimize();
	}
}

void DisplayManager::DispatchMaximize(int pWidth, int pHeight)
{
	ResizeObserverSet::iterator x = mResizeObserverSet.begin();
	for (; x != mResizeObserverSet.end(); ++x)
	{
		(*x)->OnMaximize(pWidth, pHeight);
	}
}



Lepra::uint8 DisplayManager::GetPaletteColor(int pRed, int pGreen, int pBlue, const Lepra::Color* pPalette)
{
	int	lBestMatch = 0;
	int	i;

	float lMinDist = 10000;
	float lDist;
	float lDeltaRed;
	float lDeltaGreen;
	float lDeltaBlue;

	for (i = 0; i < 256; i++) 
	{
		lDeltaRed   = (float)(pRed   - (int)pPalette[i].mRed);
		lDeltaGreen = (float)(pGreen - (int)pPalette[i].mGreen);
		lDeltaBlue  = (float)(pBlue  - (int)pPalette[i].mBlue);

		lDist = lDeltaRed   * lDeltaRed   + 
				  lDeltaGreen * lDeltaGreen + 
				  lDeltaBlue  * lDeltaBlue;

		if (i == 0 || lDist < lMinDist) 
		{
			lBestMatch = i;
			lMinDist = lDist;
		}
	}

	return (Lepra::uint8)lBestMatch;
}

void DisplayManager::GetScreenCanvas(Lepra::Canvas& pCanvas)
{
	pCanvas.Reset(GetWidth(), GetHeight(), Lepra::Canvas::IntToBitDepth(GetBitDepth()));
	pCanvas.SetPitch(GetPitch());
	pCanvas.SetBuffer(GetScreenPtr());

	if (pCanvas.GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT)
	{
		Lepra::Color lPalette[256];
		for (unsigned i = 0; i < 256; i++)
		{
			lPalette[i] = *GetPaletteColor(i);
		}
		pCanvas.SetPalette(lPalette);
	}
}

LOG_CLASS_DEFINE(UI_GFX, DisplayManager);

} // End namespace.
