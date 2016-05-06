
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/lepratarget.h"	// Must be first.
#include "../../lepra/include/unordered.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/idmanager.h"
#include "../../lepra/include/string.h"
#include "../include/uitbc.h"



namespace uilepra {
class DisplayManager;
}



namespace uitbc {



class FontManager {
public:
	enum FontId {
		kInvalidFontid = 0,
	};
	enum FontFlags {
		kNormal		= 0,
		kBold		= (1 << 0),
		kItalic		= (1 << 1),
		kUnderline	= (1 << 2),
		kStrikeout	= (1 << 3),
	};

	static FontManager* Create(uilepra::DisplayManager* display_manager);
	FontManager();
	virtual ~FontManager();

	FontId QueryAddFont(const str& font_name, double size, int flags = kNormal);
	virtual FontId AddFont(const str& font_name, double size, int flags = kNormal) = 0;
	void SetActiveFont(FontId font_id);
	FontId GetActiveFontId() const;
	str GetActiveFontName() const;

	virtual bool RenderGlyph(wchar_t c, Canvas& image, const PixelRect& rect) = 0;

	virtual int GetCharWidth(wchar_t c) const = 0;
	virtual int GetCharOffset(wchar_t c) const = 0;
	int GetStringWidth(const wstr& s) const;
	int GetStringHeight(const wstr& s) const;
	int GetFontHeight() const;
	int GetLineHeight() const;	// Font height + some extra pixels as distance to next line.

protected:
	struct Font {
		str name_;
		double size_;
		int flags_;
		FontId font_id_;
	};
	typedef std::unordered_map<int, Font*> FontTable;

	bool InternalAddFont(Font* font);

	FontTable font_table_;
	Font* current_font_;

private:
	IdManager<int> font_id_manager_;
};



}
