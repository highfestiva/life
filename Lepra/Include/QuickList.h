/*
	Class:  QuickList
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	The quicklist is a time efficient data structure which combines
	the advantages of both the linked list and the hash table. Due
	to the combination of these two data structures, the result is
	not very memory efficient.
*/

#pragma once

#include <list>
#include "HashTable.h"

#define TEMPLATE template<class T, class HashFunc>
#define QUAL QuickList<T, HashFunc>


namespace Lepra
{

template<class T, class HashFunc = HashTableHash<T>>
class QuickList
{
private:
	typedef std::list<T> TList;
	typedef HashTable<T, typename std::list<T>::iterator, HashFunc, 32> TTable;
public:

	class ConstIterator;

	class Iterator
	{
	public:
		friend class QuickList<T, HashFunc>;

		Iterator(){}
		Iterator(const Iterator& pIterator) : mIter(pIterator.mIter){}

		Iterator& operator = (const Iterator& pOther)
		{
			mIter = pOther.mIter;
			return *this;
		}

		Iterator& operator = (const Iterator* pOther)
		{
			mIter = pOther->mIter;
			return *this;
		}

		// Logical equality operator for iterator comparisons.
		bool operator==(const Iterator& pOther) const		{ return mIter == pOther.mIter; }
		bool operator==(const ConstIterator& pOther) const	{ return mIter == pOther.mIter; }
		bool operator==(T pOtherItem) const				{ return (*mIter) == pOtherItem; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& pOther) const		{ return mIter != pOther.mIter; }
		bool operator!=(const ConstIterator& pOther) const	{ return mIter != pOther.mIter; }
		bool operator!=(T pOtherItem) const				{ return (*mIter) != pOtherItem; }

		// Gets the contents of the iterator.
		T& operator*() const	{ return (*mIter); }

		// Pre- and postfix increment operators for traversing a list.
		Iterator& operator++()		{ ++mIter; return *this; }
		Iterator  operator++(int)	{ return Iterator(mIter++);}

		// Pre- and postfix decrement operators for traversing a list.
		Iterator& operator--()		{ --mIter; return *this; }
		Iterator  operator--(int)	{ return Iterator(mIter--); }

		Iterator& operator+=(int pCount)
		{
			mIter += pCount;
			return *this;
		}

		Iterator& operator-=(int pCount)
		{
			mIter -= pCount;
			return *this;
		}

		Iterator operator + (int pCount)
		{
			return Iterator(mIter + pCount);
		}

		Iterator operator - (int pCount)
		{
			return Iterator(mIter - pCount);
		}

	private:

		Iterator(const typename TList::Iterator& pIter) : mIter(pIter){}
		typename TList::Iterator mIter;
	};

	class ConstIterator
	{
	public:
		friend class QuickList<T, HashFunc>;

		ConstIterator(){}
		ConstIterator(const Iterator& pIterator) : mIter(pIterator.mIter){}
		ConstIterator(const ConstIterator& pIterator) : mIter(pIterator.mIter){}

		ConstIterator& operator = (const ConstIterator& pOther)
		{
			mIter = pOther.mIter;
			return *this;
		}

		ConstIterator& operator = (const ConstIterator* pOther)
		{
			mIter = pOther->mIter;
			return *this;
		}

		// Logical equality operator for iterator comparisons.
		bool operator==(const Iterator& pOther) const		{ return mIter == pOther.mIter; }
		bool operator==(const ConstIterator& pOther) const	{ return mIter == pOther.mIter; }
		bool operator==(T pOtherItem) const				{ return (*mIter) == pOtherItem; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& pOther) const		{ return mIter != pOther.mIter; }
		bool operator!=(const ConstIterator& pOther) const	{ return mIter != pOther.mIter; }
		bool operator!=(T pOtherItem) const				{ return (*mIter) != pOtherItem; }

		// Gets the contents of the iterator.
		T& operator*() const	{ return (*mIter); }

		// Pre- and postfix increment operators for traversing a list.
		ConstIterator& operator++()		{ ++mIter; return *this; }
		ConstIterator  operator++(int)	{ return ConstIterator(mIter++);}

