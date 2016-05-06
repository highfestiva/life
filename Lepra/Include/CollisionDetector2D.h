/*
	Class:  CollisionDetector2D
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	A class used to test collisions between 2D geometrical shapes.
	AABR = Axis Aligned Bounding Rectangle
	OBR = Oriented Bounding Rectangle
*/

#pragma once

#include "vector2d.h"
#include "aabr.h"
#include "obr.h"
#include "circle.h"

namespace lepra {

template<class _TVarType>
class CollisionDetector2D {
public:
	struct CollisionInfo {
		CollisionInfo() :
			separation_distance_(0),
			time_to_collision_(0) {
		}

		// This is either a true point, or it represents the center of the
		// contact line or area.
		Vector2D<_TVarType> point_of_collision_;

		// The contact surface normal, relative to the first object. With
		// other words, moving the first object in the direction of the
		// normal "mSeparationDistance" units away will separate the objects.
		Vector2D<_TVarType> normal_;

		// The penetration depth, or the distance along the normal which will
		// separate the two objects. This is always a nonnegative number.
		_TVarType separation_distance_;

		// 0 < time_to_collision_ < 1 for velocity tests. In static tests,
		// time_to_collision_ = 0 if there is a collision and 1 if not.
		_TVarType time_to_collision_;
	};

	CollisionDetector2D() :
		poc_enabled_(true) {
	}

	// Calculating the point of collision can be very time consuming in come cases.
	// Disabling it can improve performance, if the point of collision is of no interest.
	void SetPointOfCollisionEnabled(bool enabled) { poc_enabled_ = enabled; }

	// Enclosing tests, mainly used in combination with LooseQuadtree.
	bool IsAABR1EnclosingAABR2(const AABR<_TVarType>& box1, const AABR<_TVarType>& box2);
	bool IsAABREnclosingCircle(const AABR<_TVarType>& box, const Circle<_TVarType>& circle);
	bool IsCircle1EnclosingCircle2(const Circle<_TVarType>& circle1, const Circle<_TVarType>& circle2);
	bool IsCircleEnclosingAABR(const Circle<_TVarType>& circle, const AABR<_TVarType>& box);

	// All methods below returns true if a collision occurs, and false otherwise.

	bool StaticLineSegmentToLineSegmentTest(const Vector2D<_TVarType>& start1, const Vector2D<_TVarType>& end1,
	                                        const Vector2D<_TVarType>& start2, const Vector2D<_TVarType>& end2,
						CollisionInfo* collision_info = 0);

	// Test if one AABR collides with another.
	bool StaticAABRToAABRTest(const AABR<_TVarType>& box1,
	                          const AABR<_TVarType>& box2,
	                          CollisionInfo* collision_info = 0);

	// Test if one AABR will collide with another while moving along a given vector.
	// Also returns the time to the collision as a value between 0.0 and 1.0.
	bool VelocityAABRToAABRTest(const AABR<_TVarType>& box1,
	                            const AABR<_TVarType>& box2,
	                            const Vector2D<_TVarType>& box1_velocity,
	                            const Vector2D<_TVarType>& box2_velocity,
	                            CollisionInfo* collision_info = 0);

	// Test if one OBR collides with another.
	bool StaticOBRToOBRTest(const OBR<_TVarType>& box1, const OBR<_TVarType>& box2,
	                        CollisionInfo* collision_info = 0);
/*
	// Test if one OBR will collide with another while moving along a given vector.
	// Also returns the time to the collision as a value between 0.0 and 1.0.
	bool VelocityOBRToOBRTest(const OBR<_TVarType>& box1,
	                          const OBR<_TVarType>& box2,
	                          const Vector2D<_TVarType>& box1_velocity,
	                          const Vector2D<_TVarType>& box2_velocity,
	                          CollisionInfo* collision_info = 0);
*/
	// Test if one circle collides with another.
	bool StaticCircleToCircleTest(const Circle<_TVarType>& circle1,
	                              const Circle<_TVarType>& circle2,
				      CollisionInfo* collision_info = 0);

	// Test if one triangle collides with another.
	bool StaticTriangleToTriangleTest(const Vector2D<_TVarType> tri1[3],
					  const Vector2D<_TVarType> tri2[3],
					  CollisionInfo* collision_info = 0);


