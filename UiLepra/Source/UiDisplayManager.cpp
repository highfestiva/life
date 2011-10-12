
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiDisplayManager.h"
#include <assert.h>



namespace UiLepra
{



DisplayManager::DisplayManager():
	mScreenMode(WINDOWED),
	mOrientation(ORIENTATION_ALLOW_ANY),
	mEnumeratedDisplayMode(0),
	mEnumeratedDisplayModeCount(0)
{
	AddResizeObserver(this);
}

DisplayManager::~DisplayManager()
{
	delete[] mEnumeratedDisplayMode;
	mEnumeratedDisplayMode = 0;
	mEnumeratedDisplayModeCount = 0;

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



int DisplayManager::GetNumDisplayModes()
{
	return mEnumeratedDisplayModeCount;
}

int DisplayManager::GetNumDisplayModes(int pBitDepth)
{
	int lCount = 0;

	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mBitDepth == pBitDepth)
		{
			lCount++;
		}
	}

	return lCount;
}

int DisplayManager::GetNumDisplayModes(int pWidth, int pHeight)
{
	int lCount = 0;

	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mWidth == pWidth &&
		   mEnumeratedDisplayMode[i].mHeight == pHeight)
		{
			lCount++;
		}
	}

	return lCount;
}

int DisplayManager::GetNumDisplayModes(int pWidth, int pHeight, int pBitDepth)
{
	int lCount = 0;

	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mWidth == pWidth &&
		   mEnumeratedDisplayMode[i].mHeight == pHeight &&
		   mEnumeratedDisplayMode[i].mBitDepth == pBitDepth)
		{
			lCount++;
		}
	}

	return lCount;
}

bool DisplayManager::GetDisplayMode(DisplayMode& pDisplayMode, int pMode)
{
	bool lOk = false;
	if (pMode >= 0 && pMode < mEnumeratedDisplayModeCount)
	{
		pDisplayMode.mWidth = mEnumeratedDisplayMode[pMode].mWidth;
		pDisplayMode.mHeight = mEnumeratedDisplayMode[pMode].mHeight;
		pDisplayMode.mBitDepth = mEnumeratedDisplayMode[pMode].mBitDepth;
		pDisplayMode.mRefreshRate = mEnumeratedDisplayMode[pMode].mRefreshRate;

		lOk = true;
	}
	return lOk;
}

bool DisplayManager::GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pBitDepth)
{
	int lCount = 0;
	bool lOk = false;
	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mBitDepth == pBitDepth)
		{
			if (lCount == pMode)
			{
				pDisplayMode.mWidth = mEnumeratedDisplayMode[i].mWidth;
				pDisplayMode.mHeight = mEnumeratedDisplayMode[i].mHeight;
				pDisplayMode.mBitDepth = mEnumeratedDisplayMode[i].mBitDepth;
				pDisplayMode.mRefreshRate = mEnumeratedDisplayMode[i].mRefreshRate;

				lOk = true;
				break;
			}

			lCount++;
		}
	}
	return lOk;
}

bool DisplayManager::GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pWidth, int pHeight)
{
	int lCount = 0;
	bool lOk = false;
	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mWidth == pWidth &&
		   mEnumeratedDisplayMode[i].mHeight == pHeight)
		{
			if (lCount == pMode)
			{
				pDisplayMode.mWidth = mEnumeratedDisplayMode[i].mWidth;
				pDisplayMode.mHeight = mEnumeratedDisplayMode[i].mHeight;
				pDisplayMode.mBitDepth = mEnumeratedDisplayMode[i].mBitDepth;
				pDisplayMode.mRefreshRate = mEnumeratedDisplayMode[i].mRefreshRate;

				lOk = true;
				break;
			}

			lCount++;
		}
	}
	return lOk;
}

bool DisplayManager::GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pWidth, int pHeight, int pBitDepth)
{
	int lCount = 0;
	bool lOk = false;
	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mWidth == pWidth &&
		   mEnumeratedDisplayMode[i].mHeight == pHeight &&
		   mEnumeratedDisplayMode[i].mBitDepth == pBitDepth)
		{
			if (lCount == pMode)
			{
				pDisplayMode.mWidth = mEnumeratedDisplayMode[i].mWidth;
				pDisplayMode.mHeight = mEnumeratedDisplayMode[i].mHeight;
				pDisplayMode.mBitDepth = mEnumeratedDisplayMode[i].mBitDepth;
				pDisplayMode.mRefreshRate = mEnumeratedDisplayMode[i].mRefreshRate;

				lOk = true;
				break;
			}

			lCount++;
		}
	}
	return lOk;
}

