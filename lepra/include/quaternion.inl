/*
	Class:  Quaternion
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#define TEMPLATE template<class _TVarType>
#define QUAL Quaternion<_TVarType>


TEMPLATE QUAL::Quaternion() :
	a(1),
	b(0),
	c(0),
	d(0) {
}

TEMPLATE QUAL::Quaternion(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	data_[0] = _a;
	data_[1] = _b;
	data_[2] = _c;
	data_[3] = _d;
}

TEMPLATE QUAL::Quaternion(const _TVarType data[4]) {
	data_[0] = data[0];
	data_[1] = data[1];
	data_[2] = data[2];
	data_[3] = data[3];
}

TEMPLATE QUAL::Quaternion(const Quaternion& quaternion) {
	data_[0] = quaternion.data_[0];
	data_[1] = quaternion.data_[1];
	data_[2] = quaternion.data_[2];
	data_[3] = quaternion.data_[3];
}

TEMPLATE QUAL::Quaternion(const RotationMatrix<_TVarType>& rot_mtx) {
	Set(rot_mtx);
}

TEMPLATE QUAL::Quaternion(_TVarType angle, const Vector3D<_TVarType>& vector) {
	Set(angle, vector);
}

TEMPLATE void QUAL::Set(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	data_[0] = _a;
	data_[1] = _b;
	data_[2] = _c;
	data_[3] = _d;
}

TEMPLATE void QUAL::Set(const _TVarType data[4]) {
	data_[0] = data[0];
	data_[1] = data[1];
	data_[2] = data[2];
	data_[3] = data[3];
}

TEMPLATE void QUAL::Set(const Quaternion& quaternion) {
	data_[0] = quaternion.data_[0];
	data_[1] = quaternion.data_[1];
	data_[2] = quaternion.data_[2];
	data_[3] = quaternion.data_[3];
}

TEMPLATE void QUAL::SetConjugate(const Quaternion& quaternion) {
	a =  quaternion.a;
	b = -quaternion.b;
	c = -quaternion.c;
	d = -quaternion.d;
}

TEMPLATE void QUAL::SetIdentity() {
	a = 1;
	b = 0;
	c = 0;
	d = 0;
}

TEMPLATE void QUAL::Set(_TVarType angle, const Vector3D<_TVarType>& vector) {
	Vector3D<_TVarType> _v(vector);
	_v.Normalize();

	_TVarType sin_a = (_TVarType)sin(angle / (_TVarType)2.0);

	a = (_TVarType)cos(angle / (_TVarType)2.0);
	b = sin_a * _v.x;
	c = sin_a * _v.y;
	d = sin_a * _v.z;

	Normalize();
}

TEMPLATE void QUAL::Set(const RotationMatrix<_TVarType>& rot_mtx) {
	// This is a fool proof way of converting a matrix into a quaternion.
	// No bugs, no glitches, no unexpected behaviour...
	Vector3D<_TVarType> _v;
	_TVarType _angle;
	rot_mtx.GetRotationVector(_v, _angle);
	Set(_angle, _v);
/*
	//
	// The following method is derived from "Visualizing Quaternions" by
	// Andrew J. Hanson, and (naturally) it doesn't quite work. Why should it?
	//

	// Check the diagonal.
	_TVarType lTrace = rot_mtx.GetTrace();

	const _TVarType* matrix = (const _TVarType*)rot_mtx;

	if (lTrace > 0) {
		_TVarType s = (_TVarType)sqrt(lTrace + 1);

		a = s / (_TVarType)2;
		s = (_TVarType)0.5 / s;

		b = (matrix[5] - matrix[7]) * s;
		c = (matrix[6] - matrix[2]) * s;
		d = (matrix[1] - matrix[3]) * s;
	} else { // Diagonal is negative.
		int i, j, k;

		i = 0;

		if (matrix[4] > matrix[0])
			i = 1;

		if (matrix[8] > matrix[i * 3 + i])
			i = 2;

		int next[3] = {1, 2, 0};

		j = next[i];
		k = next[j];

		_TVarType s = (_TVarType)sqrt(matrix[i * 3 + i] - (matrix[j * 3 + j] + matrix[k * 3 + k]) + 1);

		_TVarType _q[4];
		_q[i] = s * (_TVarType)0.5;

		if (s != 0)
			s = (_TVarType)0.5 / s;

		_q[3] = (matrix[j * 3 + k] - matrix[k * 3 + j]) * s;
		_q[j] = (matrix[i * 3 + j] + matrix[j * 3 + i]) * s;
		_q[k] = (matrix[i * 3 + k] - matrix[k * 3 + i]) * s;

		a = _q[0];
		b = _q[1];
		c = _q[2];
		d = _q[3];
	}
*/
}

