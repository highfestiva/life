
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../../Lepra/Include/LepraTarget.h"
#include "../UiFontManager.h"



namespace UiLepra
{
class MacDisplayManager;
}



namespace UiTbc
{



class MacFontManager: public FontManager
{
	typedef FontManager Parent;
public:
	MacFontManager(UiLepra::MacDisplayManager* pDisplayManager);
	virtual ~MacFontManager();

	virtual void SetColor(const Color& pColor, unsigned pColorIndex = 0);
	virtual FontId AddFont(const str& pFontName, double pSize, int pFlags = NORMAL, CharacterSet pCharSet = NATIVE);
	virtual bool RenderGlyph(tchar pChar, Canvas& pImage, const PixelRect& pRect);
	virtual int GetCharWidth(const tchar pChar) const;

private:
	/*struct MacBitmapInfo
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD bmiColors[256];
	};*/

	struct MacFont: Font
	{
	};

	UiLepra::MacDisplayManager* mDisplayManager;
	/*HDC mDC;
	COLORREF mColorRef[4];*/
};



}
