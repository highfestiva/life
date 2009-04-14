/*
	Class:  CollisionDetector3D
	Author: Alexander Hugestrand
	Copyright (c) 2002-2005, Alexander Hugestrand

	NOTES:
	
	AABB = Axis Aligned Bounding Box
	OBB = Oriented Bounding Box
*/


#ifndef LEPRA_COLLISIONDETECTOR3D_H
#define LEPRA_COLLISIONDETECTOR3D_H

#include "Math.h"
#include "Vector3D.h"
#include "RotationMatrix.h"
#include "Sphere.h"
#include "AABB.h"
#include "OBB.h"
#include "Cylinder.h"

namespace Lepra
{

template<class _TVarType>
class OBBCollisionData;

template<class _TVarType>
class CollisionDetector3D
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
		Vector3D<_TVarType> mPointOfCollision;

		// The contact surface normal, relative to the first object. With
		// other words, moving the first object in the direction of the
		// normal "mSeparationDistance" units away will separate the objects.
		Vector3D<_TVarType> mNormal;

		// The penetration depth, or the distance along the normal which will
		// separate the two objects. This is always a nonnegative number.
		_TVarType mSeparationDistance;

		// 0 < mTimeToCollision < 1 for velocity tests. In static tests,
		// mTimeToCollision = 0 if there is a collision and 1 if not.
		_TVarType mTimeToCollision;
	};

	friend class OBBCollisionData<_TVarType>;

	CollisionDetector3D();
	virtual ~CollisionDetector3D();

	//
	// Point tests.
	//

	bool IsSphereEnclosingPoint(const Sphere<_TVarType>& pSphere, 
	                            const Vector3D<_TVarType>& pPoint);
	bool IsAABBEnclosingPoint(const AABB<_TVarType>& pAABB,
	                          const Vector3D<_TVarType>& pPoint);
	bool IsOBBEnclosingPoint(const OBB<_TVarType>& pOBB,
	                         const Vector3D<_TVarType>& pPoint);

	// Cylinders aligned along the X, Y and Z-axis. Ignores the orientation
	// of the cylinder. AA = Axis Aligned.
	bool IsAACylinderEnclosingPoint(const Cylinder<_TVarType>& pCylinder,
	                                const Vector3D<_TVarType>& pPoint);

	// Oriented cylinder.
	bool IsCylinderEnclosingPoint(const Cylinder<_TVarType>& pCylinder,
	                              const Vector3D<_TVarType>& pPoint);

	//
	// Enclosing tests. Can be used in combination with LooseOctree.
	//
	bool IsSphere1EnclosingSphere2(const Sphere<_TVarType>& pSphere1,
	                               const Sphere<_TVarType>& pSphere2);
	bool IsAABB1EnclosingAABB2(const AABB<_TVarType>& pAABB1,
	                           const AABB<_TVarType>& pAABB2);
	bool IsOBB1EnclosingOBB2(const OBB<_TVarType>& pOBB1,
	                         const OBB<_TVarType>& pOBB2);
	// Both cylinders must have the same alignment.
	bool IsAACylinder1EnclosingAACylinder2(const Cylinder<_TVarType>& pCylinder1,
	                                       const Cylinder<_TVarType>& pCylinder2);

	// TODO: Implement enclosing tests with oriented cylinders.

	bool IsSphereEnclosingAABB(const Sphere<_TVarType>& pSphere,
	                           const AABB<_TVarType>& pAABB);
	bool IsAABBEnclosingSphere(const AABB<_TVarType>& pAABB,
	                           const Sphere<_TVarType>& pSphere);
	bool IsSphereEnclosingOBB(const Sphere<_TVarType>& pSphere,
	                          const OBB<_TVarType>& pOBB);
	bool IsOBBEnclosingSphere(const OBB<_TVarType>& pOBB,
	                          const Sphere<_TVarType>& pSphere);
	bool IsAABBEnclosingOBB(const AABB<_TVarType>& pAABB,
	                        const OBB<_TVarType>& pOBB);
	bool IsOBBEnclosingAABB(const OBB<_TVarType>& pOBB,
	                        const AABB<_TVarType>& pAABB);

	// TODO: Implement more enclosing tests using cylinders.

	//
	// Overlapping tests.
	//

	bool IsAABBOverlappingAABB(const AABB<_TVarType>& pAABB1,
	                           const AABB<_TVarType>& pAABB2,
				   CollisionInfo* pCollisionInfo = 0);
	// TODO: The following function does not calculate point of collision. Fix!?
	bool IsOBBOverlappingOBB(const OBB<_TVarType>& pOBB1,
	                         const OBB<_TVarType>& pOBB2,
				 CollisionInfo* pCollisionInfo = 0);
	bool IsSphereOverlappingSphere(const Sphere<_TVarType>& pSphere1,
	                               const Sphere<_TVarType>& pSphere2,
				       CollisionInfo* pCollisionInfo = 0);
	bool IsAABBOverlappingOBB(const AABB<_TVarType>& pAABB,
	                          const OBB<_TVarType>& pOBB,
				  CollisionInfo* pCollisionInfo = 0);
	bool IsAABBOverlappingSphere(const AABB<_TVarType>& pAABB,
	                             const Sphere<_TVarType>& pSphere,
				     CollisionInfo* pCollisionInfo = 0);
	bool IsOBBOverlappingSphere(const OBB<_TVarType>& pOBB,
	                            const Sphere<_TVarType>& pSphere,
				    CollisionInfo* pCollisionInfo = 0);

	// And now for the most complex function of them all. OBB to OBB collision test
	// with velocity. This function will calculate the exact time of collision (TTC)
	// in seconds. 0 means that the boxes are already in collision from the start.
	// Using this function in your collision detection will guarantee that nothing will 
	// ever move through anything else.
	// The CollisionInfo parameter is mandatory in this function.

	bool AreMovingOBBsColliding(_TVarType pTimeDelta, // The elapsed time in seconds.
				    const OBB<_TVarType>& pOBB1,
				    const Vector3D<_TVarType>& pOBB1Velocity,
				    const OBB<_TVarType>& pOBB2,
				    const Vector3D<_TVarType>& pOBB2Velocity,
				    CollisionInfo& pCollisionInfo);

	// Get the point, line or surface that describes the intersection area during a collision.
	// If pNumPoints is:
	// 1 = The intersection was a point.
	// 2 = The intersection was a line.
	// >2 = The intersection is an area, described as a polygon.
	//
	// This function can only be called after a call to AreMovingOBBsColliding(), and
	// it is NOT fast.
	void GetOBBToOBBIntersection(Vector3D<_TVarType> pPoint[8], int &pNumPoints);

