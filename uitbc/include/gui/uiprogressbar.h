/*
	Class:  ProgressBar
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "uiwindow.h"
#include "../../../lepra/include/progresscallback.h"

namespace uitbc {

class ProgressBar: public Window, public ProgressCallback {
	typedef Window Parent;
public:

	enum Style {
		kStyleNormal = 0,
		kStyleBoxes,
	};


	ProgressBar(Style style = kStyleNormal,
				const Color& body_color = Color(192, 192, 192),
				const Color& progress_color_left  = Color(0, 0, 255),
				const Color& progress_color_right = Color(0, 0, 255));

	ProgressBar(Style style,
				Painter::ImageID background_image_id,
				Painter::ImageID progress_image_id);

	inline Style GetStyle();

	inline int GetBoxSize();
	inline void SetBoxSize(int box_size);

	virtual void   SetProgressMax(int max);
	virtual int  GetProgressMax();
	virtual void   SetProgressPos(int pos);
	virtual int  GetProgressPos();
	virtual int  Step();
	virtual int  Step(int steps);

	virtual void Repaint(Painter* painter);

protected:
private:

	Style style_;
	int box_size_;
	int max_;
	int pos_;

	bool user_defined_gfx_;

	Color progress_color_left_;
	Color progress_color_right_;

	Painter::ImageID progress_image_id_;
};

ProgressBar::Style ProgressBar::GetStyle() {
	return style_;
}

int ProgressBar::GetBoxSize() {
	return box_size_;
}

void ProgressBar::SetBoxSize(int box_size) {
	box_size_ = box_size;
}

}
