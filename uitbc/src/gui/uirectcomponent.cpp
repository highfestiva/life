
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uirectcomponent.h"
#include "../../include/gui/uidesktopwindow.h"



namespace uitbc {



RectComponent::RectComponent(Layout* layout):
	Component(layout),
	shaded_(false),
	hollow_(true),
	behave_solid_(false),
	image_id_(Painter::kInvalidImageid),
	corner_radius_(0),
	corner_radius_mask_(0xF) {
}

RectComponent::RectComponent(const Color& color, Layout* layout):
	Component(layout),
	shaded_(false),
	hollow_(false),
	behave_solid_(false),
	image_id_(Painter::kInvalidImageid),
	corner_radius_(0),
	corner_radius_mask_(0xF) {
	color_[0] = color;
	color_[1].Set(0, 0, 0, 0);
	color_[2].Set(0, 0, 0, 0);
	color_[3].Set(0, 0, 0, 0);
}

RectComponent::RectComponent(const Color& top_left_color,
			     const Color& top_right_color,
			     const Color& bottom_right_color,
			     const Color& bottom_left_color,
			     Layout* layout):
	Component(layout),
	shaded_(true),
	hollow_(false),
	behave_solid_(false),
	corner_radius_(0),
	corner_radius_mask_(0xF),
	image_id_(Painter::kInvalidImageid) {
	color_[0] = top_left_color;
	color_[1] = top_right_color;
	color_[2] = bottom_right_color;
	color_[3] = bottom_left_color;
}

RectComponent::RectComponent(Painter::ImageID image_id, Layout* layout):
	Component(layout),
	shaded_(false),
	hollow_(false),
	behave_solid_(false),
	corner_radius_(0),
	corner_radius_mask_(0xF),
	image_id_(image_id) {
	color_[0].Set(0, 0, 0, 0);
	color_[1].Set(0, 0, 0, 0);
	color_[2].Set(0, 0, 0, 0);
	color_[3].Set(0, 0, 0, 0);
}

RectComponent::~RectComponent() {
}

void RectComponent::RepaintBackground(Painter* painter) {
	painter->PushAttrib(Painter::kAttrAll);

	GUIImageManager* i_man = GetImageManager();

	PixelCoord pos(GetScreenPos());
	PixelCoord size(GetSize());
	PixelRect rect(pos, pos + size);

	PixelRect clipping_rect(rect);
	clipping_rect.right_++;
	clipping_rect.bottom_++;
	//painter->ReduceClippingRect(clipping_rect);

	if (hollow_ == false) {
		if (corner_radius_ == 0) {
			if (image_id_ == Painter::kInvalidImageid) {
				painter->SetColor(color_[0], 0);
				painter->SetAlphaValue(color_[0].alpha_);
				if (shaded_ == true) {
					painter->SetColor(color_[1], 1);
					painter->SetColor(color_[2], 2);
					painter->SetColor(color_[3], 3);
					painter->FillShadedRect(rect);
				} else {
					if (color_[0].alpha_ != 0) {
						painter->FillRect(rect);
					}
				}
			} else {
				i_man->DrawImage(image_id_, rect);
			}
		} else {	// Draw with rounded corners.
			painter->SetColor(color_[0], 0);
			painter->SetAlphaValue(color_[0].alpha_);
			PixelRect _rect(GetScreenPos(), GetScreenPos() + GetSize());
			painter->DrawRoundedRect(_rect, corner_radius_, corner_radius_mask_, true);
		}
	}

	painter->PopAttrib();
}

bool RectComponent::IsOver(int screen_x, int screen_y) {
	if (behave_solid_ == true) {
		return Component::IsOver(screen_x, screen_y);
	}

	if (Component::IsOver(screen_x, screen_y) == false) {
		return false;
	}

	if (image_id_ == Painter::kInvalidImageid) {
		if (hollow_ == true) {
			Component* child = Component::GetChild(screen_x, screen_y);
			if (child != 0) {
				return child->IsOver(screen_x, screen_y);
			}
			return false;
		} else {
			return true;
		}
	} else {
		PixelCoord pos(GetScreenPos());
		PixelCoord size(GetSize());
		PixelRect rect(pos, pos + size);

		GUIImageManager* i_man = GetImageManager();

		return i_man->IsOverImage(image_id_, screen_x, screen_y, rect);
	}
}



void RectComponent::SetIsHollow(bool is_hollow) {
	hollow_ = is_hollow;
}



void RectComponent::SetImage(Painter::ImageID image_id) {
	image_id_ = image_id;
	SetNeedsRepaint(true);
}

void RectComponent::SetColor(const Color& color) {
	shaded_ = false;
	SetNeedsRepaint(color_[0] != color);
	color_[0] = color;
	if (color_[0].alpha_ == 0) {
		hollow_ = true;
	}
}

void RectComponent::SetColor(const Color& top_left_color, const Color& top_right_color, const Color& bottom_right_color, const Color& bottom_left_color) {
	shaded_ = true;
	SetNeedsRepaint(color_[0] != top_left_color || color_[1] != top_right_color ||
			color_[2] != bottom_right_color || color_[3] != bottom_left_color);
	color_[0] = top_left_color;
	color_[1] = top_right_color;
	color_[2] = bottom_right_color;
	color_[3] = bottom_left_color;
	if (color_[0].alpha_ == 0) {
		hollow_ = true;
	}
}

Painter::ImageID RectComponent::GetImage() {
	return image_id_;
}

Color RectComponent::GetColor() {
	return color_[0];
}

void RectComponent::GetColor(Color& top_left_color,
			     Color& top_right_color,
			     Color& bottom_right_color,
			     Color& bottom_left_color) {
	top_left_color     = color_[0];
	top_right_color    = color_[1];
	bottom_right_color = color_[2];
	bottom_left_color  = color_[3];
}

Component::Type RectComponent::GetType() const {
	return Component::kRectcomponent;
}

bool RectComponent::IsHollow() {
	return hollow_;
}

bool RectComponent::IsShaded() {
	return shaded_;
}

void RectComponent::SetBehaveSolid(bool behave_solid) {
	behave_solid_ = behave_solid;
}

bool RectComponent::GetBehaveSolid() const {
	return behave_solid_;
}



int RectComponent::GetCornerRadius() const {
	return corner_radius_;
}

void RectComponent::SetCornerRadius(int radius) {
	corner_radius_ = radius;
}

void RectComponent::SetCornerRadiusMask(int mask) {
	corner_radius_mask_ = mask;
}



}
