
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uitextcomponent.h"
#include "../../include/uipainter.h"



namespace uitbc {



TextComponent::TextComponent():
	text_color_(OFF_BLACK),
	font_id_(FontManager::kInvalidFontid),
	is_font_active_(false),
	v_alignment_(kValignCenter),
	horizontal_margin_(3) {
}

TextComponent::~TextComponent() {
}

void TextComponent::SetFontColor(const Color& text_color) {
	deb_assert(text_color != BLACK);
	text_color_ = text_color;
	ForceRepaint();
}

void TextComponent::SetFontId(const FontManager::FontId& font_id) {
	deb_assert(!is_font_active_);
	font_id_ = font_id;
	ForceRepaint();
}

FontManager::FontId TextComponent::GetFontId() const {
	return font_id_;
}

void TextComponent::ActivateFont(Painter* painter) {
	deb_assert(!is_font_active_);
	if (is_font_active_) {
		return;
	}
	if (font_id_ != FontManager::kInvalidFontid) {
		const FontManager::FontId old_font_id = painter->GetFontManager()->GetActiveFontId();
		painter->GetFontManager()->SetActiveFont(font_id_);
		font_id_ = old_font_id;
		is_font_active_ = true;
	}
}

void TextComponent::DeactivateFont(Painter* painter) {
	deb_assert(is_font_active_ || font_id_ == FontManager::kInvalidFontid);
	if (is_font_active_) {
		const FontManager::FontId our_font_id = painter->GetFontManager()->GetActiveFontId();
		if (font_id_ != FontManager::kInvalidFontid) {
			painter->GetFontManager()->SetActiveFont(font_id_);
		}
		font_id_ = our_font_id;
	}
	is_font_active_ = false;
}

void TextComponent::SetHorizontalMargin(int horizontal_margin) {
	horizontal_margin_ = horizontal_margin;
}

void TextComponent::SetVericalAlignment(VAlign alignment) {
	v_alignment_ = alignment;
}

void TextComponent::DoPrintText(Painter* painter, const wstr& text, int x, int y) {
	painter->SetColor(text_color_, 0);
	painter->PrintText(text, x, y);
}

void TextComponent::PrintTextDeactivate(Painter* painter, const wstr& text, int x, int y) {
	DoPrintText(painter, text, x, y);
	DeactivateFont(painter);
}



Color TextComponent::GetTextColor() const {
	return (text_color_);
}

TextComponent::VAlign TextComponent::GetVAlign() const {
	return v_alignment_;
}



}
