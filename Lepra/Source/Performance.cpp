
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/math.h"
#include "../include/performance.h"



namespace lepra {



PerformanceData::PerformanceData() {
	Clear();
}

void PerformanceData::Clear() {
	first_time_ = true;
	time_of_last_measure_ = 0;
	Set(0, 0, 0);
}

void PerformanceData::Set(double minimum, double value, double maximum) {
	ResetHitCount();
	minimum_ = minimum;
	last_ = value;
	sliding_average_ = value;
	maximum_ = maximum;
}

void PerformanceData::Append(double period_value, double time_of_last_measure) {
	++hit_count_;
	time_of_last_measure_ = time_of_last_measure;
	if (first_time_) {
		first_time_ = false;
		Set(period_value, period_value, period_value);
	} else {
		if (period_value < minimum_) {
			minimum_ = period_value;
		} else if (period_value > maximum_) {
			maximum_ = period_value;
		} else {
			// Close the gap between min and max (equalizer style) so the
			// fluctuation has some relevance even after startup.
			const double step = (maximum_-minimum_) * 0.001;
			minimum_ += step;
			maximum_ -= step;
		}

		last_ = period_value;
		sliding_average_ = Math::Lerp(sliding_average_, last_, 0.05);
	}
}

void PerformanceData::ResetHitCount() {
	hit_count_ = 0;
}

int PerformanceData::GetHitCount() const {
	return hit_count_;
}

double PerformanceData::GetTimeOfLastMeasure() const {
	return (time_of_last_measure_);
}

double PerformanceData::GetMinimum() const {
	return (minimum_);
}

double PerformanceData::GetLast() const {
	return (last_);
}

double PerformanceData::GetSlidingAverage() const {
	return (sliding_average_);
}

double PerformanceData::GetMaximum() const {
	return (maximum_);
}

double PerformanceData::GetRangeFactor() const {
	if (minimum_) {
		return ((maximum_-minimum_)/minimum_);
	}
	return (0);
}



ScopePerformanceData* ScopePerformanceData::Insert(const str& name, size_t hash) {
	ScopePerformanceData* _parent = GetActive();
	if (!_parent) {
		ScopePerformanceData* root = new ScopePerformanceData(0, name, hash);
		AddRoot(root);
		SetActive(root);
		return (root);
	}
	// Check if we're root and being reactivated, otherwise it's plain ol' recursion.
	if (_parent->hash_ == hash && _parent->parent_ == 0) {
		SetActive(_parent);
		return (_parent);
	}

	ScopePerformanceData* _node;
	{
		ScopeSpinLock lock(&root_lock_);
		// Find self.
		_node = _parent->FindChild(/*name,*/ hash);
		if (!_node) {
			// Not listed, so beam us up Scotty.
			_node = new ScopePerformanceData(_parent, name, hash);
		} else {
			deb_assert(_node->GetName() == name);
		}
	}
	SetActive(_node);
	return (_node);
}

ScopePerformanceData::ScopePerformanceData(ScopePerformanceData* parent, const str& name, size_t hash):
	name_(name),
	hash_(hash),
	parent_(parent) {
	if (parent_) {
		parent_->child_array_.push_back(this);
	}
}

ScopePerformanceData::~ScopePerformanceData() {
	NodeArray::const_iterator x = child_array_.begin();
	for (; x != child_array_.end(); ++x) {
		delete *x;
	}
	child_array_.clear();
}

void ScopePerformanceData::EraseAll() {
	ScopeSpinLock lock(&root_lock_);
	roots_.clear();
}

void ScopePerformanceData::ResetAll() {
	ScopeSpinLock lock(&root_lock_);
	NodeArray::iterator x = roots_.begin();
	while (x != roots_.end()) {
		if ((*x)->GetMaximum() == 0) {
			delete *x;
			x = roots_.erase(x);
		} else {
			++x;
		}
	}
	ResetAll(roots_);
}

void ScopePerformanceData::Append(double period_value, double time_of_last_measure) {
	Parent::Append(period_value, time_of_last_measure);
	if (parent_) {
		SetActive(parent_);
	}
}

ScopePerformanceData::NodeArray ScopePerformanceData::GetRoots() {
	ScopeSpinLock lock(&root_lock_);
	NodeArray roots_copy(roots_);
	return (roots_copy);
}

const str& ScopePerformanceData::GetName() const {
	return (name_);
}

ScopePerformanceData::NodeArray ScopePerformanceData::GetChildren() const {
	ScopeSpinLock lock(&root_lock_);
	NodeArray children_copy(child_array_);
	return (children_copy);
}

const ScopePerformanceData* ScopePerformanceData::GetChild(const str& name) const {
	ScopeSpinLock lock(&root_lock_);
	NodeArray::const_iterator x = child_array_.begin();
	for (; x != child_array_.end(); ++x) {
		if ((*x)->GetName().find(name) != str::npos) {
			return *x;
		}
	}
	return 0;
}

void ScopePerformanceData::ResetAll(NodeArray& nodes) {
	NodeArray::iterator x = nodes.begin();
	for (; x != nodes.end(); ++x) {
		(*x)->Clear();
		ResetAll((*x)->child_array_);
	}
}

ScopePerformanceData* ScopePerformanceData::FindChild(/*const str& name,*/ size_t hash) const {
	NodeArray::const_iterator x = child_array_.begin();
	for (; x != child_array_.end(); ++x) {
		if ((*x)->hash_ == hash) {
			return (*x);
		}
	}
	return (0);
}

void ScopePerformanceData::AddRoot(ScopePerformanceData* node) {
	ScopeSpinLock lock(&root_lock_);
	roots_.push_back(node);
}

void ScopePerformanceData::SetActive(ScopePerformanceData* node) {
	Thread::SetExtraData(node);
}

ScopePerformanceData* ScopePerformanceData::GetActive() {
	return ((ScopePerformanceData*)Thread::GetExtraData());
}

ScopePerformanceData::ScopePerformanceData(const ScopePerformanceData&):
	hash_(0) {
	deb_assert(false);
}

void ScopePerformanceData::operator=(const ScopePerformanceData&) {
	deb_assert(false);
}

ScopePerformanceData::NodeArray ScopePerformanceData::roots_;
SpinLock ScopePerformanceData::root_lock_;



}
