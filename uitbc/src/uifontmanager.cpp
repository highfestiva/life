
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uifontmanager.h"
#include <algorithm>
#include "../../lepra/include/resourcetracker.h"



namespace uitbc {



FontManager::FontManager():
	current_font_(0),
	font_id_manager_(1, 10000, 0) {
}

FontManager::~FontManager() {
}


FontManager::FontId FontManager::QueryAddFont(const str& font_name, double size, int flags) {
	FontTable::iterator x = font_table_.begin();
	for (; x != font_table_.end(); ++x) {
		const Font* _font = x->second;
		if (_font->name_ == font_name && Math::IsEpsEqual(_font->size_, size, 0.1) && _font->flags_ == flags) {
			SetActiveFont(_font->font_id_);
			return _font->font_id_;
		}
	}
	const FontId _font_id = AddFont(font_name, size, flags);
	if (_font_id) {
		SetActiveFont(_font_id);
	}
	return _font_id;
}

void FontManager::SetActiveFont(FontId font_id) {
	if (current_font_ && current_font_->font_id_ == font_id) {
		return;
	}

	FontTable::iterator x = font_table_.find(font_id);
	if (x != font_table_.end()) {
		current_font_ = x->second;
	}
}

FontManager::FontId FontManager::GetActiveFontId() const {
	if (current_font_) {
		return (current_font_->font_id_);
	}
	return (kInvalidFontid);
}

str FontManager::GetActiveFontName() const {
	if (current_font_) {
		return (current_font_->name_);
	}
	return (str());
}



int FontManager::GetStringWidth(const wstr& s) const {
	int max_x = 0;
	int current_x = 0;
	const size_t __length = s.length();
	for (size_t i = 0; i < __length; i++) {
		wchar_t c = s[i];

		if (c == '\n') {
			current_x = 0;
		} else if(c != '\r' &&
			c != '\b' &&
			c != '\t') {
			current_x += GetCharWidth(c) + GetCharOffset(c);
		}

		if (current_x > max_x) {
			max_x = current_x;
		}
	}
	return (max_x);
}

int FontManager::GetStringHeight(const wstr& s) const {
	return GetLineHeight() * (std::count(s.begin(), s.end(), L'\n') + 1);
}

int FontManager::GetFontHeight() const {
	if (!current_font_) {
		return (0);
	}
	return (int)(current_font_->size_ + 0.5);
}

int FontManager::GetLineHeight() const {
	if (!current_font_) {
		return (0);
	}
	return (int)(current_font_->size_ + 3.5);	// Add some extra pixels for distance to next line.
}



bool FontManager::InternalAddFont(Font* font) {
	LEPRA_ACQUIRE_RESOURCE(Font);

	const int id = font_id_manager_.GetFreeId();
	bool ok = (id != 0);
	if (ok) {
		font->font_id_ = (FontId)id;
		font_table_.insert(FontTable::value_type(id, font));
	}
	return (ok);
}



}
