
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/math.h"
#include "../include/cubicspline.h"

#include <math.h>



namespace lepra {



void CubicSpline::Init(int num_points, int num_values_per_point) {
	num_points_ = num_points;
	num_values_per_point_ = num_values_per_point;

	if (point_ != 0)
		delete[] point_;

	if (segment_ != 0)
		delete[] segment_;

	if (modular_value_ != 0)
		delete[] modular_value_;

	if (min_mod_value_ != 0)
		delete[] min_mod_value_;

	if (max_mod_value_ != 0)
		delete[] max_mod_value_;

	point_   = new Point[num_points_];
	segment_ = new Segment[num_points_];

	modular_value_ = new bool[num_values_per_point];
	min_mod_value_  = new float[num_values_per_point];
	max_mod_value_  = new float[num_values_per_point];

	int i;
	for (i = 0; i < num_points_; i++) {
		point_[i].Init(num_values_per_point_);
		segment_[i].Init(num_values_per_point_);
	}

	for (i = 0; i < num_values_per_point_; i++) {
		modular_value_[i] = false;
		min_mod_value_[i] = 0.0f;
		max_mod_value_[i] = 0.0f;
	}
}

bool CubicSpline::SetPointValues(int point_index, float time_tag, const float* value) {
	point_[point_index].t_ = time_tag;
	point_[point_index].values_.Set(value);
	initialized_ = false;

	return true;
}

void CubicSpline::SetModValue(int value_index, float min_mod_value, float max_mod_value) {
	modular_value_[value_index] = true;
	min_mod_value_[value_index] = min_mod_value;
	max_mod_value_[value_index] = max_mod_value;
	initialized_ = false;
}

void CubicSpline::StartInterpolation(float time) {
	StartInterpolation(time, kImOnce, 0, 0);
	//CalcABCD();
}

void CubicSpline::StartInterpolation(float time, const float* start_slope, const float* end_slope) {
	Values _start_slope(num_values_per_point_);
	Values _end_slope(num_values_per_point_);

	_start_slope.Set(start_slope);
	_end_slope.Set(end_slope);

	StartInterpolation(time, kImOnce, &_start_slope, &_end_slope);
}

void CubicSpline::StartLoopedInterpolation(float time, float extra_time_tag) {
	extra_time_tag_ = extra_time_tag;
	StartInterpolation(time, kImLoop, 0, 0);
	//CalcABCD();
}

void CubicSpline::StartInterpolation(float time, InterpolationMode mode, Values* start_slope, Values* end_slope) {
	ReInitialize(mode, start_slope, end_slope);

	current_time_ = time;

	current_point_ = 0;
	StepForwardToCurrentFrame();
}

void CubicSpline::ReInitialize(InterpolationMode mode, Values* start_slope, Values* end_slope) {
	if (mode != mode_) {
		mode_ = mode;
		initialized_ = false;
	}

	if (initialized_ == false) {
		if (mode_ == kImOnce) {
			InitNaturalCubic(start_slope, end_slope, (start_slope != 0 && end_slope != 0));
		} else {
			InitNaturalCubicClosed();
		}

		initialized_ = true;
	}
}

void CubicSpline::StepInterpolation(float delta_time) {
	if (delta_time == 0) {
		return;	// TRICKY: optimization.
	}

	current_time_ += delta_time;

	if (mode_ == kImOnce) {
		if (current_time_ > GetMaxTime()) {
			current_time_ = GetMaxTime();
		} else if (current_time_ < GetMinTime()) {
			current_time_ = GetMinTime();
		}
	} else {
		float prev_time = current_time_;
		current_time_ = Math::Mod(current_time_, GetMinTime(), extra_time_tag_);
		delta_time = current_time_-prev_time;	// Used for stepping backwards.
	}

	if (delta_time < 0.0f) {
		while (current_point_ > 0 && point_[current_point_].t_ > current_time_) {
			current_point_--;
		}
	}
	StepForwardToCurrentFrame();

	//CalcABCD();
}

void CubicSpline::GotoAbsoluteTime(float time) {
	StepInterpolation(time-current_time_);
}

float CubicSpline::FindNearestTime(float step_length, const float* where, float& nearest_distance, float* nearest_point) {
	// Ugly search algorithm. Looks in the step direction until the distance becomes greater than
	// the starting distance. Then turns around and looks in the other direction at half the velocity.
	// And so forth. This could probably be optimized using Newton-Raphson.
	float _nearest_distance[4];
	float new_nearest_distance[4];
	float distance2 = 0;
	int x;
	for (x = 0; x < num_values_per_point_; ++x) {
		_nearest_distance[x] = GetValue(x)-where[x];
		distance2 += _nearest_distance[x]*_nearest_distance[x];
	}
	for (int y = 0; y < 20; ++y) {
		const int zc = 8;
		int z;
		for (z = 0; z < zc; ++z) {
			StepInterpolation(step_length);
			float new_distance2 = 0;
			for (int x = 0; x < num_values_per_point_; ++x) {
				new_nearest_distance[x] = GetValue(x)-where[x];
				new_distance2 += new_nearest_distance[x]*new_nearest_distance[x];
			}
			if (new_distance2 >= distance2) {
				StepInterpolation(-step_length);	// Go back to previous point.
				step_length = step_length*-0.5f;	// Change search direction; increase granularity.
				if ((current_time_ == GetMinTime() && step_length < 0) ||
					(current_time_ == GetMaxTime() && step_length > 0)) {
					step_length = -step_length;
				}
			} else {
				distance2 = new_distance2;
				for (int x = 0; x < num_values_per_point_; ++x) {
					_nearest_distance[x] = new_nearest_distance[x];
				} break;
			}
		}
		if (z == zc) {	// Nothing new found in a few loops - might as well give up.
			break;
		}
		step_length *= 0.7f;	// Increase granularity when we've found a closer spot.
	}

	// Fill in our output values.
	nearest_distance = ::sqrt(distance2);
	for (x = 0; x < num_values_per_point_; ++x) {
		nearest_point[x] = _nearest_distance[x]+where[x];
	}
	return (current_time_);
}

void CubicSpline::StepForwardToCurrentFrame() {
	if (mode_ == kImOnce) {
		while (current_point_ < (num_points_ - 2) &&
			point_[current_point_ + 1].t_ < current_time_) {
			current_point_++;
		}
	} else {
		while (current_point_ < (num_points_ - 1) &&
			point_[current_point_ + 1].t_ < current_time_) {
			current_point_++;
		}
	}

	if (current_point_ != num_points_ - 1) {
		current_segment_time_ = (current_time_ - point_[current_point_].t_) /
								(point_[current_point_ + 1].t_ - point_[current_point_].t_);
	} else {
		current_segment_time_ = (current_time_ - point_[current_point_].t_) /
								(extra_time_tag_ - point_[current_point_].t_);
	}
}

void CubicSpline::InitNaturalCubic(Values* start_slope, Values* end_slope, bool use_slopes) {
	InitModValues();

	int n = num_points_ - 1;

	Values* gamma = new Values[num_points_];
	Values* delta = new Values[num_points_];
	Values* __d     = new Values[num_points_];
	int i, j;

	for (i = 0; i < num_points_; i++) {
		gamma[i].Init(num_values_per_point_);
		delta[i].Init(num_values_per_point_);
		__d[i].Init(num_values_per_point_);
	}

	gamma[0] = 0.5f;
	for (i = 1; i < n; i++) {
		for (j = 0; j < num_values_per_point_; j++) {
			gamma[i][j] = 1.0f / (4.0f - gamma[i - 1][j]);
		}
	}

	for (j = 0; j < num_values_per_point_; j++) {
		gamma[n][j] = 1.0f / (2.0f - gamma[n - 1][j]);
		delta[0][j] = 3.0f * (point_[1].values_[j] - point_[0].values_[j]) * gamma[0][j];
	}

	float ref = point_[1].t_ - point_[0].t_;
	float factor;

	for (i = 1; i < n; i++) {
		factor = (point_[i + 1].t_ - point_[i - 1].t_) / (2.0f * ref);
		for (j = 0; j < num_values_per_point_; j++) {
			delta[i][j] = (3.0f * (point_[i + 1].values_[j] - point_[i - 1].values_[j]) * factor - delta[i - 1][j]) * gamma[i][j];
		}
	}

	factor = (point_[n].t_ - point_[n - 1].t_) / ref;
	for (j = 0; j < num_values_per_point_; j++) {
		delta[n][j] = (3.0f * (point_[n].values_[j] - point_[n - 1].values_[j]) * factor - delta[n - 1][j]) * gamma[n][j];
	}

	if (use_slopes == false) {
		__d[n] = delta[n];
	} else {
		__d[n].Set(end_slope->value_);
	}

	for (i = n - 1; i >= 0; i--) {
		if (i != 0) {
			for (j = 0; j < num_values_per_point_; j++) {
				__d[i][j] = delta[i][j] - gamma[i][j] * __d[i + 1][j];
			}
		} else {
			if (use_slopes == false) {
				for (j = 0; j < num_values_per_point_; j++) {
					__d[i][j] = delta[i][j] - gamma[i][j] * __d[i + 1][j];
				}
			} else {
				__d[i].Set(start_slope->value_);
			}
		}
	}

	float ref_t = GetMeanTDiff();
	for (i = 0; i < n; i++) {
		float scale_factor = (point_[i + 1].t_ - point_[i].t_) / ref_t;

		for (j = 0; j < num_values_per_point_; j++) {
			segment_[i].a.Set(point_[i].values_.value_);
			segment_[i].b[j] = __d[i][j] * scale_factor;
			segment_[i].c[j] = (3.0f * (point_[i + 1].values_[j] - point_[i].values_[j]) -
								   2.0f * __d[i][j] * scale_factor - __d[i + 1][j] * scale_factor);
			segment_[i].d[j] = (2.0f * (point_[i].values_[j] - point_[i + 1].values_[j]) +
								   __d[i][j] * scale_factor + __d[i + 1][j] * scale_factor);
		}
	}

	delete[] gamma;
	delete[] delta;
	delete[] __d;
}

void CubicSpline::InitNaturalCubicClosed() {
	InitModValues();

	int n = num_points_ - 1;

	Values* w = new Values[num_points_];
	Values* v = new Values[num_points_];
	Values* __y = new Values[num_points_];
	Values* __d = new Values[num_points_];
	Values __z(num_values_per_point_);
	Values __h(num_values_per_point_);
	Values g(num_values_per_point_);
	Values __f(num_values_per_point_);
	int i, j;

	for (i = 0; i < num_points_; i++) {
		w[i].Init(num_values_per_point_);
		v[i].Init(num_values_per_point_);
		__y[i].Init(num_values_per_point_);
		__d[i].Init(num_values_per_point_);
	}

	w[1] = v[1] = __z = 0.25f;

	for (j = 0; j < num_values_per_point_; j++) {
		__y[0][j] = __z[j] * 3.0f * (point_[1].values_[j] - point_[n].values_[j]);
		__f[j] = 3.0f * (point_[0].values_[j] - point_[n - 1].values_[j]);
	}

	__h = 4.0f;
	g = 1.0f;

	for (i = 1; i < n; i++) {
		for (j = 0; j < num_values_per_point_; j++) {
			v[i + 1][j] = __z[j] = 1.0f / (4.0f - v[i][j]);
			w[i + 1][j] = -__z[j] * w[i][j];
			__y[i][j] = __z[j] * (3.0f * (point_[i + 1].values_[j] - point_[i - 1].values_[j]) - __y[i - 1][j]);
			__h[j] -= g[j] * w[i][j];
			__f[j] -= g[j] * __y[i - 1][j];
			g[j] *= -v[i][j];
		}
	}

	for (j = 0; j < num_values_per_point_; j++) {
		__h[j] -= (g[j] + 1) * (v[n][j] + w[n][j]);
		__y[n][j] = __f[j] - (g[j] + 1) * __y[n - 1][j];

		__d[n][j]     = __y[n][j] / __h[j];
		__d[n - 1][j] = __y[n - 1][j] - (v[n][j] + w[n][j]) * __d[n][j];
	}

	for (i = n - 2; i >= 0; i--) {
		for (j = 0; j < num_values_per_point_; j++) {
			__d[i][j] = __y[i][j] - v[i + 1][j] * __d[i + 1][j] - w[i + 1][j] * __d[n][j];
		}
	}

	float ref_t = GetMeanTDiff();
	float scale_factor;
	for (i = 0; i < n; i++) {
		scale_factor = (point_[i + 1].t_ - point_[i].t_) / ref_t;
		segment_[i].a.Set(point_[i].values_.value_);

		for (j = 0; j < num_values_per_point_; j++) {
			segment_[i].b[j] = __d[i][j] * scale_factor;
			segment_[i].c[j] = (3.0f * (point_[i + 1].values_[j] - point_[i].values_[j]) -
									2.0f * __d[i][j] * scale_factor - __d[i + 1][j] * scale_factor);
			segment_[i].d[j] = (2.0f * (point_[i].values_[j] - point_[i + 1].values_[j]) +
									__d[i][j] * scale_factor + __d[i + 1][j] * scale_factor);
		}
	}

	scale_factor = (extra_time_tag_ - point_[n].t_) / ref_t;

	for (i = 0; i < num_values_per_point_; i++) {
		segment_[n].a[i] = point_[n].values_[i];
		segment_[n].b[i] = __d[n][i] * scale_factor;

		if (modular_value_[i] == false) {
			segment_[n].c[i] = (3.0f * (point_[0].values_[i] - point_[n].values_[i]) -
									   2.0f * __d[n][i] * scale_factor - __d[0][i] * scale_factor);
			segment_[n].d[i] = (2.0f * (point_[n].values_[i] - point_[0].values_[i]) +
									   __d[n][i] * scale_factor + __d[0][i] * scale_factor);
		} else {
			float _value = GetClosestEquivalentModValue(point_[n].values_[i],
														  point_[0].values_[i],
														  min_mod_value_[i],
														  max_mod_value_[i]);
			segment_[n].c[i] = (3.0f * (_value - point_[n].values_[i]) -
									   2.0f * __d[n][i] * scale_factor - __d[0][i] * scale_factor);
			segment_[n].d[i] = (2.0f * (point_[n].values_[i] - _value) +
									   __d[n][i] * scale_factor + __d[0][i] * scale_factor);
/*
			float new_value;
			if (OptimizeLastSegment(i, new_value) == true) {
				segment_[n].c[i] = (3.0f * (new_value - point_[n].values_[i]) -
										   2.0f * __d[n][i] * scale_factor - __d[0][i] * scale_factor);
				segment_[n].d[i] = (2.0f * (point_[n].values_[i] - new_value) +
										   __d[n][i] * scale_factor + __d[0][i] * scale_factor);
			}
*/
		}
	}

	delete[] w;
	delete[] v;
	delete[] __y;
	delete[] __d;
}

float CubicSpline::GetValue(int value_index) {
	// current_point_ is also the current segment.
	float __y = segment_[current_point_].GetValue(value_index, current_segment_time_);

	if (modular_value_[value_index] == true) {
		return Math::Mod(__y, min_mod_value_[value_index], max_mod_value_[value_index]);
	} else {
		return __y;
	}
}

float CubicSpline::GetSlope(int value_index) {
	return segment_[current_point_].GetSlope(value_index, current_segment_time_);
}

float CubicSpline::GetSlopeChange(int value_index) {
	return segment_[current_point_].GetSlopeChange(value_index, current_segment_time_);
}

float CubicSpline::GetClosestEquivalentModValue(float previous_value,
											  float current_value,
											  float min_mod,
											  float max_mod) {
	float prev  = Math::Mod(previous_value, min_mod, max_mod);
	float _value = Math::Mod(current_value, min_mod, max_mod);

	float down = _value - prev;
	float up   = (prev - min_mod) + (max_mod - _value);

	if (_value < prev)
		up = -((_value - min_mod) + (max_mod - prev));

	if (fabs(down) < fabs(up))
		return (previous_value + down);
	else
		return (previous_value - up);
}
/*
void CubicSpline::CalcABCD() {
	float dx;
	float lDT;

	if (current_point_ == num_points_ - 1) {
		dx = extra_time_tag_ - point_[current_point_].t_;
		lDT = extra_time_tag_ - current_time_;
	} else {
		dx = point_[current_point_ + 1].t_ - point_[current_point_].t_;
		lDT = point_[current_point_ + 1].t_ - current_time_;
	}

	float lDX2 = dx * dx;

	a = lDT / dx;
	b = 1.0f - a;
	c = (a * a * a - a) * lDX2 * (1.0f / 6.0f);
	d = (b * b * b - b) * lDX2 * (1.0f / 6.0f);
}
*/

float CubicSpline::GetMeanTDiff() {
	int n = num_points_ - 1;

	float mean_t_diff = 0.0f;
	for (int i = 0; i < n; i++) {
		mean_t_diff += point_[i + 1].t_ - point_[i].t_;
	}

	mean_t_diff /= (float)n;

	return mean_t_diff;
}

void CubicSpline::InitModValues() {
	for (int j = 0; j < num_values_per_point_; j++) {
		if (modular_value_[j] == true) {
			float prev_value = 0.0f;

			for (int i = 0; i < num_points_; i++) {
				point_[i].values_[j] = GetClosestEquivalentModValue(prev_value,
																	   point_[i].values_[j],
																	   min_mod_value_[j],
																	   max_mod_value_[j]);
				prev_value = point_[i].values_[j];
			}
		}
	}
}

bool CubicSpline::OptimizeLastSegment(int value_index, float& value) {
	// This function is only used in combination with modular values and
	// closed cubic splines. It checks wether the last segment (the one
	// that connects the last point with the first one) is a detour and can
	// take a shorter path. This test is done by finding the value where
	// the segment's derivative is zero, and by checking if this value is
	// closer to the destination than the segment's starting value.

	Segment* seg = &segment_[num_points_ - 1];

	int i = value_index;

	if (modular_value_[i] == true) {
		float p = (seg->c[i] / seg->d[i]) * (2.0f / 3.0f);
		float q = (seg->b[i] / seg->d[i]) * (1.0f / 3.0f);
		float s = (p * p * 0.25f) - q;

		if (s > 0.0f) {
			float x1 = -p * 0.5f + sqrt(s);
			float x2 = -p * 0.5f - sqrt(s);
			float __x = x1;	// TODO: Assign correct data. Assignment missing prior to 2006-10-19.
			bool valid_x = false;

			if (x1 >= 0.0f && x1 <= 1.0f) {
				__x = x1;
				valid_x = true;

				if (x2 >= 0.0f && x2 <= 1.0f && x2 < x1) {
					__x = x2;
				}
			} else if(x2 >= 0.0f && x2 <= 1.0f) {
				__x = x2;
				valid_x = true;
			}

			if (valid_x == true) {
				float _value = seg->GetValue(i, __x);
				value = GetClosestEquivalentModValue(_value,
														point_[0].values_[i],
														min_mod_value_[i],
														max_mod_value_[i]);
				return true;
			}
		}
	}

	return false;
}



}
