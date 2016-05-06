
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine




#pragma once

#include "gammalookup.h"
#include "math.h"



namespace lepra {



class PixelCoord {
public:
	PixelCoord() :
		x(0),
		y(0) {
	}

	PixelCoord(int _x, int _y) :
		x(_x),
		y(_y) {
	}

	PixelCoord(const PixelCoord& coords) :
		x(coords.x),
		y(coords.y) {
	}

	inline PixelCoord operator = (const PixelCoord& coords);
	inline PixelCoord operator += (const PixelCoord& coords);
	inline PixelCoord operator -= (const PixelCoord& coords);
	inline PixelCoord operator + (const PixelCoord& coords) const;
	inline PixelCoord operator - (const PixelCoord& coords) const;
	inline PixelCoord operator * (int factor) const;
	inline PixelCoord operator / (int dividend) const;

	inline bool operator == (const PixelCoord& coords) const;
	inline bool operator != (const PixelCoord& coords) const;

	inline float GetDistance(const PixelCoord& coords) const;

	int x, y;
};

PixelCoord PixelCoord::operator = (const PixelCoord& coords) {
	x = coords.x;
	y = coords.y;
	return *this;
}

PixelCoord PixelCoord::operator += (const PixelCoord& coords) {
	x += coords.x;
	y += coords.y;
	return *this;
}

PixelCoord PixelCoord::operator -= (const PixelCoord& coords) {
	x -= coords.x;
	y -= coords.y;
	return *this;
}

PixelCoord PixelCoord::operator + (const PixelCoord& coords) const {
	PixelCoord temp(*this);
	temp += coords;
	return temp;
}

PixelCoord PixelCoord::operator - (const PixelCoord& coords) const {
	PixelCoord temp(*this);
	temp -= coords;
	return temp;
}

PixelCoord PixelCoord::operator*(int factor) const {
	PixelCoord temp(*this);
	temp.x *= factor;
	temp.y *= factor;
	return temp;
}

PixelCoord PixelCoord::operator/(int dividend) const {
	PixelCoord temp(*this);
	temp.x /= dividend;
	temp.y /= dividend;
	return temp;
}

bool PixelCoord::operator == (const PixelCoord& coords) const {
	return (x == coords.x && y == coords.y);
}

bool PixelCoord::operator != (const PixelCoord& coords) const {
	return (x != coords.x || y != coords.y);
}

float PixelCoord::GetDistance(const PixelCoord& coords) const {
	int dx = x-coords.x;
	int dy = y-coords.y;
	return (::sqrt((float)(dx*dx + dy*dy)));
}



class PixelRect {
public:
	inline PixelRect();
	inline PixelRect(const PixelRect& rect);
	inline PixelRect(const PixelCoord& top_left, const PixelCoord& bottom_right);
	inline PixelRect(int left, int top, int right, int bottom);

	inline int GetWidth() const;
	inline int GetHeight() const;
	inline PixelCoord GetSize() const;

	inline int GetCenterX() const;
	inline int GetCenterY() const;

	inline bool IsInside(int x, int y) const;
	inline bool Overlap(const PixelRect& rect) const;
	inline bool VerticalOverlap(const PixelRect& rect) const;
	inline bool HorizontalOverlap(const PixelRect& rect) const;

	inline PixelRect GetOverlap(const PixelRect& rect) const;

	inline void Shrink(int num_pixels);
	inline void Enlarge(int num_pixels);
	inline void Offset(int x, int y);
	inline void Set(int left, int top, int right, int bottom);

	inline bool operator==(const PixelRect& rect) const;

