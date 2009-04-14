/*
	Class:  HashSet
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	A hash set. (Which is about the same as a hash table, but the objects inserted 
	are their own keys).

	To be able to index the table, the inserted object must be converted to a
	size_t. This is done through a hash function, which is defined like this:

	struct HashFunction
	{
		size_t operator()(const _TSetObject& pObject);
	};

	The objects must also be comparable using the equal-operator ==.
*/

#ifndef LEPRA_HASHSET_H
#define LEPRA_HASHSET_H

#include "FastAllocator.h"
#include "Hasher.h"
#include "LepraTypes.h"
#include <hash_set>

#define TEMPLATE template<class _TSetObject, class _THash, unsigned SIZE>
#define QUAL HashSet<_TSetObject, _THash, SIZE>

namespace Lepra
{

template<class _TSetObject, class _THash = std::hash<_TSetObject>, unsigned SIZE = 256>
class HashSet
{
public:
	friend class Iterator;

	typedef std::hash_set<_TSetObject, _THash> _HashSet;

	class Iterator
	{
	public:

		friend class HashSet<_TSetObject, _THash, SIZE>;

		Iterator(){}

		Iterator& operator = (const Iterator& pOther)
		{
			mIterator = pOther.mIterator;
			return *this;
		}

		// Logical equality/inequality operator for iterator comparisons.
		bool operator==(const Iterator& pOther) const		{ return mIterator == pOther.mIterator; }
		bool operator!=(const Iterator& pOther) const		{ return !operator==(pOther); }

		// Gets the contents of the iterator.
		const _TSetObject& operator*() const	{ return *mIterator; }

		_TSetObject GetObject() const { return *mIterator; }

		// Prefix increment operator for traversing a list.
		Iterator& operator++()
		{
			++mIterator;
			return *this;
		}

		// Postfix increment operator for traversing a hash table.
		Iterator  operator++(int)
		{
			Iterator i(mIterator);
			++mIterator;
			return i;
		}

	private:
		Iterator(const typename _HashSet::iterator& pIterator) :
			mIterator(pIterator)
		{
		}
		typename _HashSet::iterator mIterator;
	};

	HashSet(unsigned pSize = SIZE);
	~HashSet();

	inline bool IsEmpty() const;

	bool Insert(const _TSetObject& pObject);
	bool Remove(const _TSetObject& pObject);
	void Remove(const Iterator& pIter);

	void RemoveAll();

	Iterator Find(const _TSetObject& pObject);
	inline bool Exist(const _TSetObject& pObject);

	Iterator First();
	Iterator End();

	int GetCount() const;

private: 
	_HashSet mHashSet;
};

TEMPLATE int QUAL::GetCount() const
{
	return ((int)mHashSet.size());
}

TEMPLATE QUAL::HashSet(unsigned pSize) :
	mHashSet(pSize)
{
}

TEMPLATE QUAL::~HashSet()
{
	RemoveAll();
}

TEMPLATE bool QUAL::IsEmpty() const
{
	return mHashSet.empty();
}

TEMPLATE void QUAL::RemoveAll()
{
	mHashSet.clear();
}


TEMPLATE bool QUAL::Insert(const _TSetObject& pObject)
{
	return mHashSet.insert(pObject).second;
}

TEMPLATE bool QUAL::Remove(const _TSetObject& pObject)
{
	_HashSet::iterator lIter(mHashSet.find(pObject));
	bool lRemoved = (lIter != mHashSet.end());
	if (lRemoved == true)
	{
		mHashSet.erase(lIter);
	}

	return lRemoved;
}

TEMPLATE void QUAL::Remove(const Iterator& pIter)
{
	mHashSet.erase(pIter.mIterator);
}

TEMPLATE typename QUAL::Iterator QUAL::Find(const _TSetObject& pObject)
{
	return Iterator(mHashSet.find(pObject));
}

TEMPLATE bool QUAL::Exist(const _TSetObject& pObject)
{
	return (Find(pObject) != End());
}

TEMPLATE typename QUAL::Iterator QUAL::First()
{
	return Iterator(mHashSet.begin());
}

TEMPLATE typename QUAL::Iterator QUAL::End()
{
	return Iterator(mHashSet.end());
}

}

#undef TEMPLATE
#undef QUAL

#endif // !LEPRA_HASHSET_H
