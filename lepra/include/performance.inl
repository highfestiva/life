
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



namespace lepra {



template<class _Data> SequencialPerformanceData<_Data>::SequencialPerformanceData() {
	Clear();
}

template<class _Data> void SequencialPerformanceData<_Data>::Clear() {
	Set(0, 0, 0, 0);
}

template<class _Data> void SequencialPerformanceData<_Data>::Set(double minimum, double value, double maximum, _Data previous_value) {
	previous_value_ = previous_value;
	Parent::Set(minimum, value, maximum);
}

template<class _Data> void SequencialPerformanceData<_Data>::Append(double time, double start_time, _Data value) {
	deb_assert (time >= 0);
	if (time > 0) {
		double period_value = (value-previous_value_)/time;
		previous_value_ = value;
		Parent::Append(period_value, start_time);
	}
}



template<class _T> BasicScopeTimer<_T>::BasicScopeTimer():
	time_(false) {
}

template<class _T> BasicScopeTimer<_T>::BasicScopeTimer(_T* data):
	time_(false),
	data_(data) {
}

template<class _T> BasicScopeTimer<_T>::~BasicScopeTimer() {
	const double start = time_.GetTime();
	const double delta = time_.PopTimeDiff();
	data_->Append(delta, start);
	data_ = 0;
}

template<class _T> void BasicScopeTimer<_T>::Attach(_T* data) {
	data_ = data;
}



}