	int top_;
	int bottom_;
	int left_;
	int right_;
};

PixelRect::PixelRect() {
	top_ = 0;
	bottom_ = 0;
	left_ = 0;
	right_ = 0;
}

PixelRect::PixelRect(const PixelRect& rect) :
	top_(rect.top_),
	bottom_(rect.bottom_),
	left_(rect.left_),
	right_(rect.right_) {
}

PixelRect::PixelRect(const PixelCoord& top_left, const PixelCoord& bottom_right) :
	top_(top_left.y),
	bottom_(bottom_right.y),
	left_(top_left.x),
	right_(bottom_right.x) {
}

PixelRect::PixelRect(int left, int top, int right, int bottom) :
	top_(top),
	bottom_(bottom),
	left_(left),
	right_(right) {
}

int PixelRect::GetWidth() const {
	return (right_ - left_);
}

int PixelRect::GetHeight() const {
	return (bottom_ - top_);
}

PixelCoord PixelRect::GetSize() const {
	return (PixelCoord(GetWidth(), GetHeight()));
}

int PixelRect::GetCenterX() const {
	return left_ + (GetWidth() >> 1);
}

int PixelRect::GetCenterY() const {
	return top_ + (GetHeight() >> 1);
}

bool PixelRect::IsInside(int x, int y) const {
	if (x >= left_ && x < right_ && y >= top_ && y < bottom_) {
		return true;
	}

	return false;
}

bool PixelRect::Overlap(const PixelRect& rect) const {
	if (rect.left_ > right_ ||
	   rect.right_ < left_ ||
	   rect.top_ > bottom_ ||
	   rect.bottom_ < top_) {
		return false;
	}

	return true;
}

bool PixelRect::VerticalOverlap(const PixelRect& rect) const {
	if (rect.top_ > bottom_ ||
	   rect.bottom_ < top_) {
		return false;
	}

	return true;
}

bool PixelRect::HorizontalOverlap(const PixelRect& rect) const {
	if (rect.left_ > right_ ||
	   rect.right_ < left_) {
		return false;
	}

	return true;
}

PixelRect PixelRect::GetOverlap(const PixelRect& rect) const {
	PixelRect _rect(*this);
	if (rect.left_ > _rect.left_) {
		_rect.left_ = rect.left_;
	}
	if (rect.right_ < _rect.right_) {
		_rect.right_ = rect.right_;
	}
	if (rect.top_ > _rect.top_) {
		_rect.top_ = rect.top_;
	}
	if (rect.bottom_ < _rect.bottom_) {
		_rect.bottom_ = rect.bottom_;
	}
	return _rect;
}

void PixelRect::Set(int left, int top, int right, int bottom) {
	top_ = top;
	bottom_ = bottom;
	left_ = left;
	right_ = right;
}

bool PixelRect::operator==(const PixelRect& rect) const {
	return (top_ == rect.top_ && bottom_ == rect.bottom_ &&
		left_ == rect.left_ && right_ == rect.right_);
}

void PixelRect::Shrink(int num_pixels) {
	top_		+= num_pixels;
	bottom_	-= num_pixels;
	left_		+= num_pixels;
	right_	-= num_pixels;
}

void PixelRect::Enlarge(int num_pixels) {
	Shrink(-num_pixels);
}

void PixelRect::Offset(int x, int y) {
	top_		+= y;
	bottom_	+= y;
	left_		+= x;
	right_	+= x;
}


#define CLEAR_COLOR	Color(0, 0, 0, 0)
#define BLACK		Color(0, 0, 0)
#define OFF_BLACK	Color(1, 1, 1)
#define WHITE		Color(255, 255, 255)
#define GRAY		Color(128, 128, 128)
#define RED		Color(255, 0, 0)
#define GREEN		Color(0, 255, 0)
#define BLUE		Color(0, 0, 255)
#define YELLOW		Color(255, 255, 0)
#define MAGENTA		Color(255, 0, 255)
#define PURPLE		Color(128, 0, 255)
#define CYAN		Color(0, 255, 255)
#define ORANGE		Color(255, 150, 0)
#define BROWN		Color(128, 80, 0)
#define PINK		Color(255, 128, 128)

#define DARK_GRAY	Color(64, 64, 64)
#define DARK_RED	Color(128, 0, 0)
#define DARK_GREEN	Color(0, 128, 0)
#define DARK_BLUE	Color(0, 0, 128)
#define DARK_YELLOW	Color(128, 128, 0)
#define DARK_MAGENTA	Color(128, 0, 128)
#define DARK_CYAN	Color(0, 128, 128)

#define LIGHT_GRAY	Color(192, 192, 192)
#define LIGHT_RED	Color(255, 64, 64)
#define LIGHT_GREEN	Color(64, 255, 64)
#define LIGHT_BLUE	Color(64, 64, 255)
#define LIGHT_YELLOW	Color(255, 255, 64)
#define LIGHT_MAGENTA	Color(255, 64, 255)
#define LIGHT_CYAN	Color(64, 255, 255)

class Color {
public:

