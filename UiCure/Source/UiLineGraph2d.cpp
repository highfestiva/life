
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiLineGraph2d.h"
#include <assert.h>



namespace UiCure
{



LineGraph2d::Segment::Segment(double pp1, double pp2):
	p1(pp1),
	p2(pp2)
{
}



LineGraph2d::LineGraph2d(UiTbc::Painter* mPainter):
	mPainter(mPainter)
{
	mDisplayListId = mPainter->NewDisplayList();
}

LineGraph2d::LineGraph2d(const LineGraph2d& pCopy):
	mPainter(pCopy.mPainter),
	mData(pCopy.mData),
	mNames(pCopy.mNames)
{
	
	mDisplayListId = mPainter->NewDisplayList();
}

LineGraph2d::~LineGraph2d()
{
	mPainter->DeleteDisplayList(mDisplayListId);
	mPainter = 0;
}

void LineGraph2d::TickLine(size_t pNewSize)
{
	assert(pNewSize > 0);
	while (mData.size()+1 > pNewSize)
	{
		mData.pop_front();
	}
	mData.push_back(Segments());
	mNames.clear();
}

void LineGraph2d::AddSegment(const str& pName, double p1, double p2)
{
	assert(mData.size() > 0);
	mData.back().push_back(Segment(p1, p2));
	mNames.push_back(pName);
}

void LineGraph2d::Render(int pMarginX, float pScaleX, int& pOffsetY) const
{
	int y = pOffsetY;
	for (Fill::const_iterator iy = mData.begin(); iy != mData.end(); ++iy, ++y)
	{
		const Segments& lLines = *iy;
		int lIndex = 0;
		for (Segments::const_iterator ix = lLines.begin(); ix != lLines.end(); ++ix, ++lIndex)
		{
			ColorPicker lPicker(lIndex, mPainter);
			const int x1 = (int)(ix->p1*pScaleX) + pMarginX;
			const int x2 = (int)(ix->p2*pScaleX) + pMarginX;
			mPainter->DrawLine(x1, y, x2, y);
		}
	}
	pOffsetY = y;
}

void LineGraph2d::RenderNames(int x, int& pIoY) const
{
	assert(mData.size() > 0);
	const Segments& lLines = mData.front();
	int lIndex = 0;
	Segments::const_iterator ix = lLines.begin();
	//const double lLongest = ix->x2 - ix->x1;
	strutil::strvec::const_iterator in = mNames.begin();
	int y = pIoY;
	for (; ix != lLines.end() && in != mNames.end(); ++lIndex, ++ix, ++in, y+=10)
	{
		ColorPicker lPicker(lIndex, mPainter);
		mPainter->FillRect(x, y+1, x+8, y+8);
		//const double lPercent = (ix->x2 - ix->x1) / lLongest;
		mPainter->PrintText(*in, x+12, y);
	}
	pIoY = y;
}



LineGraph2d::ColorPicker::ColorPicker(int pIndex, UiTbc::Painter* mPainter)
{
	/*int lColor = 0;
	for (int lBit = 0; lBit < 24; ++lBit)
	{
		const int lByte = 2-lBit%3;
		const int lColorBit = lByte*8 + 7-lBit/3;
		lColor |= ((pIndex>>lBit)&1)<<lColorBit;
	}*/
	const int lColorTable[] =
	{
		0x101010, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF,
		0x404040, 0x600000, 0x006000, 0x606000, 0x000060, 0x600060, 0x006060, 0x909090,
		0xFF8000, 0xFF0080, 0x80FF00, 0x00FF80, 0x8000FF, 0x0080FF, 0x804000, 0x800040,
	};
	int lColor = lColorTable[pIndex%(sizeof(lColorTable)/sizeof(lColorTable[0]))];
	typedef uint8 u8;
	mPainter->SetColor((u8)(lColor>>16), (u8)(lColor>>8), (u8)lColor, 255);
}



void LineGraph2d::operator=(const LineGraph2d&)
{
	assert(false);
}




}
