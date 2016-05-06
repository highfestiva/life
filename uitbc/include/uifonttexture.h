
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uitbc.h"
#include "../../lepra/include/unordered.h"
#include "../../lepra/include/canvas.h"



namespace uitbc {


class FontManager;




// Stores glyphs in a texture along side each other. All glyphs appear at y=0.
class FontTexture {
public:
	FontTexture(uint32 font_hash, int font_height);
	virtual ~FontTexture();

	uint32 GetFontHash() const;

	void StoreGlyph(wchar_t c, FontManager* font_manager);
	bool GetGlyphX(wchar_t c, int& x, int& width, int& placement_offset) const;

	bool IsUpdated() const;
	void ResetIsUpdated();

	int GetWidth() const;
	int GetHeight() const;
	void* GetBuffer() const;

private:
	struct GlyphX {
		int start_x_;
		int width_;
		int placement_offset_;
	};
	typedef std::unordered_map<int32, GlyphX> GlyphXMap;
	GlyphXMap glyph_x_offset_map_;

	Canvas canvas_;
	uint32 font_hash_;
	int free_x_offset_;
	bool is_updated_;
};



}
