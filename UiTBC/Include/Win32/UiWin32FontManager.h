
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../../Lepra/Include/LepraTarget.h"
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

	virtual void SetColor(const Lepra::Color& pColor, unsigned pColorIndex = 0);
	virtual FontId AddFont(const Lepra::String& pFontName, double pSize, Lepra::uint32 pFlags = NORMAL, CharacterSet pCharSet = NATIVE);
	virtual bool RenderGlyph(Lepra::tchar pChar, Lepra::Canvas& pImage, const Lepra::PixelRect& pRect);
	virtual int GetCharWidth(const Lepra::tchar pChar) const;

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
