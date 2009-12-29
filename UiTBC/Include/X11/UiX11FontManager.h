
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../../Lepra/Include/LepraTarget.h"
#include "../UiFontManager.h"



namespace UiLepra
{
class X11DisplayManager;
}



namespace UiTbc
{



class X11FontManager: public FontManager
{
	typedef FontManager Parent;
public:
	X11FontManager(UiLepra::X11DisplayManager* pDisplayManager);
	virtual ~X11FontManager();

	virtual void SetColor(const Color& pColor, unsigned pColorIndex = 0);
	virtual FontId AddFont(const str& pFontName, double pSize, uint32 pFlags = NORMAL, CharacterSet pCharSet = NATIVE);
	virtual bool RenderGlyph(tchar pChar, Canvas& pImage, const PixelRect& pRect);
	virtual int GetCharWidth(const tchar pChar) const;

private:
	/*struct X11BitmapInfo
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD bmiColors[256];
	};

	struct X11Font: Font
	{
		HFONT mX11FontHandle;
	};*/

	UiLepra::X11DisplayManager* mDisplayManager;
	/*HDC mDC;
	COLORREF mColorRef[4];*/
};



}
