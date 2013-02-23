
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games


// To be able to index the table, the key must be converted to a size_t.
// The keys must be comparable using the equal-operator ==.



#pragma once

#include "String.h"	// Pulls in both basich hashing and narrow and wide string hashing.
#include <hash_map>

#define HASHTABLE_TEMPLATE template<class _TTableKey, class _TTableObject, class _THash, unsigned SIZE>
#define HASHTABLE_QUAL HashTable<_TTableKey, _TTableObject, _THash, SIZE>



namespace Lepra
{



template<class _TTableKey, class _TTableObject, class _THash = std::hash<_TTableKey>, unsigned SIZE = 64>
class HashTable
{
public:
	friend class Iterator;

	typedef std::hash_map<_TTableKey, _TTableObject, _THash> HashMap;

	class Iterator
	{
	public:

		friend class HashTable<_TTableKey, _TTableObject, _THash, SIZE>;

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
		_TTableObject& operator*() const	{ return (*mIterator).second; }

		_TTableObject& GetObject() const { return (*mIterator).second; }
		_TTableKey    GetKey() const { return (*mIterator).first; }

		// Prefix increment operator for traversing a hash table.
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
		Iterator(const typename HashMap::iterator& pIterator) :
			mIterator(pIterator)
		{
		}
		typename HashMap::iterator mIterator;
	};

	class ConstIterator
	{
	public:

		friend class HashTable<_TTableKey, _TTableObject, _THash, SIZE>;

		ConstIterator(){}

		ConstIterator& operator = (const ConstIterator& pOther)
		{
			mIterator = pOther.mIterator;
			return *this;
		}

		// Logical equality/inequality operator for iterator comparisons.
		bool operator==(const ConstIterator& pOther) const		{ return mIterator == pOther.mIterator; }
		bool operator!=(const ConstIterator& pOther) const		{ return !operator==(pOther); }

		// Gets the contents of the iterator.
		const _TTableObject& operator*() const	{ return (*mIterator).second; }

		const _TTableObject& GetObject() const { return (*mIterator).second; }
		_TTableKey    GetKey() const { return (*mIterator).first; }

		// Prefix increment operator for traversing a hash table.
		ConstIterator& operator++()
		{
			++mIterator;
			return *this;
		}

		// Postfix increment operator for traversing a hash table.
		ConstIterator  operator++(int)
		{
			Iterator i(mIterator);
			++mIterator;
			return i;
		}

	private:
		ConstIterator(const typename HashMap::const_iterator& pIterator) :
			mIterator(pIterator)
		{
		}
		typename HashMap::const_iterator mIterator;
	};

	HashTable(unsigned pSize = SIZE);
	HashTable(const HashTable& pOther);
	~HashTable();

	void Insert(const _TTableKey& pKey, const _TTableObject& pObject);
	void Remove(const _TTableKey& pKey);
	void Remove(const Iterator& pIter);

	void RemoveAll();

	_TTableObject FindObject(const _TTableKey& pKey) const;
	Iterator Find(const _TTableKey& pKey);
	ConstIterator Find(const _TTableKey& pKey) const;

	inline Iterator First();
	inline Iterator End();

	inline ConstIterator First() const;
	inline ConstIterator End() const;

	inline int GetCount() const;
	inline bool IsEmpty() const;
	inline bool Contains(const _TTableKey& pKey) const;

	const HashTable& operator=(const HashTable& pOther);

private: 
	HashMap mHashMap;
};

HASHTABLE_TEMPLATE int HASHTABLE_QUAL::GetCount() const
{
	return (int)mHashMap.size();
}

HASHTABLE_TEMPLATE bool HASHTABLE_QUAL::IsEmpty() const
{
	return mHashMap.empty();
}

HASHTABLE_TEMPLATE HASHTABLE_QUAL::HashTable(unsigned pSize) :
	mHashMap(pSize)
{
}

HASHTABLE_TEMPLATE HASHTABLE_QUAL::HashTable(const HashTable& pOther) :
	mHashMap(pOther.mHashMap)
{
}

HASHTABLE_TEMPLATE HASHTABLE_QUAL::~HashTable()
{
	RemoveAll();
}


HASHTABLE_TEMPLATE void HASHTABLE_QUAL::RemoveAll()
{
	mHashMap.clear();
}


HASHTABLE_TEMPLATE void HASHTABLE_QUAL::Insert(const _TTableKey& pKey, const _TTableObject& pObject)
{
	mHashMap.insert(HashMap::value_type(pKey, pObject));
}

HASHTABLE_TEMPLATE void HASHTABLE_QUAL::Remove(const _TTableKey& pKey)
{
	mHashMap.erase(pKey);
}

HASHTABLE_TEMPLATE void HASHTABLE_QUAL::Remove(const Iterator& pIter)
{
	mHashMap.erase(pIter.mIterator);
}

HASHTABLE_TEMPLATE _TTableObject HASHTABLE_QUAL::FindObject(const _TTableKey& pKey) const
{
	typename HashMap::const_iterator lIter(mHashMap.find(pKey));
	if (lIter != mHashMap.end())
	{
		return (*lIter).second;
	}
	return(0);
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::Iterator HASHTABLE_QUAL::Find(const _TTableKey& pKey)
{
	return Iterator(mHashMap.find(pKey));
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::ConstIterator HASHTABLE_QUAL::Find(const _TTableKey& pKey) const
{
	return ConstIterator(mHashMap.find(pKey));
}

HASHTABLE_TEMPLATE bool HASHTABLE_QUAL::Contains(const _TTableKey& pKey) const
{
	return mHashMap.find(pKey) != mHashMap.end();
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::Iterator HASHTABLE_QUAL::First()
{
	return Iterator(mHashMap.begin());
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::Iterator HASHTABLE_QUAL::End()
{
	return Iterator(mHashMap.end());
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::ConstIterator HASHTABLE_QUAL::First() const
{
	return ConstIterator(mHashMap.begin());
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::ConstIterator HASHTABLE_QUAL::End() const
{
	return ConstIterator(mHashMap.end());
}

HASHTABLE_TEMPLATE const HASHTABLE_QUAL& HASHTABLE_QUAL::operator=(const HashTable& pOther)
{
	mHashMap = pOther.mHashMap;
	return *this;
}



}



#undef HASHTABLE_TEMPLATE
#undef HASHTABLE_QUAL
