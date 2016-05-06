
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uifonttexture.h"
#include "../../lepra/include/resourcetracker.h"
#include "../include/uifontmanager.h"



namespace uitbc {



FontTexture::FontTexture(uint32 font_hash, int font_height):
	canvas_(64, Canvas::PowerUp(font_height), Canvas::kBitdepth32Bit),
	font_hash_(font_hash),
	free_x_offset_(0),
	is_updated_(false) {
	LEPRA_ACQUIRE_RESOURCE(FontTexture);
	canvas_.CreateBuffer();
}

FontTexture::~FontTexture() {
	LEPRA_RELEASE_RESOURCE(FontTexture);
}

uint32 FontTexture::GetFontHash() const {
	return (font_hash_);
}

void FontTexture::StoreGlyph(wchar_t c, FontManager* font_manager) {
	if (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\b') {
		return;
	}

	GlyphXMap::const_iterator x = glyph_x_offset_map_.find(c);
	if (x == glyph_x_offset_map_.end()) {
		const int char_start_x = free_x_offset_;
		const int _width = font_manager->GetCharWidth(c);
		const int offset = font_manager->GetCharOffset(c);
		glyph_x_offset_map_.insert(GlyphXMap::value_type(c, GlyphX{char_start_x, _width, offset}));
		free_x_offset_ += _width;
		if (free_x_offset_ > (int)canvas_.GetWidth()) {
			canvas_.Crop(0, 0, canvas_.GetWidth()*2, canvas_.GetHeight());
		}
		Canvas char_canvas(_width, canvas_.GetHeight(), canvas_.GetBitDepth());
		PixelRect canvas_rect(0, 0, _width, canvas_.GetHeight());
		font_manager->RenderGlyph(c, char_canvas, canvas_rect);
		canvas_.PartialCopy(char_start_x, 0, char_canvas);

		/*printf("Updated texture with glyph '%c' (height=%i):\n", c, canvas_.GetHeight());
		const int max_width = (canvas_.GetWidth() > 80)? 80 : canvas_.GetWidth();
		for (int y = 0; y < (int)canvas_.GetHeight(); ++y) {
			for (int x = 0; x < max_width; ++x) {
				if (canvas_.GetPixelColor(x, y).To32() == 0) {
					printf(" ");
				} else {
					printf("*");
				}
			}
			printf("\n");
		}*/

		is_updated_ = true;
	}
}

bool FontTexture::GetGlyphX(wchar_t c, int& _x, int& width, int& placement_offset) const {
	GlyphXMap::const_iterator x = glyph_x_offset_map_.find(c);
	deb_assert(x != glyph_x_offset_map_.end());
	if (x != glyph_x_offset_map_.end()) {
		_x = x->second.start_x_;
		width = x->second.width_;
		placement_offset = x->second.placement_offset_;
		return (true);
	}
	return (false);
}

bool FontTexture::IsUpdated() const {
	return (is_updated_);
}

void FontTexture::ResetIsUpdated() {
	is_updated_ = false;
}

int FontTexture::GetWidth() const {
	return (canvas_.GetWidth());
}

int FontTexture::GetHeight() const {
	return (canvas_.GetHeight());
}

void* FontTexture::GetBuffer() const {
	return (canvas_.GetBuffer());
}



}
