
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "HashTable.h"

#define TEMPLATE template<class KeyType, class ObjectType, class HashFunc>
#define QUAL OrderedMap<KeyType, ObjectType, HashFunc>



namespace Lepra
{



template<class KeyType, class ObjectType, class HashFunc = std::hash<KeyType> >
class OrderedMap
{
private:
	class Node
	{
	public:
		Node():
			mObject(0)
		{
		};
		Node(KeyType pKey, ObjectType pObject) :
			mKey(pKey),
			mObject(pObject)
		{
		};

		KeyType mKey;
		ObjectType mObject;
	};

	// Used to avoid warning C4503 (truncated type names).
	struct IteratorContainer
	{
		IteratorContainer(const typename std::list<Node>::iterator& pIt) : mIt(pIt) {}
		typename std::list<Node>::iterator mIt;
	};

	typedef HashTable<KeyType, IteratorContainer, HashFunc, 32> TTable;
	typedef std::list<Node> NodeList;
public:

	class ConstIterator;

	class Iterator
	{
	public:
		friend class OrderedMap<KeyType, ObjectType, HashFunc>;

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
		bool operator==(ObjectType pOtherObject) const			{ return (*mIter) == pOtherObject; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& pOther) const		{ return mIter != pOther.mIter; }
		bool operator!=(const ConstIterator& pOther) const	{ return mIter != pOther.mIter; }
		bool operator!=(ObjectType pOtherObject) const			{ return (*mIter) != pOtherObject; }

		// Gets the contents of the iterator.
		KeyType& GetKey() const { return (*mIter).mKey; }
		ObjectType& GetObject() const { return (*mIter).mObject; }

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

		Iterator(const typename NodeList::iterator& pIter) : mIter(pIter){}
		typename NodeList::iterator mIter;
	};

	class ConstIterator
	{
	public:
		friend class OrderedMap<ObjectType, HashFunc>;

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
		bool operator==(ObjectType pOtherObject) const				{ return (*mIter) == pOtherObject; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& pOther) const		{ return mIter != pOther.mIter; }
		bool operator!=(const ConstIterator& pOther) const	{ return mIter != pOther.mIter; }
		bool operator!=(ObjectType pOtherObject) const				{ return (*mIter) != pOtherObject; }

		// Gets the contents of the iterator.
		KeyType& GetKey() const { return (*mIter).mKey; }
		ObjectType& GetObject() const { return (*mIter).mObject; }

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

		ConstIterator(const typename NodeList::const_iterator& pIter) : mIter(pIter){}
		typename NodeList::const_iterator mIter;
	};

	inline OrderedMap();
	inline OrderedMap(const OrderedMap& pOther);
	inline ~OrderedMap();

	inline Iterator PushFront(KeyType pKey, ObjectType pObject);
	inline Iterator PushBack(KeyType pKey, ObjectType pObject);

	// Returns key and Object. Make sure the quick list isn't empty.
	inline void PopFront(KeyType& pKey, ObjectType& pObject);
	inline void PopBack(KeyType& pKey, ObjectType& pObject);

	inline Iterator InsertBefore(const Iterator& pBeforeHere, KeyType pKey, ObjectType pObject);
	inline Iterator InsertAfter(const Iterator& pAfterHere, KeyType pKey, ObjectType pObject);

	inline void Remove(const Iterator& pIter);
	inline bool Remove(KeyType pKey);

	inline void RemoveAll();

	inline Iterator Find(KeyType pKey);
	inline bool Exists(KeyType pKey);
	inline int GetIndex(KeyType pKey); // Returns -1 if the Object doesn't exist.

	// Will swap node position in the list. The iterators will also swap their contents,
	// so the same position in the list is conserved.
	inline void Swap(Iterator& pIt1, Iterator& pIt2);

	inline Iterator First();
	inline Iterator Last();
	inline Iterator End();

	inline ConstIterator First() const;
	inline ConstIterator Last() const;
	inline ConstIterator End() const;

	inline size_t GetCountSafe() const;
	inline size_t GetCount() const;
	inline bool IsEmpty() const;

	inline const OrderedMap& operator=(const OrderedMap& pOther);

private:

	NodeList mList;
	TTable mTable;
};

TEMPLATE QUAL::OrderedMap()
{
}

TEMPLATE QUAL::OrderedMap(const OrderedMap& pOther) :
	mList(pOther.mList),
	mTable(pOther.mTable)
{
}

TEMPLATE QUAL::~OrderedMap()
{
}

TEMPLATE typename QUAL::Iterator QUAL::PushFront(KeyType pKey, ObjectType pObject)
{
	deb_assert(mTable.Find(pKey) == mTable.End());
	mList.push_front(Node(pKey, pObject));
	typename NodeList::iterator lIter = mList.begin();
	mTable.Insert(pKey, lIter);
	deb_assert(mTable.GetCount() == (int)mList.size());
	return Iterator(lIter);
}

TEMPLATE typename QUAL::Iterator QUAL::PushBack(KeyType pKey, ObjectType pObject)
{
	deb_assert(mTable.Find(pKey) == mTable.End());
	mList.push_back(Node(pKey, pObject));
	typename NodeList::iterator lIter = mList.end();
	--lIter;
	mTable.Insert(pKey, lIter);
	deb_assert(mTable.GetCount() == (int)mList.size());
	return Iterator(lIter);
}

TEMPLATE void QUAL::PopFront(KeyType& pKey, ObjectType& pObject)
{
	if (mList.empty() == false)
	{
		Node& lNode = mList.front();
		pKey = lNode.mKey;
		pObject = lNode.mObject;
		mTable.Remove(lNode.mKey);
		mList.pop_front();
	}
	deb_assert(mTable.GetCount() == (int)mList.size());
}

TEMPLATE void QUAL::PopBack(KeyType& pKey, ObjectType& pObject)
{
	if (mList.empty() == false)
	{
		Node& lNode = mList.back();
		pKey = lNode.mKey;
		pObject = lNode.mObject;
		mTable.Remove(lNode.mKey);
		mList.pop_back();
	}
	deb_assert(mTable.GetCount() == (int)mList.size());
}

TEMPLATE typename QUAL::Iterator QUAL::InsertBefore(const Iterator& pBeforeHere, KeyType pKey, ObjectType pObject)
{
	typename NodeList::Iterator lIter(mList.InsertBefore(pBeforeHere.mIter, Node(pKey, pObject)));
	mTable.Insert(pKey, lIter);
	return lIter;
}

TEMPLATE typename QUAL::Iterator QUAL::InsertAfter(const Iterator& pAfterHere, KeyType pKey, ObjectType pObject)
{
	typename NodeList::Iterator lIter(mList.InsertAfter(pAfterHere.mIter, Node(pKey, pObject)));
	mTable.Insert(pKey, lIter);
	return lIter;
}

TEMPLATE void QUAL::Remove(const Iterator& pIter)
{
	deb_assert(mTable.GetCount() == (int)mList.size());
	mTable.Remove(pIter.GetKey());
	mList.erase(pIter.mIter);
	deb_assert(mTable.GetCount() == (int)mList.size());
}

TEMPLATE bool QUAL::Remove(KeyType pKey)
{
	deb_assert(mTable.GetCount() == (int)mList.size());
	typename TTable::Iterator lIter(mTable.Find(pKey));
	const bool lErased = (lIter != mTable.End());
	if (lErased)
	{
		mList.erase((*lIter).mIt);
		mTable.Remove(lIter);
	}
	deb_assert(mTable.GetCount() == (int)mList.size());
	return (lErased);
}

TEMPLATE void QUAL::RemoveAll()
{
	deb_assert(mTable.GetCount() == (int)mList.size());
	mList.clear();
	mTable.RemoveAll();
	deb_assert(mTable.GetCount() == (int)mList.size());
}

TEMPLATE typename QUAL::Iterator QUAL::Find(KeyType pKey)
{
	typename TTable::Iterator lIter(mTable.Find(pKey));
	if (lIter == mTable.End())
	{
		return End();
	}
	else
	{
		return Iterator((*lIter).mIt);
	}
}

TEMPLATE bool QUAL::Exists(KeyType pKey)
{
	return mTable.Contains(pKey);
}

TEMPLATE int QUAL::GetIndex(KeyType pKey)
{
	int i = 0;
	int lIndex = -1;

	typename NodeList::Iterator lIter;
	for(lIter = mList.First(); lIter != mList.End(); ++lIter, ++i)
	{
		if((*lIter).mKey == pKey)
		{
			lIndex = i;
			break;
		}
	}

	return lIndex;
}

TEMPLATE void QUAL::Swap(Iterator& pIt1, Iterator& pIt2)
{
	mList.Swap(pIt1.mIter, pIt2.mIter);
}

TEMPLATE typename QUAL::Iterator QUAL::First()
{
	return Iterator(mList.begin());
}

TEMPLATE typename QUAL::Iterator QUAL::Last()
{
	return Iterator(--mList.end());
}

TEMPLATE typename QUAL::Iterator QUAL::End()
{
	return Iterator(mList.end());
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

TEMPLATE size_t QUAL::GetCountSafe() const
{
	return (mTable.GetCount());	// This is safe as long as STLport is using member integer for size() for hash_map type.
}

TEMPLATE size_t QUAL::GetCount() const
{
	deb_assert(mTable.GetCount() == (int)mList.size());
	return (GetCountSafe());
}

TEMPLATE bool QUAL::IsEmpty() const
{
	deb_assert(mTable.GetCount() == (int)mList.size());
	return (mTable.IsEmpty());
}

TEMPLATE const OrderedMap<KeyType, ObjectType, HashFunc>& QUAL::operator=(const OrderedMap& pOther)
{
	mList = pOther.mList;
	mTable = pOther.mTable;
	return (*this);
}



}



#undef TEMPLATE
#undef QUAL
