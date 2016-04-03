
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../Lepra/Include/LepraTarget.h"
#include "../../../Lepra/Include/Unordered.h"
#include "../UiFontManager.h"
#include <ft2build.h>
#include FT_FREETYPE_H



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
	X11FontManager();
	virtual ~X11FontManager();

	virtual FontId AddFont(const str& pFontName, double pSize, int pFlags = NORMAL);
	virtual bool RenderGlyph(wchar_t pChar, Canvas& pImage, const PixelRect& pRect);
	virtual int GetCharWidth(wchar_t pChar) const;
	virtual int GetCharOffset(wchar_t pChar) const;

private:
	typedef std::pair<int,int> CharWidthOffset;
	typedef std::unordered_map<wchar_t, CharWidthOffset> CharPlacementMap;
	struct X11Font: Font
	{
		FT_Face mX11Face;
		wchar_t mX11LoadedCharFace;
		CharPlacementMap mCharPlacements;
	};

	static strutil::strvec FindAllFontFiles(const str& pPath);
	str GetFontFile(const str& pFontName, const strutil::strvec& pSuffixes) const;

	FT_Library mLibrary;
	strutil::strvec mFontFiles;

	logclass();
};



}
