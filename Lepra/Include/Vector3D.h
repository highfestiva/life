
// Author: Jonas Byström
// Copyright (c) 2002-, Pixel Doctrine



#pragma once

#include "math.h"
#include "lepratypes.h"

#pragma warning(push)
#pragma warning(disable: 4201)	// Nonstandard extention: unnamed struct.



namespace lepra {



#define LEPRA_XY_ANGLE(wanted, actual)	vec2(wanted.x, wanted.y).GetAngle(vec2(actual.x, actual.y))



template<class _TVarType>
class Vector3D {
public:
	typedef _TVarType BaseType;

	union {
	_TVarType data_[4];
	struct {
	_TVarType x;
	_TVarType y;
	_TVarType z;
	};
	};

	inline Vector3D();
	inline Vector3D(_TVarType _x, _TVarType _y, _TVarType _z);
	inline Vector3D(const _TVarType data[4]);
	inline Vector3D(const Vector3D<_TVarType>& v);

	// The cross product constructors.
	inline Vector3D(const Vector3D<_TVarType>& v1, const Vector3D<_TVarType>& v2);
	inline Vector3D(const Vector3D<_TVarType>& v1, _TVarType _x, _TVarType _y, _TVarType _z);
	inline Vector3D(_TVarType x1, _TVarType y1, _TVarType z1,
					_TVarType x2, _TVarType y2, _TVarType z2);

	inline void Set(_TVarType _x, _TVarType _y, _TVarType _z);
	inline void Set(const _TVarType data[4]);
	inline void Set(const Vector3D<_TVarType>& v);
	inline void SetPolar(_TVarType angle_z, _TVarType angle_y, _TVarType length);
	inline void SetBezier(const Vector3D<_TVarType>& va,
							const Vector3D<_TVarType>& vta,
							const Vector3D<_TVarType>& vtb,
							const Vector3D<_TVarType>& vb,
							_TVarType t);

	// Returns two vectors such that "this", y and z are all perpendicular.
	// If "this" is normalized (has length = 1), y and z will be normalize
	// as well.
	inline void GetOrthogonals(Vector3D<_TVarType>& _y, Vector3D<_TVarType>& _z) const;


	inline void Add(_TVarType _x, _TVarType _y, _TVarType _z);
	inline void Add(const _TVarType data[4]);
	inline void Add(const Vector3D<_TVarType>& v);

	inline void Sub(_TVarType _x, _TVarType _y, _TVarType _z);
	inline void Sub(const _TVarType data[4]);
	inline void Sub(const Vector3D<_TVarType>& v);

	inline void Mul(_TVarType scalar);
	inline void Div(_TVarType scalar);

	inline _TVarType Dot(const Vector3D<_TVarType>& v) const;
	inline _TVarType Dot(_TVarType _x, _TVarType _y, _TVarType _z) const;
	inline _TVarType Dot(const _TVarType data[4]) const;

	inline _TVarType GetLength() const;
	inline _TVarType GetLengthSquared() const;

	inline _TVarType GetDistance(const Vector3D<_TVarType>& v) const;
	inline _TVarType GetDistanceSquared(const Vector3D<_TVarType>& v) const;

	inline _TVarType GetAngle(const Vector3D<_TVarType>& v) const;

	// Z & Y refers to the axis of rotation.
	inline _TVarType GetPolarCoordAngleZ() const;
	inline _TVarType GetPolarCoordAngleY() const;

	void Normalize(_TVarType length = 1);
	Vector3D<_TVarType> GetNormalized(_TVarType length = 1) const;

	void Cross(const Vector3D<_TVarType>& v1, const Vector3D<_TVarType>& v2);
	inline void CrossUnit(const Vector3D<_TVarType>& v1, const Vector3D<_TVarType>& v2);
	Vector3D<_TVarType> Cross(const Vector3D<_TVarType>& v) const;
	inline Vector3D<_TVarType> CrossUnit(const Vector3D<_TVarType>& v) const;

	void GetSphericalAngles(_TVarType& theta, _TVarType& phi) const;
	Vector3D ProjectOntoPlane(const Vector3D<_TVarType>& plane_normal) const;

	inline const Vector3D& operator= (const Vector3D<_TVarType>& v);
	inline bool operator== (const Vector3D<_TVarType>& v) const;
	inline bool operator!= (const Vector3D<_TVarType>& v) const;

	inline Vector3D operator+= (const Vector3D<_TVarType>& v);
	inline Vector3D operator+ (const Vector3D<_TVarType>& v) const;
	inline Vector3D operator-= (const Vector3D<_TVarType>& v);
	inline Vector3D operator- (const Vector3D<_TVarType>& v) const;
	inline Vector3D operator- () const;

	inline Vector3D operator*= (_TVarType scalar);
	inline Vector3D operator* (_TVarType scalar) const;
	inline Vector3D operator/= (_TVarType scalar);
	inline Vector3D operator/ (_TVarType scalar) const;

	// Dot product
	inline _TVarType operator* (const Vector3D<_TVarType>& v) const;

	// Cross product
	Vector3D operator/= (const Vector3D<_TVarType>& v);
	Vector3D operator/ (const Vector3D<_TVarType>& v);

	// The data parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(uint8* data);
	int SetRawData(uint8* data);

	inline bool IsNullVector() const;

	Vector3D<float> ToFloat() const;
	Vector3D<double> ToDouble() const;
};



template<class _TVarType> inline Vector3D<_TVarType> operator * (_TVarType scalar, const Vector3D<_TVarType>& vec);
template<class _TVarType> inline Vector3D<_TVarType> operator / (_TVarType scalar, const Vector3D<_TVarType>& vec);

// Calculates the shortest distance between two lines defined by (P0, PDir) and (Q0, QDir)
// and returns the two points P and Q on these lines where the distance is minimal.
template<class _TVarType> inline void LineDistance(const Vector3D<_TVarType>& p0, const Vector3D<_TVarType>& p_dir,
		  const Vector3D<_TVarType>& q0, const Vector3D<_TVarType>& q_dir,
		  Vector3D<_TVarType>& p, Vector3D<_TVarType>& q);

// PoL: Point on line
// Dir: Direction of line
// Point: Any point
// ClosestPoint: The point on the line which is closest to Point.
// DirDistance: The distance to move from PoL along Dir to get to Point.
// Returns the shortest distance between the Point and the line.
template<class _TVarType> inline _TVarType LineDistance(const Vector3D<_TVarType>& po_l, const Vector3D<_TVarType>& dir, const Vector3D<_TVarType>& point);
template<class _TVarType> inline _TVarType LineDistance(const Vector3D<_TVarType>& po_l, const Vector3D<_TVarType>& dir, const Vector3D<_TVarType>& point, Vector3D<_TVarType>& closest_point, _TVarType& dir_distance);



#include "vector3d.inl"

typedef Vector3D<float> vec3;
typedef Vector3D<double> Vector3DD;



#pragma warning(pop)



}
