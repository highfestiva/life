
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/lepraos.h"
#include "../uifontmanager.h"



namespace uilepra {
class Win32DisplayManager;
}



namespace uitbc {



class Win32FontManager: public FontManager {
	typedef FontManager Parent;
public:
	Win32FontManager(uilepra::Win32DisplayManager* display_manager);
	virtual ~Win32FontManager();

	virtual FontId AddFont(const str& font_name, double size, int flags = kNormal);
	virtual bool RenderGlyph(wchar_t c, Canvas& image, const PixelRect& rect);
	virtual int GetCharWidth(wchar_t c) const;
	virtual int GetCharOffset(wchar_t c) const;

private:
	struct Win32Font: Font {
		HFONT win32_font_handle_;
	};

	uilepra::Win32DisplayManager* display_manager_;
	HDC dc_;
	COLORREF color_ref_[4];
};



}