bool DisplayManager::FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight)
{
	bool lModeFound = false;

	pDisplayMode.mWidth = 0;
	pDisplayMode.mHeight = 0;
	pDisplayMode.mBitDepth = 0;
	pDisplayMode.mRefreshRate = 0;

	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mWidth == pWidth &&
		   mEnumeratedDisplayMode[i].mHeight == pHeight)
		{
			if (mEnumeratedDisplayMode[i].mBitDepth == pDisplayMode.mBitDepth &&
			   mEnumeratedDisplayMode[i].mRefreshRate > pDisplayMode.mRefreshRate)
			{
				pDisplayMode.mWidth = mEnumeratedDisplayMode[i].mWidth;
				pDisplayMode.mHeight = mEnumeratedDisplayMode[i].mHeight;
				pDisplayMode.mBitDepth = mEnumeratedDisplayMode[i].mBitDepth;
				pDisplayMode.mRefreshRate = mEnumeratedDisplayMode[i].mRefreshRate;

				lModeFound = true;
			}
			else if(mEnumeratedDisplayMode[i].mBitDepth > pDisplayMode.mBitDepth)
			{
				pDisplayMode.mWidth = mEnumeratedDisplayMode[i].mWidth;
				pDisplayMode.mHeight = mEnumeratedDisplayMode[i].mHeight;
				pDisplayMode.mBitDepth = mEnumeratedDisplayMode[i].mBitDepth;
				pDisplayMode.mRefreshRate = mEnumeratedDisplayMode[i].mRefreshRate;

				lModeFound = true;
			}
		}
	}

	return lModeFound;
}

bool DisplayManager::FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight, int pBitDepth)
{
	bool lModeFound = false;

	pDisplayMode.mWidth = 0;
	pDisplayMode.mHeight = 0;
	pDisplayMode.mBitDepth = 0;
	pDisplayMode.mRefreshRate = 0;

	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mWidth == pWidth &&
		   mEnumeratedDisplayMode[i].mHeight == pHeight &&
		   mEnumeratedDisplayMode[i].mBitDepth == pBitDepth)
		{
			if (mEnumeratedDisplayMode[i].mRefreshRate > pDisplayMode.mRefreshRate)
			{
				pDisplayMode.mWidth = mEnumeratedDisplayMode[i].mWidth;
				pDisplayMode.mHeight = mEnumeratedDisplayMode[i].mHeight;
				pDisplayMode.mBitDepth = mEnumeratedDisplayMode[i].mBitDepth;
				pDisplayMode.mRefreshRate = mEnumeratedDisplayMode[i].mRefreshRate;

				lModeFound = true;
			}
		}
	}

	return lModeFound;
}

bool DisplayManager::FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight, int pBitDepth, int pRefreshRate)
{
	pDisplayMode.mWidth = 0;
	pDisplayMode.mHeight = 0;
	pDisplayMode.mBitDepth = 0;
	pDisplayMode.mRefreshRate = 0;

	bool lOk = false;
	for (int i = 0; i < mEnumeratedDisplayModeCount; i++)
	{
		if (mEnumeratedDisplayMode[i].mWidth == pWidth &&
		   mEnumeratedDisplayMode[i].mHeight == pHeight &&
		   mEnumeratedDisplayMode[i].mBitDepth == pBitDepth &&
		   mEnumeratedDisplayMode[i].mRefreshRate == pRefreshRate)
		{
			pDisplayMode.mWidth = mEnumeratedDisplayMode[i].mWidth;
			pDisplayMode.mHeight = mEnumeratedDisplayMode[i].mHeight;
			pDisplayMode.mBitDepth = mEnumeratedDisplayMode[i].mBitDepth;
			pDisplayMode.mRefreshRate = mEnumeratedDisplayMode[i].mRefreshRate;

			lOk = true;
			break;
		}
	}

	return lOk;
}



void DisplayManager::GetScreenCanvas(Canvas& pCanvas)
{
	pCanvas.Reset(GetWidth(), GetHeight(), Canvas::IntToBitDepth(GetBitDepth()));
}

void DisplayManager::GetScreenCanvas(Canvas* pCanvas)
{
	GetScreenCanvas(*pCanvas);
}



LOG_CLASS_DEFINE(UI_GFX, DisplayManager);



}
