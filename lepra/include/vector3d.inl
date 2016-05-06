
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#define V3D_TEMPLATE template<class _TVarType>
#define V3D_QUAL Vector3D<_TVarType>



V3D_TEMPLATE V3D_QUAL::Vector3D():
	x(0),
	y(0),
	z(0) {
}

V3D_TEMPLATE V3D_QUAL::Vector3D(_TVarType _x, _TVarType _y, _TVarType _z):
	x(_x),
	y(_y),
	z(_z) {
}

V3D_TEMPLATE V3D_QUAL::Vector3D(const _TVarType data[4]) {
	data_[0] = data[0];
	data_[1] = data[1];
	data_[2] = data[2];
}

V3D_TEMPLATE V3D_QUAL::Vector3D(const Vector3D<_TVarType>& _v) {
	data_[0] = _v.data_[0];
	data_[1] = _v.data_[1];
	data_[2] = _v.data_[2];
}

// Cross product constructor 1.
V3D_TEMPLATE V3D_QUAL::Vector3D(const Vector3D<_TVarType>& v1, const Vector3D<_TVarType>& v2):
	x(v1.y * v2.z - v1.z * v2.y),
	y(v1.z * v2.x - v1.x * v2.z),
	z(v1.x * v2.y - v1.y * v2.x) {
}

// Cross product constructor 2.
V3D_TEMPLATE V3D_QUAL::Vector3D(const Vector3D<_TVarType>& v1, _TVarType _x, _TVarType _y, _TVarType _z):
	x(v1.y * _z - v1.z * _y),
	y(v1.z * _x - v1.x * _z),
	z(v1.x * _y - v1.y * _x) {
}

// Cross product constructor 3.
V3D_TEMPLATE V3D_QUAL::Vector3D(_TVarType x1, _TVarType y1, _TVarType z1,
	_TVarType x2, _TVarType y2, _TVarType z2):
	x(y1 * z2 - z1 * y2),
	y(z1 * x2 - x1 * z2),
	z(x1 * y2 - y1 * x2) {
}

V3D_TEMPLATE void V3D_QUAL::Normalize(_TVarType length) {
	_TVarType _length = GetLength();
	if (_length >= MathTraits<_TVarType>::FullEps()) {
		Mul(length/_length);
	} else {
		Set(0, 0, 0);
	}
}

V3D_TEMPLATE V3D_QUAL V3D_QUAL::GetNormalized(_TVarType length) const {
	V3D_QUAL _vector(*this);
	_vector.Normalize(length);
	return (_vector);
}

V3D_TEMPLATE void V3D_QUAL::GetOrthogonals(Vector3D<_TVarType>& _y, Vector3D<_TVarType>& _z) const {
	if (fabs(z) > 0.7071) {
		// The length of the vector projected on the y-z plane.
		_TVarType l = y * y + z * z;
		_TVarType scale = (_TVarType)(1 / sqrt(l));
		_y.x = 0;
		_y.y = -z * scale;
		_y.z =  y * scale;

		// Set Z = X x Y.
		_z.x =  l * scale;	// = sqrt(l)
		_z.y = -x * _y.z;
		_z.z =  x * _y.y;
	} else {
		// The length of the vector projected on the x-y plane.
		_TVarType l = x * x + y * y;
		_TVarType scale = (_TVarType)(1 / sqrt(l));
		_y.x = -y * scale;
		_y.y =  x * scale;
		_y.z =  0;

		// Set Z = X x Y.
		_z.x = -z * _y.y;
		_z.y =  z * _y.x;
		_z.z = l * scale;	// = sqrt(l)
	}
}

