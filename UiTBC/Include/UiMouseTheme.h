/*
	Lepra::File:   MouseTheme.h
	Class:  MouseTheme
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#ifndef UIMOUSETHEME_H
#define UIMOUSETHEME_H

#include "../../Lepra/Include/Canvas.h"
#include "UiPainter.h"
#include "UiTBC.h"

namespace UiTbc
{

class MouseTheme
{
public:

	MouseTheme();
	virtual ~MouseTheme();

	void SetPainter(Painter* pPainter);
	inline Painter* GetPainter();

	virtual void UnloadCursor();
	virtual void LoadArrowCursor() = 0;
	virtual void LoadBusyCursor() = 0;
	virtual void LoadHResizeCursor() = 0;
	virtual void LoadVResizeCursor() = 0;
	virtual void LoadDiagonal1ResizeCursor() = 0;
	virtual void LoadDiagonal2ResizeCursor() = 0;
	virtual void LoadMoveCursor() = 0;
	virtual void LoadLinkSelectCursor() = 0;
	virtual void LoadPrecisionSelectCursor() = 0;
	virtual void LoadTextCursor() = 0;
	
	void DrawCursor(int x, int y);

protected:

	enum DrawMode
	{
		DRAW_ALPHABLENDED = 0,
		DRAW_ALPHATESTED,
	};

	void SetCursor(const Lepra::Canvas& pCursor, int pHotSpotX, int pHotSpotY);
	inline void SetDrawMode(DrawMode pDrawMode);

private:

#ifdef LEPRA_WINDOWS
	HICON mCursorHandle;
#else
	DrawMode mDrawMode;

	int mHotSpotX;
	int mHotSpotY;
	Lepra::Canvas mCursor;

	Painter::ImageID mCursorID;
#endif
	Painter* mPainter;
	bool mCursorLoaded;
};

Painter* MouseTheme::GetPainter()
{
	return mPainter;
}

void MouseTheme::SetDrawMode(DrawMode pDrawMode)
{
#ifdef LEPRA_WINDOWS
	pDrawMode;
#else
	mDrawMode = pDrawMode;
#endif
}

} // End namespace.

#endif