TEMPLATE RotationMatrix<_TVarType> QUAL::GetAsRotationMatrix() const {
	_TVarType ab, ac, ad;
	_TVarType bb, cc, dd;
	_TVarType bc, bd, cd;
	_TVarType b2, c2, d2;

	// Calculate coefficients.
	b2 = b + b;
	c2 = c + c;
	d2 = d + d;

	bb = b * b2;
	bc = b * c2;
	bd = b * d2;

	cc = c * c2;
	cd = c * d2;
	dd = d * d2;

	ab = a * b2;
	ac = a * c2;
	ad = a * d2;

	// And now create the matrix...
	RotationMatrix<_TVarType> _rot_mtx
	(
		(_TVarType)1.0 - (cc + dd),
		bc - ad,
		bd + ac,

		bc + ad,
		(_TVarType)1.0 - (bb + dd),
		cd - ab,

		bd - ac,
		cd + ab,
		(_TVarType)1.0 - (bb + cc)
	);
/*	RotationMatrix<_TVarType> _rot_mtx
	(
		(_TVarType)1.0 - (cc + dd),
		bc + ad,
		bd - ac,

		bc - ad,
		(_TVarType)1.0 - (bb + dd),
		cd + ab,

		bd + ac,
		cd - ab,
		(_TVarType)1.0 - (bb + cc)
	);
*/
	return _rot_mtx;
}

TEMPLATE void QUAL::GetAsRotationMatrix(RotationMatrix<_TVarType>& rot_mtx) const {
	_TVarType ab, ac, ad;
	_TVarType bb, cc, dd;
	_TVarType bc, bd, cd;
	_TVarType b2, c2, d2;

	// Calculate coefficients.
	b2 = b + b;
	c2 = c + c;
	d2 = d + d;

	bb = b * b2;
	bc = b * c2;
	bd = b * d2;

	cc = c * c2;
	cd = c * d2;
	dd = d * d2;

	ab = a * b2;
	ac = a * c2;
	ad = a * d2;

	// And now create the matrix...
	rot_mtx.Set((_TVarType)1.0 - (cc + dd),
		     bc - ad,
		     bd + ac,

		     bc + ad,
		     (_TVarType)1.0 - (bb + dd),
		     cd - ab,

		     bd - ac,
		     cd + ab,
		     (_TVarType)1.0 - (bb + cc)
		    );
/*	rot_mtx.Set
		(
			(_TVarType)1.0 - (cc + dd),
			bc + ad,
			bd - ac,

			bc - ad,
			(_TVarType)1.0 - (bb + dd),
			cd + ab,

			bd + ac,
			cd - ab,
			(_TVarType)1.0 - (bb + cc)
		);
*/
}

TEMPLATE void QUAL::Add(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	a += _a;
	b += _b;
	c += _c;
	d += _d;
}

TEMPLATE void QUAL::Add(const Quaternion& quaternion) {
	a += quaternion.a;
	b += quaternion.b;
	c += quaternion.c;
	d += quaternion.d;
}

TEMPLATE void QUAL::Sub(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	a -= _a;
	b -= _b;
	c -= _c;
	d -= _d;
}

TEMPLATE void QUAL::Sub(const Quaternion& quaternion) {
	a -= quaternion.a;
	b -= quaternion.b;
	c -= quaternion.c;
	d -= quaternion.d;
}

TEMPLATE void QUAL::Mul(_TVarType _x) {
	a *= _x;
	b *= _x;
	c *= _x;
	d *= _x;
}

TEMPLATE void QUAL::Div(_TVarType _x) {
	a /= _x;
	b /= _x;
	c /= _x;
	d /= _x;
}

TEMPLATE void QUAL::Mul(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a = a * _a - b * _b - c * _c - d * _d;
	_TVarType __b = a * _b + b * _a + c * _d - d * _c;
	_TVarType __c = a * _c + c * _a + d * _b - b * _d;
	_TVarType __d = a * _d + d * _a + b * _c - c * _b;

	a = __a;
	b = __b;
	c = __c;
	d = __d;
}

TEMPLATE void QUAL::Mul(const _TVarType data[4]) {
	_TVarType __a = data_[0] * data[0] - data_[1] * data[1] - data_[2] * data[2] - data_[3] * data[3];
	_TVarType __b = data_[0] * data[1] + data_[1] * data[0] + data_[2] * data[3] - data_[3] * data[2];
	_TVarType __c = data_[0] * data[2] + data_[2] * data[0] + data_[3] * data[1] - data_[1] * data[3];
	_TVarType __d = data_[0] * data[3] + data_[3] * data[0] + data_[1] * data[2] - data_[2] * data[1];

	data_[0] = __a;
	data_[1] = __b;
	data_[2] = __c;
	data_[3] = __d;
}

