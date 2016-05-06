/*
	Class:  CollisionDetector3D
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

template<class _TVarType>
CollisionDetector3D<_TVarType>::CollisionDetector3D() {
	obb_collision_data_ = new OBBCollisionData<_TVarType>;
}

template<class _TVarType>
CollisionDetector3D<_TVarType>::~CollisionDetector3D() {
	delete obb_collision_data_;
}


//////////////////////////////////////////////////////////////////////////////
//
// Point tests...
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsSphereEnclosingPoint(const Sphere<_TVarType>& sphere,
							    const Vector3D<_TVarType>& point) {
	_TVarType dist_squared = sphere.GetPosition().GetDistanceSquared(point);
	return (dist_squared < sphere.GetRadiusSquared());
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBEnclosingPoint(const AABB<_TVarType>& aabb,
							  const Vector3D<_TVarType>& point) {
	Vector3D<_TVarType> __min(aabb.GetPosition() - aabb.GetSize());
	Vector3D<_TVarType> __max(aabb.GetPosition() + aabb.GetSize());

	return (__min.x <= point.x && __max.x > point.x &&
		__min.y <= point.y && __max.y > point.y &&
		__min.z <= point.z && __max.z > point.z);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBEnclosingPoint(const OBB<_TVarType>& obb,
							 const Vector3D<_TVarType>& point) {
	Vector3D<_TVarType> pos_diff(point - obb.GetPosition());
	const Vector3D<_TVarType>& obb_size = obb.GetSize();

	_TVarType d = pos_diff.Dot(obb.GetRotation().GetAxisX());
	if(d > obb_size.x || d < -obb_size.x)
		return false;

	d = pos_diff.Dot(obb.GetRotation().GetAxisY());
	if(d > obb_size.y || d < -obb_size.y)
		return false;

	d = pos_diff.Dot(obb.GetRotation().GetAxisZ());
	if(d > obb_size.z || d < -obb_size.z)
		return false;

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAACylinderEnclosingPoint(const Cylinder<_TVarType>& aa_cylinder,
								const Vector3D<_TVarType>& point) {
	Vector3D<_TVarType> pos_diff(point - aa_cylinder.GetPosition());

	switch(aa_cylinder.GetAlignment()) {
		case Cylinder<_TVarType>::kAlignX: {
			_TVarType dist_squared = pos_diff.y * pos_diff.y + pos_diff.z * pos_diff.z;
			if(dist_squared < aa_cylinder.GetRadiusSquared() &&
			   pos_diff.x > -aa_cylinder.GetLength() && pos_diff.x < aa_cylinder.GetLength()) {
				return true;
			}
		}
		case Cylinder<_TVarType>::kAlignY: {
			_TVarType dist_squared = pos_diff.x * pos_diff.x + pos_diff.z * pos_diff.z;
			if(dist_squared < aa_cylinder.GetRadiusSquared() &&
			   pos_diff.y > -aa_cylinder.GetLength() && pos_diff.y < aa_cylinder.GetLength()) {
				return true;
			}
		}
		case Cylinder<_TVarType>::kAlignZ: {
			_TVarType dist_squared = pos_diff.x * pos_diff.x + pos_diff.y * pos_diff.y;
			if(dist_squared < aa_cylinder.GetRadiusSquared() &&
			   pos_diff.z > -aa_cylinder.GetLength() && pos_diff.z < aa_cylinder.GetLength()) {
				return true;
			}
		}
	}
	return false;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsCylinderEnclosingPoint(const Cylinder<_TVarType>& cylinder,
							      const Vector3D<_TVarType>& point) {
	Vector3D<_TVarType> relative_pos(cylinder.GetRotation().GetInverseRotatedVector(point - cylinder.GetPosition()));

	switch(cylinder.GetAlignment()) {
		case Cylinder<_TVarType>::kAlignX: {
			_TVarType dist_squared = relative_pos.y * relative_pos.y + relative_pos.z * relative_pos.z;
			if(dist_squared < cylinder.GetRadiusSquared() &&
			   relative_pos.x > -cylinder.GetLength() && relative_pos.x < cylinder.GetLength()) {
				return true;
			}
		}
		case Cylinder<_TVarType>::kAlignY: {
			_TVarType dist_squared = relative_pos.x * relative_pos.x + relative_pos.z * relative_pos.z;
			if(dist_squared < cylinder.GetRadiusSquared() &&
			   relative_pos.y > -cylinder.GetLength() && relative_pos.y < cylinder.GetLength()) {
				return true;
			}
		}
		case Cylinder<_TVarType>::kAlignZ: {
			_TVarType dist_squared = relative_pos.x * relative_pos.x + relative_pos.y * relative_pos.y;
			if(dist_squared < cylinder.GetRadiusSquared() &&
			   relative_pos.z > -cylinder.GetLength() && relative_pos.z < cylinder.GetLength()) {
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
bool CollisionDetector3D<_TVarType>::IsSphere1EnclosingSphere2(const Sphere<_TVarType>& sphere1,
							       const Sphere<_TVarType>& sphere2) {
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

	_TVarType distance = sphere1.GetPosition().GetDistance(sphere2.GetPosition());
	return ((distance + sphere2.GetRadius()) <= sphere1.GetRadius());
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABB1EnclosingAABB2(const AABB<_TVarType>& aab_b1,
							   const AABB<_TVarType>& aab_b2) {
	Vector3D<_TVarType> min1(aab_b1.GetPosition() - aab_b1.GetSize());
	Vector3D<_TVarType> max1(aab_b1.GetPosition() + aab_b1.GetSize());
	Vector3D<_TVarType> min2(aab_b2.GetPosition() - aab_b2.GetSize());
	Vector3D<_TVarType> max2(aab_b2.GetPosition() - aab_b2.GetSize());

	return(max1.x >= max2.x && min1.x <= min2.x &&
	       max1.y >= max2.y && min1.y <= min2.y &&
	       max1.z >= max2.z && min1.z <= min2.z);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBB1EnclosingOBB2(const OBB<_TVarType>& ob_b1,
							 const OBB<_TVarType>& ob_b2) {
	// OBB2 is only enclosed if all it's vertices are enclosed.
	Vector3D<_TVarType> x_ext(ob_b2.GetRotation().GetAxisX() * ob_b2.GetSize().x);
	Vector3D<_TVarType> y_ext(ob_b2.GetRotation().GetAxisY() * ob_b2.GetSize().y);
	Vector3D<_TVarType> z_ext(ob_b2.GetRotation().GetAxisZ() * ob_b2.GetSize().z);
	Vector3D<_TVarType> v(ob_b2.GetPosition() - x_ext - y_ext - z_ext);

	if(IsOBBEnclosingPoint(ob_b1, v) == false)
		return false;

	x_ext *= 2;
	v += x_ext;
	if(IsOBBEnclosingPoint(ob_b1, v) == false)
		return false;

	y_ext *= 2;
	v += y_ext;
	if(IsOBBEnclosingPoint(ob_b1, v) == false)
		return false;

	z_ext *= 2;
	v += z_ext;
	if(IsOBBEnclosingPoint(ob_b1, v) == false)
		return false;

	v -= y_ext;
	if(IsOBBEnclosingPoint(ob_b1, v) == false)
		return false;

	v -= x_ext;
	if(IsOBBEnclosingPoint(ob_b1, v) == false)
		return false;

	v += y_ext;
	if(IsOBBEnclosingPoint(ob_b1, v) == false)
		return false;

	v -= z_ext;
	if(IsOBBEnclosingPoint(ob_b1, v) == false)
		return false;

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAACylinder1EnclosingAACylinder2(const Cylinder<_TVarType>& cylinder1,
									const Cylinder<_TVarType>& cylinder2) {
	deb_assert(cylinder1.GetAlignment() == cylinder2.GetAlignment());

	// It's impossible for a smaller cylinder to enclose a larger one.
	if(cylinder1.GetRadius() < cylinder2.GetRadius())
		return false;

	Vector3D<_TVarType> d(cylinder2.GetPosition() - cylinder1.GetPosition());
	_TVarType coord_to_test;

	switch(cylinder1.GetAlignment()) {
		case Cylinder<_TVarType>::kAlignX: coord_to_test = d.x; break;
		case Cylinder<_TVarType>::kAlignY: coord_to_test = d.y; break;
		case Cylinder<_TVarType>::kAlignZ: coord_to_test = d.z; break;
	}

	if((coord_to_test + cylinder2.GetLength()) > cylinder1.GetLength())
		return false;
	if((coord_to_test - cylinder2.GetLength()) < -cylinder1.GetLength())
		return false;

	_TVarType dist_squared;
	switch(cylinder1.GetAlignment()) {
		case Cylinder<_TVarType>::kAlignX: dist_squared = d.y*d.y + d.z*d.z; break;
		case Cylinder<_TVarType>::kAlignY: dist_squared = d.x*d.x + d.z*d.z; break;
		case Cylinder<_TVarType>::kAlignZ: dist_squared = d.x*d.x + d.y*d.y; break;
	}

	if((dist_squared + cylinder2.GetRadiusSquared()) > cylinder1.GetRadiusSquared())
		return false;

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsSphereEnclosingAABB(const Sphere<_TVarType>& sphere,
							   const AABB<_TVarType>& aabb) {
	// This test is similar to the enclosing sphere to sphere test.
	_TVarType aabb_radius = aabb.GetSize().GetLength();
	_TVarType dist = sphere.GetPosition().GetDistance(aabb.GetPosition());
	return ((dist + aabb_radius) <= sphere.GetRadius());
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBEnclosingSphere(const AABB<_TVarType>& aabb,
							   const Sphere<_TVarType>& sphere) {
	Vector3D<_TVarType> __min(aabb.GetPosition() - aabb.GetSize());
	Vector3D<_TVarType> __max(aabb.GetPosition() + aabb.GetSize());
	Vector3D<_TVarType> r(sphere.GetRadius(), sphere.GetRadius(), sphere.GetRadius());
	Vector3D<_TVarType> s_min(sphere.GetPosition() - r);
	Vector3D<_TVarType> s_max(sphere.GetPosition() + r);

	return (__min.x < s_min.x && __max.x > s_max.x &&
	        __min.y < s_min.y && __max.y > s_max.y &&
	        __min.z < s_min.z && __max.z > s_max.z);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBEnclosingSphere(const OBB<_TVarType>& obb,
							  const Sphere<_TVarType>& sphere) {
	Vector3D<_TVarType> relative_pos(obb.GetRotation().GetInverseRotatedVector(sphere.GetPosition() - obb.GetPosition()));
	Vector3D<_TVarType> r(sphere.GetRadius(), sphere.GetRadius(), sphere.GetRadius());
	Vector3D<_TVarType> s_min(relative_pos - r);
	Vector3D<_TVarType> s_max(relative_pos + r);

	return (-obb.GetSize().x < s_min.x && obb.GetSize().x > s_max.x &&
	        -obb.GetSize().y < s_min.y && obb.GetSize().y > s_max.y &&
	        -obb.GetSize().z < s_min.z && obb.GetSize().z > s_max.z);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBEnclosingOBB(const AABB<_TVarType>& aabb,
							const OBB<_TVarType>& obb) {
	Vector3D<_TVarType> __min(aabb.GetPosition() - aabb.GetSize());
	Vector3D<_TVarType> __max(aabb.GetPosition() + aabb.GetSize());

	Vector3D<_TVarType> x_ext(obb.GetRotation().GetAxisX() * obb.GetSize().x);
	Vector3D<_TVarType> y_ext(obb.GetRotation().GetAxisY() * obb.GetSize().y);
	Vector3D<_TVarType> z_ext(obb.GetRotation().GetAxisZ() * obb.GetSize().z);
	Vector3D<_TVarType> v(obb.GetPosition() - x_ext - y_ext - z_ext);

	if(v.x < __min.x || v.x > __max.x ||
	   v.y < __min.y || v.y > __max.y ||
	   v.z < __min.z || v.z > __max.z) {
		return false;
	}

	x_ext *= 2;
	v += x_ext;
	if(v.x < __min.x || v.x > __max.x ||
	   v.y < __min.y || v.y > __max.y ||
	   v.z < __min.z || v.z > __max.z) {
		return false;
	}

	y_ext *= 2;
	v += y_ext;
	if(v.x < __min.x || v.x > __max.x ||
	   v.y < __min.y || v.y > __max.y ||
	   v.z < __min.z || v.z > __max.z) {
		return false;
	}

	z_ext *= 2;
	v += z_ext;
	if(v.x < __min.x || v.x > __max.x ||
	   v.y < __min.y || v.y > __max.y ||
	   v.z < __min.z || v.z > __max.z) {
		return false;
	}

	v -= y_ext;
	if(v.x < __min.x || v.x > __max.x ||
	   v.y < __min.y || v.y > __max.y ||
	   v.z < __min.z || v.z > __max.z) {
		return false;
	}

	v -= x_ext;
	if(v.x < __min.x || v.x > __max.x ||
	   v.y < __min.y || v.y > __max.y ||
	   v.z < __min.z || v.z > __max.z) {
		return false;
	}

	v += y_ext;
	if(v.x < __min.x || v.x > __max.x ||
	   v.y < __min.y || v.y > __max.y ||
	   v.z < __min.z || v.z > __max.z) {
		return false;
	}

	v -= z_ext;
	if(v.x < __min.x || v.x > __max.x ||
	   v.y < __min.y || v.y > __max.y ||
	   v.z < __min.z || v.z > __max.z) {
		return false;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBEnclosingAABB(const OBB<_TVarType>& obb,
							const AABB<_TVarType>& aabb) {
	// Invert the situation and test the opposite (if AABB is enclosing OBB).
	// This way if more optimal with 9 muls in this function (vector rotation),
	// and 9 more in IsAABBEnclosingOBB(), than testing each vertex of the AABB
	// against the OBB, which would require 72 muls (9 per vertex).
	Vector3D<_TVarType> rotated_pos(obb.GetRotation().GetInverseRotatedVector(aabb.GetPosition() - obb.GetPosition()) + obb.GetPosition());
	AABB<_TVarType> _aabb(obb.GetPosition(), obb.GetSize());
	OBB<_TVarType> _obb(rotated_pos, obb.GetSize(), obb.GetRotation().GetInverse());
	return IsAABBEnclosingOBB(_aabb, _obb);
}



//////////////////////////////////////////////////////////////////////////////
//
// Public overlapping test functions.
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBOverlappingAABB(const AABB<_TVarType>& aab_b1,
							   const AABB<_TVarType>& aab_b2,
							   CollisionInfo* collision_info) {
	Vector3D<_TVarType> min_dist(aab_b1.GetSize() + aab_b2.GetSize());
	Vector3D<_TVarType> dist(aab_b1.GetPosition() - aab_b2.GetPosition());

	if(dist.x < 0.0f)
		dist.x = -dist.x;
	if(dist.y < 0.0f)
		dist.y = -dist.y;
	if(dist.z < 0.0f)
		dist.z = -dist.z;

	if(dist.x > min_dist.x || dist.y > min_dist.y || dist.z > min_dist.z) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		Vector3D<_TVarType> box1_min(aab_b1.GetPosition() - aab_b1.GetSize());
		Vector3D<_TVarType> box1_max(aab_b1.GetPosition() + aab_b1.GetSize());
		Vector3D<_TVarType> box2_min(aab_b2.GetPosition() - aab_b2.GetSize());
		Vector3D<_TVarType> box2_max(aab_b2.GetPosition() + aab_b2.GetSize());

		Vector3D<_TVarType> overlap_min(std::max(box1_min.x, box2_min.x), std::max(box1_min.y, box2_min.y), std::max(box1_min.z, box2_min.z));
		Vector3D<_TVarType> overlap_max(std::min(box1_max.x, box2_max.x), std::min(box1_max.y, box2_max.y), std::min(box1_max.z, box2_max.z));

		collision_info->time_to_collision_ = 0;
		collision_info->point_of_collision_ = (overlap_min + overlap_max) * (_TVarType)0.5;

		//Always positive.
		Vector3D<_TVarType> diff(min_dist - dist);

		if(diff.x < diff.y) {
			if(diff.x < diff.z) {
				if(aab_b1.GetPosition().x > aab_b2.GetPosition().x)
					collision_info->normal_.Set(1, 0, 0);
				else
					collision_info->normal_.Set(-1, 0, 0);
				collision_info->separation_distance_ = diff.x;
			} else {
				if(aab_b1.GetPosition().z > aab_b2.GetPosition().z)
					collision_info->normal_.Set(0, 0, 1);
				else
					collision_info->normal_.Set(0, 0, -1);
				collision_info->separation_distance_ = diff.z;
			}
		} else {
			if(diff.y < diff.z) {
				if(aab_b1.GetPosition().y > aab_b2.GetPosition().y)
					collision_info->normal_.Set(0, 1, 0);
				else
					collision_info->normal_.Set(0, -1, 0);
				collision_info->separation_distance_ = diff.y;
			} else {
				if(aab_b1.GetPosition().z > aab_b2.GetPosition().z)
					collision_info->normal_.Set(0, 0, 1);
				else
					collision_info->normal_.Set(0, 0, -1);
				collision_info->separation_distance_ = diff.z;
			}
		}
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBOverlappingOBB(const OBB<_TVarType>& ob_b1,
							 const OBB<_TVarType>& ob_b2,
							 CollisionInfo* collision_info) {
	if(collision_info)
		return IsOBBOverlappingOBB(ob_b1, ob_b2, *collision_info);
	else
		return IsOBBOverlappingOBBOptimized(ob_b1, ob_b2);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsSphereOverlappingSphere(const Sphere<_TVarType>& sphere1,
							       const Sphere<_TVarType>& sphere2,
							       CollisionInfo* collision_info) {
	Vector3D<_TVarType> dist(sphere1.GetPosition() - sphere2.GetPosition());
	_TVarType min_distance = (sphere1.GetRadius() + sphere2.GetRadius());

	if(dist.GetLengthSquared() > (min_distance * min_distance)) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		_TVarType length = dist.GetLength();
		collision_info->normal_ = dist;
		collision_info->normal_ /= length; // Normalize
		collision_info->separation_distance_ = (_TVarType)(min_distance - length);
		collision_info->point_of_collision_ = sphere2.GetPosition() + collision_info->normal_ * (sphere2.GetRadius() - collision_info->separation_distance_ * (_TVarType)0.5);
		collision_info->time_to_collision_ = 0;
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBOverlappingOBB(const AABB<_TVarType>& aabb,
							  const OBB<_TVarType>& obb,
							  CollisionInfo* collision_info) {
	OBB<_TVarType> _obb(aabb.GetPosition(), aabb.GetSize(), RotationMatrix<_TVarType>());
	if(collision_info)
		return IsOBBOverlappingOBB(_obb, obb, *collision_info);
	else
		return IsOBBOverlappingOBB(_obb, obb);
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsAABBOverlappingSphere(const AABB<_TVarType>& aabb,
							     const Sphere<_TVarType>& sphere,
							     CollisionInfo* collision_info) {
	Vector3D<_TVarType> __min(aabb.GetPosition() - aabb.GetSize());
	Vector3D<_TVarType> __max(aabb.GetPosition() + aabb.GetSize());
	Vector3D<_TVarType> point_on_box(0, 0, 0);

	if(sphere.GetPosition().x < __min.x)
		point_on_box.x = __min.x;
	else if(sphere.GetPosition().x > __max.x)
		point_on_box.x = __max.x;
	else
		point_on_box.x = sphere.GetPosition().x;

	if(sphere.GetPosition().y < __min.y)
		point_on_box.y = __min.y;
	else if(sphere.GetPosition().y > __max.y)
		point_on_box.y = __max.y;
	else
		point_on_box.y = sphere.GetPosition().y;

	if(sphere.GetPosition().z < __min.z)
		point_on_box.z = __min.z;
	else if(sphere.GetPosition().z > __max.z)
		point_on_box.z = __max.z;
	else
		point_on_box.z = sphere.GetPosition().z;

	Vector3D<_TVarType> dist(point_on_box - sphere.GetPosition());
	_TVarType distance_squared = dist.GetLengthSquared();

	if(distance_squared > sphere.GetRadiusSquared()) {
		if(collision_info != 0)
			collision_info->time_to_collision_ = 1;
		return false;
	}

	if(collision_info != 0) {
		collision_info->time_to_collision_ = 0;
		collision_info->normal_ = dist;
		_TVarType length = dist.GetLength();
		collision_info->normal_ /= length; // Normalize.
		collision_info->separation_distance_ = sphere.GetRadius() - length;
		collision_info->point_of_collision_ = sphere.GetPosition() + collision_info->normal_ * ((sphere.GetRadius() + length) * (_TVarType)0.5);
	}

	return true;
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBOverlappingSphere(const OBB<_TVarType>& obb,
							    const Sphere<_TVarType>& sphere,
							    CollisionInfo* collision_info) {
	Vector3D<_TVarType> pos_diff(sphere.GetPosition() - obb.GetPosition());
	Vector3D<_TVarType> rot_pos(obb.GetRotation().GetInverseRotatedVector(pos_diff) + obb.GetPosition());

	AABB<_TVarType> _aabb(obb.GetPosition(), obb.GetSize());
	Sphere<_TVarType> _sphere(rot_pos, sphere.GetRadius());

	if(collision_info) {
		bool overlapping = IsAABBOverlappingSphere(_aabb, sphere, *collision_info);
		if(overlapping) {
			collision_info->point_of_collision_ = obb.GetRotation().GetRotatedVector(collision_info->point_of_collision_ - obb.GetPosition()) + obb.GetPosition();
			collision_info->normal_ = obb.GetRotation().GetRotatedVector(collision_info->normal_);
		}
		return overlapping;
	} else {
		return IsAABBOverlappingSphere(_aabb, sphere);
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Optimized overlapping test function.
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::IsOBBOverlappingOBBOptimized(const OBB<_TVarType>& ob_b1,
								  const OBB<_TVarType>& ob_b2) {
	_TVarType epsilon = MathTraits<_TVarType>::Eps();
	Vector3D<_TVarType> distance(ob_b2.GetPosition() - ob_b1.GetPosition());

	Vector3D<_TVarType> x_axis1(ob_b1.GetRotation().GetAxisX());
	Vector3D<_TVarType> y_axis1(ob_b1.GetRotation().GetAxisY());
	Vector3D<_TVarType> z_axis1(ob_b1.GetRotation().GetAxisZ());

	Vector3D<_TVarType> x_axis2(ob_b2.GetRotation().GetAxisX());
	Vector3D<_TVarType> y_axis2(ob_b2.GetRotation().GetAxisY());
	Vector3D<_TVarType> z_axis2(ob_b2.GetRotation().GetAxisZ());

	_TVarType axis_dot[9];
	_TVarType abs_axis_dot[9];
	_TVarType dist_dot[3];
	_TVarType projected_distance;
	_TVarType projected_size1;
	_TVarType projected_size2;
	_TVarType projected_size_sum;

	// Box1's X-Axis.
	axis_dot[0] = x_axis1.Dot(x_axis2);
	axis_dot[1] = x_axis1.Dot(y_axis2);
	axis_dot[2] = x_axis1.Dot(z_axis2);
	dist_dot[0] = x_axis1.Dot(distance);
	abs_axis_dot[0] = abs(axis_dot[0]);
	abs_axis_dot[1] = abs(axis_dot[1]);
	abs_axis_dot[2] = abs(axis_dot[2]);
	projected_distance = abs(dist_dot[0]);
	projected_size2 = ob_b2.GetSize().x * abs_axis_dot[0] +
	                  ob_b2.GetSize().y * abs_axis_dot[1] +
	                  ob_b2.GetSize().z * abs_axis_dot[2];
	projected_size_sum = projected_size2 + ob_b1.GetSize().x;
	if(projected_distance > projected_size_sum) {
		return false;
	}

	// Box1's Y-Axis.
	axis_dot[3] = y_axis1.Dot(x_axis2);
	axis_dot[4] = y_axis1.Dot(y_axis2);
	axis_dot[5] = y_axis1.Dot(z_axis2);
	dist_dot[1] = y_axis1.Dot(distance);
	abs_axis_dot[3] = abs(axis_dot[3]);
	abs_axis_dot[4] = abs(axis_dot[4]);
	abs_axis_dot[5] = abs(axis_dot[5]);
	projected_distance = abs(dist_dot[1]);
	projected_size2 = ob_b2.GetSize().x * abs_axis_dot[3] +
	                  ob_b2.GetSize().y * abs_axis_dot[4] +
	                  ob_b2.GetSize().z * abs_axis_dot[5];
	projected_size_sum = projected_size2 + ob_b1.GetSize().y;
	if(projected_distance > projected_size_sum) {
		return false;
	}

	// Box1's Z-Axis.
	axis_dot[6] = z_axis1.Dot(x_axis2);
	axis_dot[7] = z_axis1.Dot(y_axis2);
	axis_dot[8] = z_axis1.Dot(z_axis2);
	dist_dot[2] = z_axis1.Dot(distance);
	abs_axis_dot[6] = abs(axis_dot[6]);
	abs_axis_dot[7] = abs(axis_dot[7]);
	abs_axis_dot[8] = abs(axis_dot[8]);
	projected_distance = abs(dist_dot[2]);
	projected_size2 = ob_b2.GetSize().x * abs_axis_dot[6] +
	                  ob_b2.GetSize().y * abs_axis_dot[7] +
	                  ob_b2.GetSize().z * abs_axis_dot[8];
	projected_size_sum = projected_size2 + ob_b1.GetSize().z;
	if(projected_distance > projected_size_sum) {
		return false;
	}

	// Box2's X-Axis
	projected_distance = abs(x_axis2.Dot(distance));
	projected_size1 = ob_b1.GetSize().x * abs_axis_dot[0] +
	                  ob_b1.GetSize().y * abs_axis_dot[3] +
	                  ob_b1.GetSize().z * abs_axis_dot[6];
	projected_size_sum = projected_size1 + ob_b2.GetSize().x;
	if(projected_distance > projected_size_sum) {
		return false;
	}

	// Box2's Y-Axis
	projected_distance = abs(y_axis2.Dot(distance));
	projected_size1 = ob_b1.GetSize().x * abs_axis_dot[1] +
	                  ob_b1.GetSize().y * abs_axis_dot[4] +
	                  ob_b1.GetSize().z * abs_axis_dot[7];
	projected_size_sum = projected_size1 + ob_b2.GetSize().y;
	if(projected_distance > projected_size_sum) {
		return false;
	}

	// Box2's Z-Axis
	projected_distance = abs(z_axis2.Dot(distance));
	projected_size1 = ob_b1.GetSize().x * abs_axis_dot[2] +
						ob_b1.GetSize().y * abs_axis_dot[5] +
						ob_b1.GetSize().z * abs_axis_dot[8];
	projected_size_sum = projected_size1 + ob_b2.GetSize().z;
	if(projected_distance > projected_size_sum) {
		return false;
	}
	// A little check to avoid bugs when the boxes are rotated exactly 90 degrees to each other,
	// making them parallel, and the cross products = (0, 0, 0).
	if(abs_axis_dot[1] > epsilon ||
	   abs_axis_dot[2] > epsilon ||
	   abs_axis_dot[3] > epsilon ||
	   abs_axis_dot[6] > epsilon) {
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
		//		   ob_b1.GetSize().y * AY ** C +
		//		   ob_b1.GetSize().z * AZ ** C;
		//
		// ...which is the "real" way to project OBB1's extents on the axis C.
		//
		// The first product (ob_b1.GetSize().x * AX ** C) is always 0.0 though,
		// because C is perpendicular to AX (and the dot product between two perpendicular vectors is 0.0),
		// which leaves us with:
		//
		// lPS = pOBB1.GetSize().y * AY ** C +              2)
		//		   ob_b1.GetSize().z * AZ ** C;
		//
		// Substitute C with AX x BX in 2):
		//
		// lPS = pOBB1.GetSize().y * AY ** (AX x BX) +      3)
		//		   ob_b1.GetSize().z * AZ ** (AX x BX);
		//
		// Applying the rule:
		// U ** (V x W) = (U x V) ** W
		//
		// on 3) gives
		//
		// lPS = pOBB1.GetSize().y * (AY x AX) ** BX +      4)
		//		   ob_b1.GetSize().z * (AZ x AX) ** BX;
		//
		// We know that:
		//
		// AZ = AY x AX
		// AY = AZ x AX
		//
		// ...and we finally have:

		// lPS = pOBB1.GetSize().y * AZ ** BX +			  5)
		//		   ob_b1.GetSize().z * AY ** BX;
		//

		// XAxis1 cross XAxis2
		projected_distance = abs(dist_dot[2] * axis_dot[3] - dist_dot[1] * axis_dot[6]);
		projected_size1 = ob_b1.GetSize().y * abs_axis_dot[6] + ob_b1.GetSize().z * abs_axis_dot[3];
		projected_size2 = ob_b2.GetSize().y * abs_axis_dot[2] + ob_b2.GetSize().z * abs_axis_dot[1];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
			return false;
		}
	}

	if(abs_axis_dot[0] > epsilon ||
	   abs_axis_dot[2] > epsilon ||
	   abs_axis_dot[4] > epsilon ||
	   abs_axis_dot[7] > epsilon) {
		// XAxis1 cross YAxis2
		projected_distance = abs(dist_dot[2] * axis_dot[4] - dist_dot[1] * axis_dot[7]);
		projected_size1 = ob_b1.GetSize().y * abs_axis_dot[7] + ob_b1.GetSize().z * abs_axis_dot[4];
		projected_size2 = ob_b2.GetSize().x * abs_axis_dot[2] + ob_b2.GetSize().z * abs_axis_dot[0];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
			return false;
		}
	}

	if(abs_axis_dot[0] > epsilon ||
	   abs_axis_dot[1] > epsilon ||
	   abs_axis_dot[5] > epsilon ||
	   abs_axis_dot[8] > epsilon) {
		// XAxis1 cross ZAxis2
		projected_distance = abs(dist_dot[2] * axis_dot[5] - dist_dot[1] * axis_dot[8]);
		projected_size1 = ob_b1.GetSize().y * abs_axis_dot[8] + ob_b1.GetSize().z * abs_axis_dot[5];
		projected_size2 = ob_b2.GetSize().x * abs_axis_dot[1] + ob_b2.GetSize().y * abs_axis_dot[0];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
			return false;
		}
	}

	if(abs_axis_dot[0] > epsilon ||
	   abs_axis_dot[4] > epsilon ||
	   abs_axis_dot[5] > epsilon ||
	   abs_axis_dot[6] > epsilon) {
		// YAxis1 cross XAxis2
		projected_distance = abs(dist_dot[0] * axis_dot[6] - dist_dot[2] * axis_dot[0]);
		projected_size1 = ob_b1.GetSize().x * abs_axis_dot[6] + ob_b1.GetSize().z * abs_axis_dot[0];
		projected_size2 = ob_b2.GetSize().y * abs_axis_dot[5] + ob_b2.GetSize().z * abs_axis_dot[4];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
			return false;
		}
	}

	if(abs_axis_dot[1] > epsilon ||
	   abs_axis_dot[3] > epsilon ||
	   abs_axis_dot[5] > epsilon ||
	   abs_axis_dot[7] > epsilon) {
		// YAxis1 cross YAxis2
		projected_distance = abs(dist_dot[0] * axis_dot[7] - dist_dot[2] * axis_dot[1]);
		projected_size1 = ob_b1.GetSize().x * abs_axis_dot[7] + ob_b1.GetSize().z * abs_axis_dot[1];
		projected_size2 = ob_b2.GetSize().x * abs_axis_dot[5] + ob_b2.GetSize().z * abs_axis_dot[3];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
			return false;
		}
	}

	if(abs_axis_dot[2] > epsilon ||
	   abs_axis_dot[3] > epsilon ||
	   abs_axis_dot[4] > epsilon ||
	   abs_axis_dot[8] > epsilon) {
		// YAxis1 cross ZAxis2
		projected_distance = abs(dist_dot[0] * axis_dot[8] - dist_dot[2] * axis_dot[2]);
		projected_size1 = ob_b1.GetSize().x * abs_axis_dot[8] + ob_b1.GetSize().z * abs_axis_dot[2];
		projected_size2 = ob_b2.GetSize().x * abs_axis_dot[4] + ob_b2.GetSize().y * abs_axis_dot[3];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
			return false;
		}
	}

	if(abs_axis_dot[0] > epsilon ||
	   abs_axis_dot[3] > epsilon ||
	   abs_axis_dot[7] > epsilon ||
	   abs_axis_dot[8] > epsilon) {
		// ZAxis1 cross XAxis2
		projected_distance = abs(dist_dot[1] * axis_dot[0] - dist_dot[0] * axis_dot[3]);
		projected_size1 = ob_b1.GetSize().x * abs_axis_dot[3] + ob_b1.GetSize().y * abs_axis_dot[0];
		projected_size2 = ob_b2.GetSize().y * abs_axis_dot[8] + ob_b2.GetSize().z * abs_axis_dot[7];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
			return false;
		}
	}

	if(abs_axis_dot[1] > epsilon ||
	   abs_axis_dot[4] > epsilon ||
	   abs_axis_dot[6] > epsilon ||
	   abs_axis_dot[8] > epsilon) {
		// ZAxis1 cross YAxis2
		projected_distance = abs(dist_dot[1] * axis_dot[1] - dist_dot[0] * axis_dot[4]);
		projected_size1 = ob_b1.GetSize().x * abs_axis_dot[4] + ob_b1.GetSize().y * abs_axis_dot[1];
		projected_size2 = ob_b2.GetSize().x * abs_axis_dot[8] + ob_b2.GetSize().z * abs_axis_dot[6];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
			return false;
		}
	}

	if(abs_axis_dot[2] > epsilon ||
	   abs_axis_dot[5] > epsilon ||
	   abs_axis_dot[6] > epsilon ||
	   abs_axis_dot[7] > epsilon) {
		// ZAxis1 cross ZAxis2
		projected_distance = abs(dist_dot[1] * axis_dot[2] - dist_dot[0] * axis_dot[5]);
		projected_size1 = ob_b1.GetSize().x * abs_axis_dot[5] + ob_b1.GetSize().y * abs_axis_dot[2];
		projected_size2 = ob_b2.GetSize().x * abs_axis_dot[7] + ob_b2.GetSize().y * abs_axis_dot[6];
		projected_size_sum = projected_size1 + projected_size2;
		if(projected_distance > projected_size_sum) {
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
bool CollisionDetector3D<_TVarType>::IsOBBOverlappingOBB(const OBB<_TVarType>& ob_b1,
							 const OBB<_TVarType>& ob_b2,
							 CollisionInfo& collision_info) {
	// Almost the same as the IsOBBOverlappingOBB() function...
	collision_info.time_to_collision_ = 1;

	_TVarType eps = MathTraits<_TVarType>::Eps();

	Vector3D<_TVarType> distance(ob_b2.GetPosition() - ob_b1.GetPosition());

	Vector3D<_TVarType> axis1[3];
	Vector3D<_TVarType> axis2[3];

	axis1[0] = ob_b1.GetRotation().GetAxisX();
	axis1[1] = ob_b1.GetRotation().GetAxisY();
	axis1[2] = ob_b1.GetRotation().GetAxisZ();
	axis2[0] = ob_b2.GetRotation().GetAxisX();
	axis2[1] = ob_b2.GetRotation().GetAxisY();
	axis2[2] = ob_b2.GetRotation().GetAxisZ();

	_TVarType axis_dot[9];
	_TVarType abs_axis_dot[9];
	_TVarType dist_dot[3];
	_TVarType projected_distance;
	_TVarType projected_size1;
	_TVarType projected_size2;
	_TVarType projected_size_sum;

	AxisID min_overlap_axis = kAxisNone;
	_TVarType min_overlap_dist = 0.0f;
	Vector3D<_TVarType> min_overlap_vector;

	// Box1's X-Axis.
	axis_dot[0] = axis1[0].Dot(axis2[0]);
	axis_dot[1] = axis1[0].Dot(axis2[1]);
	axis_dot[2] = axis1[0].Dot(axis2[2]);
	dist_dot[0] = axis1[0].Dot(distance);
	abs_axis_dot[0] = abs(axis_dot[0]);
	abs_axis_dot[1] = abs(axis_dot[1]);
	abs_axis_dot[2] = abs(axis_dot[2]);
	projected_distance = abs(dist_dot[0]);
	projected_size2 = ob_b2.GetSize().x * abs_axis_dot[0] +
	                  ob_b2.GetSize().y * abs_axis_dot[1] +
	                  ob_b2.GetSize().z * abs_axis_dot[2];
	projected_size_sum = projected_size2 + ob_b1.GetSize().x;
	if(projected_distance > projected_size_sum) {
		return false;
	} else { //if(min_overlap_axis == kAxisNone || min_overlap_dist > (projected_size_sum - projected_distance))
		min_overlap_axis = kAxisX1;
		min_overlap_dist = projected_size_sum - projected_distance;
		min_overlap_vector = axis1[0];
	}

	// Box1's Y-Axis.
	axis_dot[3] = axis1[1].Dot(axis2[0]);
	axis_dot[4] = axis1[1].Dot(axis2[1]);
	axis_dot[5] = axis1[1].Dot(axis2[2]);
	dist_dot[1] = axis1[1].Dot(distance);
	abs_axis_dot[3] = abs(axis_dot[3]);
	abs_axis_dot[4] = abs(axis_dot[4]);
	abs_axis_dot[5] = abs(axis_dot[5]);
	projected_distance = abs(dist_dot[1]);
	projected_size2 = ob_b2.GetSize().x * abs_axis_dot[3] +
	                  ob_b2.GetSize().y * abs_axis_dot[4] +
	                  ob_b2.GetSize().z * abs_axis_dot[5];
	projected_size_sum = projected_size2 + ob_b1.GetSize().y;
	if(projected_distance > projected_size_sum) {
		return false;
	} else if(min_overlap_axis == kAxisNone || min_overlap_dist > (projected_size_sum - projected_distance)) {
		min_overlap_axis = kAxisY1;
		min_overlap_dist = projected_size_sum - projected_distance;
		min_overlap_vector = axis1[1];
	}

	// Box1's Z-Axis.
	axis_dot[6] = axis1[2].Dot(axis2[0]);
	axis_dot[7] = axis1[2].Dot(axis2[1]);
	axis_dot[8] = axis1[2].Dot(axis2[2]);
	dist_dot[2] = axis1[2].Dot(distance);
	abs_axis_dot[6] = abs(axis_dot[6]);
	abs_axis_dot[7] = abs(axis_dot[7]);
	abs_axis_dot[8] = abs(axis_dot[8]);
	projected_distance = abs(dist_dot[2]);
	projected_size2 = ob_b2.GetSize().x * abs_axis_dot[6] +
	                  ob_b2.GetSize().y * abs_axis_dot[7] +
	                  ob_b2.GetSize().z * abs_axis_dot[8];
	projected_size_sum = projected_size2 + ob_b1.GetSize().z;
	if(projected_distance > projected_size_sum) {
		return false;
	} else if(min_overlap_axis == kAxisNone || min_overlap_dist > (projected_size_sum - projected_distance)) {
		min_overlap_axis = kAxisZ1;
		min_overlap_dist = projected_size_sum - projected_distance;
		min_overlap_vector = axis1[2];
	}

	// Box2's X-Axis
	_TVarType dist_dot_temp = axis2[0].Dot(distance);
	projected_distance = abs(dist_dot_temp);
	projected_size1 = ob_b1.GetSize().x * abs_axis_dot[0] +
	                  ob_b1.GetSize().y * abs_axis_dot[3] +
	                  ob_b1.GetSize().z * abs_axis_dot[6];
	projected_size_sum = projected_size1 + ob_b2.GetSize().x;
	if(projected_distance > projected_size_sum) {
		return false;
	} else if(min_overlap_axis == kAxisNone || min_overlap_dist > (projected_size_sum - projected_distance)) {
		min_overlap_axis = kAxisX2;
		min_overlap_dist = projected_size_sum - projected_distance;
		min_overlap_vector = axis2[0];
	}

	// Box2's Y-Axis
	dist_dot_temp = axis2[1].Dot(distance);
	projected_distance = abs(dist_dot_temp);
	projected_size1 = ob_b1.GetSize().x * abs_axis_dot[1] +
	                  ob_b1.GetSize().y * abs_axis_dot[4] +
	                  ob_b1.GetSize().z * abs_axis_dot[7];
	projected_size_sum = projected_size1 + ob_b2.GetSize().y;
	if(projected_distance > projected_size_sum) {
		return false;
	} else if(min_overlap_axis == kAxisNone || min_overlap_dist > (projected_size_sum - projected_distance)) {
		min_overlap_axis = kAxisY2;
		min_overlap_dist = projected_size_sum - projected_distance;
		min_overlap_vector = axis2[1];
	}

	// Box2's Z-Axis
	dist_dot_temp = axis2[2].Dot(distance);
	projected_distance = abs(dist_dot_temp);
	projected_size1 = ob_b1.GetSize().x * abs_axis_dot[2] +
	                  ob_b1.GetSize().y * abs_axis_dot[5] +
	                  ob_b1.GetSize().z * abs_axis_dot[8];
	projected_size_sum = projected_size1 + ob_b2.GetSize().z;
	if(projected_distance > projected_size_sum) {
		return false;
	} else if(min_overlap_axis == kAxisNone || min_overlap_dist > (projected_size_sum - projected_distance)) {
		min_overlap_axis = kAxisZ2;
		min_overlap_dist = projected_size_sum - projected_distance;
		min_overlap_vector = axis2[2];
	}


	//
	// And now all the cross products...
	//

	Vector3D<_TVarType> c;
	_TVarType c_length;

	unsigned int current_axis = (unsigned int)AXIS_X1xX2;
	int i, j;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			c.Cross(axis1[i], axis2[j]);
			c_length = c.GetLength();

			if(c_length > eps) {
				c.Normalize();

				projected_distance = abs(c.Dot(distance));

				projected_size1 = ob_b1.GetSize().x * abs(c.Dot(axis1[0])) +
				                  ob_b1.GetSize().y * abs(c.Dot(axis1[1])) +
				                  ob_b1.GetSize().z * abs(c.Dot(axis1[2]));

				projected_size2 = ob_b2.GetSize().x * abs(c.Dot(axis2[0])) +
				                  ob_b2.GetSize().y * abs(c.Dot(axis2[1])) +
				                  ob_b2.GetSize().z * abs(c.Dot(axis2[2]));

				projected_size_sum = projected_size1 + projected_size2;
				if(projected_distance > projected_size_sum) {
					return false;
				} else if(min_overlap_axis == kAxisNone || min_overlap_dist > (projected_size_sum - projected_distance)) {
					min_overlap_axis = (AxisID)current_axis;
					min_overlap_dist = projected_size_sum - projected_distance;
					min_overlap_vector = c;
				}
			}

			current_axis++;
		}
	}

	// Check if min_overlap_vector has to be flipped.
	if(distance.Dot(min_overlap_vector) > 0.0f) {
		min_overlap_vector *= -1.0f;
	}

	min_overlap_vector.Normalize();
	collision_info.normal_ = min_overlap_vector;
	collision_info.separation_distance_ = min_overlap_dist;
	collision_info.time_to_collision_ = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// Collision tests with velocity.
//
//////////////////////////////////////////////////////////////////////////////

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::AreMovingOBBsColliding(_TVarType time_delta,
							    const OBB<_TVarType>& ob_b1,
							    const Vector3D<_TVarType>& ob_b1_velocity,
							    const OBB<_TVarType>& ob_b2,
							    const Vector3D<_TVarType>& ob_b2_velocity,
							    CollisionInfo& collision_info) {
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
	// TTC = time * (D0 - (A + B)) / (D0 - D1);
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

	const _TVarType epsilon = MathTraits<_TVarType>::Eps();

	obb_collision_data_->Reset();
	//JB: not a clue...	mCollisionInfo.time_to_collision_ = 0.0f;	// Reset to minimum.

	obb_collision_data_->ob_b1_rot_axis_[0] = ob_b1.GetRotation().GetAxisX();
	obb_collision_data_->ob_b1_rot_axis_[1] = ob_b1.GetRotation().GetAxisY();
	obb_collision_data_->ob_b1_rot_axis_[2] = ob_b1.GetRotation().GetAxisZ();
	obb_collision_data_->ob_b2_rot_axis_[0] = ob_b2.GetRotation().GetAxisX();
	obb_collision_data_->ob_b2_rot_axis_[1] = ob_b2.GetRotation().GetAxisY();
	obb_collision_data_->ob_b2_rot_axis_[2] = ob_b2.GetRotation().GetAxisZ();

	Vector3D<_TVarType>* ob_b1_rot_axis[3];
	Vector3D<_TVarType>* ob_b2_rot_axis[3];
	ob_b1_rot_axis[0] = &obb_collision_data_->ob_b1_rot_axis_[0];
	ob_b1_rot_axis[1] = &obb_collision_data_->ob_b1_rot_axis_[1];
	ob_b1_rot_axis[2] = &obb_collision_data_->ob_b1_rot_axis_[2];
	ob_b2_rot_axis[0] = &obb_collision_data_->ob_b2_rot_axis_[0];
	ob_b2_rot_axis[1] = &obb_collision_data_->ob_b2_rot_axis_[1];
	ob_b2_rot_axis[2] = &obb_collision_data_->ob_b2_rot_axis_[2];

	const Vector3D<_TVarType>& ob_b1_size = ob_b1.GetSize();
	const Vector3D<_TVarType>& ob_b2_size = ob_b2.GetSize();

	Vector3D<_TVarType> relative_velocity(ob_b2_velocity - ob_b1_velocity);

	// Compute difference of box centers at time 0 and given time.
	Vector3D<_TVarType> distance0(ob_b2.GetPosition() - ob_b1.GetPosition());
	Vector3D<_TVarType> distance1(distance0 + relative_velocity * time_delta);

	_TVarType ob_b1_d0[3];	// Projected distance0 on OBB1's axes.
	_TVarType ob_b1_d1[3];	// Projected distance1 on OBB1's axes.
	_TVarType ob_b2_d0[3];	// Projected distance0 on OBB2's axes.
	_TVarType ob_b2_d1[3];	// Projected distance1 on OBB2's axes.

	_TVarType* c = obb_collision_data_->c_;
	_TVarType* abs_c = obb_collision_data_->abs_c_;

	_TVarType projected_size1;
	_TVarType projected_size2;
	_TVarType projected_size_sum;
	_TVarType projected_dist;

	// 1. OBB1RotAxisX
	ob_b1_d0[0] = ob_b1_rot_axis[0]->Dot(distance0);
	ob_b1_d1[0] = ob_b1_rot_axis[0]->Dot(distance1);
	c[0] = ob_b1_rot_axis[0]->Dot(*ob_b2_rot_axis[0]);
	c[1] = ob_b1_rot_axis[0]->Dot(*ob_b2_rot_axis[1]);
	c[2] = ob_b1_rot_axis[0]->Dot(*ob_b2_rot_axis[2]);
	abs_c[0] = abs(c[0]);
	abs_c[1] = abs(c[1]);
	abs_c[2] = abs(c[2]);
	projected_size2 = ob_b2_size.x * abs_c[0] +
	                  ob_b2_size.y * abs_c[1] +
	                  ob_b2_size.z * abs_c[2];
	projected_size_sum = projected_size2 + ob_b1.GetSize().x;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(ob_b1_d0[0],
					     ob_b1_d1[0],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     kAxisX1);

	// 2. OBB1RotAxisY
	ob_b1_d0[1] = ob_b1_rot_axis[1]->Dot(distance0);
	ob_b1_d1[1] = ob_b1_rot_axis[1]->Dot(distance1);
	c[3] = ob_b1_rot_axis[1]->Dot(*ob_b2_rot_axis[0]);
	c[4] = ob_b1_rot_axis[1]->Dot(*ob_b2_rot_axis[1]);
	c[5] = ob_b1_rot_axis[1]->Dot(*ob_b2_rot_axis[2]);
	abs_c[3] = abs(c[3]);
	abs_c[4] = abs(c[4]);
	abs_c[5] = abs(c[5]);
	projected_size2 = ob_b2_size.x * abs_c[3] +
	                  ob_b2_size.y * abs_c[4] +
	                  ob_b2_size.z * abs_c[5];
	projected_size_sum = projected_size2 + ob_b1.GetSize().y;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(ob_b1_d0[1],
					     ob_b1_d1[1],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     kAxisY1);

	// 3. OBB1RotAxisZ
	ob_b1_d0[2] = ob_b1_rot_axis[2]->Dot(distance0);
	ob_b1_d1[2] = ob_b1_rot_axis[2]->Dot(distance1);
	c[6] = ob_b1_rot_axis[2]->Dot(*ob_b2_rot_axis[0]);
	c[7] = ob_b1_rot_axis[2]->Dot(*ob_b2_rot_axis[1]);
	c[8] = ob_b1_rot_axis[2]->Dot(*ob_b2_rot_axis[2]);
	abs_c[6] = abs(c[6]);
	abs_c[7] = abs(c[7]);
	abs_c[8] = abs(c[8]);
	projected_size2 = ob_b2_size.x * abs_c[6] +
	                  ob_b2_size.y * abs_c[7] +
	                  ob_b2_size.z * abs_c[8];
	projected_size_sum = projected_size2 + ob_b1.GetSize().z;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(ob_b1_d0[2],
					     ob_b1_d1[2],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     kAxisZ1);
	// 4. OBB2RotAxisX
	ob_b2_d0[0] = ob_b2_rot_axis[0]->Dot(distance0);
	ob_b2_d1[0] = ob_b2_rot_axis[0]->Dot(distance1);
	projected_size1 = ob_b1_size.x * abs_c[0] +
	                  ob_b1_size.y * abs_c[3] +
	                  ob_b1_size.z * abs_c[6];
	projected_size_sum = projected_size1 + ob_b2_size.x;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(ob_b2_d0[0],
					     ob_b2_d1[0],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     kAxisX2);

	// 5. OBB2RotAxisY
	ob_b2_d0[1] = ob_b2_rot_axis[1]->Dot(distance0);
	ob_b2_d1[1] = ob_b2_rot_axis[1]->Dot(distance1);
	projected_size1 = ob_b1_size.x * abs_c[1] +
	                  ob_b1_size.y * abs_c[4] +
	                  ob_b1_size.z * abs_c[7];
	projected_size_sum = projected_size1 + ob_b2_size.y;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(ob_b2_d0[1],
					     ob_b2_d1[1],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     kAxisY2);

	// 6. OBB2RotAxisZ
	ob_b2_d0[2] = ob_b2_rot_axis[2]->Dot(distance0);
	ob_b2_d1[2] = ob_b2_rot_axis[2]->Dot(distance1);
	projected_size1 = ob_b1_size.x * abs_c[2] +
	                  ob_b1_size.y * abs_c[5] +
	                  ob_b1_size.z * abs_c[8];
	projected_size_sum = projected_size1 + ob_b2_size.z;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_0(ob_b2_d0[2],
					     ob_b2_d1[2],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     kAxisZ2);

	// 7. OBB1RotAxisX x OBB2RotAxisX
	projected_size1 = ob_b1_size.y * abs_c[6] + ob_b1_size.z * abs_c[3];
	projected_size2 = ob_b2_size.y * abs_c[2] + ob_b2_size.z * abs_c[1];
	projected_dist = ob_b1_d0[2] * c[3] - ob_b1_d0[1] * c[6];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[2],
					     c[3],
					     ob_b1_d1[1],
					     c[6],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_X1xX2);

	// 8. OBB1RotAxisX x OBB2RotAxisY
	projected_size1 = ob_b1_size.y * abs_c[7] + ob_b1_size.z * abs_c[4];
	projected_size2 = ob_b2_size.x * abs_c[2] + ob_b2_size.z * abs_c[0];
	projected_dist = ob_b1_d0[2] * c[4] - ob_b1_d0[1] * c[7];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[2],
					     c[4],
					     ob_b1_d1[1],
					     c[7],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_X1xY2);

	// 9. OBB1RotAxisX x OBB2RotAxisZ
	projected_size1 = ob_b1_size.y * abs_c[8] + ob_b1_size.z * abs_c[5];
	projected_size2 = ob_b2_size.x * abs_c[1] + ob_b2_size.y * abs_c[0];
	projected_dist = ob_b1_d0[2] * c[5] - ob_b1_d0[1] * c[8];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[2],
					     c[5],
					     ob_b1_d1[1],
					     c[8],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_X1xZ2);

	// 10. OBB1RotAxisY x OBB2RotAxisX
	projected_size1 = ob_b1_size.x * abs_c[6] + ob_b1_size.z * abs_c[0];
	projected_size2 = ob_b2_size.y * abs_c[5] + ob_b2_size.z * abs_c[4];
	projected_dist = ob_b1_d0[0] * c[6] - ob_b1_d0[2] * c[0];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[0],
					     c[6],
					     ob_b1_d1[2],
					     c[0],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_Y1xX2);

	// 11. OBB1RotAxisY x OBB2RotAxisY
	projected_size1 = ob_b1_size.x * abs_c[7] + ob_b1_size.z * abs_c[1];
	projected_size2 = ob_b2_size.x * abs_c[5] + ob_b2_size.z * abs_c[3];
	projected_dist = ob_b1_d0[0] * c[7] - ob_b1_d0[2] * c[1];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[0],
					     c[7],
					     ob_b1_d1[2],
					     c[1],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_Y1xY2);

	// 12. OBB1RotAxisY x OBB2RotAxisZ
	projected_size1 = ob_b1_size.x * abs_c[8] + ob_b1_size.z * abs_c[2];
	projected_size2 = ob_b2_size.x * abs_c[4] + ob_b2_size.y * abs_c[3];
	projected_dist = ob_b1_d0[0] * c[8] - ob_b1_d0[2] * c[2];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[0],
					     c[8],
					     ob_b1_d1[2],
					     c[2],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_Y1xZ2);

	// 13. OBB1RotAxisZ x OBB2RotAxisX
	projected_size1 = ob_b1_size.x * abs_c[3] + ob_b1_size.y * abs_c[0];
	projected_size2 = ob_b2_size.y * abs_c[8] + ob_b2_size.z * abs_c[7];
	projected_dist = ob_b1_d0[1] * c[0] - ob_b1_d0[0] * c[3];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[1],
					     c[0],
					     ob_b1_d1[0],
					     c[3],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_Z1xX2);

	// 14. OBB1RotAxisZ x OBB2RotAxisY
	projected_size1 = ob_b1_size.x * abs_c[4] + ob_b1_size.y * abs_c[1];
	projected_size2 = ob_b2_size.x * abs_c[8] + ob_b2_size.z * abs_c[6];
	projected_dist = ob_b1_d0[1] * c[1] - ob_b1_d0[0] * c[4];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[1],
					     c[1],
					     ob_b1_d1[0],
					     c[4],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_Z1xY2);

	// 15. OBB1RotAxisZ x OBB2RotAxisZ
	projected_size1 = ob_b1_size.x * abs_c[5] + ob_b1_size.y * abs_c[2];
	projected_size2 = ob_b2_size.x * abs_c[7] + ob_b2_size.y * abs_c[6];
	projected_dist = ob_b1_d0[1] * c[2] - ob_b1_d0[0] * c[5];
	projected_size_sum = projected_size1 + projected_size2;
	TSPMACRO_TEST_MOVING_OBB_COLLISION_1(projected_dist,
					     ob_b1_d1[1],
					     c[2],
					     ob_b1_d1[0],
					     c[5],
					     projected_size_sum,
					     time_delta,
					     collision_info.time_to_collision_,
					     obb_collision_data_->axis_id_,
					     obb_collision_data_->side_,
					     AXIS_Z1xZ2);

	// At this point none of the 15 axes separate the boxes. It is still
	// possible that they are separated as viewed in any plane orthogonal
	// to the relative direction of motion W. In the worst case, the two
	// projected boxes are hexagons. This requires three separating axis
	// tests per box.

	Vector3D<_TVarType> velx_dist(relative_velocity, distance0);
	_TVarType wob_b1[3];
	_TVarType wob_b2[3];

	// 16. W x OBB1AxisX
	wob_b1[1] = relative_velocity.Dot(*ob_b1_rot_axis[1]);
	TSPMACRO_SNAP_TO_ZERO(wob_b1[1], epsilon);
	wob_b1[2] = relative_velocity.Dot(*ob_b1_rot_axis[2]);
	TSPMACRO_SNAP_TO_ZERO(wob_b1[2], epsilon);

	projected_dist = abs(ob_b1_rot_axis[0]->Dot(velx_dist));
	TSPMACRO_SNAP_TO_ZERO(projected_dist, epsilon);

	projected_size1 = ob_b1_size.y * abs(wob_b1[2]) + ob_b1_size.z * abs(wob_b1[1]);
	projected_size2 = ob_b2_size.x * abs(c[3] * wob_b1[2] - c[6] * wob_b1[1]) +
	                  ob_b2_size.y * abs(c[4] * wob_b1[2] - c[7] * wob_b1[1]) +
	                  ob_b2_size.z * abs(c[5] * wob_b1[2] - c[8] * wob_b1[1]);
	projected_size_sum = projected_size1 + projected_size2;
	if(projected_dist > projected_size_sum) {
		collision_info.time_to_collision_ = time_delta;
		return false;	// No collision.
	}

	// 17. W x OBB1AxisY
	wob_b1[0] = relative_velocity.Dot(*ob_b1_rot_axis[0]);
	TSPMACRO_SNAP_TO_ZERO(wob_b1[0], epsilon);

	projected_dist = abs(ob_b1_rot_axis[1]->Dot(velx_dist));
	TSPMACRO_SNAP_TO_ZERO(projected_dist, epsilon);

	projected_size1 = ob_b1_size.z * abs(wob_b1[0]) + ob_b1_size.x * abs(wob_b1[2]);
	projected_size2 = ob_b2_size.x * abs(c[6] * wob_b1[0] - c[0] * wob_b1[2]) +
	                  ob_b2_size.y * abs(c[7] * wob_b1[0] - c[1] * wob_b1[2]) +
	                  ob_b2_size.z * abs(c[8] * wob_b1[0] - c[2] * wob_b1[2]);
	projected_size_sum = projected_size1 + projected_size2;
	if(projected_dist > projected_size_sum) {
		collision_info.time_to_collision_ = time_delta;
		return false; // No collision.
	}

	// 18. W x OBB1AxisZ
	projected_dist = abs(ob_b1_rot_axis[2]->Dot(velx_dist));
	TSPMACRO_SNAP_TO_ZERO(projected_dist, epsilon);

	projected_size1 = ob_b1_size.x * abs(wob_b1[1]) + ob_b1_size.y * abs(wob_b1[0]);
	projected_size2 = ob_b2_size.x * abs(c[0] * wob_b1[1] - c[3] * wob_b1[0]) +
	                  ob_b2_size.y * abs(c[1] * wob_b1[1] - c[4] * wob_b1[0]) +
	                  ob_b2_size.z * abs(c[2] * wob_b1[1] - c[5] * wob_b1[0]);
	projected_size_sum = projected_size1 + projected_size2;
	if(projected_dist > projected_size_sum) {
		collision_info.time_to_collision_ = time_delta;
		return false; // No collision.
	}

	// 19. W x OBB2AxisX
	wob_b2[1] = relative_velocity.Dot(*ob_b2_rot_axis[1]);
	TSPMACRO_SNAP_TO_ZERO(wob_b2[1], epsilon);
	wob_b2[2] = relative_velocity.Dot(*ob_b2_rot_axis[2]);
	TSPMACRO_SNAP_TO_ZERO(wob_b2[2], epsilon);

	projected_dist = abs(ob_b2_rot_axis[0]->Dot(velx_dist));
	TSPMACRO_SNAP_TO_ZERO(projected_dist, epsilon);

	projected_size1 = ob_b1_size.x * abs(c[1] * wob_b2[2] - c[2] * wob_b2[1]) +
	                  ob_b1_size.y * abs(c[4] * wob_b2[2] - c[5] * wob_b2[1]) +
	                  ob_b1_size.z * abs(c[7] * wob_b2[2] - c[8] * wob_b2[1]);
	projected_size2 = ob_b2_size.y * abs(wob_b2[2]) + ob_b2_size.z * abs(wob_b2[1]);
	projected_size_sum = projected_size1 + projected_size2;
	if(projected_dist > projected_size_sum) {
		collision_info.time_to_collision_ = time_delta;
		return false; // No collision.
	}

	// 20. W x OBB2AxisY
	wob_b2[0] = relative_velocity.Dot(*ob_b2_rot_axis[0]);
	TSPMACRO_SNAP_TO_ZERO(wob_b2[0], epsilon);

	projected_dist = abs(ob_b2_rot_axis[1]->Dot(velx_dist));
	TSPMACRO_SNAP_TO_ZERO(projected_dist, epsilon);

	projected_size1 = ob_b1_size.x * abs(c[2] * wob_b2[0] - c[0] * wob_b2[2]) +
	                  ob_b1_size.y * abs(c[5] * wob_b2[0] - c[3] * wob_b2[2]) +
	                  ob_b1_size.z * abs(c[8] * wob_b2[0] - c[6] * wob_b2[2]);
	projected_size2 = ob_b2_size.z * abs(wob_b2[0]) + ob_b2_size.x * abs(wob_b2[2]);
	projected_size_sum = projected_size1 + projected_size2;
	if(projected_dist > projected_size_sum) {
		collision_info.time_to_collision_ = time_delta;
		return false; // No collision.
	}

	// 21. W x OBB2AxisZ
	projected_dist = abs(ob_b2_rot_axis[2]->Dot(velx_dist));
	TSPMACRO_SNAP_TO_ZERO(projected_dist, epsilon);

	projected_size1 = ob_b1_size.x * abs(c[0] * wob_b2[1] - c[1] * wob_b2[0]) +
	                  ob_b1_size.y * abs(c[3] * wob_b2[1] - c[4] * wob_b2[0]) +
	                  ob_b1_size.z * abs(c[6] * wob_b2[1] - c[7] * wob_b2[0]);
	projected_size2 = ob_b2_size.x * abs(wob_b2[1]) + ob_b2_size.y * abs(wob_b2[0]);
	projected_size_sum = projected_size1 + projected_size2;
	if(projected_dist > projected_size_sum) {
		collision_info.time_to_collision_ = time_delta;
		return false; // No collision.
	}

	// If we get here, there is a collision. Now we have to find the appropriate collision
	// normal.
	obb_collision_data_->valid_collision_ = true;

	switch(obb_collision_data_->axis_id_) {
		case kAxisX1:
		case kAxisY1:
		case kAxisZ1: {
			collision_info.normal_ = *ob_b1_rot_axis[(int)obb_collision_data_->axis_id_ - 1];
		} break;
		case kAxisX2:
		case kAxisY2:
		case kAxisZ2: {
			collision_info.normal_ = *ob_b2_rot_axis[(int)obb_collision_data_->axis_id_ - 4];
		} break;
		case AXIS_X1xX2:
		case AXIS_X1xY2:
		case AXIS_X1xZ2: {
			collision_info.normal_.Cross(*ob_b1_rot_axis[0], *ob_b2_rot_axis[(int)obb_collision_data_->axis_id_ - 7]);
			// The Normalize() function will check if the vector magnitude is >0.
			collision_info.normal_.Normalize();
		} break;
		case AXIS_Y1xX2:
		case AXIS_Y1xY2:
		case AXIS_Y1xZ2: {
			collision_info.normal_.Cross(*ob_b1_rot_axis[1], *ob_b2_rot_axis[(int)obb_collision_data_->axis_id_ - 10]);
			// The Normalize() function will check if the vector magnitude is >0.
			collision_info.normal_.Normalize();
		} break;
		case AXIS_Z1xX2:
		case AXIS_Z1xY2:
		case AXIS_Z1xZ2: {
			collision_info.normal_.Cross(*ob_b1_rot_axis[2], *ob_b2_rot_axis[(int)obb_collision_data_->axis_id_ - 13]);
			// The Normalize() function will check if the vector magnitude is >0.
			collision_info.normal_.Normalize();
		} break;
	};

	obb_collision_data_->axis_ = collision_info.normal_;

	// Check if the CollisionNormal has to be flipped.
	obb_collision_data_->ob_b1_collision_pos_ = ob_b1.GetPosition() + ob_b1_velocity * collision_info.time_to_collision_;
	obb_collision_data_->ob_b2_collision_pos_ = ob_b2.GetPosition() + ob_b2_velocity * collision_info.time_to_collision_;
	Vector3D<_TVarType> collision_pos_diff(obb_collision_data_->ob_b2_collision_pos_ - obb_collision_data_->ob_b1_collision_pos_);

	if(collision_info.normal_.Dot(collision_pos_diff) > 0.0f) {
		collision_info.normal_ *= -1.0f;
	}

	obb_collision_data_->ob_b1_size_[0] = ob_b1_size.x;
	obb_collision_data_->ob_b1_size_[1] = ob_b1_size.y;
	obb_collision_data_->ob_b1_size_[2] = ob_b1_size.z;
	obb_collision_data_->ob_b2_size_[0] = ob_b2_size.x;
	obb_collision_data_->ob_b2_size_[1] = ob_b2_size.y;
	obb_collision_data_->ob_b2_size_[2] = ob_b2_size.z;

	// There is no separation distance.
	collision_info.separation_distance_ = 0;
	return true;
}

template<class _TVarType>
typename CollisionDetector3D<_TVarType>::CollisionType CollisionDetector3D<_TVarType>::GetOBBCollisionType() {
	const _TVarType epsilon = MathTraits<_TVarType>::Eps();
	const _TVarType one_cmp = 1.0f - epsilon;

	// We have to figure out what kind of collision we have here...
	// Point, edge or surface.

	// To accomplish this, we first have to check if any of the boxes have an edge
	// aligned with the collision normal (the axis stored in obb_collision_data_).
	// While doing that, save all relevant values in obb_collision_data_
	// used to get the intersection later on.
	bool test_ob_b1 = true;
	bool test_ob_b2 = true;
	bool quick_test = false;
	int aligned_box = 0;

	int c_index_start = 0;
	int c_index_step = 1;
	switch(obb_collision_data_->axis_id_) {
	        case kAxisX1:
		case kAxisX2: {
			c_index_start = 0;
		} break;
		case kAxisY1:
		case kAxisY2: {
			c_index_start = 1;
		} break;
		case kAxisZ1:
		case kAxisZ2: {
			c_index_start = 2;
		} break;
	};

	// First a quick check.
	switch(obb_collision_data_->axis_id_) {
		case kAxisX1:
		case kAxisY1:
		case kAxisZ1: {
			// This is all that is needed to get the collision type...
			aligned_box = 1;
			test_ob_b1 = false;

			obb_collision_data_->box_with_separating_plane_ = 0;
			obb_collision_data_->box1_normal_index_ = (int)obb_collision_data_->axis_id_ - (int)kAxisX1;
			obb_collision_data_->box1_side_ = obb_collision_data_->side_;

			quick_test = true;
		} break;
		case kAxisX2:
		case kAxisY2:
		case kAxisZ2: {
			// This is all that is needed to get the collision type...
			aligned_box = 2;
			test_ob_b2 = false;

			obb_collision_data_->box_with_separating_plane_ = 1;
			obb_collision_data_->box2_normal_index_ = (int)obb_collision_data_->axis_id_ - (int)kAxisX2;
			obb_collision_data_->box2_side_ = -obb_collision_data_->side_;

			c_index_step = 3;
			quick_test = true;
		} break;
	};

	// If needed, make a little deeper check...

	//
	// Check OBB1.
	//

	if(test_ob_b1 == true) {
		if(quick_test == true) {
			// Quicktest means that we don't need to calculate the dot product,
			// we can just get it from the obb_collision_data_->c_ - array.

			int c_index = c_index_start;
			for(int i = 0; i < 3; i++) {
				_TVarType abs_dot = obb_collision_data_->abs_c_[c_index];
				if(abs_dot >= one_cmp) {
					aligned_box += 1;
					obb_collision_data_->box1_normal_index_ = i;

					_TVarType dot = obb_collision_data_->c_[c_index];
					obb_collision_data_->box1_side_ = dot > 0.0f ?
						obb_collision_data_->side_ :
						-obb_collision_data_->side_;
				} else if(abs_dot < epsilon) {
					obb_collision_data_->box1_edge_index_ = i;
				}

				c_index += c_index_step;
			}
		} else {
			// Normal check. We have to calculate the dot product.
			for(int i = 0; i < 3; i++) {
				_TVarType dot = obb_collision_data_->axis_.Dot(obb_collision_data_->ob_b1_rot_axis_[i]);
				_TVarType abs_dot = (_TVarType)fabs(dot);

				// If the dot product if very close to 1.0.
				if(abs_dot >= one_cmp) {
					aligned_box += 1;

					obb_collision_data_->box_with_separating_plane_ = 0;
					obb_collision_data_->box1_normal_index_ = i;

					obb_collision_data_->box1_side_ = dot > 0.0f ?
						obb_collision_data_->side_ :
						-obb_collision_data_->side_;
					break;
				} else if(abs_dot < epsilon) {
					obb_collision_data_->box1_edge_index_ = i;
				}
			}
		}
	}



	//
	// Check OBB2.
	//

	if(test_ob_b2 == true) {
		if(quick_test == true) {
			// Quicktest means that we don't need to calculate the dot product,
			// we can just get it from the obb_collision_data_->c_ - array.

			int c_index = c_index_start;
			for(int i = 0; i < 3; i++) {
				_TVarType abs_dot = obb_collision_data_->abs_c_[c_index];
				if(abs_dot >= one_cmp) {
					aligned_box += 2;
					obb_collision_data_->box2_normal_index_ = i;

					_TVarType dot = obb_collision_data_->c_[c_index];
					obb_collision_data_->box2_side_ = dot > 0.0f ?
						-obb_collision_data_->side_ :
						obb_collision_data_->side_;
				} else if(abs_dot < epsilon) {
					obb_collision_data_->box2_edge_index_ = i;
				}

				c_index += 3;
			}
		} else {
			// Normal check. We have to calculate the dot product.
			for(int i = 0; i < 3; i++) {
				_TVarType dot = obb_collision_data_->axis_.Dot(obb_collision_data_->ob_b2_rot_axis_[i]);
				_TVarType abs_dot = (_TVarType)fabs(dot);

				// If the dot product if very close to 1.0.
				if(abs_dot >= one_cmp) {
					aligned_box += 2;

					obb_collision_data_->box_with_separating_plane_ = 1;
					obb_collision_data_->box2_normal_index_ = i;

					obb_collision_data_->box2_side_ = dot > 0.0f ?
						-obb_collision_data_->side_ :
						obb_collision_data_->side_;
					break;
				} else if(abs_dot < epsilon) {
					obb_collision_data_->box2_edge_index_ = i;
				}
			}
		}
	}

	if(aligned_box == 3) {
		// Both boxes are aligned... This makes it a surface to surface collision.
		return kSurfaceCollision;
	} else if(aligned_box == 0) {
		// None of the boxes are aligned. This makes it a point collision.
		return kPointCollision;
	}

	// One box axis is aligned... Check if the other box has an edge perpendicualar to the
	// collision normal. In that case it is an edge collision, otherwise it is a point collision.
	if((aligned_box == 1 && obb_collision_data_->box2_edge_index_ != -1) ||
	   (aligned_box == 2 && obb_collision_data_->box1_edge_index_ != -1)) {
		return kEdgeCollision;
	}

	return kPointCollision;
}

template<class _TVarType>
void CollisionDetector3D<_TVarType>::GetOBBToOBBIntersection(Vector3D<_TVarType> point[8], int &num_points) {
	if(obb_collision_data_->valid_collision_ == false) {
		num_points = 0;
		return;
	}

	const _TVarType epsilon = MathTraits<_TVarType>::Eps();
	CollisionType _collision_type = GetOBBCollisionType();

	switch(_collision_type) {
	case kPointCollision:
		GetOBBPointIntersection(point, num_points);
		break;
	case kEdgeCollision:
	case kSurfaceCollision:
		GetOBBEdgeSurfaceIntersection(point, num_points, _collision_type);
		break;
	};
}

template<class _TVarType>
void CollisionDetector3D<_TVarType>::GetOBBPointIntersection(Vector3D<_TVarType> point[8], int &num_points) {
    // Determine the point of intersection
	const _TVarType epsilon = MathTraits<_TVarType>::Eps();

	num_points = 1;

	_TVarType ob_b1_signed_extents[3];
	_TVarType ob_b2_signed_extents[3];
	Vector3D<_TVarType> P;

	// The values in obb_collision_data_->c_ will be used to check
	// the alignment of the boxes, and chooses the first sign if c_[x] > 0
	// and the second sign if c_[x] is < 0.

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
	// we need are actually already calculated and stored in obb_collision_data_->c_.
	// The result so far will look like this:
	//
	// _TVarType lOBB1XExtent = obb_collision_data_->c_[1] * ob_b2_signed_extents[1] +
	//                        obb_collision_data_->c_[2] * ob_b2_signed_extents[2];
	//
	// But what happens if the boxes centers aren't ligned up as nice as in the picture above?
	// What shall we do when the boxes centers are offset in OBB1's x-axis direction? Well, that's
	// an easy one. Just project the vector between the centers of the boxes and add this one too:
	//
	// Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos -
	//                                    mOBBCollisionData->mOBB1CollisionPos);
	//
	// _TVarType x1_cd = obb_collision_data_->ob_b1_rot_axis_[0].Dot(collision_dist);
	//
	// _TVarType lOBB1XExtent = x1_cd +
	//                        obb_collision_data_->c_[1] * ob_b2_signed_extents[1] +
	//                        obb_collision_data_->c_[2] * ob_b2_signed_extents[2];
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
	// but the other way around, for OBB2 instead. Since the collision_dist vector is
	// pointing in the wrong direction, we have to subtract its projection instead of adding it:
	//
	// _TVarType x2_cd = obb_collision_data_->ob_b2_rot_axis_[0].Dot(collision_dist);
	//
	// _TVarType lOBB2XExtent = obb_collision_data_->c_[3] * ob_b1_signed_extents[1] +
	//                        obb_collision_data_->c_[6] * ob_b1_signed_extents[2] -
	//                        x2_cd;
	//
	// Now all we have to do is to project lOBB2XExtent onto OBB1's x-axis, and put it into
	// the formula. The whole thing now looks like this:
	//
	// Vector3D<_TVarType> lCollisionDist(mOBBCollisionData->mOBB2CollisionPos -
	//                                    mOBBCollisionData->mOBB1CollisionPos);
	//
	// _TVarType x1_cd = obb_collision_data_->ob_b1_rot_axis_[0].Dot(collision_dist);
	// _TVarType x2_cd = obb_collision_data_->ob_b2_rot_axis_[0].Dot(collision_dist);
	//
	// _TVarType lOBB2XExtent = obb_collision_data_->c_[3] * ob_b1_signed_extents[1] +
	//                        obb_collision_data_->c_[6] * ob_b1_signed_extents[2] -
	//                        x2_cd;
	//
	// _TVarType lOBB1XExtent = x1_cd +
	//                        obb_collision_data_->c_[0] * lOBB2XExtent +
	//                        obb_collision_data_->c_[1] * ob_b2_signed_extents[1] +
	//                        obb_collision_data_->c_[2] * ob_b2_signed_extents[2];


	switch(obb_collision_data_->axis_id_) {
		case kAxisX1:
		case kAxisY1:
		case kAxisZ1: {
			int i = ((int)obb_collision_data_->axis_id_ - 1) * 3;
			for(int j = 0; j < 3; j++, i++) {
				TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[j], -, +,
							   obb_collision_data_->side_,
							   obb_collision_data_->c_[i],
							   obb_collision_data_->ob_b2_size_[j]);
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
						  obb_collision_data_->ob_b2_rot_axis_,
						  obb_collision_data_->ob_b2_collision_pos_,
						  ob_b2_signed_extents);
			return;
		}
		case kAxisX2:
		case kAxisY2:
		case kAxisZ2: {
			int i = (int)obb_collision_data_->axis_id_ - 4;
			for(int j = 0; j < 3; j++, i += 3) {
				TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[j], +, -,
				                           obb_collision_data_->side_,
				                           obb_collision_data_->c_[i],
				                           obb_collision_data_->ob_b1_size_[j]);
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
		case AXIS_X1xX2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[1], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[6], // OBB1AxisZ * OBB2AxisX
			                           obb_collision_data_->ob_b1_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[2], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[3], // OBB1AxisY * OBB2AxisX
			                           obb_collision_data_->ob_b1_size_[2]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[1], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[2], // OBB1AxisX * OBB2AxisZ
			                           obb_collision_data_->ob_b2_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[2], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[1], // OBB1AxisX * OBB2AxisY
			                           obb_collision_data_->ob_b2_size_[2]);

			_TVarType div = 1.0f - obb_collision_data_->c_[0] * obb_collision_data_->c_[0];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType x1_cd = obb_collision_data_->ob_b1_rot_axis_[0].Dot(collision_dist);
				_TVarType x2_cd = obb_collision_data_->ob_b2_rot_axis_[0].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[3] * ob_b1_signed_extents[1] +
				                  c[6] * ob_b1_signed_extents[2] - x2_cd;
				ob_b1_signed_extents[0] = (x1_cd +
					c[0] * temp +
					c[1] * ob_b2_signed_extents[1] +
					c[2] * ob_b2_signed_extents[2]) / div;
			} else {
				ob_b1_signed_extents[0] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
		case AXIS_X1xY2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[1], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[7], // OBB1AxisZ * OBB2AxisY
			                           obb_collision_data_->ob_b1_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[2], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[4], // OBB1AxisY * OBB2AxisY
			                           obb_collision_data_->ob_b1_size_[2]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[0], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[2], // OBB1AxisX * OBB2AxisZ
			                           obb_collision_data_->ob_b2_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[2], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[0], // OBB1AxisX * OBB2AxisX
			                           obb_collision_data_->ob_b2_size_[2]);

			_TVarType div = 1.0f - obb_collision_data_->c_[1] * obb_collision_data_->c_[1];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType x1_cd = obb_collision_data_->ob_b1_rot_axis_[0].Dot(collision_dist);
				_TVarType y2_cd = obb_collision_data_->ob_b2_rot_axis_[1].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[4] * ob_b1_signed_extents[1] +
				                  c[7] * ob_b1_signed_extents[2] - y2_cd;
				ob_b1_signed_extents[0] = (x1_cd +
					c[1] * temp +
					c[0] * ob_b2_signed_extents[0] +
					c[2] * ob_b2_signed_extents[2]) / div;
			} else {
				ob_b1_signed_extents[0] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
		case AXIS_X1xZ2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[1], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[8], // OBB1AxisZ * OBB2AxisZ
			                           obb_collision_data_->ob_b1_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[2], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[5], // OBB1AxisY * OBB2AxisZ
			                           obb_collision_data_->ob_b1_size_[2]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[0], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[1], // OBB1AxisX * OBB2AxisY
			                           obb_collision_data_->ob_b2_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[1], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[0], // OBB1AxisX * OBB2AxisX
			                           obb_collision_data_->ob_b2_size_[1]);

			_TVarType div = 1.0f - obb_collision_data_->c_[2] * obb_collision_data_->c_[2];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType x1_cd = obb_collision_data_->ob_b1_rot_axis_[0].Dot(collision_dist);
				_TVarType z2_cd = obb_collision_data_->ob_b2_rot_axis_[2].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[5] * ob_b1_signed_extents[1] +
				                  c[8] * ob_b1_signed_extents[2] - z2_cd;
				ob_b1_signed_extents[0] = (x1_cd +
					c[2] * temp +
					c[0] * ob_b2_signed_extents[0] +
					c[1] * ob_b2_signed_extents[1]) / div;
			} else {
				ob_b1_signed_extents[0] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
		case AXIS_Y1xX2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[0], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[6], // OBB1AxisZ * OBB2AxisX
			                           obb_collision_data_->ob_b1_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[2], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[0], // OBB1AxisX * OBB2AxisX
			                           obb_collision_data_->ob_b1_size_[2]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[1], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[5], // OBB1AxisY * OBB2AxisZ
			                           obb_collision_data_->ob_b2_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[2], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[4], // OBB1AxisY * OBB2AxisY
			                           obb_collision_data_->ob_b2_size_[2]);

			_TVarType div = 1.0f - obb_collision_data_->c_[3] * obb_collision_data_->c_[3];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType y1_cd = obb_collision_data_->ob_b1_rot_axis_[1].Dot(collision_dist);
				_TVarType x2_cd = obb_collision_data_->ob_b2_rot_axis_[0].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[0] * ob_b1_signed_extents[0] +
				                  c[6] * ob_b1_signed_extents[2] - x2_cd;
				ob_b1_signed_extents[1] = (y1_cd +
					c[3] * temp +
					c[4] * ob_b2_signed_extents[1] +
					c[5] * ob_b2_signed_extents[2]) / div;
			} else {
				ob_b1_signed_extents[1] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
		case AXIS_Y1xY2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[0], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[7], // OBB1AxisZ * OBB2AxisY
			                           obb_collision_data_->ob_b1_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[2], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[1], // OBB1AxisX * OBB2AxisY
			                           obb_collision_data_->ob_b1_size_[2]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[0], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[5], // OBB1AxisY * OBB2AxisZ
			                           obb_collision_data_->ob_b2_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[2], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[3], // OBB1AxisY * OBB2AxisX
			                           obb_collision_data_->ob_b2_size_[2]);

			_TVarType div = 1.0f - obb_collision_data_->c_[4] * obb_collision_data_->c_[4];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType y1_cd = obb_collision_data_->ob_b1_rot_axis_[1].Dot(collision_dist);
				_TVarType y2_cd = obb_collision_data_->ob_b2_rot_axis_[1].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[1] * ob_b1_signed_extents[0] +
				                  c[7] * ob_b1_signed_extents[2] - y2_cd;
				ob_b1_signed_extents[1] = (y1_cd +
					c[4] * temp +
					c[3] * ob_b2_signed_extents[0] +
					c[5] * ob_b2_signed_extents[2]) / div;
			} else {
				ob_b1_signed_extents[1] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);

			return;
		}
		case AXIS_Y1xZ2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[0], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[8], // OBB1AxisZ * OBB2AxisZ
			                           obb_collision_data_->ob_b1_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[2], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[2], // OBB1AxisX * OBB2AxisZ
			                           obb_collision_data_->ob_b1_size_[2]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[0], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[4], // OBB1AxisY * OBB2AxisY
			                           obb_collision_data_->ob_b2_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[1], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[3], // OBB1AxisY * OBB2AxisX
			                           obb_collision_data_->ob_b2_size_[1]);

			_TVarType div = 1.0f - obb_collision_data_->c_[5] * obb_collision_data_->c_[5];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType y1_cd = obb_collision_data_->ob_b1_rot_axis_[1].Dot(collision_dist);
				_TVarType z2_cd = obb_collision_data_->ob_b2_rot_axis_[2].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[2] * ob_b1_signed_extents[0] +
				                  c[8] * ob_b1_signed_extents[2] - z2_cd;
				ob_b1_signed_extents[1] = (y1_cd +
					c[5] * temp +
					c[3] * ob_b2_signed_extents[0] +
					c[4] * ob_b2_signed_extents[1]) / div;
			} else {
				ob_b1_signed_extents[1] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
		case AXIS_Z1xX2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[0], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[3], // OBB1AxisY * OBB2AxisX
			                           obb_collision_data_->ob_b1_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[1], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[0], // OBB1AxisX * OBB2AxisX
			                           obb_collision_data_->ob_b1_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[1], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[8], // OBB1AxisZ * OBB2AxisZ
			                           obb_collision_data_->ob_b2_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[2], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[7], // OBB1AxisZ * OBB2AxisY
			                           obb_collision_data_->ob_b2_size_[2]);

			_TVarType div = 1.0f - obb_collision_data_->c_[6] * obb_collision_data_->c_[6];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType z1_cd = obb_collision_data_->ob_b1_rot_axis_[2].Dot(collision_dist);
				_TVarType x2_cd = obb_collision_data_->ob_b2_rot_axis_[0].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[0] * ob_b1_signed_extents[0] +
				                  c[3] * ob_b1_signed_extents[1] - x2_cd;
				ob_b1_signed_extents[2] = (z1_cd +
					c[6] * temp +
					c[7] * ob_b2_signed_extents[1] +
					c[8] * ob_b2_signed_extents[2]) / div;
			} else {
				ob_b1_signed_extents[2] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
		case AXIS_Z1xY2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[0], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[4], // OBB1AxisY * OBB2AxisY
			                           obb_collision_data_->ob_b1_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[1], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[1], // OBB1AxisX * OBB2AxisY
			                           obb_collision_data_->ob_b1_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[0], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[8], // OBB1AxisZ * OBB2AxisZ
			                           obb_collision_data_->ob_b2_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[2], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[6], // OBB1AxisZ * OBB2AxisX
			                           obb_collision_data_->ob_b2_size_[2]);

			_TVarType div = 1.0f - obb_collision_data_->c_[7] * obb_collision_data_->c_[7];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType z1_cd = obb_collision_data_->ob_b1_rot_axis_[2].Dot(collision_dist);
				_TVarType y2_cd = obb_collision_data_->ob_b2_rot_axis_[1].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[1] * ob_b1_signed_extents[0] +
				                  c[4] * ob_b1_signed_extents[1] - y2_cd;
				ob_b1_signed_extents[2] = (z1_cd +
					c[7] * temp +
					c[6] * ob_b2_signed_extents[0] +
					c[8] * ob_b2_signed_extents[2]) / div;
			} else {
				ob_b1_signed_extents[2] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
		case AXIS_Z1xZ2: {
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[0], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[5], // OBB1AxisY * OBB2AxisZ
			                           obb_collision_data_->ob_b1_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b1_signed_extents[1], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[2], // OBB1AxisX * OBB2AxisZ
			                           obb_collision_data_->ob_b1_size_[1]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[0], -, +,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[7], // OBB1AxisZ * OBB2AxisY
			                           obb_collision_data_->ob_b2_size_[0]);
			TSPMACRO_GET_SIGNED_EXTENT(ob_b2_signed_extents[1], +, -,
			                           obb_collision_data_->side_,
			                           obb_collision_data_->c_[6], // OBB1AxisZ * OBB2AxisX
			                           obb_collision_data_->ob_b2_size_[1]);

			_TVarType div = 1.0f - obb_collision_data_->c_[8] * obb_collision_data_->c_[8];

			if(abs(div) > epsilon) {
				Vector3D<_TVarType> collision_dist(obb_collision_data_->ob_b2_collision_pos_ -
				                                   obb_collision_data_->ob_b1_collision_pos_);

				_TVarType z1_cd = obb_collision_data_->ob_b1_rot_axis_[2].Dot(collision_dist);
				_TVarType z2_cd = obb_collision_data_->ob_b2_rot_axis_[2].Dot(collision_dist);
				_TVarType* c = obb_collision_data_->c_;

				_TVarType temp = c[2] * ob_b1_signed_extents[0] +
				                  c[5] * ob_b1_signed_extents[1] - z2_cd;
				ob_b1_signed_extents[2] = (z1_cd +
					c[8] * temp +
					c[6] * ob_b2_signed_extents[0] +
					c[7] * ob_b2_signed_extents[1]) / div;
			} else {
				ob_b1_signed_extents[2] = 0.0f;
			}
			TSPMACRO_GET_CORNER_POINT(point[0],
			                          obb_collision_data_->ob_b1_rot_axis_,
			                          obb_collision_data_->ob_b1_collision_pos_,
			                          ob_b1_signed_extents);
			return;
		}
	};
}

template<class _TVarType>
void CollisionDetector3D<_TVarType>::GetOBBEdgeSurfaceIntersection(Vector3D<_TVarType> point[8], int &num_points, CollisionType collision_type) {
	// The intersection is either a vector (edge-to-surface collision) or an area (surface-to-surface).
	// Determine the area of intersection.

	num_points = 0;

	const _TVarType epsilon = MathTraits<_TVarType>::Eps();
	const _TVarType one_cmp = 1.0f - epsilon;

	//
	// Step 1: The setup. To make things easier, setup the following arrays to make
	//                    the box with the separating plane become the box with index = 0.
	//
	Vector3D<_TVarType>* _box_orientation[2];
	Vector3D<_TVarType>* _box_center[2];
	Vector3D<_TVarType>  _plane_normal[2];

	int box_normal_index[2];
	int box_edge_index[2];
	_TVarType box_side[2];
	_TVarType* _box_extents[2];

	int box0;
	int box1;

	switch(obb_collision_data_->box_with_separating_plane_) {
		case 0: {
			box0 = 0;
			box1 = 1;
		} break;
		case 1: {
			box0 = 1;
			box1 = 0;
		} break;
		default: // Error...
		return;
	}

	box_normal_index[box0] = obb_collision_data_->box1_normal_index_;
	box_edge_index  [box0] = obb_collision_data_->box1_edge_index_;
	_box_center     [box0] = &obb_collision_data_->ob_b1_collision_pos_;
	_box_orientation[box0] = obb_collision_data_->ob_b1_rot_axis_;
	_box_extents    [box0] = obb_collision_data_->ob_b1_size_;
	box_side       [box0] = obb_collision_data_->box1_side_;
	_plane_normal   [box0] = (_box_orientation[box0][box_normal_index[box0]] * box_side[box0]);

	box_normal_index[box1] = obb_collision_data_->box2_normal_index_;
	box_edge_index  [box1] = obb_collision_data_->box2_edge_index_;
	_box_center     [box1] = &obb_collision_data_->ob_b2_collision_pos_;
	_box_orientation[box1] = obb_collision_data_->ob_b2_rot_axis_;
	_box_extents    [box1] = obb_collision_data_->ob_b2_size_;
	box_side       [box1] = obb_collision_data_->box2_side_;
	_plane_normal   [box1] = (_box_orientation[box1][box_normal_index[box1]] * box_side[box1]);

	//
	// Step 3: Get the vertices. Determine the points of the edge/surfaces.
	//

	Vector3D<_TVarType> ob_b1_side_vertices[4];
	Vector3D<_TVarType> ob_b2_side_vertices[4];
	Vector3D<_TVarType> ob_b2_edge_vertices[2];
	int ob_b1_edge_index[2];
	int ob_b2_edge_index[2];

	// Get the vertices and stuff of the side that is the separating plane...
	SetupBoxSide(*_box_center[0],
		_box_orientation[0],
		_box_extents[0],
		box_normal_index[0],
		_plane_normal[0],
		ob_b1_edge_index[0], // Return parameter.
		ob_b1_edge_index[1], // Return parameter.
		ob_b1_side_vertices);// Return parameter.

	// The normals of the planes that makes the sides of the separating plane surface.
	Vector3D<_TVarType> normals[4];
	normals[0] = _box_orientation[0][ob_b1_edge_index[1]] * -1.0f;
	normals[1] = _box_orientation[0][ob_b1_edge_index[0]];
	normals[2] = _box_orientation[0][ob_b1_edge_index[1]];
	normals[3] = _box_orientation[0][ob_b1_edge_index[0]] * -1.0f;

	if(collision_type == kEdgeCollision) {
		if(box_normal_index[0] == -1) {
			// Error, Box0 can't be anything else but parallel to itself.
			return;
		}

		// Setup a side that is perpendicular to the separating plane.
		SetupBoxSide(*_box_center[1],
			_box_orientation[1],
			_box_extents[1],
			box_edge_index[1],
			-_box_orientation[1][box_edge_index[1]],
			ob_b2_edge_index[0], // Return parameter.
			ob_b2_edge_index[1], // Return parameter.
			ob_b2_side_vertices);// Return parameter.

		//
		// Get the edge by finding the vertex that is closest to the separating plane.
		//
		_TVarType min_distance;
		int i;

		// Loop over the vertices...
		for(i = 0; i < 4; i++) {
			// The vector between the vertex and box0's center.
			Vector3D<_TVarType> delta(ob_b2_side_vertices[i] - *_box_center[0]);

			// Project it on the separating axis.
			_TVarType dot = _box_orientation[0][box_normal_index[0]].Dot(delta);

			// Get the distance from the separating plane.
			_TVarType distance = (_TVarType)fabs((_TVarType)fabs(dot) - _box_extents[0][box_normal_index[0]]);

			// Store the closest one.
			if(i == 0 || distance < min_distance) {
				min_distance = distance;

				ob_b2_edge_vertices[0] = ob_b2_side_vertices[i];
				ob_b2_edge_vertices[1] = ob_b2_side_vertices[i] +
					_box_orientation[1][box_edge_index[1]] *
					_box_extents[1][box_edge_index[1]] * 2.0f;
			}
		}

		// Now clip the edge against the sides of the separating plane surface.
		for(i = 0; i < 4; i++) {
			ClipVectorWithPlane(ob_b1_side_vertices[i],
				normals[i],
				ob_b2_edge_vertices[0],
				ob_b2_edge_vertices[1]);
		}

		point[0] = ob_b2_edge_vertices[0];
		point[1] = ob_b2_edge_vertices[1];
		num_points = 2;

		return;
	} else {
		// Surface to surface collision. We've already got what we want (the planes):
		SetupBoxSide(*_box_center[1],
			_box_orientation[1],
			_box_extents[1],
			box_normal_index[1],
			_plane_normal[1],
			ob_b2_edge_index[0],	// Return parameter.
			ob_b2_edge_index[1],	// Return parameter.
			ob_b2_side_vertices);	// Return parameter.

		// At least 4 points...
		num_points = 4;

		int i;
		for(i = 0; i < 4; i++) {
			point[i] = ob_b2_side_vertices[i];
		}

		int start_point = 3;
		int end_point = 0;
		int point_count = 0;

		Vector3D<_TVarType> clipped_points[8];
		Vector3D<_TVarType> p1;
		Vector3D<_TVarType> p2;

		for(int j = 0; j < 4; j++) {
			for(i = 0; i < num_points; i++) {
				p1 = point[start_point];
				p2 = point[end_point];
				int result = ClipVectorWithPlane(ob_b1_side_vertices[j],
													normals[j], p1, p2);
				switch(result) {
					case 1: {
						clipped_points[point_count++] = p1;
					} // TRICKY: Fallthrough!
					case 2: {
						clipped_points[point_count++] = p2;
					} break;
					case 3: {
						clipped_points[point_count++] = p2;
					} break;
				}

				start_point = end_point;
				end_point++;
			}

			for(i = 0; i < point_count; i++) {
				point[i] = clipped_points[i];
			}

			num_points = point_count;

			// Reset...
			start_point = point_count - 1;
			end_point = 0;
			point_count = 0;
		}
	}
}

template<class _TVarType>
void CollisionDetector3D<_TVarType>::SetupBoxSide(Vector3D<_TVarType> box_center,
						  Vector3D<_TVarType> box_orientation[3],
						  _TVarType box_extents[3],
						  int normal_index,
						  Vector3D<_TVarType> normal,
						  int& ret_edge_index1,	// One of the edges that is not parallel to the normal.
						  int& ret_edge_index2,	// One of the edges that is not parallel to the normal.
						  Vector3D<_TVarType> ret_side_vertices[4]) {
	// First find the correct edge indices.
	switch(normal_index) {
		case 0: {			// Normal is x-axis.
			ret_edge_index1 = 1;	// Y-axis.
			ret_edge_index2 = 2;	// Z-axis.
		} break;
		case 1: {			// Normal is y-axis.
			ret_edge_index1 = 0;	// X-axis.
			ret_edge_index2 = 2;	// Z-axis.
		} break;
		case 2: {			// Normal is z-axis.
			ret_edge_index1 = 0;	// X-axis.
			ret_edge_index2 = 1;	// Y-axis.
		} break;
	};

	Vector3D<_TVarType> box[3];
	box[normal_index]   = normal * box_extents[normal_index];
	box[ret_edge_index1] = box_orientation[ret_edge_index1] * box_extents[ret_edge_index1];
	box[ret_edge_index2] = box_orientation[ret_edge_index2] * box_extents[ret_edge_index2];

	ret_side_vertices[0] = box_center + box[0] + box[1] + box[2];
	ret_side_vertices[1] = ret_side_vertices[0] - box[ret_edge_index1] * 2.0f;
	ret_side_vertices[2] = ret_side_vertices[1] - box[ret_edge_index2] * 2.0f;
	ret_side_vertices[3] = ret_side_vertices[2] + box[ret_edge_index1] * 2.0f;
}

template<class _TVarType>
int CollisionDetector3D<_TVarType>::ClipVectorWithPlane(Vector3D<_TVarType> plane_point,
							Vector3D<_TVarType> plane_normal,
							Vector3D<_TVarType>& vector_point1,
							Vector3D<_TVarType>& vector_point2) {
	_TVarType side1;
	_TVarType side2;
	_TVarType time;

	side1 = PlaneEquation(plane_point, plane_normal, vector_point1);
	side2 = PlaneEquation(plane_point, plane_normal, vector_point2);

	if(side1 >= 0.0f && side2 >= 0.0f) {
		return 3;
	} else if(side1 <= 0.0f && side2 <= 0.0f) {
		return 0;
	} else if(side1 >= 0.0f && side2 <= 0.0f) {
		// Point2 is moved to a clipped position.
		Vector3D<_TVarType> direction(vector_point2 - vector_point1);

		GetTimeOfVectorPlaneIntersection(plane_point,
						 plane_normal,
						 vector_point1,
						 direction,
						 time);

		vector_point2 = vector_point1 + direction * time;

		return 2;
	} else {
		// Point1 is moved to a clipped position.
		Vector3D<_TVarType> direction(vector_point2 - vector_point1);

		GetTimeOfVectorPlaneIntersection(plane_point,
						 plane_normal,
						 vector_point1,
						 direction,
						 time);

		vector_point1 = vector_point1 + direction * time;

		return 1;
	}
}

template<class _TVarType>
bool CollisionDetector3D<_TVarType>::GetTimeOfVectorPlaneIntersection(const Vector3D<_TVarType>& plane_point,
														  const Vector3D<_TVarType>& plane_normal,
														  const Vector3D<_TVarType>& vector_point,
														  const Vector3D<_TVarType>& vector_direction,
														  _TVarType& return_time) {
	_TVarType d = -(plane_normal * plane_point);
	_TVarType divider = plane_normal * vector_direction;

	if(divider == 0.0f) {
		return false;
	}

	_TVarType dividend = -(plane_normal * vector_point + d);

	return_time = dividend / divider;

	return true;
}
