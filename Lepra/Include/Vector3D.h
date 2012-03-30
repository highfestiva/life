
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "Math.h"
#include "LepraTypes.h"



namespace Lepra
{



#define LEPRA_XY_ANGLE(wanted, actual)	Vector2DF(wanted.x, wanted.y).GetAngle(Vector2DF(actual.x, actual.y))



template<class _TVarType>
class Vector3D
{
	public:
		typedef _TVarType BaseType;

		_TVarType x;
		_TVarType y;
		_TVarType z;

		inline Vector3D();
		inline Vector3D(_TVarType pX, _TVarType pY, _TVarType pZ);
		inline Vector3D(const Vector3D<_TVarType>& pV);

		// The cross product constructors.
		inline Vector3D(const Vector3D<_TVarType>& pV1, const Vector3D<_TVarType>& pV2);
		inline Vector3D(const Vector3D<_TVarType>& pV1, _TVarType pX, _TVarType pY, _TVarType pZ);
		inline Vector3D(_TVarType pX1, _TVarType pY1, _TVarType pZ1, 
						_TVarType pX2, _TVarType pY2, _TVarType pZ2);

		inline void Set(_TVarType pX, _TVarType pY, _TVarType pZ);
		inline void Set(const Vector3D<_TVarType>& pV);
		inline void SetPolar(_TVarType pAngleZ, _TVarType pAngleY, _TVarType pLength);
		inline void SetBezier(const Vector3D<_TVarType>& pVA, 
							  const Vector3D<_TVarType>& pVTA, 
							  const Vector3D<_TVarType>& pVTB, 
							  const Vector3D<_TVarType>& pVB, 
							  _TVarType pT);

		// Returns two vectors such that "this", y and z are all perpendicular.
		// If "this" is normalized (has length = 1), y and z will be normalize 
		// as well.
		inline void GetOrthogonals(Vector3D<_TVarType>& pY, Vector3D<_TVarType>& pZ) const;


		inline void Add(_TVarType pX, _TVarType pY, _TVarType pZ);
		inline void Add(const Vector3D<_TVarType>& pV);

		inline void Sub(_TVarType pX, _TVarType pY, _TVarType pZ);
		inline void Sub(const Vector3D<_TVarType>& pV);

		inline void Mul(_TVarType pScalar);
		inline void Div(_TVarType pScalar);

		inline _TVarType Dot(const Vector3D<_TVarType>& pV) const;
		inline _TVarType Dot(_TVarType pX, _TVarType pY, _TVarType pZ) const;

		inline _TVarType GetLength() const;
		inline _TVarType GetLengthSquared() const;

		inline _TVarType GetDistance(const Vector3D<_TVarType>& pV) const;
		inline _TVarType GetDistanceSquared(const Vector3D<_TVarType>& pV) const;

		inline _TVarType GetAngle(const Vector3D<_TVarType>& pV) const;

		// Z & Y refers to the axis of rotation.
		inline _TVarType GetPolarCoordAngleZ() const;
		inline _TVarType GetPolarCoordAngleY() const;

		void Normalize(_TVarType pLength = 1);
		Vector3D<_TVarType> GetNormalized(_TVarType pLength = 1) const;

		void Cross(const Vector3D<_TVarType>& pV1, const Vector3D<_TVarType>& pV2);
		inline void CrossUnit(const Vector3D<_TVarType>& pV1, const Vector3D<_TVarType>& pV2);
		Vector3D<_TVarType> Cross(const Vector3D<_TVarType>& pV) const;
		inline Vector3D<_TVarType> CrossUnit(const Vector3D<_TVarType>& pV) const;

		void GetSphericalAngles(_TVarType& pTheta, _TVarType& pPhi) const;
		Vector3D ProjectOntoPlane(const Vector3D<_TVarType>& pPlaneNormal) const;

		inline const Vector3D& operator= (const Vector3D<_TVarType>& pV);
		inline bool operator== (const Vector3D<_TVarType>& pV) const;
		inline bool operator!= (const Vector3D<_TVarType>& pV) const;
		
		inline Vector3D operator+= (const Vector3D<_TVarType>& pV);
		inline Vector3D operator+ (const Vector3D<_TVarType>& pV) const;
		inline Vector3D operator-= (const Vector3D<_TVarType>& pV);
		inline Vector3D operator- (const Vector3D<_TVarType>& pV) const;
		inline Vector3D operator- () const;

		inline Vector3D operator*= (_TVarType pScalar);
		inline Vector3D operator* (_TVarType pScalar) const;
		inline Vector3D operator/= (_TVarType pScalar);
		inline Vector3D operator/ (_TVarType pScalar) const;

		// Dot product
		inline _TVarType operator* (const Vector3D<_TVarType>& pV) const;

		// Cross product
		Vector3D operator/= (const Vector3D<_TVarType>& pV);
		Vector3D operator/ (const Vector3D<_TVarType>& pV);

		// The pData parameter must containt the number of bytes
		// returned by GetRawDataSize().
		inline int GetRawDataSize();
		int GetRawData(uint8* pData);
		int SetRawData(uint8* pData);

		inline bool IsNullVector() const;

		Vector3D<float> ToFloat() const;
		Vector3D<double> ToDouble() const;
};



template<class _TVarType> inline Vector3D<_TVarType> operator * (_TVarType pScalar, const Vector3D<_TVarType>& pVec);
template<class _TVarType> inline Vector3D<_TVarType> operator / (_TVarType pScalar, const Vector3D<_TVarType>& pVec);

// Calculates the shortest distance between two lines defined by (P0, PDir) and (Q0, QDir)
// and returns the two points P and Q on these lines where the distance is minimal.
template<class _TVarType> inline void LineDistance(const Vector3D<_TVarType>& pP0, const Vector3D<_TVarType>& pPDir,
		  const Vector3D<_TVarType>& pQ0, const Vector3D<_TVarType>& pQDir,
		  Vector3D<_TVarType>& pP, Vector3D<_TVarType>& pQ);

// PoL: Point on line 
// Dir: Direction of line
// Point: Any point
// ClosestPoint: The point on the line which is closest to Point.
// DirDistance: The distance to move from PoL along Dir to get to Point.
// Returns the shortest distance between the Point and the line.
template<class _TVarType> inline _TVarType LineDistance(const Vector3D<_TVarType>& pPoL, const Vector3D<_TVarType>& pDir, const Vector3D<_TVarType>& pPoint);
template<class _TVarType> inline _TVarType LineDistance(const Vector3D<_TVarType>& pPoL, const Vector3D<_TVarType>& pDir, const Vector3D<_TVarType>& pPoint, Vector3D<_TVarType>& pClosestPoint, _TVarType& pDirDistance);



#include "Vector3D.inl"

typedef Vector3D<float> Vector3DF;
typedef Vector3D<double> Vector3DD;



}
