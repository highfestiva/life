
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/lepratarget.h"
#include "../uifontmanager.h"



namespace uilepra {
class MacDisplayManager;
}



namespace uitbc {



class MacFontManager: public FontManager {
	typedef FontManager Parent;
public:
	MacFontManager(uilepra::MacDisplayManager* display_manager);
	virtual ~MacFontManager();

	virtual FontId AddFont(const str& font_name, double size, int flags = kNormal);
	virtual bool RenderGlyph(wchar_t c, Canvas& image, const PixelRect& rect);
	virtual int GetCharWidth(wchar_t c) const;
	virtual int GetCharOffset(wchar_t c) const;

private:
	struct MacFont: Font {
		double actual_size_;
	};

	uilepra::MacDisplayManager* display_manager_;
};



}
