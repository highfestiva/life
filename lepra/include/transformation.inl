
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



TEMPLATE QUAL::Transformation() {
}

TEMPLATE QUAL::Transformation(const RotationMatrix<_TVarType>& orientation,
			      const Vector3D<_TVarType>& position) :
	orientation_(orientation),
	position_(position) {
}

TEMPLATE QUAL::Transformation(const Quaternion<_TVarType>& orientation,
			      const Vector3D<_TVarType>& position) :
	orientation_(orientation),
	position_(position) {
}

TEMPLATE QUAL::Transformation(const _TVarType data[7]) :
	orientation_(data[0], data[1], data[2], data[3]),
	position_(data[4], data[5], data[6]) {
}

TEMPLATE QUAL::Transformation(const Transformation& transformation) :
	orientation_(transformation.orientation_),
	position_(transformation.position_) {
}

TEMPLATE void QUAL::SetIdentity() {
	orientation_.SetIdentity();
	position_.Set(0, 0, 0);
}

TEMPLATE RotationMatrix<_TVarType> QUAL::GetOrientationAsMatrix() const {
	return orientation_.GetAsRotationMatrix();
}

TEMPLATE const Quaternion<_TVarType>& QUAL::GetOrientation() const {
	return orientation_;
}

TEMPLATE const Vector3D<_TVarType>& QUAL::GetPosition() const {
	return position_;
}

TEMPLATE Quaternion<_TVarType>& QUAL::GetOrientation() {
	return orientation_;
}

TEMPLATE Vector3D<_TVarType>& QUAL::GetPosition() {
	return position_;
}

TEMPLATE void QUAL::SetPosition(const Vector3D<_TVarType>& position) {
	position_ = position;
}

TEMPLATE void QUAL::SetOrientation(const RotationMatrix<_TVarType>& orientation) {
	orientation_ = orientation;
}

TEMPLATE void QUAL::SetOrientation(const Quaternion<_TVarType>& orientation) {
	orientation_ = orientation;
}

TEMPLATE Vector3D<_TVarType> QUAL::Transform(const Vector3D<_TVarType>& vector) const {
	return orientation_.GetRotatedVector(vector) + position_;
}

TEMPLATE Vector3D<_TVarType> QUAL::InverseTransform(const Vector3D<_TVarType>& vector) const {
	return orientation_.GetInverseRotatedVector(vector - position_);
}

TEMPLATE Transformation<_TVarType> QUAL::Transform(const Transformation& transformation) const {
	Transformation _transformation(orientation_ * transformation.orientation_,
					orientation_.GetRotatedVector(transformation.position_) + position_);
	return _transformation;
}

TEMPLATE Transformation<_TVarType> QUAL::InverseTransform(const Transformation& transformation) const {
	// The division is defined as O / T = O' * T.
	Transformation _transformation(orientation_,
		orientation_.GetInverseRotatedVector(transformation.position_ - position_));
	_transformation.GetOrientation().InvAMulB(transformation.orientation_);
	return (_transformation);
}

TEMPLATE void QUAL::FastInverseTransform(const Transformation& from, const quat inverse, const Transformation& to) {
	orientation_ = from.orientation_;
	orientation_.FastInverseRotatedVector(inverse, position_, to.position_ - from.position_);
	orientation_.FastInvAMulB(to.orientation_.a, to.orientation_.b, to.orientation_.c, to.orientation_.d);
}

TEMPLATE Transformation<_TVarType> QUAL::Inverse() const {
	Transformation _transformation(orientation_.GetInverse(), -position_);
	return _transformation;
}

TEMPLATE void QUAL::MoveForward(_TVarType distance) {
	position_ += orientation_.GetAxisY() * distance;
}

TEMPLATE void QUAL::MoveRight(_TVarType distance) {
	position_ += orientation_.GetAxisX() * distance;
}

TEMPLATE void QUAL::MoveUp(_TVarType distance) {
	position_ += orientation_.GetAxisZ() * distance;
}

TEMPLATE void QUAL::MoveBackward(_TVarType distance) {
	position_ -= orientation_.GetAxisY() * distance;
}

TEMPLATE void QUAL::MoveLeft(_TVarType distance) {
	position_ -= orientation_.GetAxisX() * distance;
}

TEMPLATE void QUAL::MoveDown(_TVarType distance) {
	position_ -= orientation_.GetAxisZ() * distance;
}

TEMPLATE void QUAL::RotateAroundAnchor(const Vector3D<_TVarType>& anchor,
	const Vector3D<_TVarType>& axis, _TVarType angle) {
	position_ -= anchor;
	Quaternion<_TVarType> rotation;
	rotation.RotateAroundVector(axis, angle);
	position_ *= rotation;
	position_ += anchor;
	orientation_ = rotation*orientation_;
}

TEMPLATE void QUAL::RotateYaw(_TVarType angle) {
	orientation_.RotateAroundOwnZ(angle);
}

