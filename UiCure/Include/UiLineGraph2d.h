
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "../../lepra/include/string.h"
#include "../../uitbc/include/uipainter.h"
#include "../include/uicure.h"



namespace UiCure {



struct LineGraph2d {
	LineGraph2d(uitbc::Painter* painter_);
	LineGraph2d(const LineGraph2d&);
	void operator=(const LineGraph2d&);
	virtual ~LineGraph2d();

	void TickLine(size_t new_size);	// Rolls up the graph by one step and inserts a "blank line".
	void AddSegment(const str& name, double p1, double p2);
	void Render(int margin_x, float scale_x, int& offset_y) const;
	void RenderNames(int x, int& y) const;

private:
	class ColorPicker {
	public:
		ColorPicker(int index, uitbc::Painter* painter);
	};
	struct Segment {
		Segment(double pp1, double pp2);
		double p1;
		double p2;
	};
	typedef std::vector<Segment> Segments;
	typedef std::list<Segments> Fill;
	typedef strutil::strvec SegmentNames;

	uitbc::Painter* painter_;
	uitbc::Painter::DisplayListID display_list_id_;

	Fill data_;
	SegmentNames names_;
};



}
