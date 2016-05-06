
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uirectcomponent.h"
#include "uitextcomponent.h"



namespace uitbc {



class Label: public RectComponent, public TextComponent {
	typedef RectComponent Parent;
public:

	Label(const Color& color, const wstr& text);
	virtual ~Label();

	void SetIcon(Painter::ImageID icon_id, IconAlignment alignment);
	void SetText(const wstr& text);
	const wstr& GetText() const;
	virtual void SetSelected(bool selected);

	virtual void Repaint(Painter* painter);
	virtual void ForceRepaint();

	virtual PixelCoord GetPreferredSize(bool force_adaptive);

	virtual Type GetType() const;

protected:
	void UpdateBackground();

	Painter::ImageID icon_id_;
	IconAlignment icon_alignment_;

	wstr text_;

	int text_width_;
	int text_height_;

	bool selectable_;
};



}
