
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#define V3D_TEMPLATE template<class _TVarType>
#define V3D_QUAL Vector3D<_TVarType>



V3D_TEMPLATE V3D_QUAL::Vector3D():
	x(0),
	y(0),
	z(0)
{
}

V3D_TEMPLATE V3D_QUAL::Vector3D(_TVarType pX, _TVarType pY, _TVarType pZ):
	x(pX),
	y(pY),
	z(pZ)
{
}

V3D_TEMPLATE V3D_QUAL::Vector3D(const Vector3D<_TVarType>& pV):
	x(pV.x),
	y(pV.y),
	z(pV.z)
{
}

// Cross product constructor 1.
V3D_TEMPLATE V3D_QUAL::Vector3D(const Vector3D<_TVarType>& pV1, const Vector3D<_TVarType>& pV2):
	x(pV1.y * pV2.z - pV1.z * pV2.y),
	y(pV1.z * pV2.x - pV1.x * pV2.z),
	z(pV1.x * pV2.y - pV1.y * pV2.x)
{
}

// Cross product constructor 2.
V3D_TEMPLATE V3D_QUAL::Vector3D(const Vector3D<_TVarType>& pV1, _TVarType pX, _TVarType pY, _TVarType pZ):
	x(pV1.y * pZ - pV1.z * pY),
	y(pV1.z * pX - pV1.x * pZ),
	z(pV1.x * pY - pV1.y * pX)
{
}

// Cross product constructor 3.
V3D_TEMPLATE V3D_QUAL::Vector3D(_TVarType pX1, _TVarType pY1, _TVarType pZ1,
	_TVarType pX2, _TVarType pY2, _TVarType pZ2):
	x(pY1 * pZ2 - pZ1 * pY2),
	y(pZ1 * pX2 - pX1 * pZ2),
	z(pX1 * pY2 - pY1 * pX2)
{
}

V3D_TEMPLATE void V3D_QUAL::Normalize(_TVarType pLength)
{
	_TVarType lLength = GetLength();
	if (lLength >= Math::Traits<_TVarType>::FullEps())
	{
		Div(lLength);
		Mul(pLength);
	}
	else
	{
		Set(0, 0, 0);
	}
}

V3D_TEMPLATE void V3D_QUAL::GetOrthogonals(Vector3D<_TVarType>& pY, Vector3D<_TVarType>& pZ) const
{
	if (fabs(z) > 0.7071)
	{
		// The length of the vector projected on the y-z plane.
		_TVarType lL = y * y + z * z;
		_TVarType lScale = (_TVarType)(1 / sqrt(lL));
		pY.x = 0;
		pY.y = -z * lScale;
		pY.z =  y * lScale;

		// Set Z = X x Y.
		pZ.x =  lL * lScale;	// = sqrt(lL)
		pZ.y = -x * pY.z;
		pZ.z =  x * pY.y;
	}
	else
	{
		// The length of the vector projected on the x-y plane.
		_TVarType lL = x * x + y * y;
		_TVarType lScale = (_TVarType)(1 / sqrt(lL));
		pY.x = -y * lScale;
		pY.y =  x * lScale;
		pY.z =  0;

		// Set Z = X x Y.
		pZ.x = -z * pY.y;
		pZ.y =  z * pY.x;
		pZ.z = lL * lScale;	// = sqrt(lL)
	}
}

