
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../Lepra/Include/LepraOS.h"
#include "../UiFontManager.h"



namespace UiLepra
{
class Win32DisplayManager;
}



namespace UiTbc
{



class Win32FontManager: public FontManager
{
	typedef FontManager Parent;
public:
	Win32FontManager(UiLepra::Win32DisplayManager* pDisplayManager);
	virtual ~Win32FontManager();

	virtual FontId AddFont(const str& pFontName, double pSize, int pFlags = NORMAL);
	virtual bool RenderGlyph(wchar_t pChar, Canvas& pImage, const PixelRect& pRect);
	virtual int GetCharWidth(wchar_t pChar) const;
	virtual int GetCharOffset(wchar_t pChar) const;

private:
	struct Win32Font: Font
	{
		HFONT mWin32FontHandle;
	};

	UiLepra::Win32DisplayManager* mDisplayManager;
	HDC mDC;
	COLORREF mColorRef[4];
};



}
