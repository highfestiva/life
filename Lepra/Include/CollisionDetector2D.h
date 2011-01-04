/*
	Class:  CollisionDetector2D
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	A class used to test collisions between 2D geometrical shapes.
	AABR = Axis Aligned Bounding Rectangle
	OBR = Oriented Bounding Rectangle
*/

#ifndef LEPRA_COLLISIONDETECTOR2D_H
#define LEPRA_COLLISIONDETECTOR2D_H

#include "Vector2D.h"
#include "AABR.h"
#include "OBR.h"
#include "Circle.h"

namespace Lepra
{

template<class _TVarType>
class CollisionDetector2D
{
public:
	struct CollisionInfo
	{
		CollisionInfo() : 
			mSeparationDistance(0),
			mTimeToCollision(0)
		{
		}

		// This is either a true point, or it represents the center of the 
		// contact line or area.
		Vector2D<_TVarType> mPointOfCollision;

		// The contact surface normal, relative to the first object. With
		// other words, moving the first object in the direction of the
		// normal "mSeparationDistance" units away will separate the objects.
		Vector2D<_TVarType> mNormal;

		// The penetration depth, or the distance along the normal which will
		// separate the two objects. This is always a nonnegative number.
		_TVarType mSeparationDistance;

		// 0 < mTimeToCollision < 1 for velocity tests. In static tests,
		// mTimeToCollision = 0 if there is a collision and 1 if not.
		_TVarType mTimeToCollision;
	};

	CollisionDetector2D() :
		mPOCEnabled(true)
	{
	}

	// Calculating the point of collision can be very time consuming in come cases.
	// Disabling it can improve performance, if the point of collision is of no interest.
	void SetPointOfCollisionEnabled(bool pEnabled) { mPOCEnabled = pEnabled; }

	// Enclosing tests, mainly used in combination with LooseQuadtree.
	bool IsAABR1EnclosingAABR2(const AABR<_TVarType>& pBox1, const AABR<_TVarType>& pBox2);
	bool IsAABREnclosingCircle(const AABR<_TVarType>& pBox, const Circle<_TVarType>& pCircle);
	bool IsCircle1EnclosingCircle2(const Circle<_TVarType>& pCircle1, const Circle<_TVarType>& pCircle2);
	bool IsCircleEnclosingAABR(const Circle<_TVarType>& pCircle, const AABR<_TVarType>& pBox);

	// All methods below returns true if a collision occurs, and false otherwise.

	bool StaticLineSegmentToLineSegmentTest(const Vector2D<_TVarType>& pStart1, const Vector2D<_TVarType>& pEnd1,
	                                        const Vector2D<_TVarType>& pStart2, const Vector2D<_TVarType>& pEnd2,
						CollisionInfo* pCollisionInfo = 0);

	// Test if one AABR collides with another. 
	bool StaticAABRToAABRTest(const AABR<_TVarType>& pBox1, 
	                          const AABR<_TVarType>& pBox2, 
	                          CollisionInfo* pCollisionInfo = 0);

	// Test if one AABR will collide with another while moving along a given vector.
	// Also returns the time to the collision as a value between 0.0 and 1.0.
	bool VelocityAABRToAABRTest(const AABR<_TVarType>& pBox1,
	                            const AABR<_TVarType>& pBox2,
	                            const Vector2D<_TVarType>& pBox1Velocity,
	                            const Vector2D<_TVarType>& pBox2Velocity,
	                            CollisionInfo* pCollisionInfo = 0);

	// Test if one OBR collides with another. 
	bool StaticOBRToOBRTest(const OBR<_TVarType>& pBox1, const OBR<_TVarType>& pBox2, 
	                        CollisionInfo* pCollisionInfo = 0);
/*
	// Test if one OBR will collide with another while moving along a given vector.
	// Also returns the time to the collision as a value between 0.0 and 1.0.
	bool VelocityOBRToOBRTest(const OBR<_TVarType>& pBox1,
	                          const OBR<_TVarType>& pBox2,
	                          const Vector2D<_TVarType>& pBox1Velocity,
	                          const Vector2D<_TVarType>& pBox2Velocity,
	                          CollisionInfo* pCollisionInfo = 0);
*/
	// Test if one circle collides with another.
	bool StaticCircleToCircleTest(const Circle<_TVarType>& pCircle1, 
	                              const Circle<_TVarType>& pCircle2,
				      CollisionInfo* pCollisionInfo = 0);

	// Test if one triangle collides with another.
	bool StaticTriangleToTriangleTest(const Vector2D<_TVarType> pTri1[3], 
					  const Vector2D<_TVarType> pTri2[3],
					  CollisionInfo* pCollisionInfo = 0);


