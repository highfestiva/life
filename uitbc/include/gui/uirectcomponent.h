/*
	Class:  RectComponent
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A simple component that consists of a single rectangle,
	which can have a single color, be shaded, or even have an image.
*/

#pragma once

#include "uicomponent.h"
#include "../uipainter.h"
#include "../../../lepra/include/graphics2d.h"

namespace uitbc {

class RectComponent : public Component {
public:
	RectComponent(Layout* layout = 0);
	RectComponent(const Color& color, Layout* layout = 0);
	RectComponent(const Color& top_left_color,
				  const Color& top_right_color,
				  const Color& bottom_right_color,
				  const Color& bottom_left_color,
				  Layout* layout = 0);
	RectComponent(Painter::ImageID image_id, Layout* layout = 0);

	virtual ~RectComponent();

	virtual void RepaintBackground(Painter* painter);
	virtual bool IsOver(int screen_x, int screen_y);

	void SetImage(Painter::ImageID image_id);
	virtual void SetColor(const Color& color);
	virtual void SetColor(const Color& top_left_color, const Color& top_right_color, const Color& bottom_right_color, const Color& bottom_left_color);

	Painter::ImageID GetImage();
	Color GetColor();
	void GetColor(Color& top_left_color,
			     Color& top_right_color,
			     Color& bottom_right_color,
			     Color& bottom_left_color);

	virtual Type GetType() const;

	void SetIsHollow(bool is_hollow);
	void SetBehaveSolid(bool behave_solid);
	bool GetBehaveSolid() const;

	int GetCornerRadius() const;
	void SetCornerRadius(int radius);
	void SetCornerRadiusMask(int mask);

protected:
	bool IsHollow();
	bool IsShaded();

	Color color_[4];

private:
	bool shaded_;
	bool hollow_;
	bool behave_solid_;
	int corner_radius_;
	int corner_radius_mask_;

	Painter::ImageID image_id_;
};

}
