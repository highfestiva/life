
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/cubicspline.h"
#include "../../lepra/include/math.h"
#include "../include/terrainfunction.h"



namespace tbc {

TerrainFunction::TerrainFunction(float amplitude, const vec2& position, float inner_radius, float outer_radius):
	amplitude_(amplitude),
	position_(position),
	inner_radius_(inner_radius),
	outer_radius_(outer_radius) {
	deb_assert(inner_radius_ <= outer_radius_);	// Outer radius should be AT LEAST as big as the inner ditto.
}

TerrainFunction::~TerrainFunction() {
}

void TerrainFunction::AddFunction(TerrainPatch& patch) const {
	// Read all frequently used data from pGrid.
	const vec2& world_south_west(patch.GetSouthWest());
	const vec2& world_north_east(patch.GetNorthEast());
	const int vertex_count_x = patch.GetVertexRes();
	const int vertex_count_y = patch.GetVertexRes();
	const float grid_width = patch.GetPatchSize();
	const float grid_height = patch.GetPatchSize();

	float a;

	// Calculate start X vertex index.
	int vertex_minimum_x;
	a = position_.x-outer_radius_;
	if (a <= world_south_west.x) {
		vertex_minimum_x = 0;
	} else {
		vertex_minimum_x = (int)::floor((a-world_south_west.x) * vertex_count_x / grid_width);
	}

	// Calculate end X vertex index.
	int vertex_maximum_x;
	a = position_.x+outer_radius_;
	if (a >= world_north_east.x) {
		vertex_maximum_x = vertex_count_x;
	} else {
		vertex_maximum_x = (int)::ceil((a-world_south_west.x) * vertex_count_x / grid_width);
	}

	// Calculate start Y vertex index.
	int vertex_minimum_y;
	a = position_.y-outer_radius_;
	if (a <= world_south_west.y) {
		vertex_minimum_y = 0;
	} else {
		vertex_minimum_y = (int)::floor((a-world_south_west.y) * vertex_count_y / grid_height);
	}

	// Calculate end Y vertex index.
	int vertex_maximum_y;
	a = position_.y+outer_radius_;
	if (a >= world_north_east.y) {
		vertex_maximum_y = vertex_count_y;
	} else {
		vertex_maximum_y = (int)::ceil((a-world_south_west.y) * vertex_count_y / grid_height);
	}

	patch.IterateOverPatch(*this, vertex_minimum_x, vertex_maximum_x, vertex_minimum_y, vertex_maximum_y);
}

void TerrainFunction::ModifyVertex(const vec2& world_flat_pos, vec3& vertex) const {
	const float distance = world_flat_pos.GetDistance(position_);
	if (distance < outer_radius_) {
		// We made it!
		float _scale;
		if (distance <= inner_radius_) {
			// This point is inside, or on, the inner radius.
			_scale = 1;
		} else {
			// We linearly down-scale our method parameter. This does not necessarily mean
			// that the parameter is used in a linear fasion.
			_scale = 1-(distance-inner_radius_)/(outer_radius_-inner_radius_);
		}
		const vec2 relative_normalized_pos = (world_flat_pos - position_) / outer_radius_;
		AddPoint(relative_normalized_pos.x, relative_normalized_pos.y, _scale, distance, vertex);
	}
}

float TerrainFunction::GetAmplitude() const {
	return (amplitude_);
}

const vec2& TerrainFunction::GetPosition() const {
	return (position_);
}

float TerrainFunction::GetInnerRadius() const {
	return (inner_radius_);
}

float TerrainFunction::GetOuterRadius() const {
	return (outer_radius_);
}


TerrainFunctionGroup::TerrainFunctionGroup(TerrainFunction** tf_array, int count,
					   DataPolicy array_policy,
					   SubDataPolicy tf_policy) :
	tf_array_(tf_array),
	count_(count),
	array_policy_(array_policy),
	tf_policy_(tf_policy) {
	if (array_policy_ == kFullCopy) {
		tf_array_ = new TerrainFunction*[count_];
		::memcpy(tf_array_, tf_array, count_ * sizeof(TerrainFunction*));
	}
}

TerrainFunctionGroup::~TerrainFunctionGroup() {
	if (tf_policy_ == kTakeSubdataOwnership) {
		for (int i = 0; i < count_; i++) {
			delete tf_array_[i];
		}
	}

	if (array_policy_ != kCopyReference) {
		delete[] tf_array_;
	}
}

void TerrainFunctionGroup::AddFunctions(TerrainPatch& patch) const {
	// No optimizations like the ones in TerrainFunction are possible.
	patch.IterateOverPatch(*this, 0, patch.GetVertexRes(), 0, patch.GetVertexRes());
}

void TerrainFunctionGroup::ModifyVertex(const vec2& world_flat_pos, vec3& vertex) const {
	for (int i = 0; i < count_; i++) {
		tf_array_[i]->ModifyVertex(world_flat_pos, vertex);
	}
}

TerrainConeFunction::TerrainConeFunction(float amplitude, const vec2& position, float inner_radius, float outer_radius):
	TerrainFunction(amplitude, position, inner_radius, outer_radius) {
	// Calculate the "profile length", which is the slope plus the inner radius.
	//      ____|____
	//   pl/ ir |    \
	// ___/     |     \___
	//    or
	radius_differance_ = outer_radius_-inner_radius_;
	slope_length_ = ::sqrt(radius_differance_*radius_differance_ + amplitude*amplitude);
	profile_length_ = slope_length_ + inner_radius_;
	plateau_part_ = inner_radius_/profile_length_;
}

void TerrainConeFunction::AddPoint(float relative_normalized_x, float relative_normalized_y,
	float /*scale*/, float absolute_xy_distance, vec3& point) const {
	// Does not simply crunch the terrain grid vertically, but has a smarter
	// function that distributes the grid vertices evenly around the cone profile.

	//scale;

	// Calculate a new distance, where the slope is weighed in correctly.
	const float normalized_xy_distance = absolute_xy_distance/outer_radius_;
	float new_absolute_xy_distance;
	if (normalized_xy_distance < plateau_part_) {
		// We're inside the inner radius = simple scale.
		new_absolute_xy_distance = normalized_xy_distance/plateau_part_*inner_radius_;
	} else {
		// We're between the inner radius and the outer radius.
		const float slope_part = 1-plateau_part_;
		new_absolute_xy_distance = (normalized_xy_distance-plateau_part_)/slope_part*radius_differance_ + inner_radius_;
	}

	// Move X and Y out.
	if(absolute_xy_distance != 0.0f) {
		const float xy_scale = outer_radius_/absolute_xy_distance;
		float dx = relative_normalized_x*xy_scale*new_absolute_xy_distance;
		float dy = relative_normalized_y*xy_scale*new_absolute_xy_distance;
		point.x = position_.x+dx;
		point.y = position_.y+dy;
	}

	// Calculate new amplitude from the new distance.
	float new_amplitude;
	if (new_absolute_xy_distance <= inner_radius_) {
		new_amplitude = amplitude_;
	} else {
		new_amplitude = (outer_radius_-new_absolute_xy_distance)/(outer_radius_-inner_radius_)*amplitude_;
	}
	point.z += new_amplitude;
}



TerrainHemisphereFunction::TerrainHemisphereFunction(float amplitude, const vec2& position, float inner_radius, float outer_radius):
	TerrainFunction(amplitude, position, inner_radius, outer_radius) {
	//     ___
	//   /     \
	// _|       |_
}

void TerrainHemisphereFunction::AddPoint(float relative_normalized_x, float relative_normalized_y,
	float scale, float absolute_xy_distance, vec3& point) const {
	// Does not simply crunch the terrain grid vertically, but has a smarter
	// function that distributes the grid vertices evenly around the hemisphere.

	// Calculate dx, dy and dz (the radius is 1).
	float dx = relative_normalized_x;
	float dy = relative_normalized_y;
	float dz = ::sqrt(1 - (dx*dx+dy*dy));
	// Angle between XY-plane and the point (dx, dy, dz).
	const float alpha = ::asin(dz/1);	// Divided by normalized radius 1.
	// This is the actual function. The new angle strives with a power of two towards the steepest
	const float beta = 2/PIF*alpha*alpha;
	if (absolute_xy_distance != 0) {
		// Calculate new distance from center in XY-plane.
		const float new_absolute_xy_distance = outer_radius_*::cos(beta);
		const float new_linear_xy_distance_scale = new_absolute_xy_distance/absolute_xy_distance;
		// Elongate dx and dy, since the angle in the XY-plane hasn't changed.
		dx *= new_linear_xy_distance_scale*outer_radius_;
		dy *= new_linear_xy_distance_scale*outer_radius_;
		point.x = Math::Lerp(point.x, position_.x+dx, scale);
		point.y = Math::Lerp(point.y, position_.y+dy, scale);
	}
	// Recalculate dz with the new vertical angle from our vector (dx, dy).
	// We don't scale it up by outer_radius_ (to make it a perfect hemisphere),
	// but instead by amplitude_ so that its size can be user controlled.
	dz = amplitude_*::sin(beta);

	point.z += dz*scale;
}



TerrainDuneFunction::TerrainDuneFunction(float width_proportion, float curvature, float amplitude,
	const vec2& position, float inner_radius, float outer_radius):
	TerrainFunction(amplitude, position, inner_radius, outer_radius),
	profile_spline_(0),
	width_proportion_(width_proportion) {
	vec2* coordinates = new vec2[10];
	float* times = new float[11];
	times[0] = -1.0f;
	coordinates[0].x = -1;
	coordinates[0].y = 0;
	times[1] = -0.5f;
	coordinates[1].x = -0.1f;
	coordinates[1].y = 0;
	times[2] = -0.35f;
	coordinates[2].x = 0.4f*curvature;
	coordinates[2].y = -0.25f;
	times[3] = -0.2f;
	coordinates[3].x = -0.05f*curvature;
	coordinates[3].y = 0.41f+curvature*0.4f;
	times[4] = -0.1f;
	coordinates[4].x = -0.18f*curvature;
	coordinates[4].y = 0.58f-curvature*0.4f;
	times[5] = 0.0f;
	coordinates[5].x = -0.3f*curvature;
	coordinates[5].y = 0.75f;
	times[6] = 0.1f;
	coordinates[6].x = 0.0f;
	coordinates[6].y = 1.0f;
	times[7] = 0.2f;
	coordinates[7].x = 0.2f;
	coordinates[7].y = 0.3f;
	times[8] = 0.6f;
	coordinates[8].x = 0.6f;
	coordinates[8].y = 0.0f;
	times[9] = 0.9f;
	coordinates[9].x = 1.0f;
	coordinates[9].y = 0.0f;
	times[10] = 1.0f;
	profile_spline_ = new CubicDeCasteljauSpline<vec2, float>(coordinates,
		times, 10, CubicDeCasteljauSpline<vec2, float>::kTypeBspline,
		TAKE_OWNERSHIP);
	profile_spline_->StartInterpolation(-1);
}

TerrainDuneFunction::~TerrainDuneFunction() {
	delete (profile_spline_);
	profile_spline_ = 0;
}

void TerrainDuneFunction::AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float, vec3& point) const {
	profile_spline_->GotoAbsoluteTime(relative_normalized_x);
	vec2 d;
	// Create a small bulge on the edges.
	const float t = 3*relative_normalized_x;
	const float bz = ::exp(-t*t/width_proportion_)*0.5f;

	float y_scale = ::fabs(relative_normalized_y)*outer_radius_;
	if (y_scale > inner_radius_) {
		y_scale = 1-(y_scale-inner_radius_)/(outer_radius_-inner_radius_);
	} else {
		y_scale = 1;
	}

	d = profile_spline_->GetValue();
	if (y_scale < 0.5f) {
		d.x = Math::Lerp(relative_normalized_x, d.x, y_scale*2);
		d.y = bz*y_scale*2;
	} else {
		d.y = Math::Lerp(bz, d.y, (y_scale-0.5f)*2);
	}

	point.x = Math::Lerp(point.x, position_.x+d.x*outer_radius_, scale);
	point.z += d.y*amplitude_*scale;
}



