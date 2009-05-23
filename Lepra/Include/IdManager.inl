
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include <assert.h>



namespace Lepra
{



ID_TEMPLATE
ID_QUAL::IdManager(_TInt pMinId, _TInt pMaxId, _TInt pInvalidId):
	mInvalidId(pInvalidId)
{
	assert(pMaxId+1 > pMaxId);	// Our ID segmentation model assumes that ID doesn't wrap on max+1.
	mIdSegmentList.push_back(AllocFreeSegment(pMinId, pMinId, pMaxId));
}

ID_TEMPLATE
_TInt ID_QUAL::GetMinId()
{
	return (mIdSegmentList.front().mFirstAllocId);
}

ID_TEMPLATE
_TInt ID_QUAL::GetMaxId()
{
	return (mIdSegmentList.back().mLastFreeId);
}

ID_TEMPLATE
_TInt ID_QUAL::GetInvalidId()
{
	return (mInvalidId);
}

ID_TEMPLATE
bool ID_QUAL::IsIdFree(_TInt pId) const
{
	typename IdSegmentList::const_iterator x = mIdSegmentList.begin();
	for (; x != mIdSegmentList.end(); ++x)
	{
		if (pId >= x->mFirstFreeId && pId <= x->mLastFreeId)
		{
			// We found our range. Would allocate it here.
			return (true);	// TRICKY: RAII allow less code and less error-prone code here.
		}
		else if (pId >= x->mFirstAllocId && pId < x->mFirstFreeId)
		{
			// Number already taken.
			break;
		}
	}
	return (false);
}

ID_TEMPLATE
_TInt ID_QUAL::GetFreeId()
{
	typename IdSegmentList::iterator x = mIdSegmentList.begin();
	if (x->mFirstFreeId <= x->mLastFreeId)
	{
		// TRICKY: fall through.
	}
	else if (++x != mIdSegmentList.end())	// We must be able to allocate on next (otherwise it would not be there == end of valid range).
	{
		// TRICKY: fall through.
	}
	else
	{
		// TRICKY: RAII allow less code and less error-prone code here.
		return (GetInvalidId());
	}
	_TInt lId = x->mFirstFreeId;
	AllocateIdOnSegment(lId, x);
	return (lId);
}

ID_TEMPLATE
bool ID_QUAL::ReserveId(_TInt pId)
{
	typename IdSegmentList::iterator x = mIdSegmentList.begin();
	for (; x != mIdSegmentList.end(); ++x)
	{
		if (pId >= x->mFirstFreeId && pId <= x->mLastFreeId)
		{
			// We found our range. Allocate here.
			AllocateIdOnSegment(pId, x);
			return (true);	// TRICKY: RAII allow less code and less error-prone code here.
		}
		else if (pId >= x->mFirstAllocId && pId < x->mFirstFreeId)
		{
			// Number already taken.
			break;
		}
	}
	return (false);
}

ID_TEMPLATE
bool ID_QUAL::RecycleId(_TInt pId)
{
	typename IdSegmentList::iterator x = mIdSegmentList.begin();
	for (; x != mIdSegmentList.end(); ++x)
	{
		if (pId >= x->mFirstAllocId && pId < x->mFirstFreeId)
		{
			// We found our range. Free here.
			FreeIdOnSegment(pId, x);
			return (true);	// TRICKY: RAII allow less code and less error-prone code here.
		}
		else if (pId >= x->mFirstFreeId && pId <= x->mLastFreeId)
		{
			// Number already free.
			break;
		}
	}
	return (false);
}

ID_TEMPLATE
String ID_QUAL::GetDebugState() const
{
	String lDebugState;
	typename IdSegmentList::const_iterator x = mIdSegmentList.begin();
	for (; x != mIdSegmentList.end(); ++x)
	{
		lDebugState += StringUtility::Format(_T("%i-%i, %i-%i,\n"), x->mFirstAllocId, x->mFirstFreeId-1, x->mFirstFreeId, x->mLastFreeId);
	}
	return (lDebugState);
}



ID_TEMPLATE
void ID_QUAL::AllocateIdOnSegment(_TInt pId, const typename IdSegmentList::iterator& x)
{
	if (pId == x->mFirstFreeId)	// Place in front of current free range?
	{
		// Just decrease our "free count" by increasing this segment's "first free"  by one.
		++(x->mFirstFreeId);
		// Check if we have no more free IDs left; in that case we join with next segment.
		typename IdSegmentList::iterator z = x;
		++z;
		if (z != mIdSegmentList.end() && x->mFirstFreeId > x->mLastFreeId)
		{
			// Drop the segment with no free IDs in.
			z->mFirstAllocId = x->mFirstAllocId;
			mIdSegmentList.erase(x);
		}
	}
	else if (pId == x->mLastFreeId)	// Place in back of current free range?
	{
		if (pId == GetMaxId())
		{
			// Insert a free segment with a single free number in it at the end.
			mIdSegmentList.insert(mIdSegmentList.end(), AllocFreeSegment(pId, pId+1, pId));
			--(x->mLastFreeId);
		}
		else
		{
			// "Strech" the next segment's alloc range down by decreasing it by one.
			--(x->mLastFreeId);
			typename IdSegmentList::iterator z = x;
			++z;
			--(z->mFirstAllocId);
		}
	}
	else	// Split current segment in two. Place new number on the second segment.
	{
		typename IdSegmentList::iterator z = x;
		++z;
		// Create new segment to hold this ID.
		z = mIdSegmentList.insert(z, AllocFreeSegment(pId, pId+1, x->mLastFreeId));
		// Cut the first of the two segments short.
		x->mLastFreeId = pId-1;
	}
}

ID_TEMPLATE
void ID_QUAL::FreeIdOnSegment(_TInt pId, const typename IdSegmentList::iterator& x)
{
	if (pId == x->mFirstFreeId-1)	// Drop from back of current alloc range?
	{
		// Just decrease our "alloc count" by decreasing this segment's "last alloc"  by one.
		--(x->mFirstFreeId);
		// Check if we have no more allocated IDs left; in that case we join with previous segment.
		if (x != mIdSegmentList.begin())
		{
			typename IdSegmentList::iterator z = x;
			--z;
			if (x->mFirstAllocId >= x->mFirstFreeId)
			{
				// Drop the segment with no allocated IDs in.
				z->mLastFreeId = x->mLastFreeId;
				mIdSegmentList.erase(x);
			}
		}
	}
	else if (pId == x->mFirstAllocId)	// Drop from front of current alloc range?
	{
		if (pId == GetMinId())
		{
			// Insert a free segment with a single free number in it at the front.
			mIdSegmentList.insert(x, AllocFreeSegment(pId, pId, pId));
			++(x->mFirstAllocId);
		}
		else
		{
			// "Strech" the previous segment's free range up by increasing it by one.
			++(x->mFirstAllocId);
			typename IdSegmentList::iterator z = x;
			--z;
			++(z->mLastFreeId);
		}
	}
	else	// Split current segment in two. Place free number on the first segment.
	{
		// Create new segment to hold this ID.
		typename IdSegmentList::iterator z = mIdSegmentList.insert(x, AllocFreeSegment(x->mFirstAllocId, pId, pId));
		// Cut the second of the two segments short.
		x->mFirstAllocId = pId-1;
	}
}



}
