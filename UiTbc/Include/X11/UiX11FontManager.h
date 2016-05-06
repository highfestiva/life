
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/lepratarget.h"
#include "../../../lepra/include/unordered.h"
#include "../uifontmanager.h"
#include <ft2build.h>
#include FT_FREETYPE_H



namespace uilepra {
class X11DisplayManager;
}



namespace uitbc {



class X11FontManager: public FontManager {
	typedef FontManager Parent;
public:
	X11FontManager();
	virtual ~X11FontManager();

	virtual FontId AddFont(const str& font_name, double size, int flags = kNormal);
	virtual bool RenderGlyph(wchar_t c, Canvas& image, const PixelRect& rect);
	virtual int GetCharWidth(wchar_t c) const;
	virtual int GetCharOffset(wchar_t c) const;

private:
	struct CharWidthOffset {
		bool set_;
		int width_;
		int offset_;
	};
	typedef std::unordered_map<wchar_t, CharWidthOffset> CharPlacementMap;
	struct X11Font: Font {
		FT_Face x11_face_;
		wchar_t x11_loaded_char_face_;
		CharWidthOffset char_width_offset_[128];
		CharPlacementMap char_placements_;

		X11Font();
		bool GetCharWidth(wchar_t c, int& width, int& offset);
		void PutCharWidth(wchar_t c, int width, int offset);
	};

	static strutil::strvec FindAllFontFiles(const str& path);
	str GetFontFile(const str& font_name, const strutil::strvec& suffixes) const;

	FT_Library library_;
	strutil::strvec font_files_;

	logclass();
};



}