TerrainDecorator::TerrainDecorator(TerrainFunction* function):
	function_(function) {
}

TerrainDecorator::~TerrainDecorator() {
	function_ = 0;
}



TerrainAmplitudeFunction::TerrainAmplitudeFunction(const float* amplitude_vector, unsigned amplitude_vector_length, TerrainFunction* function):
	TerrainFunction(1.0, function->GetPosition(), function->GetInnerRadius(), function->GetOuterRadius()),
	TerrainDecorator(function),
	amplitude_spline_(0) {
	deb_assert(amplitude_vector_length >= 2);

	const float time_step = 2.0f/(amplitude_vector_length+1-1);
	float time = -1;
	float* times = new float[amplitude_vector_length+1];
	for (unsigned x = 0; x < amplitude_vector_length+1; ++x) {
		times[x] = time;
		time += time_step;
	}
	amplitude_spline_ = new CubicDeCasteljauSpline<float, float, float>((float*)amplitude_vector,
		times, amplitude_vector_length, CubicDeCasteljauSpline<float, float, float>::kTypeCatmullrom,
		kFullCopy);
	delete[] (times);
	amplitude_spline_->StartInterpolation(-1);
}

TerrainAmplitudeFunction::~TerrainAmplitudeFunction() {
	delete (amplitude_spline_);
	amplitude_spline_ = 0;
}

