
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

	virtual void SetColor(const Color& pColor, unsigned pColorIndex = 0);
	virtual FontId AddFont(const str& pFontName, double pSize, int pFlags = NORMAL, CharacterSet pCharSet = NATIVE);
	virtual bool RenderGlyph(tchar pChar, Canvas& pImage, const PixelRect& pRect);
	virtual int GetCharWidth(const tchar pChar) const;

private:
	struct Win32BitmapInfo
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD bmiColors[256];
	};

	struct Win32Font: Font
	{
		HFONT mWin32FontHandle;
	};

	UiLepra::Win32DisplayManager* mDisplayManager;
	HDC mDC;
	COLORREF mColorRef[4];
};



}