	// Returns 'true' if the point is to the left of the line segment as 
	// seen from pStart to pEnd AND if it is BETWEEN the two perpendicular
	// lines that go through pStart and pEnd respectively. This differs
	// from other line segment tests.
	bool StaticPointLeftOfLineSegmentTest(const Vector2D<_TVarType>& pPoint,
	                                      const Vector2D<_TVarType>& pStart, 
	                                      const Vector2D<_TVarType>& pEnd,
	                                      CollisionInfo* pCollisionInfo = 0);

	// Returns 'true' if the point is to the left of the line as seen in
	// the direction defined by pDir.
	bool StaticPointLeftOfLineTest(const Vector2D<_TVarType>& pPoint,
	                               const Vector2D<_TVarType>& pPointOnLine, 
	                               const Vector2D<_TVarType>& pDir,
	                               CollisionInfo* pCollisionInfo = 0);

	bool StaticPointToTriangleTest(const Vector2D<_TVarType>& pPoint,
	                               const Vector2D<_TVarType> pTri[3], 
	                               CollisionInfo* pCollisionInfo = 0);


	// Combinations of geometric shapes.
	bool StaticAABRToPointTest(const AABR<_TVarType>& pBox, 
	                           const Vector2D<_TVarType>& pPoint, 
				   CollisionInfo* pCollisionInfo = 0);
	bool StaticAABRToOBRTest(const AABR<_TVarType>& pBox1, const OBR<_TVarType>& pBox2, 
	                         CollisionInfo* pCollisionInfo = 0);
	bool StaticAABRToCircleTest(const AABR<_TVarType>& pBox, 
	                            const Circle<_TVarType>& pCircle, 
				    CollisionInfo* pCollisionInfo = 0);
	bool StaticAABRToLineSegmentTest(const AABR<_TVarType>& pBox, 
	                                 const Vector2D<_TVarType>& pStart,
	                                 const Vector2D<_TVarType>& pEnd,
	                                 CollisionInfo* pCollisionInfo = 0);
	bool StaticAABRToTriangleTest(const AABR<_TVarType>& pBox, 
	                              const Vector2D<_TVarType> pTri[3], 
				      CollisionInfo* pCollisionInfo = 0);

	bool StaticOBRToPointTest(const OBR<_TVarType>& pBox, 
	                          const Vector2D<_TVarType>& pPoint, 
				  CollisionInfo* pCollisionInfo = 0);
	bool StaticOBRToCircleTest(const OBR<_TVarType>& pBox, 
	                           const Circle<_TVarType>& pCircle, 
				   CollisionInfo* pCollisionInfo = 0);
	bool StaticOBRToLineSegmentTest(const OBR<_TVarType>& pBox, 
	                                const Vector2D<_TVarType>& pStart,
	                                const Vector2D<_TVarType>& pEnd,
	                                CollisionInfo* pCollisionInfo = 0);
	bool StaticOBRToTriangleTest(const OBR<_TVarType>& pBox, 
	                             const Vector2D<_TVarType> pTri[3], 
				     CollisionInfo* pCollisionInfo = 0);

	bool StaticCircleToPointTest(const Circle<_TVarType>& pCircle, 
	                             const Vector2D<_TVarType>& pPoint, 
				     CollisionInfo* pCollisionInfo = 0);
	bool StaticCircleToLineSegmentTest(const Circle<_TVarType>& pCircle,
	                                   const Vector2D<_TVarType>& pStart,
	                                   const Vector2D<_TVarType>& pEnd,
					   CollisionInfo* pCollisionInfo = 0);
	bool StaticCircleToTriangleTest(const Circle<_TVarType>& pCircle, 
	                                const Vector2D<_TVarType> pTri[3], 
					CollisionInfo* pCollisionInfo = 0);

private:

	// Takes a convex polygon defined by pPoly and pVertexCount,
	// where the vertices has to be stored as in a triangle fan,
	// and a line defined by pPointOnLine and pDir, and returns
	// the clipped polygon in pPolyOut. The return value is the number
	// of vertices in pPolyOut, and can at most be pVertexCount + 1.
	// The part of the polygon that overlaps the left side of the
	// line will be removed.
	int ClipPolyAgainstLine(Vector2D<_TVarType>* pPoly, int pVertexCount,
				const Vector2D<_TVarType>& pPointOnLine,
				const Vector2D<_TVarType>& pDir,
				Vector2D<_TVarType>* pPolyOut);

	_TVarType TriangleDistance(const Vector2D<_TVarType> pTri[3], 
	                           const Vector2D<_TVarType>& pPoint,
			           Vector2D<_TVarType>& pClosestPoint,
				   Vector2D<_TVarType>& pNormal,
				   _TVarType& pProjection);

	bool mPOCEnabled;
};

#include "CollisionDetector2D.inl"

} // End namespace.

#endif
