/*
	Class:  CollisionDetector3D
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	AABB = Axis Aligned Bounding Box
	OBB = Oriented Bounding Box
*/


#pragma once

#include "math.h"
#include "vector3d.h"
#include "rotationmatrix.h"
#include "sphere.h"
#include "aabb.h"
#include "obb.h"
#include "cylinder.h"

namespace lepra {

template<class _TVarType>
class OBBCollisionData;

template<class _TVarType>
class CollisionDetector3D {
public:
	struct CollisionInfo {
		CollisionInfo() :
			separation_distance_(0),
			time_to_collision_(0) {
		}

		// This is either a true point, or it represents the center of the
		// contact line or area.
		Vector3D<_TVarType> point_of_collision_;

		// The contact surface normal, relative to the first object. With
		// other words, moving the first object in the direction of the
		// normal "mSeparationDistance" units away will separate the objects.
		Vector3D<_TVarType> normal_;

		// The penetration depth, or the distance along the normal which will
		// separate the two objects. This is always a nonnegative number.
		_TVarType separation_distance_;

		// 0 < time_to_collision_ < 1 for velocity tests. In static tests,
		// time_to_collision_ = 0 if there is a collision and 1 if not.
		_TVarType time_to_collision_;
	};

	friend class OBBCollisionData<_TVarType>;

	CollisionDetector3D();
	virtual ~CollisionDetector3D();

	//
	// Point tests.
	//

	static bool IsSphereEnclosingPoint(const Sphere<_TVarType>& sphere, const Vector3D<_TVarType>& _point);
	static bool IsAABBEnclosingPoint(const AABB<_TVarType>& aabb, const Vector3D<_TVarType>& _point);
	static bool IsOBBEnclosingPoint(const OBB<_TVarType>& obb, const Vector3D<_TVarType>& _point);

	// Cylinders aligned along the X, Y and Z-axis. Ignores the orientation
	// of the cylinder. AA = Axis Aligned.
	bool IsAACylinderEnclosingPoint(const Cylinder<_TVarType>& cylinder,
	                                const Vector3D<_TVarType>& _point);

	// Oriented cylinder.
	bool IsCylinderEnclosingPoint(const Cylinder<_TVarType>& cylinder,
	                              const Vector3D<_TVarType>& _point);

	//
	// Enclosing tests. Can be used in combination with LooseOctree.
	//
	bool IsSphere1EnclosingSphere2(const Sphere<_TVarType>& sphere1,
	                               const Sphere<_TVarType>& sphere2);
	bool IsAABB1EnclosingAABB2(const AABB<_TVarType>& aab_b1,
	                           const AABB<_TVarType>& aab_b2);
	bool IsOBB1EnclosingOBB2(const OBB<_TVarType>& ob_b1,
	                         const OBB<_TVarType>& ob_b2);
	// Both cylinders must have the same alignment.
	bool IsAACylinder1EnclosingAACylinder2(const Cylinder<_TVarType>& cylinder1,
	                                       const Cylinder<_TVarType>& cylinder2);

	// TODO: Implement enclosing tests with oriented cylinders.

	bool IsSphereEnclosingAABB(const Sphere<_TVarType>& sphere,
	                           const AABB<_TVarType>& aabb);
	bool IsAABBEnclosingSphere(const AABB<_TVarType>& aabb,
	                           const Sphere<_TVarType>& sphere);
	bool IsSphereEnclosingOBB(const Sphere<_TVarType>& sphere,
	                          const OBB<_TVarType>& obb);
	bool IsOBBEnclosingSphere(const OBB<_TVarType>& obb,
	                          const Sphere<_TVarType>& sphere);
	bool IsAABBEnclosingOBB(const AABB<_TVarType>& aabb,
	                        const OBB<_TVarType>& obb);
	bool IsOBBEnclosingAABB(const OBB<_TVarType>& obb,
	                        const AABB<_TVarType>& aabb);

	// TODO: Implement more enclosing tests using cylinders.

	//
	// Overlapping tests.
	//

	bool IsAABBOverlappingAABB(const AABB<_TVarType>& aab_b1,
	                           const AABB<_TVarType>& aab_b2,
				   CollisionInfo* collision_info = 0);
	// TODO: The following function does not calculate point of collision. Fix!?
	bool IsOBBOverlappingOBB(const OBB<_TVarType>& ob_b1,
	                         const OBB<_TVarType>& ob_b2,
				 CollisionInfo* collision_info = 0);
	bool IsSphereOverlappingSphere(const Sphere<_TVarType>& sphere1,
	                               const Sphere<_TVarType>& sphere2,
				       CollisionInfo* collision_info = 0);
	bool IsAABBOverlappingOBB(const AABB<_TVarType>& aabb,
	                          const OBB<_TVarType>& obb,
				  CollisionInfo* collision_info = 0);
	bool IsAABBOverlappingSphere(const AABB<_TVarType>& aabb,
	                             const Sphere<_TVarType>& sphere,
				     CollisionInfo* collision_info = 0);
	bool IsOBBOverlappingSphere(const OBB<_TVarType>& obb,
	                            const Sphere<_TVarType>& sphere,
				    CollisionInfo* collision_info = 0);