void TerrainAmplitudeFunction::AddPoint(float relative_normalized_x, float relative_normalized_y, float scale,
	float absolute_xy_distance, vec3& point) const {
	const float oz = point.z;
	function_->AddPoint(relative_normalized_x, relative_normalized_y, scale, absolute_xy_distance, point);
	amplitude_spline_->GotoAbsoluteTime(relative_normalized_y);
	const float _amplitude = amplitude_spline_->GetValue();
	const float dz = point.z-oz;
	point.z = oz+dz*_amplitude;
}



TerrainWidthFunction::TerrainWidthFunction(float width_factor, TerrainFunction* function):
	TerrainFunction(1.0, function->GetPosition(), function->GetInnerRadius(), function->GetOuterRadius()),
	TerrainDecorator(function),
	width_factor_(width_factor) {
}

void TerrainWidthFunction::AddPoint(float relative_normalized_x, float relative_normalized_y, float scale,
	float absolute_xy_distance, vec3& point) const {
	const float rx = relative_normalized_x/width_factor_;
	absolute_xy_distance = ::sqrt(rx*rx+relative_normalized_y*relative_normalized_y)*outer_radius_;
	if (absolute_xy_distance >= outer_radius_) {
		return;	// TRICKY: optimization by quick return.
	} else if (absolute_xy_distance <= inner_radius_) {
		scale = 1;
	} else {
		scale = 1-(absolute_xy_distance-inner_radius_)/(outer_radius_-inner_radius_);
	}
	float mx = point.x;
	point.x = position_.x+(point.x-position_.x)/width_factor_;
	mx -= point.x;
	function_->AddPoint(rx, relative_normalized_y, scale, absolute_xy_distance, point);
	point.x += mx;
	//point.x = (point.x-lOriginalPoint.x)*width_factor_*scale;
	//point.x = Math::Lerp(point.x, (point.x-position_.x)*width_factor_ + position_.x, scale);
}


