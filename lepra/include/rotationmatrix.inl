/*
	Class:  RotationMatrix
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

TEMPLATE QUAL::RotationMatrix() {
	MakeIdentity();
}

TEMPLATE QUAL::RotationMatrix(_TVarType p11, _TVarType p12, _TVarType p13,
			      _TVarType p21, _TVarType p22, _TVarType p23,
			      _TVarType p31, _TVarType p32, _TVarType p33) {
	matrix_[0] = p11;
	matrix_[1] = p12;
	matrix_[2] = p13;

	matrix_[3] = p21;
	matrix_[4] = p22;
	matrix_[5] = p23;

	matrix_[6] = p31;
	matrix_[7] = p32;
	matrix_[8] = p33;
}

TEMPLATE QUAL::RotationMatrix(const Vector3D<_TVarType>& vector_x,
			      const Vector3D<_TVarType>& vector_y,
			      const Vector3D<_TVarType>& vector_z) {
	matrix_[0] = vector_x.x;
	matrix_[1] = vector_y.x;
	matrix_[2] = vector_z.x;

	matrix_[3] = vector_x.y;
	matrix_[4] = vector_y.y;
	matrix_[5] = vector_z.y;

	matrix_[6] = vector_x.z;
	matrix_[7] = vector_y.z;
	matrix_[8] = vector_z.z;
}

TEMPLATE QUAL::RotationMatrix(const Vector3D<_TVarType>& vector_x,
			      const Vector3D<_TVarType>& vector_y) {
	Vector3D<_TVarType> _vector_x(vector_x);
	_vector_x.Normalize();

	Vector3D<_TVarType> _vector_y(vector_y);
	_vector_y -= _vector_x * _vector_x.Dot(_vector_y);
	_vector_y.Normalize();

	Vector3D<_TVarType> _vector_z(_vector_x, _vector_y);

	matrix_[0] = _vector_x.x;
	matrix_[1] = _vector_y.x;
	matrix_[2] = _vector_z.x;

	matrix_[3] = _vector_x.y;
	matrix_[4] = _vector_y.y;
	matrix_[5] = _vector_z.y;

	matrix_[6] = _vector_x.z;
	matrix_[7] = _vector_y.z;
	matrix_[8] = _vector_z.z;
}

TEMPLATE QUAL::RotationMatrix(const RotationMatrix& matrix, bool make_inverse /* = false */) {
	if (make_inverse == false) {
		matrix_[0] = matrix.matrix_[0];
		matrix_[1] = matrix.matrix_[1];
		matrix_[2] = matrix.matrix_[2];

		matrix_[3] = matrix.matrix_[3];
		matrix_[4] = matrix.matrix_[4];
		matrix_[5] = matrix.matrix_[5];

		matrix_[6] = matrix.matrix_[6];
		matrix_[7] = matrix.matrix_[7];
		matrix_[8] = matrix.matrix_[8];
	} else {
		matrix_[0] = matrix.matrix_[0];
		matrix_[1] = matrix.matrix_[3];
		matrix_[2] = matrix.matrix_[6];

		matrix_[3] = matrix.matrix_[1];
		matrix_[4] = matrix.matrix_[4];
		matrix_[5] = matrix.matrix_[7];

		matrix_[6] = matrix.matrix_[2];
		matrix_[7] = matrix.matrix_[5];
		matrix_[8] = matrix.matrix_[8];
	}
}

TEMPLATE QUAL::~RotationMatrix() {
}

TEMPLATE void QUAL::MakeIdentity() {
	matrix_[0] = 1.0f;
	matrix_[1] = 0.0f;
	matrix_[2] = 0.0f;

	matrix_[3] = 0.0f;
	matrix_[4] = 1.0f;
	matrix_[5] = 0.0f;

	matrix_[6] = 0.0f;
	matrix_[7] = 0.0f;
	matrix_[8] = 1.0f;
}

TEMPLATE void QUAL::MakeInverse() {
	_TVarType temp;

	temp = matrix_[3];
	matrix_[3] = matrix_[1];
	matrix_[1] = temp;

	temp = matrix_[6];
	matrix_[6] = matrix_[2];
	matrix_[2] = temp;

	temp = matrix_[7];
	matrix_[7] = matrix_[5];
	matrix_[5] = temp;
}