TEMPLATE void QUAL::Mul(const Quaternion& quaternion) {
	Mul(quaternion.data_);
}

TEMPLATE void QUAL::AMulInvB(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a =  a * _a + b * _b + c * _c + d * _d;
	_TVarType __b = -a * _b + b * _a - c * _d + d * _c;
	_TVarType __c = -a * _c + c * _a - d * _b + b * _d;
	_TVarType __d = -a * _d + d * _a - b * _c + c * _b;

	_TVarType l_recip = (_TVarType)1.0 / (_a * _a + _b * _b + _c * _c + _d * _d);

	a = __a * l_recip;
	b = __b * l_recip;
	c = __c * l_recip;
	d = __d * l_recip;
}

TEMPLATE void QUAL::AMulInvB(const _TVarType data[4]) {
	_TVarType __a =  data_[0] * data[0] + data_[1] * data[1] + data_[2] * data[2] + data_[3] * data[3];
	_TVarType __b = -data_[0] * data[1] + data_[1] * data[0] - data_[2] * data[3] + data_[3] * data[2];
	_TVarType __c = -data_[0] * data[2] + data_[2] * data[0] - data_[3] * data[1] + data_[1] * data[3];
	_TVarType __d = -data_[0] * data[3] + data_[3] * data[0] - data_[1] * data[2] + data_[2] * data[1];

	_TVarType l_recip = (_TVarType)1.0 / (data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3]);

	data_[0] = __a * l_recip;
	data_[1] = __b * l_recip;
	data_[2] = __c * l_recip;
	data_[3] = __d * l_recip;
}

TEMPLATE void QUAL::AMulInvB(const Quaternion& quaternion) {
	AMulInvB(quaternion.data_);
}

TEMPLATE void QUAL::InvAMulB(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a = a * _a + b * _b + c * _c + d * _d;
	_TVarType __b = a * _b - b * _a - c * _d + d * _c;
	_TVarType __c = a * _c - c * _a - d * _b + b * _d;
	_TVarType __d = a * _d - d * _a - b * _c + c * _b;

	_TVarType l_recip = (_TVarType)1.0 / GetNorm();

	a = __a * l_recip;
	b = __b * l_recip;
	c = __c * l_recip;
	d = __d * l_recip;
}

TEMPLATE void QUAL::FastInvAMulB(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a = a * _a + b * _b + c * _c + d * _d;
	_TVarType __b = a * _b - b * _a - c * _d + d * _c;
	_TVarType __c = a * _c - c * _a - d * _b + b * _d;
	_TVarType __d = a * _d - d * _a - b * _c + c * _b;

	a = __a;
	b = __b;
	c = __c;
	d = __d;
}

TEMPLATE void QUAL::InvAMulB(const _TVarType data[4]) {
	_TVarType __a = data_[0] * data[0] + data_[1] * data[1] + data_[2] * data[2] + data_[3] * data[3];
	_TVarType __b = data_[0] * data[1] - data_[1] * data[0] - data_[2] * data[3] + data_[3] * data[2];
	_TVarType __c = data_[0] * data[2] - data_[2] * data[0] - data_[3] * data[1] + data_[1] * data[3];
	_TVarType __d = data_[0] * data[3] - data_[3] * data[0] - data_[1] * data[2] + data_[2] * data[1];

	_TVarType l_recip = (_TVarType)1.0 / GetNorm();

	data_[0] = __a * l_recip;
	data_[1] = __b * l_recip;
	data_[2] = __c * l_recip;
	data_[3] = __d * l_recip;
}

TEMPLATE void QUAL::InvAMulB(const Quaternion& quaternion) {
	InvAMulB(quaternion.data_);
}

TEMPLATE void QUAL::InvAMulInvB(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a =  a * _a - b * _b - c * _c - d * _d;
	_TVarType __b = -a * _b - b * _a + c * _d - d * _c;
	_TVarType __c = -a * _c - c * _a + d * _b - b * _d;
	_TVarType __d = -a * _d - d * _a + b * _c - c * _b;

	_TVarType l_recip = (_TVarType)1.0 /
		(
			(a * a + b * b + c * c + d * d) *
			(_a * _a + _b * _b + _c * _c + _d * _d)
		);

	a = __a * l_recip;
	b = __b * l_recip;
	c = __c * l_recip;
	d = __d * l_recip;
}

