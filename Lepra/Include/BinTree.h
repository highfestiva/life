
// Author: Alexander Hugestrand
// Copyright (c) 2002-2006, Righteous Games



#pragma once

#include "LepraTypes.h"
#include "FastAllocator.h"



#define TEMPLATE template<class _TKey, class _TObject>
#define QUAL BinTree<_TKey, _TObject>



namespace Lepra
{



template<class _TKey, class _TObject>
class BinTree
{
public:
	class Node;
private:
	typedef FastAllocator<Node> NodeAllocator;
public:
	class Node
	{
	public:

		Node();
		Node(_TKey pKey, _TObject pObject, Node* pParent);
		~Node();

		void Init(_TKey pKey, _TObject pObject, Node* pParent);
		void Destroy(NodeAllocator* pAllocator);

		Node* Insert(Node* pNode);
		Node* Find(_TKey pKey);
		const Node* GetLeftMostNode() const;
		Node* GetLeftMostNode();
		const Node* GetRightMostNode() const;
		Node* GetRightMostNode();

		void Relink(Node* pOldNode, Node* pNewNode);
		Node* Unlink();

		Node*    mParent;
		Node*    mLeft;
		Node*    mRight;

		_TKey    mKey;
		_TObject mObject;
	};

public:

	class ConstIterator;

	class Iterator
	{
	public:
		friend class BinTree;

		Iterator() : mNode(0), mPrev(0){}
		Iterator(const Iterator& pIterator) : mNode(pIterator.mNode), mPrev(pIterator.mPrev){}

		Iterator& operator = (const Iterator& pOther)
		{
			mNode   = pOther.mNode;
			mPrev   = pOther.mPrev;

			return *this;
		}

		Iterator& operator = (const Iterator* pOther)
		{
			mNode   = pOther->mNode;
			mPrev   = pOther->mPrev;

			return *this;
		}

		// Logical equality operator for iterator comparisons.
		int  operator==(const Iterator& pOther) const		{ return mNode == pOther.mNode; }
		bool   operator==(const ConstIterator& pOther) const	{ return mNode == pOther.mNode; }

		// Logical inequality operator for iterator comparisons.
		int  operator!=(const Iterator& pOther) const		{ return !operator==(pOther); }
		bool   operator!=(const ConstIterator& pOther) const	{ return !operator==(pOther); }

		// Gets the contents of the iterator.
		_TObject& operator*() const	{ return mNode->mObject; }

		// Pre- and postfix increment operators for traversing a tree.
		Iterator& operator++()
		{ 
			if (mNode == 0)
			{
				return *this;
			}

			if (mPrev == 0 || mPrev == mNode->mLeft)
			{
				// Try to go right.
				if (mNode->mRight != 0)
				{
					mNode = mNode->mRight->GetLeftMostNode();
					mPrev = 0;
				}
				else
				{
					// Go up as long as we are coming from the right.
					do
					{
						mPrev = mNode;
						mNode = mNode->mParent;
					} while(mNode != 0 && mPrev == mNode->mRight);
				}
			}
			else
			{
				// Go up as long as we are coming from the right.
				do
				{
					mPrev = mNode;
					mNode = mNode->mParent;
				} while(mNode != 0 && mPrev == mNode->mRight);
			}

			return *this; 
		}
		Iterator  operator++(int)	
		{ 
			Iterator i(mNode, mPrev); ++(*this); return i;
		}

		// Pre- and postfix decrement operators for traversing a tree.
		Iterator& operator--()		
		{ 
			if (mNode == 0)
			{
				return *this;
			}

			if (mPrev == 0 || mPrev == mNode->mRight)
			{
				// Try to go left.
				if (mNode->mLeft != 0)
				{
					mNode = mNode->mLeft->GetRightMostNode();
					mPrev = 0;
				}
				else
				{
					// Go up as long as we are coming from the left.
					do
					{
						mPrev = mNode;
						mNode = mNode->mParent;
					} while(mNode != 0 && mPrev == mNode->mLeft);
				}
			}
			else
			{
				// Go up as long as we are coming from the left.
				do
				{
					mPrev = mNode;
					mNode = mNode->mParent;
				} while(mNode != 0 && mPrev == mNode->mLeft);
			}

			return *this; 
		}
		Iterator  operator--(int)	
		{ 
			Iterator i(mNode, mPrev); --(*this); return i; 
		}