TerrainPushFunction::TerrainPushFunction(const float* push_vector, unsigned push_vector_length, TerrainFunction* function):
	TerrainFunction(1.0, function->GetPosition(), function->GetInnerRadius(), function->GetOuterRadius()),
	TerrainDecorator(function),
	push_spline_(0) {
	deb_assert(push_vector_length >= 2);

	const float time_step = 2.0f/(push_vector_length+1-1);
	float time = -1;
	float* times = new float[push_vector_length+1];
	for (unsigned x = 0; x < push_vector_length+1; ++x) {
		times[x] = time;
		time += time_step;
	}
	push_spline_ = new CubicDeCasteljauSpline<float, float, float>((float*)push_vector,
		times, push_vector_length, CubicDeCasteljauSpline<float, float, float>::kTypeCatmullrom,
		kFullCopy);
	delete[] (times);
	push_spline_->StartInterpolation(-1);
}

TerrainPushFunction::~TerrainPushFunction() {
	delete (push_spline_);
	push_spline_ = 0;
}

void TerrainPushFunction::AddPoint(float relative_normalized_x, float relative_normalized_y, float scale,
	float absolute_xy_distance, vec3& point) const {
	function_->AddPoint(relative_normalized_x, relative_normalized_y, scale, absolute_xy_distance, point);
	push_spline_->GotoAbsoluteTime(relative_normalized_y);
	const float push = push_spline_->GetValue();
	point.x += push*scale;
}



TerrainRotateFunction::TerrainRotateFunction(float angle, TerrainFunction* function):
	TerrainFunction(1.0, function->GetPosition(), function->GetInnerRadius(), function->GetOuterRadius()),
	TerrainDecorator(function),
	angle_(angle) {
}

void TerrainRotateFunction::AddPoint(float relative_normalized_x, float relative_normalized_y, float scale,
	float absolute_xy_distance, vec3& point) const {
	// TODO: optimize by using matrices for rotation.
	const float new_relative_normalized_x = ::cos(-angle_)*relative_normalized_x - ::sin(-angle_)*relative_normalized_y;
	const float new_relative_normalized_y = ::sin(-angle_)*relative_normalized_x + ::cos(-angle_)*relative_normalized_y;
	vec3 rotate_point(new_relative_normalized_x, new_relative_normalized_y, point.z/outer_radius_);
	rotate_point *= outer_radius_;
	rotate_point.x += position_.x;
	rotate_point.y += position_.y;
	function_->AddPoint(new_relative_normalized_x, new_relative_normalized_y, scale, absolute_xy_distance, rotate_point);
	rotate_point.x -= position_.x;
	rotate_point.y -= position_.y;
	point.x = ::cos(angle_)*rotate_point.x - ::sin(angle_)*rotate_point.y + position_.x;
	point.y = ::sin(angle_)*rotate_point.x + ::cos(angle_)*rotate_point.y + position_.y;
	point.z = rotate_point.z;
}



}
