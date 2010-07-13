
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



TEMPLATE QUAL::Transformation()
{
}

TEMPLATE QUAL::Transformation(const RotationMatrix<_TVarType>& pOrientation,
			      const Vector3D<_TVarType>& pPosition) :
	mOrientation(pOrientation),
	mPosition(pPosition)
{
}

TEMPLATE QUAL::Transformation(const Quaternion<_TVarType>& pOrientation,
			      const Vector3D<_TVarType>& pPosition) :
	mOrientation(pOrientation),
	mPosition(pPosition)
{
}

TEMPLATE QUAL::Transformation(const _TVarType pData[7]) :
	mOrientation(pData[0], pData[1], pData[2], pData[3]),
	mPosition(pData[4], pData[5], pData[6])
{
}

TEMPLATE QUAL::Transformation(const Transformation& pTransformation) :
	mOrientation(pTransformation.mOrientation),
	mPosition(pTransformation.mPosition)
{
}

TEMPLATE QUAL::~Transformation()
{
}

TEMPLATE void QUAL::SetIdentity()
{
	mOrientation.SetIdentity();
	mPosition.Set(0, 0, 0);
}

TEMPLATE RotationMatrix<_TVarType> QUAL::GetOrientationAsMatrix() const
{
	return mOrientation.GetAsRotationMatrix();
}

TEMPLATE const Quaternion<_TVarType>& QUAL::GetOrientation() const
{
	return mOrientation;
}

TEMPLATE const Vector3D<_TVarType>& QUAL::GetPosition() const
{
	return mPosition;
}

TEMPLATE Quaternion<_TVarType>& QUAL::GetOrientation()
{
	return mOrientation;
}

TEMPLATE Vector3D<_TVarType>& QUAL::GetPosition()
{
	return mPosition;
}

TEMPLATE void QUAL::SetPosition(const Vector3D<_TVarType>& pPosition)
{
	mPosition = pPosition;
}

TEMPLATE void QUAL::SetOrientation(const RotationMatrix<_TVarType>& pOrientation)
{
	mOrientation = pOrientation;
}

TEMPLATE void QUAL::SetOrientation(const Quaternion<_TVarType>& pOrientation)
{
	mOrientation = pOrientation;
}

TEMPLATE Vector3D<_TVarType> QUAL::Transform(const Vector3D<_TVarType>& pVector) const
{
	return mOrientation.GetRotatedVector(pVector) + mPosition;
}

TEMPLATE Vector3D<_TVarType> QUAL::InverseTransform(const Vector3D<_TVarType>& pVector) const
{
	return mOrientation.GetInverseRotatedVector(pVector - mPosition);
}

TEMPLATE Transformation<_TVarType> QUAL::Transform(const Transformation& pTransformation) const
{
	Transformation lTransformation(mOrientation * pTransformation.mOrientation,
					mOrientation.GetRotatedVector(pTransformation.mPosition) + mPosition);
	return lTransformation;
}

TEMPLATE Transformation<_TVarType> QUAL::InverseTransform(const Transformation& pTransformation) const
{
	// The division is defined as O / T = O' * T.
	Transformation lTransformation(mOrientation,
		mOrientation.GetInverseRotatedVector(pTransformation.mPosition - mPosition));
	lTransformation.GetOrientation().InvAMulB(pTransformation.mOrientation);
	return (lTransformation);
}

TEMPLATE void QUAL::FastInverseTransform(const Transformation& pFrom, const QuaternionF pInverse, const Transformation& pTo)
{
	mOrientation = pFrom.mOrientation;
	mOrientation.FastInverseRotatedVector(pInverse, mPosition, pTo.mPosition - pFrom.mPosition);
	mOrientation.InvAMulB(pTo.mOrientation.mA, pTo.mOrientation.mB, pTo.mOrientation.mC, pTo.mOrientation.mD);
}

TEMPLATE Transformation<_TVarType> QUAL::Inverse() const
{
	Transformation lTransformation(mOrientation.GetInverse(), -mPosition);
	return lTransformation;
}

TEMPLATE void QUAL::MoveForward(_TVarType pDistance)
{
	mPosition += mOrientation.GetAxisY() * pDistance;
}

TEMPLATE void QUAL::MoveRight(_TVarType pDistance)
{
	mPosition += mOrientation.GetAxisX() * pDistance;
}