TEMPLATE void QUAL::InvAMulInvB(const _TVarType data[4]) {
	_TVarType __a =  data_[0] * data[0] - data_[1] * data[1] - data_[2] * data[2] - data_[3] * data[3];
	_TVarType __b = -data_[0] * data[1] - data_[1] * data[0] + data_[2] * data[3] - data_[3] * data[2];
	_TVarType __c = -data_[0] * data[2] - data_[2] * data[0] + data_[3] * data[1] - data_[1] * data[3];
	_TVarType __d = -data_[0] * data[3] - data_[3] * data[0] + data_[1] * data[2] - data_[2] * data[1];

	_TVarType l_recip = (_TVarType)1.0 /
		(
			(data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2] + data_[3] * data_[3]) *
			(data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3])
		);

	data_[0] = __a * l_recip;
	data_[1] = __b * l_recip;
	data_[2] = __c * l_recip;
	data_[3] = __d * l_recip;
}

TEMPLATE void QUAL::InvAMulInvB(const Quaternion& quaternion) {
	InvAMulInvB(quaternion.data_);
}


TEMPLATE void QUAL::BMulA(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a = _a * a - _b * b - _c * c - _d * d;
	_TVarType __b = _a * b + _b * a + _c * d - _d * c;
	_TVarType __c = _a * c + _c * a + _d * b - _b * d;
	_TVarType __d = _a * d + _d * a + _b * c - _c * b;

	a = __a;
	b = __b;
	c = __c;
	d = __d;
}

TEMPLATE void QUAL::BMulA(const _TVarType data[4]) {
	_TVarType __a = data[0] * data_[0] - data[1] * data_[1] - data[2] * data_[2] - data[3] * data_[3];
	_TVarType __b = data[0] * data_[1] + data[1] * data_[0] + data[2] * data_[3] - data[3] * data_[2];
	_TVarType __c = data[0] * data_[2] + data[2] * data_[0] + data[3] * data_[1] - data[1] * data_[3];
	_TVarType __d = data[0] * data_[3] + data[3] * data_[0] + data[1] * data_[2] - data[2] * data_[1];

	data_[0] = __a;
	data_[1] = __b;
	data_[2] = __c;
	data_[3] = __d;
}

TEMPLATE void QUAL::BMulA(const Quaternion& quaternion) {
	BMulA(quaternion.data_);
}


TEMPLATE void QUAL::InvBMulA(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a = _a * a + _b * b + _c * c + _d * d;
	_TVarType __b = _a * b - _b * a - _c * d + _d * c;
	_TVarType __c = _a * c - _c * a - _d * b + _b * d;
	_TVarType __d = _a * d - _d * a - _b * c + _c * b;

	_TVarType l_recip = (_TVarType)1.0 / (_a * _a + _b * _b + _c * _c + _d * _d);

	a = __a * l_recip;
	b = __b * l_recip;
	c = __c * l_recip;
	d = __d * l_recip;
}

TEMPLATE void QUAL::InvBMulA(const _TVarType data[4]) {
	_TVarType __a = data[0] * data_[0] + data[1] * data_[1] + data[2] * data_[2] + data[3] * data_[3];
	_TVarType __b = data[0] * data_[1] - data[1] * data_[0] - data[2] * data_[3] + data[3] * data_[2];
	_TVarType __c = data[0] * data_[2] - data[2] * data_[0] - data[3] * data_[1] + data[1] * data_[3];
	_TVarType __d = data[0] * data_[3] - data[3] * data_[0] - data[1] * data_[2] + data[2] * data_[1];

	_TVarType l_recip = (_TVarType)1.0 / (data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3]);

	data_[0] = __a * l_recip;
	data_[1] = __b * l_recip;
	data_[2] = __c * l_recip;
	data_[3] = __d * l_recip;
}

TEMPLATE void QUAL::InvBMulA(const Quaternion& quaternion) {
	InvBMulA(quaternion.data_);
}

TEMPLATE void QUAL::BMulInvA(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a =  _a * a + _b * b + _c * c + _d * d;
	_TVarType __b = -_a * b + _b * a - _c * d + _d * c;
	_TVarType __c = -_a * c + _c * a - _d * b + _b * d;
	_TVarType __d = -_a * d + _d * a - _b * c + _c * b;

	_TVarType l_recip = (_TVarType)1.0 / GetNorm();

	a = __a * l_recip;
	b = __b * l_recip;
	c = __c * l_recip;
	d = __d * l_recip;
}