V3D_TEMPLATE void V3D_QUAL::Cross(const Vector3D<_TVarType>& pV1, const Vector3D<_TVarType>& pV2)
{
	x = pV1.y * pV2.z - pV1.z * pV2.y;
	y = pV1.z * pV2.x - pV1.x * pV2.z;
	z = pV1.x * pV2.y - pV1.y * pV2.x;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::Cross(const Vector3D<_TVarType>& pV) const
{
	return Vector3D<_TVarType>(y * pV.z - z * pV.y,
		z * pV.x - x * pV.z,
		x * pV.y - y * pV.x);
}

V3D_TEMPLATE void V3D_QUAL::CrossUnit(const Vector3D<_TVarType>& pV1, const Vector3D<_TVarType>& pV2)
{
	Cross(pV1, pV2); 
	Normalize();
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::CrossUnit(const Vector3D<_TVarType>& pV) const
{
	Vector3D<_TVarType> lTemp(*this, pV);	// Cross product constructor.
	lTemp.Normalize();
	return lTemp;
}

V3D_TEMPLATE void V3D_QUAL::GetSphericalAngles(_TVarType& pTheta, _TVarType& pPhi) const
{
	const Vector3D<_TVarType> lZAxis(0, 0, 1);
	pTheta = ::acos((*this)*lZAxis/GetLength());

	Vector3D<_TVarType> lXY = ProjectOntoPlane(lZAxis);
	_TVarType lLength = lXY.GetLength();
	if (lLength <= Math::Traits<_TVarType>::FullEps())
	{
		pPhi = 0;
	}
	else if (lXY.y >= 0)
	{
		pPhi = ::acos(lXY*Vector3D<_TVarType>(1, 0, 0)/lLength);
	}
	else
	{
		pPhi = 2*(_TVarType)PI-::acos(lXY*Vector3D<_TVarType>(1, 0, 0)/lLength);
	}
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::ProjectOntoPlane(const Vector3D<_TVarType>& pPlaneNormal) const
{
	return ((*this)-((*this)*pPlaneNormal)*pPlaneNormal);
}

// Cross product
V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator/= (const Vector3D<_TVarType>& pV)
{
	_TVarType lCx, lCy, lCz;

	lCx = y * pV.z - z * pV.y;
	lCy = z * pV.x - x * pV.z;
	lCz = x * pV.y - y * pV.x;

	x = lCx;
	y = lCy;
	z = lCz;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator/ (const Vector3D<_TVarType>& pV)
{
	Vector3D lTemp;

	lTemp.x = y * pV.z - z * pV.y;
	lTemp.y = z * pV.x - x * pV.z;
	lTemp.z = x * pV.y - y * pV.x;

	return lTemp;
}

V3D_TEMPLATE void V3D_QUAL::Set(_TVarType pX, _TVarType pY, _TVarType pZ)
{
	x = pX;
	y = pY;
	z = pZ;
}

V3D_TEMPLATE void V3D_QUAL::Set(const Vector3D<_TVarType>& pV)
{
	x = pV.x;
	y = pV.y;
	z = pV.z;
}

V3D_TEMPLATE void V3D_QUAL::SetPolar(_TVarType pAngleZ, _TVarType pAngleY, _TVarType pLength)
{
	_TVarType lRadius = (_TVarType)cos(pAngleZ) * pLength;

	x = (_TVarType)cos(pAngleY) * lRadius;
	y = (_TVarType)sin(pAngleZ) * pLength;
	z = (_TVarType)sin(pAngleY) * lRadius;
}

V3D_TEMPLATE void V3D_QUAL::SetBezier(const Vector3D<_TVarType>& pVA, 
							  const Vector3D<_TVarType>& pVTA, 
							  const Vector3D<_TVarType>& pVTB, 
							  const Vector3D<_TVarType>& pVB, 
							  _TVarType pT)
{
	// Calculate lots of temporary variables to minimize number of operations.
	_TVarType lT1_05 = (_TVarType)(0.5 * pT);

	_TVarType lT2_10 = pT * pT;
	_TVarType lT2_05 = (_TVarType)(0.5 * lT2_10);
	_TVarType lT2_20 = (_TVarType)(2.0 * lT2_10);
	_TVarType lT2_25 = (_TVarType)(2.5 * lT2_10);

	_TVarType lT3 = lT2_10 * pT;
	_TVarType lT3_05 = (_TVarType)(0.5 * lT3);
	_TVarType lT3_15 = (_TVarType)(1.5 * lT3);

	_TVarType lTA = -lT3_05 + lT2_10 - lT1_05;
	_TVarType lTB =  lT3_15 - lT2_25 + 1.0;
	_TVarType lTC = -lT3_15 + lT2_20 + lT1_05;
	_TVarType lTD =  lT3_05 - lT2_05;

	x = (_TVarType)(lTA * pVA.x  +
			lTB * pVTA.x +
			lTC * pVTB.x +
			lTD * pVB.x);
	y = (_TVarType)(lTA * pVA.y  +
			lTB * pVTA.y +
			lTC * pVTB.y +
			lTD * pVB.y);
	z = (_TVarType)(lTA * pVA.z  +
			lTB * pVTA.z +
			lTC * pVTB.z +
			lTD * pVB.z);
}

V3D_TEMPLATE void V3D_QUAL::Add(_TVarType pX, _TVarType pY, _TVarType pZ)
{
	x += pX;
	y += pY;
	z += pZ;
}

V3D_TEMPLATE void V3D_QUAL::Add(const Vector3D<_TVarType>& pV)
{
	x += pV.x; 
	y += pV.y; 
	z += pV.z;
}

V3D_TEMPLATE void V3D_QUAL::Sub(_TVarType pX, _TVarType pY, _TVarType pZ)
{
	x -= pX;
	y -= pY;
	z -= pZ;
}

V3D_TEMPLATE void V3D_QUAL::Sub(const Vector3D<_TVarType>& pV)
{
	x -= pV.x;
	y -= pV.y;
	z -= pV.z;
}

V3D_TEMPLATE void V3D_QUAL::Mul(_TVarType pScalar)
{
	x *= pScalar;
	y *= pScalar;
	z *= pScalar;
}

V3D_TEMPLATE void V3D_QUAL::Div(_TVarType pScalar)
{
	_TVarType lInvScalar = 1 / pScalar;
	x *= lInvScalar; 
	y *= lInvScalar; 
	z *= lInvScalar;
}

V3D_TEMPLATE _TVarType V3D_QUAL::Dot(const Vector3D<_TVarType>& pV) const
{
	return (x * pV.x + y * pV.y + z * pV.z);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetLength() const
{
	return (_TVarType)sqrt(x * x + y * y + z * z);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetLengthSquared() const
{
	return (x * x + y * y + z * z);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetDistance(const Vector3D<_TVarType>& pV) const
{
	_TVarType lDx = pV.x - x;
	_TVarType lDy = pV.y - y;
	_TVarType lDz = pV.z - z;

	return sqrt(lDx * lDx + lDy * lDy + lDz * lDz);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetDistanceSquared(const Vector3D<_TVarType>& pV) const
{
	_TVarType lDx = pV.x - x;
	_TVarType lDy = pV.y - y;
	_TVarType lDz = pV.z - z;

	return (lDx * lDx + lDy * lDy + lDz * lDz);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetPolarCoordAngleZ() const
{
	_TVarType lX = (_TVarType)sqrt(x * x + z * z);
	_TVarType lY = y;

	if (lX > Math::Traits<_TVarType>::FullEps())
		return atan(lY / lX);
	else
		return (_TVarType)(PI / 2.0);
}

V3D_TEMPLATE _TVarType V3D_QUAL::GetPolarCoordAngleY() const
{
	_TVarType lX = x;
	_TVarType lY = z;

	if (fabs(lX) <= Math::Traits<_TVarType>::FullEps())
	{
		if (lY >= 0)
			return (_TVarType)(PI / 2.0);
		else
			return (_TVarType)(-PI / 2.0);
	}

	if (lX >= 0)
	{
		return atan(lY / lX);
	}
	else
	{
		if (lY >= 0)
			return (_TVarType)((PI / 2) - atan(lY / lX));
		else
			return (_TVarType)(-(PI / 2) - atan(lY / lX));
	}
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator= (const Vector3D<_TVarType>& pV)
{
	x = pV.x;
	y = pV.y;
	z = pV.z;

	return *this;
}

V3D_TEMPLATE bool V3D_QUAL::operator== (const Vector3D<_TVarType>& pV) const
{
	return (x == pV.x && y == pV.y && z == pV.z);
}

V3D_TEMPLATE bool V3D_QUAL::operator!= (const Vector3D<_TVarType>& pV) const
{
	return (x != pV.x || y != pV.y || z != pV.z);
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator+= (const Vector3D<_TVarType>& pV)
{
	x += pV.x;
	y += pV.y;
	z += pV.z;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator+ (const Vector3D<_TVarType>& pV) const
{
	Vector3D lTemp;

	lTemp.x = x + pV.x;
	lTemp.y = y + pV.y;
	lTemp.z = z + pV.z;

	return lTemp;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator-= (const Vector3D<_TVarType>& pV)
{
	x -= pV.x;
	y -= pV.y;
	z -= pV.z;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator- (const Vector3D<_TVarType>& pV) const
{
	Vector3D lTemp;

	lTemp.x = x - pV.x;
	lTemp.y = y - pV.y;
	lTemp.z = z - pV.z;

	return lTemp;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator- () const
{
	Vector3D lTemp;

	lTemp.x = -x;
	lTemp.y = -y;
	lTemp.z = -z;

	return lTemp;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator*= (_TVarType pScalar)
{
	x *= pScalar;
	y *= pScalar;
	z *= pScalar;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator* (_TVarType pScalar) const
{
	Vector3D lTemp;

	lTemp.x = x * pScalar;
	lTemp.y = y * pScalar;
	lTemp.z = z * pScalar;

	return lTemp;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator/= (_TVarType pScalar)
{
	_TVarType lInvScalar = 1 / pScalar;

	x *= lInvScalar;
	y *= lInvScalar;
	z *= lInvScalar;

	return *this;
}

V3D_TEMPLATE Vector3D<_TVarType> V3D_QUAL::operator/ (_TVarType pScalar) const
{
	_TVarType lInvScalar = 1 / pScalar;

	Vector3D lTemp;

	lTemp.x = x * lInvScalar;
	lTemp.y = y * lInvScalar;
	lTemp.z = z * lInvScalar;

	return lTemp;
}

#if !defined(LEPRA_MSVC)

inline V3D_TEMPLATE Vector3D<_TVarType> operator * (_TVarType pScalar, const Vector3D<_TVarType>& pVector)
{
	return pVector * pScalar;
}

inline V3D_TEMPLATE Vector3D<_TVarType> operator / (_TVarType pScalar, const Vector3D<_TVarType>& pVector)
{
	return pVector / pScalar;
}

inline V3D_TEMPLATE void LineDistance(const Vector3D<_TVarType>& pP0, const Vector3D<_TVarType>& pPDir,
		  const Vector3D<_TVarType>& pQ0, const Vector3D<_TVarType>& pQDir,
		  Vector3D<_TVarType>& pP, Vector3D<_TVarType>& pQ)
{
	Vector3D<_TVarType> u(pPDir);
	Vector3D<_TVarType> v(pQDir);

	u.Normalize();
	v.Normalize();

	Vector3D<_TVarType> w0(pP0 - pQ0);
	_TVarType b = u.Dot(v);
	_TVarType d = u.Dot(w0);
	_TVarType e = v.Dot(w0);

	_TVarType sc = (b*e - d) / (1.0f - b*b);
	_TVarType tc = (e - b*d) / (1.0f - b*b);

	pP = (pP0 + u * sc);
	pQ = (pQ0 + v * tc);
}

inline _TVarType LineDistance(const Vector3D<_TVarType>& pPoL, const Vector3D<_TVarType>& pDir, const Vector3D<_TVarType>& pPoint)
{
	Vector3D<_TVarType> lNormDir(pDir);
	lNormDir.Normalize();
	Vector3D<_TVarType> lV(pPoint - pPoL);
	return (lV - lNormDir * lNormDir.Dot(lV)).GetLength();
}

_TVarType LineDistance(const Vector3D<_TVarType>& pPoL, const Vector3D<_TVarType>& pDir, const Vector3D<_TVarType>& pPoint, Vector3D<_TVarType>& pClosestPoint, _TVarType& pDirDistance)
{
	Vector3D<_TVarType> lNormDir(pDir);
	lNormDir.Normalize();
	Vector3D<_TVarType> lV(pPoint - pPoL);
	pDirDistance = lNormDir.Dot(lV);
	Vector3D<_TVarType> lD(lNormDir * pDirDistance);
	pClosestPoint = pPoL + lD;
	return (lV - lD).GetLength();
}

#else // LEPRA_MSVC

// Visual Studio .NET can't handle function templates... (.NET 2003 can? .NET 2005 can!).
// Need to implement the functions separately like this.

inline Vector3D<float32> operator * (float32 pScalar, const Vector3D<float32>& pVector)
{
	return pVector * pScalar;
}

inline Vector3D<float32> operator / (float32 pScalar, const Vector3D<float32>& pVector)
{
	return pVector / pScalar;
}

inline Vector3D<float64> operator * (float64 pScalar, const Vector3D<float64>& pVector)
{
	return pVector * pScalar;
}

inline Vector3D<float64> operator / (float64 pScalar, const Vector3D<float64>& pVector)
{
	return pVector / pScalar;
}

inline void LineDistance(const Vector3D<float>& pP0, const Vector3D<float>& pPDir,
		  const Vector3D<float>& pQ0, const Vector3D<float>& pQDir,
		  Vector3D<float>& pP, Vector3D<float>& pQ)
{
	Vector3D<float> u(pPDir);
	Vector3D<float> v(pQDir);

	u.Normalize();
	v.Normalize();

	Vector3D<float> w0(pP0 - pQ0);
	float b = u.Dot(v);
	float d = u.Dot(w0);
	float e = v.Dot(w0);

	float sc = (b*e - d) / (1.0f - b*b);
	float tc = (e - b*d) / (1.0f - b*b);

	pP = (pP0 + u * sc);
	pQ = (pQ0 + v * tc);
}

inline void LineDistance(const Vector3D<double>& pP0, const Vector3D<double>& pPDir,
		  const Vector3D<double>& pQ0, const Vector3D<double>& pQDir,
		  Vector3D<double>& pP, Vector3D<double>& pQ)
{
	Vector3D<double> u(pPDir);
	Vector3D<double> v(pQDir);

	u.Normalize();
	v.Normalize();

	Vector3D<double> w0(pP0 - pQ0);
	double b = u.Dot(v);
	double d = u.Dot(w0);
	double e = v.Dot(w0);

	double sc = (b*e - d) / (1.0f - b*b);
	double tc = (e - b*d) / (1.0f - b*b);

	pP = (pP0 + u * sc);
	pQ = (pQ0 + v * tc);
}

inline float LineDistance(const Vector3D<float>& pPoL, const Vector3D<float>& pDir, const Vector3D<float>& pPoint)
{
	Vector3D<float> lNormDir(pDir);
	lNormDir.Normalize();
	Vector3D<float> lV(pPoint - pPoL);
	return (lV - lNormDir * lNormDir.Dot(lV)).GetLength();
}

inline double LineDistance(const Vector3D<double>& pPoL, const Vector3D<double>& pDir, const Vector3D<double>& pPoint)
{
	Vector3D<double> lNormDir(pDir);
	lNormDir.Normalize();
	Vector3D<double> lV(pPoint - pPoL);
	return (lV - lNormDir * lNormDir.Dot(lV)).GetLength();
}

inline float LineDistance(const Vector3D<float>& pPoL, const Vector3D<float>& pDir, const Vector3D<float>& pPoint, Vector3D<float>& pClosestPoint, float pDirDistance)
{
	Vector3D<float> lNormDir(pDir);
	lNormDir.Normalize();
	Vector3D<float> lV(pPoint - pPoL);
	pDirDistance = lNormDir.Dot(lV);
	Vector3D<float> lD(lNormDir * pDirDistance);
	pClosestPoint = pPoL + lD;
	return (lV - lD).GetLength();
}

inline double LineDistance(const Vector3D<double>& pPoL, const Vector3D<double>& pDir, const Vector3D<double>& pPoint, Vector3D<double>& pClosestPoint, double pDirDistance)
{
	Vector3D<double> lNormDir(pDir);
	lNormDir.Normalize();
	Vector3D<double> lV(pPoint - pPoL);
	pDirDistance = lNormDir.Dot(lV);
	Vector3D<double> lD(lNormDir * pDirDistance);
	pClosestPoint = pPoL + lD;
	return (lV - lD).GetLength();
}


#endif // !LEPRA_MSVC/LEPRA_MSVC

V3D_TEMPLATE _TVarType V3D_QUAL::operator* (const Vector3D<_TVarType>& pV) const
{
	return(x * pV.x + y * pV.y + z * pV.z);
}

V3D_TEMPLATE int V3D_QUAL::GetRawDataSize()
{
	return sizeof(_TVarType) * 3;
}

V3D_TEMPLATE int V3D_QUAL::GetRawData(uint8* pData)
{
	_TVarType* lData = (_TVarType*)pData;
	lData[0] = x;
	lData[1] = y;
	lData[2] = z;

	return GetRawDataSize();
}

V3D_TEMPLATE int V3D_QUAL::SetRawData(uint8* pData)
{
	_TVarType* lData = (_TVarType*)pData;
	x = lData[0];
	y = lData[1];
	z = lData[2];

	return GetRawDataSize();
}

V3D_TEMPLATE bool V3D_QUAL::IsNullVector() const
{
	return (fabs(x) <= Math::Traits<_TVarType>::FullEps() &&
			fabs(y) <= Math::Traits<_TVarType>::FullEps() &&
			fabs(z) <= Math::Traits<_TVarType>::FullEps());
}

V3D_TEMPLATE Vector3D<float> V3D_QUAL::ToFloat() const
{
	return Vector3D<float>((float)x, (float)y, (float)z);
}

V3D_TEMPLATE Vector3D<double> V3D_QUAL::ToDouble() const
{
	return Vector3D<double>((double)x, (double)y, (double)z);
}

#undef V3D_TEMPLATE
#undef V3D_QUAL
