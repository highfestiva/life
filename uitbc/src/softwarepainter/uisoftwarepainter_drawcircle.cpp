/*
	lepra::File:   SoftwarePainter_DrawCircle.cpp
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "pch.h"
#include "../../include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

#include <math.h>

namespace uitbc {

#define MACRO_DRAWCIRCLE(x, y, radius, value, type, unitsize, color, drawpixelmacro) \
{ \
	lepra::Canvas* canvas = value->canvas_; \
	lepra::PixelRect& clipping_rect = value->clipping_rect_; \
 \
	lepra::float64 __y; \
	int __y; \
	int prev_span_left_x = (int)(-radius); \
	type* screen = (type*)canvas->GetBuffer(); \
 \
	for (__y = 0, __y = 0; __y <= radius + 1.0f; __y += 1.0f, __y++) \
	{ \
		int half_span_width = (int)(cos(asin(__y / radius)) * radius); \
		int lower_span_y = y + __y; \
		int upper_span_y = y - __y; \
 \
		if ( upper_span_y >= clipping_rect.top_ && \
			upper_span_y <  clipping_rect.bottom_) \
		{ \
			int offset = upper_span_y * canvas->GetPitch(); \
 \
			for (int __x = prev_span_left_x; __x <= -half_span_width; __x++) \
			{ \
				int left_x = x + __x; \
				int right_x = x - __x; \
 \
				if ( left_x >= clipping_rect.left_ && \
					left_x <  clipping_rect.right_) \
				{ \
					type* dest = &screen[(offset + left_x) * unitsize]; \
					drawpixelmacro(dest, color); \
				} \
				if ( right_x >= clipping_rect.left_ && \
					right_x <  clipping_rect.right_) \
				{ \
					type* dest = &screen[(offset + right_x) * unitsize]; \
					drawpixelmacro(dest, color); \
				} \
			} \
		} \
 \
		if ( lower_span_y >= clipping_rect.top_ && \
			lower_span_y <  clipping_rect.bottom_) \
		{ \
			int offset = lower_span_y * canvas->GetPitch(); \
 \
			for (int __x = prev_span_left_x; __x <= -half_span_width; __x++) \
			{ \
				int left_x = x + __x; \
				int right_x = x - __x; \
 \
				if ( left_x >= clipping_rect.left_ && \
					left_x <  clipping_rect.right_) \
				{ \
					type* dest = &screen[(offset + left_x) * unitsize]; \
					drawpixelmacro(dest, color); \
				} \
				if ( right_x >= clipping_rect.left_ && \
					right_x <  clipping_rect.right_) \
				{ \
					type* dest = &screen[(offset + right_x) * unitsize]; \
					drawpixelmacro(dest, color); \
				} \
			} \
		} \
 \
		prev_span_left_x = -half_span_width; \
	} \
}

void SoftwarePainter::DrawCircle8BitNormal(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::uint8 __color = value->color_[0];
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint8, 1, __color, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawCircle16BitNormal(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::uint16 __color = value->color_[0];
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint16, 1, __color, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawCircle24BitNormal(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::Color& __color = value->color_[0];
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint8, 3, __color, MACRO_DrawPixel24BitNormal);
}

void SoftwarePainter::DrawCircle32BitNormal(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	if (value->mIncrementalAlpha == true) {
		lepra::Color& __color = value->color_[0];
		MACRO_DRAWCIRCLE(x, y, _radius, value,
			lepra::uint8, 4, __color, MACRO_DrawPixel32BitNormal);
	} else {
		unsigned __color = value->color_[0];
		MACRO_DRAWCIRCLE(x, y, _radius, value,
			unsigned, 1, __color, MACRO_DrawPixelNormal);
	}
}

void SoftwarePainter::DrawCircle15BitBlend(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	BlendColor __color(value->color_[0], value->alpha_value_);
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint16, 1, __color, MACRO_DrawPixel15BitBlend);
}

void SoftwarePainter::DrawCircle16BitBlend(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	BlendColor __color(value->color_[0], value->alpha_value_);
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint16, 1, __color, MACRO_DrawPixel16BitBlend);
}

void SoftwarePainter::DrawCircle24BitBlend(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	BlendColor __color(value->color_[0], value->alpha_value_);
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint8, 3, __color, MACRO_DrawPixelBlend);
}

void SoftwarePainter::DrawCircle32BitBlend(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	BlendColor __color(value->color_[0], value->alpha_value_);
	if (value->mIncrementalAlpha == true) {
		MACRO_DRAWCIRCLE(x, y, _radius, value,
			lepra::uint8, 4, __color, MACRO_DrawPixel32BitBlend);
	} else {
		MACRO_DRAWCIRCLE(x, y, _radius, value,
			lepra::uint8, 4, __color, MACRO_DrawPixelBlend);
	}
}

void SoftwarePainter::DrawCircle8BitXor(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::uint8 __color = value->color_[0];
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint8, 1, __color, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawCircle16BitXor(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::uint16 __color = value->color_[0];
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint16, 1, __color, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawCircle24BitXor(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::Color& __color = value->color_[0];
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint8, 3, __color, MACRO_DrawPixel24BitXor);
}

void SoftwarePainter::DrawCircle32BitXor(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	if (value->mIncrementalAlpha == true) {
		lepra::Color& __color = value->color_[0];
		MACRO_DRAWCIRCLE(x, y, _radius, value,
			lepra::uint8, 4, __color, MACRO_DrawPixel32BitXor);
	} else {
		unsigned __color = value->color_[0];
		MACRO_DRAWCIRCLE(x, y, _radius, value,
			unsigned, 1, __color, MACRO_DrawPixelXor);
	}
}

void SoftwarePainter::DrawCircle8BitAdd(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::uint8 __color = value->color_[0];
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint8, 1, __color, MACRO_DrawPixel8BitAdd);
}

void SoftwarePainter::DrawCircle15BitAdd(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::Color __color(value->color_[0]);
	__color.red_   >>= 3;
	__color.green_ >>= 3;
	__color.blue_  >>= 3;

	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint16, 1, __color, MACRO_DrawPixel15BitAdd);
}

void SoftwarePainter::DrawCircle16BitAdd(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::Color __color(value->color_[0]);
	__color.red_   >>= 3;
	__color.green_ >>= 2;
	__color.blue_  >>= 3;

	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint16, 1, __color, MACRO_DrawPixel16BitAdd);
}

void SoftwarePainter::DrawCircle24BitAdd(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::Color& __color = value->color_[0];
	MACRO_DRAWCIRCLE(x, y, _radius, value,
		lepra::uint8, 3, __color, MACRO_DrawPixelAdd);
}

void SoftwarePainter::DrawCircle32BitAdd(int x, int y, lepra::float64 _radius, SoftwarePainter* value) {
	lepra::Color& __color = value->color_[0];

	if (value->mIncrementalAlpha == true) {
		MACRO_DRAWCIRCLE(x, y, _radius, value,
			lepra::uint8, 4, __color, MACRO_DrawPixelAdd32Bit);
	} else {
		MACRO_DRAWCIRCLE(x, y, _radius, value,
			lepra::uint8, 4, __color, MACRO_DrawPixelAdd);
	}
}

#undef MACRO_DRAWCIRCLE

}