TEMPLATE void QUAL::BMulInvA(const _TVarType data[4]) {
	_TVarType __a =  data[0] * data_[0] + data[1] * data_[1] + data[2] * data_[2] + data[3] * data_[3];
	_TVarType __b = -data[0] * data_[1] + data[1] * data_[0] - data[2] * data_[3] + data[3] * data_[2];
	_TVarType __c = -data[0] * data_[2] + data[2] * data_[0] - data[3] * data_[1] + data[1] * data_[3];
	_TVarType __d = -data[0] * data_[3] + data[3] * data_[0] - data[1] * data_[2] + data[2] * data_[1];

	_TVarType l_recip = (_TVarType)1.0 / GetNorm();

	data_[0] = __a * l_recip;
	data_[1] = __b * l_recip;
	data_[2] = __c * l_recip;
	data_[3] = __d * l_recip;
}

TEMPLATE void QUAL::BMulInvA(const Quaternion& quaternion) {
	BMulInvA(quaternion.data_);
}

TEMPLATE void QUAL::InvBMulInvA(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	_TVarType __a =  _a * a - _b * b - _c * c - _d * d;
	_TVarType __b = -_a * b - _b * a + _c * d - _d * c;
	_TVarType __c = -_a * c - _c * a + _d * b - _b * d;
	_TVarType __d = -_a * d - _d * a + _b * c - _c * b;

	_TVarType l_recip = (_TVarType)1.0 /
		(
			(a * a + b * b + c * c + d * d) *
			(_a * _a + _b * _b + _c * _c + _d * _d)
		);

	a = __a * l_recip;
	b = __b * l_recip;
	c = __c * l_recip;
	d = __d * l_recip;
}

TEMPLATE void QUAL::InvBMulInvA(const _TVarType data[4]) {
	_TVarType __a =  data[0] * data_[0] - data[1] * data_[1] - data[2] * data_[2] - data[3] * data_[3];
	_TVarType __b = -data[0] * data_[1] - data[1] * data_[0] + data[2] * data_[3] - data[3] * data_[2];
	_TVarType __c = -data[0] * data_[2] - data[2] * data_[0] + data[3] * data_[1] - data[1] * data_[3];
	_TVarType __d = -data[0] * data_[3] - data[3] * data_[0] + data[1] * data_[2] - data[2] * data_[1];

	_TVarType l_recip = (_TVarType)1.0 /
		(
			(data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2] + data_[3] * data_[3]) *
			(data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3])
		);

	data_[0] = __a * l_recip;
	data_[1] = __b * l_recip;
	data_[2] = __c * l_recip;
	data_[3] = __d * l_recip;
}

TEMPLATE void QUAL::InvBMulInvA(const Quaternion& quaternion) {
	InvBMulInvA(quaternion.data_);
}



TEMPLATE void QUAL::Div(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d) {
	AMulInvB(_a, _b, _c, _d);
}

TEMPLATE void QUAL::Div(const _TVarType data[4]) {
	// Division is defined as qp', where q = this, and p' is the inverse
	// of the quaternion data.
	// Note that we can redefine division as p'q as well, but with other
	// results.
	AMulInvB(data);

	// Division is defined as q'p, where q' = inverse of this, and p is the
	// quaternion (_a, _b, _c, _d).
	//InvAMulB(_a, _b, _c, _d);
}