	private:

		Iterator(Node* pNode) : mNode(pNode), mPrev(0){}
		Iterator(Node* pNode, Node* pPrev) : mNode(pNode), mPrev(pPrev){}

		Node* mNode;
		Node* mPrev;
	};

	class ConstIterator
	{
	public:
		friend class BinTree;

		ConstIterator() : mNode(0){}
		ConstIterator(const Iterator& pIterator) : mNode(pIterator.mNode){}

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
		bool operator==(const ConstIterator& pOther) const		{ return mNode == pOther.mNode; }
		bool operator==(const Iterator& pOther) const			{ return mNode == pOther.mNode; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const ConstIterator& pOther) const		{ return !operator==(pOther); }
		bool operator!=(const Iterator& pOther) const			{ return !operator==(pOther); }

		// Gets the contents of the iterator.
		_TObject& operator*() const	{ return mNode->mObject; }

		ConstIterator& operator++()
		{ 
			if (mNode == 0)
			{
				return *this;
			}

			if (mPrev == 0 || mPrev == mNode->mLeft)
			{
				// Try to go right.
				if (mNode->mRight != 0)
				{
					mNode = mNode->mRight->GetLeftMostNode();
					mPrev = 0;
				}
				else
				{
					// Go up as long as we are coming from the right.
					do
					{
						mPrev = mNode;
						mNode = mNode->mParent;
					} while(mNode != 0 && mPrev == mNode->mRight);
				}
			}
			else
			{
				// Go up as long as we are coming from the right.
				do
				{
					mPrev = mNode;
					mNode = mNode->mParent;
				} while(mNode != 0 && mPrev == mNode->mRight);
			}

			return *this; 
		}
		ConstIterator  operator++(int)	
		{ 
			ConstIterator i(mNode, mPrev); ++(*this); return i;
		}

		// Pre- and postfix decrement operators for traversing a tree.
		ConstIterator& operator--()		
		{ 
			if (mNode == 0)
			{
				return *this;
			}

			if (mPrev == 0 || mPrev == mNode->mRight)
			{
				// Try to go left.
				if (mNode->mLeft != 0)
				{
					mNode = mNode->mLeft->GetRightMostNode();
					mPrev = 0;
				}
				else
				{
					// Go up as long as we are coming from the left.
					do
					{
						mPrev = mNode;
						mNode = mNode->mParent;
					} while(mNode != 0 && mPrev == mNode->mLeft);
				}
			}
			else
			{
				// Go up as long as we are coming from the left.
				do
				{
					mPrev = mNode;
					mNode = mNode->mParent;
				} while(mNode != 0 && mPrev == mNode->mLeft);
			}

			return *this; 
		}
		ConstIterator  operator--(int)	
		{ 
			ConstIterator i(mNode, mPrev); --(*this); return i; 
		}

	private:

		ConstIterator(Node* pNode) : mNode(pNode){}
		ConstIterator(Node* pNode, Node* pPrev) : mNode(pNode), mPrev(pPrev){}

		const Node* mNode;
		const Node* mPrev;
	};

	inline BinTree();
	inline ~BinTree();

	void Insert(_TKey pKey, _TObject pObject);

	void Remove(const Iterator& pItem);
	void Remove(_TKey pKey);
	void RemoveAll();

	// Will rearrange the nodes in the tree to make it perfectly balanced.
	void Balance();

	inline Iterator Find(_TKey pKey);
	inline Iterator FindN(int pNum);
	inline bool Exists(_TKey pKey);

	inline Iterator First();
	inline Iterator Last();
	inline Iterator End();

