
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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

	virtual FontId AddFont(const str& pFontName, double pSize, int pFlags = NORMAL);
	virtual bool RenderGlyph(wchar_t pChar, Canvas& pImage, const PixelRect& pRect);
	virtual int GetCharWidth(wchar_t pChar) const;
	virtual int GetCharOffset(wchar_t pChar) const;

private:
	struct MacFont: Font
	{
		double mActualSize;
	};

	UiLepra::MacDisplayManager* mDisplayManager;
};



}