		// Pre- and postfix decrement operators for traversing a list.
		ConstIterator& operator--()		{ --mIter; return *this; }
		ConstIterator  operator--(int)	{ return ConstIterator(mIter--); }

		ConstIterator& operator+=(int pCount)
		{
			mIter += pCount;
			return *this;
		}

		ConstIterator& operator-=(int pCount)
		{
			mIter -= pCount;
			return *this;
		}

		ConstIterator operator + (int pCount)
		{
			return ConstIterator(mIter + pCount);
		}

		ConstIterator operator - (int pCount)
		{
			return ConstIterator(mIter - pCount);
		}

	private:

		ConstIterator(const typename TList::ConstIterator& pIter) : mIter(pIter){}
		typename TList::ConstIterator mIter;
	};

	inline QuickList();
	inline QuickList(const QuickList& pOther);
	inline ~QuickList();

	inline Iterator PushFront(T pItem);
	inline Iterator PushBack(T pItem);

	inline Iterator PushFrontUnique(T pItem);
	inline Iterator PushBackUnique(T pItem);

	inline T PopFront();
	inline T PopBack();

	inline Iterator InsertBefore(const Iterator& pBeforeHere, T pItem);
	inline Iterator InsertAfter(const Iterator& pAfterHere, T pItem);

	inline void Remove(const Iterator& pItem);
	inline void Remove(T pItem);

	inline void RemoveAll();
	inline void DeleteAll();

	inline Iterator Find(T pItem);
	inline Iterator FindN(int pNum);
	inline bool Exists(T pItem);
	inline int GetIndex(T pItem); // Returns -1 if the item doesn't exist.

	// Will swap node position in the list. The iterators will also swap their contents,
	// so the same position in the list is conserved.
	inline void Swap(Iterator& pIt1, Iterator& pIt2);

	// Swaps if not already first/last in the list.
	inline void SwapWithNext(Iterator& pIt);
	inline void SwapWithPrevious(Iterator& pIt);

	// A special function used in caches. If there is a hit on an item in the cache,
	// it should be moved towards the beginning or the end of the list, to be able to 
	// remove rarely used items first when the cache grows too large.
	inline void CompeteToFront(Iterator& pIt);
	inline void CompeteToLast(Iterator& pIt);

	inline Iterator First();
	inline Iterator Last();
	inline Iterator End();

	inline ConstIterator First() const;
	inline ConstIterator Last() const;
	inline ConstIterator End() const;

	inline int GetCount() const;
	inline bool IsEmpty() const;

	inline const QuickList& operator=(const QuickList& pOther);

private:

	TList mList;
	TTable mTable;
};

TEMPLATE QUAL::QuickList()
{
}

TEMPLATE QUAL::QuickList(const QuickList& pOther) :
	mList(pOther.mList),
	mTable(pOther.mTable)
{
}

TEMPLATE QUAL::~QuickList()
{
}

TEMPLATE typename QUAL::Iterator QUAL::PushFront(T pItem)
{
	TList::Iterator lIter = mList.PushFront(pItem);
	mTable.Insert(pItem, lIter);
	return Iterator(lIter);
}

TEMPLATE typename QUAL::Iterator QUAL::PushBack(T pItem)
{
	TList::Iterator lIter = mList.PushBack(pItem);
	mTable.Insert(pItem, lIter);
	return Iterator(lIter);
}

TEMPLATE typename QUAL::Iterator QUAL::PushFrontUnique(T pItem)
{
	if (mTable.Find(pItem) == mTable.End())
	{
		return PushFront(pItem);
	}
	else
	{
		return End();
	}
}

TEMPLATE typename QUAL::Iterator QUAL::PushBackUnique(T pItem)
{
	if (mTable.Find(pItem) == mTable.End())
	{
		return PushBack(pItem);
	}
	else
	{
		return End();
	}
}

TEMPLATE T QUAL::PopFront()
{
	if (mList.IsEmpty() == true)
	{
		// Well... We have to return /something/.
		return mList.PopFront();
	}
	else
	{
		T lObject = mList.PopFront();
		mTable.Remove(lObject);
		return lObject;
	}
}

TEMPLATE T QUAL::PopBack()
{
	if (mList.IsEmpty() == true)
	{
		// Well... We have to return /something/.
		return mList.PopBack();
	}
	else
	{
		T lObject = mList.PopBack();
		mTable.Remove(lObject);
		return lObject;
	}
}

TEMPLATE typename QUAL::Iterator QUAL::InsertBefore(const Iterator& pBeforeHere, T pItem)
{
	TList::Iterator lIter(mList.InsertBefore(pBeforeHere.mIter, pItem));
	mTable.Insert(pItem, lIter);
	return lIter;
}

TEMPLATE typename QUAL::Iterator QUAL::InsertAfter(const Iterator& pAfterHere, T pItem)
{
	TList::Iterator lIter(mList.InsertAfter(pAfterHere.mIter, pItem));
	mTable.Insert(pItem, lIter);
	return lIter;
}

TEMPLATE void QUAL::Remove(const Iterator& pItem)
{
	mList.Remove(pItem.mIter);
	mTable.Remove(*pItem.mIter);
}

TEMPLATE void QUAL::Remove(T pItem)
{
	TTable::Iterator lIter(mTable.Find(pItem));
	mList.Remove(*lIter);
	mTable.Remove(lIter);
}

TEMPLATE void QUAL::RemoveAll()
{
	mList.RemoveAll();
	mTable.RemoveAll();
}

TEMPLATE void QUAL::DeleteAll()
{
	mList.DeleteAll();
	mTable.RemoveAll();
}

TEMPLATE typename QUAL::Iterator QUAL::Find(T pItem)
{
	TTable::Iterator lIter(mTable.Find(pItem));
	if (lIter == mTable.End())
	{
		return End();
	}
	else
	{
		return Iterator(*lIter);
	}
}

TEMPLATE typename QUAL::Iterator QUAL::FindN(int pNum)
{
	// This function is unoptimized.
	return Iterator(mList.FindN(pNum));
}

TEMPLATE bool QUAL::Exists(T pItem)
{
	return mTable.Contains(pItem);
}

TEMPLATE int QUAL::GetIndex(T pItem)
{
	// This function is unoptimized.
	return mList.GetIndex(pItem);
}

TEMPLATE void QUAL::Swap(Iterator& pIt1, Iterator& pIt2)
{
	mList.Swap(pIt1.mIter, pIt2.mIter);
}

TEMPLATE void QUAL::SwapWithNext(Iterator& pIt)
{
	mList.SwapWithNext(pIt.mIter);
}

TEMPLATE void QUAL::SwapWithPrevious(Iterator& pIt)
{
	mList.SwapWithPrevious(pIt.mIter);
}

TEMPLATE void QUAL::CompeteToFront(Iterator& pIt)
{
	mList.CompeteToFront(pIt.mIter);
}

TEMPLATE void QUAL::CompeteToLast(Iterator& pIt)
{
	mList.CompeteToLast(pIt.mIter);
}

TEMPLATE typename QUAL::Iterator QUAL::First()
{
	return Iterator(mList.First());
}

TEMPLATE typename QUAL::Iterator QUAL::Last()
{
	return Iterator(mList.Last());
}

TEMPLATE typename QUAL::Iterator QUAL::End()
{
	return Iterator(mList.End());
}

TEMPLATE typename QUAL::ConstIterator QUAL::First() const
{
	return ConstIterator(mList.First());
}

TEMPLATE typename QUAL::ConstIterator QUAL::Last() const
{
	return ConstIterator(mList.Last());
}

TEMPLATE typename QUAL::ConstIterator QUAL::End() const
{
	return ConstIterator(mList.End());
}

TEMPLATE int QUAL::GetCount() const
{
	return mList.GetCount();
}

TEMPLATE bool QUAL::IsEmpty() const
{
	return mList.IsEmpty();
}

TEMPLATE const QuickList<T, HashFunc>& QUAL::operator=(const QuickList& pOther)
{
	mList = pOther.mList;
	mTable = pOther.mTable;
}

}

#undef TEMPLATE
#undef QUAL