V3D_TEMPLATE void V3D_QUAL::Cross(const Vector3D<_TVarType>& v1, const Vector3D<_TVarType>& v2) {
	x = v1.y * v2.z - v1.z * v2.y;
	y = v1.z * v2.x - v1.x * v2.z;
	z = v1.x * v2.y - v1.y * v2.x;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::Cross(const Vector3D<_TVarType>& _v) const {
	return Vector3D<_TVarType>(y * _v.z - z * _v.y,
		z * _v.x - x * _v.z,
		x * _v.y - y * _v.x);
}

V3D_TEMPLATE void V3D_QUAL::CrossUnit(const Vector3D<_TVarType>& v1, const Vector3D<_TVarType>& v2) {
	Cross(v1, v2);
	Normalize();
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::CrossUnit(const Vector3D<_TVarType>& _v) const {
	Vector3D<_TVarType> temp(*this, _v);	// Cross product constructor.
	temp.Normalize();
	return temp;
}

V3D_TEMPLATE void V3D_QUAL::GetSphericalAngles(_TVarType& theta, _TVarType& phi) const {
	const Vector3D<_TVarType> z_axis(0, 0, 1);
	theta = ::acos((*this)*z_axis/GetLength());

	Vector3D<_TVarType> xy = ProjectOntoPlane(z_axis);
	_TVarType _length = xy.GetLength();
	if (_length <= MathTraits<_TVarType>::FullEps()) {
		phi = 0;
	} else if (xy.y >= 0) {
		phi = ::acos(xy*Vector3D<_TVarType>(1, 0, 0)/_length);
	} else {
		phi = 2*(_TVarType)PI-::acos(xy*Vector3D<_TVarType>(1, 0, 0)/_length);
	}
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::ProjectOntoPlane(const Vector3D<_TVarType>& plane_normal) const {
	return ((*this)-((*this)*plane_normal)*plane_normal);
}

// Cross product
V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator/= (const Vector3D<_TVarType>& _v) {
	_TVarType cx, cy, cz;

	cx = y * _v.z - z * _v.y;
	cy = z * _v.x - x * _v.z;
	cz = x * _v.y - y * _v.x;

	x = cx;
	y = cy;
	z = cz;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator/ (const Vector3D<_TVarType>& _v) {
	Vector3D temp;

	temp.x = y * _v.z - z * _v.y;
	temp.y = z * _v.x - x * _v.z;
	temp.z = x * _v.y - y * _v.x;

	return temp;
}

V3D_TEMPLATE void V3D_QUAL::Set(_TVarType _x, _TVarType _y, _TVarType _z) {
	x = _x;
	y = _y;
	z = _z;
}

V3D_TEMPLATE void V3D_QUAL::Set(const _TVarType data[4]) {
	data_[0] = data[0];
	data_[1] = data[1];
	data_[2] = data[2];
}

V3D_TEMPLATE void V3D_QUAL::Set(const Vector3D<_TVarType>& _v) {
	x = _v.x;
	y = _v.y;
	z = _v.z;
}

V3D_TEMPLATE void V3D_QUAL::SetPolar(_TVarType angle_z, _TVarType angle_y, _TVarType length) {
	_TVarType radius = (_TVarType)cos(angle_z) * length;

	x = (_TVarType)cos(angle_y) * radius;
	y = (_TVarType)sin(angle_z) * length;
	z = (_TVarType)sin(angle_y) * radius;
}

V3D_TEMPLATE void V3D_QUAL::SetBezier(const Vector3D<_TVarType>& va,
							  const Vector3D<_TVarType>& vta,
							  const Vector3D<_TVarType>& vtb,
							  const Vector3D<_TVarType>& vb,
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
	_TVarType tb =  t3_15 - t2_25 + 1.0;
	_TVarType __tc = -t3_15 + t2_20 + t1_05;
	_TVarType td =  t3_05 - t2_05;

	x = (_TVarType)(ta * va.x  +
			tb * vta.x +
			__tc * vtb.x +
			td * vb.x);
	y = (_TVarType)(ta * va.y  +
			tb * vta.y +
			__tc * vtb.y +
			td * vb.y);
	z = (_TVarType)(ta * va.z  +
			tb * vta.z +
			__tc * vtb.z +
			td * vb.z);
}

V3D_TEMPLATE void V3D_QUAL::Add(_TVarType _x, _TVarType _y, _TVarType _z) {
	x += _x;
	y += _y;
	z += _z;
}

V3D_TEMPLATE void V3D_QUAL::Add(const _TVarType data[4]) {
	data_[0] += data[0];
	data_[1] += data[1];
	data_[2] += data[2];
}

V3D_TEMPLATE void V3D_QUAL::Add(const Vector3D<_TVarType>& _v) {
	Add(_v.data_);
}

V3D_TEMPLATE void V3D_QUAL::Sub(_TVarType _x, _TVarType _y, _TVarType _z) {
	x -= _x;
	y -= _y;
	z -= _z;
}

V3D_TEMPLATE void V3D_QUAL::Sub(const _TVarType data[4]) {
	data_[0] -= data[0];
	data_[1] -= data[1];
	data_[2] -= data[2];
}

V3D_TEMPLATE void V3D_QUAL::Sub(const Vector3D<_TVarType>& _v) {
	Sub(_v.data_);
}

V3D_TEMPLATE void V3D_QUAL::Mul(_TVarType scalar) {
	data_[0] *= scalar;
	data_[1] *= scalar;
	data_[2] *= scalar;
}

V3D_TEMPLATE void V3D_QUAL::Div(_TVarType scalar) {
	_TVarType inv_scalar = 1 / scalar;
	data_[0] *= inv_scalar;
	data_[1] *= inv_scalar;
	data_[2] *= inv_scalar;
}

V3D_TEMPLATE _TVarType V3D_QUAL::Dot(const Vector3D<_TVarType>& _v) const {
	return Dot(_v.data_);
}

V3D_TEMPLATE _TVarType V3D_QUAL::Dot(_TVarType _x, _TVarType _y, _TVarType _z) const {
	return x*_x + y*_y + z*_z;
}

V3D_TEMPLATE _TVarType V3D_QUAL::Dot(const _TVarType data[4]) const {
	_TVarType dx = data_[0]*data[0];
	_TVarType dy = data_[1]*data[1];
	_TVarType dz = data_[2]*data[2];
	return dx+dy+dz;
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetLength() const {
	return (_TVarType)sqrt(x * x + y * y + z * z);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetLengthSquared() const {
	return (x * x + y * y + z * z);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetDistance(const Vector3D<_TVarType>& _v) const {
	_TVarType __dx = _v.x - x;
	_TVarType __dy = _v.y - y;
	_TVarType __dz = _v.z - z;

	return sqrt(__dx * __dx + __dy * __dy + __dz * __dz);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetDistanceSquared(const Vector3D<_TVarType>& _v) const {
	_TVarType __dx = _v.x - x;
	_TVarType __dy = _v.y - y;
	_TVarType __dz = _v.z - z;

	return (__dx * __dx + __dy * __dy + __dz * __dz);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetAngle(const Vector3D<_TVarType>& _v) const {
	return Math::Acos(Dot(_v) / GetLength() / _v.GetLength());
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetPolarCoordAngleZ() const {
	_TVarType __x = (_TVarType)sqrt(x*x + z*z);
	return ::atan2(y, (x>=0)?__x:-__x);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetPolarCoordAngleY() const {
	return ::atan2(z, x);
}

V3D_TEMPLATE const Vector3D<_TVarType>& V3D_QUAL::operator= (const Vector3D<_TVarType>& _v) {
	data_[0] = _v.data_[0];
	data_[1] = _v.data_[1];
	data_[2] = _v.data_[2];
	return *this;
}

V3D_TEMPLATE bool V3D_QUAL::operator== (const Vector3D<_TVarType>& _v) const {
	return (x == _v.x && y == _v.y && z == _v.z);
}

V3D_TEMPLATE bool V3D_QUAL::operator!= (const Vector3D<_TVarType>& _v) const {
	return (x != _v.x || y != _v.y || z != _v.z);
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator+= (const Vector3D<_TVarType>& _v) {
	x += _v.x;
	y += _v.y;
	z += _v.z;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator+ (const Vector3D<_TVarType>& _v) const {
	Vector3D temp;

	temp.x = x + _v.x;
	temp.y = y + _v.y;
	temp.z = z + _v.z;

	return temp;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator-= (const Vector3D<_TVarType>& _v) {
	x -= _v.x;
	y -= _v.y;
	z -= _v.z;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator- (const Vector3D<_TVarType>& _v) const {
	Vector3D temp;

	temp.x = x - _v.x;
	temp.y = y - _v.y;
	temp.z = z - _v.z;

	return temp;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator- () const {
	Vector3D temp;

	temp.x = -x;
	temp.y = -y;
	temp.z = -z;

	return temp;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator*= (_TVarType scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator* (_TVarType scalar) const {
	Vector3D temp;

	temp.x = x * scalar;
	temp.y = y * scalar;
	temp.z = z * scalar;

	return temp;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator/= (_TVarType scalar) {
	_TVarType inv_scalar = 1 / scalar;

	x *= inv_scalar;
	y *= inv_scalar;
	z *= inv_scalar;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator/ (_TVarType scalar) const {
	_TVarType inv_scalar = 1 / scalar;

	Vector3D temp;

	temp.x = x * inv_scalar;
	temp.y = y * inv_scalar;
	temp.z = z * inv_scalar;

	return temp;
}

#if !defined(LEPRA_MSVC)

V3D_TEMPLATE inline Vector3D<_TVarType> operator * (_TVarType scalar, const Vector3D<_TVarType>& vector) {
	return vector * scalar;
}

V3D_TEMPLATE inline Vector3D<_TVarType> operator / (_TVarType scalar, const Vector3D<_TVarType>& vector) {
	return vector / scalar;
}

V3D_TEMPLATE inline void LineDistance(const Vector3D<_TVarType>& p0, const Vector3D<_TVarType>& p_dir,
		  const Vector3D<_TVarType>& q0, const Vector3D<_TVarType>& q_dir,
		  Vector3D<_TVarType>& p, Vector3D<_TVarType>& q) {
	Vector3D<_TVarType> u(p_dir);
	Vector3D<_TVarType> v(q_dir);

	u.Normalize();
	v.Normalize();

	Vector3D<_TVarType> w0(p0 - q0);
	_TVarType b = u.Dot(v);
	_TVarType d = u.Dot(w0);
	_TVarType e = v.Dot(w0);

	_TVarType sc = (b*e - d) / (1.0f - b*b);
	_TVarType tc = (e - b*d) / (1.0f - b*b);

	p = (p0 + u * sc);
	q = (q0 + v * tc);
}

V3D_TEMPLATE inline _TVarType LineDistance(const Vector3D<_TVarType>& po_l, const Vector3D<_TVarType>& dir, const Vector3D<_TVarType>& point) {
	Vector3D<_TVarType> norm_dir(dir);
	norm_dir.Normalize();
	Vector3D<_TVarType> __v(point - po_l);
	return (__v - norm_dir * norm_dir.Dot(__v)).GetLength();
}

V3D_TEMPLATE inline _TVarType LineDistance(const Vector3D<_TVarType>& po_l, const Vector3D<_TVarType>& dir, const Vector3D<_TVarType>& point, Vector3D<_TVarType>& closest_point, _TVarType& dir_distance) {
	Vector3D<_TVarType> norm_dir(dir);
	norm_dir.Normalize();
	Vector3D<_TVarType> __v(point - po_l);
	dir_distance = norm_dir.Dot(__v);
	Vector3D<_TVarType> __d(norm_dir * dir_distance);
	closest_point = po_l + __d;
	return (__v - __d).GetLength();
}

#else // LEPRA_MSVC

// Visual Studio .NET can't handle function templates... (.NET 2003 can? .NET 2005 can!).
// Need to implement the functions separately like this.

inline Vector3D<float32> operator * (float32 scalar, const Vector3D<float32>& vector) {
	return vector * scalar;
}

inline Vector3D<float32> operator / (float32 scalar, const Vector3D<float32>& vector) {
	return vector / scalar;
}

inline Vector3D<float64> operator * (float64 scalar, const Vector3D<float64>& vector) {
	return vector * scalar;
}

inline Vector3D<float64> operator / (float64 scalar, const Vector3D<float64>& vector) {
	return vector / scalar;
}

inline void LineDistance(const Vector3D<float>& p0, const Vector3D<float>& p_dir,
		  const Vector3D<float>& q0, const Vector3D<float>& q_dir,
		  Vector3D<float>& p, Vector3D<float>& q) {
	Vector3D<float> u(p_dir);
	Vector3D<float> v(q_dir);

	u.Normalize();
	v.Normalize();

	Vector3D<float> w0(p0 - q0);
	float b = u.Dot(v);
	float d = u.Dot(w0);
	float e = v.Dot(w0);

	float sc = (b*e - d) / (1.0f - b*b);
	float tc = (e - b*d) / (1.0f - b*b);

	p = (p0 + u * sc);
	q = (q0 + v * tc);
}

inline void LineDistance(const Vector3D<double>& p0, const Vector3D<double>& p_dir,
		  const Vector3D<double>& q0, const Vector3D<double>& q_dir,
		  Vector3D<double>& p, Vector3D<double>& q) {
	Vector3D<double> u(p_dir);
	Vector3D<double> v(q_dir);

	u.Normalize();
	v.Normalize();

	Vector3D<double> w0(p0 - q0);
	double b = u.Dot(v);
	double d = u.Dot(w0);
	double e = v.Dot(w0);

	double sc = (b*e - d) / (1.0f - b*b);
	double tc = (e - b*d) / (1.0f - b*b);

	p = (p0 + u * sc);
	q = (q0 + v * tc);
}

inline float LineDistance(const Vector3D<float>& po_l, const Vector3D<float>& dir, const Vector3D<float>& point) {
	Vector3D<float> norm_dir(dir);
	norm_dir.Normalize();
	Vector3D<float> __v(point - po_l);
	return (__v - norm_dir * norm_dir.Dot(__v)).GetLength();
}

inline double LineDistance(const Vector3D<double>& po_l, const Vector3D<double>& dir, const Vector3D<double>& point) {
	Vector3D<double> norm_dir(dir);
	norm_dir.Normalize();
	Vector3D<double> __v(point - po_l);
	return (__v - norm_dir * norm_dir.Dot(__v)).GetLength();
}

inline float LineDistance(const Vector3D<float>& po_l, const Vector3D<float>& dir, const Vector3D<float>& point, Vector3D<float>& closest_point, float dir_distance) {
	Vector3D<float> norm_dir(dir);
	norm_dir.Normalize();
	Vector3D<float> __v(point - po_l);
	dir_distance = norm_dir.Dot(__v);
	Vector3D<float> __d(norm_dir * dir_distance);
	closest_point = po_l + __d;
	return (__v - __d).GetLength();
}

inline double LineDistance(const Vector3D<double>& po_l, const Vector3D<double>& dir, const Vector3D<double>& point, Vector3D<double>& closest_point, double dir_distance) {
	Vector3D<double> norm_dir(dir);
	norm_dir.Normalize();
	Vector3D<double> __v(point - po_l);
	dir_distance = norm_dir.Dot(__v);
	Vector3D<double> __d(norm_dir * dir_distance);
	closest_point = po_l + __d;
	return (__v - __d).GetLength();
}

#endif // !LEPRA_MSVC/LEPRA_MSVC

V3D_TEMPLATE _TVarType V3D_QUAL::operator* (const Vector3D<_TVarType>& _v) const {
	return(x * _v.x + y * _v.y + z * _v.z);
}

V3D_TEMPLATE int V3D_QUAL::GetRawDataSize() {
	return sizeof(_TVarType) * 3;
}

V3D_TEMPLATE int V3D_QUAL::GetRawData(uint8* data) {
	_TVarType* _data = (_TVarType*)data;
	_data[0] = x;
	_data[1] = y;
	_data[2] = z;

	return GetRawDataSize();
}

V3D_TEMPLATE int V3D_QUAL::SetRawData(uint8* data) {
	_TVarType* _data = (_TVarType*)data;
	x = _data[0];
	y = _data[1];
	z = _data[2];

	return GetRawDataSize();
}

V3D_TEMPLATE bool V3D_QUAL::IsNullVector() const {
	return (fabs(x) <= MathTraits<_TVarType>::FullEps() &&
			fabs(y) <= MathTraits<_TVarType>::FullEps() &&
			fabs(z) <= MathTraits<_TVarType>::FullEps());
}

V3D_TEMPLATE Vector3D<float> V3D_QUAL::ToFloat() const {
	return Vector3D<float>((float)x, (float)y, (float)z);
}

V3D_TEMPLATE Vector3D<double> V3D_QUAL::ToDouble() const {
	return Vector3D<double>((double)x, (double)y, (double)z);
}

#undef V3D_TEMPLATE
#undef V3D_QUAL
