
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// JB 2008-07-15: totally rewrote from scratch. Reduced the number of lines by 76 %!
//
// A class that manages Id numbers, to be able to recycle used Ids.
// Keeps the Id number spans as defragmented as possible at all times.
// Avoid using this class in time critical tasks.



#pragma once



#include <list>
#include "String.h"



#define ID_TEMPLATE template<class _TInt>
#define ID_QUAL IdManager<_TInt>



namespace Lepra
{



ID_TEMPLATE
class IdManager
{
public:
	// pMinId and pMaxId defines the valid Id-range [pMinId, pMaxId].
	// pInvalidId mustn't be a part of this range. Thus, the expression:
	// "pInvalidId < pMinId or pInvalidId > pMaxId"
	// must be true.
	IdManager(_TInt pMinId, _TInt pMaxId, _TInt pInvalidId);

	inline _TInt GetMinId();
	inline _TInt GetMaxId();
	inline _TInt GetInvalidId();

	bool IsIdFree(_TInt pId) const;
	_TInt GetFreeId();
	bool ReserveId(_TInt pId);
	bool RecycleId(_TInt pId);

	str GetDebugState() const;

private:
	struct AllocFreeSegment
	{
		AllocFreeSegment(_TInt pFirstAlloc, _TInt pFirstFree, _TInt pLastFree):
			mFirstAllocId(pFirstAlloc),
			mFirstFreeId(pFirstFree),
			mLastFreeId(pLastFree)
		{
		}
		_TInt mFirstAllocId;
		_TInt mFirstFreeId;
		_TInt mLastFreeId;
	};

	typedef std::list<AllocFreeSegment> IdSegmentList;

	void AllocateIdOnSegment(_TInt pId, const typename IdSegmentList::iterator& x);
	void FreeIdOnSegment(_TInt pId, const typename IdSegmentList::iterator& x);

	IdSegmentList mIdSegmentList;

	_TInt mInvalidId;
};



}



#include "IdManager.inl"



#undef ID_TEMPLATE
#undef ID_QUAL
