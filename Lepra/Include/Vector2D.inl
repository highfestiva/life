/*
	Class:  Vector2D
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#define V2D_TEMPLATE template<class _TVarType>
#define V2D_QUAL Vector2D<_TVarType>

V2D_TEMPLATE V2D_QUAL::Vector2D()
{
	x = 0;
	y = 0;
}

V2D_TEMPLATE V2D_QUAL::Vector2D(_TVarType pX, _TVarType pY)
{
	x = pX;
	y = pY;
}

V2D_TEMPLATE V2D_QUAL::Vector2D(const Vector2D<_TVarType>& pV)
{
	x = pV.x;
	y = pV.y;
}

V2D_TEMPLATE void V2D_QUAL::Set(_TVarType pX, _TVarType pY)
{
	x = pX;
	y = pY;
}

V2D_TEMPLATE void V2D_QUAL::Set(const Vector2D<_TVarType>& pV)
{
	x = pV.x;
	y = pV.y;
}

V2D_TEMPLATE void V2D_QUAL::SetPerpCW(const Vector2D<_TVarType>& pV)
{
	x = pV.y;
	y = -pV.x;
}

V2D_TEMPLATE void V2D_QUAL::SetPerpCCW(const Vector2D<_TVarType>& pV)
{
	x = -pV.y;
	y = pV.x;
}

V2D_TEMPLATE void V2D_QUAL::SetPolar(_TVarType pAngle, _TVarType pLength)
{
	x = (_TVarType)cos(pAngle) * pLength;
	y = (_TVarType)sin(pAngle) * pLength;
}

V2D_TEMPLATE void V2D_QUAL::SetBezier(const Vector2D<_TVarType>& pVA, 
						const Vector2D<_TVarType>& pVTA, 
						const Vector2D<_TVarType>& pVTB, 
						const Vector2D<_TVarType>& pVB, 
						_TVarType pT)
{
	// Calculate lots of temporary variables to minimize number of operations.
	_TVarType lT1_05 = (_TVarType)(0.5 * lT);

	_TVarType lT2_10 = lT * lT;
	_TVarType lT2_05 = (_TVarType)(0.5 * lT2_10);
	_TVarType lT2_20 = (_TVarType)(2.0 * lT2_10);
	_TVarType lT2_25 = (_TVarType)(2.5 * lT2_10);

	_TVarType lT3 = lT2_10 * lT;
	_TVarType lT3_05 = (_TVarType)(0.5 * lT3);
	_TVarType lT3_15 = (_TVarType)(1.5 * lT3);

	_TVarType lTA = -lT3_05 + lT2_10 - lT1_05;
	_TVarType lTB = lT3_15 - lT2_25 + 1.0;
	_TVarType lTC = -lT3_15 + lT2_20 + lT1_05;
	_TVarType lTD = lT3_05 - lT2_05;

	x = (_TVarType)(lTA * pVA.x  +
					lTB * pVTA.x +
					lTC * pVTB.x +
					lTD * pVB.x);
	y = (_TVarType)(lTA * pVA.y  +
					lTB * pVTA.y +
					lTC * pVTB.y +
					lTD * pVB.y);
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::GetPerpCW()
{
	return Vector2D<_TVarType>(y, -x);
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::GetPerpCCW()
{
	return Vector2D<_TVarType>(-y, x);
}

V2D_TEMPLATE void V2D_QUAL::Add(_TVarType pX, _TVarType pY)
{
	x += pX;
	y += pY;
}

V2D_TEMPLATE void V2D_QUAL::Add(const Vector2D<_TVarType>& pV)
{
	x += pV.x; 
	y += pV.y; 
}

V2D_TEMPLATE void V2D_QUAL::Sub(_TVarType pX, _TVarType pY)
{
	x -= pX;
	y -= pY;
}

V2D_TEMPLATE void V2D_QUAL::Sub(const Vector2D<_TVarType>& pV)
{
	x -= pV.x;
	y -= pV.y;
}

V2D_TEMPLATE void V2D_QUAL::Mul(_TVarType pScalar)
{
	x *= pScalar;
	y *= pScalar;
}

V2D_TEMPLATE void V2D_QUAL::Div(_TVarType pScalar)
{
	_TVarType lInvScalar = 1.0f / pScalar;
	x *= lInvScalar; 
	y *= lInvScalar; 
}

V2D_TEMPLATE _TVarType V2D_QUAL::Dot(const Vector2D<_TVarType>& pV) const
{
	return (x * pV.x + y * pV.y);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetLength() const
{
	return (_TVarType)sqrt(x * x + y * y);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetLengthSquared() const
{
	return (x * x + y * y);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetDistance(const Vector2D<_TVarType>& pV) const
{
	_TVarType lDx = pV.x - x;
	_TVarType lDy = pV.y - y;

	return (_TVarType)sqrt(lDx * lDx + lDy * lDy);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetDistanceSquared(const Vector2D<_TVarType>& pV) const
{
	_TVarType lDx = pV.x - x;
	_TVarType lDy = pV.y - y;

	return (lDx * lDx + lDy * lDy);
}

V2D_TEMPLATE _TVarType V2D_QUAL::GetAngle() const
{
	if (fabs(x) <= MathTraits<_TVarType>::FullEps())
	{
		if (y >= 0)
			return (_TVarType)(PI/2);
		else
			return (_TVarType)(-PI/2);
	}

	if (x >= 0)
	{
		return atan(y / x);
	}
	else
	{
		if (y >= 0)
			return (_TVarType)((PI / 2) - atan(y / x));
		else
			return (_TVarType)(-(PI / 2) - atan(y / x));
	}
}

V2D_TEMPLATE void V2D_QUAL::Cross(const Vector2D<_TVarType>& pV)
{
	x = -pV.y;
	y = pV.x;
}

V2D_TEMPLATE void V2D_QUAL::CrossUnit(const Vector2D<_TVarType>& pV)
{
	Cross(pV);
	Normalize();
}

V2D_TEMPLATE void V2D_QUAL::RotateAround(const Vector2D<_TVarType>& pOrigin, _TVarType pAngle)
{
	_TVarType lX = x - pOrigin.x;
	_TVarType lY = y - pOrigin.y;
	_TVarType lSinA = (_TVarType)sin(pAngle);
	_TVarType lCosA = (_TVarType)cos(pAngle);

	x = lCosA * lX - lSinA * lY + pOrigin.x;
	y = lSinA * lX + lCosA * lY + pOrigin.y;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator= (const Vector2D<_TVarType>& pV)
{
	x = pV.x;
	y = pV.y;

	return *this;
}

V2D_TEMPLATE bool V2D_QUAL::operator== (const Vector2D<_TVarType>& pV) const
{
	return (x == pV.x && y == pV.y);
}

V2D_TEMPLATE bool V2D_QUAL::operator!= (const Vector2D<_TVarType>& pV) const
{
	return (x != pV.x || y != pV.y);
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator+= (const Vector2D<_TVarType>& pV)
{
	x += pV.x;
	y += pV.y;

	return *this;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator+ (const Vector2D<_TVarType>& pV) const
{
	Vector2D lTemp;

	lTemp.x = x + pV.x;
	lTemp.y = y + pV.y;

	return lTemp;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator-= (const Vector2D<_TVarType>& pV)
{
	x -= pV.x;
	y -= pV.y;

	return *this;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator- (const Vector2D<_TVarType>& pV) const
{
	Vector2D lTemp;

	lTemp.x = x - pV.x;
	lTemp.y = y - pV.y;

	return lTemp;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator- () const
{
	Vector2D lTemp;

	lTemp.x = -x;
	lTemp.y = -y;

	return lTemp;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator*= (_TVarType pScalar)
{
	x *= pScalar;
	y *= pScalar;

	return *this;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator* (_TVarType pScalar) const
{
	Vector2D lTemp;

	lTemp.x = x * pScalar;
	lTemp.y = y * pScalar;

	return lTemp;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator/= (_TVarType pScalar)
{
	_TVarType lInvScalar = 1.0f / pScalar;

	x *= lInvScalar;
	y *= lInvScalar;

	return *this;
}

V2D_TEMPLATE Vector2D<_TVarType> V2D_QUAL::operator/ (_TVarType pScalar) const
{
	_TVarType lInvScalar = 1.0f / pScalar;

	Vector2D lTemp;

	lTemp.x = x * lInvScalar;
	lTemp.y = y * lInvScalar;

	return lTemp;
}

V2D_TEMPLATE _TVarType V2D_QUAL::operator* (const Vector2D<_TVarType>& pV) const
{
	return(x * pV.x + y * pV.y);
}

V2D_TEMPLATE void V2D_QUAL::Normalize(_TVarType pLength)
{
	_TVarType lLength = GetLength();

	if (lLength >= MathTraits<_TVarType>::FullEps())
	{
		Div(lLength);
		Mul(pLength);
	}
	else
	{
		Set(0.0f, 0.0f);
	}
}

V2D_TEMPLATE int V2D_QUAL::GetRawDataSize()
{
	return sizeof(_TVarType) * 2;
}

V2D_TEMPLATE int V2D_QUAL::GetRawData(uint8* pData)
{
	_TVarType* lData = (_TVarType*)pData;
	lData[0] = x;
	lData[1] = y;
}

V2D_TEMPLATE int V2D_QUAL::SetRawData(uint8* pData)
{
	_TVarType* lData = (_TVarType*)pData;
	x = lData[0];
	y = lData[1];
}

V2D_TEMPLATE bool V2D_QUAL::IsNullVector() const
{
	return (fabs(x) <= MathTraits<_TVarType>::FullEps() &&
		fabs(y) <= MathTraits<_TVarType>::FullEps());
}

V2D_TEMPLATE Vector2D<float> V2D_QUAL::ToFloat() const
{
	return Vector2D<float>((float)x, (float)y);
}

V2D_TEMPLATE Vector2D<double> V2D_QUAL::ToDouble() const
{
	return Vector2D<double>((double)x, (double)y);
}

inline float LineDistance(const Vector2D<float>& pPoL, const Vector2D<float>& pDir, const Vector2D<float>& pPoint)
{
	Vector2D<float> lNormDir(pDir);
	lNormDir.Normalize();
	Vector2D<float> lV(pPoint - pPoL);
	return abs(lNormDir.GetPerpCW().Dot(lV));
}

inline double LineDistance(const Vector2D<double>& pPoL, const Vector2D<double>& pDir, const Vector2D<double>& pPoint)
{
	Vector2D<double> lNormDir(pDir);
	lNormDir.Normalize();
	Vector2D<double> lV(pPoint - pPoL);
	return abs(lNormDir.GetPerpCW().Dot(lV));
}

inline float LineDistance(const Vector2D<float>& pPoL, const Vector2D<float>& pDir, const Vector2D<float>& pPoint, Vector2D<float>& pClosestPoint, float& pDirDistance)
{
	Vector2D<float> lNormDir(pDir);
	lNormDir.Normalize();
	Vector2D<float> lV(pPoint - pPoL);
	pDirDistance = lNormDir.Dot(lV);
	Vector2D<float> lD(lNormDir * pDirDistance);
	pClosestPoint = pPoL + lD;
	return abs(lNormDir.GetPerpCW().Dot(lV));
}

inline double LineDistance(const Vector2D<double>& pPoL, const Vector2D<double>& pDir, const Vector2D<double>& pPoint, Vector2D<double>& pClosestPoint, double& pDirDistance)
{
	Vector2D<double> lNormDir(pDir);
	lNormDir.Normalize();
	Vector2D<double> lV(pPoint - pPoL);
	pDirDistance = lNormDir.Dot(lV);
	Vector2D<double> lD(lNormDir * pDirDistance);
	pClosestPoint = pPoL + lD;
	return abs(lNormDir.GetPerpCW().Dot(lV));
}

inline float LineSegmentDistance(const Vector2D<float>& pStart, const Vector2D<float>& pEnd, const Vector2D<float>& pPoint, Vector2D<float>& pClosestPoint, float& pDirDistance)
{
	Vector2D<float> lNormDir(pEnd - pStart);
	float lMaxDistance = lNormDir.GetLength();
	lNormDir /= lMaxDistance; // Normalize.

	Vector2D<float> lV(pPoint - pStart);
	pDirDistance = lNormDir.Dot(lV);

	if(pDirDistance < 0)
		pDirDistance = 0;
	else if(pDirDistance > lMaxDistance)
		pDirDistance = lMaxDistance;

	pClosestPoint = pStart + lNormDir * pDirDistance;
	return pPoint.GetDistance(pClosestPoint);
}

inline double LineSegmentDistance(const Vector2D<double>& pStart, const Vector2D<double>& pEnd, const Vector2D<double>& pPoint, Vector2D<double>& pClosestPoint, double& pDirDistance)
{
	Vector2D<double> lNormDir(pEnd - pStart);
	double lMaxDistance = lNormDir.GetLength();
	lNormDir /= lMaxDistance; // Normalize.

	Vector2D<double> lV(pPoint - pStart);
	pDirDistance = lNormDir.Dot(lV);

	if(pDirDistance < 0)
		pDirDistance = 0;
	else if(pDirDistance > lMaxDistance)
		pDirDistance = lMaxDistance;

	pClosestPoint = pStart + lNormDir * pDirDistance;
	return pPoint.GetDistance(pClosestPoint);
}

#undef V2D_TEMPLATE
#undef V2D_QUAL
