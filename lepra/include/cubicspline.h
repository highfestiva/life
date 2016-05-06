/*
	Class:  CubicSpline
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	CubicSpline is using the natural cubic spline algorithm. It was hard to
	find any good information about it on the internet, and I actually don't
	fully understand the math and the algorithms used. The original algorithm
	only supported equally spaced points, and I managed to change it to support
	unequal spaces by scaling the derivatives in InitNaturalCubic() and in
	InitNaturalCubicClosed().

	HOW TO USE THIS CLASS:

	Start by calling Init() to allocate all needed memory and such.
	Then setup each point in ascending time order by calling SetPointValues().
	Each point can take several values to interpolate.

	After setting all points, you can start interpolating the spline
	by calling StartInterpolation(), and step by calling StepInterpolation().
	When the interpolation has started, you can retrieve all interpolated
	values by calling GetValue() passing the appropriate value index.
*/

#pragma once

#include "lepratypes.h"

namespace lepra {

class CubicSpline {
public:

	inline CubicSpline();
	inline ~CubicSpline();

	// num_points = number of control points.
	// num_values_per_point = number of values to interpolate per point.
	void Init(int num_points, int num_values_per_point);

	// The points in the spline must be sorted in ascending time order.
	// If the time tag given is less than or equal to the previous point's
	// time tag, the point will not be updated, and the function will return
	// false.
	bool SetPointValues(int point_index, float time_tag, const float* value);

	// Defines a "modular" value (a value which will be wrapped around whithin
	// the range defined by pMinModValue and pMaxModValue).
	// This works perfectly with open splines, but not as well with closed (looped)
	// splines (but it works ok anyway).
	void SetModValue(int value_index, float min_mod_value, float max_mod_value);

	inline float GetMinTime() const;
	inline float GetMaxTime() const;

	// Returns time t such that 0 <= t <= 1, where t = 0 has PointIndex = x, and t = 1
	// PointIndex = x + 1.
	inline float GetCurrentSegmentTime();

	// Initializes spline interpolation at time time.
	void StartInterpolation(float time);
	void StartInterpolation(float time, const float* start_slope, const float* end_slope);
	void StartLoopedInterpolation(float time, float extra_time_tag);
	void StepInterpolation(float delta_time);
	void GotoAbsoluteTime(float time);
	float FindNearestTime(float step_length, const float* where, float& nearest_distance, float* nearest_point);

	float GetValue(int value_index);	// y
	float GetSlope(int value_index);	// y'
	float GetSlopeChange(int value_index);	// y''

	inline int GetNumPoints();
	inline int GetNumValuesPerPoint();

protected:
private:
	enum InterpolationMode {
		kImOnce = 0,
		kImLoop
	};

	// This class provides the interface to treat an array of float values
	// as if they were only one value.
	class Values {
	public:

		inline Values() :
			value_(0) {
		}

		inline Values(int num_values) :
			value_(0) {
			Init(num_values);
		}

		inline Values(const Values& values) :
			value_(0) {
			Init(values.num_values_);
			Set(values.value_);
		}

		inline ~Values() {
			if (value_ != 0)
				delete[] value_;
		}

		inline void Init(int num_values) {
			if (value_ != 0)
				delete[] value_;

			value_    = new float[num_values];

			for (int i = 0; i < num_values; i++) {
				value_[i] = 0;
			}

			num_values_ = num_values;
		}

		inline void Set(const float* value) {
			for (int i = 0; i < num_values_; i++) {
				value_[i] = value[i];
			}
		}

		inline void Set(float value) {
			for (int i = 0; i < num_values_; i++) {
				value_[i] = value;
			}
		}

		inline float& operator[] (int index) {
			return value_[index];
		}

		inline Values operator=  (const Values& values) { Set(values.value_); return *this; }
		inline Values operator=  (float value) { Set(value); return *this; }

		inline Values operator+= (const Values& values) {	for (int i = 0; i < num_values_; i++) value_[i] += values.value_[i]; return *this; }
		inline Values operator-= (const Values& values) {	for (int i = 0; i < num_values_; i++) value_[i] -= values.value_[i]; return *this; }
		inline Values operator*= (const Values& values) {	for (int i = 0; i < num_values_; i++) value_[i] *= values.value_[i]; return *this; }
		inline Values operator/= (const Values& values) {	for (int i = 0; i < num_values_; i++) value_[i] /= values.value_[i]; return *this; }

		inline Values operator-  () const {
			Values temp(*this);
			temp *= -1.0f;
			return temp;
		}

