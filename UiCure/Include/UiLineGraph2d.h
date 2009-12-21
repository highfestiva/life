
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <list>
#include "../../Lepra/Include/String.h"
#include "../../UiTbc/Include/UiPainter.h"
#include "../Include/UiCure.h"



namespace UiCure
{



struct LineGraph2d
{
	LineGraph2d(UiTbc::Painter* mPainter);
	LineGraph2d(const LineGraph2d&);
	virtual ~LineGraph2d();

	void TickLine(size_t pNewSize);	// Rolls up the graph by one step and inserts a "blank line".
	void AddSegment(const str& pName, double p1, double p2);
	void Render(int pMarginX, float pScaleX, int& pOffsetY) const;
	void RenderNames(int x, int& y) const;

private:
	class ColorPicker
	{
	public:
		ColorPicker(int pIndex, UiTbc::Painter* pPainter);
	};
	struct Segment
	{
		Segment(double pp1, double pp2);
		double p1;
		double p2;
	};
	typedef std::vector<Segment> Segments;
	typedef std::list<Segments> Fill;
	typedef strutil::strvec SegmentNames;

	UiTbc::Painter* mPainter;
	UiTbc::Painter::DisplayListID mDisplayListId;

	Fill mData;
	SegmentNames mNames;

	void operator=(const LineGraph2d&);
};



}
