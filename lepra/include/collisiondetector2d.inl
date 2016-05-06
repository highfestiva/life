
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../include/collisiondetector2d.h"
#include "../include/math.h"



template<class _TVarType>
bool CollisionDetector2D<_TVarType>::IsAABR1EnclosingAABR2(const AABR<_TVarType>& box1, const AABR<_TVarType>& box2) {
	Vector2D<_TVarType> min1(box1.GetPosition() - box1.GetSize());
	Vector2D<_TVarType> max1(box1.GetPosition() + box1.GetSize());
	Vector2D<_TVarType> min2(box2.GetPosition() - box2.GetSize());
	Vector2D<_TVarType> max2(box2.GetPosition() + box2.GetSize());
	return (min1.x <= min2.x && min1.y <= min2.y && max1.x >= max2.x && max1.y >= max2.y);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::IsAABREnclosingCircle(const AABR<_TVarType>& box, const Circle<_TVarType>& circle) {
	Vector2D<_TVarType> dim(circle.GetRadius(), circle.GetRadius());
	Vector2D<_TVarType> c_min(circle.GetPosition() - dim);
	Vector2D<_TVarType> c_max(circle.GetPosition() + dim);
	Vector2D<_TVarType> __min(box.GetPosition() - box.GetSize());
	Vector2D<_TVarType> __max(box.GetPosition() + box.GetSize());
	return (__min.x <= c_min.x && __min.y <= c_min.y && __max.x >= c_max.x && __max.y >= c_max.y);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::IsCircle1EnclosingCircle2(const Circle<_TVarType>& circle1, const Circle<_TVarType>& circle2) {
	_TVarType dist = circle2.GetPosition().GetDistance(circle1.GetPosition());
	return (circle1.GetRadius() >= dist + circle2.GetRadius());
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::IsCircleEnclosingAABR(const Circle<_TVarType>& circle, const AABR<_TVarType>& box) {
	return IsCircle1EnclosingCircle2(circle, Circle<_TVarType>(box.GetPosition(), box.GetSize().GetLength()));
}


template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticLineSegmentToLineSegmentTest(const Vector2D<_TVarType>& start1, const Vector2D<_TVarType>& end1,
                                        const Vector2D<_TVarType>& start2, const Vector2D<_TVarType>& end2,
					CollisionInfo* collision_info) {
	if(collision_info != 0)
		collision_info->time_to_collision_ = 1;

	Vector2D<_TVarType> dir1(end1 - start1);
	Vector2D<_TVarType> dir2(end2 - start2);
	_TVarType denominator = dir2.y * dir1.x - dir2.x * dir1.y;

	if(abs(denominator) < MathTraits<_TVarType>::Eps())
		return false;

	Vector2D<_TVarType> diff(start1 - start2);
	_TVarType numerator = dir2.x * diff.y - dir2.y * diff.x;
	_TVarType t = numerator / denominator;

	// Not truly the intersection... Should be start1 + dir1 * t, but
	// start1 was removed as a small optimization.
	Vector2D<_TVarType> intersection(dir1 * t);

	_TVarType length = dir1.GetLength();
	dir1 /= length; // Normalize.

	// Would have been dir1.Dot(intersection - start1) unoptimized.
	_TVarType projection1 = dir1.Dot(intersection);

	if(projection1 < 0 || projection1 > length)
		return false;

	length = dir2.GetLength();
	dir2 /= length; // Normalize.

	// No difference in performance at this line, but this would have been
	// dir2.Dot(intersection - start2) unoptimized.
	_TVarType projection2 = dir2.Dot(intersection + diff);

	if(projection2 < 0 || projection2 > length)
		return false;

	if(collision_info != 0) {
		collision_info->time_to_collision_ = 0;
		collision_info->point_of_collision_ = intersection + start1;

		// Split the line segments at the point of intersection into
		// four subsegments...
		Vector2D<_TVarType> v1(start1 - collision_info->point_of_collision_);
		Vector2D<_TVarType> v2(end1   - collision_info->point_of_collision_);
		Vector2D<_TVarType> v3(start2 - collision_info->point_of_collision_);
		Vector2D<_TVarType> v4(end2   - collision_info->point_of_collision_);

		// The normals of the lines. Note that they are already normalized.
		Vector2D<_TVarType> normal1(dir1.y, -dir1.x);
		Vector2D<_TVarType> normal2(dir2.y, -dir2.x);

		// Calculate the projections of the subsegments.
		_TVarType p1 = normal2.Dot(v1);
		_TVarType p2 = normal2.Dot(v2);
		_TVarType p3 = normal1.Dot(v3);
		_TVarType p4 = normal1.Dot(v4);

		// Find the minimum projection.
		int min_p_index = 0;
		_TVarType min_p = p1;
		_TVarType min_abs_p = abs(p1);
		_TVarType abs_p = abs(p2);
		if(abs_p < min_abs_p) {
			min_abs_p = abs_p;
			min_p = p2;
			min_p_index = 1;
		}
		abs_p = abs(p3);
		if(abs_p < min_abs_p) {
			min_abs_p = abs_p;
			min_p = p3;
			min_p_index = 2;
		}
		abs_p = abs(p4);
		if(abs_p < min_abs_p) {
			min_abs_p = abs_p;
			min_p = p4;
			min_p_index = 3;
		}

		// Now we know the separation distance.
		collision_info->separation_distance_ = min_abs_p;

		// Find the collision normal...
		switch(min_p_index) {
			case 0: collision_info->normal_ = p1 > 0 ? -normal2 : normal2; break;
			case 1: collision_info->normal_ = p2 > 0 ? -normal2 : normal2; break;
			case 2: collision_info->normal_ = p3 > 0 ? normal1 : -normal1; break;
			case 3: collision_info->normal_ = p4 > 0 ? normal1 : -normal1; break;
		};
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToAABRTest(const AABR<_TVarType>& box1,
							  const AABR<_TVarType>& box2,
							  CollisionInfo* collision_info) {
	Vector2D<_TVarType> min_dist(box1.GetSize() + box2.GetSize());
	Vector2D<_TVarType> dist(box1.GetPosition() - box2.GetPosition());

	if(dist.x < 0.0f)
		dist.x = -dist.x;
	if(dist.y < 0.0f)
		dist.y = -dist.y;

	if(dist.x > min_dist.x || dist.y > min_dist.y) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		Vector2D<_TVarType> box1_extent_min(box1.GetPosition() - box1.GetSize());
		Vector2D<_TVarType> box1_extent_max(box1.GetPosition() + box1.GetSize());
		Vector2D<_TVarType> box2_extent_min(box2.GetPosition() - box2.GetSize());
		Vector2D<_TVarType> box2_extent_max(box2.GetPosition() + box2.GetSize());

		Vector2D<_TVarType> overlap_min(std::max(box1_extent_min.x, box2_extent_min.x), std::max(box1_extent_min.y, box2_extent_min.y));
		Vector2D<_TVarType> overlap_max(std::min(box1_extent_max.x, box2_extent_max.x), std::min(box1_extent_max.y, box2_extent_max.y));

		collision_info->time_to_collision_ = 0;
		collision_info->point_of_collision_ = (overlap_min + overlap_max) * (_TVarType)0.5;

		//Always positive.
		Vector2D<_TVarType> diff(min_dist - dist);
		if(diff.x < diff.y) {
			collision_info->separation_distance_ = diff.x;
			if(dist.x >= 0)
				collision_info->normal_.Set(1, 0);
			else
				collision_info->normal_.Set(-1, 0);
		} else {
			collision_info->separation_distance_ = diff.y;
			if(dist.y >= 0)
				collision_info->normal_.Set(0, 1);
			else
				collision_info->normal_.Set(0, -1);
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::VelocityAABRToAABRTest(const AABR<_TVarType>& box1,
                                                            const AABR<_TVarType>& box2,
                                                            const Vector2D<_TVarType>& box1_velocity,
                                                            const Vector2D<_TVarType>& box2_velocity,
                                                            CollisionInfo* collision_info) {
	if(collision_info != 0)
		collision_info->time_to_collision_ = 0;

	Vector2D<_TVarType> relative_velocity(box1_velocity - box2_velocity);
	Vector2D<_TVarType> min_dist(box1.GetSize() + box2.GetSize());
	Vector2D<_TVarType> dist(box1.GetPosition() - box2.GetPosition());
	Vector2D<_TVarType> abs_dist(abs(dist.x), abs(dist.y));

	Vector2D<_TVarType> max_allowed_move(abs_dist - min_dist);

	// Test x-axis. Even if the boxes are colliding from start,
	// don't count it as a collision if they are moving apart.
	if(max_allowed_move.x > abs(relative_velocity.x) ||
	   (relative_velocity.x >= 0 && dist.x >= 0) ||
	   (relative_velocity.x < 0 && dist.x < 0)) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		if(dist.x >= 0)
			collision_info->normal_.Set(-1, 0);
		else
			collision_info->normal_.Set(1, 0);

		if(max_allowed_move.x >= 0) {
			collision_info->separation_distance_ = 0;
			collision_info->time_to_collision_ = (_TVarType)max_allowed_move.x / (_TVarType)abs(relative_velocity.x);
		} else {
			collision_info->separation_distance_ = -max_allowed_move.x;
			collision_info->time_to_collision_ = 0;
		}
	}

	// Test y-axis. Even if the boxes are colliding from start,
	// don't count it as a collision if they are moving apart.
	if(max_allowed_move.y > abs(relative_velocity.y) ||
	   (relative_velocity.y >= 0 && dist.y >= 0) ||
	   (relative_velocity.y < 0 && dist.y < 0)) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		_TVarType time_to_collision = (_TVarType)max_allowed_move.y / (_TVarType)abs(relative_velocity.y);
		if(time_to_collision > collision_info->time_to_collision_) {
			if(dist.y >= 0)
				collision_info->normal_.Set(0, -1);
			else
				collision_info->normal_.Set(0, 1);

			if(max_allowed_move.y >= 0) {
				collision_info->separation_distance_ = 0;
				collision_info->time_to_collision_ = time_to_collision;
			} else {
				collision_info->separation_distance_ = -max_allowed_move.y;
				collision_info->time_to_collision_ = 0;
			}
		}
	}

	// Check for collision by looking along he velocity vector.
	Vector2D<_TVarType> _normal(relative_velocity.y, -relative_velocity.x);
	_normal.Normalize();

	_TVarType box1_projection = abs(_normal.Dot(box1.GetSize()));
	_TVarType box2_projection = abs(_normal.Dot(box2.GetSize()));
	_TVarType dist_projection = abs(_normal.Dot(dist));

	_TVarType min_sep_dist = box1_projection + box2_projection;

	if(dist_projection > min_sep_dist)
		return false;

	if(collision_info != 0) {
		// Calculate the point of collision.
		Vector2D<_TVarType> box1_extent_min(box1.GetPosition() - box1.GetSize());
		Vector2D<_TVarType> box1_extent_max(box1.GetPosition() + box1.GetSize());
		Vector2D<_TVarType> box2_extent_min(box2.GetPosition() - box2.GetSize());
		Vector2D<_TVarType> box2_extent_max(box2.GetPosition() + box2.GetSize());

		Vector2D<_TVarType> overlap_min(max(box1_extent_min.x, box2_extent_min.x), max(box1_extent_min.y, box2_extent_min.y));
		Vector2D<_TVarType> overlap_max(min(box1_extent_max.x, box2_extent_max.x), min(box1_extent_max.y, box2_extent_max.y));

		Vector2D<_TVarType> diff(overlap_max - overlap_min);

		collision_info->point_of_collision_ = overlap_min + diff * (_TVarType)0.5;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToOBRTest(const OBR<_TVarType>& box1,
							const OBR<_TVarType>& box2,
                                                        CollisionInfo* collision_info) {
	if(collision_info != 0)
		collision_info->time_to_collision_ = 0;

	Vector2D<_TVarType> dist(box1.GetPosition() - box2.GetPosition());
	Vector2D<_TVarType> size(box1.GetSize() + box2.GetSize());

	Vector2D<_TVarType> box1_extent_x(box1.GetExtentX());
	Vector2D<_TVarType> box1_extent_y(box1.GetExtentY());
	Vector2D<_TVarType> box2_extent_x(box2.GetExtentX());
	Vector2D<_TVarType> box2_extent_y(box2.GetExtentY());

	// The first plane to test.
	Vector2D<_TVarType> _normal(box1_extent_x);
	_normal.Normalize();
	_TVarType box1_projection = box1.GetSize().x;
	_TVarType box2_projection = abs(_normal.Dot(box2_extent_x)) + abs(_normal.Dot(box2_extent_y));
	_TVarType dist_projection = _normal.Dot(dist);
	_TVarType abs_dist_projection = abs(dist_projection);
	if(abs_dist_projection > box1_projection + box2_projection) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		collision_info->time_to_collision_ = 0;
		collision_info->separation_distance_ = box1_projection + box2_projection - abs_dist_projection;
		if(dist_projection > 0)
			collision_info->normal_ = _normal;
		else
			collision_info->normal_ = -_normal;
	}

	// The second plane to test.
	_normal.Set(box1_extent_y);
	_normal.Normalize();
	box1_projection = box1.GetSize().y;
	box2_projection = abs(_normal.Dot(box2_extent_x)) + abs(_normal.Dot(box2_extent_y));
	dist_projection = _normal.Dot(dist);
	abs_dist_projection = abs(dist_projection);
	if(abs_dist_projection > box1_projection + box2_projection) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		_TVarType penetration_depth = box1_projection + box2_projection - abs_dist_projection;
		if(penetration_depth < collision_info->separation_distance_) {
			collision_info->time_to_collision_ = 0;
			collision_info->separation_distance_ = penetration_depth;
			if(dist_projection > 0)
				collision_info->normal_ = _normal;
			else
				collision_info->normal_ = -_normal;
		}
	}

	// The third plane to test.
	_normal.Set(box2_extent_x);
	_normal.Normalize();
	box1_projection = abs(_normal.Dot(box1_extent_x)) + abs(_normal.Dot(box1_extent_y));
	box2_projection = box2.GetSize().x;
	dist_projection = _normal.Dot(dist);
	abs_dist_projection = abs(dist_projection);
	if(abs_dist_projection > box1_projection + box2_projection) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		_TVarType penetration_depth = box1_projection + box2_projection - abs_dist_projection;
		if(penetration_depth < collision_info->separation_distance_) {
			collision_info->time_to_collision_ = 0;
			collision_info->separation_distance_ = penetration_depth;
			if(dist_projection > 0)
				collision_info->normal_ = _normal;
			else
				collision_info->normal_ = -_normal;
		}
	}

	// The fourth and final plane to test.
	_normal.Set(box2_extent_y);
	_normal.Normalize();
	box1_projection = abs(_normal.Dot(box1_extent_x)) + abs(_normal.Dot(box1_extent_y));
	box2_projection = box2.GetSize().y;
	dist_projection = _normal.Dot(dist);
	abs_dist_projection = abs(dist_projection);
	if(abs_dist_projection > box1_projection + box2_projection) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		_TVarType penetration_depth = box1_projection + box2_projection - abs_dist_projection;
		if(penetration_depth < collision_info->separation_distance_) {
			collision_info->time_to_collision_ = 0;
			collision_info->separation_distance_ = penetration_depth;
			if(dist_projection > 0)
				collision_info->normal_ = _normal;
			else
				collision_info->normal_ = -_normal;
		}
	}

	if(collision_info != 0 && poc_enabled_) {
		// Calculate point of collision. First find the overlap, which is a
		// convex polygon consisting of a maximum of 8 vertices. Clip each side
		// of Box1 against each side of Box2 using the same algorithm used in
		// uitbc::Software3DPainter, and finally take the mean coordinates of
		// the clipped points.
		collision_info->point_of_collision_.Set(0, 0);

		Vector2D<_TVarType> in_poly[8];
		Vector2D<_TVarType> out_poly[8];
		Vector2D<_TVarType> box2_po_l[4]; // Point on line.
		Vector2D<_TVarType> box2_line_dir[4];

		in_poly[0] = box1.GetPosition() + box1_extent_x + box1_extent_y;
		in_poly[1] = box1.GetPosition() + box1_extent_x - box1_extent_y;
		in_poly[2] = box1.GetPosition() - box1_extent_x - box1_extent_y;
		in_poly[3] = box1.GetPosition() - box1_extent_x + box1_extent_y;

		// Initialize in the wrong order.
		int v_count = 4;
		Vector2D<_TVarType>* in = out_poly;
		Vector2D<_TVarType>* out = in_poly;

		// Setup directions clockwise.
		box2_line_dir[0] = -box2_extent_y;
		box2_line_dir[1] =  box2_extent_x;
		box2_line_dir[2] =  box2_extent_y;
		box2_line_dir[3] = -box2_extent_x;

		box2_po_l[0] = box2.GetPosition() + box2_extent_x;
		box2_po_l[1] = box2.GetPosition() + box2_extent_y;
		box2_po_l[2] = box2.GetPosition() - box2_extent_x;
		box2_po_l[3] = box2.GetPosition() - box2_extent_y;

		int i;
		for(i = 0; i < 4; i++) {
			// Swap in & out.
			Vector2D<_TVarType>* temp = in;
			in = out;
			out = temp;
			v_count = ClipPolyAgainstLine(in, v_count, box2_po_l[i], box2_line_dir[i], out);
		}

		for(i = 0; i < v_count; i++) {
			collision_info->point_of_collision_ += out[i];
		}
		collision_info->point_of_collision_ /= (_TVarType)v_count;
	}

	return true;
}
/*
// TODO: Implement this.
template<class _TVarType>
bool CollisionDetector2D<_TVarType>::VelocityOBRToOBRTest(const OBR<_TVarType>& box1,
                                                          const OBR<_TVarType>& box2,
                                                          const Vector2D<_TVarType>& box1_velocity,
                                                          const Vector2D<_TVarType>& box2_velocity,
                                                          CollisionInfo* collision_info) {
	return false;
}
*/
template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticCircleToCircleTest(const Circle<_TVarType>& circle1,
                                                              const Circle<_TVarType>& circle2,
			                                      CollisionInfo* collision_info) {
	_TVarType min_distance = circle1.GetRadius() + circle2.GetRadius();
	_TVarType min_distance_sq = min_distance * min_distance;
	_TVarType distance_sq = circle1.GetPosition().GetDistanceSquared(circle2.GetPosition());
	if(distance_sq > min_distance_sq) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		collision_info->normal_ = (circle1.GetPosition() - circle2.GetPosition());
		_TVarType length = collision_info->normal_.GetLength();
		collision_info->normal_ /= length; // Normalize
		collision_info->separation_distance_ = (_TVarType)(min_distance - length);
		collision_info->point_of_collision_ = circle2.GetPosition() + collision_info->normal_ * (circle2.GetRadius() - collision_info->separation_distance_ * (_TVarType)0.5);
		collision_info->time_to_collision_ = 0;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticTriangleToTriangleTest(const Vector2D<_TVarType> tri1[3],
								  const Vector2D<_TVarType> tri2[3],
								  CollisionInfo* collision_info) {
	if(collision_info != 0)
		collision_info->time_to_collision_ = 1;

	 // Find out wether the triangles are clockwise.
	Vector2D<_TVarType> perp1((tri1[1] - tri1[0]).GetPerpCW());
	Vector2D<_TVarType> perp2((tri2[1] - tri2[0]).GetPerpCW());
	_TVarType p1 = perp1.Dot(tri1[2] - tri1[0]);
	_TVarType p2 = perp2.Dot(tri2[2] - tri2[0]);

	// Copy triangle data and make sure they are stored in clockwise order.
	Vector2D<_TVarType> _tri1[3];
	Vector2D<_TVarType> _tri2[3];

	if(p1 >= 0) {
		_tri1[0] = tri1[0];
		_tri1[1] = tri1[1];
		_tri1[2] = tri1[2];
	} else {
		_tri1[0] = tri1[0];
		_tri1[1] = tri1[2];
		_tri1[2] = tri1[1];
	}

	if(p2 >= 0) {
		_tri2[0] = tri2[0];
		_tri2[1] = tri2[1];
		_tri2[2] = tri2[2];
	} else {
		_tri2[0] = tri2[0];
		_tri2[1] = tri2[2];
		_tri2[2] = tri2[1];
	}

	// We can't use the regular technique of separating "planes" (lines) as usual,
	// since the triangles are not (in general) symmetric along any axis. Instead,
	// the separating line has to be fully defined (as opposed to only knowing the
	// slope). Each side of each triangle defines one such line, against which we
	// have to clip the other triangle before applying the method of separating
	// lines.

	// Now find the overlap between the triangles.
	Vector2D<_TVarType> out[6];
	Vector2D<_TVarType> edge(_tri2[1] - _tri2[0]);
	int v_count;
	if((v_count = ClipPolyAgainstLine(_tri1, 3, _tri2[0], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri2[0]));
		collision_info->separation_distance_ = _projection;
		collision_info->normal_ = -n;
	}

	edge = _tri2[2] - _tri2[1];
	if((v_count = ClipPolyAgainstLine(_tri1, 3, _tri2[1], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri2[1]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = -n;
		}
	}

	edge = _tri2[0] - _tri2[2];
	if((v_count = ClipPolyAgainstLine(_tri1, 3, _tri2[2], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri2[2]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = -n;
		}
	}

	edge = _tri1[1] - _tri1[0];
	if((v_count = ClipPolyAgainstLine(_tri2, 3, _tri1[0], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri1[0]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = n;
		}
	}

	edge = _tri1[2] - _tri1[1];
	if((v_count = ClipPolyAgainstLine(_tri2, 3, _tri1[1], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri1[1]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = n;
		}
	}

	edge = _tri1[0] - _tri1[2];
	if((v_count = ClipPolyAgainstLine(_tri2, 3, _tri1[2], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri1[2]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = n;
		}

		collision_info->time_to_collision_ = 0;

		// Now we have to find the overlap in order to find the point of collision.
		if(poc_enabled_) {
			Vector2D<_TVarType> temp[6];
			int v_count;
			v_count = ClipPolyAgainstLine(_tri1, 3, _tri2[0], _tri2[1] - _tri2[0], out);
			v_count = ClipPolyAgainstLine(out, v_count, _tri2[1], _tri2[2] - _tri2[1], temp);
			v_count = ClipPolyAgainstLine(temp, v_count, _tri2[2], _tri2[0] - _tri2[2], out);

			collision_info->point_of_collision_.Set(0, 0);
			int i;
			for(i = 0; i < v_count; i++) {
				collision_info->point_of_collision_ += out[i];
			}
			collision_info->point_of_collision_ /= (_TVarType)v_count;
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticPointLeftOfLineSegmentTest(const Vector2D<_TVarType>& point,
                                                                      const Vector2D<_TVarType>& start,
                                                                      const Vector2D<_TVarType>& end,
                                                                      CollisionInfo* collision_info) {
	Vector2D<_TVarType> l(end - start);
	_TVarType length = l.GetLength();
	l /= length; // Normalize.
	Vector2D<_TVarType> p(point - start);

	_TVarType _projection = l.Dot(p);
	if(_projection < 0 || _projection > length) {
		// Not between the end points of the line segment.
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	// Line normal, pointing to the right.
	Vector2D<_TVarType> l_perp(l.y, -l.x);

	_projection = l_perp.Dot(p);

	if(_projection > 0) {
		// Point is to the right of the line segment.
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		collision_info->normal_ = l_perp;
		collision_info->separation_distance_ = -_projection;
		collision_info->point_of_collision_ = point;
		collision_info->time_to_collision_ = 0;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticPointLeftOfLineTest(const Vector2D<_TVarType>& point,
							       const Vector2D<_TVarType>& point_on_line,
							       const Vector2D<_TVarType>& dir,
							       CollisionInfo* collision_info) {
	Vector2D<_TVarType> _normal(dir.y, -dir.x);
	_normal.Normalize();
	_TVarType _projection = _normal.Dot(point - point_on_line);
	if(_projection > 0) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		collision_info->normal_ = _normal;
		collision_info->separation_distance_ = -_projection;
		collision_info->point_of_collision_ = point;
		collision_info->time_to_collision_ = 0;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticPointToTriangleTest(const Vector2D<_TVarType>& point,
							       const Vector2D<_TVarType> tri[3],
							       CollisionInfo* collision_info) {
	if(collision_info != 0)
		collision_info->time_to_collision_ = 1;

	 // Find out wether the triangle is clockwise.
	Vector2D<_TVarType> perp((tri[1] - tri[0]).GetPerpCW());
	_TVarType p = perp.Dot(tri[2] - tri[0]);

	Vector2D<_TVarType> _tri[3];
	if(p >= 0) {
		_tri[0] = tri[0];
		_tri[1] = tri[1];
		_tri[2] = tri[2];
	} else {
		_tri[0] = tri[0];
		_tri[1] = tri[2];
		_tri[2] = tri[1];
	}
	Vector2D<_TVarType> _normal((_tri[1] - _tri[0]).GetPerpCW());
	_TVarType _projection = _normal.Dot(point - _tri[0]);
	if(_projection < 0)
		return false;

	if(collision_info != 0) {
		_TVarType length_recip = 1 / _normal.GetLength();
		collision_info->normal_ = _normal * -length_recip; // Normalize.
		collision_info->separation_distance_ = _projection * length_recip;
	}

	_normal.SetPerpCW(_tri[2] - _tri[1]);
	_projection = _normal.Dot(point - _tri[1]);
	if(_projection < 0)
		return false;

	if(collision_info != 0) {
		_TVarType length_recip = 1 / _normal.GetLength();
		_projection *= length_recip;

		if(_projection < collision_info->separation_distance_) {
			collision_info->normal_ = _normal * -length_recip; // Normalize.
			collision_info->separation_distance_ = _projection;
		}
	}

	_normal.SetPerpCW(_tri[0] - _tri[2]);
	_projection = _normal.Dot(point - _tri[2]);
	if(_projection < 0)
		return false;

	if(collision_info != 0) {
		_TVarType length_recip = 1 / _normal.GetLength();
		_projection *= length_recip;

		if(_projection < collision_info->separation_distance_) {
			collision_info->normal_ = _normal * -length_recip; // Normalize.
			collision_info->separation_distance_ = _projection;
		}

		collision_info->time_to_collision_ = 0;
		collision_info->point_of_collision_ = point;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToPointTest(const AABR<_TVarType>& box,
							   const Vector2D<_TVarType>& point,
							   CollisionInfo* collision_info) {
	Vector2D<_TVarType> box_extent_min(box.GetPosition() - box.GetSize());
	Vector2D<_TVarType> box_extent_max(box.GetPosition() + box.GetSize());

	if(point.x >= box_extent_min.x && point.x < box_extent_max.x &&
	   point.y >= box_extent_min.y && point.y < box_extent_max.y) {
		if(collision_info != 0) {
			collision_info->time_to_collision_ = 0;
			collision_info->point_of_collision_ = point;

			// Assume point penetrated left side of box.
			collision_info->normal_.Set(1, 0); // Relative to box.
			collision_info->separation_distance_ = abs(point.x - box_extent_min.x);

			// Check right side.
			_TVarType penetration_depth = abs(point.x - box_extent_max.x);
			if(penetration_depth < collision_info->separation_distance_) {
				collision_info->normal_.Set(-1, 0); // Relative to box.
				collision_info->separation_distance_ = abs(point.x - box_extent_max.x);
			}

			// Check top side.
			penetration_depth = abs(point.y - box_extent_min.y);
			if(penetration_depth < collision_info->separation_distance_) {
				collision_info->normal_.Set(0, 1); // Relative to box.
				collision_info->separation_distance_ = abs(point.y - box_extent_min.y);
			}

			// Check bottom side.
			penetration_depth = abs(point.y - box_extent_max.y);
			if(penetration_depth < collision_info->separation_distance_) {
				collision_info->normal_.Set(0, -1); // Relative to box.
				collision_info->separation_distance_ = abs(point.y - box_extent_max.y);
			}
		}

		return true;
	} else {
		if(collision_info != 0) {
			collision_info->time_to_collision_ = 1;
		}
		return false;
	}
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToOBRTest(const AABR<_TVarType>& box1,
							 const OBR<_TVarType>& box2,
							 CollisionInfo* collision_info) {
	// TODO: Implement a case specific optimized version if you think it's worth
	// all the trouble. =)
	OBR<_TVarType> _box1(box1.GetPosition(), box1.GetSize());
	return StaticOBRToOBRTest(_box1, box2, collision_info);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToCircleTest(const AABR<_TVarType>& box,
							    const Circle<_TVarType>& circle,
							    CollisionInfo* collision_info) {
	Vector2D<_TVarType> __min(box.GetPosition() - box.GetSize());
	Vector2D<_TVarType> __max(box.GetPosition() + box.GetSize());
	Vector2D<_TVarType> point_on_box(0, 0);

	if(circle.GetPosition().x < __min.x)
		point_on_box.x = __min.x;
	else if(circle.GetPosition().x > __max.x)
		point_on_box.x = __max.x;
	else
		point_on_box.x = circle.GetPosition().x;

	if(circle.GetPosition().y < __min.y)
		point_on_box.y = __min.y;
	else if(circle.GetPosition().y > __max.y)
		point_on_box.y = __max.y;
	else if(collision_info != 0)
		point_on_box.y = circle.GetPosition().y;

	Vector2D<_TVarType> dist(point_on_box - circle.GetPosition());
	_TVarType distance_squared = dist.GetLengthSquared();

	if(distance_squared > circle.GetRadiusSquared()) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		collision_info->time_to_collision_ = 0;
		collision_info->normal_ = dist;
		_TVarType length = dist.GetLength();
		collision_info->normal_ /= length; // Normalize.
		collision_info->separation_distance_ = circle.GetRadius() - length;
		collision_info->point_of_collision_ = circle.GetPosition() + collision_info->normal_ * ((circle.GetRadius() + length) * (_TVarType)0.5);
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToLineSegmentTest(const AABR<_TVarType>& box,
								 const Vector2D<_TVarType>& start,
								 const Vector2D<_TVarType>& end,
								 CollisionInfo* collision_info) {
	// TODO: Implement a case specific optimized version if you think it's worth
	// all the trouble. =)
	OBR<_TVarType> obr(box.GetPosition(), box.GetSize(), 0);
	return StaticOBRToLineSegmentTest(obr, start, end, collision_info);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticAABRToTriangleTest(const AABR<_TVarType>& box,
                                                              const Vector2D<_TVarType> tri[3],
                                                              CollisionInfo* collision_info) {
	// TODO: Implement a case specific optimized version if you think it's worth
	// all the trouble. =)
	OBR<_TVarType> obr(box.GetPosition(), box.GetSize(), 0);
	return StaticOBRToTriangleTest(obr, tri, collision_info);
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToPointTest(const OBR<_TVarType>& box,
							  const Vector2D<_TVarType>& point,
							  CollisionInfo* collision_info) {
	Vector2D<_TVarType> dist(point - box.GetPosition());
	Vector2D<_TVarType> x_axis(box.GetExtentX());
	x_axis.Normalize();
	_TVarType x_projection = x_axis.Dot(dist);
	_TVarType abs_x_projection = abs(x_projection);
	if(abs_x_projection > box.GetSize().x) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	Vector2D<_TVarType> y_axis(box.GetExtentY());
	y_axis.Normalize();
	_TVarType y_projection = y_axis.Dot(dist);
	_TVarType abs_y_projection = abs(y_projection);
	if(abs_y_projection > box.GetSize().y) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		collision_info->time_to_collision_ = 1;
		collision_info->point_of_collision_ = point;

		_TVarType x_diff = box.GetSize().x - abs_x_projection;
		_TVarType y_diff = box.GetSize().y - abs_y_projection;

		if(x_diff < y_diff) {
			collision_info->normal_ = x_projection > 0 ? -x_axis : x_axis;
			collision_info->separation_distance_ = x_diff;
		} else {
			collision_info->normal_ = y_projection > 0 ? -y_axis : y_axis;
			collision_info->separation_distance_ = y_diff;
		}
		collision_info->normal_.Normalize();
	}

	return true;
}


template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToCircleTest(const OBR<_TVarType>& box,
							   const Circle<_TVarType>& circle,
							   CollisionInfo* collision_info) {
	Vector2D<_TVarType> dist(circle.GetPosition() - box.GetPosition());
	Vector2D<_TVarType> x_axis(box.GetExtentX());
	x_axis.Normalize();
	_TVarType x_projection = x_axis.Dot(dist);
	_TVarType abs_x_projection = abs(x_projection);
	if(abs_x_projection > box.GetSize().x + circle.GetRadius()) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	Vector2D<_TVarType> y_axis(box.GetExtentY());
	y_axis.Normalize();
	_TVarType y_projection = y_axis.Dot(dist);
	_TVarType abs_y_projection = abs(y_projection);
	if(abs_y_projection > box.GetSize().y + circle.GetRadius()) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	// Find the point on the box which is closest to the circle.
	Vector2D<_TVarType> po_l[4];
	po_l[0] = po_l[1] = box.GetPosition() - box.GetExtentX() - box.GetExtentY();
	po_l[2] = po_l[3] = box.GetPosition() + box.GetExtentX() + box.GetExtentY();

	Vector2D<_TVarType> _dir[4];
	_dir[0] = y_axis;
	_dir[1] = x_axis;
	_dir[2] = -y_axis;
	_dir[3] = -x_axis;

	_TVarType max_dir_dist[4];
	max_dir_dist[0] = max_dir_dist[2] = box.GetSize().y * 2;
	max_dir_dist[1] = max_dir_dist[3] = box.GetSize().x * 2;

	_TVarType min_distance = 0;
	Vector2D<_TVarType> _closest_point;
	for(int i = 0; i < 4; i++) {
		Vector2D<_TVarType> cp;
		_TVarType dir_distance;
		_TVarType distance = LineDistance(po_l[i], _dir[i], circle.GetPosition(), cp, dir_distance);

		if(dir_distance > max_dir_dist[i]) {
			cp = po_l[i] + _dir[i] * max_dir_dist[i];
			distance = circle.GetPosition().GetDistance(cp);
		} else if(dir_distance < 0) {
			cp = po_l[i];
			distance = circle.GetPosition().GetDistance(cp);
		}

		if(i == 0 || distance < min_distance) {
			min_distance = distance;
			_closest_point = cp;
		}
	}

	Vector2D<_TVarType> _normal(circle.GetPosition() - _closest_point);
	_normal /= min_distance; // Normalize.

	_TVarType box_projection = abs(_normal.Dot(box.GetExtentX())) + abs(_normal.Dot(box.GetExtentY()));
	_TVarType dist_projection = _normal.Dot(dist);
	_TVarType abs_dist_projection = abs(dist_projection);
	if(abs_dist_projection > box_projection + circle.GetRadius()) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		collision_info->time_to_collision_ = 0;
		if(dist_projection > 0)
			_normal *= -1;
		collision_info->normal_ = _normal;
		collision_info->separation_distance_ = box_projection + circle.GetRadius() - abs_dist_projection;
		collision_info->point_of_collision_ = circle.GetPosition() + _normal * (circle.GetRadius() - collision_info->separation_distance_ * (_TVarType)0.5);
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToLineSegmentTest(const OBR<_TVarType>& box,
							        const Vector2D<_TVarType>& start,
							        const Vector2D<_TVarType>& end,
							        CollisionInfo* collision_info) {
	Vector2D<_TVarType> extent_x(box.GetExtentX());
	Vector2D<_TVarType> extent_y(box.GetExtentY());
	Vector2D<_TVarType> _dir(end - start);
	Vector2D<_TVarType> _normal(_dir.y, -_dir.x);
	_normal.Normalize();

	_dir *= (_TVarType)0.5;
	Vector2D<_TVarType> line_center(start + _dir);
	Vector2D<_TVarType> dist(box.GetPosition() - line_center);

	_TVarType dist_projection = _normal.Dot(dist);
	_TVarType box_projection = abs(_normal.Dot(extent_x)) + abs(_normal.Dot(extent_y));

	if(abs(dist_projection) > abs(box_projection)) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		if(dist_projection > 0) {
			collision_info->normal_ = _normal;
			collision_info->separation_distance_ = abs(box_projection) - dist_projection;
		} else {
			collision_info->normal_ = -_normal;
			collision_info->separation_distance_ = abs(box_projection) + dist_projection;
		}
	}

	_normal = extent_x;
	_normal.Normalize();

	dist_projection = _normal.Dot(dist);
	box_projection = box.GetSize().x;
	_TVarType line_projection = _normal.Dot(_dir);
	_TVarType line_box_projection = abs(line_projection) + box_projection;

	if(abs(dist_projection) > line_box_projection) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		_TVarType separation_distance = line_box_projection - abs(dist_projection);
		if(separation_distance < collision_info->separation_distance_) {
			collision_info->separation_distance_ = separation_distance;
			if(dist_projection > 0)
				collision_info->normal_ = _normal;
			else
				collision_info->normal_ = -_normal;
		}
	}

	_normal = extent_y;
	_normal.Normalize();

	dist_projection = _normal.Dot(dist);
	box_projection = box.GetSize().y;
	line_projection = _normal.Dot(_dir);
	line_box_projection = abs(line_projection) + box_projection;

	if(abs(dist_projection) > line_box_projection) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	} else if(collision_info != 0) {
		_TVarType separation_distance = line_box_projection - abs(dist_projection);
		if(separation_distance < collision_info->separation_distance_) {
			collision_info->separation_distance_ = separation_distance;
			if(dist_projection > 0)
				collision_info->normal_ = _normal;
			else
				collision_info->normal_ = -_normal;
		}

		collision_info->time_to_collision_ = 0;

		// All code below calculates the point of collision.
		if(poc_enabled_) {
			Vector2D<_TVarType> corner1 = box.GetPosition() + extent_x + extent_y;
			Vector2D<_TVarType> corner2 = box.GetPosition() + extent_x - extent_y;
			Vector2D<_TVarType> corner3 = box.GetPosition() - extent_x - extent_y;
			Vector2D<_TVarType> corner4 = box.GetPosition() - extent_x + extent_y;

			CollisionInfo c_info[4];
			int index1 = -1;
			int index2 = -1;
			if(StaticLineSegmentToLineSegmentTest(start, end, corner1, corner2, &c_info[0])) {
				index1 = 0;
			}
			if(StaticLineSegmentToLineSegmentTest(start, end, corner2, corner3, &c_info[1])) {
				if(index1 < 0)
					index1 = 1;
				else
					index2 = 1;
			}
			if((index1 < 0 || index2 < 0) && StaticLineSegmentToLineSegmentTest(start, end, corner3, corner4, &c_info[2])) {
				if(index1 < 0)
					index1 = 2;
				else
					index2 = 2;
			}
			if((index1 < 0 || index2 < 0) && StaticLineSegmentToLineSegmentTest(start, end, corner4, corner1, &c_info[3])) {
				if(index1 < 0)
					index1 = 3;
				else
					index2 = 3;
			}

			if(index2 < 0) {
				// TODO: Fix. This is not following the definition. It should be
				// (c_info[index1].point_of_collision_ + start) * 0.5 or
				// (c_info[index1].point_of_collision_ + end) * 0.5.
				// The problem is just to figure out which one.
				collision_info->point_of_collision_ = c_info[index1].point_of_collision_;
			} else {
				collision_info->point_of_collision_ = (c_info[index1].point_of_collision_ + c_info[index2].point_of_collision_) * (_TVarType)0.5;
			}
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticOBRToTriangleTest(const OBR<_TVarType>& box,
							     const Vector2D<_TVarType> tri[3],
							     CollisionInfo* collision_info) {
	if(collision_info != 0)
		collision_info->time_to_collision_ = 1;

	 // Find out wether the triangle is clockwise.
	Vector2D<_TVarType> perp((tri[1] - tri[0]).GetPerpCW());
	_TVarType p = perp.Dot(tri[2] - tri[0]);

	// Copy triangle data and make sure it is stored in clockwise order.
	Vector2D<_TVarType> _tri[3];

	if(p >= 0) {
		_tri[0] = tri[0];
		_tri[1] = tri[1];
		_tri[2] = tri[2];
	} else {
		_tri[0] = tri[0];
		_tri[1] = tri[2];
		_tri[2] = tri[1];
	}

	// Find the corners of the box, store them in clockwise order.
	Vector2D<_TVarType> extent_x(box.GetExtentX());
	Vector2D<_TVarType> extent_y(box.GetExtentY());
	Vector2D<_TVarType> _box[4];
	_box[0] = box.GetPosition() - extent_x - extent_y;
	_box[1] = box.GetPosition() - extent_x + extent_y;
	_box[2] = box.GetPosition() + extent_x + extent_y;
	_box[3] = box.GetPosition() + extent_x - extent_y;


	// We can't use the regular technique of separating "planes" (lines) as usual,
	// since the triangles are not (in general) symmetric along any axis. Instead,
	// the separating line has to be fully defined (as opposed to only knowing the
	// slope). Each side of the triangle and the OBR defines one such line, against
	// which we have to clip the other shape before applying the method of separating
	// lines.

	// Now find the overlap.
	Vector2D<_TVarType> out[7];
	Vector2D<_TVarType> edge(_tri[1] - _tri[0]);
	int v_count;
	if((v_count = ClipPolyAgainstLine(_box, 4, _tri[0], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri[0]));
		collision_info->separation_distance_ = _projection;
		collision_info->normal_ = -n;
	}

	edge = _tri[2] - _tri[1];
	if((v_count = ClipPolyAgainstLine(_box, 4, _tri[1], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri[1]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = -n;
		}
	}

	edge = _tri[0] - _tri[2];
	if((v_count = ClipPolyAgainstLine(_box, 4, _tri[2], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _tri[2]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = -n;
		}
	}

	edge = _box[1] - _box[0];
	if((v_count = ClipPolyAgainstLine(_tri, 3, _box[0], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _box[0]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = n;
		}
	}

	edge = _box[2] - _box[1];
	if((v_count = ClipPolyAgainstLine(_tri, 3, _box[1], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _box[1]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = n;
		}
	}

	edge = _box[3] - _box[2];
	if((v_count = ClipPolyAgainstLine(_tri, 3, _box[2], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _box[2]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = n;
		}
	}

	edge = _box[0] - _box[3];
	if((v_count = ClipPolyAgainstLine(_tri, 3, _box[3], edge, out)) == 0)
		return false;
	if(collision_info != 0) {
		Vector2D<_TVarType> n(edge.y, -edge.x);
		n.Normalize();
		_TVarType _projection = 0;
		for(int i = 0; i < v_count; i++)
			_projection += abs(n.Dot(out[i] - _box[3]));
		if(_projection < collision_info->separation_distance_) {
			collision_info->separation_distance_ = _projection;
			collision_info->normal_ = n;
		}

		collision_info->time_to_collision_ = 0;

		if(poc_enabled_) {
			// Now we have to find the overlap in order to find the point of collision.
			Vector2D<_TVarType> temp[7];
			int v_count;
			v_count = ClipPolyAgainstLine(_box, 4, _tri[0], _tri[1] - _tri[0], out);
			v_count = ClipPolyAgainstLine(out, v_count, _tri[1], _tri[2] - _tri[1], temp);
			v_count = ClipPolyAgainstLine(temp, v_count, _tri[2], _tri[0] - _tri[2], out);

			collision_info->point_of_collision_.Set(0, 0);
			int i;
			for(i = 0; i < v_count; i++) {
				collision_info->point_of_collision_ += out[i];
			}
			collision_info->point_of_collision_ /= (_TVarType)v_count;
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticCircleToPointTest(const Circle<_TVarType>& circle,
							     const Vector2D<_TVarType>& point,
							     CollisionInfo* collision_info) {
	_TVarType distance_sq = circle.GetPosition().GetDistanceSquared(point);
	if(distance_sq > circle.GetRadiusSquared()) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 0;
		return false;
	}

	if(collision_info != 0) {
		collision_info->time_to_collision_ = 1;
		collision_info->point_of_collision_ = point;
		collision_info->normal_ = circle.GetPosition() - point;
		_TVarType distance = collision_info->normal_.GetLength();
		collision_info->separation_distance_ = circle.GetRadius() - distance;

		if(distance == 0)
			collision_info->normal_.Set(1, 0);
		else
			collision_info->normal_ /= distance; // Normalize.
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticCircleToLineSegmentTest(const Circle<_TVarType>& circle,
								   const Vector2D<_TVarType>& start,
								   const Vector2D<_TVarType>& end,
								   CollisionInfo* collision_info) {
	Vector2D<_TVarType> _dir(end - start);
	Vector2D<_TVarType> _closest_point;
	_TVarType start_to_cp_dist;
	_TVarType distance = LineDistance(start, _dir, circle.GetPosition(), _closest_point, start_to_cp_dist);

	if(distance > circle.GetRadius()) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	bool check_against_cp = false;
	_TVarType line_length = _dir.GetLength();
	if(start_to_cp_dist < 0) {
		_closest_point = start;
		check_against_cp = true;
	} else if(start_to_cp_dist > line_length) {
		_closest_point = end;
		check_against_cp = true;
	}

	if(check_against_cp == true) {
		distance = circle.GetPosition().GetDistance(_closest_point);

		if(distance > circle.GetRadius()) {
			if(collision_info != 0)
				collision_info->time_to_collision_ = 1;
			return false;
		}
	}

	if(collision_info != 0) {
		collision_info->time_to_collision_ = 0;
		collision_info->point_of_collision_ = _closest_point;
		collision_info->normal_ = circle.GetPosition() - _closest_point;
		collision_info->normal_.Normalize();
		collision_info->separation_distance_ = circle.GetRadius() - distance;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector2D<_TVarType>::StaticCircleToTriangleTest(const Circle<_TVarType>& circle,
								const Vector2D<_TVarType> tri[3],
								CollisionInfo* collision_info) {
	Vector2D<_TVarType> cp; // Closest point.
	Vector2D<_TVarType> _normal;
	_TVarType _projection;
	_TVarType distance = TriangleDistance(tri, circle.GetPosition(), cp, _normal, _projection);

	if(distance > circle.GetRadius()) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		if(distance == 0) {
			collision_info->normal_ = _normal;
			collision_info->separation_distance_ = _projection + circle.GetRadius();
		} else {
			collision_info->normal_ = circle.GetPosition() - cp;
			collision_info->normal_.Normalize();
			collision_info->separation_distance_ = circle.GetRadius() - distance;
		}

		collision_info->time_to_collision_ = 0;
		collision_info->point_of_collision_ = cp;
	}

	return true;
}

template<class _TVarType>
int CollisionDetector2D<_TVarType>::ClipPolyAgainstLine(Vector2D<_TVarType>* poly, int vertex_count,
							const Vector2D<_TVarType>& point_on_line,
							const Vector2D<_TVarType>& dir,
							Vector2D<_TVarType>* poly_out) {
	Vector2D<_TVarType> _normal(dir.y, -dir.x);
	_normal.Normalize();
	_TVarType c = -_normal.Dot(point_on_line);

	int out_v_count = 0;
	int _start = vertex_count - 1;
	int _end = 0;
	while(_end < vertex_count) {
		_TVarType s = _normal.Dot(poly[_start]) + c;
		_TVarType e = _normal.Dot(poly[_end]) + c;

		if(s <= 0) {
			if(e > 0) {
				// Exiting the clipping plane. Start point clipped, store start point.
				const _TVarType t = -s / (e - s);
				poly_out[out_v_count++] = poly[_start] + (poly[_end] - poly[_start]) * t;
			}
		} else {
			if (e <= 0) {
				// Entering the clipping plane. Store both points. The first point is just copied.
				poly_out[out_v_count++] = poly[_start];
				const _TVarType t = -e / (s - e);
				poly_out[out_v_count++] = poly[_end] + (poly[_start] - poly[_end]) * t;
			} else {
				// Both points are visible, store the start point.
				poly_out[out_v_count++] = poly[_start];
			}
		}
		_start = _end;
		_end++;
	}

	return out_v_count;
}

template<class _TVarType>
_TVarType CollisionDetector2D<_TVarType>::TriangleDistance(const Vector2D<_TVarType> tri[3],
							   const Vector2D<_TVarType>& point,
							   Vector2D<_TVarType>& closest_point,
							   Vector2D<_TVarType>& normal,
							   _TVarType& projection) {
	Vector2D<_TVarType> dir0(tri[1] - tri[0]);
	Vector2D<_TVarType> dir1(tri[2] - tri[1]);
	Vector2D<_TVarType> dir2(tri[0] - tri[2]);

	// Relative Position.
	Vector2D<_TVarType> r_p0(point - tri[0]);
	Vector2D<_TVarType> r_p1(point - tri[1]);
	Vector2D<_TVarType> r_p2(point - tri[2]);

	Vector2D<_TVarType> normal0(dir0.y, -dir0.x);
	Vector2D<_TVarType> normal1(dir1.y, -dir1.x);
	Vector2D<_TVarType> normal2(dir2.y, -dir2.x);

	_TVarType n_p0 = normal0.Dot(r_p0);
	_TVarType n_p1 = normal1.Dot(r_p1);
	_TVarType n_p2 = normal2.Dot(r_p2);

	if((n_p0 <= 0 && n_p1 <= 0 && n_p2 <= 0) || (n_p0 >= 0 && n_p1 >= 0 && n_p2 >= 0)) {
		// Inside the triangle.
		closest_point = point;

		projection = abs(n_p0);
		normal = normal0;
		_TVarType abs_n_p1 = abs(n_p1);
		_TVarType abs_n_p2 = abs(n_p2);
		if(abs_n_p1 < projection) {
			projection = abs_n_p1;
			normal = normal1;
		}
		if(abs_n_p2 < projection) {
			projection = abs_n_p2;
			normal = normal2;
		}

		_TVarType length = normal.GetLength();
		normal /= length;
		projection /= length;

		if(n_p0 > 0) {
			normal = -normal;
		}

		return 0;
	}

	_TVarType l0 = dir0.GetLength();
	_TVarType l1 = dir1.GetLength();
	_TVarType l2 = dir2.GetLength();

	// Normalize.
	dir0 /= l0;
	dir1 /= l1;
	dir2 /= l2;

	_TVarType d_p0 = dir0.Dot(r_p0);
	_TVarType d_p1 = dir1.Dot(r_p1);
	_TVarType d_p2 = dir2.Dot(r_p2);

	Vector2D<_TVarType> c_p0;
	Vector2D<_TVarType> c_p1;
	Vector2D<_TVarType> c_p2;
	if(d_p0 < 0)
		c_p0 = tri[0];
	else if(d_p0 > l0)
		c_p0 = tri[1];
	else
		c_p0 = tri[0] + dir0 * d_p0;

	if(d_p1 < 0)
		c_p1 = tri[1];
	else if(d_p1 > l1)
		c_p1 = tri[2];
	else
		c_p1 = tri[1] + dir1 * d_p1;

	if(d_p2 < 0)
		c_p2 = tri[2];
	else if(d_p2 > l2)
		c_p2 = tri[0];
	else
		c_p2 = tri[2] + dir2 * d_p2;

	_TVarType min_distance = c_p0.GetDistance(point);
	closest_point = c_p0;

	_TVarType distance = c_p1.GetDistance(point);
	if(distance < min_distance) {
		min_distance = distance;
		closest_point = c_p1;
	}

	distance = c_p2.GetDistance(point);
	if(distance < min_distance) {
		min_distance = distance;
		closest_point = c_p2;
	}

	return min_distance;
}

/*
namespace Collision {

	bool CollisionDetector2D<_TVarType>::StaticAABRToTriangleTest(const AABR2D& box, const Triangle2D& triangle, Vector2D& pSeparation) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::StaticOBRToTriangleTest(const OBR2D& box, const Triangle2D& triangle, Vector2D& pSeparation) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::StaticCircleToTriangleTest(const Circle& circle, const Triangle2D& triangle, Vector2D& pSeparation) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::StaticTriangleToPointTest(const Triangle2D& triangle, const Vector2D& point, Vector2D& pSeparation) {
		return false;
	}


	bool CollisionDetector2D<_TVarType>::VelocityAABRToPointTest(const AABR2D& box,
								 const Vector2D& point,
								 const Vector2D& pBoxVelocity,
								 const Vector2D& pPointVelocity,
								 _TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityAABRToOBRTest(const AABR2D& box1,
							   const OBR2D& box2,
  							   const Vector2D& box1_velocity,
							   const Vector2D& box2_velocity,
							   _TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityAABRToCircleTest(const AABR2D& box,
								  const Circle& circle,
  								  const Vector2D& pBoxVelocity,
								  const Vector2D& pCircleVelocity,
								  _TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityAABRToTriangleTest(const AABR2D& box,
									const Triangle2D& triangle,
  									const Vector2D& pBoxVelocity,
									const Vector2D& pTriangleVelocity,
									_TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityOBRToPointTest(const OBR2D& box,
								const Vector2D& point,
								const Vector2D& pBoxVelocity,
								const Vector2D& pPointVelocity,
								_TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityOBRToCircleTest(const OBR2D& box,
								 const Circle& circle,
								 const Vector2D& pBoxVelocity,
								 const Vector2D& pCircleVelocity,
								 _TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityOBRToTriangleTest(const OBR2D& box,
								   const Triangle2D& triangle,
								   const Vector2D& pBoxVelocity,
								   const Vector2D& pTriangleVelocity,
								   _TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityCircleToPointTest(const Circle& circle,
								   const Vector2D& point,
								   const Vector2D& pCircleVelocity,
								   const Vector2D& pPointVelocity,
								   _TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityCircleToTriangleTest(const Circle& circle,
									  const Triangle2D& triangle,
									  const Vector2D& pCircleVelocity,
									  const Vector2D& pTriangleVelocity,
									  _TVarType& time_to_collision) {
		return false;
	}

	bool CollisionDetector2D<_TVarType>::VelocityTriangleToPointTest(const Triangle2D& triangle,
									 const Vector2D& point,
									 const Vector2D& pTriangleVelocity,
									 const Vector2D& pPointVelocity,
									 _TVarType& time_to_collision) {
		return false;
	}

}
*/
