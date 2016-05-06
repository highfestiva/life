
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../../lepra/include/cyclicarray.h"
#include "../../../lepra/include/diskfile.h"
#include "../../../lepra/include/path.h"
#include "../../include/x11/uix11fontmanager.h"



namespace uitbc {



FontManager* FontManager::Create(uilepra::DisplayManager* display_manager) {
	return new X11FontManager;
}



X11FontManager::X11Font::X11Font():
	x11_loaded_char_face_(0) {
	::memset(&char_width_offset_[0], 0, sizeof(char_width_offset_));
}

bool X11FontManager::X11Font::GetCharWidth(wchar_t c, int& _width, int& offset) {
	if (c < LEPRA_ARRAY_COUNT(char_width_offset_)) {
		_width  = char_width_offset_[c].width_;
		offset = char_width_offset_[c].offset_;
		return char_width_offset_[c].set_;
	}
	CharPlacementMap::const_iterator x = char_placements_.find(c);
	if (x != char_placements_.end()) {
		_width  = x->second.width_;
		offset = x->second.offset_;
		return true;
	}
	return false;
}

void X11FontManager::X11Font::PutCharWidth(wchar_t c, int _width, int offset) {
	if (c < LEPRA_ARRAY_COUNT(char_width_offset_)) {
		char_width_offset_[c].set_ = true;
		char_width_offset_[c].width_ = _width;
		char_width_offset_[c].offset_ = offset;
	} else {
		char_placements_[c] = {true, _width, offset};
	}
}



X11FontManager::X11FontManager() {
	FT_Init_FreeType(&library_);
}

X11FontManager::~X11FontManager() {
	FontTable::iterator x = font_table_.begin();
	if (x != font_table_.end()) {
		X11Font* font = (X11Font*)x->second;
		FT_Done_Face(font->x11_face_);
		delete font;
	}
	font_table_.clear();

	FT_Done_FreeType(library_);
}




X11FontManager::FontId X11FontManager::AddFont(const str& font_name, double size, int flags) {
	if (font_files_.empty()) {
		strutil::Append(font_files_, FindAllFontFiles("/usr/share/fonts"));
		strutil::Append(font_files_, FindAllFontFiles("/usr/local/share/fonts"));
	}
	strutil::strvec _suffixes;
	if (flags & (kBold|kItalic)) {
		_suffixes.push_back("-BoldItalic");
		_suffixes.push_back("bi.");
		_suffixes.push_back("z.");
		_suffixes.push_back("-Italic");
		_suffixes.push_back("i.");
		_suffixes.push_back("-Bold");
		_suffixes.push_back("b.");
		_suffixes.push_back("b");
		_suffixes.push_back("");
	} else if (flags & kBold) {
		_suffixes.push_back("-Bold.");
		_suffixes.push_back("b.");
		_suffixes.push_back("Bold");
		_suffixes.push_back("b");
		_suffixes.push_back("");
	} else if (flags & kItalic) {
		_suffixes.push_back("-Italic.");
		_suffixes.push_back("i.");
		_suffixes.push_back("Italic");
		_suffixes.push_back("i");
		_suffixes.push_back("");
	}
	_suffixes.push_back("-Regular.");
	_suffixes.push_back(".");
	_suffixes.push_back("Regular");
	_suffixes.push_back("Light");
	_suffixes.push_back("");
	const str font_file = GetFontFile(font_name, _suffixes);
	FT_Face face;
	if (font_file.empty() || FT_New_Face(library_, font_file.c_str(), 0, &face) != 0) {
		log_.Debugf("Unable to find font %s.", font_name.c_str());
		return kInvalidFontid;
	}
	FT_Set_Char_Size(face, 0, int(size*0.7*64), 90, 90);
	X11Font* font = new X11Font;
	font->x11_face_ = face;
	font->x11_loaded_char_face_ = 0;
	font->name_ = font_name;
	font->size_ = size;
	font->flags_ = flags;
	if (!InternalAddFont(font)) {
		delete (font);
		FT_Done_Face(face);
		return kInvalidFontid;
	}
	return font->font_id_;
}

bool X11FontManager::RenderGlyph(wchar_t c, Canvas& image, const PixelRect& rect) {
	bool ok = (current_font_ != 0);
	if (ok) {
		ok = (rect.top_ >= 0 && rect.left_ >= 0 &&
			rect.GetWidth() >= 1 && rect.GetHeight() >= 1 &&
			rect.right_ <= (int)image.GetWidth() && rect.bottom_ <= (int)image.GetHeight());
	}
	if (ok) {
		X11Font* font = (X11Font*)current_font_;
		if (font->x11_loaded_char_face_ != c) {
			FT_Load_Char(font->x11_face_, c, FT_LOAD_RENDER|FT_LOAD_TARGET_LCD|FT_LOAD_PEDANTIC|FT_LOAD_FORCE_AUTOHINT);
			font->x11_loaded_char_face_ = c;
		}
		const FT_GlyphSlot __glyph = font->x11_face_->glyph;
		const FT_Bitmap* face_bitmap = &__glyph->bitmap;
		const uint8* raw_face_bitmap = __glyph->bitmap.buffer;
		const int __width = face_bitmap->width/3;
		const int height = face_bitmap->rows;
		const int base_line = int(font->size_ * font->x11_face_->bbox.yMax / float(font->x11_face_->bbox.yMax - font->x11_face_->bbox.yMin)) + 1;
		const int y_offset = base_line - __glyph->metrics.horiBearingY / 64;
		deb_assert(__width <= rect.GetWidth());
		deb_assert(y_offset >= 0);
		deb_assert(y_offset+height <= font->size_);
		const int byte_pitch = std::abs(face_bitmap->pitch);
		uint8* __bitmap = new uint8[3*rect.GetWidth()*rect.GetHeight()];
		::memset(__bitmap, 0, 3*rect.GetWidth()*rect.GetHeight());
		for (int y = 0; y < height; ++y) {
			const uint8* s = &raw_face_bitmap[y*byte_pitch];
			uint8* d = &__bitmap[(y_offset + y) * 3 * image.GetWidth()];
			for (int x = 0; x < 3*__width; ++x) {
				*d++ = *s++;
			}
		}

		Canvas::BitDepth source_bit_depth = Canvas::kBitdepth24Bit;
		Canvas::BitDepth target_bit_depth = image.GetBitDepth();
		image.Reset(rect.GetWidth(), rect.GetHeight(), source_bit_depth);
		image.SetBuffer(__bitmap, false, true);
		__bitmap = 0;	// Don't delete it, ownership now taken by canvas.
		image.ConvertBitDepth(target_bit_depth);
		if (face_bitmap->pitch > 0) {
			image.FlipVertical();	// Glyph is upside down...
		}
		image.SwapRGBOrder();	// FreeType fonts are always kBgr.
		for (int y = 0; y < rect.GetHeight(); ++y) {
			for (int x = 0; x < rect.GetWidth(); ++x) {
				Color color;
				image.GetPixelColor(x, y, color);
				int intensity = color.SumRgb();
				// Sum controls part of light intensity, average controls part.
				intensity = (intensity*20 + (intensity/3)*80) / 100;
				intensity = (intensity > 255)? 255 : intensity;
				color.red_ = color.green_ = color.blue_ = 255;
				color.alpha_ = (uint8)(intensity);
				image.SetPixelColor(x, y, color);
			}
		}
	}
	return (ok);
}



int X11FontManager::GetCharWidth(wchar_t c) const {
	if (!current_font_) {
		deb_assert(false);
		return 10;
	}

	X11Font* font = (X11Font*)current_font_;
	int __width;
	int _offset;
	if (font->GetCharWidth(c, __width, _offset)) {
		return __width;
	}
	if (font->x11_loaded_char_face_ != c) {
		FT_Load_Char(font->x11_face_, c, FT_LOAD_RENDER|FT_LOAD_TARGET_LCD|FT_LOAD_PEDANTIC|FT_LOAD_FORCE_AUTOHINT);
		font->x11_loaded_char_face_ = c;
	}
	__width = (font->x11_face_->glyph->metrics.horiAdvance - font->x11_face_->glyph->metrics.horiBearingX) / 64 + 1;
	_offset = (font->x11_face_->glyph->metrics.horiBearingX - 63) / 64;
	font->PutCharWidth(c, __width, _offset);
	return __width;
}

int X11FontManager::GetCharOffset(wchar_t c) const {
	X11Font* font = (X11Font*)current_font_;
	int __width;
	int _offset;
	if (!font->GetCharWidth(c, __width, _offset)) {
		GetCharWidth(c);
		font->GetCharWidth(c, __width, _offset);
	}
	return std::min(0, _offset);
}



strutil::strvec X11FontManager::FindAllFontFiles(const str& path) {
	strutil::strvec files;
	DiskFile::FindData data;
	if (!DiskFile::FindFirst(Path::JoinPath(path, "*"), data)) {
		return files;
	}
	do {
		if (data.IsSubDir()) {
			strutil::Append(files, FindAllFontFiles(data.GetName()));
		} else if (strutil::EndsWith(data.GetName(), ".ttf") || strutil::EndsWith(data.GetName(), ".otf")) {
			files.push_back(data.GetName());
		}
	} while (DiskFile::FindNext(data));
	return files;
}

str X11FontManager::GetFontFile(const str& font_name, const strutil::strvec& suffixes) const {
	strutil::strvec names = strutil::Split(font_name, " ");
	str name0 = names[0];
	str full_name = strutil::Join(names, "");
	strutil::ToLower(name0);
	strutil::ToLower(full_name);
	for (auto suffix: suffixes) {
		strutil::ToLower(suffix);
		for (auto font_file: font_files_) {
			str lower_font_file = font_file;
			strutil::ToLower(lower_font_file);
			//log_.Infof("%s == %s || %s", lower_font_file.c_str(), (full_name+suffix).c_str(), name0.c_str());
			if (lower_font_file.find(full_name+suffix) != str::npos) {
				return font_file;
			}
			if (lower_font_file.find(name0+suffix) != str::npos) {
				return font_file;
			}
		}
	}
	return str();
}



loginstance(kUiGfx2D, X11FontManager);



}