private:
	enum AxisID
	{
		AXIS_NONE = 0,
		AXIS_X1,
		AXIS_Y1,
		AXIS_Z1,
		AXIS_X2,
		AXIS_Y2,
		AXIS_Z2,
		AXIS_X1xX2,
		AXIS_X1xY2,
		AXIS_X1xZ2,
		AXIS_Y1xX2,
		AXIS_Y1xY2,
		AXIS_Y1xZ2,
		AXIS_Z1xX2,
		AXIS_Z1xY2,
		AXIS_Z1xZ2,
		AXIS_WxX1,
		AXIS_WxY1,
		AXIS_WxZ1,
		AXIS_WxX2,
		AXIS_WxY2,
		AXIS_WxZ2
	};

	enum CollisionType
	{
		NO_COLLISION = 0,
		POINT_COLLISION,
		EDGE_COLLISION,
		SURFACE_COLLISION,
	};

	// Optimized overlapping test. Does not calculate any collision info.
	bool IsOBBOverlappingOBBOptimized(const OBB<_TVarType>& pOBB1,
	                                  const OBB<_TVarType>& pOBB2);

	// Unoptimized test that calculates collision info.
	bool IsOBBOverlappingOBB(const OBB<_TVarType>& pOBB1,
	                         const OBB<_TVarType>& pOBB2,
				 CollisionInfo& pCollisionInfo);


	CollisionType GetOBBCollisionType();
	void GetOBBPointIntersection(Vector3D<_TVarType> pPoint[8], int &pNumPoints);
	void GetOBBEdgeSurfaceIntersection(Vector3D<_TVarType> pPoint[8], int &pNumPoints, CollisionType pCollisionType);

	// Used by GetOBBEdgeSurfaceIntersection().
	void SetupBoxSide(Vector3D<_TVarType> pBoxCenter, 
			  Vector3D<_TVarType> pBoxOrientation[3], 
			  _TVarType pBoxExtents[3], 
			  int pNormalIndex,
			  Vector3D<_TVarType> pNormal,
			  int& pRetEdgeIndex1,	// One of the edges that is not parallel to the normal.
			  int& pRetEdgeIndex2,	// One of the edges that is not parallel to the normal.
			  Vector3D<_TVarType> pRetSideVertices[4]);

	int ClipVectorWithPlane(Vector3D<_TVarType> pPlanePoint, 
				Vector3D<_TVarType> pPlaneNormal,
				Vector3D<_TVarType>& pVectorPoint1, 
				Vector3D<_TVarType>& pVectorPoint2);

	bool GetTimeOfVectorPlaneIntersection(const Vector3D<_TVarType>& pPlanePoint,
					      const Vector3D<_TVarType>& pPlaneNormal,
					      const Vector3D<_TVarType>& pVectorPoint,
					      const Vector3D<_TVarType>& pVectorDirection,
					      _TVarType& pReturnTime);

	inline _TVarType PlaneEquation(const Vector3D<_TVarType>& pPlanePoint,
				       const Vector3D<_TVarType>& pPlaneNormal,
				       const Vector3D<_TVarType>& pTestPoint);

	OBBCollisionData<_TVarType>* mOBBCollisionData;
};