TEMPLATE void QUAL::MoveUp(_TVarType pDistance)
{
	mPosition += mOrientation.GetAxisZ() * pDistance;
}

TEMPLATE void QUAL::MoveBackward(_TVarType pDistance)
{
	mPosition -= mOrientation.GetAxisY() * pDistance;
}

TEMPLATE void QUAL::MoveLeft(_TVarType pDistance)
{
	mPosition -= mOrientation.GetAxisX() * pDistance;
}

TEMPLATE void QUAL::MoveDown(_TVarType pDistance)
{
	mPosition -= mOrientation.GetAxisZ() * pDistance;
}

TEMPLATE void QUAL::RotateAroundAnchor(const Vector3D<_TVarType>& pAnchor,
	const Vector3D<_TVarType>& pAxis, _TVarType pAngle)
{
	mPosition -= pAnchor;
	Quaternion<_TVarType> lRotation;
	lRotation.RotateAroundVector(pAxis, pAngle);
	mPosition *= lRotation;
	mPosition += pAnchor;
	mOrientation = lRotation*mOrientation;
}

TEMPLATE void QUAL::RotateYaw(_TVarType pAngle)
{
	mOrientation.RotateAroundOwnZ(pAngle);
}

TEMPLATE void QUAL::RotatePitch(_TVarType pAngle)
{
	mOrientation.RotateAroundOwnX(pAngle);
}

TEMPLATE void QUAL::RotateRoll(_TVarType pAngle)
{
	mOrientation.RotateAroundOwnY(pAngle);
}

TEMPLATE void QUAL::MoveNorth(_TVarType pDistance)
{
	mPosition += Vector3D<_TVarType>(0, 0, pDistance);
}

TEMPLATE void QUAL::MoveEast(_TVarType pDistance)
{
	mPosition += Vector3D<_TVarType>(pDistance, 0, 0);
}

TEMPLATE void QUAL::MoveWorldUp(_TVarType pDistance)
{
	mPosition += Vector3D<_TVarType>(0, pDistance, 0);
}

TEMPLATE void QUAL::MoveSouth(_TVarType pDistance)
{
	mPosition -= Vector3D<_TVarType>(0, 0, pDistance);
}

TEMPLATE void QUAL::MoveWest(_TVarType pDistance)
{
	mPosition -= Vector3D<_TVarType>(pDistance, 0, 0);
}

TEMPLATE void QUAL::MoveWorldDown(_TVarType pDistance)
{
	mPosition -= Vector3D<_TVarType>(0, pDistance, 0);
}

TEMPLATE void QUAL::RotateWorldX(_TVarType pAngle)
{
	mOrientation.RotateAroundWorldX(pAngle);
}

TEMPLATE void QUAL::RotateWorldY(_TVarType pAngle)
{
	mOrientation.RotateAroundWorldY(pAngle);
}

TEMPLATE void QUAL::RotateWorldZ(_TVarType pAngle)
{
	mOrientation.RotateAroundWorldZ(pAngle);
}

TEMPLATE bool QUAL::operator == (const Transformation& pTransformation)
{
	return (mPosition == pTransformation.mPosition &&
		mOrientation == pTransformation.mOrientation);
}

TEMPLATE bool QUAL::operator != (const Transformation& pTransformation)
{
	return (mPosition != pTransformation.mPosition ||
		mOrientation != pTransformation.mOrientation);
}

TEMPLATE Transformation<_TVarType>& QUAL::operator = (const Transformation& pTransformation)
{
	mPosition = pTransformation.mPosition;
	mOrientation = pTransformation.mOrientation;
	return *this;
}

TEMPLATE Transformation<_TVarType>& QUAL::operator += (const Vector3D<_TVarType>& pVector)
{
	mPosition += pVector;
	return *this;
}

TEMPLATE Transformation<_TVarType> QUAL::operator +  (const Vector3D<_TVarType>& pVector) const
{
	return Transformation(mOrientation, mPosition + pVector);
}

TEMPLATE Transformation<_TVarType>& QUAL::operator *= (const Transformation& pTransformation)
{
	mPosition = mPosition + mOrientation.GetRotatedVector(pTransformation.mPosition);
	mOrientation *= pTransformation.mOrientation;
	return *this;
}