TEMPLATE void QUAL::Div(const Quaternion& quaternion) {
	Div(quaternion.data_);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetRotatedVector(const Vector3D<_TVarType>& vector) const {
	Quaternion _q(0, vector.x, vector.y, vector.z);
	_q = (*this) * _q * GetInverse();	// TODO: assume unit, and use conjugate instead of inverse.

	Vector3D<_TVarType> _v(_q.b, _q.c, _q.d);
	return _v;
}

TEMPLATE void QUAL::FastRotatedVector(const Quaternion& inverse, Vector3D<_TVarType>& target, const Vector3D<_TVarType>& source) const {
	Quaternion<_TVarType> _q(*this);
	_q.Mul(0, source.x, source.y, source.z);
	_q.Mul(inverse);
	target.x = _q.b;
	target.y = _q.c;
	target.z = _q.d;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseRotatedVector(const Vector3D<_TVarType>& vector) const {
	Quaternion _q(0, vector.x, vector.y, vector.z);
	_q = GetInverse() * _q * (*this);	// TODO: assume unit, and use conjugate instead of inverse.
	return Vector3D<_TVarType>(_q.b, _q.c, _q.d);
}

TEMPLATE void QUAL::FastInverseRotatedVector(const Quaternion& inverse, Vector3D<_TVarType>& target, const Vector3D<_TVarType>& source) const {
	Quaternion<_TVarType> _q(inverse);
	_q.Mul(0, source.x, source.y, source.z);
	_q.Mul(a, b, c, d);
	target.x = _q.b;
	target.y = _q.c;
	target.z = _q.d;
}

TEMPLATE void QUAL::MakeConjugate() {
	b = -b;
	c = -c;
	d = -d;
}

TEMPLATE Quaternion<_TVarType> QUAL::GetConjugate() const {
	Quaternion temp(*this);
	temp.MakeConjugate();
	return temp;
}

TEMPLATE void QUAL::MakeInverse() {
	_TVarType length_squared = GetNorm();
	MakeConjugate();
	Div(length_squared);
}

TEMPLATE Quaternion<_TVarType> QUAL::GetInverse() const {
	Quaternion temp(*this);
	temp.MakeInverse();
	return temp;
}

TEMPLATE void QUAL::MakeSquare() {
	// Return q*q, where q = this.
	_TVarType l2xA = a * 2.0f;
	a  = a * a - (b * b + c * c + d * d);
	b *= l2xA;
	c *= l2xA;
	d *= l2xA;
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::GetSquare() const {
	Quaternion temp(*this);
	temp.Square();
	return temp;
}

TEMPLATE _TVarType QUAL::GetNorm() const {
	return data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2] + data_[3] * data_[3];
}

TEMPLATE _TVarType QUAL::GetMagnitude() const {
	return sqrt(GetNorm());
}

TEMPLATE void QUAL::Negate() {
	a = -a;
	b = -b;
	c = -c;
	d = -d;
}

TEMPLATE void QUAL::Normalize(_TVarType length) {
	_TVarType _length = GetMagnitude();

	if (_length >= MathTraits<_TVarType>::FullEps()) {
		Mul(length/_length);
	} else {
		Set(0, 0, 0, 0);
	}
}

TEMPLATE void QUAL::Slerp(const Quaternion<_TVarType>& from,
			  const Quaternion<_TVarType>& to,
			  _TVarType t) {
	// Calculate cosine (the dot product between two quaternions).
	_TVarType cos_omega = from.a * to.a +
				from.b * to.b +
				from.c * to.c +
				from.d * to.d;

	_TVarType _to[4];

	// Adjust signs if necessary.
	if (cos_omega < 0) {
		cos_omega = -cos_omega;
		_to[0] = -to.a;
		_to[1] = -to.b;
		_to[2] = -to.c;
		_to[3] = -to.d;
	} else {
		_to[0] = to.a;
		_to[1] = to.b;
		_to[2] = to.c;
		_to[3] = to.d;
	}

	_TVarType scale0;
	_TVarType scale1;

	if (cos_omega < ((_TVarType)1.0 - MathTraits<_TVarType>::FullEps())) {
		// Standard case (slerp).
		_TVarType omega = acos(cos_omega);
		_TVarType sin_omega = (_TVarType)1.0 / sin(omega);
		scale0 = sin(((_TVarType)1.0 - t) * omega) * sin_omega;
		scale1 = sin(t * omega) * sin_omega;
	} else {
		// The quaternions are very close to each other, and we
		// can do a linear interpolation instead to avoid division
		// by zero.
		scale0 = (_TVarType)1.0 - t;
		scale1 = t;
	}

	// And finally, perform the actual interpolation.
	a = from.a * scale0 + _to[0] * scale1;
	b = from.b * scale0 + _to[1] * scale1;
	c = from.c * scale0 + _to[2] * scale1;
	d = from.d * scale0 + _to[3] * scale1;
}

TEMPLATE void QUAL::RotateAroundWorldX(_TVarType angle) {
	_TVarType half_angle = angle / (_TVarType)2.0;
	Quaternion _q((_TVarType)cos(half_angle), (_TVarType)sin(half_angle), 0, 0);
	BMulA(_q);
}

TEMPLATE void QUAL::RotateAroundWorldY(_TVarType angle) {
	_TVarType half_angle = angle / (_TVarType)2.0;
	Quaternion _q((_TVarType)cos(half_angle), 0, (_TVarType)sin(half_angle), 0);
	BMulA(_q);
}

TEMPLATE void QUAL::RotateAroundWorldZ(_TVarType angle) {
	_TVarType half_angle = angle / (_TVarType)2.0;
	Quaternion _q((_TVarType)cos(half_angle), 0, 0, (_TVarType)sin(half_angle));
	BMulA(_q);
}

TEMPLATE void QUAL::RotateAroundOwnX(_TVarType angle) {
	_TVarType half_angle = angle / (_TVarType)2.0;
	Quaternion _q((_TVarType)cos(half_angle), (_TVarType)sin(half_angle), 0, 0);
	Mul(_q);
}

TEMPLATE void QUAL::RotateAroundOwnY(_TVarType angle) {
	_TVarType half_angle = angle / (_TVarType)2.0;
	Quaternion _q((_TVarType)cos(half_angle), 0, (_TVarType)sin(half_angle), 0);
	Mul(_q);
}

TEMPLATE void QUAL::RotateAroundOwnZ(_TVarType angle) {
	_TVarType half_angle = angle / (_TVarType)2.0;
	Quaternion _q((_TVarType)cos(half_angle), 0, 0, (_TVarType)sin(half_angle));
	Mul(_q);
}

TEMPLATE void QUAL::RotateAroundVector(const Vector3D<_TVarType>& vector, _TVarType angle) {
	Quaternion _q(angle, vector);
	BMulA(_q);
}

TEMPLATE void QUAL::GetRotationVector(Vector3D<_TVarType>& vector) const {
	vector.Set(b, c, d);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetRotationVector() const {
	return Vector3D<_TVarType>(b, c, d);
}

TEMPLATE _TVarType QUAL::GetRotationAngle() const {
	_TVarType _angle;
	if (a <= -1) {
		_angle = PI;
	} else if (a >= 1) {
		_angle = 0;
	} else {
		_angle = ::acos(a);
	}
	_angle *= (_TVarType)2.0;
	return (_angle);
}

TEMPLATE void QUAL::GetEulerAngles(Vector3D<_TVarType>& angles) const {
	GetEulerAngles(angles.x, angles.y, angles.z);
}

TEMPLATE void QUAL::GetEulerAngles(_TVarType& yaw, _TVarType& pitch, _TVarType& roll) const {
	const _TVarType w2 = a*a;
	const _TVarType x2 = b*b;
	const _TVarType y2 = c*c;
	const _TVarType z2 = d*d;
	const _TVarType unit_length = w2 + x2 + y2 + z2;	// Normalised == 1, otherwise correction divisor.
	const _TVarType abcd = a*b + c*d;
	_TVarType _yaw;
	_TVarType _pitch;
	_TVarType _roll;
	if (abcd > ((_TVarType)0.5-MathTraits<_TVarType>::FullEps())*unit_length) {
		_yaw = 2 * atan2(c, a);
		_pitch = (_TVarType)PI/2;
		_roll = 0;
	} else if (abcd < (-(_TVarType)0.5+MathTraits<_TVarType>::FullEps())*unit_length) {
		_yaw = -2 * ::atan2(c, a);
		_pitch = -(_TVarType)PI/2;
		_roll = 0;
	} else {
		const _TVarType adbc = a*d - b*c;
		const _TVarType acbd = a*c - b*d;
		_yaw = ::atan2(2*adbc, 1 - 2*(z2+x2));
		_pitch = ::asin(2*abcd/unit_length);
		_roll = ::atan2(2*acbd, 1 - 2*(y2+x2));
	}

	yaw = _yaw;
	pitch = _pitch;
	roll = _roll;
}

TEMPLATE void QUAL::SetEulerAngles(const Vector3D<_TVarType>& angles) {
	SetEulerAngles(angles.x, angles.y, angles.z);
}

TEMPLATE void QUAL::SetEulerAngles(_TVarType yaw, _TVarType pitch, _TVarType roll) {
	SetIdentity();
	RotateAroundOwnZ(yaw);
	RotateAroundOwnX(pitch);
	RotateAroundOwnY(roll);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisX() const {
	_TVarType ac, ad;
	_TVarType cc, dd;
	_TVarType bc, bd;
	_TVarType c2, d2;

	// Calculate coefficients.
	c2 = c + c;
	d2 = d + d;

	bc = b * c2;
	bd = b * d2;

	cc = c * c2;
	dd = d * d2;

	ac = a * c2;
	ad = a * d2;

/*	Vector3D<_TVarType> _v
	(
		(_TVarType)1.0 - (cc + dd),
		bc - ad,
		bd + ac
	);
*/
	Vector3D<_TVarType> _v
	(
		(_TVarType)1.0 - (cc + dd),
		bc + ad,
		bd - ac
	);

	return _v;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisY() const {
	_TVarType ab, ad;
	_TVarType bb, dd;
	_TVarType bc, cd;
	_TVarType b2, c2, d2;

	// Calculate coefficients.
	b2 = b + b;
	c2 = c + c;
	d2 = d + d;

	bb = b * b2;
	dd = d * d2;

	bc = b * c2;
	cd = c * d2;

	ab = a * b2;
	ad = a * d2;

/*	Vector3D<_TVarType> _v
	(
		bc + ad,
		(_TVarType)1.0 - (bb + dd),
		cd - ab
	);
*/
	Vector3D<_TVarType> _v
	(
		bc - ad,
		(_TVarType)1.0 - (bb + dd),
		cd + ab
	);

	return _v;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisZ() const {
	_TVarType ab, ac;
	_TVarType bb, cc;
	_TVarType bd, cd;
	_TVarType b2, c2, d2;

	// Calculate coefficients.
	b2 = b + b;
	c2 = c + c;
	d2 = d + d;

	bb = b * b2;
	cc = c * c2;

	bd = b * d2;
	cd = c * d2;

	ab = a * b2;
	ac = a * c2;

/*	Vector3D<_TVarType> _v
	(
		bd - ac,
		cd + ab,
		(_TVarType)1.0 - (bb + cc)
	);
*/
	Vector3D<_TVarType> _v
	(
		bd + ac,
		cd - ab,
		(_TVarType)1.0 - (bb + cc)
	);

	return _v;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisX() const {
	return GetInverse().GetAxisX();
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisY() const {
	return GetInverse().GetAxisY();
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisZ() const {
	return GetInverse().GetAxisZ();
}

TEMPLATE Quaternion<_TVarType> QUAL::operator= (const Quaternion<_TVarType>& q) {
	Set(q);
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator= (const RotationMatrix<_TVarType>& rot_mtx) {
	Set(rot_mtx);
	return *this;
}

TEMPLATE bool QUAL::operator== (const Quaternion<_TVarType>& q) const {
	return a == q.a &&
	       b == q.b &&
	       c == q.c &&
	       d == q.d;
}

TEMPLATE bool QUAL::operator!= (const Quaternion<_TVarType>& q) const {
	return a != q.a ||
	       b != q.b ||
	       c != q.c ||
	       d != q.d;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator+= (const Quaternion<_TVarType>& q) {
	Add(q);
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator+ (const Quaternion<_TVarType>& q) const {
	Quaternion temp(*this);
	temp.Add(q);
	return temp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator-= (const Quaternion<_TVarType>& q) {
	Sub(q);
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator- (const Quaternion<_TVarType>& q) const {
	Quaternion temp(*this);
	temp.Sub(q);
	return temp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator- () const {
	Quaternion temp(*this);
	temp.Negate();
	return temp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator* (const Quaternion<_TVarType>& q) const {
	Quaternion temp(*this);
	temp.Mul(q);
	return temp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator*= (const Quaternion<_TVarType>& q) {
	Mul(q);
	return *this;
}

TEMPLATE Vector3D<_TVarType> QUAL::operator * (const Vector3D<_TVarType>& v) const {
	return GetRotatedVector(v);
}

#if !defined(LEPRA_MSVC)

TEMPLATE Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& vector, const Quaternion<_TVarType>& q) {
	return q.GetInverseRotatedVector(vector);
}

TEMPLATE Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& vector, const Quaternion<_TVarType>& q) {
	vector = q*vector;
	return (vector);
}

#else // LEPRA_MSVC

// Visual Studio .NET can't handle function templates... (.NET 2003 can? .NET 2005 can!).
// Need to implement the functions separately like this.

inline Vector3D<float32> operator * (const Vector3D<float32>& vector, const Quaternion<float32>& q) {
	return q.GetInverseRotatedVector(vector);
}

inline Vector3D<float32>& operator *= (Vector3D<float32>& vector, const Quaternion<float32>& q) {
	vector = q*vector;
	return (vector);
}

inline Vector3D<float64> operator * (const Vector3D<float64>& vector, const Quaternion<float64>& q) {
	return q.GetInverseRotatedVector(vector);
}

inline Vector3D<float64>& operator *= (Vector3D<float64>& vector, const Quaternion<float64>& q) {
	vector = q*vector;
	return (vector);
}

#endif // !LEPRA_MSVC/LEPRA_MSVC


TEMPLATE Quaternion<_TVarType> QUAL::operator* (_TVarType scalar) const {
	Quaternion temp(*this);
	temp *= scalar;
	return temp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator*= (_TVarType scalar) {
	a *= scalar;
	b *= scalar;
	c *= scalar;
	d *= scalar;
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator/ (const Quaternion<_TVarType>& q) const {
	Quaternion temp(*this);
	temp.Div(q);
	return temp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator/= (const Quaternion<_TVarType>& q) {
	Div(q);
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator/ (_TVarType scalar) const {
	Quaternion temp(*this);
	temp /= scalar;
	return temp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator/= (_TVarType scalar) {
	scalar = 1 / scalar;
	a *= scalar;
	b *= scalar;
	c *= scalar;
	d *= scalar;
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator* () const {
	Quaternion temp(*this);
	temp.MakeConjugate();
	return temp;
}

TEMPLATE Quaternion<float> QUAL::ToFloat() const {
	return Quaternion<float>((float)a, (float)b, (float)c, (float)d);
}

TEMPLATE Quaternion<double> QUAL::ToDouble() const {
	return Quaternion<double>((double)a, (double)b, (double)c, (double)d);
}

#undef TEMPLATE
#undef QUAL
