
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uibordercomponent.h"
#include <math.h>
#include "../../include/gui/uidesktopwindow.h"



namespace uitbc {



BorderComponent::BorderComponent(BorderPart part, const Color& color, BorderShadeFunc shade_func):
	RectComponent(color),
	image_border_(false),
	resizable_(false),
	resizing_(false),
	part_(part),
	shading_(shade_func),
	body_color_(color) {
	Set(color, shade_func);
}

BorderComponent::BorderComponent(BorderPart part, Painter::ImageID image_id):
	RectComponent(image_id),
	image_border_(true),
	resizable_(false),
	resizing_(false),
	part_(part),
	shading_(kLinear) {
	GUIImageManager* i_man = GetImageManager();
	PixelCoord image_size(i_man->GetImageSize(image_id));

	switch(part_) {
	case kTopleftCorner:
	case kToprightCorner:
	case kBottomrightCorner:
	case kBottomleftCorner:
		SetPreferredSize(image_size.x, image_size.y);
		break;
	case kTopBorder:
	case kBottomBorder:
		SetPreferredSize(0, image_size.y);
		break;
	case kLeftBorder:
	case kRightBorder:
		SetPreferredSize(image_size.x, 0);
		break;
	}
}

BorderComponent::~BorderComponent() {
}

void BorderComponent::SetSunken(bool sunken) {
	CalcColors();
	if (sunken == true) {
		Color temp(light_color_);
		light_color_ = dark_color_;
		dark_color_ = temp;
	}
}

void BorderComponent::Set(const Color& color, BorderShadeFunc shade_func) {
	body_color_ = color;
	shading_ = shade_func;

	CalcColors();
	RectComponent::SetColor(color);

	image_border_ = false;
}

void BorderComponent::Set(Painter::ImageID image_id) {
	RectComponent::SetImage(image_id);
	image_border_ = true;
}

void BorderComponent::CalcColors() {
	light_color_ = body_color_ + Color(
		(uint8)((256-(int)body_color_.red_)/2),
		(uint8)((256-(int)body_color_.green_)/2),
		(uint8)((256-(int)body_color_.blue_)/2),
		body_color_.alpha_) + 8;
	dark_color_ = body_color_ / 3.0f;
}

void BorderComponent::Repaint(Painter* painter) {
	if (image_border_ == true) {
		Parent::Repaint(painter);
	} else {
		PixelRect rect(GetScreenPos(), GetScreenPos() + GetSize());

		painter->PushAttrib(Painter::kAttrAll);
		painter->SetAlphaValue(body_color_.alpha_);

		switch(part_) {
			case kTopleftCorner: {
				PixelCoord top_left(rect.left_, rect.top_);
				PixelCoord top_right(rect.right_, rect.top_);
				PixelCoord bottom_left(rect.left_, rect.bottom_);
				PixelCoord bottom_right(rect.right_, rect.bottom_);

				if (shading_ == kLinear) {
					painter->SetColor(light_color_, 0);
					painter->SetColor(light_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->FillShadedTriangle(top_left, top_right, bottom_right);
					painter->FillShadedTriangle(bottom_left, top_left, bottom_right);
				} else {
					PixelCoord middle(rect.GetCenterX(), rect.GetCenterY());
					PixelCoord middle_bottom(rect.GetCenterX(), rect.bottom_);
					PixelCoord middle_right(rect.right_, rect.GetCenterY());

					painter->SetColor(body_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(light_color_, 2);

					painter->FillShadedTriangle(bottom_left, top_left, middle);
					painter->FillShadedTriangle(top_left, top_right, middle);

					painter->SetColor(light_color_, 1);

					painter->FillShadedTriangle(bottom_left, middle, middle_bottom);
					painter->FillShadedTriangle(top_right, middle_right, middle);
					painter->FillShadedTriangle(bottom_right, middle, middle_right);
					painter->FillShadedTriangle(bottom_right, middle_bottom, middle);
				}
			} break;
			case kToprightCorner: {
				PixelCoord top_left(rect.left_, rect.top_);
				PixelCoord top_right(rect.right_, rect.top_);
				PixelCoord bottom_left(rect.left_, rect.bottom_);
				PixelCoord bottom_right(rect.right_, rect.bottom_);

				if (shading_ == kLinear) {
					painter->SetColor(light_color_, 0);
					painter->SetColor(light_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->FillShadedTriangle(top_left, top_right, bottom_left);

					painter->SetColor(dark_color_, 0);
					painter->SetColor(dark_color_, 1);
					painter->FillShadedTriangle(top_right, bottom_right, bottom_left);
				} else {
					PixelCoord middle(rect.GetCenterX(), rect.GetCenterY());
					PixelCoord middle_bottom(rect.GetCenterX(), rect.bottom_);
					PixelCoord middle_left(rect.left_, rect.GetCenterY());

					painter->SetColor(body_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(light_color_, 2);

					painter->FillShadedTriangle(top_left, top_right, middle);

					painter->SetColor(dark_color_, 2);
					painter->FillShadedTriangle(top_right, bottom_right, middle);

					painter->SetColor(dark_color_, 1);

					painter->FillShadedTriangle(bottom_right, middle_bottom, middle);
					painter->FillShadedTriangle(bottom_left, middle, middle_bottom);

					painter->SetColor(light_color_, 1);
					painter->SetColor(light_color_, 2);
					painter->FillShadedTriangle(bottom_left, middle_left, middle);
					painter->FillShadedTriangle(top_left, middle, middle_left);
				}
			} break;
			case kBottomrightCorner: {
				PixelCoord top_left(rect.left_, rect.top_);
				PixelCoord top_right(rect.right_, rect.top_);
				PixelCoord bottom_left(rect.left_, rect.bottom_);
				PixelCoord bottom_right(rect.right_, rect.bottom_);

				if (shading_ == kLinear) {
					painter->SetColor(dark_color_, 0);
					painter->SetColor(dark_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->FillShadedTriangle(bottom_right, bottom_left, top_left);
					painter->FillShadedTriangle(top_right, bottom_right, top_left);
				} else {
					PixelCoord middle(rect.GetCenterX(), rect.GetCenterY());
					PixelCoord middle_top(rect.GetCenterX(), rect.top_);
					PixelCoord middle_left(rect.left_, rect.GetCenterY());

					painter->SetColor(body_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(dark_color_, 2);

					painter->FillShadedTriangle(bottom_right, bottom_left, middle);
					painter->FillShadedTriangle(top_right, bottom_right, middle);

					painter->SetColor(dark_color_, 1);

					painter->FillShadedTriangle(bottom_left, middle_left, middle);
					painter->FillShadedTriangle(top_left, middle, middle_left);
					painter->FillShadedTriangle(top_left, middle_top, middle);
					painter->FillShadedTriangle(top_right, middle, middle_top);
				}
			} break;
			case kBottomleftCorner: {
				PixelCoord top_left(rect.left_, rect.top_);
				PixelCoord top_right(rect.right_, rect.top_);
				PixelCoord bottom_left(rect.left_, rect.bottom_);
				PixelCoord bottom_right(rect.right_, rect.bottom_);

				if (shading_ == kLinear) {
					painter->SetColor(light_color_, 0);
					painter->SetColor(light_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->FillShadedTriangle(bottom_left, top_left, top_right);

					painter->SetColor(dark_color_, 0);
					painter->SetColor(dark_color_, 1);
					painter->FillShadedTriangle(bottom_right, bottom_left, top_right);
				} else {
					PixelCoord middle(rect.GetCenterX(), rect.GetCenterY());
					PixelCoord middle_top(rect.GetCenterX(), rect.top_);
					PixelCoord middle_right(rect.right_, rect.GetCenterY());

					painter->SetColor(body_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(light_color_, 2);

					painter->FillShadedTriangle(bottom_left, top_left, middle);

					painter->SetColor(dark_color_, 2);
					painter->FillShadedTriangle(bottom_right, bottom_left, middle);

					painter->SetColor(dark_color_, 1);

					painter->FillShadedTriangle(bottom_right, middle, middle_right);
					painter->FillShadedTriangle(top_right, middle_right, middle);

					painter->SetColor(light_color_, 1);
					painter->SetColor(light_color_, 2);
					painter->FillShadedTriangle(top_left, middle_top, middle);
					painter->FillShadedTriangle(top_right, middle, middle_top);
				}
			} break;
			case kTopBorder: {
				if (shading_ == kLinear) {
					painter->SetColor(light_color_, 0);
					painter->SetColor(light_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->SetColor(body_color_, 3);
					painter->FillShadedRect(rect);
				} else {
					int middle_y = rect.GetCenterY();
					PixelRect top_rect(rect.left_, rect.top_, rect.right_, middle_y);
					PixelRect bottom_rect(rect.left_, middle_y, rect.right_, rect.bottom_);

					painter->SetColor(body_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(light_color_, 2);
					painter->SetColor(light_color_, 3);
					painter->FillShadedRect(top_rect);

					painter->SetColor(light_color_, 0);
					painter->SetColor(light_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->SetColor(body_color_, 3);
					painter->FillShadedRect(bottom_rect);
				}
			} break;
			case kBottomBorder: {
				if (shading_ == kLinear) {
					painter->SetColor(body_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(dark_color_, 2);
					painter->SetColor(dark_color_, 3);
					painter->FillShadedRect(rect);
				} else {
					int middle_y = rect.GetCenterY();
					PixelRect top_rect(rect.left_, rect.top_, rect.right_, middle_y);
					PixelRect bottom_rect(rect.left_, middle_y, rect.right_, rect.bottom_);

					painter->SetColor(body_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(dark_color_, 2);
					painter->SetColor(dark_color_, 3);
					painter->FillShadedRect(top_rect);

					painter->SetColor(dark_color_, 0);
					painter->SetColor(dark_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->SetColor(body_color_, 3);
					painter->FillShadedRect(bottom_rect);
				}
			} break;
			case kLeftBorder: {
				if (shading_ == kLinear) {
					painter->SetColor(light_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->SetColor(light_color_, 3);
					painter->FillShadedRect(rect);
				} else {
					int middle_x = rect.GetCenterX();
					PixelRect left_rect(rect.left_, rect.top_, middle_x, rect.bottom_);
					PixelRect right_rect(middle_x, rect.top_, rect.right_, rect.bottom_);

					painter->SetColor(body_color_, 0);
					painter->SetColor(light_color_, 1);
					painter->SetColor(light_color_, 2);
					painter->SetColor(body_color_, 3);
					painter->FillShadedRect(left_rect);

					painter->SetColor(light_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->SetColor(light_color_, 3);
					painter->FillShadedRect(right_rect);
				}
			} break;
			case kRightBorder: {
				if (shading_ == kLinear) {
					painter->SetColor(body_color_, 0);
					painter->SetColor(dark_color_, 1);
					painter->SetColor(dark_color_, 2);
					painter->SetColor(body_color_, 3);
					painter->FillShadedRect(rect);
				} else {
					int middle_x = rect.GetCenterX();
					PixelRect left_rect(rect.left_, rect.top_, middle_x, rect.bottom_);
					PixelRect right_rect(middle_x, rect.top_, rect.right_, rect.bottom_);

					painter->SetColor(body_color_, 0);
					painter->SetColor(dark_color_, 1);
					painter->SetColor(dark_color_, 2);
					painter->SetColor(body_color_, 3);
					painter->FillShadedRect(left_rect);

					painter->SetColor(dark_color_, 0);
					painter->SetColor(body_color_, 1);
					painter->SetColor(body_color_, 2);
					painter->SetColor(dark_color_, 3);
					painter->FillShadedRect(right_rect);
				}
			} break;
		}

		painter->PopAttrib();

		SetNeedsRepaint(false);
	}
}

bool BorderComponent::OnLButtonDown(int mouse_x, int mouse_y) {
	if (resizable_ == true && IsOver(mouse_x, mouse_y) == true) {
		resizing_ = true;
		SetMouseFocus();
		return true;
	} else {
		return Parent::OnLButtonDown(mouse_x, mouse_y);
	}
}

bool BorderComponent::OnLButtonUp(int mouse_x, int mouse_y) {
	if (resizing_ == true) {
		resizing_ = false;
		ReleaseMouseFocus();

		Component* d_win = GetParentOfType(kDesktopwindow);
		if (d_win != 0) {
			// Call OnMouseMove() to update the mouse cursor icon.
			d_win->OnMouseMove(mouse_x, mouse_y, 0, 0);
		}
	}

	return Parent::OnLButtonUp(mouse_x, mouse_y);
}

bool BorderComponent::OnMouseMove(int /*mouse_x*/, int /*mouse_y*/, int mouse_dx, int mouse_dy) {
	if (resizing_ == true) {
		Component* win = GetParentOfType(kWindow);
		PixelCoord size(win->GetSize());
		PixelCoord min_size(win->GetMinSize());
		PixelCoord new_size(size);
		PixelCoord move(0, 0);

		if (win != 0) {
			switch(part_) {
			case kTopleftCorner:
				new_size += PixelCoord(-mouse_dx, -mouse_dy);
				move.x = mouse_dx;
				move.y = mouse_dy;
				break;
			case kBottomrightCorner:
				new_size += PixelCoord(mouse_dx, mouse_dy);
				break;
			case kBottomleftCorner:
				new_size += PixelCoord(-mouse_dx, mouse_dy);
				move.x = mouse_dx;
				break;
			case kToprightCorner:
				new_size += PixelCoord(mouse_dx, -mouse_dy);
				move.y = mouse_dy;
				break;
			case kTopBorder:
				new_size += PixelCoord(0, -mouse_dy);
				move.y = mouse_dy;
				break;
			case kBottomBorder:
				new_size += PixelCoord(0, mouse_dy);
				break;
			case kLeftBorder:
				new_size += PixelCoord(-mouse_dx, 0);
				move.x = mouse_dx;
				break;
			case kRightBorder:
				new_size += PixelCoord(mouse_dx, 0);
				break;
			default:
				break;
			};

			if (new_size.x < min_size.x) {
				if (move.x > 0) {
					move.x -= (min_size.x - new_size.x);
				}

				new_size.x = min_size.x;
			}

			if (new_size.y < min_size.y) {
				if (move.y > 0) {
					move.y -= (min_size.y - new_size.y);
				}

				new_size.y = min_size.y;
			}

			win->SetPos(win->GetPos() + move);
			win->SetPreferredSize(new_size, false);
		}

		return true;
	} else if(resizable_ == true) {
		// TODO: set mouse cursor:
		//switch(part_)
		//{
		//case kTopleftCorner:
		//case kBottomrightCorner:
		//	lMTheme->LoadDiagonal1ResizeCursor();
		//	break;
		//case kBottomleftCorner:
		//case kToprightCorner:
		//	lMTheme->LoadDiagonal2ResizeCursor();
		//	break;
		//case kTopBorder:
		//case kBottomBorder:
		//	lMTheme->LoadVResizeCursor();
		//	break;
		//case kLeftBorder:
		//case kRightBorder:
		//	lMTheme->LoadHResizeCursor();
		//	break;
		//default:
		//	lMTheme->LoadArrowCursor();
		//};
		return true;
	} else {
		return false;
	}
}



}
