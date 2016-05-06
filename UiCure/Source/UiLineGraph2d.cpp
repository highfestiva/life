
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uilinegraph2d.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/lepraassert.h"



namespace UiCure {



LineGraph2d::Segment::Segment(double pp1, double pp2):
	p1(pp1),
	p2(pp2) {
}



LineGraph2d::LineGraph2d(uitbc::Painter* painter_):
	painter_(painter_) {
	display_list_id_ = painter_->NewDisplayList();
}

LineGraph2d::LineGraph2d(const LineGraph2d& copy):
	painter_(copy.painter_),
	data_(copy.data_),
	names_(copy.names_) {
	display_list_id_ = painter_->NewDisplayList();
}

void LineGraph2d::operator=(const LineGraph2d& copy) {
	painter_ = copy.painter_;
	data_ = copy.data_;
	names_ = copy.names_;
	display_list_id_ = painter_->NewDisplayList();
}

LineGraph2d::~LineGraph2d() {
	painter_->DeleteDisplayList(display_list_id_);
	painter_ = 0;
}

void LineGraph2d::TickLine(size_t new_size) {
	deb_assert(new_size > 0);
	while (data_.size()+1 > new_size) {
		data_.pop_front();
	}
	data_.push_back(Segments());
	names_.clear();
}

void LineGraph2d::AddSegment(const str& name, double p1, double p2) {
	deb_assert(data_.size() > 0);
	data_.back().push_back(Segment(p1, p2));
	names_.push_back(name);
}

void LineGraph2d::Render(int margin_x, float scale_x, int& offset_y) const {
	int y = offset_y;
	for (Fill::const_iterator iy = data_.begin(); iy != data_.end(); ++iy, ++y) {
		const Segments& lines = *iy;
		int _index = 0;
		for (Segments::const_iterator ix = lines.begin(); ix != lines.end(); ++ix, ++_index) {
			ColorPicker picker(_index, painter_);
			const int x1 = (int)(ix->p1*scale_x) + margin_x;
			const int x2 = (int)(ix->p2*scale_x) + margin_x;
			painter_->DrawLine(x1, y, x2, y);
		}
	}
	offset_y = y;
}

void LineGraph2d::RenderNames(int x, int& io_y) const {
	deb_assert(data_.size() > 0);
	const Segments& lines = data_.front();
	int _index = 0;
	Segments::const_iterator ix = lines.begin();
	//const double lLongest = ix->x2 - ix->x1;
	strutil::strvec::const_iterator in = names_.begin();
	int y = io_y;
	for (; ix != lines.end() && in != names_.end(); ++_index, ++ix, ++in, y+=10) {
		ColorPicker picker(_index, painter_);
		painter_->FillRect(x, y+1, x+8, y+8);
		//const double lPercent = (ix->x2 - ix->x1) / lLongest;
		painter_->PrintText(wstrutil::Encode(*in), x+12, y);
	}
	io_y = y;
}



LineGraph2d::ColorPicker::ColorPicker(int index, uitbc::Painter* painter_) {
	/*int color = 0;
	for (int lBit = 0; lBit < 24; ++lBit) {
		const int lByte = 2-lBit%3;
		const int lColorBit = lByte*8 + 7-lBit/3;
		color |= ((index>>lBit)&1)<<lColorBit;
	}*/
	const int color_table[] =
	{
		0x101010, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF,
		0x404040, 0x600000, 0x006000, 0x606000, 0x000060, 0x600060, 0x006060, 0x909090,
		0xFF8000, 0xFF0080, 0x80FF00, 0x00FF80, 0x8000FF, 0x0080FF, 0x804000, 0x800040,
	};
	int color = color_table[index%LEPRA_ARRAY_COUNT(color_table)];
	if (index >= LEPRA_ARRAY_COUNT(color_table)) {
		color >>= 1;
		color &= 0x7F7F7F;
	}
	typedef uint8 u8;
	painter_->SetColor((u8)(color>>16), (u8)(color>>8), (u8)color, 255);
}



}
