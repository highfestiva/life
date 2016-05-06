
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/contextpath.h"
#include "../../tbc/include/chunkybonegeometry.h"



namespace cure {



ContextPath::SplinePath::SplinePath(vec3* key_frames,
	float* time_tags,
	int count,
	str type,
	float distance_normal,
	float likeliness):
	Parent(key_frames, time_tags, count, SplineShape::kTypeCatmullrom, TAKE_OWNERSHIP),
	type_(type),
	distance_normal_(distance_normal),
	likeliness_(likeliness) {
	EnableModulo(false);
}

ContextPath::SplinePath::SplinePath(const SplinePath& original):
	Parent(original),
	type_(original.type_),
	distance_normal_(original.distance_normal_),
	likeliness_(original.likeliness_) {
}

const str& ContextPath::SplinePath::GetType() const {
	return type_;
}

float ContextPath::SplinePath::GetDistanceNormal() const {
	return distance_normal_;
}

float ContextPath::SplinePath::GetLikeliness() const {
	return likeliness_;
}

float ContextPath::SplinePath::GetTimeAsDistance(float time) const {
	return time / distance_normal_;
}

float ContextPath::SplinePath::GetDistanceLeft() const {
	const float _time = GetCurrentInterpolationTime();
	return GetTimeAsDistance(1-_time);
}



ContextPath::ContextPath(ResourceManager* resource_manager, const str& class_id):
	CppContextObject(resource_manager, class_id) {
}

ContextPath::~ContextPath() {
	PathArray::iterator x = path_array_.begin();
	for (; x != path_array_.end(); ++x) {
		delete *x;
	}
	path_array_.clear();
}



void ContextPath::SetTagIndex(int index) {
	const tbc::ChunkyClass::Tag& tag = ((CppContextObject*)parent_)->GetClass()->GetTag(index);
	deb_assert(tag.float_value_list_.size() == 1);
	deb_assert(tag.string_value_list_.size() <= 1);
	const size_t body_count = tag.body_index_list_.size();
	deb_assert(body_count >= 2);
	if (tag.float_value_list_.size() != 1 || body_count < 2) {
		return;
	}

	str _type;
	if (tag.string_value_list_.size() == 1) {
		_type = tag.string_value_list_[0];
	}

	// 1. Store positions.
	// 2. Calculate total distance (shortest distance through all vertices).
	// 3. Scale time on each key so 1.0 corresponds total length.
	//
	// TRICKY: this algorithm adds one extra head vertex and two extra tail vertices to the
	// path, to avoid spline looping.
	float total_distance = 1;	// Start at an offset (head).
	tbc::ChunkyPhysics* physics = parent_->GetPhysics();
	vec3* path_positions = new vec3[1+body_count+2];
	float* times = new float[1+body_count+2];
	size_t x;
	for (x = 0; x < body_count; ++x) {
		const int bone_index = tag.body_index_list_[x];
#ifdef LEPRA_DEBUG
		tbc::ChunkyBoneGeometry* bone = physics->GetBoneGeometry(bone_index);
		deb_assert(bone->GetBoneType() == tbc::ChunkyBoneGeometry::kBonePosition);
#endif // Debug
		path_positions[x+1] = physics->GetBoneTransformation(bone_index).GetPosition();
		if (x > 0) {	// We only start from the start position (not origo).
			total_distance += path_positions[x+1].GetDistance(path_positions[x]);
			times[x+1] = total_distance;
		}
	}
	times[0] = -1;
	path_positions[0] = path_positions[1];
	times[1] = 0;
	times[1+body_count] = total_distance+1.0f;
	times[1+body_count+1] = total_distance+2.0f;
	const float scale = 1 / total_distance;
	for (size_t x = 0; x <= 1+body_count+1; ++x) {
		times[x] *= scale;
	}
	path_positions[1+body_count] = path_positions[1+body_count-1];
	path_positions[1+body_count+1] = path_positions[1+body_count-1];
	const float _likeliness = tag.float_value_list_[0];
	SplinePath* spline_path = new SplinePath(path_positions, times, body_count+2, _type, scale, _likeliness);
	spline_path->StartInterpolation(0);
	path_array_.push_back(spline_path);
}

int ContextPath::GetPathCount() const {
	return (int)path_array_.size();
}

ContextPath::SplinePath* ContextPath::GetPath(int index) const {
	if (!(index >= 0 && index < GetPathCount())) {
		// This shouldn't happen... Probably already killed.
		return 0;
	}
	return path_array_[index];
}

ContextPath::SplinePath* ContextPath::GetPath(const str& type) const {
	PathArray::const_iterator x = path_array_.begin();
	for (; x != path_array_.end(); ++x) {
		if ((*x)->GetType() == type) {
			return *x;
		}
	}
	return 0;
}



}