TEMPLATE void QUAL::RotatePitch(_TVarType angle) {
	orientation_.RotateAroundOwnX(angle);
}

TEMPLATE void QUAL::RotateRoll(_TVarType angle) {
	orientation_.RotateAroundOwnY(angle);
}

TEMPLATE void QUAL::MoveNorth(_TVarType distance) {
	position_ += Vector3D<_TVarType>(0, 0, distance);
}

TEMPLATE void QUAL::MoveEast(_TVarType distance) {
	position_ += Vector3D<_TVarType>(distance, 0, 0);
}

TEMPLATE void QUAL::MoveWorldUp(_TVarType distance) {
	position_ += Vector3D<_TVarType>(0, distance, 0);
}

TEMPLATE void QUAL::MoveSouth(_TVarType distance) {
	position_ -= Vector3D<_TVarType>(0, 0, distance);
}

TEMPLATE void QUAL::MoveWest(_TVarType distance) {
	position_ -= Vector3D<_TVarType>(distance, 0, 0);
}

TEMPLATE void QUAL::MoveWorldDown(_TVarType distance) {
	position_ -= Vector3D<_TVarType>(0, distance, 0);
}

TEMPLATE void QUAL::RotateWorldX(_TVarType angle) {
	orientation_.RotateAroundWorldX(angle);
}

TEMPLATE void QUAL::RotateWorldY(_TVarType angle) {
	orientation_.RotateAroundWorldY(angle);
}

TEMPLATE void QUAL::RotateWorldZ(_TVarType angle) {
	orientation_.RotateAroundWorldZ(angle);
}

TEMPLATE bool QUAL::operator == (const Transformation& transformation) {
	return (position_ == transformation.position_ &&
		orientation_ == transformation.orientation_);
}

TEMPLATE bool QUAL::operator != (const Transformation& transformation) {
	return (position_ != transformation.position_ ||
		orientation_ != transformation.orientation_);
}

TEMPLATE Transformation<_TVarType>& QUAL::operator = (const Transformation& transformation) {
	position_ = transformation.position_;
	orientation_ = transformation.orientation_;
	return *this;
}

TEMPLATE Transformation<_TVarType>& QUAL::operator += (const Vector3D<_TVarType>& vector) {
	position_ += vector;
	return *this;
}

TEMPLATE Transformation<_TVarType> QUAL::operator +  (const Vector3D<_TVarType>& vector) const {
	return Transformation(orientation_, position_ + vector);
}

TEMPLATE Transformation<_TVarType>& QUAL::operator *= (const Transformation& transformation) {
	position_ = position_ + orientation_.GetRotatedVector(transformation.position_);
	orientation_ *= transformation.orientation_;
	return *this;
}

TEMPLATE Transformation<_TVarType> QUAL::operator * (const Transformation& transformation) const {
	return (Transform(transformation));
}

TEMPLATE Transformation<_TVarType>& QUAL::operator /= (const Transformation& transformation) {
	position_ = orientation_.GetInverseRotatedVector(transformation.position_ - position_);
	orientation_ /= transformation.orientation_;
	return *this;
}

TEMPLATE Transformation<_TVarType> QUAL::operator / (const Transformation& transformation) const {
	return Transformation(orientation_ / transformation.orientation_,
			      orientation_.GetInverseRotatedVector(transformation.position_ - position_));
}

TEMPLATE Vector3D<_TVarType> QUAL::operator * (const Vector3D<_TVarType>& vector) const {
	return orientation_.GetInverseRotatedVector(vector - position_);
}

TEMPLATE inline Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& vector, const Transformation<_TVarType>& transformation) {
	return transformation.orientation_.GetRotatedVector(vector) + transformation.position_;
}

TEMPLATE inline const Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& vector, const Transformation<_TVarType>& transformation) {
	vector = transformation.orientation_.GetRotatedVector(vector) + transformation.position_;
	return vector;
}

TEMPLATE void QUAL::Get(_TVarType data[7]) const {
	data[0] = orientation_.a;
	data[1] = orientation_.b;
	data[2] = orientation_.c;
	data[3] = orientation_.d;
	data[4] = position_.x;
	data[5] = position_.y;
	data[6] = position_.z;
}