	inline ConstIterator First() const;
	inline ConstIterator Last() const;
	inline ConstIterator End() const;

	inline int GetCount() const;
	inline bool IsEmpty() const;

	// Copies the pointer to the node allocator of pTree. pTree is 
	// considered the "owner" of the allocator, and is responsible of 
	// deleting it. This means that you can't delete the owner tree 
	// before the others.
	//
	// You can only call this function on empty trees. If the tree isn't
	// empty, the function has no effect.
	inline void ShareNodePool(BinTree* pTree);

private:
	inline NodeAllocator* GetNodeAllocator();
	inline void SetNodeAllocator(NodeAllocator* pNodeAllocator);

	inline Node* NewNode(_TKey pKey, _TObject pObject, Node* pParent);
	inline void RecycleNode(Node* pNode);

	void Balance(int pStart, int pEnd, Node* pNewTopNode);

	bool mAllocatorOwner;
	NodeAllocator* mNodeAllocator;

	int mCount;
	Node* mTopNode;
};

TEMPLATE QUAL::BinTree() :
	mAllocatorOwner(true),
	mCount(0),
	mTopNode(0)
{
	mNodeAllocator = new NodeAllocator;
}

TEMPLATE QUAL::~BinTree()
{
	if (mTopNode != 0)
	{
		RecycleNode(mTopNode);
	}

	if (mAllocatorOwner == true)
	{
		delete mNodeAllocator;
	}
}

TEMPLATE int QUAL::GetCount() const
{
	return mCount;
}

TEMPLATE typename QUAL::Iterator QUAL::First()
{
	if (mTopNode == 0)
	{
		return End();
	}

	Iterator lIter(mTopNode->GetLeftMostNode());
	return lIter;
}

TEMPLATE typename QUAL::Iterator QUAL::Last()
{
	if (mTopNode == 0)
	{
		return End();
	}

	// Keep going right until you reach the bottom.
	Iterator lIter(mTopNode->GetRightMostNode());
	return lIter;
}

TEMPLATE typename QUAL::Iterator QUAL::End()
{
	return Iterator(0);
}

TEMPLATE typename QUAL::ConstIterator QUAL::First() const
{
	if (mTopNode == 0)
	{
		return End();
	}

	// Keep going left until you reach the bottom.
	ConstIterator lIter(mTopNode);
	while (lIter.mNode->mLeft != 0)
	{
		lIter.mNode = lIter.mNode->mLeft;
	}
	
	return lIter;
}

TEMPLATE typename QUAL::ConstIterator QUAL::Last() const
{
	if (mTopNode == 0)
	{
		return End();
	}

	// Keep going right until you reach the bottom.
	ConstIterator lIter(mTopNode);
	while (lIter.mNode->mRight != 0)
	{
		lIter.mNode = lIter.mNode->mRight;
	}

	return lIter;
}

TEMPLATE typename QUAL::ConstIterator QUAL::End() const
{
	return ConstIterator(0);
}

TEMPLATE bool QUAL::IsEmpty() const
{
	return (mTopNode == 0);
}

TEMPLATE void QUAL::Insert(_TKey pKey, _TObject pObject)
{
	if (mTopNode == 0)
	{
		mTopNode = NewNode(pKey, pObject, 0);
		mCount = 1;
	}
	else
	{
		Node* lNode = NewNode(pKey, pObject, 0);
		if (mTopNode->Insert(lNode) != 0)
		{
			mCount++;
		}
		else
		{
			// The node wasn't inserted since the key value already exist.
			RecycleNode(lNode);
		}
	}
}

TEMPLATE void QUAL::Remove(const Iterator& pIter)
{
	Node* lNode = pIter.mNode;

	if (lNode != 0)
	{
		if (lNode == mTopNode)
		{
			mTopNode = lNode->Unlink();
		}
		else
		{
			lNode->Unlink();
		}

		lNode->mParent = 0;
		lNode->mLeft   = 0;
		lNode->mRight  = 0;
		RecycleNode(lNode);

		mCount--;
	}
}

TEMPLATE void QUAL::Remove(_TKey pKey)
{
	Node* lNode = mTopNode->Find(pKey);

	if (lNode != 0)
	{
		if (lNode == mTopNode)
		{
			mTopNode = lNode->Unlink();
		}
		else
		{
			lNode->Unlink();
		}

		lNode->mParent = 0;
		lNode->mLeft   = 0;
		lNode->mRight  = 0;
		RecycleNode(lNode);

		mCount--;
	}
}

TEMPLATE void QUAL::RemoveAll()
{
	mTopNode->Destroy(mNodeAllocator);
	mTopNode = 0;
	mCount = 0;
}

TEMPLATE typename QUAL::Iterator QUAL::Find(_TKey pKey)
{
	if (mTopNode == 0)
	{
		return End();
	}

	Node* lNode = mTopNode->Find(pKey);

	if (lNode == 0)
	{
		return End();
	}

	return Iterator(lNode);
}

TEMPLATE typename QUAL::Iterator QUAL::FindN(int pNum)
{
	Iterator lIter = First();
	for (int i = 0; i < pNum && lIter != End(); ++i, ++lIter){}
	return lIter;
}

TEMPLATE bool QUAL::Exists(_TKey pKey)
{
	return (Find(pKey) != End());
}

TEMPLATE void QUAL::Balance()
{
	if (mCount <= 2)
	{
		return;
	}

	int lCount = mCount;

	int lMiddle = mCount / 2;
	Iterator lIter = FindN(lMiddle);
	Node* lTopNode = NewNode(lIter.mNode->mKey,
							   lIter.mNode->mObject,
							   0);
	Remove(lIter);

	Balance(0, mCount, lTopNode);
	RemoveAll();
	mTopNode = lTopNode;
	mCount = lCount;
}

TEMPLATE void QUAL::Balance(int pStart, int pEnd, Node* pNewTopNode)
{
	if (pEnd == pStart)
	{
		return;
	}

	int lMiddle = (pStart + pEnd) / 2;
	
	Iterator lIter = FindN(lMiddle);
	Node* lNode = NewNode(lIter.mNode->mKey,
							lIter.mNode->mObject,
							0);
	pNewTopNode->Insert(lNode);

	Balance(pStart, lMiddle, pNewTopNode);
	Balance(lMiddle + 1, pEnd, pNewTopNode);
}

TEMPLATE void QUAL::ShareNodePool(BinTree* pTree)
{
	if (IsEmpty() == true)
	{
		SetNodeAllocator(pTree->GetNodeAllocator());
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

TEMPLATE typename QUAL::Node* QUAL::NewNode(_TKey pKey, _TObject pObject, Node* pParent)
{
	Node* lNode = mNodeAllocator->Alloc();
	lNode->Init(pKey, pObject, pParent);
	return lNode;
}

TEMPLATE void QUAL::RecycleNode(Node* pNode)
{
	mNodeAllocator->Free(pNode);
}












//
// class Node
//

TEMPLATE QUAL::Node::Node() : 
	mParent(0), 
	mLeft(0), 
	mRight(0) 
{
}

TEMPLATE QUAL::Node::Node(_TKey pKey, _TObject pObject, Node* pParent) : 
	mParent(pParent), 
	mLeft(0), 
	mRight(0), 
	mKey(pKey),
	mObject(pObject)
{
}

TEMPLATE QUAL::Node::~Node()
{
}

TEMPLATE void QUAL::Node::Init(_TKey pKey, _TObject pObject, Node* pParent)
{
	mParent = pParent;
	mLeft = 0;
	mRight = 0;
	mKey = pKey;
	mObject = pObject;
}

TEMPLATE void QUAL::Node::Destroy(NodeAllocator* pAllocator)
{
	if (mLeft != 0)
	{
		mLeft->Destroy(pAllocator);
		mLeft = 0;
	}
	if (mRight != 0)
	{
		mRight->Destroy(pAllocator);
		mRight = 0;
	}

	pAllocator->Free(this);
}

TEMPLATE typename QUAL::Node* QUAL::Node::Insert(Node* pNode)
{
	if (mKey == pNode->mKey)
	{
		mObject = pNode->mObject;

		// No new node was created.
		return 0;
	}

	if (pNode->mKey < mKey)
	{
		if (mLeft == 0)
		{
			mLeft = pNode;
			pNode->mParent = this;
			return mLeft;
		}
		else
		{
			return mLeft->Insert(pNode);
		}
	}
	else
	{
		if (mRight == 0)
		{
			mRight = pNode;
			pNode->mParent = this;
			return mRight;
		}
		else
		{
			return mRight->Insert(pNode);
		}
	}
}

TEMPLATE typename QUAL::Node* QUAL::Node::Find(_TKey pKey)
{
	if (mKey == pKey)
	{
		return this;
	}
	else if(pKey < mKey)
	{
		if (mLeft == 0)
		{
			return 0;
		}

		return mLeft->Find(pKey);
	}
	else
	{
		if (mRight == 0)
		{
			return 0;
		}

		return mRight->Find(pKey);
	}
}

TEMPLATE const typename QUAL::Node* QUAL::Node::GetLeftMostNode() const
{
	if (mLeft == 0)
	{
		return this;
	}
	else
	{
		return mLeft->GetLeftMostNode();
	}
}

TEMPLATE typename QUAL::Node* QUAL::Node::GetLeftMostNode()
{
	if (mLeft == 0)
	{
		return this;
	}
	else
	{
		return mLeft->GetLeftMostNode();
	}
}

TEMPLATE const typename QUAL::Node* QUAL::Node::GetRightMostNode() const
{
	if (mRight == 0)
	{
		return this;
	}
	else
	{
		return mRight->GetRightMostNode();
	}
}

TEMPLATE typename QUAL::Node* QUAL::Node::GetRightMostNode()
{
	if (mRight == 0)
	{
		return this;
	}
	else
	{
		return mRight->GetRightMostNode();
	}
}

TEMPLATE void QUAL::Node::Relink(Node* pOldNode, Node* pNewNode)
{
	if (mLeft == pOldNode)
	{
		mLeft = pNewNode;
	}
	else if(mRight == pOldNode)
	{
		mRight = pNewNode;
	}

	if (pNewNode != 0)
	{
		pNewNode->mParent = this;
	}
}

TEMPLATE typename QUAL::Node* QUAL::Node::Unlink()
{
	if (mLeft == 0 && mRight == 0)
	{
		if (mParent != 0)
		{
			mParent->Relink(this, 0);
		}
		return 0;
	}
	else if(mLeft == 0 && mRight != 0)
	{
		if (mParent != 0)
		{
			mParent->Relink(this, mRight);
		}
		else
		{
			mRight->mParent = 0;
		}

		return mRight;
	}
	else if(mLeft != 0 && mRight == 0)
	{
		if (mParent != 0)
		{
			mParent->Relink(this, mLeft);
		}
		else
		{
			mLeft->mParent = 0;
		}

		return mLeft;
	}
	else
	{
		// This algorithm will keep the tree somewhat balanced,
		// if it's balanced already.

		Node* lRL = mRight->GetLeftMostNode();

		lRL->mLeft = mLeft;
		mLeft->mParent = lRL;


/*
		// This code is the original algorithm. If you uncomment this,
		// you should remove all the code below.
		if (mParent != 0)
		{
			mParent->Relink(this, mRight);
		}
		mRight->mParent = mParent;
*/



		if (lRL != mRight)
		{
			lRL->mParent->Relink(lRL, 0);
		}

		lRL->mParent = mParent;

		if (mParent != 0)
		{
			mParent->Relink(this, lRL);
		}

		if (lRL != mRight)
		{
			Node* lRR = lRL->GetRightMostNode();
			lRR->mRight = mRight;
			mRight->mParent = lRR;
		}

		return mRight;
	}
}



}



#undef TEMPLATE
#undef QUAL