		inline Values operator+  (const Values& values) const {
			Values temp(*this);
			return (temp += values);
		}
		inline Values operator-  (const Values& values) const {
			Values temp(*this);
			return (temp -= values);
		}
		inline Values operator*  (const Values& values) const {
			Values temp(*this);
			return (temp *= values);
		}
		inline Values operator/  (const Values& values) const {
			Values temp(*this);
			return (temp /= values);
		}

		inline Values operator+= (float value) { for(int i = 0; i < num_values_; i++) value_[i] += value; return *this; }
		inline Values operator-= (float value) { for(int i = 0; i < num_values_; i++) value_[i] -= value; return *this; }
		inline Values operator*= (float value) { for(int i = 0; i < num_values_; i++) value_[i] *= value; return *this; }
		inline Values operator/= (float value) { for(int i = 0; i < num_values_; i++) value_[i] /= value; return *this; }

		inline Values operator+  (float value) const {
			Values temp(*this);
			return (temp += value);
		}
		inline Values operator-  (float value) const {
			Values temp(*this);
			return (temp -= value);
		}
		inline Values operator*  (float value) const {
			Values temp(*this);
			return (temp *= value);
		}
		inline Values operator/  (float value) const {
			Values temp(*this);
			return (temp /= value);
		}

		inline friend Values operator+ (float value, const Values& values) {
			return (values + value);
		}

		inline friend Values operator- (float value, const Values& values) {
			Values temp(values.num_values_);
			for (int i = 0; i < values.num_values_; i++) {
				temp.value_[i] = value - values.value_[i];
			}
			return temp;
		}

		inline friend Values operator* (float value, const Values& values) {
			return (values * value);
		}

		inline friend Values operator/ (float value, const Values& values) {
			Values temp(values.num_values_);
			for (int i = 0; i < values.num_values_; i++) {
				temp.value_[i] = value / values.value_[i];
			}
			return temp;
		}

		int num_values_;
		float* value_;
	};




	class Point {
	public:

		inline Point() :
			t_(0) {
		}

		inline void Init(int num_values) {
			values_.Init(num_values);
		}

		float t_;			// Time tag, or x-coordinate.
		Values values_;	// Y-coordinates.
	};




	// Cubic spline segment.
	class Segment {
	public:

		inline void Init(int num_values) {
			a.Init(num_values);
			b.Init(num_values);
			c.Init(num_values);
			d.Init(num_values);
		}

		// 0 <= t <= 1 within the segment.
		inline float GetValue(int value_index, float t) {
			int i = value_index;
			return ((((d[i] * t) + c[i]) * t + b[i]) * t) + a[i];
		}

		inline float GetSlope(int value_index, float t) {
			int i = value_index;
			return 3.0f * d[i] * t * t + 2.0f * c[i] * t + b[i];
		}

		inline float GetSlopeChange(int value_index, float t) {
			int i = value_index;
			return 6.0f * d[i] * t + 2.0f * c[i];
		}

		Values a;
		Values b;
		Values c;
		Values d;
	};

	void StartInterpolation(float time, InterpolationMode mode, Values* start_slope, Values* end_slope);
	void ReInitialize(InterpolationMode mode, Values* start_slope, Values* end_slope);
	void StepForwardToCurrentFrame();

	void InitNaturalCubic(Values* start_slope, Values* end_slope, bool use_slopes);
	void InitNaturalCubicClosed();

	void InitModValues();
	bool OptimizeLastSegment(int value_index, float& value);

//	void CalcABCD();

	float GetClosestEquivalentModValue(float previous_value,
									   float current_value,
									   float min_mod,
									   float max_mod);

	float GetMeanTDiff();

	// Spline data.
	int num_points_;
	int num_values_per_point_;
	Point* point_;
	Segment* segment_;

	bool initialized_;
	InterpolationMode mode_;

	// Modular values.
	bool* modular_value_;
	float* min_mod_value_;
	float* max_mod_value_;

	// Interpolation data.
	int current_point_;
	float current_time_;
	float current_segment_time_;
	float extra_time_tag_;
};

CubicSpline::CubicSpline() :
	num_points_(0),
	num_values_per_point_(0),
	point_(0),
	segment_(0),
	initialized_(false),
	mode_(kImOnce),
	modular_value_(0),
	min_mod_value_(0),
	max_mod_value_(0),
	current_point_(0),
	current_time_(0),
	current_segment_time_(0),
	extra_time_tag_(0) {
}

CubicSpline::~CubicSpline() {
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
}

float CubicSpline::GetMinTime() const {
	return point_[0].t_;
}

float CubicSpline::GetMaxTime() const {
	return point_[num_points_ - 1].t_;
}

float CubicSpline::GetCurrentSegmentTime() {
	return current_segment_time_;
}

int CubicSpline::GetNumPoints() {
	return num_points_;
}

int CubicSpline::GetNumValuesPerPoint() {
	return num_values_per_point_;
}

}
