/*
	Class:  ProgressBar
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uiprogressbar.h"
#include <math.h>

namespace uitbc {
ProgressBar::ProgressBar(Style style,
						 const Color& body_color,
						 const Color& progress_color_left,
						 const Color& progress_color_right) :
	Window(kBorderSunken | kBorderLinearshading, 2, body_color),
	style_(style),
	box_size_(16),
	max_(1),
	pos_(0),
	user_defined_gfx_(false),
	progress_color_left_(progress_color_left),
	progress_color_right_(progress_color_right),
	progress_image_id_(Painter::kInvalidImageid) {
	Init();
}

ProgressBar::ProgressBar(Style style, Painter::ImageID background_image_id, Painter::ImageID progress_image_id):
	Window(background_image_id),
	style_(style),
	box_size_(16),
	max_(1),
	pos_(0),
	user_defined_gfx_(true),
	progress_color_left_(0, 0, 0),
	progress_color_right_(0, 0, 0),
	progress_image_id_(progress_image_id) {
	Init();
}

void ProgressBar::SetProgressMax(int max) {
	max_ = max;
	SetNeedsRepaint(true);
}

int ProgressBar::GetProgressMax() {
	return max_;
}

void ProgressBar::SetProgressPos(int pos) {
	pos_ = pos;
	SetNeedsRepaint(true);
}

int ProgressBar::GetProgressPos() {
	return pos_;
}

int ProgressBar::Step() {
	pos_++;
	pos_ = pos_ < 0 ? 0 : (pos_ > max_ ? max_ : pos_);
	SetNeedsRepaint(true);
	return pos_;
}

int ProgressBar::Step(int steps) {
	pos_ += steps;
	pos_ = pos_ < 0 ? 0 : (pos_ > max_ ? max_ : pos_);
	SetNeedsRepaint(true);
	return pos_;
}

void ProgressBar::Repaint(Painter* painter) {
	GUIImageManager* i_man = GetImageManager();

	Parent::Repaint(painter);

	painter->PushAttrib(Painter::kAttrAll);

	PixelRect rect = GetClientRect();
	painter->SetClippingRect(rect);

	int progress = (int)floor(((float64)pos_ / (float64)max_) * (float64)rect.GetWidth() + 0.5);
	int num_boxes = progress / (box_size_ + 1);
	if (progress % (box_size_ + 1) > 0) {
		num_boxes++;
	}

	if (user_defined_gfx_ == true) {
		if (style_ == kStyleNormal) {
			rect.right_ = rect.left_ + progress + 1;
			i_man->DrawImage(progress_image_id_, rect);
		} else {
			PixelRect box_rect(rect.left_, rect.top_, rect.left_ + (box_size_ + 1) * num_boxes, rect.bottom_);
			i_man->DrawImage(progress_image_id_, box_rect);
		}
	} else {
		if (style_ == kStyleNormal) {
			Color right(progress_color_left_, progress_color_right_, pos_/(float)max_);
			rect.right_ = rect.left_ + progress;
			painter->SetColor(progress_color_left_, 0);
			painter->SetColor(right, 1);
			painter->SetColor(right, 2);
			painter->SetColor(progress_color_left_, 3);
			painter->FillShadedRect(rect);
		} else {
			for (int i = 0; i < num_boxes; i++) {
				int x = i * (box_size_ + 1);
				painter->SetColor(Color(progress_color_left_, progress_color_right_, (x + (box_size_ + 1) * 0.5f) / rect.GetWidth()));
				painter->SetAlphaValue(progress_color_left_.alpha_);
				painter->FillRect(x + rect.left_, rect.top_, x + rect.left_ + box_size_, rect.bottom_);
			}
		}
	}

	painter->PopAttrib();
}

}