TEMPLATE Transformation<_TVarType> QUAL::operator * (const Transformation& pTransformation) const
{
	return (Transform(pTransformation));
}

TEMPLATE Transformation<_TVarType>& QUAL::operator /= (const Transformation& pTransformation)
{
	mPosition = mOrientation.GetInverseRotatedVector(pTransformation.mPosition - mPosition);
	mOrientation /= pTransformation.mOrientation;
	return *this;
}

TEMPLATE Transformation<_TVarType> QUAL::operator / (const Transformation& pTransformation) const
{
	return Transformation(mOrientation / pTransformation.mOrientation,
			      mOrientation.GetInverseRotatedVector(pTransformation.mPosition - mPosition));
}

TEMPLATE Vector3D<_TVarType> QUAL::operator * (const Vector3D<_TVarType>& pVector) const
{
	return mOrientation.GetInverseRotatedVector(pVector - mPosition);
}

TEMPLATE inline Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& pVector, const Transformation<_TVarType>& pTransformation)
{
	return pTransformation.mOrientation.GetRotatedVector(pVector) + pTransformation.mPosition;
}

TEMPLATE inline const Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& pVector, const Transformation<_TVarType>& pTransformation)
{
	pVector = pTransformation.mOrientation.GetRotatedVector(pVector) + pTransformation.mPosition;
	return pVector;
}

TEMPLATE void QUAL::Get(_TVarType pData[7]) const
{
	pData[0] = mOrientation.GetA();
	pData[1] = mOrientation.GetB();
	pData[2] = mOrientation.GetC();
	pData[3] = mOrientation.GetD();
	pData[4] = mPosition.x;
	pData[5] = mPosition.y;
	pData[6] = mPosition.z;
}