template<class _TVarType>
_TVarType CollisionDetector3D<_TVarType>::PlaneEquation(const Vector3D<_TVarType>& pPlanePoint, 
							const Vector3D<_TVarType>& pPlaneNormal, 
							const Vector3D<_TVarType>& pTestPoint)
{
	// Ax + By + Cz + D = 0
	// D = -(Ax + By + Cz) = -Normal dot PlanePoint;
	
	_TVarType lD = -(pPlaneNormal * pPlanePoint);
	return pPlaneNormal * pTestPoint + lD;
}

template<class _TVarType>
class OBBCollisionData
{
public:
	OBBCollisionData()
	{
		Reset();
	}

	inline void Reset()
	{
		mAxisID = CollisionDetector3D<_TVarType>::AXIS_NONE;
		mSide = 1.0f;
		mValidCollision = false;

		mBoxWithSeparatingPlane = -1;
		mBox1NormalIndex = -1;
		mBox2NormalIndex = -1;
		mBox1EdgeIndex = -1;
		mBox2EdgeIndex = -1;
		mBox1Side = 1;
		mBox2Side = 1;
	}

	bool mValidCollision;

	typename CollisionDetector3D<_TVarType>::AxisID mAxisID;
	_TVarType mSide; // -1.0f or +1.0f

	_TVarType mC[9]; // Dot products between OBB1's axes and OBB2's.
	_TVarType mAbsC[9]; // Dot products between OBB1's axes and OBB2's.

	Vector3D<_TVarType> mAxis;

	Vector3D<_TVarType> mOBB1CollisionPos;
	Vector3D<_TVarType> mOBB2CollisionPos;
	Vector3D<_TVarType> mOBB1RotAxis[3];
	Vector3D<_TVarType> mOBB2RotAxis[3];
	_TVarType mOBB1Size[3];
	_TVarType mOBB2Size[3];

	// Data used to optimize the search of the intersection area.
	int mBoxWithSeparatingPlane;	// 0 or 1
	int mBox1NormalIndex;		// 0 through 2 if Box0 is the box with the separating plane.
	int mBox2NormalIndex;		// 0 through 2 if Box1 is the box with the separating plane.
	int mBox1EdgeIndex;		// 0 through 2 if EDGE_COLLISION and Box0 has the edge.
	int mBox2EdgeIndex;		// 0 through 2 if EDGE_COLLISION and Box1 has the edge.
	_TVarType mBox1Side;			// -1 or 1
	_TVarType mBox2Side;			// -1 or 1
};

#define TSPMACRO_SNAP_TO_ZERO(x, epsilon) \
{ \
	if(x < epsilon && x > -epsilon) \
	{ \
		x = 0.0f; \
	} \
}