	// And now for the most complex function of them all. OBB to OBB collision test
	// with velocity. This function will calculate the exact time of collision (TTC)
	// in seconds. 0 means that the boxes are already in collision from the start.
	// Using this function in your collision detection will guarantee that nothing will
	// ever move through anything else.
	// The CollisionInfo parameter is mandatory in this function.

	bool AreMovingOBBsColliding(_TVarType time_delta, // The elapsed time in seconds.
				    const OBB<_TVarType>& ob_b1,
				    const Vector3D<_TVarType>& ob_b1_velocity,
				    const OBB<_TVarType>& ob_b2,
				    const Vector3D<_TVarType>& ob_b2_velocity,
				    CollisionInfo& collision_info);

	// Get the point, line or surface that describes the intersection area during a collision.
	// If num_points is:
	// 1 = The intersection was a point.
	// 2 = The intersection was a line.
	// >2 = The intersection is an area, described as a polygon.
	//
	// This function can only be called after a call to AreMovingOBBsColliding(), and
	// it is NOT fast.
	void GetOBBToOBBIntersection(Vector3D<_TVarType> _point[8], int &num_points);

private:
	enum AxisID {
		kAxisNone = 0,
		kAxisX1,
		kAxisY1,
		kAxisZ1,
		kAxisX2,
		kAxisY2,
		kAxisZ2,
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

	enum CollisionType {
		kNoCollision = 0,
		kPointCollision,
		kEdgeCollision,
		kSurfaceCollision,
	};

	// Optimized overlapping test. Does not calculate any collision info.
	bool IsOBBOverlappingOBBOptimized(const OBB<_TVarType>& ob_b1,
	                                  const OBB<_TVarType>& ob_b2);

	// Unoptimized test that calculates collision info.
	bool IsOBBOverlappingOBB(const OBB<_TVarType>& ob_b1,
	                         const OBB<_TVarType>& ob_b2,
				 CollisionInfo& collision_info);


	CollisionType GetOBBCollisionType();
	void GetOBBPointIntersection(Vector3D<_TVarType> _point[8], int &num_points);
	void GetOBBEdgeSurfaceIntersection(Vector3D<_TVarType> _point[8], int &num_points, CollisionType collision_type);

	// Used by GetOBBEdgeSurfaceIntersection().
	void SetupBoxSide(Vector3D<_TVarType> box_center,
			  Vector3D<_TVarType> box_orientation[3],
			  _TVarType box_extents[3],
			  int normal_index,
			  Vector3D<_TVarType> normal,
			  int& ret_edge_index1,	// One of the edges that is not parallel to the normal.
			  int& ret_edge_index2,	// One of the edges that is not parallel to the normal.
			  Vector3D<_TVarType> ret_side_vertices[4]);

	int ClipVectorWithPlane(Vector3D<_TVarType> plane_point,
				Vector3D<_TVarType> plane_normal,
				Vector3D<_TVarType>& vector_point1,
				Vector3D<_TVarType>& vector_point2);

	bool GetTimeOfVectorPlaneIntersection(const Vector3D<_TVarType>& plane_point,
					      const Vector3D<_TVarType>& plane_normal,
					      const Vector3D<_TVarType>& vector_point,
					      const Vector3D<_TVarType>& vector_direction,
					      _TVarType& return_time);

	inline _TVarType PlaneEquation(const Vector3D<_TVarType>& plane_point,
				       const Vector3D<_TVarType>& plane_normal,
				       const Vector3D<_TVarType>& test_point);

	OBBCollisionData<_TVarType>* obb_collision_data_;
};

template<class _TVarType>
_TVarType CollisionDetector3D<_TVarType>::PlaneEquation(const Vector3D<_TVarType>& plane_point,
							const Vector3D<_TVarType>& plane_normal,
							const Vector3D<_TVarType>& test_point) {
	// Ax + By + Cz + D = 0
	// D = -(Ax + By + Cz) = -Normal dot PlanePoint;

	_TVarType d = -(plane_normal * plane_point);
	return plane_normal * test_point + d;
}

template<class _TVarType>
class OBBCollisionData {
public:
	OBBCollisionData() {
		Reset();
	}

