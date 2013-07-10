
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/CollisionDetector2D.h"
#include "../Include/Math.h"



template<class _TVarType>
bool CollisionDetector2D<_TVarType>::IsAABR1EnclosingAABR2(const AABR<_TVarType>& pBox1, const AABR<_TVarType>& pBox2)
{
	Vector2D<_TVarType> lMin1(pBox1.GetPosition() - pBox1.GetSize());
	Vector2D<_TVarType> lMax1(pBox1.GetPosition() + pBox1.GetSize());
	Vector2D<_TVarType> lMin2(pBox2.GetPosition() - pBox2.GetSize());
	Vector2D<_TVarType> lMax2(pBox2.GetPosition() + pBox2.GetSize());
	return (lMin1.x <= lMin2.x && lMin1.y <= lMin2.y && lMax1.x >= lMax2.x && lMax1.y >= lMax2.y);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::IsAABREnclosingCircle(const AABR<_TVarType>& pBox, const Circle<_TVarType>& pCircle)
{
	Vector2D<_TVarType> lDim(pCircle.GetRadius(), pCircle.GetRadius());
	Vector2D<_TVarType> lCMin(pCircle.GetPosition() - lDim);
	Vector2D<_TVarType> lCMax(pCircle.GetPosition() + lDim);
	Vector2D<_TVarType> lMin(pBox.GetPosition() - pBox.GetSize());
	Vector2D<_TVarType> lMax(pBox.GetPosition() + pBox.GetSize());
	return (lMin.x <= lCMin.x && lMin.y <= lCMin.y && lMax.x >= lCMax.x && lMax.y >= lCMax.y);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::IsCircle1EnclosingCircle2(const Circle<_TVarType>& pCircle1, const Circle<_TVarType>& pCircle2)
{
	_TVarType lDist = pCircle2.GetPosition().GetDistance(pCircle1.GetPosition());
	return (pCircle1.GetRadius() >= lDist + pCircle2.GetRadius());
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::IsCircleEnclosingAABR(const Circle<_TVarType>& pCircle, const AABR<_TVarType>& pBox)
{
	return IsCircle1EnclosingCircle2(pCircle, Circle<_TVarType>(pBox.GetPosition(), pBox.GetSize().GetLength()));
}


template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticLineSegmentToLineSegmentTest(const Vector2D<_TVarType>& pStart1, const Vector2D<_TVarType>& pEnd1,
                                        const Vector2D<_TVarType>& pStart2, const Vector2D<_TVarType>& pEnd2,
					CollisionInfo* pCollisionInfo)
{
	if(pCollisionInfo != 0)
		pCollisionInfo->mTimeToCollision = 1;

	Vector2D<_TVarType> lDir1(pEnd1 - pStart1);
	Vector2D<_TVarType> lDir2(pEnd2 - pStart2);
	_TVarType lDenominator = lDir2.y * lDir1.x - lDir2.x * lDir1.y;

	if(abs(lDenominator) < MathTraits<_TVarType>::Eps())
		return false;

	Vector2D<_TVarType> lDiff(pStart1 - pStart2);
	_TVarType lNumerator = lDir2.x * lDiff.y - lDir2.y * lDiff.x;
	_TVarType t = lNumerator / lDenominator;

	// Not truly the intersection... Should be pStart1 + lDir1 * t, but
	// pStart1 was removed as a small optimization.
	Vector2D<_TVarType> lIntersection(lDir1 * t);

	_TVarType lLength = lDir1.GetLength();
	lDir1 /= lLength; // Normalize.

	// Would have been lDir1.Dot(lIntersection - pStart1) unoptimized.
	_TVarType lProjection1 = lDir1.Dot(lIntersection);

	if(lProjection1 < 0 || lProjection1 > lLength)
		return false;

	lLength = lDir2.GetLength();
	lDir2 /= lLength; // Normalize.

	// No difference in performance at this line, but this would have been 
	// lDir2.Dot(lIntersection - pStart2) unoptimized.
	_TVarType lProjection2 = lDir2.Dot(lIntersection + lDiff);

	if(lProjection2 < 0 || lProjection2 > lLength)
		return false;

	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mTimeToCollision = 0;
		pCollisionInfo->mPointOfCollision = lIntersection + pStart1;

		// Split the line segments at the point of intersection into 
		// four subsegments...
		Vector2D<_TVarType> lV1(pStart1 - pCollisionInfo->mPointOfCollision);
		Vector2D<_TVarType> lV2(pEnd1   - pCollisionInfo->mPointOfCollision);
		Vector2D<_TVarType> lV3(pStart2 - pCollisionInfo->mPointOfCollision);
		Vector2D<_TVarType> lV4(pEnd2   - pCollisionInfo->mPointOfCollision);

		// The normals of the lines. Note that they are already normalized.
		Vector2D<_TVarType> lNormal1(lDir1.y, -lDir1.x);
		Vector2D<_TVarType> lNormal2(lDir2.y, -lDir2.x);

		// Calculate the projections of the subsegments.
		_TVarType lP1 = lNormal2.Dot(lV1);
		_TVarType lP2 = lNormal2.Dot(lV2);
		_TVarType lP3 = lNormal1.Dot(lV3);
		_TVarType lP4 = lNormal1.Dot(lV4);

		// Find the minimum projection.
		int lMinPIndex = 0;
		_TVarType lMinP = lP1;
		_TVarType lMinAbsP = abs(lP1);
		_TVarType lAbsP = abs(lP2);
		if(lAbsP < lMinAbsP)
		{
			lMinAbsP = lAbsP;
			lMinP = lP2;
			lMinPIndex = 1;
		}
		lAbsP = abs(lP3);
		if(lAbsP < lMinAbsP)
		{
			lMinAbsP = lAbsP;
			lMinP = lP3;
			lMinPIndex = 2;
		}
		lAbsP = abs(lP4);
		if(lAbsP < lMinAbsP)
		{
			lMinAbsP = lAbsP;
			lMinP = lP4;
			lMinPIndex = 3;
		}

		// Now we know the separation distance.
		pCollisionInfo->mSeparationDistance = lMinAbsP;

		// Find the collision normal...
		switch(lMinPIndex)
		{
			case 0: pCollisionInfo->mNormal = lP1 > 0 ? -lNormal2 : lNormal2; break;
			case 1: pCollisionInfo->mNormal = lP2 > 0 ? -lNormal2 : lNormal2; break;
			case 2: pCollisionInfo->mNormal = lP3 > 0 ? lNormal1 : -lNormal1; break;
			case 3: pCollisionInfo->mNormal = lP4 > 0 ? lNormal1 : -lNormal1; break;
		};
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToAABRTest(const AABR<_TVarType>& pBox1, 
							  const AABR<_TVarType>& pBox2, 
							  CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lMinDist(pBox1.GetSize() + pBox2.GetSize());
	Vector2D<_TVarType> lDist(pBox1.GetPosition() - pBox2.GetPosition());

	if(lDist.x < 0.0f)
		lDist.x = -lDist.x;
	if(lDist.y < 0.0f)
		lDist.y = -lDist.y;

	if(lDist.x > lMinDist.x || lDist.y > lMinDist.y)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lBox1ExtentMin(pBox1.GetPosition() - pBox1.GetSize());
		Vector2D<_TVarType> lBox1ExtentMax(pBox1.GetPosition() + pBox1.GetSize());
		Vector2D<_TVarType> lBox2ExtentMin(pBox2.GetPosition() - pBox2.GetSize());
		Vector2D<_TVarType> lBox2ExtentMax(pBox2.GetPosition() + pBox2.GetSize());

		Vector2D<_TVarType> lOverlapMin(std::max(lBox1ExtentMin.x, lBox2ExtentMin.x), std::max(lBox1ExtentMin.y, lBox2ExtentMin.y));
		Vector2D<_TVarType> lOverlapMax(std::min(lBox1ExtentMax.x, lBox2ExtentMax.x), std::min(lBox1ExtentMax.y, lBox2ExtentMax.y));

		pCollisionInfo->mTimeToCollision = 0;
		pCollisionInfo->mPointOfCollision = (lOverlapMin + lOverlapMax) * (_TVarType)0.5;

		//Always positive.
		Vector2D<_TVarType> lDiff(lMinDist - lDist);
		if(lDiff.x < lDiff.y)
		{
			pCollisionInfo->mSeparationDistance = lDiff.x;
			if(lDist.x >= 0)
				pCollisionInfo->mNormal.Set(1, 0);
			else
				pCollisionInfo->mNormal.Set(-1, 0);
		}
		else
		{
			pCollisionInfo->mSeparationDistance = lDiff.y;
			if(lDist.y >= 0)
				pCollisionInfo->mNormal.Set(0, 1);
			else
				pCollisionInfo->mNormal.Set(0, -1);
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::VelocityAABRToAABRTest(const AABR<_TVarType>& pBox1,
                                                            const AABR<_TVarType>& pBox2,
                                                            const Vector2D<_TVarType>& pBox1Velocity,
                                                            const Vector2D<_TVarType>& pBox2Velocity,
                                                            CollisionInfo* pCollisionInfo)
{
	if(pCollisionInfo != 0)
		pCollisionInfo->mTimeToCollision = 0;

	Vector2D<_TVarType> lRelativeVelocity(pBox1Velocity - pBox2Velocity);
	Vector2D<_TVarType> lMinDist(pBox1.GetSize() + pBox2.GetSize());
	Vector2D<_TVarType> lDist(pBox1.GetPosition() - pBox2.GetPosition());
	Vector2D<_TVarType> lAbsDist(abs(lDist.x), abs(lDist.y));

	Vector2D<_TVarType> lMaxAllowedMove(lAbsDist - lMinDist);

	// Test x-axis. Even if the boxes are colliding from start,
	// don't count it as a collision if they are moving apart.
	if(lMaxAllowedMove.x > abs(lRelativeVelocity.x) ||
	   (lRelativeVelocity.x >= 0 && lDist.x >= 0) ||
	   (lRelativeVelocity.x < 0 && lDist.x < 0))
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		if(lDist.x >= 0)
			pCollisionInfo->mNormal.Set(-1, 0);
		else
			pCollisionInfo->mNormal.Set(1, 0);

		if(lMaxAllowedMove.x >= 0)
		{
			pCollisionInfo->mSeparationDistance = 0;
			pCollisionInfo->mTimeToCollision = (_TVarType)lMaxAllowedMove.x / (_TVarType)abs(lRelativeVelocity.x);
		}
		else
		{
			pCollisionInfo->mSeparationDistance = -lMaxAllowedMove.x;
			pCollisionInfo->mTimeToCollision = 0;
		}
	}

	// Test y-axis. Even if the boxes are colliding from start,
	// don't count it as a collision if they are moving apart.
	if(lMaxAllowedMove.y > abs(lRelativeVelocity.y) ||
	   (lRelativeVelocity.y >= 0 && lDist.y >= 0) ||
	   (lRelativeVelocity.y < 0 && lDist.y < 0))
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		_TVarType lTimeToCollision = (_TVarType)lMaxAllowedMove.y / (_TVarType)abs(lRelativeVelocity.y);
		if(lTimeToCollision > pCollisionInfo->mTimeToCollision)
		{
			if(lDist.y >= 0)
				pCollisionInfo->mNormal.Set(0, -1);
			else
				pCollisionInfo->mNormal.Set(0, 1);

			if(lMaxAllowedMove.y >= 0)
			{
				pCollisionInfo->mSeparationDistance = 0;
				pCollisionInfo->mTimeToCollision = lTimeToCollision;
			}
			else
			{
				pCollisionInfo->mSeparationDistance = -lMaxAllowedMove.y;
				pCollisionInfo->mTimeToCollision = 0;
			}
		}
	}

	// Check for collision by looking along he velocity vector.
	Vector2D<_TVarType> lNormal(lRelativeVelocity.y, -lRelativeVelocity.x);
	lNormal.Normalize();

	_TVarType lBox1Projection = abs(lNormal.Dot(pBox1.GetSize()));
	_TVarType lBox2Projection = abs(lNormal.Dot(pBox2.GetSize()));
	_TVarType lDistProjection = abs(lNormal.Dot(lDist));

	_TVarType lMinSepDist = lBox1Projection + lBox2Projection;

	if(lDistProjection > lMinSepDist)
		return false;

	if(pCollisionInfo != 0)
	{
		// Calculate the point of collision.
		Vector2D<_TVarType> lBox1ExtentMin(pBox1.GetPosition() - pBox1.GetSize());
		Vector2D<_TVarType> lBox1ExtentMax(pBox1.GetPosition() + pBox1.GetSize());
		Vector2D<_TVarType> lBox2ExtentMin(pBox2.GetPosition() - pBox2.GetSize());
		Vector2D<_TVarType> lBox2ExtentMax(pBox2.GetPosition() + pBox2.GetSize());

		Vector2D<_TVarType> lOverlapMin(max(lBox1ExtentMin.x, lBox2ExtentMin.x), max(lBox1ExtentMin.y, lBox2ExtentMin.y));
		Vector2D<_TVarType> lOverlapMax(min(lBox1ExtentMax.x, lBox2ExtentMax.x), min(lBox1ExtentMax.y, lBox2ExtentMax.y));

		Vector2D<_TVarType> lDiff(lOverlapMax - lOverlapMin);

		pCollisionInfo->mPointOfCollision = lOverlapMin + lDiff * (_TVarType)0.5;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToOBRTest(const OBR<_TVarType>& pBox1, 
							const OBR<_TVarType>& pBox2, 
                                                        CollisionInfo* pCollisionInfo)
{
	if(pCollisionInfo != 0)
		pCollisionInfo->mTimeToCollision = 0;

	Vector2D<_TVarType> lDist(pBox1.GetPosition() - pBox2.GetPosition());
	Vector2D<_TVarType> lSize(pBox1.GetSize() + pBox2.GetSize());

	Vector2D<_TVarType> lBox1ExtentX(pBox1.GetExtentX());
	Vector2D<_TVarType> lBox1ExtentY(pBox1.GetExtentY());
	Vector2D<_TVarType> lBox2ExtentX(pBox2.GetExtentX());
	Vector2D<_TVarType> lBox2ExtentY(pBox2.GetExtentY());

	// The first plane to test.
	Vector2D<_TVarType> lNormal(lBox1ExtentX);
	lNormal.Normalize();
	_TVarType lBox1Projection = pBox1.GetSize().x;
	_TVarType lBox2Projection = abs(lNormal.Dot(lBox2ExtentX)) + abs(lNormal.Dot(lBox2ExtentY));
	_TVarType lDistProjection = lNormal.Dot(lDist);
	_TVarType lAbsDistProjection = abs(lDistProjection);
	if(lAbsDistProjection > lBox1Projection + lBox2Projection)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		pCollisionInfo->mTimeToCollision = 0;
		pCollisionInfo->mSeparationDistance = lBox1Projection + lBox2Projection - lAbsDistProjection;
		if(lDistProjection > 0)
			pCollisionInfo->mNormal = lNormal;
		else
			pCollisionInfo->mNormal = -lNormal;
	}

	// The second plane to test.
	lNormal.Set(lBox1ExtentY);
	lNormal.Normalize();
	lBox1Projection = pBox1.GetSize().y;
	lBox2Projection = abs(lNormal.Dot(lBox2ExtentX)) + abs(lNormal.Dot(lBox2ExtentY));
	lDistProjection = lNormal.Dot(lDist);
	lAbsDistProjection = abs(lDistProjection);
	if(lAbsDistProjection > lBox1Projection + lBox2Projection)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		_TVarType lPenetrationDepth = lBox1Projection + lBox2Projection - lAbsDistProjection;
		if(lPenetrationDepth < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mTimeToCollision = 0;
			pCollisionInfo->mSeparationDistance = lPenetrationDepth;
			if(lDistProjection > 0)
				pCollisionInfo->mNormal = lNormal;
			else
				pCollisionInfo->mNormal = -lNormal;
		}
	}

	// The third plane to test.
	lNormal.Set(lBox2ExtentX);
	lNormal.Normalize();
	lBox1Projection = abs(lNormal.Dot(lBox1ExtentX)) + abs(lNormal.Dot(lBox1ExtentY));
	lBox2Projection = pBox2.GetSize().x;
	lDistProjection = lNormal.Dot(lDist);
	lAbsDistProjection = abs(lDistProjection);
	if(lAbsDistProjection > lBox1Projection + lBox2Projection)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		_TVarType lPenetrationDepth = lBox1Projection + lBox2Projection - lAbsDistProjection;
		if(lPenetrationDepth < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mTimeToCollision = 0;
			pCollisionInfo->mSeparationDistance = lPenetrationDepth;
			if(lDistProjection > 0)
				pCollisionInfo->mNormal = lNormal;
			else
				pCollisionInfo->mNormal = -lNormal;
		}
	}

	// The fourth and final plane to test.
	lNormal.Set(lBox2ExtentY);
	lNormal.Normalize();
	lBox1Projection = abs(lNormal.Dot(lBox1ExtentX)) + abs(lNormal.Dot(lBox1ExtentY));
	lBox2Projection = pBox2.GetSize().y;
	lDistProjection = lNormal.Dot(lDist);
	lAbsDistProjection = abs(lDistProjection);
	if(lAbsDistProjection > lBox1Projection + lBox2Projection)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		_TVarType lPenetrationDepth = lBox1Projection + lBox2Projection - lAbsDistProjection;
		if(lPenetrationDepth < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mTimeToCollision = 0;
			pCollisionInfo->mSeparationDistance = lPenetrationDepth;
			if(lDistProjection > 0)
				pCollisionInfo->mNormal = lNormal;
			else
				pCollisionInfo->mNormal = -lNormal;
		}
	}

	if(pCollisionInfo != 0 && mPOCEnabled)
	{
		// Calculate point of collision. First find the overlap, which is a 
		// convex polygon consisting of a maximum of 8 vertices. Clip each side
		// of Box1 against each side of Box2 using the same algorithm used in
		// UiTbc::Software3DPainter, and finally take the mean coordinates of
		// the clipped points.
		pCollisionInfo->mPointOfCollision.Set(0, 0);

		Vector2D<_TVarType> lInPoly[8];
		Vector2D<_TVarType> lOutPoly[8];
		Vector2D<_TVarType> lBox2PoL[4]; // Point on line.
		Vector2D<_TVarType> lBox2LineDir[4];
		
		lInPoly[0] = pBox1.GetPosition() + lBox1ExtentX + lBox1ExtentY;
		lInPoly[1] = pBox1.GetPosition() + lBox1ExtentX - lBox1ExtentY;
		lInPoly[2] = pBox1.GetPosition() - lBox1ExtentX - lBox1ExtentY;
		lInPoly[3] = pBox1.GetPosition() - lBox1ExtentX + lBox1ExtentY;

		// Initialize in the wrong order.
		int lVCount = 4;
		Vector2D<_TVarType>* lIn = lOutPoly;
		Vector2D<_TVarType>* lOut = lInPoly;

		// Setup directions clockwise.
		lBox2LineDir[0] = -lBox2ExtentY;
		lBox2LineDir[1] =  lBox2ExtentX;
		lBox2LineDir[2] =  lBox2ExtentY;
		lBox2LineDir[3] = -lBox2ExtentX;

		lBox2PoL[0] = pBox2.GetPosition() + lBox2ExtentX;
		lBox2PoL[1] = pBox2.GetPosition() + lBox2ExtentY;
		lBox2PoL[2] = pBox2.GetPosition() - lBox2ExtentX;
		lBox2PoL[3] = pBox2.GetPosition() - lBox2ExtentY;

		int i;
		for(i = 0; i < 4; i++)
		{
			// Swap in & out.
			Vector2D<_TVarType>* lTemp = lIn;
			lIn = lOut;
			lOut = lTemp;
			lVCount = ClipPolyAgainstLine(lIn, lVCount, lBox2PoL[i], lBox2LineDir[i], lOut);
		}

		for(i = 0; i < lVCount; i++)
		{
			pCollisionInfo->mPointOfCollision += lOut[i];
		}
		pCollisionInfo->mPointOfCollision /= (_TVarType)lVCount;
	}

	return true;
}
/*
// TODO: Implement this.
template<class _TVarType>
bool CollisionDetector2D<_TVarType>::VelocityOBRToOBRTest(const OBR<_TVarType>& pBox1,
                                                          const OBR<_TVarType>& pBox2,
                                                          const Vector2D<_TVarType>& pBox1Velocity,
                                                          const Vector2D<_TVarType>& pBox2Velocity,
                                                          CollisionInfo* pCollisionInfo)
{
	return false;
}
*/
template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticCircleToCircleTest(const Circle<_TVarType>& pCircle1, 
                                                              const Circle<_TVarType>& pCircle2,
			                                      CollisionInfo* pCollisionInfo)
{
	_TVarType lMinDistance = pCircle1.GetRadius() + pCircle2.GetRadius();
	_TVarType lMinDistanceSq = lMinDistance * lMinDistance;
	_TVarType lDistanceSq = pCircle1.GetPosition().GetDistanceSquared(pCircle2.GetPosition());
	if(lDistanceSq > lMinDistanceSq)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mNormal = (pCircle1.GetPosition() - pCircle2.GetPosition());
		_TVarType lLength = pCollisionInfo->mNormal.GetLength();
		pCollisionInfo->mNormal /= lLength; // Normalize
		pCollisionInfo->mSeparationDistance = (_TVarType)(lMinDistance - lLength);
		pCollisionInfo->mPointOfCollision = pCircle2.GetPosition() + pCollisionInfo->mNormal * (pCircle2.GetRadius() - pCollisionInfo->mSeparationDistance * (_TVarType)0.5);
		pCollisionInfo->mTimeToCollision = 0;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticTriangleToTriangleTest(const Vector2D<_TVarType> pTri1[3], 
								  const Vector2D<_TVarType> pTri2[3],
								  CollisionInfo* pCollisionInfo)
{
	if(pCollisionInfo != 0)
		pCollisionInfo->mTimeToCollision = 1;

	 // Find out wether the triangles are clockwise.
	Vector2D<_TVarType> lPerp1((pTri1[1] - pTri1[0]).GetPerpCW());
	Vector2D<_TVarType> lPerp2((pTri2[1] - pTri2[0]).GetPerpCW());
	_TVarType lP1 = lPerp1.Dot(pTri1[2] - pTri1[0]);
	_TVarType lP2 = lPerp2.Dot(pTri2[2] - pTri2[0]);

	// Copy triangle data and make sure they are stored in clockwise order.
	Vector2D<_TVarType> lTri1[3];
	Vector2D<_TVarType> lTri2[3];

	if(lP1 >= 0)
	{
		lTri1[0] = pTri1[0];
		lTri1[1] = pTri1[1];
		lTri1[2] = pTri1[2];
	}
	else
	{
		lTri1[0] = pTri1[0];
		lTri1[1] = pTri1[2];
		lTri1[2] = pTri1[1];
	}

	if(lP2 >= 0)
	{
		lTri2[0] = pTri2[0];
		lTri2[1] = pTri2[1];
		lTri2[2] = pTri2[2];
	}
	else
	{
		lTri2[0] = pTri2[0];
		lTri2[1] = pTri2[2];
		lTri2[2] = pTri2[1];
	}

	// We can't use the regular technique of separating "planes" (lines) as usual, 
	// since the triangles are not (in general) symmetric along any axis. Instead,
	// the separating line has to be fully defined (as opposed to only knowing the
	// slope). Each side of each triangle defines one such line, against which we
	// have to clip the other triangle before applying the method of separating
	// lines.

	// Now find the overlap between the triangles.
	Vector2D<_TVarType> lOut[6];
	Vector2D<_TVarType> lEdge(lTri2[1] - lTri2[0]);
	int lVCount;
	if((lVCount = ClipPolyAgainstLine(lTri1, 3, lTri2[0], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri2[0]));
		pCollisionInfo->mSeparationDistance = lProjection;
		pCollisionInfo->mNormal = -lN;
	}

	lEdge = lTri2[2] - lTri2[1];
	if((lVCount = ClipPolyAgainstLine(lTri1, 3, lTri2[1], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri2[1]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = -lN;
		}
	}

	lEdge = lTri2[0] - lTri2[2];
	if((lVCount = ClipPolyAgainstLine(lTri1, 3, lTri2[2], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri2[2]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = -lN;
		}
	}

	lEdge = lTri1[1] - lTri1[0];
	if((lVCount = ClipPolyAgainstLine(lTri2, 3, lTri1[0], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri1[0]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = lN;
		}
	}

	lEdge = lTri1[2] - lTri1[1];
	if((lVCount = ClipPolyAgainstLine(lTri2, 3, lTri1[1], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri1[1]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = lN;
		}
	}

	lEdge = lTri1[0] - lTri1[2];
	if((lVCount = ClipPolyAgainstLine(lTri2, 3, lTri1[2], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri1[2]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = lN;
		}

		pCollisionInfo->mTimeToCollision = 0;

		// Now we have to find the overlap in order to find the point of collision.
		if(mPOCEnabled)
		{
			Vector2D<_TVarType> lTemp[6];
			int lVCount;
			lVCount = ClipPolyAgainstLine(lTri1, 3, lTri2[0], lTri2[1] - lTri2[0], lOut);
			lVCount = ClipPolyAgainstLine(lOut, lVCount, lTri2[1], lTri2[2] - lTri2[1], lTemp);
			lVCount = ClipPolyAgainstLine(lTemp, lVCount, lTri2[2], lTri2[0] - lTri2[2], lOut);

			pCollisionInfo->mPointOfCollision.Set(0, 0);
			int i;
			for(i = 0; i < lVCount; i++)
			{
				pCollisionInfo->mPointOfCollision += lOut[i];
			}
			pCollisionInfo->mPointOfCollision /= (_TVarType)lVCount;
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticPointLeftOfLineSegmentTest(const Vector2D<_TVarType>& pPoint,
                                                                      const Vector2D<_TVarType>& pStart, 
                                                                      const Vector2D<_TVarType>& pEnd,
                                                                      CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lL(pEnd - pStart);
	_TVarType lLength = lL.GetLength();
	lL /= lLength; // Normalize.
	Vector2D<_TVarType> lP(pPoint - pStart);

	_TVarType lProjection = lL.Dot(lP);
	if(lProjection < 0 || lProjection > lLength)
	{
		// Not between the end points of the line segment.
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	// Line normal, pointing to the right.
	Vector2D<_TVarType> lLPerp(lL.y, -lL.x);

	lProjection = lLPerp.Dot(lP);

	if(lProjection > 0)
	{
		// Point is to the right of the line segment.
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mNormal = lLPerp;
		pCollisionInfo->mSeparationDistance = -lProjection;
		pCollisionInfo->mPointOfCollision = pPoint;
		pCollisionInfo->mTimeToCollision = 0;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticPointLeftOfLineTest(const Vector2D<_TVarType>& pPoint,
							       const Vector2D<_TVarType>& pPointOnLine, 
							       const Vector2D<_TVarType>& pDir,
							       CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lNormal(pDir.y, -pDir.x);
	lNormal.Normalize();
	_TVarType lProjection = lNormal.Dot(pPoint - pPointOnLine);
	if(lProjection > 0)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mNormal = lNormal;
		pCollisionInfo->mSeparationDistance = -lProjection;
		pCollisionInfo->mPointOfCollision = pPoint;
		pCollisionInfo->mTimeToCollision = 0;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticPointToTriangleTest(const Vector2D<_TVarType>& pPoint,
							       const Vector2D<_TVarType> pTri[3], 
							       CollisionInfo* pCollisionInfo)
{
	if(pCollisionInfo != 0)
		pCollisionInfo->mTimeToCollision = 1;

	 // Find out wether the triangle is clockwise.
	Vector2D<_TVarType> lPerp((pTri[1] - pTri[0]).GetPerpCW());
	_TVarType lP = lPerp.Dot(pTri[2] - pTri[0]);

	Vector2D<_TVarType> lTri[3];
	if(lP >= 0)
	{
		lTri[0] = pTri[0];
		lTri[1] = pTri[1];
		lTri[2] = pTri[2];
	}
	else
	{
		lTri[0] = pTri[0];
		lTri[1] = pTri[2];
		lTri[2] = pTri[1];
	}
	Vector2D<_TVarType> lNormal((lTri[1] - lTri[0]).GetPerpCW());
	_TVarType lProjection = lNormal.Dot(pPoint - lTri[0]);
	if(lProjection < 0)
		return false;

	if(pCollisionInfo != 0)
	{
		_TVarType lLengthRecip = 1 / lNormal.GetLength();
		pCollisionInfo->mNormal = lNormal * -lLengthRecip; // Normalize.
		pCollisionInfo->mSeparationDistance = lProjection * lLengthRecip;
	}

	lNormal.SetPerpCW(lTri[2] - lTri[1]);
	lProjection = lNormal.Dot(pPoint - lTri[1]);
	if(lProjection < 0)
		return false;

	if(pCollisionInfo != 0)
	{
		_TVarType lLengthRecip = 1 / lNormal.GetLength();
		lProjection *= lLengthRecip;

		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mNormal = lNormal * -lLengthRecip; // Normalize.
			pCollisionInfo->mSeparationDistance = lProjection;
		}
	}

	lNormal.SetPerpCW(lTri[0] - lTri[2]);
	lProjection = lNormal.Dot(pPoint - lTri[2]);
	if(lProjection < 0)
		return false;

	if(pCollisionInfo != 0)
	{
		_TVarType lLengthRecip = 1 / lNormal.GetLength();
		lProjection *= lLengthRecip;

		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mNormal = lNormal * -lLengthRecip; // Normalize.
			pCollisionInfo->mSeparationDistance = lProjection;
		}

		pCollisionInfo->mTimeToCollision = 0;
		pCollisionInfo->mPointOfCollision = pPoint;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToPointTest(const AABR<_TVarType>& pBox, 
							   const Vector2D<_TVarType>& pPoint, 
							   CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lBoxExtentMin(pBox.GetPosition() - pBox.GetSize());
	Vector2D<_TVarType> lBoxExtentMax(pBox.GetPosition() + pBox.GetSize());

	if(pPoint.x >= lBoxExtentMin.x && pPoint.x < lBoxExtentMax.x &&
	   pPoint.y >= lBoxExtentMin.y && pPoint.y < lBoxExtentMax.y)
	{
		if(pCollisionInfo != 0)
		{
			pCollisionInfo->mTimeToCollision = 0;
			pCollisionInfo->mPointOfCollision = pPoint;

			// Assume point penetrated left side of box.
			pCollisionInfo->mNormal.Set(1, 0); // Relative to box.
			pCollisionInfo->mSeparationDistance = abs(pPoint.x - lBoxExtentMin.x);
			
			// Check right side.
			_TVarType lPenetrationDepth = abs(pPoint.x - lBoxExtentMax.x);
			if(lPenetrationDepth < pCollisionInfo->mSeparationDistance)
			{
				pCollisionInfo->mNormal.Set(-1, 0); // Relative to box.
				pCollisionInfo->mSeparationDistance = abs(pPoint.x - lBoxExtentMax.x);
			}

			// Check top side.
			lPenetrationDepth = abs(pPoint.y - lBoxExtentMin.y);
			if(lPenetrationDepth < pCollisionInfo->mSeparationDistance)
			{
				pCollisionInfo->mNormal.Set(0, 1); // Relative to box.
				pCollisionInfo->mSeparationDistance = abs(pPoint.y - lBoxExtentMin.y);
			}

			// Check bottom side.
			lPenetrationDepth = abs(pPoint.y - lBoxExtentMax.y);
			if(lPenetrationDepth < pCollisionInfo->mSeparationDistance)
			{
				pCollisionInfo->mNormal.Set(0, -1); // Relative to box.
				pCollisionInfo->mSeparationDistance = abs(pPoint.y - lBoxExtentMax.y);
			}
		}

		return true;
	}
	else
	{
		if(pCollisionInfo != 0)
		{
			pCollisionInfo->mTimeToCollision = 1;
		}
		return false;
	}
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToOBRTest(const AABR<_TVarType>& pBox1, 
							 const OBR<_TVarType>& pBox2, 
							 CollisionInfo* pCollisionInfo)
{
	// TODO: Implement a case specific optimized version if you think it's worth
	// all the trouble. =)
	OBR<_TVarType> lBox1(pBox1.GetPosition(), pBox1.GetSize());
	return StaticOBRToOBRTest(lBox1, pBox2, pCollisionInfo);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToCircleTest(const AABR<_TVarType>& pBox, 
							    const Circle<_TVarType>& pCircle, 
							    CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lMin(pBox.GetPosition() - pBox.GetSize());
	Vector2D<_TVarType> lMax(pBox.GetPosition() + pBox.GetSize());
	Vector2D<_TVarType> lPointOnBox(0, 0);

	if(pCircle.GetPosition().x < lMin.x)
		lPointOnBox.x = lMin.x;
	else if(pCircle.GetPosition().x > lMax.x)
		lPointOnBox.x = lMax.x;
	else
		lPointOnBox.x = pCircle.GetPosition().x;

	if(pCircle.GetPosition().y < lMin.y)
		lPointOnBox.y = lMin.y;
	else if(pCircle.GetPosition().y > lMax.y)
		lPointOnBox.y = lMax.y;
	else if(pCollisionInfo != 0)
		lPointOnBox.y = pCircle.GetPosition().y;

	Vector2D<_TVarType> lDist(lPointOnBox - pCircle.GetPosition());
	_TVarType lDistanceSquared = lDist.GetLengthSquared();

	if(lDistanceSquared > pCircle.GetRadiusSquared())
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
		pCollisionInfo->mSeparationDistance = pCircle.GetRadius() - lLength;
		pCollisionInfo->mPointOfCollision = pCircle.GetPosition() + pCollisionInfo->mNormal * ((pCircle.GetRadius() + lLength) * (_TVarType)0.5);
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToLineSegmentTest(const AABR<_TVarType>& pBox, 
								 const Vector2D<_TVarType>& pStart,
								 const Vector2D<_TVarType>& pEnd,
								 CollisionInfo* pCollisionInfo)
{
	// TODO: Implement a case specific optimized version if you think it's worth
	// all the trouble. =)
	OBR<_TVarType> lOBR(pBox.GetPosition(), pBox.GetSize(), 0);
	return StaticOBRToLineSegmentTest(lOBR, pStart, pEnd, pCollisionInfo);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToTriangleTest(const AABR<_TVarType>& pBox, 
                                                              const Vector2D<_TVarType> pTri[3], 
                                                              CollisionInfo* pCollisionInfo)
{
	// TODO: Implement a case specific optimized version if you think it's worth
	// all the trouble. =)
	OBR<_TVarType> lOBR(pBox.GetPosition(), pBox.GetSize(), 0);
	return StaticOBRToTriangleTest(lOBR, pTri, pCollisionInfo);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToPointTest(const OBR<_TVarType>& pBox, 
							  const Vector2D<_TVarType>& pPoint, 
							  CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lDist(pPoint - pBox.GetPosition());
	Vector2D<_TVarType> lXAxis(pBox.GetExtentX());
	lXAxis.Normalize();
	_TVarType lXProjection = lXAxis.Dot(lDist);
	_TVarType lAbsXProjection = abs(lXProjection);
	if(lAbsXProjection > pBox.GetSize().x)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	Vector2D<_TVarType> lYAxis(pBox.GetExtentY());
	lYAxis.Normalize();
	_TVarType lYProjection = lYAxis.Dot(lDist);
	_TVarType lAbsYProjection = abs(lYProjection);
	if(lAbsYProjection > pBox.GetSize().y)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mTimeToCollision = 1;
		pCollisionInfo->mPointOfCollision = pPoint;

		_TVarType lXDiff = pBox.GetSize().x - lAbsXProjection;
		_TVarType lYDiff = pBox.GetSize().y - lAbsYProjection;

		if(lXDiff < lYDiff)
		{
			pCollisionInfo->mNormal = lXProjection > 0 ? -lXAxis : lXAxis;
			pCollisionInfo->mSeparationDistance = lXDiff;
		}
		else
		{
			pCollisionInfo->mNormal = lYProjection > 0 ? -lYAxis : lYAxis;
			pCollisionInfo->mSeparationDistance = lYDiff;
		}
		pCollisionInfo->mNormal.Normalize();
	}

	return true;
}


template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToCircleTest(const OBR<_TVarType>& pBox, 
							   const Circle<_TVarType>& pCircle,
							   CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lDist(pCircle.GetPosition() - pBox.GetPosition());
	Vector2D<_TVarType> lXAxis(pBox.GetExtentX());
	lXAxis.Normalize();
	_TVarType lXProjection = lXAxis.Dot(lDist);
	_TVarType lAbsXProjection = abs(lXProjection);
	if(lAbsXProjection > pBox.GetSize().x + pCircle.GetRadius())
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	Vector2D<_TVarType> lYAxis(pBox.GetExtentY());
	lYAxis.Normalize();
	_TVarType lYProjection = lYAxis.Dot(lDist);
	_TVarType lAbsYProjection = abs(lYProjection);
	if(lAbsYProjection > pBox.GetSize().y + pCircle.GetRadius())
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	// Find the point on the box which is closest to the circle.
	Vector2D<_TVarType> lPoL[4];
	lPoL[0] = lPoL[1] = pBox.GetPosition() - pBox.GetExtentX() - pBox.GetExtentY();
	lPoL[2] = lPoL[3] = pBox.GetPosition() + pBox.GetExtentX() + pBox.GetExtentY();

	Vector2D<_TVarType> lDir[4];
	lDir[0] = lYAxis;
	lDir[1] = lXAxis;
	lDir[2] = -lYAxis;
	lDir[3] = -lXAxis;

	_TVarType lMaxDirDist[4];
	lMaxDirDist[0] = lMaxDirDist[2] = pBox.GetSize().y * 2;
	lMaxDirDist[1] = lMaxDirDist[3] = pBox.GetSize().x * 2;

	_TVarType lMinDistance = 0;
	Vector2D<_TVarType> lClosestPoint;
	for(int i = 0; i < 4; i++)
	{
		Vector2D<_TVarType> lCP;
		_TVarType lDirDistance;
		_TVarType lDistance = LineDistance(lPoL[i], lDir[i], pCircle.GetPosition(), lCP, lDirDistance);

		if(lDirDistance > lMaxDirDist[i])
		{
			lCP = lPoL[i] + lDir[i] * lMaxDirDist[i];
			lDistance = pCircle.GetPosition().GetDistance(lCP);
		}
		else if(lDirDistance < 0)
		{
			lCP = lPoL[i];
			lDistance = pCircle.GetPosition().GetDistance(lCP);
		}

		if(i == 0 || lDistance < lMinDistance)
		{
			lMinDistance = lDistance;
			lClosestPoint = lCP;
		}
	}

	Vector2D<_TVarType> lNormal(pCircle.GetPosition() - lClosestPoint);
	lNormal /= lMinDistance; // Normalize.

	_TVarType lBoxProjection = abs(lNormal.Dot(pBox.GetExtentX())) + abs(lNormal.Dot(pBox.GetExtentY()));
	_TVarType lDistProjection = lNormal.Dot(lDist);
	_TVarType lAbsDistProjection = abs(lDistProjection);
	if(lAbsDistProjection > lBoxProjection + pCircle.GetRadius())
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mTimeToCollision = 0;
		if(lDistProjection > 0)
			lNormal *= -1;
		pCollisionInfo->mNormal = lNormal;
		pCollisionInfo->mSeparationDistance = lBoxProjection + pCircle.GetRadius() - lAbsDistProjection;
		pCollisionInfo->mPointOfCollision = pCircle.GetPosition() + lNormal * (pCircle.GetRadius() - pCollisionInfo->mSeparationDistance * (_TVarType)0.5);
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToLineSegmentTest(const OBR<_TVarType>& pBox, 
							        const Vector2D<_TVarType>& pStart,
							        const Vector2D<_TVarType>& pEnd,
							        CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lExtentX(pBox.GetExtentX());
	Vector2D<_TVarType> lExtentY(pBox.GetExtentY());
	Vector2D<_TVarType> lDir(pEnd - pStart);
	Vector2D<_TVarType> lNormal(lDir.y, -lDir.x);
	lNormal.Normalize();

	lDir *= (_TVarType)0.5;
	Vector2D<_TVarType> lLineCenter(pStart + lDir);
	Vector2D<_TVarType> lDist(pBox.GetPosition() - lLineCenter);

	_TVarType lDistProjection = lNormal.Dot(lDist);
	_TVarType lBoxProjection = abs(lNormal.Dot(lExtentX)) + abs(lNormal.Dot(lExtentY));

	if(abs(lDistProjection) > abs(lBoxProjection))
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		if(lDistProjection > 0)
		{
			pCollisionInfo->mNormal = lNormal;
			pCollisionInfo->mSeparationDistance = abs(lBoxProjection) - lDistProjection;
		}
		else
		{
			pCollisionInfo->mNormal = -lNormal;
			pCollisionInfo->mSeparationDistance = abs(lBoxProjection) + lDistProjection;
		}
	}

	lNormal = lExtentX;
	lNormal.Normalize();

	lDistProjection = lNormal.Dot(lDist);
	lBoxProjection = pBox.GetSize().x;
	_TVarType lLineProjection = lNormal.Dot(lDir);
	_TVarType lLineBoxProjection = abs(lLineProjection) + lBoxProjection;

	if(abs(lDistProjection) > lLineBoxProjection)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		_TVarType lSeparationDistance = lLineBoxProjection - abs(lDistProjection);
		if(lSeparationDistance < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lSeparationDistance;
			if(lDistProjection > 0)
				pCollisionInfo->mNormal = lNormal;
			else
				pCollisionInfo->mNormal = -lNormal;
		}
	}

	lNormal = lExtentY;
	lNormal.Normalize();

	lDistProjection = lNormal.Dot(lDist);
	lBoxProjection = pBox.GetSize().y;
	lLineProjection = lNormal.Dot(lDir);
	lLineBoxProjection = abs(lLineProjection) + lBoxProjection;

	if(abs(lDistProjection) > lLineBoxProjection)
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}
	else if(pCollisionInfo != 0)
	{
		_TVarType lSeparationDistance = lLineBoxProjection - abs(lDistProjection);
		if(lSeparationDistance < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lSeparationDistance;
			if(lDistProjection > 0)
				pCollisionInfo->mNormal = lNormal;
			else
				pCollisionInfo->mNormal = -lNormal;
		}

		pCollisionInfo->mTimeToCollision = 0;

		// All code below calculates the point of collision.
		if(mPOCEnabled)
		{
			Vector2D<_TVarType> lCorner1 = pBox.GetPosition() + lExtentX + lExtentY;
			Vector2D<_TVarType> lCorner2 = pBox.GetPosition() + lExtentX - lExtentY;
			Vector2D<_TVarType> lCorner3 = pBox.GetPosition() - lExtentX - lExtentY;
			Vector2D<_TVarType> lCorner4 = pBox.GetPosition() - lExtentX + lExtentY;

			CollisionInfo lCInfo[4];
			int lIndex1 = -1;
			int lIndex2 = -1;
			if(StaticLineSegmentToLineSegmentTest(pStart, pEnd, lCorner1, lCorner2, &lCInfo[0]))
			{
				lIndex1 = 0;
			}
			if(StaticLineSegmentToLineSegmentTest(pStart, pEnd, lCorner2, lCorner3, &lCInfo[1]))
			{
				if(lIndex1 < 0)
					lIndex1 = 1;
				else
					lIndex2 = 1;
			}
			if((lIndex1 < 0 || lIndex2 < 0) && StaticLineSegmentToLineSegmentTest(pStart, pEnd, lCorner3, lCorner4, &lCInfo[2]))
			{
				if(lIndex1 < 0)
					lIndex1 = 2;
				else
					lIndex2 = 2;
			}
			if((lIndex1 < 0 || lIndex2 < 0) && StaticLineSegmentToLineSegmentTest(pStart, pEnd, lCorner4, lCorner1, &lCInfo[3]))
			{
				if(lIndex1 < 0)
					lIndex1 = 3;
				else
					lIndex2 = 3;
			}

			if(lIndex2 < 0)
			{
				// TODO: Fix. This is not following the definition. It should be
				// (lCInfo[lIndex1].mPointOfCollision + pStart) * 0.5 or
				// (lCInfo[lIndex1].mPointOfCollision + pEnd) * 0.5.
				// The problem is just to figure out which one.
				pCollisionInfo->mPointOfCollision = lCInfo[lIndex1].mPointOfCollision;
			}
			else
			{
				pCollisionInfo->mPointOfCollision = (lCInfo[lIndex1].mPointOfCollision + lCInfo[lIndex2].mPointOfCollision) * (_TVarType)0.5;
			}
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToTriangleTest(const OBR<_TVarType>& pBox, 
							     const Vector2D<_TVarType> pTri[3], 
							     CollisionInfo* pCollisionInfo)
{
	if(pCollisionInfo != 0)
		pCollisionInfo->mTimeToCollision = 1;

	 // Find out wether the triangle is clockwise.
	Vector2D<_TVarType> lPerp((pTri[1] - pTri[0]).GetPerpCW());
	_TVarType lP = lPerp.Dot(pTri[2] - pTri[0]);

	// Copy triangle data and make sure it is stored in clockwise order.
	Vector2D<_TVarType> lTri[3];

	if(lP >= 0)
	{
		lTri[0] = pTri[0];
		lTri[1] = pTri[1];
		lTri[2] = pTri[2];
	}
	else
	{
		lTri[0] = pTri[0];
		lTri[1] = pTri[2];
		lTri[2] = pTri[1];
	}

	// Find the corners of the box, store them in clockwise order.
	Vector2D<_TVarType> lExtentX(pBox.GetExtentX());
	Vector2D<_TVarType> lExtentY(pBox.GetExtentY());
	Vector2D<_TVarType> lBox[4];
	lBox[0] = pBox.GetPosition() - lExtentX - lExtentY;
	lBox[1] = pBox.GetPosition() - lExtentX + lExtentY;
	lBox[2] = pBox.GetPosition() + lExtentX + lExtentY;
	lBox[3] = pBox.GetPosition() + lExtentX - lExtentY;


	// We can't use the regular technique of separating "planes" (lines) as usual, 
	// since the triangles are not (in general) symmetric along any axis. Instead,
	// the separating line has to be fully defined (as opposed to only knowing the
	// slope). Each side of the triangle and the OBR defines one such line, against 
	// which we have to clip the other shape before applying the method of separating
	// lines.

	// Now find the overlap.
	Vector2D<_TVarType> lOut[7];
	Vector2D<_TVarType> lEdge(lTri[1] - lTri[0]);
	int lVCount;
	if((lVCount = ClipPolyAgainstLine(lBox, 4, lTri[0], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri[0]));
		pCollisionInfo->mSeparationDistance = lProjection;
		pCollisionInfo->mNormal = -lN;
	}

	lEdge = lTri[2] - lTri[1];
	if((lVCount = ClipPolyAgainstLine(lBox, 4, lTri[1], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri[1]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = -lN;
		}
	}

	lEdge = lTri[0] - lTri[2];
	if((lVCount = ClipPolyAgainstLine(lBox, 4, lTri[2], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lTri[2]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = -lN;
		}
	}

	lEdge = lBox[1] - lBox[0];
	if((lVCount = ClipPolyAgainstLine(lTri, 3, lBox[0], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lBox[0]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = lN;
		}
	}

	lEdge = lBox[2] - lBox[1];
	if((lVCount = ClipPolyAgainstLine(lTri, 3, lBox[1], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lBox[1]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = lN;
		}
	}

	lEdge = lBox[3] - lBox[2];
	if((lVCount = ClipPolyAgainstLine(lTri, 3, lBox[2], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lBox[2]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = lN;
		}
	}

	lEdge = lBox[0] - lBox[3];
	if((lVCount = ClipPolyAgainstLine(lTri, 3, lBox[3], lEdge, lOut)) == 0)
		return false;
	if(pCollisionInfo != 0)
	{
		Vector2D<_TVarType> lN(lEdge.y, -lEdge.x);
		lN.Normalize();
		_TVarType lProjection = 0;
		for(int i = 0; i < lVCount; i++)
			lProjection += abs(lN.Dot(lOut[i] - lBox[3]));
		if(lProjection < pCollisionInfo->mSeparationDistance)
		{
			pCollisionInfo->mSeparationDistance = lProjection;
			pCollisionInfo->mNormal = lN;
		}

		pCollisionInfo->mTimeToCollision = 0;

		if(mPOCEnabled)
		{
			// Now we have to find the overlap in order to find the point of collision.
			Vector2D<_TVarType> lTemp[7];
			int lVCount;
			lVCount = ClipPolyAgainstLine(lBox, 4, lTri[0], lTri[1] - lTri[0], lOut);
			lVCount = ClipPolyAgainstLine(lOut, lVCount, lTri[1], lTri[2] - lTri[1], lTemp);
			lVCount = ClipPolyAgainstLine(lTemp, lVCount, lTri[2], lTri[0] - lTri[2], lOut);

			pCollisionInfo->mPointOfCollision.Set(0, 0);
			int i;
			for(i = 0; i < lVCount; i++)
			{
				pCollisionInfo->mPointOfCollision += lOut[i];
			}
			pCollisionInfo->mPointOfCollision /= (_TVarType)lVCount;
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticCircleToPointTest(const Circle<_TVarType>& pCircle, 
							     const Vector2D<_TVarType>& pPoint, 
							     CollisionInfo* pCollisionInfo)
{
	_TVarType lDistanceSq = pCircle.GetPosition().GetDistanceSquared(pPoint);
	if(lDistanceSq > pCircle.GetRadiusSquared())
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 0;
		return false;
	}
	
	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mTimeToCollision = 1;
		pCollisionInfo->mPointOfCollision = pPoint;
		pCollisionInfo->mNormal = pCircle.GetPosition() - pPoint;
		_TVarType lDistance = pCollisionInfo->mNormal.GetLength();
		pCollisionInfo->mSeparationDistance = pCircle.GetRadius() - lDistance;

		if(lDistance == 0)
			pCollisionInfo->mNormal.Set(1, 0);
		else
			pCollisionInfo->mNormal /= lDistance; // Normalize.
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticCircleToLineSegmentTest(const Circle<_TVarType>& pCircle,
								   const Vector2D<_TVarType>& pStart,
								   const Vector2D<_TVarType>& pEnd,
								   CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lDir(pEnd - pStart);
	Vector2D<_TVarType> lClosestPoint;
	_TVarType lStartToCPDist;
	_TVarType lDistance = LineDistance(pStart, lDir, pCircle.GetPosition(), lClosestPoint, lStartToCPDist);

	if(lDistance > pCircle.GetRadius())
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	bool lCheckAgainstCP = false;
	_TVarType lLineLength = lDir.GetLength();
	if(lStartToCPDist < 0)
	{
		lClosestPoint = pStart;
		lCheckAgainstCP = true;
	}
	else if(lStartToCPDist > lLineLength)
	{
		lClosestPoint = pEnd;
		lCheckAgainstCP = true;
	}

	if(lCheckAgainstCP == true)
	{
		lDistance = pCircle.GetPosition().GetDistance(lClosestPoint);

		if(lDistance > pCircle.GetRadius())
		{
			if(pCollisionInfo != 0)
				pCollisionInfo->mTimeToCollision = 1;
			return false;
		}
	}

	if(pCollisionInfo != 0)
	{
		pCollisionInfo->mTimeToCollision = 0;
		pCollisionInfo->mPointOfCollision = lClosestPoint;
		pCollisionInfo->mNormal = pCircle.GetPosition() - lClosestPoint;
		pCollisionInfo->mNormal.Normalize();
		pCollisionInfo->mSeparationDistance = pCircle.GetRadius() - lDistance;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticCircleToTriangleTest(const Circle<_TVarType>& pCircle, 
								const Vector2D<_TVarType> pTri[3], 
								CollisionInfo* pCollisionInfo)
{
	Vector2D<_TVarType> lCP; // Closest point.
	Vector2D<_TVarType> lNormal;
	_TVarType lProjection;
	_TVarType lDistance = TriangleDistance(pTri, pCircle.GetPosition(), lCP, lNormal, lProjection);

	if(lDistance > pCircle.GetRadius())
	{
		if(pCollisionInfo != 0)
			pCollisionInfo->mTimeToCollision = 1;
		return false;
	}

	if(pCollisionInfo != 0)
	{
		if(lDistance == 0)
		{
			pCollisionInfo->mNormal = lNormal;
			pCollisionInfo->mSeparationDistance = lProjection + pCircle.GetRadius();
		}
		else
		{
			pCollisionInfo->mNormal = pCircle.GetPosition() - lCP;
			pCollisionInfo->mNormal.Normalize();
			pCollisionInfo->mSeparationDistance = pCircle.GetRadius() - lDistance;
		}

		pCollisionInfo->mTimeToCollision = 0;
		pCollisionInfo->mPointOfCollision = lCP;
	}

	return true;
}

template<class _TVarType>
int CollisionDetector2D<_TVarType>::ClipPolyAgainstLine(Vector2D<_TVarType>* pPoly, int pVertexCount,
							const Vector2D<_TVarType>& pPointOnLine,
							const Vector2D<_TVarType>& pDir,
							Vector2D<_TVarType>* pPolyOut)
{
	Vector2D<_TVarType> lNormal(pDir.y, -pDir.x);
	lNormal.Normalize();
	_TVarType lC = -lNormal.Dot(pPointOnLine);

	int lOutVCount = 0;
	int lStart = pVertexCount - 1;
	int lEnd = 0;
	while(lEnd < pVertexCount)
	{
		_TVarType lS = lNormal.Dot(pPoly[lStart]) + lC;
		_TVarType lE = lNormal.Dot(pPoly[lEnd]) + lC;

		if(lS <= 0)
		{
			if(lE > 0)
			{
				// Exiting the clipping plane. Start point clipped, store start point.
				const _TVarType t = -lS / (lE - lS);
				pPolyOut[lOutVCount++] = pPoly[lStart] + (pPoly[lEnd] - pPoly[lStart]) * t;
			}
		}
		else
		{
			if (lE <= 0)
			{
				// Entering the clipping plane. Store both points. The first point is just copied.
				pPolyOut[lOutVCount++] = pPoly[lStart];
				const _TVarType t = -lE / (lS - lE);
				pPolyOut[lOutVCount++] = pPoly[lEnd] + (pPoly[lStart] - pPoly[lEnd]) * t;
			}
			else
			{
				// Both points are visible, store the start point.
				pPolyOut[lOutVCount++] = pPoly[lStart];
			}
		}
		lStart = lEnd;
		lEnd++;
	}

	return lOutVCount;
}

template<class _TVarType>
_TVarType CollisionDetector2D<_TVarType>::TriangleDistance(const Vector2D<_TVarType> pTri[3], 
							   const Vector2D<_TVarType>& pPoint,
							   Vector2D<_TVarType>& pClosestPoint,
							   Vector2D<_TVarType>& pNormal,
							   _TVarType& pProjection)
{
	Vector2D<_TVarType> lDir0(pTri[1] - pTri[0]);
	Vector2D<_TVarType> lDir1(pTri[2] - pTri[1]);
	Vector2D<_TVarType> lDir2(pTri[0] - pTri[2]);

	// Relative Position.
	Vector2D<_TVarType> lRP0(pPoint - pTri[0]);
	Vector2D<_TVarType> lRP1(pPoint - pTri[1]);
	Vector2D<_TVarType> lRP2(pPoint - pTri[2]);

	Vector2D<_TVarType> lNormal0(lDir0.y, -lDir0.x);
	Vector2D<_TVarType> lNormal1(lDir1.y, -lDir1.x);
	Vector2D<_TVarType> lNormal2(lDir2.y, -lDir2.x);

	_TVarType lNP0 = lNormal0.Dot(lRP0);
	_TVarType lNP1 = lNormal1.Dot(lRP1);
	_TVarType lNP2 = lNormal2.Dot(lRP2);

	if((lNP0 <= 0 && lNP1 <= 0 && lNP2 <= 0) || (lNP0 >= 0 && lNP1 >= 0 && lNP2 >= 0))
	{
		// Inside the triangle.
		pClosestPoint = pPoint;

		pProjection = abs(lNP0);
		pNormal = lNormal0;
		_TVarType lAbsNP1 = abs(lNP1);
		_TVarType lAbsNP2 = abs(lNP2);
		if(lAbsNP1 < pProjection)
		{
			pProjection = lAbsNP1;
			pNormal = lNormal1;
		}
		if(lAbsNP2 < pProjection)
		{
			pProjection = lAbsNP2;
			pNormal = lNormal2;
		}

		_TVarType lLength = pNormal.GetLength();
		pNormal /= lLength;
		pProjection /= lLength;

		if(lNP0 > 0)
		{
			pNormal = -pNormal;
		}

		return 0;
	}

	_TVarType lL0 = lDir0.GetLength();
	_TVarType lL1 = lDir1.GetLength();
	_TVarType lL2 = lDir2.GetLength();

	// Normalize.
	lDir0 /= lL0;
	lDir1 /= lL1;
	lDir2 /= lL2;

	_TVarType lDP0 = lDir0.Dot(lRP0);
	_TVarType lDP1 = lDir1.Dot(lRP1);
	_TVarType lDP2 = lDir2.Dot(lRP2);

	Vector2D<_TVarType> lCP0;
	Vector2D<_TVarType> lCP1;
	Vector2D<_TVarType> lCP2;
	if(lDP0 < 0)
		lCP0 = pTri[0];
	else if(lDP0 > lL0)
		lCP0 = pTri[1];
	else
		lCP0 = pTri[0] + lDir0 * lDP0;

	if(lDP1 < 0)
		lCP1 = pTri[1];
	else if(lDP1 > lL1)
		lCP1 = pTri[2];
	else
		lCP1 = pTri[1] + lDir1 * lDP1;

	if(lDP2 < 0)
		lCP2 = pTri[2];
	else if(lDP2 > lL2)
		lCP2 = pTri[0];
	else
		lCP2 = pTri[2] + lDir2 * lDP2;

	_TVarType lMinDistance = lCP0.GetDistance(pPoint);
	pClosestPoint = lCP0;

	_TVarType lDistance = lCP1.GetDistance(pPoint);
	if(lDistance < lMinDistance)
	{
		lMinDistance = lDistance;
		pClosestPoint = lCP1;
	}

	lDistance = lCP2.GetDistance(pPoint);
	if(lDistance < lMinDistance)
	{
		lMinDistance = lDistance;
		pClosestPoint = lCP2;
	}

	return lMinDistance;
}

/*
namespace Collision
{

	bool CollisionDetector2D<_TVarType>::StaticAABRToTriangleTest(const AABR2D& pBox, const Triangle2D& pTriangle, Vector2D& pSeparation)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::StaticOBRToTriangleTest(const OBR2D& pBox, const Triangle2D& pTriangle, Vector2D& pSeparation)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::StaticCircleToTriangleTest(const Circle& pCircle, const Triangle2D& pTriangle, Vector2D& pSeparation)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::StaticTriangleToPointTest(const Triangle2D& pTriangle, const Vector2D& pPoint, Vector2D& pSeparation)
	{
		return false;
	}


	bool CollisionDetector2D<_TVarType>::VelocityAABRToPointTest(const AABR2D& pBox, 
								 const Vector2D& pPoint,
								 const Vector2D& pBoxVelocity,
								 const Vector2D& pPointVelocity,
								 _TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityAABRToOBRTest(const AABR2D& pBox1, 
							   const OBR2D& pBox2,
  							   const Vector2D& pBox1Velocity,
							   const Vector2D& pBox2Velocity,
							   _TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityAABRToCircleTest(const AABR2D& pBox, 
								  const Circle& pCircle,
  								  const Vector2D& pBoxVelocity,
								  const Vector2D& pCircleVelocity,
								  _TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityAABRToTriangleTest(const AABR2D& pBox, 
									const Triangle2D& pTriangle,
  									const Vector2D& pBoxVelocity,
									const Vector2D& pTriangleVelocity,
									_TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityOBRToPointTest(const OBR2D& pBox,
								const Vector2D& pPoint,
								const Vector2D& pBoxVelocity,
								const Vector2D& pPointVelocity,
								_TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityOBRToCircleTest(const OBR2D& pBox,
								 const Circle& pCircle,
								 const Vector2D& pBoxVelocity,
								 const Vector2D& pCircleVelocity,
								 _TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityOBRToTriangleTest(const OBR2D& pBox, 
								   const Triangle2D& pTriangle,
								   const Vector2D& pBoxVelocity,
								   const Vector2D& pTriangleVelocity,
								   _TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityCircleToPointTest(const Circle& pCircle, 
								   const Vector2D& pPoint,
								   const Vector2D& pCircleVelocity,
								   const Vector2D& pPointVelocity,
								   _TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityCircleToTriangleTest(const Circle& pCircle, 
									  const Triangle2D& pTriangle,
									  const Vector2D& pCircleVelocity,
									  const Vector2D& pTriangleVelocity,
									  _TVarType& pTimeToCollision)
	{
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityTriangleToPointTest(const Triangle2D& pTriangle, 
									 const Vector2D& pPoint,
									 const Vector2D& pTriangleVelocity,
									 const Vector2D& pPointVelocity,
									 _TVarType& pTimeToCollision)
	{
		return false;
	}

}
*/
