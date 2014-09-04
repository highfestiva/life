
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "LepraTypes.h"
#include "Math.h"



namespace Lepra
{



template<class _TVarType>
class Vector2D
{
public:
	typedef _TVarType BaseType;

	_TVarType x;
	_TVarType y;

	inline Vector2D();
	inline Vector2D(_TVarType pX, _TVarType pY);
	inline Vector2D(const Vector2D<_TVarType>& pV);

	inline void Set(_TVarType pX, _TVarType pY);
	inline void Set(const Vector2D<_TVarType>& pV);
	// Set perpendicular...
	inline void SetPerpCW(const Vector2D<_TVarType>& pV);
	inline void SetPerpCCW(const Vector2D<_TVarType>& pV);
	inline void SetPolar(_TVarType pAngle, _TVarType pLength);
	inline void SetBezier(const Vector2D<_TVarType>& pVA, 
						  const Vector2D<_TVarType>& pVTA, 
						  const Vector2D<_TVarType>& pVTB, 
						  const Vector2D<_TVarType>& pVB, 
						  _TVarType pT);

	inline Vector2D<_TVarType> GetPerpCW();
	inline Vector2D<_TVarType> GetPerpCCW();

	inline void Add(_TVarType pX, _TVarType pY);
	inline void Add(const Vector2D<_TVarType>& pV);

	inline void Sub(_TVarType pX, _TVarType pY);
	inline void Sub(const Vector2D<_TVarType>& pV);

	inline void Mul(_TVarType pScalar);
	inline void Div(_TVarType pScalar);

	inline _TVarType Dot(const Vector2D& pV) const;

	inline _TVarType GetLength() const;
	inline _TVarType GetLengthSquared() const;

	inline _TVarType GetDistance(const Vector2D& pV) const;
	inline _TVarType GetDistanceSquared(const Vector2D& pV) const;

	inline _TVarType GetAngle() const;
	inline _TVarType GetAngle(const Vector2D& pV) const;

	void Normalize(_TVarType pLength = 1.0f);
	Vector2D<_TVarType> GetNormalized() const;

	void Cross(const Vector2D<_TVarType>& pV);
	void CrossUnit(const Vector2D<_TVarType>& pV);

	void RotateAround(const Vector2D<_TVarType>& pOrigin, _TVarType pAngle);

	inline Vector2D operator= (const Vector2D& pV);
	inline bool operator== (const Vector2D<_TVarType>& pV) const;
	inline bool operator!= (const Vector2D<_TVarType>& pV) const;
	
	inline Vector2D operator+= (const Vector2D& pV);
	inline Vector2D operator+ (const Vector2D& pV) const;
	inline Vector2D operator-= (const Vector2D& pV);
	inline Vector2D operator- (const Vector2D& pV) const;
	inline Vector2D operator- () const;

	inline Vector2D operator*= (_TVarType pScalar);
	inline Vector2D operator* (_TVarType pScalar) const;
	inline Vector2D operator/= (_TVarType pScalar);
	inline Vector2D operator/ (_TVarType pScalar) const;

	// Dot product
	inline _TVarType operator* (const Vector2D& pV) const;

	// The pData parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(uint8* pData);
	int SetRawData(uint8* pData);

	inline bool IsNullVector() const;

	Vector2D<float> ToFloat() const;
	Vector2D<double> ToDouble() const;

protected:
private:
};


template<class _TVarType> inline Vector2D<_TVarType> operator * (_TVarType pScalar, const Vector2D<_TVarType>& pVec);
template<class _TVarType> inline Vector2D<_TVarType> operator / (_TVarType pScalar, const Vector2D<_TVarType>& pVec);

// PoL: Point on line 
// Dir: Direction of line
// Point: Any point
// ClosestPoint: The point on the line which is closest to Point.
// DirDistance: The distance to move from PoL along Dir to get to Point.
// Returns the shortest distance between the Point and the line.
template<class _TVarType> inline _TVarType LineDistance(const Vector2D<_TVarType>& pPoL, const Vector2D<_TVarType>& pDir, const Vector2D<_TVarType>& pPoint);
template<class _TVarType> inline _TVarType LineDistance(const Vector2D<_TVarType>& pPoL, const Vector2D<_TVarType>& pDir, const Vector2D<_TVarType>& pPoint, Vector2D<_TVarType>& pClosestPoint, _TVarType& pDirDistance);
template<class _TVarType> inline _TVarType LineSegmentDistance(const Vector2D<_TVarType>& pStart, const Vector2D<_TVarType>& pEnd, const Vector2D<_TVarType>& pPoint, Vector2D<_TVarType>& pClosestPoint, _TVarType& pDirDistance);



#include "Vector2D.inl"

typedef Vector2D<float> vec2;
typedef Vector2D<double> Vector2DD;
typedef Vector2D<int> Vector2DI;



}