TEMPLATE QUAL QUAL::GetInverse() const {
	RotationMatrix rot_mtx(*this);
	rot_mtx.MakeInverse();

	return rot_mtx;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisX() const {
	return Vector3D<_TVarType>(matrix_[0], matrix_[3], matrix_[6]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisY() const {
	return Vector3D<_TVarType>(matrix_[1], matrix_[4], matrix_[7]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisZ() const {
	return Vector3D<_TVarType>(matrix_[2], matrix_[5], matrix_[8]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisByIndex(int index) {
	switch(index) {
	case 0:
		return Vector3D<_TVarType>(matrix_[0], matrix_[3], matrix_[6]);
	case 1:
		return Vector3D<_TVarType>(matrix_[1], matrix_[4], matrix_[7]);
	case 2:
		return Vector3D<_TVarType>(matrix_[2], matrix_[5], matrix_[8]);
	default:
		return Vector3D<_TVarType>(0, 0, 0);
	};
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisX() const {
	return Vector3D<_TVarType>(matrix_[0], matrix_[1], matrix_[2]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisY() const {
	return Vector3D<_TVarType>(matrix_[3], matrix_[4], matrix_[5]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisZ() const {
	return Vector3D<_TVarType>(matrix_[6], matrix_[7], matrix_[8]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisByIndex(int index) {
	switch(index) {
	case 0:
		return Vector3D<_TVarType>(matrix_[0], matrix_[1], matrix_[2]);
	case 1:
		return Vector3D<_TVarType>(matrix_[3], matrix_[4], matrix_[5]);
	case 2:
		return Vector3D<_TVarType>(matrix_[6], matrix_[7], matrix_[8]);
	default:
		return Vector3D<_TVarType>(0, 0, 0);
	};
}

TEMPLATE void QUAL::SetAxisX(const Vector3D<_TVarType>& axis_x) {
	matrix_[0] = axis_x.x;
	matrix_[3] = axis_x.y;
	matrix_[6] = axis_x.z;
}

TEMPLATE void QUAL::SetAxisY(const Vector3D<_TVarType>& axis_y) {
	matrix_[1] = axis_y.x;
	matrix_[4] = axis_y.y;
	matrix_[7] = axis_y.z;
}

TEMPLATE void QUAL::SetAxisZ(const Vector3D<_TVarType>& axis_z) {
	matrix_[2] = axis_z.x;
	matrix_[5] = axis_z.y;
	matrix_[8] = axis_z.z;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetRotatedVector(const Vector3D<_TVarType>& vector) const {
	// Normal rotation requires inverted rotation axes.
	return Vector3D<_TVarType>
	       (
		matrix_[0] * vector.x + matrix_[1] * vector.y + matrix_[2] * vector.z,
		matrix_[3] * vector.x + matrix_[4] * vector.y + matrix_[5] * vector.z,
		matrix_[6] * vector.x + matrix_[7] * vector.y + matrix_[8] * vector.z
	       );
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseRotatedVector(const Vector3D<_TVarType>& vector) const {
	// Inverse rotation requires normal rotation axes.
	return Vector3D<_TVarType>
	       (
		matrix_[0] * vector.x + matrix_[3] * vector.y + matrix_[6] * vector.z,
		matrix_[1] * vector.x + matrix_[4] * vector.y + matrix_[7] * vector.z,
		matrix_[2] * vector.x + matrix_[5] * vector.y + matrix_[8] * vector.z
	       );
}

TEMPLATE _TVarType QUAL::GetElement(int index) const {
	return matrix_[index];
}

TEMPLATE _TVarType QUAL::GetTrace() const {
	return matrix_[0] + matrix_[4] + matrix_[8];
}

TEMPLATE bool QUAL::operator == (const RotationMatrix& matrix) {
	return matrix_[0] == matrix.matrix_[0] &&
	       matrix_[1] == matrix.matrix_[1] &&
	       matrix_[2] == matrix.matrix_[2] &&
	       matrix_[3] == matrix.matrix_[3] &&
	       matrix_[4] == matrix.matrix_[4] &&
	       matrix_[5] == matrix.matrix_[5] &&
	       matrix_[6] == matrix.matrix_[6] &&
	       matrix_[7] == matrix.matrix_[7] &&
	       matrix_[8] == matrix.matrix_[8];
}

TEMPLATE bool QUAL::operator != (const RotationMatrix& matrix) {
	return !(*this == matrix);
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator =  (const RotationMatrix& matrix) {
	Set(matrix);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator +  (const RotationMatrix& matrix) const {
	RotationMatrix temp(this);
	temp.Add(matrix);
	return temp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator += (const RotationMatrix& matrix) {
	Add(matrix);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator -  (const RotationMatrix& matrix) const {
	RotationMatrix temp(this);
	temp.Sub(matrix);
	return temp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator -= (const RotationMatrix& matrix) {
	Sub(matrix);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator *  (const RotationMatrix& matrix) const {
	RotationMatrix temp(*this);
	temp.Mul(matrix);
	return temp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator *= (const RotationMatrix& matrix) {
	Mul(matrix);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator /  (const RotationMatrix& matrix) const {
	RotationMatrix temp(*this);
	temp.Mul(matrix.GetInverse());
	return temp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator /= (const RotationMatrix& matrix) {
	Mul(matrix.GetInverse());
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator ! () {
	RotationMatrix temp(this);
	temp.MakeInverse();
	return temp;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator - () const {
	RotationMatrix temp(this);
	temp.Mul(-1);
	return temp;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator +  (const _TVarType scalar) const {
	RotationMatrix temp(this);
	temp.Add(scalar);
	return temp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator += (const _TVarType scalar) {
	Add(scalar);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator -  (const _TVarType scalar) const {
	RotationMatrix temp(this);
	temp.Sub(scalar);
	return temp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator -= (const _TVarType scalar) {
	Sub(scalar);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator * (const _TVarType scalar) const {
	RotationMatrix temp(this);
	temp.Mul(scalar);
	return temp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator *= (const _TVarType scalar) {
	Mul(scalar);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator / (const _TVarType scalar) const {
	RotationMatrix temp(this);
	temp.Mul(1.0f / scalar);
	return temp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator /= (const _TVarType scalar) {
	Mul(1.0f / scalar);
	return *this;
}

TEMPLATE Vector3D<_TVarType> QUAL::operator * (const Vector3D<_TVarType>& vector) const {
	return GetRotatedVector(vector);
}

TEMPLATE QUAL::operator const _TVarType* () const {
	return matrix_;
}

TEMPLATE QUAL::operator _TVarType* () {
	return matrix_;
}

#if !defined(LEPRA_MSVC)

TEMPLATE inline Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& vector, const RotationMatrix<_TVarType>& mtx) {
	return mtx.GetInverseRotatedVector(vector);
}

TEMPLATE inline Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& vector, const RotationMatrix<_TVarType>& mtx) {
	vector = mtx.GetInverseRotatedVector(vector);
	return vector;
}

#else // LEPRA_MSVC

// Visual Studio .NET can't handle function templates... (.NET 2003 can? .NET 2005 can!).
// Need to implement the functions separately like this.

inline Vector3D<float32> operator * (const Vector3D<float32>& vector, const RotationMatrix<float32>& mtx) {
	return mtx.GetInverseRotatedVector(vector);
}

inline Vector3D<float32>& operator *= (Vector3D<float32>& vector, const RotationMatrix<float32>& mtx) {
	vector = mtx.GetInverseRotatedVector(vector);
	return vector;
}

inline Vector3D<float64> operator * (const Vector3D<float64>& vector, const RotationMatrix<float64>& mtx) {
	return mtx.GetInverseRotatedVector(vector);
}

inline Vector3D<float64>& operator *= (Vector3D<float64>& vector, const RotationMatrix<float64>& mtx) {
	vector = mtx.GetInverseRotatedVector(vector);
	return vector;
}

#endif // !LEPRA_MSVC/LEPRA_MSVC



TEMPLATE void QUAL::RotateAroundWorldX(_TVarType angle) {
	_TVarType cos_a = (_TVarType)cos(angle);
	_TVarType sin_a = (_TVarType)sin(angle);

	_TVarType __y, __z;

	__y = matrix_[3]; // X.y
	__z = matrix_[6]; // X.z
	matrix_[3] = (cos_a * __y - sin_a * __z);
	matrix_[6] = (sin_a * __y + cos_a * __z);

	__y = matrix_[4]; // Y.y
	__z = matrix_[7]; // Y.z
	matrix_[4] = (cos_a * __y - sin_a * __z);
	matrix_[7] = (sin_a * __y + cos_a * __z);

	__y = matrix_[5]; // Z.y
	__z = matrix_[8]; // Z.z
	matrix_[5] = (cos_a * __y - sin_a * __z);
	matrix_[8] = (sin_a * __y + cos_a * __z);
}

TEMPLATE void QUAL::RotateAroundWorldY(_TVarType angle) {
	_TVarType cos_a = (_TVarType)cos(angle);
	_TVarType sin_a = (_TVarType)sin(angle);

	_TVarType __x, __z;

	__x = matrix_[0]; // X.x
	__z = matrix_[6]; // X.z
	matrix_[0] = (sin_a * __z + cos_a * __x);
	matrix_[6] = (cos_a * __z - sin_a * __x);

	__x = matrix_[1]; // Y.x
	__z = matrix_[7]; // Y.z
	matrix_[1] = (sin_a * __z + cos_a * __x);
	matrix_[7] = (cos_a * __z - sin_a * __x);

	__x = matrix_[2]; // Z.x
	__z = matrix_[8]; // Z.z
	matrix_[2] = (sin_a * __z + cos_a * __x);
	matrix_[8] = (cos_a * __z - sin_a * __x);
}

TEMPLATE void QUAL::RotateAroundWorldZ(_TVarType angle) {
	_TVarType cos_a = (_TVarType)cos(angle);
	_TVarType sin_a = (_TVarType)sin(angle);

	_TVarType __x, __y;

	__x = matrix_[0]; // X.x
	__y = matrix_[3]; // X.y
	matrix_[0] = cos_a * __x - sin_a * __y;
	matrix_[3] = sin_a * __x + cos_a * __y;

	__x = matrix_[1]; // Y.x
	__y = matrix_[4]; // Y.y
	matrix_[1] = cos_a * __x - sin_a * __y;
	matrix_[4] = sin_a * __x + cos_a * __y;

	__x = matrix_[2]; // Z.x
	__y = matrix_[5]; // Z.y
	matrix_[2] = cos_a * __x - sin_a * __y;
	matrix_[5] = sin_a * __x + cos_a * __y;
}

TEMPLATE void QUAL::RotateAroundOwnX(_TVarType angle) {
	_TVarType cos_a = (_TVarType)cos(angle);
	_TVarType sin_a = (_TVarType)sin(angle);

	Mul(RotationMatrix(1,  0,       0,
			   0,  cos_a,-sin_a,
			   0,  sin_a, cos_a));
}

TEMPLATE void QUAL::RotateAroundOwnY(_TVarType angle) {
	_TVarType cos_a = (_TVarType)cos(angle);
	_TVarType sin_a = (_TVarType)sin(angle);

	Mul(RotationMatrix( cos_a,  0,  sin_a,
			    0,        1,  0,
			   -sin_a,  0,  cos_a));
}

TEMPLATE void QUAL::RotateAroundOwnZ(_TVarType angle) {
	_TVarType cos_a = (_TVarType)cos(angle);
	_TVarType sin_a = (_TVarType)sin(angle);

	Mul(RotationMatrix(cos_a, -sin_a, 0,
			   sin_a,  cos_a, 0,
			   0,        0,       1));
}

TEMPLATE void QUAL::RotateAroundVector(const Vector3D<_TVarType>& vector, _TVarType angle) {
	if (vector.IsNullVector() == true) {
		return;
	}

	Vector3D<_TVarType> x_axis(vector);
	Vector3D<_TVarType> y_axis;
	Vector3D<_TVarType> z_axis;

	// Generate a y- and a z-axis.

	x_axis.Normalize();

	// Check if vector is parallel to the world y-axis. No matter what, the resulting
	// coordinate system will always have the same orientation.
	_TVarType abs_dot = (_TVarType)fabs(x_axis.Dot(Vector3D<_TVarType>(0.0f, 1.0f, 0.0f)));

	if ((1.0f - abs_dot) > 1e-6f) {
		// It wasn't parallel. Generate the z-axis first.
		z_axis.CrossUnit(x_axis, Vector3D<_TVarType>(0.0f, 1.0f, 0.0f));
		y_axis.CrossUnit(z_axis, x_axis);
	} else {
		// It was parallel. Generate the y-axis first.
		y_axis.CrossUnit(Vector3D<_TVarType>(0.0f, 0.0f, 1.0f), x_axis);
		z_axis.CrossUnit(x_axis, y_axis);
	}

	RotationMatrix _mtx(x_axis, y_axis, z_axis);

	InvBMulA(_mtx);	// this = Mtx' * this

	RotateAroundWorldX(angle);

	BMulA(_mtx);	// this = Mtx * this
}

TEMPLATE void QUAL::GetRotationVector(Vector3D<_TVarType>& vector, _TVarType& angle) const {
	_TVarType cos_a = (matrix_[0] + matrix_[4] + matrix_[8] - (_TVarType)1.0) * (_TVarType)0.5;
	angle = (_TVarType)acos(cos_a);

	vector.x = matrix_[7] - matrix_[5];
	vector.y = matrix_[2] - matrix_[6];
	vector.z = matrix_[3] - matrix_[1];

	if (vector.IsNullVector() == true) {
		// We can get a null vector in two cases:
		// 1. The matrix is the identity matrix, in which case we don't care about the rotation vector.
		// 2. The matrix is rotated exactly 180 degrees compared to the identity matrix,
		//    and we need to find the rotation vector by checking the rotation axes.

		const _TVarType epsilon_one = 1.0f - 1e-6f;

		// AxisX.x
		if (matrix_[0] >= epsilon_one) {
			vector.x = 1.0f;
			vector.y = 0.0f;
			vector.z = 0.0f;
		}
		// AxisY.y
		else if(matrix_[4] >= epsilon_one) {
			vector.x = 0.0f;
			vector.y = 1.0f;
			vector.z = 0.0f;
		}
		// AxisZ.z
		else if(matrix_[8] >= epsilon_one) {
			vector.x = 0.0f;
			vector.y = 0.0f;
			vector.z = 1.0f;
		} else {
			// ???
			vector.x = 1.0f;
			vector.y = 1.0f;
			vector.z = 1.0f;
		}
	}
}

TEMPLATE void QUAL::Reorthogonalize() {

	//
	// Step 1. (Fix the x-axis)
	//

	// Normalize x-axis
	_TVarType inv_length = (_TVarType)(1.0 / sqrt(matrix_[0] * matrix_[0] + matrix_[1] * matrix_[1] + matrix_[2] * matrix_[2]));

	matrix_[0] *= inv_length;
	matrix_[1] *= inv_length;
	matrix_[2] *= inv_length;

	//
	// Step 2. (Fix the y-axis)
	//

	// Calculate the dot product between the x- and the y-axis.
	_TVarType dot = matrix_[0] * matrix_[3] + matrix_[1] * matrix_[4] + matrix_[2] * matrix_[5];

	// Make the y-axis orthogonal to the x-axis.
	matrix_[3] -= dot * matrix_[0];
	matrix_[4] -= dot * matrix_[1];
	matrix_[5] -= dot * matrix_[2];

	// Normalize y-axis
	inv_length = (_TVarType)(1.0 / sqrt(matrix_[3] * matrix_[3] + matrix_[4] * matrix_[4] + matrix_[5] * matrix_[5]));

	matrix_[3] *= inv_length;
	matrix_[4] *= inv_length;
	matrix_[5] *= inv_length;

	//
	// Step 3. (Fix the z-axis)
	//

	// Calculate the dot product between the x- and the z-axis.
	dot = matrix_[0] * matrix_[6] + matrix_[1] * matrix_[7] + matrix_[2] * matrix_[8];

	// Make the z-axis orthogonal to the x-axis.
	matrix_[6] -= dot * matrix_[0];
	matrix_[7] -= dot * matrix_[1];
	matrix_[8] -= dot * matrix_[2];

	// Calculate the dot product between the y- and the z-axis.
	dot = matrix_[3] * matrix_[6] + matrix_[4] * matrix_[7] + matrix_[5] * matrix_[8];

	// Make the z-axis orthogonal to the y-axis.
	matrix_[6] -= dot * matrix_[3];
	matrix_[7] -= dot * matrix_[4];
	matrix_[8] -= dot * matrix_[5];

	// Normalize z-axis
	inv_length = (_TVarType)(1.0 / sqrt(matrix_[6] * matrix_[6] + matrix_[7] * matrix_[7] + matrix_[8] * matrix_[8]));

	matrix_[6] *= inv_length;
	matrix_[7] *= inv_length;
	matrix_[8] *= inv_length;
}

TEMPLATE void QUAL::Get3x3Array(_TVarType* array) const {
	array[0] = matrix_[0];
	array[1] = matrix_[1];
	array[2] = matrix_[2];

	array[3] = matrix_[3];
	array[4] = matrix_[4];
	array[5] = matrix_[5];

	array[6] = matrix_[6];
	array[7] = matrix_[7];
	array[8] = matrix_[8];
}

TEMPLATE void QUAL::GetInverse3x3Array(_TVarType* array) const {
	array[0] = matrix_[0];
	array[1] = matrix_[3];
	array[2] = matrix_[6];

	array[3] = matrix_[1];
	array[4] = matrix_[4];
	array[5] = matrix_[7];

	array[6] = matrix_[2];
	array[7] = matrix_[5];
	array[8] = matrix_[8];
}

TEMPLATE void QUAL::Get4x4Array(_TVarType* array) const {
	array[0] = matrix_[0];
	array[1] = matrix_[1];
	array[2] = matrix_[2];
	array[3]  = 0.0f;

	array[4] = matrix_[3];
	array[5] = matrix_[4];
	array[6] = matrix_[5];
	array[7]  = 0.0f;

	array[8]  = matrix_[6];
	array[9]  = matrix_[7];
	array[10] = matrix_[8];
	array[11] = 0.0f;

	array[12] = 0.0f;
	array[13] = 0.0f;
	array[14] = 0.0f;
	array[15] = 1.0f;
}

TEMPLATE void QUAL::GetInverse4x4Array(_TVarType* array) const {
	array[0]  = matrix_[0];
	array[1]  = matrix_[3];
	array[2]  = matrix_[6];
	array[3]  = 0.0f;

	array[4]  = matrix_[1];
	array[5]  = matrix_[4];
	array[6]  = matrix_[7];
	array[7]  = 0.0f;

	array[8]  = matrix_[2];
	array[9]  = matrix_[5];
	array[10] = matrix_[8];
	array[11] = 0.0f;

	array[12] = 0.0f;
	array[13] = 0.0f;
	array[14] = 0.0f;
	array[15] = 1.0f;
}

TEMPLATE void QUAL::Mul(const RotationMatrix& matrix) {
	_TVarType temp[9];

	temp[0] = matrix_[0] * matrix.matrix_[0] +
		     matrix_[1] * matrix.matrix_[3] +
		     matrix_[2] * matrix.matrix_[6];
	temp[1] = matrix_[0] * matrix.matrix_[1] +
		     matrix_[1] * matrix.matrix_[4] +
		     matrix_[2] * matrix.matrix_[7];
	temp[2] = matrix_[0] * matrix.matrix_[2] +
		     matrix_[1] * matrix.matrix_[5] +
		     matrix_[2] * matrix.matrix_[8];

	temp[3] = matrix_[3] * matrix.matrix_[0] +
		     matrix_[4] * matrix.matrix_[3] +
		     matrix_[5] * matrix.matrix_[6];
	temp[4] = matrix_[3] * matrix.matrix_[1] +
		     matrix_[4] * matrix.matrix_[4] +
		     matrix_[5] * matrix.matrix_[7];
	temp[5] = matrix_[3] * matrix.matrix_[2] +
		     matrix_[4] * matrix.matrix_[5] +
		     matrix_[5] * matrix.matrix_[8];

	temp[6] = matrix_[6] * matrix.matrix_[0] +
		     matrix_[7] * matrix.matrix_[3] +
		     matrix_[8] * matrix.matrix_[6];
	temp[7] = matrix_[6] * matrix.matrix_[1] +
		     matrix_[7] * matrix.matrix_[4] +
		     matrix_[8] * matrix.matrix_[7];
	temp[8] = matrix_[6] * matrix.matrix_[2] +
		     matrix_[7] * matrix.matrix_[5] +
		     matrix_[8] * matrix.matrix_[8];

	matrix_[0] = temp[0];
	matrix_[1] = temp[1];
	matrix_[2] = temp[2];
	matrix_[3] = temp[3];
	matrix_[4] = temp[4];
	matrix_[5] = temp[5];
	matrix_[6] = temp[6];
	matrix_[7] = temp[7];
	matrix_[8] = temp[8];
}

TEMPLATE void QUAL::Mul(const RotationMatrix& matrix1, const RotationMatrix& matrix2) {
	matrix_[0] = matrix1.matrix_[0] * matrix2.matrix_[0] +
			matrix1.matrix_[1] * matrix2.matrix_[3] +
			matrix1.matrix_[2] * matrix2.matrix_[6];
	matrix_[1] = matrix1.matrix_[0] * matrix2.matrix_[1] +
			matrix1.matrix_[1] * matrix2.matrix_[4] +
			matrix1.matrix_[2] * matrix2.matrix_[7];
	matrix_[2] = matrix1.matrix_[0] * matrix2.matrix_[2] +
			matrix1.matrix_[1] * matrix2.matrix_[5] +
			matrix1.matrix_[2] * matrix2.matrix_[8];

	matrix_[3] = matrix1.matrix_[3] * matrix2.matrix_[0] +
			matrix1.matrix_[4] * matrix2.matrix_[3] +
			matrix1.matrix_[5] * matrix2.matrix_[6];
	matrix_[4] = matrix1.matrix_[3] * matrix2.matrix_[1] +
			matrix1.matrix_[4] * matrix2.matrix_[4] +
			matrix1.matrix_[5] * matrix2.matrix_[7];
	matrix_[5] = matrix1.matrix_[3] * matrix2.matrix_[2] +
			matrix1.matrix_[4] * matrix2.matrix_[5] +
			matrix1.matrix_[5] * matrix2.matrix_[8];

	matrix_[6] = matrix1.matrix_[6] * matrix2.matrix_[0] +
			matrix1.matrix_[7] * matrix2.matrix_[3] +
			matrix1.matrix_[8] * matrix2.matrix_[6];
	matrix_[7] = matrix1.matrix_[6] * matrix2.matrix_[1] +
			matrix1.matrix_[7] * matrix2.matrix_[4] +
			matrix1.matrix_[8] * matrix2.matrix_[7];
	matrix_[8] = matrix1.matrix_[6] * matrix2.matrix_[2] +
			matrix1.matrix_[7] * matrix2.matrix_[5] +
			matrix1.matrix_[8] * matrix2.matrix_[8];
}

TEMPLATE void QUAL::Mul(_TVarType scalar) {
	matrix_[0] *= scalar;
	matrix_[1] *= scalar;
	matrix_[2] *= scalar;

	matrix_[3] *= scalar;
	matrix_[4] *= scalar;
	matrix_[5] *= scalar;

	matrix_[6] *= scalar;
	matrix_[7] *= scalar;
	matrix_[8] *= scalar;
}

TEMPLATE void QUAL::InvAMulB(const RotationMatrix& matrix) {
	_TVarType temp[9];

	temp[0] = matrix_[0] * matrix.matrix_[0] +
		     matrix_[3] * matrix.matrix_[3] +
		     matrix_[6] * matrix.matrix_[6];
	temp[1] = matrix_[0] * matrix.matrix_[1] +
		     matrix_[3] * matrix.matrix_[4] +
		     matrix_[6] * matrix.matrix_[7];
	temp[2] = matrix_[0] * matrix.matrix_[2] +
		     matrix_[3] * matrix.matrix_[5] +
		     matrix_[6] * matrix.matrix_[8];

	temp[3] = matrix_[1] * matrix.matrix_[0] +
		     matrix_[4] * matrix.matrix_[3] +
		     matrix_[7] * matrix.matrix_[6];
	temp[4] = matrix_[1] * matrix.matrix_[1] +
		     matrix_[4] * matrix.matrix_[4] +
		     matrix_[7] * matrix.matrix_[7];
	temp[5] = matrix_[1] * matrix.matrix_[2] +
		     matrix_[4] * matrix.matrix_[5] +
		     matrix_[7] * matrix.matrix_[8];

	temp[6] = matrix_[2] * matrix.matrix_[0] +
		     matrix_[5] * matrix.matrix_[3] +
		     matrix_[8] * matrix.matrix_[6];
	temp[7] = matrix_[2] * matrix.matrix_[1] +
		     matrix_[5] * matrix.matrix_[4] +
		     matrix_[8] * matrix.matrix_[7];
	temp[8] = matrix_[2] * matrix.matrix_[2] +
		     matrix_[5] * matrix.matrix_[5] +
		     matrix_[8] * matrix.matrix_[8];

	matrix_[0] = temp[0];
	matrix_[1] = temp[1];
	matrix_[2] = temp[2];
	matrix_[3] = temp[3];
	matrix_[4] = temp[4];
	matrix_[5] = temp[5];
	matrix_[6] = temp[6];
	matrix_[7] = temp[7];
	matrix_[8] = temp[8];
}

TEMPLATE void QUAL::InvAMulInvB(const RotationMatrix& matrix) {
	_TVarType temp[9];

	temp[0] = matrix_[0] * matrix.matrix_[0] +
		     matrix_[3] * matrix.matrix_[1] +
		     matrix_[6] * matrix.matrix_[2];
	temp[1] = matrix_[0] * matrix.matrix_[3] +
		     matrix_[3] * matrix.matrix_[4] +
		     matrix_[6] * matrix.matrix_[5];
	temp[2] = matrix_[0] * matrix.matrix_[6] +
		     matrix_[3] * matrix.matrix_[7] +
		     matrix_[6] * matrix.matrix_[8];

	temp[3] = matrix_[1] * matrix.matrix_[0] +
		     matrix_[4] * matrix.matrix_[1] +
		     matrix_[7] * matrix.matrix_[2];
	temp[4] = matrix_[1] * matrix.matrix_[3] +
		     matrix_[4] * matrix.matrix_[4] +
		     matrix_[7] * matrix.matrix_[5];
	temp[5] = matrix_[1] * matrix.matrix_[6] +
		     matrix_[4] * matrix.matrix_[7] +
		     matrix_[7] * matrix.matrix_[8];

	temp[6] = matrix_[2] * matrix.matrix_[0] +
		     matrix_[5] * matrix.matrix_[1] +
		     matrix_[8] * matrix.matrix_[2];
	temp[7] = matrix_[2] * matrix.matrix_[3] +
		     matrix_[5] * matrix.matrix_[4] +
		     matrix_[8] * matrix.matrix_[5];
	temp[8] = matrix_[2] * matrix.matrix_[6] +
		     matrix_[5] * matrix.matrix_[7] +
		     matrix_[8] * matrix.matrix_[8];

	matrix_[0] = temp[0];
	matrix_[1] = temp[1];
	matrix_[2] = temp[2];
	matrix_[3] = temp[3];
	matrix_[4] = temp[4];
	matrix_[5] = temp[5];
	matrix_[6] = temp[6];
	matrix_[7] = temp[7];
	matrix_[8] = temp[8];
}

TEMPLATE void QUAL::AMulInvB(const RotationMatrix& matrix) {
	_TVarType temp[9];

	temp[0] = matrix_[0] * matrix.matrix_[0] +
		     matrix_[1] * matrix.matrix_[1] +
		     matrix_[2] * matrix.matrix_[2];
	temp[1] = matrix_[0] * matrix.matrix_[3] +
		     matrix_[1] * matrix.matrix_[4] +
		     matrix_[2] * matrix.matrix_[5];
	temp[2] = matrix_[0] * matrix.matrix_[6] +
		     matrix_[1] * matrix.matrix_[7] +
		     matrix_[2] * matrix.matrix_[8];

	temp[3] = matrix_[3] * matrix.matrix_[0] +
		     matrix_[4] * matrix.matrix_[1] +
		     matrix_[5] * matrix.matrix_[2];
	temp[4] = matrix_[3] * matrix.matrix_[3] +
		     matrix_[4] * matrix.matrix_[4] +
		     matrix_[5] * matrix.matrix_[5];
	temp[5] = matrix_[3] * matrix.matrix_[6] +
		     matrix_[4] * matrix.matrix_[7] +
		     matrix_[5] * matrix.matrix_[8];

	temp[6] = matrix_[6] * matrix.matrix_[0] +
		     matrix_[7] * matrix.matrix_[1] +
		     matrix_[8] * matrix.matrix_[2];
	temp[7] = matrix_[6] * matrix.matrix_[3] +
		     matrix_[7] * matrix.matrix_[4] +
		     matrix_[8] * matrix.matrix_[5];
	temp[8] = matrix_[6] * matrix.matrix_[6] +
		     matrix_[7] * matrix.matrix_[7] +
		     matrix_[8] * matrix.matrix_[8];

	matrix_[0] = temp[0];
	matrix_[1] = temp[1];
	matrix_[2] = temp[2];
	matrix_[3] = temp[3];
	matrix_[4] = temp[4];
	matrix_[5] = temp[5];
	matrix_[6] = temp[6];
	matrix_[7] = temp[7];
	matrix_[8] = temp[8];
}



TEMPLATE void QUAL::BMulA(const RotationMatrix& matrix) {
	_TVarType temp[9];

	temp[0] = matrix.matrix_[0] * matrix_[0] +
		     matrix.matrix_[1] * matrix_[3] +
		     matrix.matrix_[2] * matrix_[6];
	temp[1] = matrix.matrix_[0] * matrix_[1] +
		     matrix.matrix_[1] * matrix_[4] +
		     matrix.matrix_[2] * matrix_[7];
	temp[2] = matrix.matrix_[0] * matrix_[2] +
		     matrix.matrix_[1] * matrix_[5] +
		     matrix.matrix_[2] * matrix_[8];

	temp[3] = matrix.matrix_[3] * matrix_[0] +
		     matrix.matrix_[4] * matrix_[3] +
		     matrix.matrix_[5] * matrix_[6];
	temp[4] = matrix.matrix_[3] * matrix_[1] +
		     matrix.matrix_[4] * matrix_[4] +
		     matrix.matrix_[5] * matrix_[7];
	temp[5] = matrix.matrix_[3] * matrix_[2] +
		     matrix.matrix_[4] * matrix_[5] +
		     matrix.matrix_[5] * matrix_[8];

	temp[6] = matrix.matrix_[6] * matrix_[0] +
		     matrix.matrix_[7] * matrix_[3] +
		     matrix.matrix_[8] * matrix_[6];
	temp[7] = matrix.matrix_[6] * matrix_[1] +
		     matrix.matrix_[7] * matrix_[4] +
		     matrix.matrix_[8] * matrix_[7];
	temp[8] = matrix.matrix_[6] * matrix_[2] +
		     matrix.matrix_[7] * matrix_[5] +
		     matrix.matrix_[8] * matrix_[8];

	matrix_[0] = temp[0];
	matrix_[1] = temp[1];
	matrix_[2] = temp[2];
	matrix_[3] = temp[3];
	matrix_[4] = temp[4];
	matrix_[5] = temp[5];
	matrix_[6] = temp[6];
	matrix_[7] = temp[7];
	matrix_[8] = temp[8];
}



TEMPLATE void QUAL::BMulInvA(const RotationMatrix& matrix) {
	_TVarType temp[9];

	temp[0] = matrix.matrix_[0] * matrix_[0] +
		     matrix.matrix_[1] * matrix_[3] +
		     matrix.matrix_[2] * matrix_[6];
	temp[1] = matrix.matrix_[0] * matrix_[1] +
		     matrix.matrix_[1] * matrix_[4] +
		     matrix.matrix_[2] * matrix_[7];
	temp[2] = matrix.matrix_[0] * matrix_[2] +
		     matrix.matrix_[1] * matrix_[5] +
		     matrix.matrix_[2] * matrix_[8];

	temp[3] = matrix.matrix_[3] * matrix_[0] +
		     matrix.matrix_[4] * matrix_[3] +
		     matrix.matrix_[5] * matrix_[6];
	temp[4] = matrix.matrix_[3] * matrix_[1] +
		     matrix.matrix_[4] * matrix_[4] +
		     matrix.matrix_[5] * matrix_[7];
	temp[5] = matrix.matrix_[3] * matrix_[2] +
		     matrix.matrix_[4] * matrix_[5] +
		     matrix.matrix_[5] * matrix_[8];

	temp[6] = matrix.matrix_[6] * matrix_[0] +
		     matrix.matrix_[7] * matrix_[3] +
		     matrix.matrix_[8] * matrix_[6];
	temp[7] = matrix.matrix_[6] * matrix_[1] +
		     matrix.matrix_[7] * matrix_[4] +
		     matrix.matrix_[8] * matrix_[7];
	temp[8] = matrix.matrix_[6] * matrix_[2] +
		     matrix.matrix_[7] * matrix_[5] +
		     matrix.matrix_[8] * matrix_[8];

	matrix_[0] = temp[0];
	matrix_[1] = temp[1];
	matrix_[2] = temp[2];
	matrix_[3] = temp[3];
	matrix_[4] = temp[4];
	matrix_[5] = temp[5];
	matrix_[6] = temp[6];
	matrix_[7] = temp[7];
	matrix_[8] = temp[8];
}



TEMPLATE void QUAL::InvBMulInvA(const RotationMatrix& matrix) {
	_TVarType temp[9];

	temp[0] = matrix.matrix_[0] * matrix_[0] +
		     matrix.matrix_[3] * matrix_[1] +
		     matrix.matrix_[6] * matrix_[2];
	temp[1] = matrix.matrix_[0] * matrix_[3] +
		     matrix.matrix_[3] * matrix_[4] +
		     matrix.matrix_[6] * matrix_[5];
	temp[2] = matrix.matrix_[0] * matrix_[6] +
		     matrix.matrix_[3] * matrix_[7] +
		     matrix.matrix_[6] * matrix_[8];

	temp[3] = matrix.matrix_[1] * matrix_[0] +
		     matrix.matrix_[4] * matrix_[1] +
		     matrix.matrix_[7] * matrix_[2];
	temp[4] = matrix.matrix_[1] * matrix_[3] +
		     matrix.matrix_[4] * matrix_[4] +
		     matrix.matrix_[7] * matrix_[5];
	temp[5] = matrix.matrix_[1] * matrix_[6] +
		     matrix.matrix_[4] * matrix_[7] +
		     matrix.matrix_[7] * matrix_[8];

	temp[6] = matrix.matrix_[2] * matrix_[0] +
		     matrix.matrix_[5] * matrix_[1] +
		     matrix.matrix_[8] * matrix_[2];
	temp[7] = matrix.matrix_[2] * matrix_[3] +
		     matrix.matrix_[5] * matrix_[4] +
		     matrix.matrix_[8] * matrix_[5];
	temp[8] = matrix.matrix_[2] * matrix_[6] +
		     matrix.matrix_[5] * matrix_[7] +
		     matrix.matrix_[8] * matrix_[8];

	matrix_[0] = temp[0];
	matrix_[1] = temp[1];
	matrix_[2] = temp[2];
	matrix_[3] = temp[3];
	matrix_[4] = temp[4];
	matrix_[5] = temp[5];
	matrix_[6] = temp[6];
	matrix_[7] = temp[7];
	matrix_[8] = temp[8];
}



TEMPLATE void QUAL::InvBMulA(const RotationMatrix& matrix) {
	_TVarType temp[9];

	temp[0] = matrix.matrix_[0] * matrix_[0] +
		     matrix.matrix_[3] * matrix_[3] +
		     matrix.matrix_[6] * matrix_[6];
	temp[1] = matrix.matrix_[0] * matrix_[1] +
		     matrix.matrix_[3] * matrix_[4] +
		     matrix.matrix_[6] * matrix_[7];
	temp[2] = matrix.matrix_[0] * matrix_[2] +
		     matrix.matrix_[3] * matrix_[5] +
		     matrix.matrix_[6] * matrix_[8];

	temp[3] = matrix.matrix_[1] * matrix_[0] +
		     matrix.matrix_[4] * matrix_[3] +
		     matrix.matrix_[7] * matrix_[6];
	temp[4] = matrix.matrix_[1] * matrix_[1] +
		     matrix.matrix_[4] * matrix_[4] +
		     matrix.matrix_[7] * matrix_[7];
	temp[5] = matrix.matrix_[1] * matrix_[2] +
		     matrix.matrix_[4] * matrix_[5] +
		     matrix.matrix_[7] * matrix_[8];

	temp[6] = matrix.matrix_[2] * matrix_[0] +
		     matrix.matrix_[5] * matrix_[3] +
		     matrix.matrix_[8] * matrix_[6];
	temp[7] = matrix.matrix_[2] * matrix_[1] +
		     matrix.matrix_[5] * matrix_[4] +
		     matrix.matrix_[8] * matrix_[7];
	temp[8] = matrix.matrix_[2] * matrix_[2] +
		     matrix.matrix_[5] * matrix_[5] +
		     matrix.matrix_[8] * matrix_[8];

	matrix_[0] = temp[0];
	matrix_[1] = temp[1];
	matrix_[2] = temp[2];
	matrix_[3] = temp[3];
	matrix_[4] = temp[4];
	matrix_[5] = temp[5];
	matrix_[6] = temp[6];
	matrix_[7] = temp[7];
	matrix_[8] = temp[8];
}



TEMPLATE void QUAL::Add(const RotationMatrix& matrix) {
	matrix_[0] += matrix.matrix_[0];
	matrix_[1] += matrix.matrix_[1];
	matrix_[2] += matrix.matrix_[2];

	matrix_[3] += matrix.matrix_[3];
	matrix_[4] += matrix.matrix_[4];
	matrix_[5] += matrix.matrix_[5];

	matrix_[6] += matrix.matrix_[6];
	matrix_[7] += matrix.matrix_[7];
	matrix_[8] += matrix.matrix_[8];
}

TEMPLATE void QUAL::Add(const RotationMatrix& matrix1, const RotationMatrix& matrix2) {
	matrix_[0] = matrix1.matrix_[0] + matrix2.matrix_[0];
	matrix_[1] = matrix1.matrix_[1] + matrix2.matrix_[1];
	matrix_[2] = matrix1.matrix_[2] + matrix2.matrix_[2];

	matrix_[3] = matrix1.matrix_[3] + matrix2.matrix_[3];
	matrix_[4] = matrix1.matrix_[4] + matrix2.matrix_[4];
	matrix_[5] = matrix1.matrix_[5] + matrix2.matrix_[5];

	matrix_[6] = matrix1.matrix_[6] + matrix2.matrix_[6];
	matrix_[7] = matrix1.matrix_[7] + matrix2.matrix_[7];
	matrix_[8] = matrix1.matrix_[8] + matrix2.matrix_[8];
}

TEMPLATE void QUAL::Add(_TVarType scalar) {
	matrix_[0] += scalar;
	matrix_[1] += scalar;
	matrix_[2] += scalar;

	matrix_[3] += scalar;
	matrix_[4] += scalar;
	matrix_[5] += scalar;

	matrix_[6] += scalar;
	matrix_[7] += scalar;
	matrix_[8] += scalar;
}

TEMPLATE void QUAL::Sub(const RotationMatrix& matrix) {
	matrix_[0] -= matrix.matrix_[0];
	matrix_[1] -= matrix.matrix_[1];
	matrix_[2] -= matrix.matrix_[2];

	matrix_[3] -= matrix.matrix_[3];
	matrix_[4] -= matrix.matrix_[4];
	matrix_[5] -= matrix.matrix_[5];

	matrix_[6] -= matrix.matrix_[6];
	matrix_[7] -= matrix.matrix_[7];
	matrix_[8] -= matrix.matrix_[8];
}

TEMPLATE void QUAL::Sub(const RotationMatrix& matrix1, const RotationMatrix& matrix2) {
	matrix_[0] = matrix1.matrix_[0] - matrix2.matrix_[0];
	matrix_[1] = matrix1.matrix_[1] - matrix2.matrix_[1];
	matrix_[2] = matrix1.matrix_[2] - matrix2.matrix_[2];

	matrix_[3] = matrix1.matrix_[3] - matrix2.matrix_[3];
	matrix_[4] = matrix1.matrix_[4] - matrix2.matrix_[4];
	matrix_[5] = matrix1.matrix_[5] - matrix2.matrix_[5];

	matrix_[6] = matrix1.matrix_[6] - matrix2.matrix_[6];
	matrix_[7] = matrix1.matrix_[7] - matrix2.matrix_[7];
	matrix_[8] = matrix1.matrix_[8] - matrix2.matrix_[8];
}

TEMPLATE void QUAL::Sub(_TVarType scalar) {
	matrix_[0] -= scalar;
	matrix_[1] -= scalar;
	matrix_[2] -= scalar;

	matrix_[3] -= scalar;
	matrix_[4] -= scalar;
	matrix_[5] -= scalar;

	matrix_[6] -= scalar;
	matrix_[7] -= scalar;
	matrix_[8] -= scalar;
}

TEMPLATE void QUAL::Set(const RotationMatrix& matrix) {
	matrix_[0] = matrix.matrix_[0];
	matrix_[1] = matrix.matrix_[1];
	matrix_[2] = matrix.matrix_[2];

	matrix_[3] = matrix.matrix_[3];
	matrix_[4] = matrix.matrix_[4];
	matrix_[5] = matrix.matrix_[5];

	matrix_[6] = matrix.matrix_[6];
	matrix_[7] = matrix.matrix_[7];
	matrix_[8] = matrix.matrix_[8];
}

TEMPLATE void QUAL::Set(_TVarType* matrix) {
	matrix_[0] = matrix[0];
	matrix_[1] = matrix[1];
	matrix_[2] = matrix[2];

	matrix_[3] = matrix[3];
	matrix_[4] = matrix[4];
	matrix_[5] = matrix[5];

	matrix_[6] = matrix[6];
	matrix_[7] = matrix[7];
	matrix_[8] = matrix[8];
}

TEMPLATE void QUAL::Set(_TVarType p11, _TVarType p12, _TVarType p13,
			_TVarType p21, _TVarType p22, _TVarType p23,
			_TVarType p31, _TVarType p32, _TVarType p33) {
	matrix_[0] = p11;
	matrix_[1] = p12;
	matrix_[2] = p13;

	matrix_[3] = p21;
	matrix_[4] = p22;
	matrix_[5] = p23;

	matrix_[6] = p31;
	matrix_[7] = p32;
	matrix_[8] = p33;
}

TEMPLATE int QUAL::GetRawDataSize() {
	return sizeof(_TVarType) * 9;
}

TEMPLATE int QUAL::GetRawData(uint8* data) {
	_TVarType* _data = (_TVarType*)data;

	_data[0] = matrix_[0];
	_data[1] = matrix_[1];
	_data[2] = matrix_[2];
	_data[3] = matrix_[3];
	_data[4] = matrix_[4];
	_data[5] = matrix_[5];
	_data[6] = matrix_[6];
	_data[7] = matrix_[7];
	_data[8] = matrix_[8];

	return GetRawDataSize();
}

TEMPLATE int QUAL::SetRawData(uint8* data) {
	_TVarType* _data = (_TVarType*)data;

	matrix_[0] = _data[0];
	matrix_[1] = _data[1];
	matrix_[2] = _data[2];
	matrix_[3] = _data[3];
	matrix_[4] = _data[4];
	matrix_[5] = _data[5];
	matrix_[6] = _data[6];
	matrix_[7] = _data[7];
	matrix_[8] = _data[8];

	return GetRawDataSize();
}

TEMPLATE RotationMatrix<float> QUAL::ToFloat() const {
	return RotationMatrix<float>((float)matrix_[0], (float)matrix_[1], (float)matrix_[2],
				     (float)matrix_[3], (float)matrix_[4], (float)matrix_[5],
				     (float)matrix_[6], (float)matrix_[7], (float)matrix_[8]);
}

TEMPLATE RotationMatrix<double> QUAL::ToDouble() const {
	return RotationMatrix<double>((double)matrix_[0], (double)matrix_[1], (double)matrix_[2],
				      (double)matrix_[3], (double)matrix_[4], (double)matrix_[5],
				      (double)matrix_[6], (double)matrix_[7], (double)matrix_[8]);
}
