
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"
#include "math.h"



namespace lepra {



template<class _TVarType>
class Vector2D {
public:
	typedef _TVarType BaseType;

	_TVarType x;
	_TVarType y;

	inline Vector2D();
	inline Vector2D(_TVarType _x, _TVarType _y);
	inline Vector2D(const Vector2D<_TVarType>& v);

	inline void Set(_TVarType _x, _TVarType _y);
	inline void Set(const Vector2D<_TVarType>& v);
	// Set perpendicular...
	inline void SetPerpCW(const Vector2D<_TVarType>& v);
	inline void SetPerpCCW(const Vector2D<_TVarType>& v);
	inline void SetPolar(_TVarType angle, _TVarType length);
	inline void SetBezier(const Vector2D<_TVarType>& va,
						  const Vector2D<_TVarType>& vta,
						  const Vector2D<_TVarType>& vtb,
						  const Vector2D<_TVarType>& vb,
						  _TVarType t);

	inline Vector2D<_TVarType> GetPerpCW();
	inline Vector2D<_TVarType> GetPerpCCW();

	inline void Add(_TVarType _x, _TVarType _y);
	inline void Add(const Vector2D<_TVarType>& v);

	inline void Sub(_TVarType _x, _TVarType _y);
	inline void Sub(const Vector2D<_TVarType>& v);

	inline void Mul(_TVarType scalar);
	inline void Div(_TVarType scalar);

	inline _TVarType Dot(const Vector2D& v) const;

	inline _TVarType GetLength() const;
	inline _TVarType GetLengthSquared() const;

	inline _TVarType GetDistance(const Vector2D& v) const;
	inline _TVarType GetDistanceSquared(const Vector2D& v) const;

	inline _TVarType GetAngle() const;
	inline _TVarType GetAngle(const Vector2D& v) const;

	void Normalize(_TVarType length = 1.0f);
	Vector2D<_TVarType> GetNormalized() const;

	void Cross(const Vector2D<_TVarType>& v);
	void CrossUnit(const Vector2D<_TVarType>& v);

	void RotateAround(const Vector2D<_TVarType>& origin, _TVarType angle);

	inline Vector2D operator= (const Vector2D& v);
	inline bool operator== (const Vector2D<_TVarType>& v) const;
	inline bool operator!= (const Vector2D<_TVarType>& v) const;

	inline Vector2D operator+= (const Vector2D& v);
	inline Vector2D operator+ (const Vector2D& v) const;
	inline Vector2D operator-= (const Vector2D& v);
	inline Vector2D operator- (const Vector2D& v) const;
	inline Vector2D operator- () const;

	inline Vector2D operator*= (_TVarType scalar);
	inline Vector2D operator* (_TVarType scalar) const;
	inline Vector2D operator/= (_TVarType scalar);
	inline Vector2D operator/ (_TVarType scalar) const;

	// Dot product
	inline _TVarType operator* (const Vector2D& v) const;

	// The data parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(uint8* data);
	int SetRawData(uint8* data);

	inline bool IsNullVector() const;

	Vector2D<float> ToFloat() const;
	Vector2D<double> ToDouble() const;

protected:
private:
};


template<class _TVarType> inline Vector2D<_TVarType> operator * (_TVarType scalar, const Vector2D<_TVarType>& vec);
template<class _TVarType> inline Vector2D<_TVarType> operator / (_TVarType scalar, const Vector2D<_TVarType>& vec);

// PoL: Point on line
// Dir: Direction of line
// Point: Any point
// ClosestPoint: The point on the line which is closest to Point.
// DirDistance: The distance to move from PoL along Dir to get to Point.
// Returns the shortest distance between the Point and the line.
template<class _TVarType> inline _TVarType LineDistance(const Vector2D<_TVarType>& po_l, const Vector2D<_TVarType>& dir, const Vector2D<_TVarType>& point);
template<class _TVarType> inline _TVarType LineDistance(const Vector2D<_TVarType>& po_l, const Vector2D<_TVarType>& dir, const Vector2D<_TVarType>& point, Vector2D<_TVarType>& closest_point, _TVarType& dir_distance);
template<class _TVarType> inline _TVarType LineSegmentDistance(const Vector2D<_TVarType>& start, const Vector2D<_TVarType>& end, const Vector2D<_TVarType>& point, Vector2D<_TVarType>& closest_point, _TVarType& dir_distance);



#include "vector2d.inl"

typedef Vector2D<float> vec2;
typedef Vector2D<double> Vector2DD;
typedef Vector2D<int> Vector2DI;



}