	// Returns 'true' if the point is to the left of the line segment as
	// seen from start to end AND if it is BETWEEN the two perpendicular
	// lines that go through start and end respectively. This differs
	// from other line segment tests.
	bool StaticPointLeftOfLineSegmentTest(const Vector2D<_TVarType>& point,
	                                      const Vector2D<_TVarType>& start,
	                                      const Vector2D<_TVarType>& end,
	                                      CollisionInfo* collision_info = 0);

	// Returns 'true' if the point is to the left of the line as seen in
	// the direction defined by dir.
	bool StaticPointLeftOfLineTest(const Vector2D<_TVarType>& point,
	                               const Vector2D<_TVarType>& point_on_line,
	                               const Vector2D<_TVarType>& dir,
	                               CollisionInfo* collision_info = 0);

	bool StaticPointToTriangleTest(const Vector2D<_TVarType>& point,
	                               const Vector2D<_TVarType> tri[3],
	                               CollisionInfo* collision_info = 0);


	// Combinations of geometric shapes.
	bool StaticAABRToPointTest(const AABR<_TVarType>& box,
	                           const Vector2D<_TVarType>& point,
				   CollisionInfo* collision_info = 0);
	bool StaticAABRToOBRTest(const AABR<_TVarType>& box1, const OBR<_TVarType>& box2,
	                         CollisionInfo* collision_info = 0);
	bool StaticAABRToCircleTest(const AABR<_TVarType>& box,
	                            const Circle<_TVarType>& circle,
				    CollisionInfo* collision_info = 0);
	bool StaticAABRToLineSegmentTest(const AABR<_TVarType>& box,
	                                 const Vector2D<_TVarType>& start,
	                                 const Vector2D<_TVarType>& end,
	                                 CollisionInfo* collision_info = 0);
	bool StaticAABRToTriangleTest(const AABR<_TVarType>& box,
	                              const Vector2D<_TVarType> tri[3],
				      CollisionInfo* collision_info = 0);

	bool StaticOBRToPointTest(const OBR<_TVarType>& box,
	                          const Vector2D<_TVarType>& point,
				  CollisionInfo* collision_info = 0);
	bool StaticOBRToCircleTest(const OBR<_TVarType>& box,
	                           const Circle<_TVarType>& circle,
				   CollisionInfo* collision_info = 0);
	bool StaticOBRToLineSegmentTest(const OBR<_TVarType>& box,
	                                const Vector2D<_TVarType>& start,
	                                const Vector2D<_TVarType>& end,
	                                CollisionInfo* collision_info = 0);
	bool StaticOBRToTriangleTest(const OBR<_TVarType>& box,
	                             const Vector2D<_TVarType> tri[3],
				     CollisionInfo* collision_info = 0);

	bool StaticCircleToPointTest(const Circle<_TVarType>& circle,
	                             const Vector2D<_TVarType>& point,
				     CollisionInfo* collision_info = 0);
	bool StaticCircleToLineSegmentTest(const Circle<_TVarType>& circle,
	                                   const Vector2D<_TVarType>& start,
	                                   const Vector2D<_TVarType>& end,
					   CollisionInfo* collision_info = 0);
	bool StaticCircleToTriangleTest(const Circle<_TVarType>& circle,
	                                const Vector2D<_TVarType> tri[3],
					CollisionInfo* collision_info = 0);

private:

	// Takes a convex polygon defined by poly and vertex_count,
	// where the vertices has to be stored as in a triangle fan,
	// and a line defined by point_on_line and dir, and returns
	// the clipped polygon in poly_out. The return value is the number
	// of vertices in poly_out, and can at most be vertex_count + 1.
	// The part of the polygon that overlaps the left side of the
	// line will be removed.
	int ClipPolyAgainstLine(Vector2D<_TVarType>* poly, int vertex_count,
				const Vector2D<_TVarType>& point_on_line,
				const Vector2D<_TVarType>& dir,
				Vector2D<_TVarType>* poly_out);

	_TVarType TriangleDistance(const Vector2D<_TVarType> tri[3],
	                           const Vector2D<_TVarType>& point,
			           Vector2D<_TVarType>& closest_point,
				   Vector2D<_TVarType>& normal,
				   _TVarType& projection);

	bool poc_enabled_;
};

#include "collisiondetector2d.inl"

}
