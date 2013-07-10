
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Math.h"



#define LEPRA_ARRAY_COUNT(a)	(sizeof(a)/sizeof(a[0]))
#define LEPRA_ARRAY_ASSIGN(a,v)					\
{								\
	for (size_t x = 0; x < LEPRA_ARRAY_COUNT(a); ++x)	\
	{							\
		a[x] = v[x];					\
	}							\
}



namespace Lepra
{



template<class _T, int _Count> class CyclicArray
{
public:
	CyclicArray();

	void Clear();

	int GetCount() const;

	void PushFront(_T& pElement);
	void PushBack(_T& pElement);
	void PopFront();
	void PopBack();

	// Returns the element at the relative index.
	const _T& operator[](unsigned pIndex) const;

private:
	int mFirstElement;
	int mElementCount;
	_T mArray[_Count];
};



template<class _T, int _Count> 
CyclicArray<_T, _Count>::CyclicArray():
	mFirstElement(0),
	mElementCount(0)
{
}

template<class _T, int _Count> 
void CyclicArray<_T, _Count>::Clear()
{
	mElementCount = 0;
}

template<class _T, int _Count> 
int CyclicArray<_T, _Count>::GetCount() const
{
	return mElementCount;
}

template<class _T, int _Count> 
void CyclicArray<_T, _Count>::PushFront(_T& pElement)
{
	mFirstElement = Math::Mod(mFirstElement - 1, _Count);
	mArray[mFirstElement] = pElement;

	if (mElementCount < _Count)
	{
		++mElementCount;
	}
}

template<class _T, int _Count> 
void CyclicArray<_T, _Count>::PushBack(_T& pElement)
{
	mArray[(mFirstElement + mElementCount) % _Count] = pElement;

	if (mElementCount < _Count)
	{
		++mElementCount;
	}
	else
	{
		// We have overwritten the first element. Point to the next one.
		mFirstElement = (mFirstElement + 1) % _Count;
	}
}

template<class _T, int _Count> 
void CyclicArray<_T, _Count>::PopFront()
{
	if (mElementCount > 0)
	{
		mFirstElement = Math::Mod(mFirstElement + 1, _Count);
		--mElementCount;
	}
}

template<class _T, int _Count> 
void CyclicArray<_T, _Count>::PopBack()
{
	if (mElementCount > 0)
	{
		--mElementCount;
	}
}

template<class _T, int _Count> 
const _T& CyclicArray<_T, _Count>::operator[](unsigned pIndex) const
{
	return mArray[(mFirstElement + pIndex) % _Count];
}



}
