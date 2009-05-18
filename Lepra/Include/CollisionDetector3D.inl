/*
	Class:  CollisionDetector3D
	Author: Alexander Hugestrand
	Copyright (c) 2002-2005, Alexander Hugestrand
*/

template<class _TVarType>
CollisionDetector3D<_TVarType>::CollisionDetector3D()
{
	mOBBCollisionData = new OBBCollisionData<_TVarType>;
}

template<class _TVarType>
CollisionDetector3D<_TVarType>::~CollisionDetector3D()
{
	delete mOBBCollisionData;
}


//////////////////////////////////////////////////////////////////////////////
//
// Point tests...
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsSphereEnclosingPoint(const Sphere<_TVarType>& pSphere,
							    const Vector3D<_TVarType>& pPoint)
{
	_TVarType lDistSquared = pSphere.GetPosition().GetDistanceSquared(pPoint);
	return (lDistSquared < pSphere.GetRadiusSquared());
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBEnclosingPoint(const AABB<_TVarType>& pAABB, 
							  const Vector3D<_TVarType>& pPoint)
{
	Vector3D<_TVarType> lMin(pAABB.GetPosition() - pAABB.GetSize());
	Vector3D<_TVarType> lMax(pAABB.GetPosition() + pAABB.GetSize());

	return (lMin.x <= pPoint.x && lMax.x > pPoint.x &&
		lMin.y <= pPoint.y && lMax.y > pPoint.y &&
		lMin.z <= pPoint.z && lMax.z > pPoint.z);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBEnclosingPoint(const OBB<_TVarType>& pOBB, 
							 const Vector3D<_TVarType>& pPoint)
{
	Vector3D<_TVarType> lPosDiff(pPoint - pOBB.GetPosition());
	const Vector3D<_TVarType>& lOBBSize = pOBB.GetSize();

	_TVarType lD = lPosDiff.Dot(pOBB.GetRotation().GetAxisX());
	if(lD > lOBBSize.x || lD < -lOBBSize.x)
		return false;
	
	lD = lPosDiff.Dot(pOBB.GetRotation().GetAxisY());
	if(lD > lOBBSize.y || lD < -lOBBSize.y)
		return false;

	lD = lPosDiff.Dot(pOBB.GetRotation().GetAxisZ());
	if(lD > lOBBSize.z || lD < -lOBBSize.z)
		return false;

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAACylinderEnclosingPoint(const Cylinder<_TVarType>& pAACylinder,
								const Vector3D<_TVarType>& pPoint)
{
	Vector3D<_TVarType> lPosDiff(pPoint - pAACylinder.GetPosition());

	switch(pAACylinder.GetAlignment())
	{
		case Cylinder<_TVarType>::ALIGN_X:
		{
			_TVarType lDistSquared = lPosDiff.y * lPosDiff.y + lPosDiff.z * lPosDiff.z;
			if(lDistSquared < pAACylinder.GetRadiusSquared() &&
			   lPosDiff.x > -pAACylinder.GetLength() && lPosDiff.x < pAACylinder.GetLength())
			{
				return true;
			}
		}
		case Cylinder<_TVarType>::ALIGN_Y:
		{
			_TVarType lDistSquared = lPosDiff.x * lPosDiff.x + lPosDiff.z * lPosDiff.z;
			if(lDistSquared < pAACylinder.GetRadiusSquared() &&
			   lPosDiff.y > -pAACylinder.GetLength() && lPosDiff.y < pAACylinder.GetLength())
			{
				return true;
			}
		}
		case Cylinder<_TVarType>::ALIGN_Z:
		{
			_TVarType lDistSquared = lPosDiff.x * lPosDiff.x + lPosDiff.y * lPosDiff.y;
			if(lDistSquared < pAACylinder.GetRadiusSquared() &&
			   lPosDiff.z > -pAACylinder.GetLength() && lPosDiff.z < pAACylinder.GetLength())
			{
				return true;
			}
		}
	}
	return false;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsCylinderEnclosingPoint(const Cylinder<_TVarType>& pCylinder,
							      const Vector3D<_TVarType>& pPoint)
{
	Vector3D<_TVarType> lRelativePos(pCylinder.GetRotation().GetInverseRotatedVector(pPoint - pCylinder.GetPosition()));
	
	switch(pCylinder.GetAlignment())
	{
		case Cylinder<_TVarType>::ALIGN_X:
		{
			_TVarType lDistSquared = lRelativePos.y * lRelativePos.y + lRelativePos.z * lRelativePos.z;
			if(lDistSquared < pCylinder.GetRadiusSquared() &&
			   lRelativePos.x > -pCylinder.GetLength() && lRelativePos.x < pCylinder.GetLength())
			{
				return true;
			}
		}
		case Cylinder<_TVarType>::ALIGN_Y:
		{
			_TVarType lDistSquared = lRelativePos.x * lRelativePos.x + lRelativePos.z * lRelativePos.z;
			if(lDistSquared < pCylinder.GetRadiusSquared() &&
			   lRelativePos.y > -pCylinder.GetLength() && lRelativePos.y < pCylinder.GetLength())
			{
				return true;
			}
		}
		case Cylinder<_TVarType>::ALIGN_Z:
		{
			_TVarType lDistSquared = lRelativePos.x * lRelativePos.x + lRelativePos.y * lRelativePos.y;
			if(lDistSquared < pCylinder.GetRadiusSquared() &&
			   lRelativePos.z > -pCylinder.GetLength() && lRelativePos.z < pCylinder.GetLength())
			{
				return true;
			}
		}
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////////
//
// Enclosing tests.
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsSphere1EnclosingSphere2(const Sphere<_TVarType>& pSphere1,
							       const Sphere<_TVarType>& pSphere2)
{
	// Can't optimize using the squared distance here. The reason is the expression
	// in the if-statement:
	//
	// dist + radius2 <= radius1
	//
	// Squared, it becomes:
	//
	// dist^2 + 2*dist*radius2 + radius2^2 <= radius1^2
	//
	// ...which still requires that we know the actual distance.

	_TVarType lDistance = pSphere1.GetPosition().GetDistance(pSphere2.GetPosition());
	return ((lDist + pSphere2.GetRadius()) <= pSphere1.GetRadius());
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABB1EnclosingAABB2(const AABB<_TVarType>& pAABB1,
							   const AABB<_TVarType>& pAABB2)
{
	Vector3D<_TVarType> lMin1(pAABB1.GetPosition() - pAABB1.GetSize());
	Vector3D<_TVarType> lMax1(pAABB1.GetPosition() + pAABB1.GetSize());
	Vector3D<_TVarType> lMin2(pAABB2.GetPosition() - pAABB2.GetSize());
	Vector3D<_TVarType> lMax2(pAABB2.GetPosition() - pAABB2.GetSize());

	return(lMax1.x >= lMax2.x && lMin1.x <= lMin2.x && 
	       lMax1.y >= lMax2.y && lMin1.y <= lMin2.y && 
	       lMax1.z >= lMax2.z && lMin1.z <= lMin2.z);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBB1EnclosingOBB2(const OBB<_TVarType>& pOBB1,
							 const OBB<_TVarType>& pOBB2)
{
	// OBB2 is only enclosed if all it's vertices are enclosed.
	Vector3D<_TVarType> lXExt(pOBB2.GetRotation().GetAxisX() * pOBB2.GetSize().x);
	Vector3D<_TVarType> lYExt(pOBB2.GetRotation().GetAxisY() * pOBB2.GetSize().y);
	Vector3D<_TVarType> lZExt(pOBB2.GetRotation().GetAxisZ() * pOBB2.GetSize().z);
	Vector3D<_TVarType> lV(pOBB2.GetPosition() - lXExt - lYExt - lZExt);

	if(IsOBBEnclosingPoint(pOBB1, lV) == false)
		return false;

	lXExt *= 2;
	lV += lXExt;
	if(IsOBBEnclosingPoint(pOBB1, lV) == false)
		return false;

	lYExt *= 2;
	lV += lYExt;
	if(IsOBBEnclosingPoint(pOBB1, lV) == false)
		return false;

	lZExt *= 2;
	lV += lZExt;
	if(IsOBBEnclosingPoint(pOBB1, lV) == false)
		return false;

	lV -= lYExt;
	if(IsOBBEnclosingPoint(pOBB1, lV) == false)
		return false;

	lV -= lXExt;
	if(IsOBBEnclosingPoint(pOBB1, lV) == false)
		return false;

	lV += lYExt;
	if(IsOBBEnclosingPoint(pOBB1, lV) == false)
		return false;

	lV -= lZExt;
	if(IsOBBEnclosingPoint(pOBB1, lV) == false)
		return false;

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAACylinder1EnclosingAACylinder2(const Cylinder<_TVarType>& pCylinder1,
									const Cylinder<_TVarType>& pCylinder2)
{
	assert(pCylinder1.GetAlignment() == pCylinder2.GetAlignment());

	// It's impossible for a smaller cylinder to enclose a larger one.
	if(pCylinder1.GetRadius() < pCylinder2.GetRadius())
		return false;

	Vector3D<_TVarType> lD(pCylinder2.GetPosition() - pCylinder1.GetPosition());
	_TVarType lCoordToTest;

	switch(pCylinder1.GetAlignment())
	{
		case Cylinder<_TVarType>::ALIGN_X: lCoordToTest = lD.x; break;
		case Cylinder<_TVarType>::ALIGN_Y: lCoordToTest = lD.y; break;
		case Cylinder<_TVarType>::ALIGN_Z: lCoordToTest = lD.z; break;
	}

	if((lCoordToTest + pCylinder2.GetLength()) > pCylinder1.GetLength())
		return false;
	if((lCoordToTest - pCylinder2.GetLength()) < -pCylinder1.GetLength())
		return false;

	_TVarType lDistSquared;
	switch(pCylinder1.GetAlignment())
	{
		case Cylinder<_TVarType>::ALIGN_X: lDistSquared = lD.y*lD.y + lD.z*lD.z; break;
		case Cylinder<_TVarType>::ALIGN_Y: lDistSquared = lD.x*lD.x + lD.z*lD.z; break;
		case Cylinder<_TVarType>::ALIGN_Z: lDistSquared = lD.x*lD.x + lD.y*lD.y; break;
	}

	if((lDistSquared + pCylinder2.GetRadiusSquared()) > pCylinder1.GetRadiusSquared())
		return false;

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsSphereEnclosingAABB(const Sphere<_TVarType>& pSphere,
							   const AABB<_TVarType>& pAABB)
{
	// This test is similar to the enclosing sphere to sphere test.
	_TVarType lAABBRadius = pAABB.GetSize().GetLength();
	_TVarType lDist = pSphere.GetPosition().GetDistance(pAABB.GetPosition());
	return ((lDist + lAABBRadius) <= pSphere.GetRadius());
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBEnclosingSphere(const AABB<_TVarType>& pAABB,
							   const Sphere<_TVarType>& pSphere)
{
	Vector3D<_TVarType> lMin(pAABB.GetPosition() - pAABB.GetSize());
	Vector3D<_TVarType> lMax(pAABB.GetPosition() + pAABB.GetSize());
	Vector3D<_TVarType> lR(pSphere.GetRadius(), pSphere.GetRadius(), pSphere.GetRadius());
	Vector3D<_TVarType> lSMin(pSphere.GetPosition() - lR);
	Vector3D<_TVarType> lSMax(pSphere.GetPosition() + lR);

	return (lMin.x < lSMin.x && lMax.x > lSMax.x &&
	        lMin.y < lSMin.y && lMax.y > lSMax.y &&
	        lMin.z < lSMin.z && lMax.z > lSMax.z);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBEnclosingSphere(const OBB<_TVarType>& pOBB,
							  const Sphere<_TVarType>& pSphere)
{
	Vector3D<_TVarType> lRelativePos(pOBB.GetRotation().GetInverseRotatedVector(pSphere.GetPosition() - pOBB.GetPosition()));
	Vector3D<_TVarType> lR(pSphere.GetRadius(), pSphere.GetRadius(), pSphere.GetRadius());
	Vector3D<_TVarType> lSMin(lRelativePos - lR);
	Vector3D<_TVarType> lSMax(lRelativePos + lR);

	return (-pOBB.GetSize().x < lSMin.x && pOBB.GetSize().x > lSMax.x &&
	        -pOBB.GetSize().y < lSMin.y && pOBB.GetSize().y > lSMax.y &&
	        -pOBB.GetSize().z < lSMin.z && pOBB.GetSize().z > lSMax.z);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBEnclosingOBB(const AABB<_TVarType>& pAABB,
							const OBB<_TVarType>& pOBB)
{
	Vector3D<_TVarType> lMin(pAABB.GetPosition() - pAABB.GetSize());
	Vector3D<_TVarType> lMax(pAABB.GetPosition() + pAABB.GetSize());

	Vector3D<_TVarType> lXExt(pOBB.GetRotation().GetAxisX() * pOBB.GetSize().x);
	Vector3D<_TVarType> lYExt(pOBB.GetRotation().GetAxisY() * pOBB.GetSize().y);
	Vector3D<_TVarType> lZExt(pOBB.GetRotation().GetAxisZ() * pOBB.GetSize().z);
	Vector3D<_TVarType> lV(pOBB.GetPosition() - lXExt - lYExt - lZExt);

	if(lV.x < lMin.x || lV.x > lMax.x ||
	   lV.y < lMin.y || lV.y > lMax.y ||
	   lV.z < lMin.z || lV.z > lMax.z)
	{
		return false;
	}

	lXExt *= 2;
	lV += lXExt;
	if(lV.x < lMin.x || lV.x > lMax.x ||
	   lV.y < lMin.y || lV.y > lMax.y ||
	   lV.z < lMin.z || lV.z > lMax.z)
	{
		return false;
	}

	lYExt *= 2;
	lV += lYExt;
	if(lV.x < lMin.x || lV.x > lMax.x ||
	   lV.y < lMin.y || lV.y > lMax.y ||
	   lV.z < lMin.z || lV.z > lMax.z)
	{
		return false;
	}

	lZExt *= 2;
	lV += lZExt;
	if(lV.x < lMin.x || lV.x > lMax.x ||
	   lV.y < lMin.y || lV.y > lMax.y ||
	   lV.z < lMin.z || lV.z > lMax.z)
	{
		return false;
	}

	lV -= lYExt;
	if(lV.x < lMin.x || lV.x > lMax.x ||
	   lV.y < lMin.y || lV.y > lMax.y ||
	   lV.z < lMin.z || lV.z > lMax.z)
	{
		return false;
	}

	lV -= lXExt;
	if(lV.x < lMin.x || lV.x > lMax.x ||
	   lV.y < lMin.y || lV.y > lMax.y ||
	   lV.z < lMin.z || lV.z > lMax.z)
	{
		return false;
	}

	lV += lYExt;
	if(lV.x < lMin.x || lV.x > lMax.x ||
	   lV.y < lMin.y || lV.y > lMax.y ||
	   lV.z < lMin.z || lV.z > lMax.z)
	{
		return false;
	}

	lV -= lZExt;
	if(lV.x < lMin.x || lV.x > lMax.x ||
	   lV.y < lMin.y || lV.y > lMax.y ||
	   lV.z < lMin.z || lV.z > lMax.z)
	{
		return false;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBEnclosingAABB(const OBB<_TVarType>& pOBB,
							const AABB<_TVarType>& pAABB)
{
	// Invert the situation and test the opposite (if AABB is enclosing OBB).
	// This way if more optimal with 9 muls in this function (vector rotation), 
	// and 9 more in IsAABBEnclosingOBB(), than testing each vertex of the AABB 
	// against the OBB, which would require 72 muls (9 per vertex).
	Vector3D<_TVarType> lRotatedPos(pOBB.GetRotation().GetInverseRotatedVector(pAABB.GetPosition() - pOBB.GetPosition()) + pOBB.GetPosition());
	AABB<_TVarType> lAABB(pOBB.GetPosition(), pOBB.GetSize());
	OBB<_TVarType> lOBB(lRotatedPos, pOBB.GetSize(), pOBB.GetRotation().GetInverse());
	return IsAABBEnclosingOBB(lAABB, lOBB);
}



//////////////////////////////////////////////////////////////////////////////
//
// Public overlapping test functions.
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBOverlappingAABB(const AABB<_TVarType>& pAABB1,
							   const AABB<_TVarType>& pAABB2,
							   CollisionInfo* pCollisionInfo)
{
	Vector3D<_TVarType> lMinDist(pAABB1.GetSize() + pAABB2.GetSize());
	Vector3D<_TVarType> lDist(pAABB1.GetPosition() - pAABB2.GetPosition());

	if(lDist.x < 0.0f)
		lDist.x = -lDist.x;
	if(lDist.y < 0.0f)
		lDist.y = -lDist.y;
	if(lDist.z < 0.0f)
		lDist.z = -lDist.z;

	if(lDist.x > lMinDist.x || lDist.y > lMinDist.y || lDist.z > lMinDist.z)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		Vector3D<_TVarType> lBox1Min(pAABB1.GetPosition() - pAABB1.GetSize());
		Vector3D<_TVarType> lBox1Max(pAABB1.GetPosition() + pAABB1.GetSize());
		Vector3D<_TVarType> lBox2Min(pAABB2.GetPosition() - pAABB2.GetSize());
		Vector3D<_TVarType> lBox2Max(pAABB2.GetPosition() + pAABB2.GetSize());

		Vector3D<_TVarType> lOverlapMin(max(lBox1Min.x, lBox2Min.x), max(lBox1Min.y, lBox2Min.y), max(lBox1Min.z, lBox2Min.z));
		Vector3D<_TVarType> lOverlapMax(min(lBox1Max.x, lBox2Max.x), min(lBox1Max.y, lBox2Max.y), min(lBox1Max.z, lBox2Max.z));

		pCollisionInfo->mTimeToCollision = 0;
		pCollisionInfo->mPointOfCollision = (lOverlapMin + lOverlapMax) * (_TVarType)0.5;

		//Always positive.
		Vector3D<_TVarType> lDiff(lMinDist - lDist);

		if(lDiff.x < lDiff.y)
		{
			if(lDiff.x < lDiff.z)
			{
				if(pAABB1.GetPosition().x > pAABB2.GetPosition().x)
					pCollisionInfo->mNormal.Set(1, 0, 0);
				else
					pCollisionInfo->mNormal.Set(-1, 0, 0);
				pCollisionInfo->mSeparationDistance = lDiff.x;
			}
			else
			{
				if(pAABB1.GetPosition().z > pAABB2.GetPosition().z)
					pCollisionInfo->mNormal.Set(0, 0, 1);
				else
					pCollisionInfo->mNormal.Set(0, 0, -1);
				pCollisionInfo->mSeparationDistance = lDiff.z;
			}
		}
		else
		{
			if(lDiff.y < lDiff.z)
			{
				if(pAABB1.GetPosition().y > pAABB2.GetPosition().y)
					pCollisionInfo->mNormal.Set(0, 1, 0);
				else
					pCollisionInfo->mNormal.Set(0, -1, 0);
				pCollisionInfo->mSeparationDistance = lDiff.y;
			}
			else
			{
				if(pAABB1.GetPosition().z > pAABB2.GetPosition().z)
					pCollisionInfo->mNormal.Set(0, 0, 1);
				else
					pCollisionInfo->mNormal.Set(0, 0, -1);
				pCollisionInfo->mSeparationDistance = lDiff.z;
			}
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBOverlappingOBB(const OBB<_TVarType>& pOBB1,
							 const OBB<_TVarType>& pOBB2,
							 CollisionInfo* pCollisionInfo)
{
	if(pCollisionInfo)
		return IsOBBOverlappingOBB(pOBB1, pOBB2, *pCollisionInfo);
	else
		return IsOBBOverlappingOBBOptimized(pOBB1, pOBB2);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsSphereOverlappingSphere(const Sphere<_TVarType>& pSphere1,
							       const Sphere<_TVarType>& pSphere2,
							       CollisionInfo* pCollisionInfo)
{
	Vector3D<_TVarType> lDist(pSphere1.GetPosition() - pSphere2.GetPosition());
	_TVarType lMinDistance = (pSphere1.GetRadius() + pSphere2.GetRadius());

	if(lDist.GetLengthSquared() > (lMinDistance * lMinDistance))
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		_TVarType lLength = lDist.GetLength();
		pCollisionInfo->mNormal = lDist;
		pCollisionInfo->mNormal /= lLength; // Normalize
		pCollisionInfo->mSeparationDistance = (_TVarType)(lMinDistance - lLength);
		pCollisionInfo->mPointOfCollision = pSphere2.GetPosition() + pCollisionInfo->mNormal * (pSphere2.GetRadius() - pCollisionInfo->mSeparationDistance * (_TVarType)0.5);
		pCollisionInfo->mTimeToCollision = 0;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBOverlappingOBB(const AABB<_TVarType>& pAABB,
							  const OBB<_TVarType>& pOBB,
							  CollisionInfo* pCollisionInfo)
{
	OBB lOBB(pAABB.GetPosition(), pAABB.GetSize(), RotationMatrix<_TVarType>());
	if(pCollisionInfo)
		return IsOBBOverlappingOBB(lOBB, pOBB, *pCollisionInfo);
	else
		return IsOBBOverlappingOBB(lOBB, pOBB);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBOverlappingSphere(const AABB<_TVarType>& pAABB,
							     const Sphere<_TVarType>& pSphere,
							     CollisionInfo* pCollisionInfo)
{
	Vector3D<_TVarType> lMin(pAABB.GetPosition() - pAABB.GetSize());
	Vector3D<_TVarType> lMax(pAABB.GetPosition() + pAABB.GetSize());
	Vector3D<_TVarType> lPointOnBox(0, 0, 0);

	if(pSphere.GetPosition().x < lMin.x)
		lPointOnBox.x = lMin.x;
	else if(pSphere.GetPosition().x > lMax.x)
		lPointOnBox.x = lMax.x;
	else
		lPointOnBox.x = pSphere.GetPosition().x;

	if(pSphere.GetPosition().y < lMin.y)
		lPointOnBox.y = lMin.y;
	else if(pSphere.GetPosition().y > lMax.y)
		lPointOnBox.y = lMax.y;
	else
		lPointOnBox.y = pSphere.GetPosition().y;

	if(pSphere.GetPosition().z < lMin.z)
		lPointOnBox.z = lMin.z;
	else if(pSphere.GetPosition().z > lMax.z)
		lPointOnBox.z = lMax.z;
	else
		lPointOnBox.z = pSphere.GetPosition().z;

	Vector3D<_TVarType> lDist(lPointOnBox - pSphere.GetPosition());
	_TVarType lDistanceSquared = lDist.GetLengthSquared();

	if(lDistanceSquared > pSphere.GetRadiusSquared())
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mTimeToCollision = 0;
		pCollisionInfo->mNormal = lDist;
		_TVarType lLength = lDist.GetLength();
		pCollisionInfo->mNormal /= lLength; // Normalize.
		pCollisionInfo->mSeparationDistance = pSphere.GetRadius() - lLength;
		pCollisionInfo->mPointOfCollision = pSphere.GetPosition() + pCollisionInfo->mNormal * ((pSphere.GetRadius() + lLength) * (_TVarType)0.5);
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBOverlappingSphere(const OBB<_TVarType>& pOBB,
							    const Sphere<_TVarType>& pSphere,
							    CollisionInfo* pCollisionInfo)
{
	Vector3D<_TVarType> lPosDiff(pSphere.GetPosition() - pOBB.GetPosition());
	Vector3D<_TVarType> lRotPos(pOBB.GetRotation().GetInverseRotatedVector(lPosDiff) + pOBB.GetPosition());

	AABB lAABB(pOBB.GetPosition(), pOBB.GetSize());
	Sphere lSphere(lRotPos, pSphere.GetRadius());

	if(pCollisionInfo)
	{
		bool lOverlapping = IsAABBOverlappingSphere(lAABB, pSphere, *pCollisionInfo);
		if(lOverlapping)
		{
			pCollisionInfo->mPointOfCollision = pOBB.GetRotation().GetRotatedVector(pCollisionInfo->mPointOfCollision - pOBB.GetPosition()) + pOBB.GetPosition();
			pCollisionInfo->mNormal = pOBB.GetRotation().GetRotatedVector(pCollisionInfo->mNormal);
		}
		return lOverlapping;
	}
	else
	{
		return IsAABBOverlappingSphere(lAABB, pSphere);
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Optimized overlapping test function.
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBOverlappingOBBOptimized(const OBB<_TVarType>& pOBB1,
								  const OBB<_TVarType>& pOBB2)
{
	_TVarType lEpsilon = Lepra::MathTraits<_TVarType>::Eps();
	Vector3D<_TVarType> lDistance(pOBB2.GetPosition() - pOBB1.GetPosition());

	Vector3D<_TVarType> lXAxis1(pOBB1.GetRotation().GetAxisX());
	Vector3D<_TVarType> lYAxis1(pOBB1.GetRotation().GetAxisY());
	Vector3D<_TVarType> lZAxis1(pOBB1.GetRotation().GetAxisZ());

	Vector3D<_TVarType> lXAxis2(pOBB2.GetRotation().GetAxisX());
	Vector3D<_TVarType> lYAxis2(pOBB2.GetRotation().GetAxisY());
	Vector3D<_TVarType> lZAxis2(pOBB2.GetRotation().GetAxisZ());

	_TVarType lAxisDot[9];
	_TVarType lAbsAxisDot[9];
	_TVarType lDistDot[3];
	_TVarType lProjectedDistance;
	_TVarType lProjectedSize1;
	_TVarType lProjectedSize2;
	_TVarType lProjectedSizeSum;

	// Box1's X-Axis.
	lAxisDot[0] = lXAxis1.Dot(lXAxis2);
	lAxisDot[1] = lXAxis1.Dot(lYAxis2);
	lAxisDot[2] = lXAxis1.Dot(lZAxis2);
	lDistDot[0] = lXAxis1.Dot(lDistance);
	lAbsAxisDot[0] = abs(lAxisDot[0]);
	lAbsAxisDot[1] = abs(lAxisDot[1]);
	lAbsAxisDot[2] = abs(lAxisDot[2]);
	lProjectedDistance = abs(lDistDot[0]);
	lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[0] +
	                  pOBB2.GetSize().y * lAbsAxisDot[1] + 
	                  pOBB2.GetSize().z * lAbsAxisDot[2];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().x;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}

	// Box1's Y-Axis.
	lAxisDot[3] = lYAxis1.Dot(lXAxis2);
	lAxisDot[4] = lYAxis1.Dot(lYAxis2);
	lAxisDot[5] = lYAxis1.Dot(lZAxis2);
	lDistDot[1] = lYAxis1.Dot(lDistance);
	lAbsAxisDot[3] = abs(lAxisDot[3]);
	lAbsAxisDot[4] = abs(lAxisDot[4]);
	lAbsAxisDot[5] = abs(lAxisDot[5]);
	lProjectedDistance = abs(lDistDot[1]);
	lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[3] +
	                  pOBB2.GetSize().y * lAbsAxisDot[4] + 
	                  pOBB2.GetSize().z * lAbsAxisDot[5];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().y;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}

	// Box1's Z-Axis.
	lAxisDot[6] = lZAxis1.Dot(lXAxis2);
	lAxisDot[7] = lZAxis1.Dot(lYAxis2);
	lAxisDot[8] = lZAxis1.Dot(lZAxis2);
	lDistDot[2] = lZAxis1.Dot(lDistance);
	lAbsAxisDot[6] = abs(lAxisDot[6]);
	lAbsAxisDot[7] = abs(lAxisDot[7]);
	lAbsAxisDot[8] = abs(lAxisDot[8]);
	lProjectedDistance = abs(lDistDot[2]);
	lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[6] +
	                  pOBB2.GetSize().y * lAbsAxisDot[7] + 
	                  pOBB2.GetSize().z * lAbsAxisDot[8];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().z;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}

	// Box2's X-Axis
	lProjectedDistance = abs(lXAxis2.Dot(lDistance));
	lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[0] +
	                  pOBB1.GetSize().y * lAbsAxisDot[3] +
	                  pOBB1.GetSize().z * lAbsAxisDot[6];
	lProjectedSizeSum = lProjectedSize1 + pOBB2.GetSize().x;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}

	// Box2's Y-Axis
	lProjectedDistance = abs(lYAxis2.Dot(lDistance));
	lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[1] +
	                  pOBB1.GetSize().y * lAbsAxisDot[4] +
	                  pOBB1.GetSize().z * lAbsAxisDot[7];
	lProjectedSizeSum = lProjectedSize1 + pOBB2.GetSize().y;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}

	// Box2's Z-Axis
	lProjectedDistance = abs(lZAxis2.Dot(lDistance));
	lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[2] +
						pOBB1.GetSize().y * lAbsAxisDot[5] +
						pOBB1.GetSize().z * lAbsAxisDot[8];
	lProjectedSizeSum = lProjectedSize1 + pOBB2.GetSize().z;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}
	// A little check to avoid bugs when the boxes are rotated exactly 90 degrees to each other,
	// making them parallel, and the cross products = (0, 0, 0).
	if(lAbsAxisDot[1] > lEpsilon ||
	   lAbsAxisDot[2] > lEpsilon ||
	   lAbsAxisDot[3] > lEpsilon ||
	   lAbsAxisDot[6] > lEpsilon)
	{
		//
		// This is the first cross product to test as a separating plane.
		// It is quite optimized (as you can see, the crossproduct is never calculated). 
		// I'm going to try to explain the math behind this optimization...
		// ** is the dot product, x is the cross product, and * is a regular scalar multiplication.
		//
		// A = OBB1
		// B = OBB2
		// AX is OBB1's rotation axis x etc.
		// lPS = lProjectedSize
		//
		// If C = AX x BX then:
		//
		// lPS = pOBB1.GetSize().x * AX ** C +           1)
		//		   pOBB1.GetSize().y * AY ** C + 
		//		   pOBB1.GetSize().z * AZ ** C;
		//
		// ...which is the "real" way to project OBB1's extents on the axis C.
		//
		// The first product (pOBB1.GetSize().x * AX ** C) is always 0.0 though, 
		// because C is perpendicular to AX (and the dot product between two perpendicular vectors is 0.0), 
		// which leaves us with:
		//
		// lPS = pOBB1.GetSize().y * AY ** C +              2)
		//		   pOBB1.GetSize().z * AZ ** C;
		//
		// Substitute C with AX x BX in 2):
		//
		// lPS = pOBB1.GetSize().y * AY ** (AX x BX) +      3)
		//		   pOBB1.GetSize().z * AZ ** (AX x BX);
		//
		// Applying the rule:
		// U ** (V x W) = (U x V) ** W
		//
		// on 3) gives
		//
		// lPS = pOBB1.GetSize().y * (AY x AX) ** BX +      4)
		//		   pOBB1.GetSize().z * (AZ x AX) ** BX;
		//
		// We know that:
		//
		// AZ = AY x AX
		// AY = AZ x AX
		//
		// ...and we finally have:

		// lPS = pOBB1.GetSize().y * AZ ** BX +			  5)
		//		   pOBB1.GetSize().z * AY ** BX;
		//

		// XAxis1 cross XAxis2
		lProjectedDistance = abs(lDistDot[2] * lAxisDot[3] - lDistDot[1] * lAxisDot[6]);
		lProjectedSize1 = pOBB1.GetSize().y * lAbsAxisDot[6] + pOBB1.GetSize().z * lAbsAxisDot[3];
		lProjectedSize2 = pOBB2.GetSize().y * lAbsAxisDot[2] + pOBB2.GetSize().z * lAbsAxisDot[1];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}

	if(lAbsAxisDot[0] > lEpsilon ||
	   lAbsAxisDot[2] > lEpsilon ||
	   lAbsAxisDot[4] > lEpsilon ||
	   lAbsAxisDot[7] > lEpsilon)
	{
		// XAxis1 cross YAxis2
		lProjectedDistance = abs(lDistDot[2] * lAxisDot[4] - lDistDot[1] * lAxisDot[7]);
		lProjectedSize1 = pOBB1.GetSize().y * lAbsAxisDot[7] + pOBB1.GetSize().z * lAbsAxisDot[4];
		lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[2] + pOBB2.GetSize().z * lAbsAxisDot[0];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}

	if(lAbsAxisDot[0] > lEpsilon ||
	   lAbsAxisDot[1] > lEpsilon ||
	   lAbsAxisDot[5] > lEpsilon ||
	   lAbsAxisDot[8] > lEpsilon)
	{
		// XAxis1 cross ZAxis2
		lProjectedDistance = abs(lDistDot[2] * lAxisDot[5] - lDistDot[1] * lAxisDot[8]);
		lProjectedSize1 = pOBB1.GetSize().y * lAbsAxisDot[8] + pOBB1.GetSize().z * lAbsAxisDot[5];
		lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[1] + pOBB2.GetSize().y * lAbsAxisDot[0];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}

	if(lAbsAxisDot[0] > lEpsilon ||
	   lAbsAxisDot[4] > lEpsilon ||
	   lAbsAxisDot[5] > lEpsilon ||
	   lAbsAxisDot[6] > lEpsilon)
	{
		// YAxis1 cross XAxis2
		lProjectedDistance = abs(lDistDot[0] * lAxisDot[6] - lDistDot[2] * lAxisDot[0]);
		lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[6] + pOBB1.GetSize().z * lAbsAxisDot[0];
		lProjectedSize2 = pOBB2.GetSize().y * lAbsAxisDot[5] + pOBB2.GetSize().z * lAbsAxisDot[4];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}

	if(lAbsAxisDot[1] > lEpsilon ||
	   lAbsAxisDot[3] > lEpsilon ||
	   lAbsAxisDot[5] > lEpsilon ||
	   lAbsAxisDot[7] > lEpsilon)
	{
		// YAxis1 cross YAxis2
		lProjectedDistance = abs(lDistDot[0] * lAxisDot[7] - lDistDot[2] * lAxisDot[1]);
		lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[7] + pOBB1.GetSize().z * lAbsAxisDot[1];
		lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[5] + pOBB2.GetSize().z * lAbsAxisDot[3];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}

	if(lAbsAxisDot[2] > lEpsilon ||
	   lAbsAxisDot[3] > lEpsilon ||
	   lAbsAxisDot[4] > lEpsilon ||
	   lAbsAxisDot[8] > lEpsilon)
	{
		// YAxis1 cross ZAxis2
		lProjectedDistance = abs(lDistDot[0] * lAxisDot[8] - lDistDot[2] * lAxisDot[2]);
		lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[8] + pOBB1.GetSize().z * lAbsAxisDot[2];
		lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[4] + pOBB2.GetSize().y * lAbsAxisDot[3];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}

	if(lAbsAxisDot[0] > lEpsilon ||
	   lAbsAxisDot[3] > lEpsilon ||
	   lAbsAxisDot[7] > lEpsilon ||
	   lAbsAxisDot[8] > lEpsilon)
	{
		// ZAxis1 cross XAxis2
		lProjectedDistance = abs(lDistDot[1] * lAxisDot[0] - lDistDot[0] * lAxisDot[3]);
		lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[3] + pOBB1.GetSize().y * lAbsAxisDot[0];
		lProjectedSize2 = pOBB2.GetSize().y * lAbsAxisDot[8] + pOBB2.GetSize().z * lAbsAxisDot[7];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}

	if(lAbsAxisDot[1] > lEpsilon ||
	   lAbsAxisDot[4] > lEpsilon ||
	   lAbsAxisDot[6] > lEpsilon ||
	   lAbsAxisDot[8] > lEpsilon)
	{
		// ZAxis1 cross YAxis2
		lProjectedDistance = abs(lDistDot[1] * lAxisDot[1] - lDistDot[0] * lAxisDot[4]);
		lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[4] + pOBB1.GetSize().y * lAbsAxisDot[1];
		lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[8] + pOBB2.GetSize().z * lAbsAxisDot[6];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}

	if(lAbsAxisDot[2] > lEpsilon ||
	   lAbsAxisDot[5] > lEpsilon ||
	   lAbsAxisDot[6] > lEpsilon ||
	   lAbsAxisDot[7] > lEpsilon)
	{
		// ZAxis1 cross ZAxis2
		lProjectedDistance = abs(lDistDot[1] * lAxisDot[2] - lDistDot[0] * lAxisDot[5]);
		lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[5] + pOBB1.GetSize().y * lAbsAxisDot[2];
		lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[7] + pOBB2.GetSize().y * lAbsAxisDot[6];
		lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
		if(lProjectedDistance > lProjectedSizeSum)
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// Unoptimized overlapping test function.
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBOverlappingOBB(const OBB<_TVarType>& pOBB1,
							 const OBB<_TVarType>& pOBB2,
							 CollisionInfo& pCollisionInfo)
{
	// Almost the same as the IsOBBOverlappingOBB() function...
	pCollisionInfo.mTimeToCollision = 1;

	_TVarType lEps = Lepra::MathTraits<_TVarType>::Eps();

	Vector3D<_TVarType> lDistance(pOBB2.GetPosition() - pOBB1.GetPosition());

	Vector3D<_TVarType> lAxis1[3];
	Vector3D<_TVarType> lAxis2[3];
	
	lAxis1[0] = pOBB1.GetRotation().GetAxisX();
	lAxis1[1] = pOBB1.GetRotation().GetAxisY();
	lAxis1[2] = pOBB1.GetRotation().GetAxisZ();
	lAxis2[0] = pOBB2.GetRotation().GetAxisX();
	lAxis2[1] = pOBB2.GetRotation().GetAxisY();
	lAxis2[2] = pOBB2.GetRotation().GetAxisZ();

	_TVarType lAxisDot[9];
	_TVarType lAbsAxisDot[9];
	_TVarType lDistDot[3];
	_TVarType lProjectedDistance;
	_TVarType lProjectedSize1;
	_TVarType lProjectedSize2;
	_TVarType lProjectedSizeSum;

	AxisID lMinOverlapAxis = AXIS_NONE;
	_TVarType lMinOverlapDist = 0.0f;
	Vector3D<_TVarType> lMinOverlapVector;

	// Box1's X-Axis.
	lAxisDot[0] = lAxis1[0].Dot(lAxis2[0]);
	lAxisDot[1] = lAxis1[0].Dot(lAxis2[1]);
	lAxisDot[2] = lAxis1[0].Dot(lAxis2[2]);
	lDistDot[0] = lAxis1[0].Dot(lDistance);
	lAbsAxisDot[0] = abs(lAxisDot[0]);
	lAbsAxisDot[1] = abs(lAxisDot[1]);
	lAbsAxisDot[2] = abs(lAxisDot[2]);
	lProjectedDistance = abs(lDistDot[0]);
	lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[0] +
	                  pOBB2.GetSize().y * lAbsAxisDot[1] + 
	                  pOBB2.GetSize().z * lAbsAxisDot[2];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().x;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}
	else //if(lMinOverlapAxis == AXIS_NONE || lMinOverlapDist > (lProjectedSizeSum - lProjectedDistance))
	{
		lMinOverlapAxis = AXIS_X1;
		lMinOverlapDist = lProjectedSizeSum - lProjectedDistance;
		lMinOverlapVector = lAxis1[0];
	}

	// Box1's Y-Axis.
	lAxisDot[3] = lAxis1[1].Dot(lAxis2[0]);
	lAxisDot[4] = lAxis1[1].Dot(lAxis2[1]);
	lAxisDot[5] = lAxis1[1].Dot(lAxis2[2]);
	lDistDot[1] = lAxis1[1].Dot(lDistance);
	lAbsAxisDot[3] = abs(lAxisDot[3]);
	lAbsAxisDot[4] = abs(lAxisDot[4]);
	lAbsAxisDot[5] = abs(lAxisDot[5]);
	lProjectedDistance = abs(lDistDot[1]);
	lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[3] +
	                  pOBB2.GetSize().y * lAbsAxisDot[4] + 
	                  pOBB2.GetSize().z * lAbsAxisDot[5];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().y;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}
	else if(lMinOverlapAxis == AXIS_NONE || lMinOverlapDist > (lProjectedSizeSum - lProjectedDistance))
	{
		lMinOverlapAxis = AXIS_Y1;
		lMinOverlapDist = lProjectedSizeSum - lProjectedDistance;
		lMinOverlapVector = lAxis1[1];
	}

	// Box1's Z-Axis.
	lAxisDot[6] = lAxis1[2].Dot(lAxis2[0]);
	lAxisDot[7] = lAxis1[2].Dot(lAxis2[1]);
	lAxisDot[8] = lAxis1[2].Dot(lAxis2[2]);
	lDistDot[2] = lAxis1[2].Dot(lDistance);
	lAbsAxisDot[6] = abs(lAxisDot[6]);
	lAbsAxisDot[7] = abs(lAxisDot[7]);
	lAbsAxisDot[8] = abs(lAxisDot[8]);
	lProjectedDistance = abs(lDistDot[2]);
	lProjectedSize2 = pOBB2.GetSize().x * lAbsAxisDot[6] +
	                  pOBB2.GetSize().y * lAbsAxisDot[7] + 
	                  pOBB2.GetSize().z * lAbsAxisDot[8];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().z;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}
	else if(lMinOverlapAxis == AXIS_NONE || lMinOverlapDist > (lProjectedSizeSum - lProjectedDistance))
	{
		lMinOverlapAxis = AXIS_Z1;
		lMinOverlapDist = lProjectedSizeSum - lProjectedDistance;
		lMinOverlapVector = lAxis1[2];
	}

	// Box2's X-Axis
	_TVarType lDistDotTemp = lAxis2[0].Dot(lDistance);
	lProjectedDistance = abs(lDistDotTemp);
	lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[0] +
	                  pOBB1.GetSize().y * lAbsAxisDot[3] +
	                  pOBB1.GetSize().z * lAbsAxisDot[6];
	lProjectedSizeSum = lProjectedSize1 + pOBB2.GetSize().x;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}
	else if(lMinOverlapAxis == AXIS_NONE || lMinOverlapDist > (lProjectedSizeSum - lProjectedDistance))
	{
		lMinOverlapAxis = AXIS_X2;
		lMinOverlapDist = lProjectedSizeSum - lProjectedDistance;
		lMinOverlapVector = lAxis2[0];
	}

	// Box2's Y-Axis
	lDistDotTemp = lAxis2[1].Dot(lDistance);
	lProjectedDistance = abs(lDistDotTemp);
	lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[1] +
	                  pOBB1.GetSize().y * lAbsAxisDot[4] +
	                  pOBB1.GetSize().z * lAbsAxisDot[7];
	lProjectedSizeSum = lProjectedSize1 + pOBB2.GetSize().y;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}
	else if(lMinOverlapAxis == AXIS_NONE || lMinOverlapDist > (lProjectedSizeSum - lProjectedDistance))
	{
		lMinOverlapAxis = AXIS_Y2;
		lMinOverlapDist = lProjectedSizeSum - lProjectedDistance;
		lMinOverlapVector = lAxis2[1];
	}

	// Box2's Z-Axis
	lDistDotTemp = lAxis2[2].Dot(lDistance);
	lProjectedDistance = abs(lDistDotTemp);
	lProjectedSize1 = pOBB1.GetSize().x * lAbsAxisDot[2] +
	                  pOBB1.GetSize().y * lAbsAxisDot[5] +
	                  pOBB1.GetSize().z * lAbsAxisDot[8];
	lProjectedSizeSum = lProjectedSize1 + pOBB2.GetSize().z;
	if(lProjectedDistance > lProjectedSizeSum)
	{
		return false;
	}
	else if(lMinOverlapAxis == AXIS_NONE || lMinOverlapDist > (lProjectedSizeSum - lProjectedDistance))
	{
		lMinOverlapAxis = AXIS_Z2;
		lMinOverlapDist = lProjectedSizeSum - lProjectedDistance;
		lMinOverlapVector = lAxis2[2];
	}


	//
	// And now all the cross products...
	//

	Vector3D<_TVarType> lC;
	_TVarType lCLength;

	unsigned int lCurrentAxis = (unsigned int)AXIS_X1xX2;
	int i, j;

	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			lC.Cross(lAxis1[i], lAxis2[j]);
			lCLength = lC.GetLength();

			if(lCLength > lEps)
			{
				lC.Normalize();

				lProjectedDistance = abs(lC.Dot(lDistance));

				lProjectedSize1 = pOBB1.GetSize().x * abs(lC.Dot(lAxis1[0])) + 
				                  pOBB1.GetSize().y * abs(lC.Dot(lAxis1[1])) + 
				                  pOBB1.GetSize().z * abs(lC.Dot(lAxis1[2]));

				lProjectedSize2 = pOBB2.GetSize().x * abs(lC.Dot(lAxis2[0])) + 
				                  pOBB2.GetSize().y * abs(lC.Dot(lAxis2[1])) + 
				                  pOBB2.GetSize().z * abs(lC.Dot(lAxis2[2]));

				lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
				if(lProjectedDistance > lProjectedSizeSum)
				{
					return false;
				}
				else if(lMinOverlapAxis == AXIS_NONE || lMinOverlapDist > (lProjectedSizeSum - lProjectedDistance))
				{
					lMinOverlapAxis = (AxisID)lCurrentAxis;
					lMinOverlapDist = lProjectedSizeSum - lProjectedDistance;
					lMinOverlapVector = lC;
				}
			}

			lCurrentAxis++;
		}
	}

	// Check if lMinOverlapVector has to be flipped.
	if(lDistance.Dot(lMinOverlapVector) > 0.0f)
	{
		lMinOverlapVector *= -1.0f;
	}

	lMinOverlapVector.Normalize();
	pCollisionInfo.mNormal = lMinOverlapVector;
	pCollisionInfo.mSeparationDistance = lMinOverlapDist;
	pCollisionInfo.mTimeToCollision = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// Collision tests with velocity.
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::AreMovingOBBsColliding(_TVarType pTimeDelta,
							    const OBB<_TVarType>& pOBB1,
							    const Vector3D<_TVarType>& pOBB1Velocity,
							    const OBB<_TVarType>& pOBB2,
							    const Vector3D<_TVarType>& pOBB2Velocity,
							    CollisionInfo& pCollisionInfo)
{
	// How this algorithm works:
	//
	// Try if there is any axis that can separate the boxes, just like
	// IsOBBOverlappingOBB() does. The difference is that we have to check
	// twice per axis, one check before the movement, and one check after.
	//
	// Example:
	//
	// OBB1            OBB2
	// -----------          / \
	// |ax.Y^    |   axisY/     \axisX
	// |    |    |      /  \   /  \
	// |    *--->|     <     *     >
	// |    axisX|      \         /
	// |         |        \     /
	// -----------          \ /
	//      *---------------> D0 = Projected distance on OBB1 axisX.
	//
	//      |---->           |----->
	//         A                B
	//
	// A = Projected (half) size of OBB1 on OBB1's axisX.
	// B = Projected (half) size of OBB2 on OBB1's axisX.
	//
	// if(A + B < D) then the boxes are not colliding (OBB1's axis separates the boxes).
	// 
	// In this example we are going to check if OBB1's x-axis can separate the boxes.
	// First we project the distance between the boxes BEFORE movement (like the picture above),
	// and then we do the same for the distance AFTER the movement.
	//
	// If both distance vectors (before and after movement) are pointing in the same direction,
	// and the boxes don't overlap, the boxes will not collide during their movement, and then we 
	// can return from this function and stop testing any further. In the picture above distance 
	// D0 is the distance before movement, and is pointing to the right. If the distance after 
	// movement (D1) is also pointing to the right, and the boxes don't overlap, we have no collision.
	//
	// If we DO have a collision on this axis, it doesn't mean that the boxes are actually
	// colliding. There may be other axes that can separate them. Therefore, we have to test
	// all axes (see IsOBBOverlappingOBB() for more details about the axes).
	//
	// After testing all axes, how do we calculate the TTC (time to collision)?
	// It is calculated like this (per axis):
	//
	// TTC = pTime * (D0 - (A + B)) / (D0 - D1);
	//
	// If the collision is immediate (TTC = 0), D0 = (A + B), otherwise D0 > (A + B).
	// The change in distance is D0 - D1, and the collision occurs after moving as far as
	// D0 - (A + B). This means that the formula: (D0 - (A + B)) / (D0 - D1) generates
	// a value between 0 and 1. Multiply this value with the time, and you have the exact
	// number of seconds to the collision.
	//
	// The axis that will result in the largest TTC possible, is the one where the collision
	// actually occurs, and is the correct TTC to return. All code for the testing and calculation
	// of the TTC is within the macros:
	// TSPMACRO_TEST_MOVING_OBB_COLLISION_0 and TSPMACRO_TEST_MOVING_OBB_COLLISION_1.
	// 

	const _TVarType lEpsilon = Lepra::MathTraits<_TVarType>::Eps();

	mOBBCollisionData->Reset();
	mCollisionInfo.mTimeToCollision = 0.0f;	// Reset to minimum.

	mOBBCollisionData->mOBB1RotAxis[0] = pOBB1.GetRotation().GetAxisX();
	mOBBCollisionData->mOBB1RotAxis[1] = pOBB1.GetRotation().GetAxisY();
	mOBBCollisionData->mOBB1RotAxis[2] = pOBB1.GetRotation().GetAxisZ();
	mOBBCollisionData->mOBB2RotAxis[0] = pOBB2.GetRotation().GetAxisX();
	mOBBCollisionData->mOBB2RotAxis[1] = pOBB2.GetRotation().GetAxisY();
	mOBBCollisionData->mOBB2RotAxis[2] = pOBB2.GetRotation().GetAxisZ();

	Vector3D<_TVarType>* lOBB1RotAxis[3];
	Vector3D<_TVarType>* lOBB2RotAxis[3];
	lOBB1RotAxis[0] = &mOBBCollisionData->mOBB1RotAxis[0];
	lOBB1RotAxis[1] = &mOBBCollisionData->mOBB1RotAxis[1];
	lOBB1RotAxis[2] = &mOBBCollisionData->mOBB1RotAxis[2];
	lOBB2RotAxis[0] = &mOBBCollisionData->mOBB2RotAxis[0];
	lOBB2RotAxis[1] = &mOBBCollisionData->mOBB2RotAxis[1];
	lOBB2RotAxis[2] = &mOBBCollisionData->mOBB2RotAxis[2];

	const Vector3D<_TVarType>& lOBB1Size = pOBB1.GetSize();
	const Vector3D<_TVarType>& lOBB2Size = pOBB2.GetSize();

	Vector3D<_TVarType> lRelativeVelocity(pOBB2Velocity - pOBB1Velocity);

	// Compute difference of box centers at time 0 and time 'pTime'.
	Vector3D<_TVarType> lDistance0(pOBB2.GetPosition() - pOBB1.GetPosition());
	Vector3D<_TVarType> lDistance1(lDistance0 + lRelativeVelocity * pTime);

	_TVarType lOBB1D0[3];	// Projected lDistance0 on OBB1's axes.
	_TVarType lOBB1D1[3];	// Projected lDistance1 on OBB1's axes.
	_TVarType lOBB2D0[3];	// Projected lDistance0 on OBB2's axes.
	_TVarType lOBB2D1[3];	// Projected lDistance1 on OBB2's axes.

	_TVarType* lC = mOBBCollisionData->mC;
	_TVarType* lAbsC = mOBBCollisionData->mAbsC;

	_TVarType lProjectedSize1;
	_TVarType lProjectedSize2;
	_TVarType lProjectedSizeSum;
	_TVarType lProjectedDist;

	// 1. OBB1RotAxisX
	lOBB1D0[0] = lOBB1RotAxis[0]->Dot(lDistance0);
	lOBB1D1[0] = lOBB1RotAxis[0]->Dot(lDistance1);
	lC[0] = lOBB1RotAxis[0]->Dot(*lOBB2RotAxis[0]);
	lC[1] = lOBB1RotAxis[0]->Dot(*lOBB2RotAxis[1]);
	lC[2] = lOBB1RotAxis[0]->Dot(*lOBB2RotAxis[2]);
	lAbsC[0] = abs(lC[0]);
	lAbsC[1] = abs(lC[1]);
	lAbsC[2] = abs(lC[2]);
	lProjectedSize2 = lOBB2Size.x * lAbsC[0] + 
	                  lOBB2Size.y * lAbsC[1] + 
	                  lOBB2Size.z * lAbsC[2];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().x;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(lOBB1D0[0],
					     lOBB1D1[0],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_X1);

	// 2. OBB1RotAxisY
	lOBB1D0[1] = lOBB1RotAxis[1]->Dot(lDistance0);
	lOBB1D1[1] = lOBB1RotAxis[1]->Dot(lDistance1);
	lC[3] = lOBB1RotAxis[1]->Dot(*lOBB2RotAxis[0]);
	lC[4] = lOBB1RotAxis[1]->Dot(*lOBB2RotAxis[1]);
	lC[5] = lOBB1RotAxis[1]->Dot(*lOBB2RotAxis[2]);
	lAbsC[3] = abs(lC[3]);
	lAbsC[4] = abs(lC[4]);
	lAbsC[5] = abs(lC[5]);
	lProjectedSize2 = lOBB2Size.x * lAbsC[3] + 
	                  lOBB2Size.y * lAbsC[4] + 
	                  lOBB2Size.z * lAbsC[5];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().y;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(lOBB1D0[1],
					     lOBB1D1[1],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Y1);

	// 3. OBB1RotAxisZ
	lOBB1D0[2] = lOBB1RotAxis[2]->Dot(lDistance0);
	lOBB1D1[2] = lOBB1RotAxis[2]->Dot(lDistance1);
	lC[6] = lOBB1RotAxis[2]->Dot(*lOBB2RotAxis[0]);
	lC[7] = lOBB1RotAxis[2]->Dot(*lOBB2RotAxis[1]);
	lC[8] = lOBB1RotAxis[2]->Dot(*lOBB2RotAxis[2]);
	lAbsC[6] = abs(lC[6]);
	lAbsC[7] = abs(lC[7]);
	lAbsC[8] = abs(lC[8]);
	lProjectedSize2 = lOBB2Size.x * lAbsC[6] + 
	                  lOBB2Size.y * lAbsC[7] + 
	                  lOBB2Size.z * lAbsC[8];
	lProjectedSizeSum = lProjectedSize2 + pOBB1.GetSize().z;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(lOBB1D0[2],
					     lOBB1D1[2],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Z1);
	// 4. OBB2RotAxisX
	lOBB2D0[0] = lOBB2RotAxis[0]->Dot(lDistance0);
	lOBB2D1[0] = lOBB2RotAxis[0]->Dot(lDistance1);
	lProjectedSize1 = lOBB1Size.x * lAbsC[0] + 
	                  lOBB1Size.y * lAbsC[3] + 
	                  lOBB1Size.z * lAbsC[6];
	lProjectedSizeSum = lProjectedSize1 + lOBB2Size.x;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(lOBB2D0[0],
					     lOBB2D1[0],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_X2);

	// 5. OBB2RotAxisY
	lOBB2D0[1] = lOBB2RotAxis[1]->Dot(lDistance0);
	lOBB2D1[1] = lOBB2RotAxis[1]->Dot(lDistance1);
	lProjectedSize1 = lOBB1Size.x * lAbsC[1] + 
	                  lOBB1Size.y * lAbsC[4] + 
	                  lOBB1Size.z * lAbsC[7];
	lProjectedSizeSum = lProjectedSize1 + lOBB2Size.y;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(lOBB2D0[1],
					     lOBB2D1[1],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Y2);

	// 6. OBB2RotAxisZ
	lOBB2D0[2] = lOBB2RotAxis[2]->Dot(lDistance0);
	lOBB2D1[2] = lOBB2RotAxis[2]->Dot(lDistance1);
	lProjectedSize1 = lOBB1Size.x * lAbsC[2] + 
	                  lOBB1Size.y * lAbsC[5] + 
	                  lOBB1Size.z * lAbsC[8];
	lProjectedSizeSum = lProjectedSize1 + lOBB2Size.z;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(lOBB2D0[2],
					     lOBB2D1[2],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Z2);

	// 7. OBB1RotAxisX x OBB2RotAxisX
	lProjectedSize1 = lOBB1Size.y * lAbsC[6] + lOBB1Size.z * lAbsC[3];
	lProjectedSize2 = lOBB2Size.y * lAbsC[2] + lOBB2Size.z * lAbsC[1];
	lProjectedDist = lOBB1D0[2] * lC[3] - lOBB1D0[1] * lC[6];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[2],
					     lC[3],
					     lOBB1D1[1],
					     lC[6],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_X1xX2);

	// 8. OBB1RotAxisX x OBB2RotAxisY
	lProjectedSize1 = lOBB1Size.y * lAbsC[7] + lOBB1Size.z * lAbsC[4];
	lProjectedSize2 = lOBB2Size.x * lAbsC[2] + lOBB2Size.z * lAbsC[0];
	lProjectedDist = lOBB1D0[2] * lC[4] - lOBB1D0[1] * lC[7];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[2],
					     lC[4],
					     lOBB1D1[1],
					     lC[7],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_X1xY2);

	// 9. OBB1RotAxisX x OBB2RotAxisZ
	lProjectedSize1 = lOBB1Size.y * lAbsC[8] + lOBB1Size.z * lAbsC[5];
	lProjectedSize2 = lOBB2Size.x * lAbsC[1] + lOBB2Size.y * lAbsC[0];
	lProjectedDist = lOBB1D0[2] * lC[5] - lOBB1D0[1] * lC[8];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[2],
					     lC[5],
					     lOBB1D1[1],
					     lC[8],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_X1xZ2);

	// 10. OBB1RotAxisY x OBB2RotAxisX
	lProjectedSize1 = lOBB1Size.x * lAbsC[6] + lOBB1Size.z * lAbsC[0];
	lProjectedSize2 = lOBB2Size.y * lAbsC[5] + lOBB2Size.z * lAbsC[4];
	lProjectedDist = lOBB1D0[0] * lC[6] - lOBB1D0[2] * lC[0];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[0],
					     lC[6],
					     lOBB1D1[2],
					     lC[0],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Y1xX2);

	// 11. OBB1RotAxisY x OBB2RotAxisY
	lProjectedSize1 = lOBB1Size.x * lAbsC[7] + lOBB1Size.z * lAbsC[1];
	lProjectedSize2 = lOBB2Size.x * lAbsC[5] + lOBB2Size.z * lAbsC[3];
	lProjectedDist = lOBB1D0[0] * lC[7] - lOBB1D0[2] * lC[1];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[0],
					     lC[7],
					     lOBB1D1[2],
					     lC[1],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Y1xY2);

	// 12. OBB1RotAxisY x OBB2RotAxisZ
	lProjectedSize1 = lOBB1Size.x * lAbsC[8] + lOBB1Size.z * lAbsC[2];
	lProjectedSize2 = lOBB2Size.x * lAbsC[4] + lOBB2Size.y * lAbsC[3];
	lProjectedDist = lOBB1D0[0] * lC[8] - lOBB1D0[2] * lC[2];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[0],
					     lC[8],
					     lOBB1D1[2],
					     lC[2],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Y1xZ2);

	// 13. OBB1RotAxisZ x OBB2RotAxisX
	lProjectedSize1 = lOBB1Size.x * lAbsC[3] + lOBB1Size.y * lAbsC[0];
	lProjectedSize2 = lOBB2Size.y * lAbsC[8] + lOBB2Size.z * lAbsC[7];
	lProjectedDist = lOBB1D0[1] * lC[0] - lOBB1D0[0] * lC[3];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[1],
					     lC[0],
					     lOBB1D1[0],
					     lC[3],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Z1xX2);

	// 14. OBB1RotAxisZ x OBB2RotAxisY
	lProjectedSize1 = lOBB1Size.x * lAbsC[4] + lOBB1Size.y * lAbsC[1];
	lProjectedSize2 = lOBB2Size.x * lAbsC[8] + lOBB2Size.z * lAbsC[6];
	lProjectedDist = lOBB1D0[1] * lC[1] - lOBB1D0[0] * lC[4];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[1],
					     lC[1],
					     lOBB1D1[0],
					     lC[4],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Z1xY2);

	// 15. OBB1RotAxisZ x OBB2RotAxisZ
	lProjectedSize1 = lOBB1Size.x * lAbsC[5] + lOBB1Size.y * lAbsC[2];
	lProjectedSize2 = lOBB2Size.x * lAbsC[7] + lOBB2Size.y * lAbsC[6];
	lProjectedDist = lOBB1D0[1] * lC[2] - lOBB1D0[0] * lC[5];
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(lProjectedDist,
					     lOBB1D1[1],
					     lC[2],
					     lOBB1D1[0],
					     lC[5],
					     lProjectedSizeSum,
					     pTimeDelta,
					     pCollisionInfo.mTimeToCollision,
					     mOBBCollisionData->mAxisID,
					     mOBBCollisionData->mSide,
					     AXIS_Z1xZ2);

	// At this point none of the 15 axes separate the boxes. It is still
	// possible that they are separated as viewed in any plane orthogonal
	// to the relative direction of motion W. In the worst case, the two
	// projected boxes are hexagons. This requires three separating axis
	// tests per box.

	Vector3D<_TVarType> lVelxDist(lRelativeVelocity, lDistance0);
	_TVarType lWOBB1[3];
	_TVarType lWOBB2[3];

	// 16. W x OBB1AxisX
	lWOBB1[1] = lRelativeVelocity.Dot(*lOBB1RotAxis[1]);
	TSPMACRO_SNAP_TO_ZERO(lWOBB1[1], lEpsilon);
	lWOBB1[2] = lRelativeVelocity.Dot(*lOBB1RotAxis[2]);
	TSPMACRO_SNAP_TO_ZERO(lWOBB1[2], lEpsilon);

	lProjectedDist = abs(lOBB1RotAxis[0]->Dot(lVelxDist));
	TSPMACRO_SNAP_TO_ZERO(lProjectedDist, lEpsilon);

	lProjectedSize1 = lOBB1Size.y * abs(lWOBB1[2]) + lOBB1Size.z * abs(lWOBB1[1]);
	lProjectedSize2 = lOBB2Size.x * abs(lC[3] * lWOBB1[2] - lC[6] * lWOBB1[1]) +
	                  lOBB2Size.y * abs(lC[4] * lWOBB1[2] - lC[7] * lWOBB1[1]) +
	                  lOBB2Size.z * abs(lC[5] * lWOBB1[2] - lC[8] * lWOBB1[1]);
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	if(lProjectedDist > lProjectedSizeSum)
	{
		pCollisionInfo.mTimeToCollision = pTimeDelta;
		return false;	// No collision.
	}

	// 17. W x OBB1AxisY
	lWOBB1[0] = lRelativeVelocity.Dot(*lOBB1RotAxis[0]);
	TSPMACRO_SNAP_TO_ZERO(lWOBB1[0], lEpsilon);

	lProjectedDist = abs(lOBB1RotAxis[1]->Dot(lVelxDist));
	TSPMACRO_SNAP_TO_ZERO(lProjectedDist, lEpsilon);

	lProjectedSize1 = lOBB1Size.z * abs(lWOBB1[0]) + lOBB1Size.x * abs(lWOBB1[2]);
	lProjectedSize2 = lOBB2Size.x * abs(lC[6] * lWOBB1[0] - lC[0] * lWOBB1[2]) +
	                  lOBB2Size.y * abs(lC[7] * lWOBB1[0] - lC[1] * lWOBB1[2]) +
	                  lOBB2Size.z * abs(lC[8] * lWOBB1[0] - lC[2] * lWOBB1[2]);
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	if(lProjectedDist > lProjectedSizeSum)
	{
		pCollisionInfo.mTimeToCollision = pTimeDelta;
		return false; // No collision.
	}

	// 18. W x OBB1AxisZ
	lProjectedDist = abs(lOBB1RotAxis[2]->Dot(lVelxDist));
	TSPMACRO_SNAP_TO_ZERO(lProjectedDist, lEpsilon);

	lProjectedSize1 = lOBB1Size.x * abs(lWOBB1[1]) + lOBB1Size.y * abs(lWOBB1[0]);
	lProjectedSize2 = lOBB2Size.x * abs(lC[0] * lWOBB1[1] - lC[3] * lWOBB1[0]) +
	                  lOBB2Size.y * abs(lC[1] * lWOBB1[1] - lC[4] * lWOBB1[0]) +
	                  lOBB2Size.z * abs(lC[2] * lWOBB1[1] - lC[5] * lWOBB1[0]);
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	if(lProjectedDist > lProjectedSizeSum)
	{
		pCollisionInfo.mTimeToCollision = pTimeDelta;
		return false; // No collision.
	}

	// 19. W x OBB2AxisX
	lWOBB2[1] = lRelativeVelocity.Dot(*lOBB2RotAxis[1]);
	TSPMACRO_SNAP_TO_ZERO(lWOBB2[1], lEpsilon);
	lWOBB2[2] = lRelativeVelocity.Dot(*lOBB2RotAxis[2]);
	TSPMACRO_SNAP_TO_ZERO(lWOBB2[2], lEpsilon);

	lProjectedDist = abs(lOBB2RotAxis[0]->Dot(lVelxDist));
	TSPMACRO_SNAP_TO_ZERO(lProjectedDist, lEpsilon);

	lProjectedSize1 = lOBB1Size.x * abs(lC[1] * lWOBB2[2] - lC[2] * lWOBB2[1]) +
	                  lOBB1Size.y * abs(lC[4] * lWOBB2[2] - lC[5] * lWOBB2[1]) +
	                  lOBB1Size.z * abs(lC[7] * lWOBB2[2] - lC[8] * lWOBB2[1]);
	lProjectedSize2 = lOBB2Size.y * abs(lWOBB2[2]) + lOBB2Size.z * abs(lWOBB2[1]);
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	if(lProjectedDist > lProjectedSizeSum)
	{
		pCollisionInfo.mTimeToCollision = pTimeDelta;
		return false; // No collision.
	}

	// 20. W x OBB2AxisY
	lWOBB2[0] = lRelativeVelocity.Dot(*lOBB2RotAxis[0]);
	TSPMACRO_SNAP_TO_ZERO(lWOBB2[0], lEpsilon);

	lProjectedDist = abs(lOBB2RotAxis[1]->Dot(lVelxDist));
	TSPMACRO_SNAP_TO_ZERO(lProjectedDist, lEpsilon);

	lProjectedSize1 = lOBB1Size.x * abs(lC[2] * lWOBB2[0] - lC[0] * lWOBB2[2]) +
	                  lOBB1Size.y * abs(lC[5] * lWOBB2[0] - lC[3] * lWOBB2[2]) +
	                  lOBB1Size.z * abs(lC[8] * lWOBB2[0] - lC[6] * lWOBB2[2]);
	lProjectedSize2 = lOBB2Size.z * abs(lWOBB2[0]) + lOBB2Size.x * abs(lWOBB2[2]);
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	if(lProjectedDist > lProjectedSizeSum)
	{
		pCollisionInfo.mTimeToCollision = pTimeDelta;
		return false; // No collision.
	}

	// 21. W x OBB2AxisZ
	lProjectedDist = abs(lOBB2RotAxis[2]->Dot(lVelxDist));
	TSPMACRO_SNAP_TO_ZERO(lProjectedDist, lEpsilon);

	lProjectedSize1 = lOBB1Size.x * abs(lC[0] * lWOBB2[1] - lC[1] * lWOBB2[0]) +
	                  lOBB1Size.y * abs(lC[3] * lWOBB2[1] - lC[4] * lWOBB2[0]) +
	                  lOBB1Size.z * abs(lC[6] * lWOBB2[1] - lC[7] * lWOBB2[0]);
	lProjectedSize2 = lOBB2Size.x * abs(lWOBB2[1]) + lOBB2Size.y * abs(lWOBB2[0]);
	lProjectedSizeSum = lProjectedSize1 + lProjectedSize2;
	if(lProjectedDist > lProjectedSizeSum)
	{
		pCollisionInfo.mTimeToCollision = pTimeDelta;
		return false; // No collision.
	}

	// If we get here, there is a collision. Now we have to find the appropriate collision
	// normal.
	mOBBCollisionData->mValidCollision = true;

	switch(mOBBCollisionData->mAxisID)
	{
		case AXIS_X1:
		case AXIS_Y1:
		case AXIS_Z1:
		{
			pCollisionInfo.mNormal = *lOBB1RotAxis[(int)mOBBCollisionData->mAxisID - 1];
		} break;
		case AXIS_X2:
		case AXIS_Y2:
		case AXIS_Z2:
		{
			pCollisionInfo.mNormal = *lOBB2RotAxis[(int)mOBBCollisionData->mAxisID - 4];
		} break;
		case AXIS_X1xX2:
		case AXIS_X1xY2:
		case AXIS_X1xZ2:
		{
			pCollisionInfo.mNormal.Cross(*lOBB1RotAxis[0], *lOBB2RotAxis[(int)mOBBCollisionData->mAxisID - 7]);
			// The Normalize() function will check if the vector magnitude is >0.
			pCollisionInfo.mNormal.Normalize();
		} break;
		case AXIS_Y1xX2:
		case AXIS_Y1xY2:
		case AXIS_Y1xZ2:
		{
			pCollisionInfo.mNormal.Cross(*lOBB1RotAxis[1], *lOBB2RotAxis[(int)mOBBCollisionData->mAxisID - 10]);
			// The Normalize() function will check if the vector magnitude is >0.
			pCollisionInfo.mNormal.Normalize();
		} break;
		case AXIS_Z1xX2:
		case AXIS_Z1xY2:
		case AXIS_Z1xZ2:
		{
			pCollisionInfo.mNormal.Cross(*lOBB1RotAxis[2], *lOBB2RotAxis[(int)mOBBCollisionData->mAxisID - 13]);
			// The Normalize() function will check if the vector magnitude is >0.
			pCollisionInfo.mNormal.Normalize();
		} break;
	};

	mOBBCollisionData->mAxis = pCollisionInfo.mNormal;

	// Check if the CollisionNormal has to be flipped.
	mOBBCollisionData->mOBB1CollisionPos = pOBB1.GetPosition() + pOBB1Velocity * pCollisionInfo.mTimeToCollision;
	mOBBCollisionData->mOBB2CollisionPos = pOBB2.GetPosition() + pOBB2Velocity * pCollisionInfo.mTimeToCollision;
	Vector3D<_TVarType> lCollisionPosDiff(mOBBCollisionData->mOBB2CollisionPos - mOBBCollisionData->mOBB1CollisionPos);

	if(pCollisionInfo.mNormal.Dot(lCollisionPosDiff) > 0.0f)
	{
		pCollisionInfo.mNormal *= -1.0f;
	}

	mOBBCollisionData->mOBB1Size[0] = lOBB1Size.x;
	mOBBCollisionData->mOBB1Size[1] = lOBB1Size.y;
	mOBBCollisionData->mOBB1Size[2] = lOBB1Size.z;
	mOBBCollisionData->mOBB2Size[0] = lOBB2Size.x;
	mOBBCollisionData->mOBB2Size[1] = lOBB2Size.y;
	mOBBCollisionData->mOBB2Size[2] = lOBB2Size.z;

	// There is no separation distance.
	pCollisionInfo.mSeparationDistance = 0;
	return true;
}

template<class _TVarType>
typename CollisionDetector3D<_TVarType>::CollisionType CollisionDetector3D<_TVarType>::GetOBBCollisionType()
{
	const _TVarType lEpsilon = Lepra::MathTraits<_TVarType>::Eps();
	const _TVarType lOneCmp = 1.0f - lEpsilon;

	// We have to figure out what kind of collision we have here... 
	// Point, edge or surface.

	// To accomplish this, we first have to check if any of the boxes have an edge
	// aligned with the collision normal (the axis stored in mOBBCollisionData).
	// While doing that, save all relevant values in mOBBCollisionData
	// used to get the intersection later on.
	bool lTestOBB1 = true;
	bool lTestOBB2 = true;
	bool lQuickTest = false;
	int lAlignedBox = 0;

	int lCIndexStart = 0;
	int lCIndexStep = 1;
	switch(mOBBCollisionData->mAxisID)
	{
	        case AXIS_X1:
		case AXIS_X2:
		{
			lCIndexStart = 0;
		} break;
		case AXIS_Y1:
		case AXIS_Y2:
		{
			lCIndexStart = 1;
		} break;
		case AXIS_Z1:
		case AXIS_Z2:
		{
			lCIndexStart = 2;
		} break;
	};

	// First a quick check.
	switch(mOBBCollisionData->mAxisID)
	{
		case AXIS_X1:
		case AXIS_Y1:
		case AXIS_Z1:
		{
			// This is all that is needed to get the collision type...
			lAlignedBox = 1;
			lTestOBB1 = false;

			mOBBCollisionData->mBoxWithSeparatingPlane = 0;
			mOBBCollisionData->mBox1NormalIndex = (int)mOBBCollisionData->mAxisID - (int)AXIS_X1;
			mOBBCollisionData->mBox1Side = mOBBCollisionData->mSide;

			lQuickTest = true;
		} break;
		case AXIS_X2:
		case AXIS_Y2:
		case AXIS_Z2:
		{
			// This is all that is needed to get the collision type...
			lAlignedBox = 2;
			lTestOBB2 = false;

			mOBBCollisionData->mBoxWithSeparatingPlane = 1;
			mOBBCollisionData->mBox2NormalIndex = (int)mOBBCollisionData->mAxisID - (int)AXIS_X2;
			mOBBCollisionData->mBox2Side = -mOBBCollisionData->mSide;

			lCIndexStep = 3;
			lQuickTest = true;
		} break;
	};

	// If needed, make a little deeper check...

	//
	// Check OBB1.
	//

	if(lTestOBB1 == true)
	{
		if(lQuickTest == true)
		{
			// Quicktest means that we don't need to calculate the dot product,
			// we can just get it from the mOBBCollisionData->mC - array.

			int lCIndex = lCIndexStart;
			for(int i = 0; i < 3; i++)
			{
				_TVarType lAbsDot = mOBBCollisionData->mAbsC[lCIndex];
				if(lAbsDot >= lOneCmp)
				{
					lAlignedBox += 1;
					mOBBCollisionData->mBox1NormalIndex = i;

					_TVarType lDot = mOBBCollisionData->mC[lCIndex];
					mOBBCollisionData->mBox1Side = lDot > 0.0f ?
						mOBBCollisionData->mSide :
						-mOBBCollisionData->mSide;
				}
				else if(lAbsDot < lEpsilon)
				{
					mOBBCollisionData->mBox1EdgeIndex = i;
				}

				lCIndex += lCIndexStep;
			}
		}
		else
		{
			// Normal check. We have to calculate the dot product.
			for(int i = 0; i < 3; i++)
			{
				_TVarType lDot = mOBBCollisionData->mAxis.Dot(mOBBCollisionData->mOBB1RotAxis[i]);
				_TVarType lAbsDot = (_TVarType)fabs(lDot);

				// If the dot product if very close to 1.0.
				if(lAbsDot >= lOneCmp)
				{
					lAlignedBox += 1;

					mOBBCollisionData->mBoxWithSeparatingPlane = 0;
					mOBBCollisionData->mBox1NormalIndex = i;

					mOBBCollisionData->mBox1Side = lDot > 0.0f ?
						mOBBCollisionData->mSide :
						-mOBBCollisionData->mSide;
					break;
				}
				else if(lAbsDot < lEpsilon)
				{
					mOBBCollisionData->mBox1EdgeIndex = i;
				}
			}
		}
	}



	//
	// Check OBB2.
	//

	if(lTestOBB2 == true)
	{
		if(lQuickTest == true)
		{
			// Quicktest means that we don't need to calculate the dot product,
			// we can just get it from the mOBBCollisionData->mC - array.

			int lCIndex = lCIndexStart;
			for(int i = 0; i < 3; i++)
			{
				_TVarType lAbsDot = mOBBCollisionData->mAbsC[lCIndex];
				if(lAbsDot >= lOneCmp)
				{
					lAlignedBox += 2;
					mOBBCollisionData->mBox2NormalIndex = i;

					_TVarType lDot = mOBBCollisionData->mC[lCIndex];
					mOBBCollisionData->mBox2Side = lDot > 0.0f ?
						-mOBBCollisionData->mSide :
						mOBBCollisionData->mSide;
				}
				else if(lAbsDot < lEpsilon)
				{
					mOBBCollisionData->mBox2EdgeIndex = i;
				}

				lCIndex += 3;
			}
		}
		else
		{
			// Normal check. We have to calculate the dot product.
			for(int i = 0; i < 3; i++)
			{
				_TVarType lDot = mOBBCollisionData->mAxis.Dot(mOBBCollisionData->mOBB2RotAxis[i]);
				_TVarType lAbsDot = (_TVarType)fabs(lDot);

				// If the dot product if very close to 1.0.
				if(lAbsDot >= lOneCmp)
				{
					lAlignedBox += 2;

					mOBBCollisionData->mBoxWithSeparatingPlane = 1;
					mOBBCollisionData->mBox2NormalIndex = i;

					mOBBCollisionData->mBox2Side = lDot > 0.0f ?
						-mOBBCollisionData->mSide :
						mOBBCollisionData->mSide;
					break;
				}
				else if(lAbsDot < lEpsilon)
				{
					mOBBCollisionData->mBox2EdgeIndex = i;
				}
			}
		}
	}

	if(lAlignedBox == 3)
	{
		// Both boxes are aligned... This makes it a surface to surface collision.
		return SURFACE_COLLISION;
	}
	else if(lAlignedBox == 0)
	{
		// None of the boxes are aligned. This makes it a point collision.
		return POINT_COLLISION;
	}

	// One box axis is aligned... Check if the other box has an edge perpendicualar to the
	// collision normal. In that case it is an edge collision, otherwise it is a point collision.
	if((lAlignedBox == 1 && mOBBCollisionData->mBox2EdgeIndex != -1) ||
	   (lAlignedBox == 2 && mOBBCollisionData->mBox1EdgeIndex != -1))
	{
		return EDGE_COLLISION;
	}

	return POINT_COLLISION;
}

template<class _TVarType>
void CollisionDetector3D<_TVarType>::GetOBBToOBBIntersection(Vector3D<_TVarType> pPoint[8], int &pNumPoints)
{
	if(mOBBCollisionData->mValidCollision == false)
	{
		pNumPoints = 0;
		return;
	}

	const _TVarType lEpsilon = Lepra::MathTraits<_TVarType>::Eps();
	CollisionType lCollisionType = GetOBBCollisionType();

	switch(lCollisionType)
	{
	case POINT_COLLISION:
		GetOBBPointIntersection(pPoint, pNumPoints);
		break;
	case EDGE_COLLISION:
	case SURFACE_COLLISION:
		GetOBBEdgeSurfaceIntersection(pPoint, pNumPoints, lCollisionType);
		break;
	};
}

template<class _TVarType>
void CollisionDetector3D<_TVarType>::GetOBBPointIntersection(Vector3D<_TVarType> pPoint[8], int &pNumPoints)
{
    // Determine the point of intersection
	const _TVarType lEpsilon = Lepra::MathTraits<_TVarType>::Eps();

	pNumPoints = 1;

	_TVarType lOBB1SignedExtents[3];
	_TVarType lOBB2SignedExtents[3];
	Vector3D<_TVarType> P;

	// The values in mOBBCollisionData->mC will be used to check
	// the alignment of the boxes, and chooses the first sign if mC[x] > 0
	// and the second sign if mC[x] is < 0.

	// Here I'll explain the math behind finding the point of intersection when 
	// the axis is one of the cross products.
	//
	// The cross-product-axes will only be the case when the boxes have collided edge to edge. 
	// In the case where an edge or a corner collides with a surface, the surface itself 
	// is the separating plane. And the surface is of course one side of one of the boxes.
	// Corner to corner and corner to edge are both cases that so rare they will probably never 
	// happen. And if they do (applying Murphys law), the result will be the same when handling 
	// it as a corner to a surface case anyway. That leaves us with the edge to edge case.
	//
	// Let's look at the case with AXIS_X1xX2.
	//
	// The following picture is a little bit incorrect. It shows an edge to surface collision,
	// but this is a good start to get the hang of the math.
	//
	// OBB1 (z-axis goes into the screen).
	// ---------------        / \
	// |      ^      |      /     \
	// |      |x-axis|    /\ z-axis \
	// |      |      |  /    \        \
	// |<-----*      |*<       *       > OBB2 (x-axis pointing out from the screen).
	// | y-axis      |  \     /       /
	// |             |   \  / y-axis/
	// |             |     \      /
	// ---------------       \  /
	//
	// In the case above, OBB2's x-axis is perpendicular to OBB1's x-axis, and the
	// distance between the two boxes is a vector perpendicular to OBB1's and OBB2's x-axis 
	// as well. We are now going to find the point on OBB1 where OBB2 intersects.
	// Using the macro TSPMACRO_GET_CORNER_POINT() will give us one of the corners of OBB1,
	// if all extents are as they should be (and of course they are).
	//
	// To get a corner of the box, all you have to do is to start from the 
	// center of the box and add or subtract the box's axes and multiply them with their
	// corresponding extent. Whether you should add or subtract the axes depends on which
	// corner you want. Since the axes are normalized (have a length of 1.0), you have
	// to multiply them with the box's half size (extents), in all three dimensions to get
	// to the actual corner.
	//
	// In our case above we can start by subtracting the y-axis of OBB1 and we will get
	// to its bottom side (the right side from the viewers perspective), where the intersection 
	// occurs. If we add or subtract the z-axis as well we will get to the actual edge.
	//
	// The problem now is the x-axis. From the center of the box, we don't want to step
	// to the actual corner, we only want to step a fraction of the x-axis total length.
	// Since the intersection in the picture above is already in the center, we don't want 
	// to go anywhere.
	//
	// What we can do to make TSPMACRO_GET_CORNER_POINT() give us the correct point is to
	// temporarily change the OBB1's x-extent. In the case above we can just set it to 0, but
	// this is due to the fact that OBB2 is rotated exactly a multiple of 45 degrees around its
	// x-axis. I can't draw an arbitrarily rotated box using ascii-graphics, so you have to 
	// imagine it... 
	// 
	// Ok, 0 is in the middle. If you project OBB2's y- and z-axis on OBB1's x-axis, and multiply
	// them with their corresponding extents, and add them together, you will actually get the 
	// correct extent! In the case above, the projection of the z-axis will be a positive number
	// (cosine of 45 degrees actually), and the projection of the y-axis will be negative by the 
	// same amount, (cosine of -45 degrees). Adding them together will result in 0, in this case.
	// 
	// Projecting a vector on another vector is done using the dot product. All the dot products
	// we need are actually already calculated and stored in mOBBCollisionData->mC.
	// The result so far will look like this:
	//
	// _TVarType lOBB1XExtent = mOBBCollisionData->mC[1] * lOBB2SignedExtents[1] + 
	//                        mOBBCollisionData->mC[2] * lOBB2SignedExtents[2];
	//
	// But what happens if the boxes centers aren't ligned up as nice as in the picture above?
	// What shall we do when the boxes centers are offset in OBB1's x-axis direction? Well, that's
	// an easy one. Just project the vector between the centers of the boxes and add this one too:
	//
	// Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
	//                                    mOBBCollisionData->mOBB1CollisionPos);
	//
	// _TVarType lX1CD = mOBBCollisionData->mOBB1RotAxis[0].Dot(lCollisionDist);
	// 
	// _TVarType lOBB1XExtent = lX1CD +
	//                        mOBBCollisionData->mC[1] * lOBB2SignedExtents[1] + 
	//                        mOBBCollisionData->mC[2] * lOBB2SignedExtents[2];
	//
	// Now, if the boxes are rotated a little bit relative to each other as if OBB2 was
	// rotated around OBB1's y-axis, we've got some more trouble to take care of... There will
	// be no difference looking at OBB2's colliding edge's center, but since this is and edge-
	// to-edge collision, and not a surface-to-edge collision as the picture illustrates, we
	// can't take for sure that the point on OBB2's colliding edge is its center. OBB1 should
	// be drawn rotated a little bit around its x-axis. Rotating OBB1 will move the point of
	// collision along OBB2's colliding edge. If you have trouble following me in this discussion,
	// try playing around with a pair of boxes IRL (in real life) to get the picture of it.
	//
	// What we have to do is to do exactly the same calculations that we've already done,
	// but the other way around, for OBB2 instead. Since the lCollisionDist vector is
	// pointing in the wrong direction, we have to subtract its projection instead of adding it:
	//
	// _TVarType lX2CD = mOBBCollisionData->mOBB2RotAxis[0].Dot(lCollisionDist);
	// 
	// _TVarType lOBB2XExtent = mOBBCollisionData->mC[3] * lOBB1SignedExtents[1] + 
	//                        mOBBCollisionData->mC[6] * lOBB1SignedExtents[2] -
	//                        lX2CD;
	//
	// Now all we have to do is to project lOBB2XExtent onto OBB1's x-axis, and put it into
	// the formula. The whole thing now looks like this:
	// 
	// Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
	//                                    mOBBCollisionData->mOBB1CollisionPos);
	//
	// _TVarType lX1CD = mOBBCollisionData->mOBB1RotAxis[0].Dot(lCollisionDist);
	// _TVarType lX2CD = mOBBCollisionData->mOBB2RotAxis[0].Dot(lCollisionDist);
	// 
	// _TVarType lOBB2XExtent = mOBBCollisionData->mC[3] * lOBB1SignedExtents[1] + 
	//                        mOBBCollisionData->mC[6] * lOBB1SignedExtents[2] -
	//                        lX2CD;
	//
	// _TVarType lOBB1XExtent = lX1CD +
	//                        mOBBCollisionData->mC[0] * lOBB2XExtent +
	//                        mOBBCollisionData->mC[1] * lOBB2SignedExtents[1] + 
	//                        mOBBCollisionData->mC[2] * lOBB2SignedExtents[2];


	switch(mOBBCollisionData->mAxisID)
	{
		case AXIS_X1:
		case AXIS_Y1:
		case AXIS_Z1:
		{
			int i = ((int)mOBBCollisionData->mAxisID - 1) * 3;
			for(int j = 0; j < 3; j++, i++)
			{
				TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[j], -, +, 
							   mOBBCollisionData->mSide,
							   mOBBCollisionData->mC[i],
							   mOBBCollisionData->mOBB2Size[j]);
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
						  mOBBCollisionData->mOBB2RotAxis, 
						  mOBBCollisionData->mOBB2CollisionPos, 
						  lOBB2SignedExtents);
			return;
		}
		case AXIS_X2:
		case AXIS_Y2:
		case AXIS_Z2:
		{
			int i = (int)mOBBCollisionData->mAxisID - 4;
			for(int j = 0; j < 3; j++, i += 3)
			{
				TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[j], +, -, 
				                           mOBBCollisionData->mSide,
				                           mOBBCollisionData->mC[i],
				                           mOBBCollisionData->mOBB1Size[j]);
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
		case AXIS_X1xX2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[1], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[6], // OBB1AxisZ * OBB2AxisX
			                           mOBBCollisionData->mOBB1Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[2], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[3], // OBB1AxisY * OBB2AxisX
			                           mOBBCollisionData->mOBB1Size[2]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[1], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[2], // OBB1AxisX * OBB2AxisZ
			                           mOBBCollisionData->mOBB2Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[2], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[1], // OBB1AxisX * OBB2AxisY
			                           mOBBCollisionData->mOBB2Size[2]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[0] * mOBBCollisionData->mC[0];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lX1CD = mOBBCollisionData->mOBB1RotAxis[0].Dot(lCollisionDist);
				_TVarType lX2CD = mOBBCollisionData->mOBB2RotAxis[0].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[3] * lOBB1SignedExtents[1] + 
				                  lC[6] * lOBB1SignedExtents[2] - lX2CD;
				lOBB1SignedExtents[0] = (lX1CD + 
					lC[0] * lTemp + 
					lC[1] * lOBB2SignedExtents[1] + 
					lC[2] * lOBB2SignedExtents[2]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[0] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
		case AXIS_X1xY2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[1], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[7], // OBB1AxisZ * OBB2AxisY
			                           mOBBCollisionData->mOBB1Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[2], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[4], // OBB1AxisY * OBB2AxisY
			                           mOBBCollisionData->mOBB1Size[2]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[0], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[2], // OBB1AxisX * OBB2AxisZ
			                           mOBBCollisionData->mOBB2Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[2], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[0], // OBB1AxisX * OBB2AxisX
			                           mOBBCollisionData->mOBB2Size[2]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[1] * mOBBCollisionData->mC[1];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lX1CD = mOBBCollisionData->mOBB1RotAxis[0].Dot(lCollisionDist);
				_TVarType lY2CD = mOBBCollisionData->mOBB2RotAxis[1].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[4] * lOBB1SignedExtents[1] + 
				                  lC[7] * lOBB1SignedExtents[2] - lY2CD;
				lOBB1SignedExtents[0] = (lX1CD + 
					lC[1] * lTemp + 
					lC[0] * lOBB2SignedExtents[0] + 
					lC[2] * lOBB2SignedExtents[2]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[0] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
		case AXIS_X1xZ2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[1], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[8], // OBB1AxisZ * OBB2AxisZ
			                           mOBBCollisionData->mOBB1Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[2], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[5], // OBB1AxisY * OBB2AxisZ
			                           mOBBCollisionData->mOBB1Size[2]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[0], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[1], // OBB1AxisX * OBB2AxisY
			                           mOBBCollisionData->mOBB2Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[1], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[0], // OBB1AxisX * OBB2AxisX
			                           mOBBCollisionData->mOBB2Size[1]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[2] * mOBBCollisionData->mC[2];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lX1CD = mOBBCollisionData->mOBB1RotAxis[0].Dot(lCollisionDist);
				_TVarType lZ2CD = mOBBCollisionData->mOBB2RotAxis[2].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[5] * lOBB1SignedExtents[1] + 
				                  lC[8] * lOBB1SignedExtents[2] - lZ2CD;
				lOBB1SignedExtents[0] = (lX1CD + 
					lC[2] * lTemp + 
					lC[0] * lOBB2SignedExtents[0] + 
					lC[1] * lOBB2SignedExtents[1]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[0] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
		case AXIS_Y1xX2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[0], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[6], // OBB1AxisZ * OBB2AxisX
			                           mOBBCollisionData->mOBB1Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[2], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[0], // OBB1AxisX * OBB2AxisX
			                           mOBBCollisionData->mOBB1Size[2]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[1], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[5], // OBB1AxisY * OBB2AxisZ
			                           mOBBCollisionData->mOBB2Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[2], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[4], // OBB1AxisY * OBB2AxisY
			                           mOBBCollisionData->mOBB2Size[2]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[3] * mOBBCollisionData->mC[3];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lY1CD = mOBBCollisionData->mOBB1RotAxis[1].Dot(lCollisionDist);
				_TVarType lX2CD = mOBBCollisionData->mOBB2RotAxis[0].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[0] * lOBB1SignedExtents[0] + 
				                  lC[6] * lOBB1SignedExtents[2] - lX2CD;
				lOBB1SignedExtents[1] = (lY1CD + 
					lC[3] * lTemp + 
					lC[4] * lOBB2SignedExtents[1] + 
					lC[5] * lOBB2SignedExtents[2]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[1] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
		case AXIS_Y1xY2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[0], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[7], // OBB1AxisZ * OBB2AxisY
			                           mOBBCollisionData->mOBB1Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[2], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[1], // OBB1AxisX * OBB2AxisY
			                           mOBBCollisionData->mOBB1Size[2]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[0], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[5], // OBB1AxisY * OBB2AxisZ
			                           mOBBCollisionData->mOBB2Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[2], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[3], // OBB1AxisY * OBB2AxisX
			                           mOBBCollisionData->mOBB2Size[2]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[4] * mOBBCollisionData->mC[4];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lY1CD = mOBBCollisionData->mOBB1RotAxis[1].Dot(lCollisionDist);
				_TVarType lY2CD = mOBBCollisionData->mOBB2RotAxis[1].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[1] * lOBB1SignedExtents[0] + 
				                  lC[7] * lOBB1SignedExtents[2] - lY2CD;
				lOBB1SignedExtents[1] = (lY1CD + 
					lC[4] * lTemp + 
					lC[3] * lOBB2SignedExtents[0] + 
					lC[5] * lOBB2SignedExtents[2]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[1] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);

			return;
		}
		case AXIS_Y1xZ2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[0], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[8], // OBB1AxisZ * OBB2AxisZ
			                           mOBBCollisionData->mOBB1Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[2], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[2], // OBB1AxisX * OBB2AxisZ
			                           mOBBCollisionData->mOBB1Size[2]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[0], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[4], // OBB1AxisY * OBB2AxisY
			                           mOBBCollisionData->mOBB2Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[1], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[3], // OBB1AxisY * OBB2AxisX
			                           mOBBCollisionData->mOBB2Size[1]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[5] * mOBBCollisionData->mC[5];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lY1CD = mOBBCollisionData->mOBB1RotAxis[1].Dot(lCollisionDist);
				_TVarType lZ2CD = mOBBCollisionData->mOBB2RotAxis[2].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[2] * lOBB1SignedExtents[0] + 
				                  lC[8] * lOBB1SignedExtents[2] - lZ2CD;
				lOBB1SignedExtents[1] = (lY1CD + 
					lC[5] * lTemp + 
					lC[3] * lOBB2SignedExtents[0] + 
					lC[4] * lOBB2SignedExtents[1]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[1] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
		case AXIS_Z1xX2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[0], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[3], // OBB1AxisY * OBB2AxisX
			                           mOBBCollisionData->mOBB1Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[1], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[0], // OBB1AxisX * OBB2AxisX
			                           mOBBCollisionData->mOBB1Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[1], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[8], // OBB1AxisZ * OBB2AxisZ
			                           mOBBCollisionData->mOBB2Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[2], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[7], // OBB1AxisZ * OBB2AxisY
			                           mOBBCollisionData->mOBB2Size[2]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[6] * mOBBCollisionData->mC[6];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lZ1CD = mOBBCollisionData->mOBB1RotAxis[2].Dot(lCollisionDist);
				_TVarType lX2CD = mOBBCollisionData->mOBB2RotAxis[0].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[0] * lOBB1SignedExtents[0] + 
				                  lC[3] * lOBB1SignedExtents[1] - lX2CD;
				lOBB1SignedExtents[2] = (lZ1CD + 
					lC[6] * lTemp + 
					lC[7] * lOBB2SignedExtents[1] + 
					lC[8] * lOBB2SignedExtents[2]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[2] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
		case AXIS_Z1xY2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[0], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[4], // OBB1AxisY * OBB2AxisY
			                           mOBBCollisionData->mOBB1Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[1], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[1], // OBB1AxisX * OBB2AxisY
			                           mOBBCollisionData->mOBB1Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[0], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[8], // OBB1AxisZ * OBB2AxisZ
			                           mOBBCollisionData->mOBB2Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[2], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[6], // OBB1AxisZ * OBB2AxisX
			                           mOBBCollisionData->mOBB2Size[2]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[7] * mOBBCollisionData->mC[7];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lZ1CD = mOBBCollisionData->mOBB1RotAxis[2].Dot(lCollisionDist);
				_TVarType lY2CD = mOBBCollisionData->mOBB2RotAxis[1].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[1] * lOBB1SignedExtents[0] + 
				                  lC[4] * lOBB1SignedExtents[1] - lY2CD;
				lOBB1SignedExtents[2] = (lZ1CD + 
					lC[7] * lTemp + 
					lC[6] * lOBB2SignedExtents[0] + 
					lC[8] * lOBB2SignedExtents[2]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[2] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
		case AXIS_Z1xZ2:
		{
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[0], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[5], // OBB1AxisY * OBB2AxisZ
			                           mOBBCollisionData->mOBB1Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB1SignedExtents[1], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[2], // OBB1AxisX * OBB2AxisZ
			                           mOBBCollisionData->mOBB1Size[1]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[0], -, +, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[7], // OBB1AxisZ * OBB2AxisY
			                           mOBBCollisionData->mOBB2Size[0]);
			TSPMACRO_GET_SIGNED_EXTENT(lOBB2SignedExtents[1], +, -, 
			                           mOBBCollisionData->mSide,
			                           mOBBCollisionData->mC[6], // OBB1AxisZ * OBB2AxisX
			                           mOBBCollisionData->mOBB2Size[1]);

			_TVarType lDiv = 1.0f - mOBBCollisionData->mC[8] * mOBBCollisionData->mC[8];

			if(abs(lDiv) > lEpsilon)
			{
				Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos - 
				                                   mOBBCollisionData->mOBB1CollisionPos);

				_TVarType lZ1CD = mOBBCollisionData->mOBB1RotAxis[2].Dot(lCollisionDist);
				_TVarType lZ2CD = mOBBCollisionData->mOBB2RotAxis[2].Dot(lCollisionDist);
				_TVarType* lC = mOBBCollisionData->mC;

				_TVarType lTemp = lC[2] * lOBB1SignedExtents[0] + 
				                  lC[5] * lOBB1SignedExtents[1] - lZ2CD;
				lOBB1SignedExtents[2] = (lZ1CD + 
					lC[8] * lTemp + 
					lC[6] * lOBB2SignedExtents[0] + 
					lC[7] * lOBB2SignedExtents[1]) / lDiv;
			}
			else
			{
				lOBB1SignedExtents[2] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(pPoint[0],
			                          mOBBCollisionData->mOBB1RotAxis,
			                          mOBBCollisionData->mOBB1CollisionPos,
			                          lOBB1SignedExtents);
			return;
		}
	};
}

template<class _TVarType>
void CollisionDetector3D<_TVarType>::GetOBBEdgeSurfaceIntersection(Vector3D<_TVarType> pPoint[8], int &pNumPoints, CollisionType pCollisionType)
{
	// The intersection is either a vector (edge-to-surface collision) or an area (surface-to-surface).
	// Determine the area of intersection.

	pNumPoints = 0;

	const _TVarType lEpsilon = Lepra::MathTraits<_TVarType>::Eps();
	const _TVarType lOneCmp = 1.0f - lEpsilon;

	//
	// Step 1: The setup. To make things easier, setup the following arrays to make 
	//                    the box with the separating plane become the box with index = 0.
	//
	Vector3D<_TVarType>* lBoxOrientation[2];
	Vector3D<_TVarType>* lBoxCenter[2];
	Vector3D<_TVarType>  lPlaneNormal[2];

	int lBoxNormalIndex[2];
	int lBoxEdgeIndex[2];
	_TVarType lBoxSide[2];
	_TVarType* lBoxExtents[2];

	int lBox0;
	int lBox1;

	switch(mOBBCollisionData->mBoxWithSeparatingPlane)
	{
		case 0:
		{
			lBox0 = 0;
			lBox1 = 1;
		} break;
		case 1:
		{
			lBox0 = 1;
			lBox1 = 0;
		} break;
		default: // Error...
		return;
	}

	lBoxNormalIndex[lBox0] = mOBBCollisionData->mBox1NormalIndex;
	lBoxEdgeIndex  [lBox0] = mOBBCollisionData->mBox1EdgeIndex;
	lBoxCenter     [lBox0] = &mOBBCollisionData->mOBB1CollisionPos;
	lBoxOrientation[lBox0] = mOBBCollisionData->mOBB1RotAxis;
	lBoxExtents    [lBox0] = mOBBCollisionData->mOBB1Size;
	lBoxSide       [lBox0] = mOBBCollisionData->mBox1Side;
	lPlaneNormal   [lBox0] = (lBoxOrientation[lBox0][lBoxNormalIndex[lBox0]] * lBoxSide[lBox0]);

	lBoxNormalIndex[lBox1] = mOBBCollisionData->mBox2NormalIndex;
	lBoxEdgeIndex  [lBox1] = mOBBCollisionData->mBox2EdgeIndex;
	lBoxCenter     [lBox1] = &mOBBCollisionData->mOBB2CollisionPos;
	lBoxOrientation[lBox1] = mOBBCollisionData->mOBB2RotAxis;
	lBoxExtents    [lBox1] = mOBBCollisionData->mOBB2Size;
	lBoxSide       [lBox1] = mOBBCollisionData->mBox2Side;
	lPlaneNormal   [lBox1] = (lBoxOrientation[lBox1][lBoxNormalIndex[lBox1]] * lBoxSide[lBox1]);

	//
	// Step 3: Get the vertices. Determine the points of the edge/surfaces.
	//

	Vector3D<_TVarType> lOBB1SideVertices[4];
	Vector3D<_TVarType> lOBB2SideVertices[4];
	Vector3D<_TVarType> lOBB2EdgeVertices[2];
	int lOBB1EdgeIndex[2];
	int lOBB2EdgeIndex[2];

	// Get the vertices and stuff of the side that is the separating plane...
	SetupBoxSide(*lBoxCenter[0],
		lBoxOrientation[0],
		lBoxExtents[0],
		lBoxNormalIndex[0],
		lPlaneNormal[0],
		lOBB1EdgeIndex[0], // Return parameter.
		lOBB1EdgeIndex[1], // Return parameter.
		lOBB1SideVertices);// Return parameter.

	// The normals of the planes that makes the sides of the separating plane surface.
	Vector3D<_TVarType> lNormals[4];
	lNormals[0] = lBoxOrientation[0][lOBB1EdgeIndex[1]] * -1.0f;
	lNormals[1] = lBoxOrientation[0][lOBB1EdgeIndex[0]];
	lNormals[2] = lBoxOrientation[0][lOBB1EdgeIndex[1]];
	lNormals[3] = lBoxOrientation[0][lOBB1EdgeIndex[0]] * -1.0f;

	if(pCollisionType == EDGE_COLLISION)
	{
		if(lBoxNormalIndex[0] == -1)
		{
			// Error, Box0 can't be anything else but parallel to itself.
			return;
		}

		// Setup a side that is perpendicular to the separating plane.
		SetupBoxSide(*lBoxCenter[1],
			lBoxOrientation[1],
			lBoxExtents[1],
			lBoxEdgeIndex[1],
			-lBoxOrientation[1][lBoxEdgeIndex[1]],
			lOBB2EdgeIndex[0], // Return parameter.
			lOBB2EdgeIndex[1], // Return parameter.
			lOBB2SideVertices);// Return parameter.

		//
		// Get the edge by finding the vertex that is closest to the separating plane.
		//
		_TVarType lMinDistance;
		int i;

		// Loop over the vertices...
		for(i = 0; i < 4; i++)
		{
			// The vector between the vertex and box0's center.
			Vector3D<_TVarType> lDelta(lOBB2SideVertices[i] - *lBoxCenter[0]);

			// Project it on the separating axis.
			_TVarType lDot = lBoxOrientation[0][lBoxNormalIndex[0]].Dot(lDelta);

			// Get the distance from the separating plane.
			_TVarType lDistance = (_TVarType)fabs((_TVarType)fabs(lDot) - lBoxExtents[0][lBoxNormalIndex[0]]);

			// Store the closest one.
			if(i == 0 || lDistance < lMinDistance)
			{
				lMinDistance = lDistance;

				lOBB2EdgeVertices[0] = lOBB2SideVertices[i];
				lOBB2EdgeVertices[1] = lOBB2SideVertices[i] + 
					lBoxOrientation[1][lBoxEdgeIndex[1]] * 
					lBoxExtents[1][lBoxEdgeIndex[1]] * 2.0f;
			}
		}

		// Now clip the edge against the sides of the separating plane surface.
		for(i = 0; i < 4; i++)
		{
			ClipVectorWithPlane(lOBB1SideVertices[i], 
				lNormals[i], 
				lOBB2EdgeVertices[0], 
				lOBB2EdgeVertices[1]);
		}

		pPoint[0] = lOBB2EdgeVertices[0];
		pPoint[1] = lOBB2EdgeVertices[1];
		pNumPoints = 2;
		
		return;
	}
	else
	{
		// Surface to surface collision. We've already got what we want (the planes):
		SetupBoxSide(*lBoxCenter[1],
			lBoxOrientation[1],
			lBoxExtents[1],
			lBoxNormalIndex[1],
			lPlaneNormal[1],
			lOBB2EdgeIndex[0],	// Return parameter.
			lOBB2EdgeIndex[1],	// Return parameter.
			lOBB2SideVertices);	// Return parameter.

		// At least 4 points...
		pNumPoints = 4;

		int i;
		for(i = 0; i < 4; i++)
		{
			pPoint[i] = lOBB2SideVertices[i];
		}

		int lStartPoint = 3;
		int lEndPoint = 0;
		int lPointCount = 0;

		Vector3D<_TVarType> lClippedPoints[8];
		Vector3D<_TVarType> lP1;
		Vector3D<_TVarType> lP2;

		for(int j = 0; j < 4; j++)
		{
			for(i = 0; i < pNumPoints; i++)
			{
				lP1 = pPoint[lStartPoint];
				lP2 = pPoint[lEndPoint];
				int lResult = ClipVectorWithPlane(lOBB1SideVertices[j], 
													lNormals[j], lP1, lP2);
				switch(lResult)
				{
					case 1:
					{
						lClippedPoints[lPointCount++] = lP1;
					} // TRICKY: Fallthrough!
					case 2:
					{
						lClippedPoints[lPointCount++] = lP2;
					} break;
					case 3:
					{
						lClippedPoints[lPointCount++] = lP2;
					} break;
				}

				lStartPoint = lEndPoint;
				lEndPoint++;
			}

			for(i = 0; i < lPointCount; i++)
			{
				pPoint[i] = lClippedPoints[i];
			}

			pNumPoints = lPointCount;

			// Reset...
			lStartPoint = lPointCount - 1;
			lEndPoint = 0;
			lPointCount = 0;
		}
	}
}

template<class _TVarType>
void CollisionDetector3D<_TVarType>::SetupBoxSide(Vector3D<_TVarType> pBoxCenter, 
						  Vector3D<_TVarType> pBoxOrientation[3], 
						  _TVarType pBoxExtents[3], 
						  int pNormalIndex,
						  Vector3D<_TVarType> pNormal,
						  int& pRetEdgeIndex1,	// One of the edges that is not parallel to the normal.
						  int& pRetEdgeIndex2,	// One of the edges that is not parallel to the normal.
						  Vector3D<_TVarType> pRetSideVertices[4])
{
	// First find the correct edge indices.
	switch(pNormalIndex)
	{
		case 0:			// Normal is x-axis.
		{
			pRetEdgeIndex1 = 1;	// Y-axis.
			pRetEdgeIndex2 = 2;	// Z-axis.
		} break;
		case 1:			// Normal is y-axis.
		{
			pRetEdgeIndex1 = 0;	// X-axis.
			pRetEdgeIndex2 = 2;	// Z-axis.
		} break;
		case 2:			// Normal is z-axis.
		{
			pRetEdgeIndex1 = 0;	// X-axis.
			pRetEdgeIndex2 = 1;	// Y-axis.
		} break;
	};

	Vector3D<_TVarType> lBox[3];
	lBox[pNormalIndex]   = pNormal * pBoxExtents[pNormalIndex];
	lBox[pRetEdgeIndex1] = pBoxOrientation[pRetEdgeIndex1] * pBoxExtents[pRetEdgeIndex1];
	lBox[pRetEdgeIndex2] = pBoxOrientation[pRetEdgeIndex2] * pBoxExtents[pRetEdgeIndex2];

	pRetSideVertices[0] = pBoxCenter + lBox[0] + lBox[1] + lBox[2];
	pRetSideVertices[1] = pRetSideVertices[0] - lBox[pRetEdgeIndex1] * 2.0f;
	pRetSideVertices[2] = pRetSideVertices[1] - lBox[pRetEdgeIndex2] * 2.0f;
	pRetSideVertices[3] = pRetSideVertices[2] + lBox[pRetEdgeIndex1] * 2.0f;
}

template<class _TVarType>
int CollisionDetector3D<_TVarType>::ClipVectorWithPlane(Vector3D<_TVarType> pPlanePoint, 
							Vector3D<_TVarType> pPlaneNormal,
							Vector3D<_TVarType>& pVectorPoint1, 
							Vector3D<_TVarType>& pVectorPoint2)
{
	_TVarType lSide1;
	_TVarType lSide2;
	_TVarType lTime;

	lSide1 = PlaneEquation(pPlanePoint, pPlaneNormal, pVectorPoint1);
	lSide2 = PlaneEquation(pPlanePoint, pPlaneNormal, pVectorPoint2);

	if(lSide1 >= 0.0f && lSide2 >= 0.0f)
	{
		return 3;
	}
	else if(lSide1 <= 0.0f && lSide2 <= 0.0f)
	{
		return 0;
	}
	else if(lSide1 >= 0.0f && lSide2 <= 0.0f)
	{
		// Point2 is moved to a clipped position.
		Vector3D<_TVarType> lDirection(pVectorPoint2 - pVectorPoint1);

		GetTimeOfVectorPlaneIntersection(pPlanePoint,
						 pPlaneNormal,
						 pVectorPoint1,
						 lDirection,
						 lTime);
		
		pVectorPoint2 = pVectorPoint1 + lDirection * lTime;

		return 2;
	}
	else
	{
		// Point1 is moved to a clipped position.
		Vector3D<_TVarType> lDirection(pVectorPoint2 - pVectorPoint1);

		GetTimeOfVectorPlaneIntersection(pPlanePoint, 
						 pPlaneNormal, 
						 pVectorPoint1,
						 lDirection,
						 lTime);
		
		pVectorPoint1 = pVectorPoint1 + lDirection * lTime;

		return 1;
	}
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::GetTimeOfVectorPlaneIntersection(const Vector3D<_TVarType>& pPlanePoint,
														  const Vector3D<_TVarType>& pPlaneNormal,
														  const Vector3D<_TVarType>& pVectorPoint,
														  const Vector3D<_TVarType>& pVectorDirection,
														  _TVarType& pReturnTime)
{
	_TVarType lD = -(pPlaneNormal * pPlanePoint);
	_TVarType lDivider = pPlaneNormal * pVectorDirection;

	if(lDivider == 0.0f)
	{
		return false;
	}

	_TVarType lDividend = -(pPlaneNormal * pVectorPoint + lD);

	pReturnTime = lDividend / lDivider;

	return true;
}
