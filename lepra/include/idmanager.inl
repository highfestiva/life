
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "lepraassert.h"



namespace lepra {



ID_TEMPLATE
ID_QUAL::IdManager(_TInt min_id, _TInt max_id, _TInt invalid_id):
	invalid_id_(invalid_id) {
	deb_assert(max_id+1 > max_id);	// Our ID segmentation model assumes that ID doesn't wrap on max+1.
	id_segment_list_.push_back(AllocFreeSegment(min_id, min_id, max_id));
}

ID_TEMPLATE
_TInt ID_QUAL::GetMinId() const {
	return (id_segment_list_.front().first_alloc_id_);
}

ID_TEMPLATE
_TInt ID_QUAL::GetMaxId() const {
	return (id_segment_list_.back().last_free_id_);
}

ID_TEMPLATE
_TInt ID_QUAL::GetInvalidId() const {
	return (invalid_id_);
}

ID_TEMPLATE
bool ID_QUAL::IsIdFree(_TInt id) const {
	typename IdSegmentList::const_iterator x = id_segment_list_.begin();
	for (; x != id_segment_list_.end(); ++x) {
		if (id >= x->first_free_id_ && id <= x->last_free_id_) {
			// We found our range. Would allocate it here.
			return (true);	// TRICKY: RAII allow less code and less error-prone code here.
		} else if (id >= x->first_alloc_id_ && id < x->first_free_id_) {
			// Number already taken.
			break;
		}
	}
	return (false);
}

ID_TEMPLATE
_TInt ID_QUAL::GetFreeId() {
	typename IdSegmentList::iterator x = id_segment_list_.begin();
	if (x->first_free_id_ <= x->last_free_id_) {
		// TRICKY: fall through.
	} else if (++x != id_segment_list_.end()) {	// We must be able to allocate on next (otherwise it would not be there == end of valid range).
		// TRICKY: fall through.
	} else {
		// TRICKY: RAII allow less code and less error-prone code here.
		return (GetInvalidId());
	}
	_TInt _id = x->first_free_id_;
	AllocateIdOnSegment(_id, x);
	return (_id);
}

ID_TEMPLATE
bool ID_QUAL::ReserveId(_TInt id) {
	typename IdSegmentList::iterator x = id_segment_list_.begin();
	for (; x != id_segment_list_.end(); ++x) {
		if (id >= x->first_free_id_ && id <= x->last_free_id_) {
			// We found our range. Allocate here.
			AllocateIdOnSegment(id, x);
			return (true);	// TRICKY: RAII allow less code and less error-prone code here.
		} else if (id >= x->first_alloc_id_ && id < x->first_free_id_) {
			// Number already taken.
			break;
		}
	}
	return (false);
}

ID_TEMPLATE
bool ID_QUAL::RecycleId(_TInt id) {
	typename IdSegmentList::iterator x = id_segment_list_.begin();
	for (; x != id_segment_list_.end(); ++x) {
		if (id >= x->first_alloc_id_ && id < x->first_free_id_) {
			// We found our range. Free here.
			FreeIdOnSegment(id, x);
			return (true);	// TRICKY: RAII allow less code and less error-prone code here.
		} else if (id >= x->first_free_id_ && id <= x->last_free_id_) {
			// Number already free.
			break;
		}
	}
	return (false);
}

ID_TEMPLATE
str ID_QUAL::GetDebugState() const {
	str debug_state;
	typename IdSegmentList::const_iterator x = id_segment_list_.begin();
	for (; x != id_segment_list_.end(); ++x) {
		debug_state += strutil::Format("%i-%i, %i-%i,\n", x->first_alloc_id_, x->first_free_id_-1, x->first_free_id_, x->last_free_id_);
	}
	return (debug_state);
}



ID_TEMPLATE
void ID_QUAL::AllocateIdOnSegment(_TInt id, const typename IdSegmentList::iterator& x) {
	if (id == x->first_free_id_) {	// Place in front of current free range?
		// Just decrease our "free count" by increasing this segment's "first free"  by one.
		++(x->first_free_id_);
		// Check if we have no more free IDs left; in that case we join with next segment.
		typename IdSegmentList::iterator z = x;
		++z;
		if (z != id_segment_list_.end() && x->first_free_id_ > x->last_free_id_) {
			// Drop the segment with no free IDs in.
			z->first_alloc_id_ = x->first_alloc_id_;
			id_segment_list_.erase(x);
		}
	} else if (id == x->last_free_id_) {	// Place in back of current free range?
		if (id == GetMaxId()) {
			// Insert a free segment with a single free number in it at the end.
			id_segment_list_.insert(id_segment_list_.end(), AllocFreeSegment(id, id+1, id));
			--(x->last_free_id_);
		} else {
			// "Strech" the next segment's alloc range down by decreasing it by one.
			--(x->last_free_id_);
			typename IdSegmentList::iterator z = x;
			++z;
			--(z->first_alloc_id_);
		}
	} else {	// Split current segment in two. Place new number on the second segment.
		typename IdSegmentList::iterator z = x;
		++z;
		// Create new segment to hold this ID.
		z = id_segment_list_.insert(z, AllocFreeSegment(id, id+1, x->last_free_id_));
		// Cut the first of the two segments short.
		x->last_free_id_ = id-1;
	}
}

ID_TEMPLATE
void ID_QUAL::FreeIdOnSegment(_TInt id, const typename IdSegmentList::iterator& x) {
	if (id == x->first_free_id_-1) {	// Drop from back of current alloc range?
		// Just decrease our "alloc count" by decreasing this segment's "last alloc"  by one.
		--(x->first_free_id_);
		// Check if we have no more allocated IDs left; in that case we join with previous segment.
		if (x != id_segment_list_.begin()) {
			typename IdSegmentList::iterator z = x;
			--z;
			if (x->first_alloc_id_ >= x->first_free_id_) {
				// Drop the segment with no allocated IDs in.
				z->last_free_id_ = x->last_free_id_;
				id_segment_list_.erase(x);
			}
		}
	} else if (id == x->first_alloc_id_) {	// Drop from front of current alloc range?
		if (id == GetMinId()) {
			// Insert a free segment with a single free number in it at the front.
			id_segment_list_.insert(x, AllocFreeSegment(id, id, id));
			++(x->first_alloc_id_);
		} else {
			// "Strech" the previous segment's free range up by increasing it by one.
			++(x->first_alloc_id_);
			typename IdSegmentList::iterator z = x;
			--z;
			++(z->last_free_id_);
		}
	} else {	// Split current segment in two. Place free number on the first segment.
		// Create new segment to hold this ID.
		id_segment_list_.insert(x, AllocFreeSegment(x->first_alloc_id_, id, id));
		// Cut the second of the two segments short.
		x->first_alloc_id_ = id-1;
	}
}



}
