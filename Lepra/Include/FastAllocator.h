
// Author:      Jonas Byström
// Copyright (c) Pixel Doctrine

// NOTES:

// This class is only intended for performance gain when frequently 
// allocating objects of the same type.

// Warning: When deleting an object of this class, all memory that's
// allocated using it will be freed!

// Description of the algorithm:

// This class first allocates an array of chunk pointers, with room
// for 10 chunks by default. The first time you call Alloc(), the first
// chunk will be initialized with 8 objects. When you've allocated all
// objects, another chunk will be allocated with exponentially more 
// objects (27 objects in the next level). How many objects are 
// allocated per chunk is defined in the function GetNumNodes().

// This procedure goes on until all chunks are used, at which point
// the FastAllocator allocates even more chunks, and so on.

// The chunks are never deleted, which means that at some point in the
// program excecution, the	maximum peak will be reached and no more
// chunks will ever be allocated. At that point, the allocator will
// be performing at its best.

// How many chunks that will be allocated at first, and at every 
// reallocation of new chunks, can be configured through the parameter
// pNumChunksPerAlloc in the constructor.



#pragma once

#include "LepraTypes.h"
#include <stddef.h>
#ifndef LEPRA_WINDOWS
#include <stdint.h>
#endif // Windows

#define FA_TEMPLATE template<class _Obj, class _LockType>
#define FA_QUAL FastAllocator<_Obj, _LockType>



namespace Lepra
{



class DummyLock
{
public:
	inline void Acquire()    {}
	inline void TryAcquire() {}
	inline void Release() {}
};

template<class _Obj, class _LockType = DummyLock>
class FastAllocator
{
public:
	
	FastAllocator(int pNumChunksPerAlloc = 10);
	FastAllocator(FastAllocator& pTakeOver);
	virtual ~FastAllocator();

	_Obj* Alloc();
	void  Free(_Obj* pObject);
	void  FreeAll();

protected:
private:

	void AllocateMoreChunks();
	void AllocateNextChunk();
	void ResetChunk(int pChunk);

	// Returns the number of nodes in chunk pChunk.
	inline int GetNumNodes(int pChunk)
	{
		int x = pChunk + 2;
		return x * x;
	}

	class Node
	{
	public:

		Node() :
			mNextNode(0)
		{
		}

		inline static Node* GetNode(_Obj* pObject)
		{
			// Doing some magic to find the correct Node address.
			uint8* lRawNode = (uint8*)pObject;
			Node* lNode = (Node*)0;
			lRawNode -= (intptr_t)(&lNode->mObject);
			//lRawNode -= offsetof(Node, mObject);
			return (Node*)lRawNode;
		}

		Node* mNextNode;
		_Obj  mObject;
	};

	_LockType mLock;

	int mNumChunks;
	int mCurrentChunk;
	int mNumChunksPerAlloc;
	Node** mChunk;
	Node* mFirstFreeNode;
};

FA_TEMPLATE FA_QUAL::FastAllocator(int pNumChunksPerAlloc) :
	mNumChunks(0),
	mCurrentChunk(-1),
	mNumChunksPerAlloc(pNumChunksPerAlloc),
	mChunk(0),
	mFirstFreeNode(0)
{
	AllocateMoreChunks();
}

FA_TEMPLATE FA_QUAL::FastAllocator(FastAllocator& pTakeOver) :
	mNumChunks(pTakeOver.mNumChunks),
	mCurrentChunk(pTakeOver.mCurrentChunk),
	mNumChunksPerAlloc(pTakeOver.mNumChunksPerAlloc),
	mChunk(pTakeOver.mChunk),
	mFirstFreeNode(pTakeOver.mFirstFreeNode)
{
	// Reset pTakeOver.
	pTakeOver.mNumChunks = 0;
	pTakeOver.mCurrentChunk = -1;
	pTakeOver.mChunk = 0;
	pTakeOver.mFirstFreeNode = 0;
	pTakeOver.AllocateMoreChunks();
}

FA_TEMPLATE FA_QUAL::~FastAllocator()
{
	if (mChunk != 0)
	{
//		FreeAll();

		for (int i = 0; i < mNumChunks; i++)
		{
			if (mChunk[i] != 0)
			{
				delete[] mChunk[i];
			}
		}

		delete[] mChunk;
		mChunk = 0;
	}
}

FA_TEMPLATE _Obj* FA_QUAL::Alloc()
{
	mLock.Acquire();

	if (!mFirstFreeNode)
	{
		AllocateNextChunk();
	}

	_Obj* lObject = &mFirstFreeNode->mObject;
	mFirstFreeNode = mFirstFreeNode->mNextNode;

	mLock.Release();

	return lObject;
}

FA_TEMPLATE void FA_QUAL::Free(_Obj* pObject)
{
	mLock.Acquire();

	Node* lNode = Node::GetNode(pObject);
	lNode->mNextNode = mFirstFreeNode;
	mFirstFreeNode = lNode;

	mLock.Release();
}

FA_TEMPLATE void FA_QUAL::FreeAll()
{
	mLock.Acquire();

	for (int i = 0; i < mNumChunks; i++)
	{
		ResetChunk(i);
	}

	if (mCurrentChunk > 0)
	{
		mCurrentChunk = 0;
		mFirstFreeNode = mChunk[0];
	}

	mLock.Release();
}

FA_TEMPLATE void FA_QUAL::AllocateMoreChunks()
{
	Node** lChunk = new Node*[mNumChunks + mNumChunksPerAlloc];

	int i;
	for (i = 0; i < mNumChunks; i++)
	{
		lChunk[i] = mChunk[i];
	}

	for (; i < mNumChunks + mNumChunksPerAlloc; i++)
	{
		lChunk[i] = 0;
	}

	if (mChunk != 0)
	{
		delete[] mChunk;
	}

	mChunk = lChunk;
	mNumChunks += mNumChunksPerAlloc;
}

FA_TEMPLATE void FA_QUAL::AllocateNextChunk()
{
	mCurrentChunk++;
	if (mCurrentChunk >= mNumChunks)
	{
		AllocateMoreChunks();
	}

	// We need to check wether the chunk is already allocated or not,
	// because FreeAll() doesn't free this memory.
	if (mChunk[mCurrentChunk] == 0)
	{
		mChunk[mCurrentChunk] = new Node[GetNumNodes(mCurrentChunk)];
	}

	// Since this function is only called if mFirstFreeNode
	// is null, we can safely set it without further checking.
	mFirstFreeNode = mChunk[mCurrentChunk];

	ResetChunk(mCurrentChunk);
}

FA_TEMPLATE void FA_QUAL::ResetChunk(int pChunk)
{
	Node* lNode = mChunk[pChunk];

	// Check needed, since we may call this function on a chunk
	// with no allocated nodes.
	if (lNode)
	{
		int lLoopMax = GetNumNodes(pChunk) - 1;
		int i;
		for (i = 0; i < lLoopMax; i++)
		{
			lNode[i].mNextNode = &lNode[i + 1];
		}
		lNode[i].mNextNode = 0;
	}
}



}



#undef FA_TEMPLATE
#undef FA_QUAL