TEMPLATE void QUAL::GetAs4x4Matrix(_TVarType pMatrix[16]) const
{
	RotationMatrix<_TVarType> lOrientation(mOrientation.GetAsRotationMatrix());

	pMatrix[0]  = lOrientation.GetElement(0);
	pMatrix[1]  = lOrientation.GetElement(1);
	pMatrix[2]  = lOrientation.GetElement(2);
	pMatrix[3]  = mPosition.x;

	pMatrix[4]  = lOrientation.GetElement(3);
	pMatrix[5]  = lOrientation.GetElement(4);
	pMatrix[6]  = lOrientation.GetElement(5);
	pMatrix[7]  = mPosition.y;

	pMatrix[8]  = lOrientation.GetElement(6);
	pMatrix[9]  = lOrientation.GetElement(7);
	pMatrix[10] = lOrientation.GetElement(8);
	pMatrix[11] = mPosition.z;

	pMatrix[12] = 0;
	pMatrix[13] = 0;
	pMatrix[14] = 0;
	pMatrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4TransposeMatrix(_TVarType pMatrix[16]) const
{
	RotationMatrix<_TVarType> lOrientation(mOrientation.GetAsRotationMatrix());

	pMatrix[0]  = lOrientation.GetElement(0);
	pMatrix[1]  = lOrientation.GetElement(3);
	pMatrix[2]  = lOrientation.GetElement(6);
	pMatrix[3]  = 0;

	pMatrix[4]  = lOrientation.GetElement(1);
	pMatrix[5]  = lOrientation.GetElement(4);
	pMatrix[6]  = lOrientation.GetElement(7);
	pMatrix[7]  = 0;

	pMatrix[8]  = lOrientation.GetElement(2);
	pMatrix[9]  = lOrientation.GetElement(5);
	pMatrix[10] = lOrientation.GetElement(8);
	pMatrix[11] = 0;

	pMatrix[12] = mPosition.x;
	pMatrix[13] = mPosition.y;
	pMatrix[14] = mPosition.z;
	pMatrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4InverseMatrix(_TVarType pMatrix[16]) const
{
	RotationMatrix<_TVarType> lOrientation(mOrientation.GetAsRotationMatrix());

	pMatrix[0]  = lOrientation.GetElement(0);
	pMatrix[1]  = lOrientation.GetElement(3);
	pMatrix[2]  = lOrientation.GetElement(6);
	pMatrix[3]  = -mPosition.x;

	pMatrix[4]  = lOrientation.GetElement(1);
	pMatrix[5]  = lOrientation.GetElement(4);
	pMatrix[6]  = lOrientation.GetElement(7);
	pMatrix[7]  = -mPosition.y;

	pMatrix[8]  = lOrientation.GetElement(2);
	pMatrix[9]  = lOrientation.GetElement(5);
	pMatrix[10] = lOrientation.GetElement(8);
	pMatrix[11] = -mPosition.z;

	pMatrix[12] = 0;
	pMatrix[13] = 0;
	pMatrix[14] = 0;
	pMatrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4InverseTransposeMatrix(_TVarType pMatrix[16]) const
{
	RotationMatrix<_TVarType> lOrientation(mOrientation.GetAsRotationMatrix());

	pMatrix[0]  = lOrientation.GetElement(0);
	pMatrix[1]  = lOrientation.GetElement(1);
	pMatrix[2]  = lOrientation.GetElement(2);
	pMatrix[3]  = 0;

	pMatrix[4]  = lOrientation.GetElement(3);
	pMatrix[5]  = lOrientation.GetElement(4);
	pMatrix[6]  = lOrientation.GetElement(5);
	pMatrix[7]  = 0;

	pMatrix[8]  = lOrientation.GetElement(6);
	pMatrix[9]  = lOrientation.GetElement(7);
	pMatrix[10] = lOrientation.GetElement(8);
	pMatrix[11] = 0;

	pMatrix[12] = -mPosition.x;
	pMatrix[13] = -mPosition.y;
	pMatrix[14] = -mPosition.z;
	pMatrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4OrientationMatrix(_TVarType pMatrix[16]) const
{
	RotationMatrix<_TVarType> lOrientation(mOrientation.GetAsRotationMatrix());

	pMatrix[0]  = lOrientation.GetElement(0);
	pMatrix[1]  = lOrientation.GetElement(1);
	pMatrix[2]  = lOrientation.GetElement(2);
	pMatrix[3]  = 0;

	pMatrix[4]  = lOrientation.GetElement(3);
	pMatrix[5]  = lOrientation.GetElement(4);
	pMatrix[6]  = lOrientation.GetElement(5);
	pMatrix[7]  = 0;

	pMatrix[8]  = lOrientation.GetElement(6);
	pMatrix[9]  = lOrientation.GetElement(7);
	pMatrix[10] = lOrientation.GetElement(8);
	pMatrix[11] = 0;

	pMatrix[12] = 0;
	pMatrix[13] = 0;
	pMatrix[14] = 0;
	pMatrix[15] = 1;
}

TEMPLATE void QUAL::GetAs4x4InverseOrientationMatrix(_TVarType pMatrix[16]) const
{
	RotationMatrix<_TVarType> lOrientation(mOrientation.GetAsRotationMatrix());

	pMatrix[0]  = lOrientation.GetElement(0);
	pMatrix[1]  = lOrientation.GetElement(3);
	pMatrix[2]  = lOrientation.GetElement(6);
	pMatrix[3]  = 0;

	pMatrix[4]  = lOrientation.GetElement(1);
	pMatrix[5]  = lOrientation.GetElement(4);
	pMatrix[6]  = lOrientation.GetElement(7);
	pMatrix[7]  = 0;

	pMatrix[8]  = lOrientation.GetElement(2);
	pMatrix[9]  = lOrientation.GetElement(5);
	pMatrix[10] = lOrientation.GetElement(8);
	pMatrix[11] = 0;

	pMatrix[12] = 0;
	pMatrix[13] = 0;
	pMatrix[14] = 0;
	pMatrix[15] = 1;
}

TEMPLATE void QUAL::Interpolate(const Transformation& pStart,
				const Transformation& pEnd,
				_TVarType pTime)
{
	mOrientation.Slerp(pStart.mOrientation, pEnd.mOrientation, pTime);
	mPosition = pStart.mPosition + (pEnd.mPosition - pStart.mPosition) * pTime;
}

TEMPLATE Transformation<float> QUAL::ToFloat() const
{
	return Transformation<float>(mOrientation.ToFloat(), mPosition.ToFloat());
}

TEMPLATE Transformation<double> QUAL::ToDouble() const
{
	return Transformation<double>(mOrientation.ToDouble(), mPosition.ToDouble());
}
