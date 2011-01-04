/*
JB: use std::map instead. It has a balanced red-black tree in the bottom and thus also a complexity of O(log n),
    however it is faster in many ways, such as the assignment operator (which is not even implemented here).



	Class:  SkipList
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	A skip list (which is a Map, just like a hash table).
	
	Never heard of skip lists before? Well, you are not alone.

	It is a fast and SORTED(!) list (map) of key and value pairs. Each
	key maps to exactly one value (or object). Thus, there cannot be 
	more than one value per key.

	The list is sorted in ascending key order.

	All operations (insert, remove and find) have a time complexity
	of O(log n).

	The skip list stores redundant data, and requires about twice as
	much memory as a binary tree for instance.

	So what's the point using a skip list instead of a binary tree?

	Well... I have no idea.

	In the worst case, both a binary tree and a skip list will perform
	just as bad as a linked list, which is O(n). But since a skip list
	is using a probabilistic approach, there is always a high probability 
	that it will perform at O(log n).

	If you can guarantee that your binary tree is always perfecly balanced,
	and if you can do that in constant time, then using a skip list is just 
	plain stupid. :)
/

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "FastAllocator.h"
#include "LepraTypes.h"
#include "Random.h"

#define TEMPLATE template<class _TKey, class _TObject>
#define QUAL SkipList<_TKey, _TObject>

namespace Lepra
{

template<class _TKey, class _TObject>
class SkipList
{
private:
	class ListNode
	{
	public:

		inline ListNode() : mNext(0), mPrev(0), mUp(0), mDown(0) {}

		void Init()
		{
			mNext = 0;
			mPrev = 0;
			mUp   = 0;
			mDown = 0;
		}

		ListNode* mNext;
		ListNode* mPrev;
		ListNode* mUp;
		ListNode* mDown;

		_TKey     mKey;
		_TObject  mObject;
	};

public:
	class ConstIterator;

	class Iterator
	{
	public:
		friend class SkipList;

		Iterator() : mNode(0){}
		Iterator(const Iterator& pIterator) : mNode(pIterator.mNode){}

		Iterator& operator = (const Iterator& pOther)
		{
			mNode = pOther.mNode;

			return *this;
		}

		Iterator& operator = (const Iterator* pOther)
		{
			mNode = pOther->mNode;

			return *this;
		}

		// Logical equality operator for iterator comparisons.
		inline bool operator==(const Iterator& pOther) const		{ return mNode == pOther.mNode; }
		inline bool operator==(const ConstIterator& pOther) const	{ return mNode == pOther.mNode; }

		// Logical inequality operator for iterator comparisons.
		inline bool operator!=(const Iterator& pOther) const		{ return !operator==(pOther); }
		inline bool operator!=(const ConstIterator& pOther) const	{ return !operator==(pOther); }

		// Gets the contents of the iterator.
		_TKey& GetKey() const		{ return (mNode->mKey); }
		_TObject& operator*() const	{ return mNode->mObject; }

		// Pre- and postfix increment operators for traversing a list.
		inline Iterator& operator++()
		{
			mNode = mNode->mNext; 
			if (mNode->mNext == 0)	// Set to end.
				mNode = 0;
			return *this; 
		}
		inline Iterator  operator++(int)
		{ 
			ListNode* lTemp = mNode; 
			mNode = mNode->mNext; 
			if (mNode->mNext == 0)	// Set to end.
				mNode = 0;
			return Iterator(lTemp); 
		}

		// Pre- and postfix decrement operators for traversing a list.
		inline Iterator& operator--()		
		{ 
			mNode = mNode->mPrev; 
			if (mNode->mPrev == 0)	// Set to end.
				mNode = 0;
			return *this; 
		}
		inline Iterator operator--(int)
		{ 
			ListNode* lTemp = mNode; 
			mNode = mNode->mPrev;
			if (mNode->mPrev == 0)	// Set to end.
				mNode = 0;
			return Iterator(lTemp); 
		}

		Iterator& operator+=(int pCount)
		{
			for (int i = 0; i < pCount; i++, ++*this);
			return *this;
		}

		Iterator& operator-=(int pCount)
		{
			for (int i = 0; i < pCount; i++, --*this);
			return *this;
		}

		Iterator operator + (int pCount)
		{
			Iterator i(mNode);
			i += pCount;
			return i;
		}

		Iterator operator - (int pCount)
		{
			Iterator i(mNode);
			i -= pCount;
			return i;
		}

	private:

		Iterator(ListNode* pNode) : mNode(pNode){}
		ListNode* mNode;
	};

	class ConstIterator
	{
	public:
		friend class SkipList;

		ConstIterator() : mNode(0){}
		ConstIterator(const ConstIterator& pIterator) : mNode(pIterator.mNode){}

		ConstIterator& operator = (const ConstIterator& pOther)
		{
			mNode = pOther.mNode;
			return *this;
		}

		ConstIterator& operator = (const ConstIterator* pOther)
		{
			mNode = pOther->mNode;
			return *this;
		}

		ConstIterator& operator = (const Iterator& pOther)
		{
			mNode = pOther.mNode;
			return *this;
		}

		ConstIterator& operator = (const Iterator* pOther)
		{
			mNode = pOther->mNode;
			return *this;
		}


		// Logical equality operator for iterator comparisons.
		bool operator==(const Iterator& pOther) const		{ return mNode == pOther.mNode; }
		bool operator==(const ConstIterator& pOther) const	{ return mNode == pOther.mNode; }
		bool operator==(_TObject pOtherObject) const			{ return mNode->mObject == pOtherObject; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& pOther) const		{ return !operator==(pOther); }
		bool operator!=(const ConstIterator& pOther) const	{ return !operator==(pOther); }
		bool operator!=(_TObject pOtherObject) const			{ return !operator==(pOtherObject); }

		// Gets the contents of the iterator.
		_TKey& GetKey() const		{ return (mNode->mKey); }
		_TObject& operator*() const	{ return mNode->mObject; }

		// Pre- and postfix increment operators for traversing a list.
		inline ConstIterator& operator++()
		{
			mNode = mNode->mNext; 
			if (mNode->mNext == 0)	// Set to end.
				mNode = 0;
			return *this; 
		}
		inline ConstIterator  operator++(int)	
		{ 
			ListNode* lTemp = mNode; 
			mNode = mNode->mNext; 
			if (mNode->mNext == 0)	// Set to end.
				mNode = 0;
			return ConstIterator(lTemp); 
		}

		// Pre- and postfix decrement operators for traversing a list.
		inline ConstIterator& operator--()		
		{ 
			mNode = mNode->mPrev; 
			if (mNode->mPrev == 0)	// Set to end.
				mNode = 0;
			return *this; 
		}
		inline ConstIterator  operator--(int)	
		{ 
			ListNode* lTemp = mNode; 
			mNode = mNode->mPrev;
			if (mNode->mPrev == 0)	// Set to end.
				mNode = 0;
			return ConstIterator(lTemp); 
		}

		ConstIterator& operator+=(int pCount)
		{
			for (int i = 0; i < pCount; i++, ++*this);
			return *this;
		}

		ConstIterator& operator-=(int pCount)
		{
			for (int i = 0; i < pCount; i++, --*this);
			return *this;
		}

		ConstIterator operator + (int pCount)
		{
			ConstIterator i(mNode);
			i += pCount;
			return i;
		}

		ConstIterator operator - (int pCount)
		{
			ConstIterator i(mNode);
			i -= pCount;
			return i;
		}

	private:

		ConstIterator(const ListNode* pNode) : mNode(pNode){}
		const ListNode* mNode;
	};

	SkipList();
	virtual ~SkipList();

	void Insert(const _TKey& pKey, const _TObject& pObject);
	void Remove(const _TKey& pKey);
	void Remove(const Iterator& pIter);

	void RemoveAll();

	_TObject FindObject(const _TKey& pKey) const;
	Iterator Find(const _TKey& pKey) const;

	inline Iterator First() const;
	inline Iterator Last() const;
	inline Iterator End() const;

	inline int GetCount() const;
	inline bool IsEmpty() const;

	// Copies the pointer to the node allocator of pList. pList is 
	// considered the "owner" of the allocator, and is responsible of 
	// deleting it. This means that you can't delete the owner list 
	// before the others.
	//
	// You can only call this function on empty lists. If the list isn't
	// empty, the function has no effect.
	inline void ShareNodePool(SkipList* pList);

protected:
private:
	typedef FastAllocator<ListNode> NodeAllocator;

	inline NodeAllocator* GetNodeAllocator();
	inline void SetNodeAllocator(NodeAllocator* pNodeAllocator);

	inline ListNode* NewNode();
	inline void RecycleNode(ListNode* pNode);

	void AddNewTopLevel();
	void RemoveTopLevel();

	ListNode* FindClosestNode(ListNode* pLeft, const _TKey& pKey) const;
	ListNode* SearchLevel(ListNode* pLeft, const _TKey& pKey) const;
	ListNode* Insert(ListNode* pLeft, const _TKey& pKey, const _TObject& pObject);

	inline bool IsEndNode(ListNode* pNode) const;

	bool mAllocatorOwner;
	NodeAllocator* mNodeAllocator;

	ListNode* mTopLeft;
	ListNode* mBottomLeft;
	ListNode* mBottomRight;

	int mCount;
};

TEMPLATE QUAL::SkipList() :
	mAllocatorOwner(true),
	mNodeAllocator(0),
	mTopLeft(0),
	mBottomLeft(0),
	mBottomRight(0),
	mCount(0)
{
	mNodeAllocator = new NodeAllocator();
	AddNewTopLevel();
}

TEMPLATE QUAL::~SkipList()
{
	// Delete all nodes...
	while (mTopLeft != 0)
	{
		RemoveTopLevel();
	}

	if (mAllocatorOwner == true)
	{
		delete mNodeAllocator;
	}
}

TEMPLATE void QUAL::Insert(const _TKey& pKey, const _TObject& pObject)
{
	ListNode* lNodeDown = Insert(mTopLeft, pKey, pObject);

	while (lNodeDown != 0)
	{
		AddNewTopLevel();

		// Create the new node to insert.
		ListNode* lNewNode = NewNode();
		lNewNode->mKey = pKey;
		lNewNode->mObject = pObject;

		// Insert horizontally at the current level.
		lNewNode->mPrev   = mTopLeft;
		lNewNode->mNext   = mTopLeft->mNext;
		mTopLeft->mNext   = lNewNode;
		lNewNode->mNext->mPrev = lNewNode;

		// Link up and down.
		lNewNode->mDown = lNodeDown;
		lNodeDown->mUp  = lNewNode;

		// Randomize with 50% probability if we should add this node
		// at the higher level.
		if (Random::GetRandomNumber()&0x400)	// 50-50: bit is 1 or 0.
		{
			lNodeDown = lNewNode;
		}
		else
		{
			lNodeDown = 0;
		}
	}
}

TEMPLATE typename QUAL::ListNode* QUAL::Insert(ListNode* pLeft, 
					       const _TKey& pKey, 
					       const _TObject& pObject)
{
	ListNode* lNode = SearchLevel(pLeft, pKey);

	if (lNode->mPrev != 0 && lNode->mKey == pKey)
	{
		// The key already exist, so just update the contents (the object).
		while (lNode != 0)
		{
			lNode->mObject = pObject;
			lNode = lNode->mDown;
		}
		return 0;
	}

	bool lCreateNewNode = false;
	ListNode* lNodeDown = 0;

	// If we are at the lowest level.
	if (lNode->mDown == 0)
	{
		lCreateNewNode = true;
		mCount++;
	}
	else
	{
		// Continue searching down.
		lNodeDown = Insert(lNode->mDown, pKey, pObject);
		lCreateNewNode = (lNodeDown != 0);
	}

	if (lCreateNewNode == true)
	{
		// Create the new node to insert.
		ListNode* lNewNode = NewNode();
		lNewNode->mKey = pKey;
		lNewNode->mObject = pObject;

		// Insert horizontally at the current level.
		lNewNode->mPrev   = lNode;
		lNewNode->mNext   = lNode->mNext;
		lNode->mNext      = lNewNode;
		lNewNode->mNext->mPrev = lNewNode;

		if (lNodeDown != 0)
		{
			// Link up and down.
			lNewNode->mDown = lNodeDown;
			lNodeDown->mUp  = lNewNode;
		}

		// Randomize with 50% probability if we should add this node
		// at the higher level.
		if (Random::GetRandomNumber()&0x10)	// 50-50: bit is 1 or 0.
		{
			return lNewNode;
		}
	}

	return 0;
}

TEMPLATE void QUAL::Remove(const _TKey& pKey)
{
	ListNode* lNode = FindClosestNode(mTopLeft, pKey);

	if (IsEndNode(lNode) == false && lNode->mKey == pKey)
	{
		while (lNode != 0)
		{
			lNode->mPrev->mNext = lNode->mNext;
			lNode->mNext->mPrev = lNode->mPrev;

			ListNode* lTemp = lNode;
			lNode = lNode->mDown;
			// Hugge-TODO for JB: This fix is wrong. Try to find your bug again.
			//if (lNode)	// JB-TODO for Hugge: verify that this check is valid.
			{
				RecycleNode(lTemp);
			}
		}

		mCount--;
	}
}

TEMPLATE void QUAL::Remove(const Iterator& pIter)
{
	ListNode* lNode = pIter.mNode;

	if (lNode == 0 || IsEndNode(lNode))
	{
		return;
	}
	
	mCount--;

	while (lNode != 0)
	{
		lNode->mPrev->mNext = lNode->mNext;
		lNode->mNext->mPrev = lNode->mPrev;

		ListNode* lTemp = lNode;
		lNode = lNode->mUp;

		// Hugge: Fixed the bug. TODO for JB: Verify that it works.
		//if (lNode)	// JB-TODO for Hugge: verify that this code is correct. At least it doesn't crash anymore. :)
		{
			RecycleNode(lTemp);
		}
	}
}

TEMPLATE void QUAL::RemoveAll()
{
	// Delete all nodes...
	while (mTopLeft != 0)
	{
		RemoveTopLevel();
	}
	mCount = 0;

	AddNewTopLevel();
}

TEMPLATE _TObject QUAL::FindObject(const _TKey& pKey) const
{
	ListNode* lNode = FindClosestNode(mTopLeft, pKey);

	if (lNode->mKey == pKey)
	{
		return lNode->mObject;
	}
	else
	{
		return 0;
	}
}

TEMPLATE typename QUAL::Iterator QUAL::Find(const _TKey& pKey) const
{
	ListNode* lNode = FindClosestNode(mTopLeft, pKey);

	if (IsEndNode(lNode) == true)
	{
		return End();
	}

	if (lNode->mKey == pKey)
	{
		return Iterator(lNode);
	}
	else
	{
		return End();
	}
}

TEMPLATE typename QUAL::Iterator QUAL::First() const
{
	if (IsEndNode(mBottomLeft->mNext) == true)
	{
		return End();
	}
	return Iterator(mBottomLeft->mNext);
}

TEMPLATE typename QUAL::Iterator QUAL::Last() const
{
	if (IsEndNode(mBottomRight->mPrev) == true)
	{
		return End();
	}
	return Iterator(mBottomRight->mPrev);
}

TEMPLATE typename QUAL::Iterator QUAL::End() const
{
	return Iterator(0);
}

TEMPLATE int QUAL::GetCount() const
{
	return mCount;
}

TEMPLATE bool QUAL::IsEmpty() const
{
	return (mCount == 0);
}

TEMPLATE void QUAL::AddNewTopLevel()
{
	ListNode* lTopLeft  = NewNode();
	ListNode* lTopRight = NewNode();

	lTopLeft->mNext  = lTopRight;
	lTopRight->mPrev = lTopLeft;

	if (mTopLeft != 0)
	{
		lTopLeft->mDown = mTopLeft;
		mTopLeft->mUp   = lTopLeft;
	
		// Find the rightmost node.
		ListNode* lTemp = mTopLeft;
		while (lTemp->mNext != 0)
		{
			lTemp = lTemp->mNext;
		}

		lTopRight->mDown = lTemp;
		lTemp->mUp = lTopRight;
	}

	mTopLeft = lTopLeft;

	if (mBottomLeft == 0 || mBottomRight == 0)
	{
		mBottomLeft  = lTopLeft;
		mBottomRight = lTopRight;
	}
}

TEMPLATE void QUAL::RemoveTopLevel()
{
	if (mTopLeft != 0)
	{
		ListNode* lNode = mTopLeft;
		mTopLeft = mTopLeft->mDown;

		while (lNode != 0)
		{
			if (lNode->mDown != 0)
			{
				lNode->mDown->mUp = 0;
			}

			ListNode* lTemp = lNode;
			lNode = lNode->mNext;
			RecycleNode(lTemp);
		}
	}

	if (mTopLeft == 0)
	{
		mBottomLeft  = 0;
		mBottomRight = 0;
	}
}

TEMPLATE typename QUAL::ListNode* QUAL::FindClosestNode(ListNode* pLeft, const _TKey& pKey) const
{
	// Search this level...
	ListNode* lNode = SearchLevel(pLeft, pKey);

	if (lNode->mKey == pKey ||
	   lNode->mDown == 0)
	{
		// We have found the node.
		return lNode;
	}

	if (lNode->mDown != 0)
	{
		// Step back and down one level, and continue searching.
		return FindClosestNode(lNode->mDown, pKey);
	}
	else
	{
		return lNode;
	}
}

TEMPLATE typename QUAL::ListNode* QUAL::SearchLevel(ListNode* pLeft, const _TKey& pKey) const
{
	ListNode* lCurrentNode = pLeft;

	while (lCurrentNode->mNext != 0 &&
		  (lCurrentNode->mPrev == 0 ||
		  lCurrentNode->mKey < pKey))
	{
		lCurrentNode = lCurrentNode->mNext;
	}

	// Have we found the node?
	if (lCurrentNode->mKey == pKey)
	{
		return lCurrentNode;
	}
	else
	{
		return lCurrentNode->mPrev;
	}
}

TEMPLATE void QUAL::ShareNodePool(SkipList* pList)
{
	if (IsEmpty() == true)
	{
		SetNodeAllocator(pList->GetNodeAllocator());
	}
}

TEMPLATE typename QUAL::NodeAllocator* QUAL::GetNodeAllocator()
{
	return mNodeAllocator;
}

TEMPLATE void QUAL::SetNodeAllocator(NodeAllocator* pNodeAllocator)
{
	if (mNodeAllocator != 0)
	{
		delete mNodeAllocator;
	}

	mNodeAllocator = pNodeAllocator;
	mAllocatorOwner = false;
}

TEMPLATE typename QUAL::ListNode* QUAL::NewNode()
{
	ListNode* lNode = mNodeAllocator->Alloc();
	lNode->Init();
	return lNode;
}

TEMPLATE void QUAL::RecycleNode(ListNode* pNode)
{
	mNodeAllocator->Free(pNode);
}

TEMPLATE bool QUAL::IsEndNode(ListNode* pNode) const
{
	return ((pNode->mNext == 0) || (pNode->mPrev == 0));
}

} // End namespace.

#undef TEMPLATE
#undef QUAL

#endif
*/