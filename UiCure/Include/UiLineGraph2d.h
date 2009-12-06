
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include <list>
#include "../../Lepra/Include/String.h"



namespace UiTbc
{
class Painter;
}



namespace UiCure
{



// This is really just a container for being able to draw a "2D line graph".
struct LineGraph2d
{
	void TickLine(size_t pNewSize);	// Rolls up the graph by one step and inserts a "blank line".
	void AddSegment(const Lepra::String& pName, double p1, double p2);
	void Render(UiTbc::Painter* pPainter, int pMarginX, float pScaleX, int& pOffsetY) const;
	void RenderNames(UiTbc::Painter* pPainter, int x, int& y) const;

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
	typedef Lepra::StringUtility::StringVector SegmentNames;

	Fill mData;
	SegmentNames mNames;
};



}
