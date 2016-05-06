
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// JB 2008-07-15: totally rewrote from scratch. Reduced the number of lines by 76 %!
//
// A class that manages Id numbers, to be able to recycle used Ids.
// Keeps the Id number spans as defragmented as possible at all times.
// Avoid using this class in time critical tasks.



#pragma once



#include <list>
#include "string.h"



#define ID_TEMPLATE template<class _TInt>
#define ID_QUAL IdManager<_TInt>



namespace lepra {



ID_TEMPLATE
class IdManager {
public:
	// min_id and max_id defines the valid Id-range [min_id, max_id].
	// invalid_id mustn't be a part of this range. Thus, the expression:
	// "pInvalidId < pMinId or pInvalidId > pMaxId"
	// must be true.
	IdManager(_TInt min_id, _TInt max_id, _TInt invalid_id);

	inline _TInt GetMinId() const;
	inline _TInt GetMaxId() const;
	inline _TInt GetInvalidId() const;

	bool IsIdFree(_TInt id) const;
	_TInt GetFreeId();
	bool ReserveId(_TInt id);
	bool RecycleId(_TInt id);

	str GetDebugState() const;

private:
	struct AllocFreeSegment {
		AllocFreeSegment(_TInt first_alloc, _TInt first_free, _TInt last_free):
			first_alloc_id_(first_alloc),
			first_free_id_(first_free),
			last_free_id_(last_free) {
		}
		_TInt first_alloc_id_;
		_TInt first_free_id_;
		_TInt last_free_id_;
	};

	typedef std::list<AllocFreeSegment> IdSegmentList;

	void AllocateIdOnSegment(_TInt id, const typename IdSegmentList::iterator& x);
	void FreeIdOnSegment(_TInt id, const typename IdSegmentList::iterator& x);

	IdSegmentList id_segment_list_;

	_TInt invalid_id_;
};



}



#include "idmanager.inl"



#undef ID_TEMPLATE
#undef ID_QUAL
