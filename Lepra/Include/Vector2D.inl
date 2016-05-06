/*
	Class:  Vector2D
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#define V2D_TEMPLATE template<class _TVarType>
#define V2D_QUAL Vector2D<_TVarType>

V2D_TEMPLATE V2D_QUAL::Vector2D() {
	x = 0;
	y = 0;
}

V2D_TEMPLATE V2D_QUAL::Vector2D(_TVarType _x, _TVarType _y) {
	x = _x;
	y = _y;
}

V2D_TEMPLATE V2D_QUAL::Vector2D(const Vector2D<_TVarType>& v) {
	x = v.x;
	y = v.y;
}

V2D_TEMPLATE void V2D_QUAL::Set(_TVarType _x, _TVarType _y) {
	x = _x;
	y = _y;
}

V2D_TEMPLATE void V2D_QUAL::Set(const Vector2D<_TVarType>& v) {
	x = v.x;
	y = v.y;
}

V2D_TEMPLATE void V2D_QUAL::SetPerpCW(const Vector2D<_TVarType>& v) {
	x = v.y;
	y = -v.x;
}

V2D_TEMPLATE void V2D_QUAL::SetPerpCCW(const Vector2D<_TVarType>& v) {
	x = -v.y;
	y = v.x;
}

V2D_TEMPLATE void V2D_QUAL::SetPolar(_TVarType angle, _TVarType length) {
	x = (_TVarType)cos(angle) * length;
	y = (_TVarType)sin(angle) * length;
}

V2D_TEMPLATE void V2D_QUAL::SetBezier(const Vector2D<_TVarType>& va,
						const Vector2D<_TVarType>& vta,
						const Vector2D<_TVarType>& vtb,
						const Vector2D<_TVarType>& vb,
						_TVarType t) {
	// Calculate lots of temporary variables to minimize number of operations.
	_TVarType t1_05 = (_TVarType)(0.5 * t);

	_TVarType t2_10 = t * t;
	_TVarType t2_05 = (_TVarType)(0.5 * t2_10);
	_TVarType t2_20 = (_TVarType)(2.0 * t2_10);
	_TVarType t2_25 = (_TVarType)(2.5 * t2_10);

	_TVarType t3 = t2_10 * t;
	_TVarType t3_05 = (_TVarType)(0.5 * t3);
	_TVarType t3_15 = (_TVarType)(1.5 * t3);

	_TVarType ta = -t3_05 + t2_10 - t1_05;
	_TVarType tb = t3_15 - t2_25 + 1.0;
	_TVarType tc = -t3_15 + t2_20 + t1_05;
	_TVarType td = t3_05 - t2_05;

	x = (_TVarType)(ta * va.x  +
					tb * vta.x +
					tc * vtb.x +
					td * vb.x);
	y = (_TVarType)(ta * va.y  +
					tb * vta.y +
					tc * vtb.y +
					td * vb.y);
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::GetPerpCW() {
	return Vector2D<_TVarType>(y, -x);
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::GetPerpCCW() {
	return Vector2D<_TVarType>(-y, x);
}

V2D_TEMPLATE void V2D_QUAL::Add(_TVarType _x, _TVarType _y) {
	x += _x;
	y += _y;
}

V2D_TEMPLATE void V2D_QUAL::Add(const Vector2D<_TVarType>& v) {
	x += v.x;
	y += v.y;
}

V2D_TEMPLATE void V2D_QUAL::Sub(_TVarType _x, _TVarType _y) {
	x -= _x;
	y -= _y;
}

V2D_TEMPLATE void V2D_QUAL::Sub(const Vector2D<_TVarType>& v) {
	x -= v.x;
	y -= v.y;
}

V2D_TEMPLATE void V2D_QUAL::Mul(_TVarType scalar) {
	x *= scalar;
	y *= scalar;
}

V2D_TEMPLATE void V2D_QUAL::Div(_TVarType scalar) {
	_TVarType inv_scalar = 1.0f / scalar;
	x *= inv_scalar;
	y *= inv_scalar;
}

V2D_TEMPLATE _TVarType V2D_QUAL::Dot(const Vector2D<_TVarType>& v) const {
	return (x * v.x + y * v.y);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetLength() const {
	return (_TVarType)sqrt(x * x + y * y);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetLengthSquared() const {
	return (x * x + y * y);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetDistance(const Vector2D<_TVarType>& v) const {
	_TVarType dx = v.x - x;
	_TVarType dy = v.y - y;

	return (_TVarType)sqrt(dx * dx + dy * dy);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetDistanceSquared(const Vector2D<_TVarType>& v) const {
	_TVarType dx = v.x - x;
	_TVarType dy = v.y - y;

	return (dx * dx + dy * dy);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetAngle() const {
	return atan2(y, x);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetAngle(const Vector2D<_TVarType>& v) const {
	BaseType a = v.GetAngle() - GetAngle();
	if (a  < -(BaseType)PI) {
		a += 2*(BaseType)PI;
	} else if (a  > +(BaseType)PI) {
		a -= 2*(BaseType)PI;
	}
	return a;
}

V2D_TEMPLATE void V2D_QUAL::Cross(const Vector2D<_TVarType>& v) {
	x = -v.y;
	y = v.x;
}

V2D_TEMPLATE void V2D_QUAL::CrossUnit(const Vector2D<_TVarType>& v) {
	Cross(v);
	Normalize();
}

V2D_TEMPLATE void V2D_QUAL::RotateAround(const Vector2D<_TVarType>& origin, _TVarType angle) {
	_TVarType __x = x - origin.x;
	_TVarType __y = y - origin.y;
	_TVarType sin_a = (_TVarType)sin(angle);
	_TVarType cos_a = (_TVarType)cos(angle);

	x = cos_a * __x - sin_a * __y + origin.x;
	y = sin_a * __x + cos_a * __y + origin.y;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator= (const Vector2D<_TVarType>& v) {
	x = v.x;
	y = v.y;

	return *this;
}

V2D_TEMPLATE bool V2D_QUAL::operator== (const Vector2D<_TVarType>& v) const {
	return (x == v.x && y == v.y);
}

V2D_TEMPLATE bool V2D_QUAL::operator!= (const Vector2D<_TVarType>& v) const {
	return (x != v.x || y != v.y);
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator+= (const Vector2D<_TVarType>& v) {
	x += v.x;
	y += v.y;

	return *this;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator+ (const Vector2D<_TVarType>& v) const {
	Vector2D temp;

	temp.x = x + v.x;
	temp.y = y + v.y;

	return temp;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator-= (const Vector2D<_TVarType>& v) {
	x -= v.x;
	y -= v.y;

	return *this;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator- (const Vector2D<_TVarType>& v) const {
	Vector2D temp;

	temp.x = x - v.x;
	temp.y = y - v.y;

	return temp;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator- () const {
	Vector2D temp;

	temp.x = -x;
	temp.y = -y;

	return temp;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator*= (_TVarType scalar) {
	x *= scalar;
	y *= scalar;

	return *this;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator* (_TVarType scalar) const {
	Vector2D temp;

	temp.x = x * scalar;
	temp.y = y * scalar;

	return temp;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator/= (_TVarType scalar) {
	_TVarType inv_scalar = 1.0f / scalar;

	x *= inv_scalar;
	y *= inv_scalar;

	return *this;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator/ (_TVarType scalar) const {
	_TVarType inv_scalar = 1.0f / scalar;

	Vector2D temp;

	temp.x = x * inv_scalar;
	temp.y = y * inv_scalar;

	return temp;
}

V2D_TEMPLATE _TVarType V2D_QUAL::operator* (const Vector2D<_TVarType>& v) const {
	return(x * v.x + y * v.y);
}

V2D_TEMPLATE void V2D_QUAL::Normalize(_TVarType length) {
	_TVarType _length = GetLength();

	if (_length >= MathTraits<_TVarType>::FullEps()) {
		Div(_length);
		Mul(length);
	} else {
		Set(0.0f, 0.0f);
	}
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::GetNormalized() const {
	_TVarType _length = GetLength();
	if (_length >= MathTraits<_TVarType>::FullEps()) {
		return Vector2D<_TVarType>(x/_length, y/_length);
	}
	return Vector2D<_TVarType>(0, 0);
}

V2D_TEMPLATE int V2D_QUAL::GetRawDataSize() {
	return sizeof(_TVarType) * 2;
}

V2D_TEMPLATE int V2D_QUAL::GetRawData(uint8* data) {
	_TVarType* _data = (_TVarType*)data;
	_data[0] = x;
	_data[1] = y;
	return (sizeof(_data[0])*2);
}

V2D_TEMPLATE int V2D_QUAL::SetRawData(uint8* data) {
	_TVarType* _data = (_TVarType*)data;
	x = _data[0];
	y = _data[1];
	return (sizeof(_data[0])*2);
}

V2D_TEMPLATE bool V2D_QUAL::IsNullVector() const {
	return (fabs(x) <= MathTraits<_TVarType>::FullEps() &&
		fabs(y) <= MathTraits<_TVarType>::FullEps());
}

V2D_TEMPLATE Vector2D<float> V2D_QUAL::ToFloat() const {
	return Vector2D<float>((float)x, (float)y);
}

V2D_TEMPLATE Vector2D<double> V2D_QUAL::ToDouble() const {
	return Vector2D<double>((double)x, (double)y);
}

template<class _TVarType>
inline Vector2D<_TVarType> operator*(_TVarType scalar, const Vector2D<_TVarType>& vec) {
	return vec * scalar;
}

template<class _TVarType>
inline Vector2D<_TVarType> operator / (_TVarType scalar, const Vector2D<_TVarType>& vec) {
	return vec / scalar;
}

inline float LineDistance(const Vector2D<float>& po_l, const Vector2D<float>& dir, const Vector2D<float>& point) {
	Vector2D<float> norm_dir(dir);
	norm_dir.Normalize();
	Vector2D<float> _v(point - po_l);
	return std::abs(norm_dir.GetPerpCW().Dot(_v));
}

inline double LineDistance(const Vector2D<double>& po_l, const Vector2D<double>& dir, const Vector2D<double>& point) {
	Vector2D<double> norm_dir(dir);
	norm_dir.Normalize();
	Vector2D<double> _v(point - po_l);
	return std::abs(norm_dir.GetPerpCW().Dot(_v));
}

inline float LineDistance(const Vector2D<float>& po_l, const Vector2D<float>& dir, const Vector2D<float>& point, Vector2D<float>& closest_point, float& dir_distance) {
	Vector2D<float> norm_dir(dir);
	norm_dir.Normalize();
	Vector2D<float> _v(point - po_l);
	dir_distance = norm_dir.Dot(_v);
	Vector2D<float> d(norm_dir * dir_distance);
	closest_point = po_l + d;
	return std::abs(norm_dir.GetPerpCW().Dot(_v));
}

inline double LineDistance(const Vector2D<double>& po_l, const Vector2D<double>& dir, const Vector2D<double>& point, Vector2D<double>& closest_point, double& dir_distance) {
	Vector2D<double> norm_dir(dir);
	norm_dir.Normalize();
	Vector2D<double> _v(point - po_l);
	dir_distance = norm_dir.Dot(_v);
	Vector2D<double> d(norm_dir * dir_distance);
	closest_point = po_l + d;
	return std::abs(norm_dir.GetPerpCW().Dot(_v));
}

inline float LineSegmentDistance(const Vector2D<float>& start, const Vector2D<float>& end, const Vector2D<float>& point, Vector2D<float>& closest_point, float& dir_distance) {
	Vector2D<float> norm_dir(end - start);
	float max_distance = norm_dir.GetLength();
	norm_dir /= max_distance; // Normalize.

	Vector2D<float> _v(point - start);
	dir_distance = norm_dir.Dot(_v);

	if(dir_distance < 0)
		dir_distance = 0;
	else if(dir_distance > max_distance)
		dir_distance = max_distance;

	closest_point = start + norm_dir * dir_distance;
	return point.GetDistance(closest_point);
}

inline double LineSegmentDistance(const Vector2D<double>& start, const Vector2D<double>& end, const Vector2D<double>& point, Vector2D<double>& closest_point, double& dir_distance) {
	Vector2D<double> norm_dir(end - start);
	double max_distance = norm_dir.GetLength();
	norm_dir /= max_distance; // Normalize.

	Vector2D<double> _v(point - start);
	dir_distance = norm_dir.Dot(_v);

	if(dir_distance < 0)
		dir_distance = 0;
	else if(dir_distance > max_distance)
		dir_distance = max_distance;

	closest_point = start + norm_dir * dir_distance;
	return point.GetDistance(closest_point);
}

#undef V2D_TEMPLATE
#undef V2D_QUAL
