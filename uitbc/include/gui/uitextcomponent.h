
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "../uifontmanager.h"



namespace uitbc {



class Painter;



class TextComponent {
public:
	enum IconAlignment {
		kIconLeft = 0,
		kIconCenter,
		kIconRight,
	};
	enum VAlign {
		kValignTop,
		kValignCenter,
		kValignBottom,
	};

	TextComponent();
	virtual ~TextComponent();

	void SetFontColor(const Color& text_color);
	void SetFontId(const FontManager::FontId& font_id);
	FontManager::FontId GetFontId() const;
	void ActivateFont(Painter* painter);
	void DeactivateFont(Painter* painter);
	void SetHorizontalMargin(int horizontal_margin);
	void SetVericalAlignment(VAlign alignment);

protected:
	virtual void ForceRepaint() = 0;

	void DoPrintText(Painter* painter, const wstr& text, int x, int y);
	void PrintTextDeactivate(Painter* painter, const wstr& text, int x, int y);
	Color GetTextColor() const;
	VAlign GetVAlign() const;

	Color text_color_;
	bool is_font_active_;
	FontManager::FontId font_id_;
	int horizontal_margin_;
	VAlign v_alignment_;
};



}