TEMPLATE void QUAL::GetAs4x4Matrix(_TVarType matrix[16]) const {
	RotationMatrix<_TVarType> _orientation(orientation_.GetAsRotationMatrix());

	matrix[0]  = _orientation.GetElement(0);
	matrix[1]  = _orientation.GetElement(1);
	matrix[2]  = _orientation.GetElement(2);
	matrix[3]  = position_.x;

	matrix[4]  = _orientation.GetElement(3);
	matrix[5]  = _orientation.GetElement(4);
	matrix[6]  = _orientation.GetElement(5);
	matrix[7]  = position_.y;

	matrix[8]  = _orientation.GetElement(6);
	matrix[9]  = _orientation.GetElement(7);
	matrix[10] = _orientation.GetElement(8);
	matrix[11] = position_.z;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4TransposeMatrix(_TVarType matrix[16]) const {
	GetAs4x4TransposeMatrix(1, matrix);
}

TEMPLATE void QUAL::GetAs4x4TransposeMatrix(_TVarType scale, _TVarType matrix[16]) const {
	RotationMatrix<_TVarType> _orientation(orientation_.GetAsRotationMatrix());
	if (scale != 1) {
		_orientation *= scale;
	}

	matrix[0]  = _orientation.GetElement(0);
	matrix[1]  = _orientation.GetElement(3);
	matrix[2]  = _orientation.GetElement(6);
	matrix[3]  = 0;

	matrix[4]  = _orientation.GetElement(1);
	matrix[5]  = _orientation.GetElement(4);
	matrix[6]  = _orientation.GetElement(7);
	matrix[7]  = 0;

	matrix[8]  = _orientation.GetElement(2);
	matrix[9]  = _orientation.GetElement(5);
	matrix[10] = _orientation.GetElement(8);
	matrix[11] = 0;

	matrix[12] = position_.x;
	matrix[13] = position_.y;
	matrix[14] = position_.z;
	matrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4InverseMatrix(_TVarType matrix[16]) const {
	RotationMatrix<_TVarType> _orientation(orientation_.GetAsRotationMatrix());

	matrix[0]  = _orientation.GetElement(0);
	matrix[1]  = _orientation.GetElement(3);
	matrix[2]  = _orientation.GetElement(6);
	matrix[3]  = -position_.x;

	matrix[4]  = _orientation.GetElement(1);
	matrix[5]  = _orientation.GetElement(4);
	matrix[6]  = _orientation.GetElement(7);
	matrix[7]  = -position_.y;

	matrix[8]  = _orientation.GetElement(2);
	matrix[9]  = _orientation.GetElement(5);
	matrix[10] = _orientation.GetElement(8);
	matrix[11] = -position_.z;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4InverseTransposeMatrix(_TVarType matrix[16]) const {
	RotationMatrix<_TVarType> _orientation(orientation_.GetAsRotationMatrix());

	matrix[0]  = _orientation.GetElement(0);
	matrix[1]  = _orientation.GetElement(1);
	matrix[2]  = _orientation.GetElement(2);
	matrix[3]  = 0;

	matrix[4]  = _orientation.GetElement(3);
	matrix[5]  = _orientation.GetElement(4);
	matrix[6]  = _orientation.GetElement(5);
	matrix[7]  = 0;

	matrix[8]  = _orientation.GetElement(6);
	matrix[9]  = _orientation.GetElement(7);
	matrix[10] = _orientation.GetElement(8);
	matrix[11] = 0;

	matrix[12] = -position_.x;
	matrix[13] = -position_.y;
	matrix[14] = -position_.z;
	matrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4OrientationMatrix(_TVarType matrix[16]) const {
	RotationMatrix<_TVarType> _orientation(orientation_.GetAsRotationMatrix());

	matrix[0]  = _orientation.GetElement(0);
	matrix[1]  = _orientation.GetElement(1);
	matrix[2]  = _orientation.GetElement(2);
	matrix[3]  = 0;

	matrix[4]  = _orientation.GetElement(3);
	matrix[5]  = _orientation.GetElement(4);
	matrix[6]  = _orientation.GetElement(5);
	matrix[7]  = 0;

	matrix[8]  = _orientation.GetElement(6);
	matrix[9]  = _orientation.GetElement(7);
	matrix[10] = _orientation.GetElement(8);
	matrix[11] = 0;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4InverseOrientationMatrix(_TVarType matrix[16]) const {
	RotationMatrix<_TVarType> _orientation(orientation_.GetAsRotationMatrix());

	matrix[0]  = _orientation.GetElement(0);
	matrix[1]  = _orientation.GetElement(3);
	matrix[2]  = _orientation.GetElement(6);
	matrix[3]  = 0;

	matrix[4]  = _orientation.GetElement(1);
	matrix[5]  = _orientation.GetElement(4);
	matrix[6]  = _orientation.GetElement(7);
	matrix[7]  = 0;

	matrix[8]  = _orientation.GetElement(2);
	matrix[9]  = _orientation.GetElement(5);
	matrix[10] = _orientation.GetElement(8);
	matrix[11] = 0;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}

TEMPLATE void QUAL::Interpolate(const Transformation& start,
				const Transformation& end,
				_TVarType time) {
	orientation_.Slerp(start.orientation_, end.orientation_, time);
	position_ = start.position_ + (end.position_ - start.position_) * time;
}

TEMPLATE Transformation<float> QUAL::ToFloat() const {
	return Transformation<float>(orientation_.ToFloat(), position_.ToFloat());
}

TEMPLATE Transformation<double> QUAL::ToDouble() const {
	return Transformation<double>(orientation_.ToDouble(), position_.ToDouble());
}