#define TSPMACRO_TEST_MOVING_OBB_COLLISION_0(projectedDist0, \
					     projectedDist1, \
					     projectedSizeSum, \
					     time, \
					     retTimeMax, \
					     retAxisID, \
					     retSide, \
					     currentAxisID) \
{ \
	/* Test if collision. If not, return. Else, store the maximum time to collision */ \
	/* and other data that goes with it for later reference. */ \
	/* It will be used to compute the collision surface normal. */ \
	if((projectedDist0) > (projectedSizeSum)) \
	{ \
		if((projectedDist1) > (projectedSizeSum)) \
		{ \
			retTimeMax = time; \
			return false; /* No collision */ \
		} \
 \
		_TVarType lTime = (time) * ((projectedSizeSum) - (projectedDist0)) / ((projectedDist1) - (projectedDist0)); \
 \
		if(lTime > retTimeMax) \
		{ \
			retTimeMax = lTime; \
			retAxisID = currentAxisID; \
			retSide = 1; \
		} \
	} \
	else if((projectedDist0) < -(projectedSizeSum)) \
	{ \
		if((projectedDist1) < -(projectedSizeSum)) \
		{ \
			retTimeMax = time; \
			return false; /* No collision */ \
		} \
 \
		_TVarType lTime = -(time) * ((projectedSizeSum) + (projectedDist0)) / ((projectedDist1) - (projectedDist0)); \
 \
		if(lTime > retTimeMax) \
		{ \
			retTimeMax = lTime; \
			retAxisID = currentAxisID; \
			retSide = -1; \
		} \
	} \
	/* else the collision is immediate, and time to collision = 0.0 */ \
}

#define TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projectedDist0, \
					     q0, \
					     q1, \
					     q2, \
					     q3, \
					     projectedSizeSum, \
					     time, \
					     retTimeMax, \
					     retAxisID, \
					     retSide, \
					     currentAxisID) \
{ \
	_TVarType lProjectedDist1; \
 \
	if((projectedDist0) > (projectedSizeSum)) \
	{ \
		lProjectedDist1 = (q0) * (q1) - (q2) * (q3); \
 \
		if(lProjectedDist1 > (projectedSizeSum)) \
		{ \
			retTimeMax = time; \
			return false; /* No collision */ \
		} \
 \
		_TVarType lTime = (time) * ((projectedSizeSum) - (projectedDist0)) / (lProjectedDist1 - (projectedDist0)); \
		if ( lTime > retTimeMax ) \
		{ \
			retTimeMax = lTime; \
			retAxisID = currentAxisID; \
			retSide = 1; \
		} \
	} \
	else if((projectedDist0) < -(projectedSizeSum)) \
	{ \
		lProjectedDist1 = (q0) * (q1) - (q2) * (q3); \
 \
		if(lProjectedDist1 < -(projectedSizeSum)) \
		{ \
			retTimeMax = time; \
			return false; /* No collision */ \
		} \
 \
		_TVarType lTime = -(time) * ((projectedSizeSum) + (projectedDist0)) / (lProjectedDist1 - (projectedDist0)); \
 \
		if(lTime > retTimeMax) \
		{ \
			retTimeMax = lTime; \
			retAxisID = currentAxisID; \
			retSide = -1; \
		} \
	} \
}

#define TSPMACRO_GET_SIGNED_EXTENT(signedExtent, sign0, sign1, side, c, extent) \
{ \
	/* Extent = half the size of the OBB in one dimension. */ \
	/* That means half the width, height or depth. */ \
	/* The signs and the side are giving the signedExtent its correct sign */ \
	/* when combined. */ \
	if(c > 0.0f) \
	{ \
		signedExtent = sign0##side*extent; \
	} \
	else if(c < 0.0f) \
	{ \
		signedExtent = sign1##side*extent; \
	} \
	else \
	{ \
		signedExtent = 0.0f; \
	} \
}

#define TSPMACRO_GET_CORNER_POINT(point, boxWorldAxis, boxCollisionPos, signedExtents) \
{ \
	/* The point returned is one of the corners of the box. */ \
	point = boxCollisionPos + \
		boxWorldAxis[0] * signedExtents[0] + \
		boxWorldAxis[1] * signedExtents[1] + \
		boxWorldAxis[2] * signedExtents[2]; \
}

#include "CollisionDetector3D.inl"

} // End namespace

#endif