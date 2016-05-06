
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "math.h"



#define LEPRA_ARRAY_COUNT(a)	(sizeof(a)/sizeof(a[0]))
#define LEPRA_ARRAY_ASSIGN(a,v)					\
{								\
	for (size_t x = 0; x < LEPRA_ARRAY_COUNT(a); ++x)	\
	{							\
		a[x] = v[x];					\
	}							\
}
#define LEPRA_INSTANCE_CLEAR(ptr,end)		::memset(ptr, 0, (size_t)(((char*)(end))-((char*)(ptr))))
#define LEPRA_INSTANCE_COPY(ptr,end,from)	::memcpy(ptr, from, (size_t)(((char*)(end))-((char*)(ptr))))



namespace lepra {



template<class _T, int _Count> class CyclicArray {
public:
	CyclicArray();

	void Clear();

	int GetCount() const;

	void PushFront(_T& element);
	void PushBack(_T& element);
	void PopFront();
	void PopBack();

	// Returns the element at the relative index.
	const _T& operator[](unsigned index) const;

private:
	int first_element_;
	int element_count_;
	_T array_[_Count];
};



template<class _T, int _Count>
CyclicArray<_T, _Count>::CyclicArray():
	first_element_(0),
	element_count_(0) {
}

template<class _T, int _Count>
void CyclicArray<_T, _Count>::Clear() {
	element_count_ = 0;
}

template<class _T, int _Count>
int CyclicArray<_T, _Count>::GetCount() const {
	return element_count_;
}

template<class _T, int _Count>
void CyclicArray<_T, _Count>::PushFront(_T& element) {
	first_element_ = Math::Mod(first_element_ - 1, _Count);
	array_[first_element_] = element;

	if (element_count_ < _Count) {
		++element_count_;
	}
}

template<class _T, int _Count>
void CyclicArray<_T, _Count>::PushBack(_T& element) {
	array_[(first_element_ + element_count_) % _Count] = element;

	if (element_count_ < _Count) {
		++element_count_;
	} else {
		// We have overwritten the first element. Point to the next one.
		first_element_ = (first_element_ + 1) % _Count;
	}
}

template<class _T, int _Count>
void CyclicArray<_T, _Count>::PopFront() {
	if (element_count_ > 0) {
		first_element_ = Math::Mod(first_element_ + 1, _Count);
		--element_count_;
	}
}

template<class _T, int _Count>
void CyclicArray<_T, _Count>::PopBack() {
	if (element_count_ > 0) {
		--element_count_;
	}
}

template<class _T, int _Count>
const _T& CyclicArray<_T, _Count>::operator[](unsigned index) const {
	return array_[(first_element_ + index) % _Count];
}



}
