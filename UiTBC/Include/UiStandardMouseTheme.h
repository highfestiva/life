/*
	Lepra::File:   StandardMouseTheme.h
	Class:  StandardMouseTheme
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#ifndef UISTANDARDMOUSETHEME_H
#define UISTANDARDMOUSETHEME_H

#include "UiMouseTheme.h"
#include "UiStandardMouseTheme.h"

namespace UiTbc
{

class StandardMouseTheme : public MouseTheme
{
public:
	void UnloadCursor();

	void LoadArrowCursor();
	void LoadBusyCursor();
	void LoadHResizeCursor();
	void LoadVResizeCursor();
	void LoadDiagonal1ResizeCursor();
	void LoadDiagonal2ResizeCursor();
	void LoadMoveCursor();
	void LoadLinkSelectCursor();
	void LoadPrecisionSelectCursor();
	void LoadTextCursor();

private:

	enum CursorType
	{
		CT_NONE = 0,
		CT_ARROW,
		CT_BUSY,
		CT_HRESIZE,
		CT_VRESIZE,
		CT_DIAG1,
		CT_DIAG2,
		CT_MOVE,
		CT_LINKSEL,
		CT_PRECSEL,
		CT_TEXT
	};

	void AddAlphaChannel(Lepra::Canvas* pImage);

	CursorType mCursorTypeLoaded;

	static unsigned char smArrowCursor[];
	static unsigned char smBusyCursor[];
	static unsigned char smHResizeCursor[];
	static unsigned char smVResizeCursor[];
	static unsigned char smDiagonal1ResizeCursor[];
	static unsigned char smDiagonal2ResizeCursor[];
	static unsigned char smMoveCursor[];
	static unsigned char smLinkSelectCursor[];
	static unsigned char smPrecisionSelectCursor[];
	static unsigned char smTextCursor[];
};

} // End namespace.

#endif