	inline void Reset() {
		axis_id_ = CollisionDetector3D<_TVarType>::kAxisNone;
		side_ = 1.0f;
		valid_collision_ = false;

		box_with_separating_plane_ = -1;
		box1_normal_index_ = -1;
		box2_normal_index_ = -1;
		box1_edge_index_ = -1;
		box2_edge_index_ = -1;
		box1_side_ = 1;
		box2_side_ = 1;
	}

	bool valid_collision_;

	typename CollisionDetector3D<_TVarType>::AxisID axis_id_;
	_TVarType side_; // -1.0f or +1.0f

	_TVarType c_[9]; // Dot products between OBB1's axes and OBB2's.
	_TVarType abs_c_[9]; // Dot products between OBB1's axes and OBB2's.

	Vector3D<_TVarType> axis_;

	Vector3D<_TVarType> ob_b1_collision_pos_;
	Vector3D<_TVarType> ob_b2_collision_pos_;
	Vector3D<_TVarType> ob_b1_rot_axis_[3];
	Vector3D<_TVarType> ob_b2_rot_axis_[3];
	_TVarType ob_b1_size_[3];
	_TVarType ob_b2_size_[3];

	// Data used to optimize the search of the intersection area.
	int box_with_separating_plane_;	// 0 or 1
	int box1_normal_index_;		// 0 through 2 if Box0 is the box with the separating plane.
	int box2_normal_index_;		// 0 through 2 if Box1 is the box with the separating plane.
	int box1_edge_index_;		// 0 through 2 if kEdgeCollision and Box0 has the edge.
	int box2_edge_index_;		// 0 through 2 if kEdgeCollision and Box1 has the edge.
	_TVarType box1_side_;			// -1 or 1
	_TVarType box2_side_;			// -1 or 1
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
		_TVarType __time = (time) * ((projectedSizeSum) - (projectedDist0)) / ((projectedDist1) - (projectedDist0)); \
 \
		if(__time > retTimeMax) \
		{ \
			retTimeMax = __time; \
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
		_TVarType __time = -(time) * ((projectedSizeSum) + (projectedDist0)) / ((projectedDist1) - (projectedDist0)); \
 \
		if(__time > retTimeMax) \
		{ \
			retTimeMax = __time; \
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
	_TVarType projected_dist1; \
 \
	if((projectedDist0) > (projectedSizeSum)) \
	{ \
		projected_dist1 = (q0) * (q1) - (q2) * (q3); \
 \
		if(projected_dist1 > (projectedSizeSum)) \
		{ \
			retTimeMax = time; \
			return false; /* No collision */ \
		} \
 \
		_TVarType __time = (time) * ((projectedSizeSum) - (projectedDist0)) / (projected_dist1 - (projectedDist0)); \
		if ( __time > retTimeMax ) \
		{ \
			retTimeMax = __time; \
			retAxisID = currentAxisID; \
			retSide = 1; \
		} \
	} \
	else if((projectedDist0) < -(projectedSizeSum)) \
	{ \
		projected_dist1 = (q0) * (q1) - (q2) * (q3); \
 \
		if(projected_dist1 < -(projectedSizeSum)) \
		{ \
			retTimeMax = time; \
			return false; /* No collision */ \
		} \
 \
		_TVarType __time = -(time) * ((projectedSizeSum) + (projectedDist0)) / (projected_dist1 - (projectedDist0)); \
 \
		if(__time > retTimeMax) \
		{ \
			retTimeMax = __time; \
			retAxisID = currentAxisID; \
			retSide = -1; \
		} \
	} \
}

#define TSPMACRO_GET_SIGNED_EXTENT(signedExtent_, sign0, sign1, side, c, extent) \
{ \
	/* Extent = half the size of the OBB in one dimension. */ \
	/* That means half the width, height or depth. */ \
	/* The signs and the side are giving the signedExtent_ its correct sign */ \
	/* when combined. */ \
	if(c > 0.0f) \
	{ \
		signedExtent_ = sign0(side*extent); \
	} \
	else if(c < 0.0f) \
	{ \
		signedExtent_ = sign1(side*extent); \
	} \
	else \
	{ \
		signedExtent_ = 0.0f; \
	} \
}

#define TSPMACRO_GET_CORNER_POINT(point, boxWorldAxis, boxCollisionPos, signedExtents_) \
{ \
	/* The point returned is one of the corners of the box. */ \
	point = boxCollisionPos + \
		boxWorldAxis[0] * signedExtents_[0] + \
		boxWorldAxis[1] * signedExtents_[1] + \
		boxWorldAxis[2] * signedExtents_[2]; \
}

#include "collisiondetector3d.inl"

} // End namespace
