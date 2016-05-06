
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



template<class T, class TimeType, class TBase>
CubicDeCasteljauSpline<T, TimeType, TBase>::CubicDeCasteljauSpline(T* key_frames,
	TimeType* time_tags, int count, SplineType spline_type, DataPolicy policy) {
	Set(key_frames, time_tags, count, spline_type, policy);
}

template<class T, class TimeType, class TBase>
CubicDeCasteljauSpline<T, TimeType, TBase>::CubicDeCasteljauSpline(const CubicDeCasteljauSpline& original, DataPolicy policy) {
	Set(original.key_frames_, original.time_tags_, original.count_, original.spline_type_, policy);
}

template<class T, class TimeType, class TBase>
CubicDeCasteljauSpline<T, TimeType, TBase>::~CubicDeCasteljauSpline() {
	if (policy_ != kCopyReference) {
		delete[] key_frames_;
		delete[] time_tags_;
	}

	if (temp_buffer_ != 0) {
		delete[] temp_buffer_;
	}
}

template<class T, class TimeType, class TBase>
void CubicDeCasteljauSpline<T, TimeType, TBase>::Set(T* key_frames,
	TimeType* time_tags, int count, SplineType spline_type, DataPolicy policy) {
	key_frames_ = key_frames;
	temp_buffer_ = 0;
	time_tags_ = time_tags;
	count_ = count;
	spline_type_ = spline_type;
	policy_ = policy;
	current_time_ = 0;
	enable_modulo_ = true;

	if (policy_ == kFullCopy) {
		key_frames_ = new T[count_];
		time_tags_ = new TimeType[count_ + 1];

		::memcpy(key_frames_, key_frames, count_ * sizeof(T));
		::memcpy(time_tags_, time_tags, (count_ + 1) * sizeof(TimeType));
	}

	if (spline_type_ == kTypeBezier) {
		temp_buffer_ = new T[count_];
	}

	// Check if valid.
	/*for (int i = 1; i <= count_; i++) {
		deb_assert(time_tags_[i - 1] < time_tags_[i]);
	}*/
}

template<class T, class TimeType, class TBase>
void CubicDeCasteljauSpline<T, TimeType, TBase>::EnableModulo(bool enable) {
	enable_modulo_ = enable;
}

template<class T, class TimeType, class TBase>
void CubicDeCasteljauSpline<T, TimeType, TBase>::StartInterpolation(TimeType time) {
	time = Math::Mod(time, time_tags_[0], time_tags_[count_]);

	int _count = count_ + 1;
	current_frame_[1] = FindFrameAtTime(time, 0);
	current_frame_[0] = Math::Mod(current_frame_[1] - 1, _count);
	current_frame_[2] = Math::Mod(current_frame_[1] + 1, _count);
	current_frame_[3] = Math::Mod(current_frame_[2] + 1, _count);

	current_time_ = time;
}

template<class T, class TimeType, class TBase>
void CubicDeCasteljauSpline<T, TimeType, TBase>::GotoAbsoluteTime(TimeType time) {
	TimeType relative_time = time-GetCurrentInterpolationTime();
	StepInterpolation(relative_time);
}