	inline Color();
	inline Color(const Color& color);
	inline Color(uint8 red, uint8 green, uint8 blue);
	inline Color(uint8 red, uint8 green, uint8 blue, uint8 color_index);
	static inline Color CreateColor(float red, float green, float blue, float alpha);

	// Interpolation constructor.
	inline Color(const Color& color1, const Color& color2, float t);

	// Set()-overloads for all standard types. This makes life easier when setting the color,
	// because you don't have to typecast all variables to uint8 all the time.
	// The float overloads assumes that the rgb values are in the range [0, 1].
	// All integer types in the range [0, 255].
	inline void Set(int8 red, int8 green, int8 blue, int8 color_index);
	inline void Set(uint8 red, uint8 green, uint8 blue, uint8 color_index);
	inline void Set(int16   red,  int16   green,  int16   blue,  int16   color_index);
	inline void Set(uint16  red, uint16  green, uint16  blue, uint16  color_index);
	inline void Set(int32 red, int32 green, int32 blue, int32 color_index);
	inline void Set(uint32 red, uint32 green, uint32 blue, uint32 color_index);
	inline void Set(int64   red,  int64   green,  int64   blue,  int64   color_index);
	inline void Set(uint64  red, uint64  green, uint64  blue, uint64  color_index);
	inline void Set(float red,  float green,  float blue,  float color_index);

	inline float GetRf() const;
	inline float GetGf() const;
	inline float GetBf() const;
	inline float GetAf() const;
	inline uint32 To32() const;

	inline Color operator = (const Color& color);

	// The following operators will operate on all color components
	// (Red, Green, Blue) but NOT(!) the alpha channel.
	inline Color operator += (const Color& color);
	inline Color operator + (const Color& color) const;
	inline Color operator -= (const Color& color);
	inline Color operator - (const Color& color) const;

	inline Color operator += (int value);
	inline Color operator +  (int value) const;
	inline Color operator -= (int value);
	inline Color operator -  (int value) const;
	inline Color operator *= (int scalar);
	inline Color operator *  (int scalar) const;
	inline Color operator /= (int scalar);
	inline Color operator /  (int scalar) const;
	inline Color operator *= (float scalar);
	inline Color operator *  (float scalar) const;
	inline Color operator /= (float scalar);
	inline Color operator /  (float scalar) const;

	inline bool operator==(const Color& other) const;
	inline bool operator!=(const Color& other) const;

	inline int SumRgb() const;

