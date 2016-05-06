/*
	Class:  BorderComponent
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "uirectcomponent.h"

namespace uitbc {

class BorderComponent: public RectComponent {
	typedef RectComponent Parent;
public:

	enum BorderPart {
		kInvalidPart = 0,

		kTopleftCorner,
		kToprightCorner,
		kBottomrightCorner,
		kBottomleftCorner,
		kTopBorder,
		kBottomBorder,
		kLeftBorder,
		kRightBorder,
	};

	enum BorderShadeFunc {
		kLinear = 0,
		kZigzag,
	};

	BorderComponent(BorderPart part, const Color& color, BorderShadeFunc shade_func);
	BorderComponent(BorderPart part, Painter::ImageID image_id);
	virtual ~BorderComponent();

	void SetSunken(bool sunken);

	inline void SetResizable(bool resizable);

	void Set(const Color& color, BorderShadeFunc shade_func);
	void Set(Painter::ImageID image_id);

	virtual void Repaint(Painter* painter);

	virtual bool OnLButtonDown(int mouse_x, int mouse_y);
	virtual bool OnLButtonUp(int mouse_x, int mouse_y);

	virtual bool OnMouseMove(int mouse_x, int mouse_y, int mouse_dx, int mouse_dy);

	inline virtual Type GetType() const;

protected:
private:

	void CalcColors();

	bool image_border_;
	bool resizable_;
	bool resizing_;

	BorderPart part_;
	BorderShadeFunc shading_;

	Color body_color_;
	Color light_color_;
	Color dark_color_;
};

Component::Type BorderComponent::GetType() const {
	return Component::kBorder;
}

void BorderComponent::SetResizable(bool resizable) {
	resizable_ = resizable;
}

}
