/*
	Class:  MouseTheme
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../Include/UiMouseTheme.h"

namespace UiTbc
{

MouseTheme::MouseTheme() :
#ifdef LEPRA_WINDOWS
	mCursorHandle(NULL),
#else
	mHotSpotX(0),
	mHotSpotY(0),
	mCursorID(Painter::INVALID_IMAGEID),
	mDrawMode(DRAW_ALPHATESTED),
#endif
	mPainter(0),
	mCursorLoaded(false)
{
}

MouseTheme::~MouseTheme()
{
#ifdef LEPRA_WINDOWS
	if (mCursorHandle != NULL)
	{
		::DestroyIcon(mCursorHandle);
	}
#endif
	SetPainter(0);
}

void MouseTheme::SetPainter(Painter* pPainter)
{
#ifdef LEPRA_WINDOWS
	mPainter = pPainter;
#else
	if (pPainter == mPainter)
	{
		return;
	}

	if (mPainter != 0)
	{
		mPainter->RemoveImage(mCursorID);
	}

	mPainter = pPainter;

	if (mPainter != 0)
	{
		mCursorID = mPainter->AddImage(&mCursor, 0);
	}
#endif
}

void MouseTheme::UnloadCursor()
{
#ifdef LEPRA_WINDOWS
	if (mCursorHandle != NULL)
	{
		::DestroyIcon(mCursorHandle);
		mCursorHandle = NULL;
	}

	::SetCursor(NULL);
#else
	mPainter->RemoveImage(mCursorID);
	mCursorID = Painter::INVALID_IMAGEID;
#endif

	mCursorLoaded = false;
}

void MouseTheme::DrawCursor(int x, int y)
{
#ifdef LEPRA_WINDOWS
	x;
	y;
#else
	if (mCursorLoaded == true && mPainter != 0)
	{
		switch(mDrawMode)
		{
			case DRAW_ALPHABLENDED:
				mPainter->PushAttrib(Painter::ATTR_RENDERMODE | Painter::ATTR_ALPHAVALUE);
				mPainter->SetAlphaValue(255);
				mPainter->SetRenderMode(Painter::RM_ALPHABLEND);
				mPainter->DrawImage(mCursorID, x - mHotSpotX, y - mHotSpotY);
				mPainter->PopAttrib();
				break;
			case DRAW_ALPHATESTED:
				mPainter->PushAttrib(Painter::ATTR_RENDERMODE);
				mPainter->SetRenderMode(Painter::RM_ALPHATEST);
				mPainter->DrawImage(mCursorID, x - mHotSpotX, y - mHotSpotY);
				mPainter->PopAttrib();
				break;

		}
	}
#endif
}

void MouseTheme::SetCursor(const Lepra::Canvas& pCursor, int pHotSpotX, int pHotSpotY)
{
#ifdef LEPRA_WINDOWS

	// This code sets the OS cursor... This way the cursor won't lag
	// as much as the rest of the application.

	if (mCursorHandle != NULL)
	{
		::DestroyIcon(mCursorHandle);
	}

	Lepra::Canvas lAlpha;
	pCursor.GetAlphaChannel(lAlpha);

	ICONINFO lInfo;
	lInfo.hbmMask = ::CreateBitmap(lAlpha.GetWidth(), lAlpha.GetHeight(), 1, Lepra::Canvas::BitDepthToInt(lAlpha.GetBitDepth()), lAlpha.GetBuffer());
	lInfo.hbmColor = ::CreateBitmap(pCursor.GetWidth(), pCursor.GetHeight(), 1, Lepra::Canvas::BitDepthToInt(pCursor.GetBitDepth()), pCursor.GetBuffer());
	lInfo.xHotspot = pHotSpotX;
	lInfo.yHotspot = pHotSpotY;
	lInfo.fIcon = FALSE;

	mCursorHandle = ::CreateIconIndirect(&lInfo);

	::SetCursor(mCursorHandle);

	::DeleteObject(lInfo.hbmMask);
	::DeleteObject(lInfo.hbmColor);
#else
	// Default behaviour... Draw the mouse using the software painter.

	mCursor = pCursor;

	mHotSpotX = pHotSpotX;
	mHotSpotY = pHotSpotY;

	if (mPainter != 0)
	{
		if (mCursorID != Painter::INVALID_IMAGEID)
		{
			mPainter->UpdateImage(mCursorID, &mCursor, 0);
		}
		else
		{
			mCursorID = mPainter->AddImage(&mCursor, 0);
		}
	}
#endif
}

} // End namespace.