	uint8 red_;
	uint8 green_;
	uint8 blue_;
	uint8 alpha_;	// Only used in 8-bit color mode.

private:
	inline int Max(int v1, int v2) const;
	inline int Min(int v1, int v2) const;
};

Color::Color() {
	red_   = 0;
	green_ = 0;
	blue_  = 0;
	alpha_ = 255;	// Treat as alpha.
}

Color::Color(const Color& color) {
	red_   = color.red_;
	green_ = color.green_;
	blue_  = color.blue_;
	alpha_ = color.alpha_;
}

Color::Color(uint8 red, uint8 green, uint8 blue) {
	red_   = red;
	green_ = green;
	blue_  = blue;
	alpha_ = 255;
}

Color::Color(uint8 red, uint8 green, uint8 blue, uint8 color_index) {
	red_   = red;
	green_ = green;
	blue_  = blue;
	alpha_ = color_index;
}

Color Color::CreateColor(float red, float green, float blue, float alpha) {
	Color c;
	c.Set(red, green, blue, alpha);
	return c;
}

Color::Color(const Color& color1, const Color& color2, float t) {
	const float r1 = GammaLookup::GammaToLinearFloat(color1.red_);
	const float g1 = GammaLookup::GammaToLinearFloat(color1.green_);
	const float b1 = GammaLookup::GammaToLinearFloat(color1.blue_);
	const float r2 = GammaLookup::GammaToLinearFloat(color2.red_);
	const float g2 = GammaLookup::GammaToLinearFloat(color2.green_);
	const float b2 = GammaLookup::GammaToLinearFloat(color2.blue_);

	red_   = GammaLookup::LinearToGamma((int)Math::Lerp(r1, r2, t));
	green_ = GammaLookup::LinearToGamma((int)Math::Lerp(g1, g2, t));
	blue_  = GammaLookup::LinearToGamma((int)Math::Lerp(b1, b2, t));
	alpha_ = (uint8)Math::Lerp((float)color1.alpha_, (1/255.0f)*color1.alpha_*color2.alpha_, t);
}

int Color::Max(int v1, int v2) const {
	return (v1 > v2 ? v1 : v2);
}

int Color::Min(int v1, int v2) const {
	return (v1 < v2 ? v1 : v2);
}

void Color::Set(int8 red, int8 green, int8 blue, int8 color_index) {
	Set((uint8)red, (uint8)green, (uint8)blue, (uint8)color_index);
}

void Color::Set(uint8 red, uint8 green, uint8 blue, uint8 color_index) {
	red_	= red;
	green_	= green;
	blue_	= blue;
	alpha_	= color_index;
}

void Color::Set(int16 red, int16 green, int16 blue, int16 color_index) {
	Set((uint8)red, (uint8)green, (uint8)blue, (uint8)color_index);
}

void Color::Set(uint16 red, uint16 green, uint16 blue, uint16 color_index) {
	Set((uint8)red, (uint8)green, (uint8)blue, (uint8)color_index);
}

void Color::Set(int32 red, int32 green, int32 blue, int32 color_index) {
	Set((uint8)red, (uint8)green, (uint8)blue, (uint8)color_index);
}

void Color::Set(uint32 red, uint32 green, uint32 blue, uint32 color_index) {
	Set((uint8)red, (uint8)green, (uint8)blue, (uint8)color_index);
}

void Color::Set(int64 red, int64 green, int64 blue, int64 color_index) {
	Set((uint8)red, (uint8)green, (uint8)blue, (uint8)color_index);
}

void Color::Set(uint64 red, uint64 green, uint64 blue, uint64 color_index) {
	Set((uint8)red, (uint8)green, (uint8)blue, (uint8)color_index);
}

void Color::Set(float red, float green, float blue, float color_index) {
	Set((uint8)(red * 255.0f),
		(uint8)(green * 255.0f),
		(uint8)(blue * 255.0f),
		(uint8)(color_index * 255.0f)); // Color index treated as alpha channel.
}

float Color::GetRf() const {
	return (red_/255.0f);
}

float Color::GetGf() const {
	return (green_/255.0f);
}

float Color::GetBf() const {
	return (blue_/255.0f);
}

float Color::GetAf() const {
	return (alpha_/255.0f);
}

uint32 Color::To32() const {
	typedef uint32 u;
	return ((((u)red_)<<24) + (((u)green_)<<16) + (((u)blue_)<<8) + (u)alpha_);
}

Color Color::operator = (const Color& color) {
	red_   = color.red_;
	green_ = color.green_;
	blue_  = color.blue_;
	alpha_ = color.alpha_;
	return *this;
}

Color Color::operator += (const Color& color) {
	red_   = (uint8)Min(255, (int)red_   + (int)color.red_);
	green_ = (uint8)Min(255, (int)green_ + (int)color.green_);
	blue_  = (uint8)Min(255, (int)blue_  + (int)color.blue_);
	alpha_ = (uint8)Min(255, (int)alpha_ + (int)color.alpha_);
	return *this;
}

Color Color::operator + (const Color& color) const {
	return Color((uint8)Min(255, (int)red_   + (int)color.red_),
		         (uint8)Min(255, (int)green_ + (int)color.green_),
				 (uint8)Min(255, (int)blue_  + (int)color.blue_),
				 (uint8)Min(255, (int)alpha_ + (int)color.alpha_));
}

Color Color::operator -= (const Color& color) {
	red_   = (uint8)Max(0, (int)red_   - (int)color.red_);
	green_ = (uint8)Max(0, (int)green_ - (int)color.green_);
	blue_  = (uint8)Max(0, (int)blue_  - (int)color.blue_);
	alpha_ = (uint8)Max(0, (int)alpha_ - (int)color.alpha_);
	return *this;
}

Color Color::operator - (const Color& color) const {
	return Color((uint8)Max(0, (int)red_   - (int)color.red_),
		         (uint8)Max(0, (int)green_ - (int)color.green_),
				 (uint8)Max(0, (int)blue_  - (int)color.blue_),
				 (uint8)Max(0, (int)alpha_ - (int)color.alpha_));
}

Color Color::operator += (int value) {
	red_   = (uint8)Max(0, Min(255, (int)red_   + value));
	green_ = (uint8)Max(0, Min(255, (int)green_ + value));
	blue_  = (uint8)Max(0, Min(255, (int)blue_  + value));
	alpha_ = (uint8)Max(0, Min(255, (int)alpha_ + value));
	return *this;
}

Color Color::operator + (int value) const {
	return Color((uint8)Max(0, Min(255, (int)red_   + value)),
		         (uint8)Max(0, Min(255, (int)green_ + value)),
				 (uint8)Max(0, Min(255, (int)blue_  + value)),
				 alpha_);
}

Color Color::operator -= (int value) {
	red_   = (uint8)Min(255, Max(0, (int)red_   - value));
	green_ = (uint8)Min(255, Max(0, (int)green_ - value));
	blue_  = (uint8)Min(255, Max(0, (int)blue_  - value));
	return *this;
}

Color Color::operator - (int value) const {
	return Color((uint8)Min(255, Max(0, (int)red_   - value)),
		         (uint8)Min(255, Max(0, (int)green_ - value)),
				 (uint8)Min(255, Max(0, (int)blue_  - value)),
				 alpha_);
}

Color Color::operator *= (int scalar) {
	red_   = (uint8)Max(0, Min(255, (int)red_   * scalar));
	green_ = (uint8)Max(0, Min(255, (int)green_ * scalar));
	blue_  = (uint8)Max(0, Min(255, (int)blue_  * scalar));
	return *this;
}

Color Color::operator * (int scalar) const {
	return Color((uint8)Max(0, Min(255, (int)red_   * scalar)),
		         (uint8)Max(0, Min(255, (int)green_ * scalar)),
				 (uint8)Max(0, Min(255, (int)blue_  * scalar)),
				 alpha_);
}

Color Color::operator /= (int scalar) {
	red_   = (uint8)Max(0, Min(255, (int)red_   / scalar));
	green_ = (uint8)Max(0, Min(255, (int)green_ / scalar));
	blue_  = (uint8)Max(0, Min(255, (int)blue_  / scalar));
	return *this;
}

Color Color::operator / (int scalar) const {
	return Color((uint8)Max(0, Min(255, (int)red_   / scalar)),
		         (uint8)Max(0, Min(255, (int)green_ / scalar)),
				 (uint8)Max(0, Min(255, (int)blue_  / scalar)),
				 alpha_);
}

Color Color::operator *= (float scalar) {
	red_   = (uint8)Max(0, Min(255, (int)((float)red_   * scalar)));
	green_ = (uint8)Max(0, Min(255, (int)((float)green_ * scalar)));
	blue_  = (uint8)Max(0, Min(255, (int)((float)blue_  * scalar)));
	return *this;
}

Color Color::operator * (float scalar) const {
	return Color((uint8)Max(0, Min(255, (int)((float)red_   * scalar))),
		         (uint8)Max(0, Min(255, (int)((float)green_ * scalar))),
				 (uint8)Max(0, Min(255, (int)((float)blue_  * scalar))),
				 alpha_);
}

Color Color::operator /= (float scalar) {
	red_   = (uint8)Max(0, Min(255, (int)((float)red_   / scalar)));
	green_ = (uint8)Max(0, Min(255, (int)((float)green_ / scalar)));
	blue_  = (uint8)Max(0, Min(255, (int)((float)blue_  / scalar)));
	return *this;
}

Color Color::operator / (float scalar) const {
	return Color((uint8)Max(0, Min(255, (int)((float)red_   / scalar))),
		         (uint8)Max(0, Min(255, (int)((float)green_ / scalar))),
				 (uint8)Max(0, Min(255, (int)((float)blue_  / scalar))),
				 alpha_);
}

bool Color::operator==(const Color& other) const {
	return red_   == other.red_   &&
		   green_ == other.green_ &&
		   blue_  == other.blue_  &&
		   alpha_ == other.alpha_;
}

bool Color::operator!=(const Color& other) const {
	return red_   != other.red_   ||
		   green_ != other.green_ ||
		   blue_  != other.blue_  ||
		   alpha_ != other.alpha_;
}



int Color::SumRgb() const {
	return (red_+green_+blue_);
}



}