template<class T, class TimeType, class TBase>
void CubicDeCasteljauSpline<T, TimeType, TBase>::StepInterpolation(TimeType time_step) {
	TimeType prev_time = current_time_;
	current_time_ += time_step;
	/*if (current_time_ < GetMinimumTime()) {
		current_time_ = GetMinimumTime();
	} else if (current_time_ > GetMaximumTime()) {
		current_time_ = GetMaximumTime();
	}*/
	if (enable_modulo_) {
		const TimeType modulo = Math::Mod(current_time_, time_tags_[0], time_tags_[count_]);
		current_time_ = modulo;
	} else {
		if (current_time_ < time_tags_[0]) {
			current_time_ = time_tags_[0];
		} else if (current_time_ > time_tags_[count_]) {
			current_time_ = time_tags_[count_];
		}
	}

	if (current_time_ < prev_time && time_step > 0) {	// Note: don't change. Checks for "looped" condition (as opposed to "backed").
		// We have moved looped. Search from the start.
		current_frame_[1] = FindFrameAtTime(current_time_, 0);
	} else {
		current_frame_[1] = FindFrameAtTime(current_time_, current_frame_[1]);
	}

	current_frame_[0] = Math::Mod(current_frame_[1] - 1, count_);
	current_frame_[2] = Math::Mod(current_frame_[1] + 1, count_);
	current_frame_[3] = Math::Mod(current_frame_[2] + 1, count_);
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::GetCurrentInterpolationTime() const {
	return current_time_;
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::FindNearestTime(TimeType step_length, const T& where,
	TBase& nearest_distance, T& nearest_point, int steps) {
	// Ugly search algorithm. Looks in the step direction until the distance becomes greater than
	// the starting distance. Then turns around and looks in the other direction at half the velocity.
	// And so forth. This could probably be optimized using Newton-Raphson, or (worst-case) Runge-Cutta.
	TimeType nearest_time = current_time_;
	T _nearest_distance = GetValue()-where;
	typename T::BaseType distance2 = _nearest_distance.GetLengthSquared();
	T new_nearest_distance;
	for (int y = 0; y < steps; ++y) {
		const int zc = 8;
		int z;
		for (z = 0; z < zc; ++z) {
			StepInterpolation(step_length);
			new_nearest_distance = GetValue()-where;
			typename T::BaseType new_distance2 = new_nearest_distance.GetLengthSquared();
			if (new_distance2 >= distance2) {
				StepInterpolation(-step_length);	// Go back to previous point.
				step_length = step_length*-0.5f;	// Change search direction; increase granularity.
				if ((current_time_ == GetMinimumTime() && step_length < 0) ||
					(current_time_ == GetMaximumTime() && step_length > 0)) {
					step_length = -step_length;
				}
			} else {
				distance2 = new_distance2;
				_nearest_distance = new_nearest_distance;
				nearest_time = current_time_;
				break;
			}
		}
		if (z == zc) {	// Nothing new found in a few loops - might as well give up.
			break;
		}
		step_length *= 0.7f;	// Increase granularity when we've found a closer spot.
	}

	// Fill in our output values.
	nearest_distance = ::sqrt(distance2);
	nearest_point = _nearest_distance+where;
	return (nearest_time);
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetValue() const {
	switch (spline_type_) {
		case kTypeBezier:	return (GetBezierValue());	// TRICKY: returns as an optimization.
		case kTypeCatmullrom:	return (GetCatmullRomValue());	// TRICKY: returns as an optimization.
		default:		return (GetBSplineValue());	// TRICKY: returns as an optimization.
	}
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetBezierValue() const {
	TimeType t = (current_time_ - time_tags_[0]) / (time_tags_[count_] - time_tags_[0]);

	int i;
	int j;

	for (i = 0; i < count_; i++) {
		temp_buffer_[i] = key_frames_[i];
	}

	for (j = count_ - 1; j > 0; j--) {
		for (i = 0; i < j; i++) {
			temp_buffer_[i] = Lerp(temp_buffer_[i], temp_buffer_[i + 1], t);
		}
	}

	return temp_buffer_[0];
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetCatmullRomValue() const {
	TimeType t = GetCurrentSegmentTime();
	return GetValue(t + (TimeType)1.0, t, t - (TimeType)1.0, (t + (TimeType)1.0) * (TimeType)0.5, t * (TimeType)0.5, t);
/*	T& k1 = key_frames_[current_frame_[0]];
	T& k2 = key_frames_[current_frame_[1]];
	T& k3 = key_frames_[current_frame_[2]];
	T& k4 = key_frames_[current_frame_[3]];

	// Use locally defined Lerp to avoid problems with VS-compilers that doesn't support
	// template functions.
	T l12 = Lerp(k1, k2, t + (TimeType)1.0);
	T l23 = Lerp(k2, k3, t);
	T l34 = Lerp(k3, k4, t - (TimeType)1.0);
	return Lerp(Lerp(l12, l23, (t + (TimeType)1.0) * (TimeType)0.5), Lerp(l23, l34, t * (TimeType)0.5), t);
*/
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetBSplineValue() const {
	TimeType t = GetCurrentSegmentTime();
	return GetValue((t + (TimeType)2.0) / (TimeType)3.0, (t + (TimeType)1.0) / (TimeType)3.0, t / (TimeType)3.0, (t + (TimeType)1.0) * (TimeType)0.5, t * (TimeType)0.5, t);
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetSlope() {
	// TODO: fixme! This is just a slow and simple approximation.
	const TimeType delta = (GetMaximumTime()-GetMinimumTime())*(TimeType)0.001;
	StepInterpolation(-delta);
	const T value1 = GetValue();
	StepInterpolation(+delta*2);
	const T value2 = GetValue();
	const T slope = value2-value1;
	return (slope);
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetValue(TimeType t12, TimeType t23, TimeType t34,
				     TimeType t123, TimeType t234,
				     TimeType t) const {
	T& k1 = key_frames_[current_frame_[0]];
	T& k2 = key_frames_[current_frame_[1]];
	T& k3 = key_frames_[current_frame_[2]];
	T& k4 = key_frames_[current_frame_[3]];

	// Use locally defined Lerp to avoid problems with VS-compilers that doesn't support
	// template functions.
	T l12 = Lerp(k1, k2, t12);
	T l23 = Lerp(k2, k3, t23);
	T l34 = Lerp(k3, k4, t34);
	return Lerp(Lerp(l12, l23, t123), Lerp(l23, l34, t234), t);
}

template<class T, class TimeType, class TBase>
int CubicDeCasteljauSpline<T, TimeType, TBase>::FindFrameAtTime(TimeType time, int start_frame) const {
	deb_assert(time >= time_tags_[0] && time <= time_tags_[count_]);

	int current = Math::Mod(start_frame, count_ + 1);
	int next = Math::Mod(current + 1, count_ + 1);

	// Loop forward.
	while(time_tags_[current] < time && time_tags_[next] < time) {
		current = next;
		next = Math::Mod(current + 1, count_ + 1);
	}

	// Loop backward.
	while(time_tags_[current] > time && time_tags_[next] > time) {
		next = current;
		current = Math::Mod(next - 1, count_ + 1);
	}

	return current;
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::Lerp(const T& a, const T& b, TimeType t) const {
	return (a + (b-a)*t);
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::GetMinimumTime() const {
	return (time_tags_[0]);
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::GetMaximumTime() const {
	return (time_tags_[count_]);
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::GetCurrentSegmentTime() const {
	return (current_time_ - time_tags_[current_frame_[1]]) / (time_tags_[current_frame_[2]] - time_tags_[current_frame_[1]]);
}
