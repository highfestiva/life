/*
	Class:  DynamicArray
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

/*
JB: use std::vector instead.

#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include "LepraTypes.h"

#define TEMPLATE template<class T>
#define QUAL DynamicArray<T>
#define CHUNKSIZE_EXP 5					// 2^CHUNKSIZE_EXP = CHUNKSIZE
#define CHUNKSIZE (1 << CHUNKSIZE_EXP)

namespace Lepra
{

TEMPLATE
class DynamicArray
{
	friend class Iterator;
private:
	class Chunk
	{
	public:

		Chunk() :
			mObjectCount(0)
		{
		}

		Chunk(const Chunk& pChunk) :
			mObjectCount(pChunk.mObjectCount)
		{
			for (int i = 0; i < mObjectCount; i++)
			{
				mTArray[i] = pChunk.mTArray[i];
			}
		}

		T mTArray[CHUNKSIZE];
		int mObjectCount;
	};
public:

	class Iterator
	{
	public:
		friend class DynamicArray<T>;

		Iterator() : 
			mChunk(0),
			mNumChunks(0),
			mCurrentChunk(0),
			mCurrentIndex(0),
			mCurrentChunk(0),
			mNode(0)
		{
		}

		Iterator(const Iterator& pOther) :
			mChunk(pOther.mChunk),
			mNumChunks(pOther.mNumChunks),
			mCurrentChunk(pOther.mCurrentChunk),
			mCurrentIndex(pOther.mCurrentIndex),
			mCurrentChunk(pOther.mCurrentChunk),
			mNode(pOther.mNode)
		{
		}

		Iterator& operator = (const Iterator& pOther)
		{
			mChunk = pOther.mChunk;
			mNumChunks = pOther.mNumChunks;
			mCurrentChunk = pOther.mCurrentChunk;
			mCurrentIndex = pOther.mCurrentIndex;
			mCurrentChunk = pOther.mCurrentChunk;
			mNode = pOther.mNode;

			return *this;
		}

		Iterator& operator = (const Iterator* pOther)
		{
			mChunk = pOther->mChunk;
			mNumChunks = pOther->mNumChunks;
			mCurrentChunk = pOther->mCurrentChunk;
			mCurrentIndex = pOther->mCurrentIndex;
			mCurrentChunk = pOther->mCurrentChunk;
			mNode = pOther->mNode;

			return *this;
		}

		// Logical equality operator for iterator comparisons.
		int operator==(const Iterator& pOther) const      { return pOther.mNode == mNode; }
		int operator==(T pOtherItem) const                { return pOtherItem == *mNode; }

		// Logical inequality operator for iterator comparisons.
		int operator!=(const Iterator& pOther) const		{ return !operator==(pOther); }
		int operator!=(T pOtherItem) const			{ return !operator==(pOtherItem); }

		// Gets the contents of the iterator.
		T& operator*() const	{ return *mNode; }

		// Pre- and postfix increment operators for traversing a list.
		Iterator& operator++()
		{
			if (mNode == 0)
			{
				// This is an End iterator.
				return *this;
			}

			mCurrentIndex++;

			if (mCurrentIndex < mCurrentChunk->mObjectCount)
			{
				mNode = &mCurrentChunk->mTArray[mCurrentIndex];
			}
			else if(mCurrentIndex < CHUNKSIZE)
			{
				// This is the end of the array.
				mNode = 0;
			}
			else
			{
				mCurrentIndex = 0;
				mCurrentChunk++;

				if (mCurrentChunk < (int)*mNumChunks)
				{
					mCurrentChunk = mChunk[mCurrentChunk];
					mNode = &mCurrentChunk->mTArray[mCurrentIndex];
				}
				else
				{
					mCurrentChunk = 0;
					mNode = 0;
					mCurrentIndex = CHUNKSIZE;
					return *this;
				}
			}

			return *this;
		}

		Iterator  operator++(int)
		{
			if (mNode == 0)
			{
				// This is an End iterator.
				return *this;
			}

			Iterator i(*this); 

			mCurrentIndex++;

			if (mCurrentIndex < mCurrentChunk->mObjectCount)
			{
				mNode = &mCurrentChunk->mTArray[mCurrentIndex];
			}
			else if(mCurrentIndex < CHUNKSIZE)
			{
				// This is the end of the array.
				mNode = 0;
			}
			else
			{
				mCurrentIndex = 0;
				mCurrentChunk++;

				if (mCurrentChunk < *mNumChunks)
				{
					mCurrentChunk = mChunk[mCurrentChunk];
					mNode = &mCurrentChunk->mTArray[mCurrentIndex];
				}
				else
				{
					mCurrentChunk = 0;
					mNode = 0;
					mCurrentIndex = CHUNKSIZE;
					return i;
				}
			}
			
			return i; 
		}

		// Pre- and postfix decrement operators for traversing a list.
		Iterator& operator--()
		{
			mCurrentIndex--;

			if (mCurrentIndex < 0)
			{
				mCurrentIndex = CHUNKSIZE - 1;
				mCurrentChunk--;

				if (mCurrentChunk >= 0)
				{
					mCurrentChunk = mChunk[mCurrentChunk];
				}
				else
				{
					mCurrentChunk = 0;
					mNode = 0;
					mCurrentIndex = -1;
					return *this;
				}
			}

			mNode = &mCurrentChunk->mTArray[mCurrentIndex];

			return *this;
		}

		Iterator  operator--(int)
		{
			Iterator i(*this);

			mCurrentIndex--;

			if (mCurrentIndex < 0)
			{
				mCurrentIndex = CHUNKSIZE - 1;
				mCurrentChunk--;

				if (mCurrentChunk >= 0)
				{
					mCurrentChunk = mChunk[mCurrentChunk];
				}
				else
				{
					mCurrentChunk = 0;
					mNode = 0;
					mCurrentIndex = -1;
					return *this;
				}
			}

			mNode = &mCurrentChunk->mTArray[mCurrentIndex];

			return i;
		}

	private:
		Iterator(Chunk** pChunk,
				 const unsigned* pNumChunks,
				 int pChunk,
				 int pIndex) :
			mChunk(pChunk),
			mNumChunks(pNumChunks),
			mCurrentChunk(pChunk),
			mCurrentIndex(pIndex),
			mCurrentChunk(0),
			mNode(0)
		{
			if (mChunk != 0)
			{
				mCurrentChunk = mChunk[pChunk];
			}

			if (mCurrentChunk != 0)
			{
				mNode = &mCurrentChunk->mTArray[pIndex];
			}
		}

		const unsigned* mNumChunks;
		int mCurrentChunk;
		int mCurrentIndex;
		T* mNode;
		Chunk* mCurrentChunk;
		Chunk** mChunk;
	};

	DynamicArray();
	DynamicArray(const DynamicArray& pArray);
	virtual ~DynamicArray();
	void Clear();
	void operator=(const DynamicArray& pOther);

	inline T& operator[](unsigned pIndex)
	{
		unsigned lChunk = (pIndex >> CHUNKSIZE_EXP);
		unsigned lIndex = (pIndex & (CHUNKSIZE - 1));

		return mChunk[lChunk]->mTArray[lIndex];
	}

	inline const T& operator[](unsigned pIndex) const
	{
		unsigned lChunk = (pIndex >> CHUNKSIZE_EXP);
		unsigned lIndex = (pIndex & (CHUNKSIZE - 1));

		return mChunk[lChunk]->mTArray[lIndex];
	}

	Iterator PushFront(const T& pObject); // Avoid using this!
	Iterator PushBack(const T& pObject);

	// The iterator pBeforeHere will be invalid after a call to InsertBefore().
	// Please DO NOT call these functions! They are SLOW! Use a linked
	// list instead if you need to do operations like this.
	Iterator InsertBefore(const Iterator& pBeforeHere, const T& pObject);
	Iterator InsertAfter(const Iterator& pAfterHere, const T& pObject);

	T PopFront(); // Avoid using this!
	T PopBack();

	// OBS! Indexes are changed after removing an item!
	void Remove(const Iterator& pItem);
	void Remove(T pItem);
	void RemoveN(unsigned pIndex);

	void RemoveAll();

	inline Iterator First() const;
	inline Iterator Last() const;
	inline Iterator End() const;

	Iterator Find(const T& pObject);
	bool Exists(const T& pObject);

	inline bool IsEmpty() const;
	inline int GetCount() const;

	inline unsigned GetIndex(const Iterator& pIter);

private:
	void CheckObjectInc();
	void CheckChunkArrayInc();

	int mObjectCount;

	Chunk** mChunk;
	unsigned mNumChunks;
	unsigned mChunkArraySize;
};

TEMPLATE QUAL::DynamicArray()
{
	mNumChunks = 0;
	mChunkArraySize = 0;
	mChunk = 0;
	mObjectCount = 0;
}

TEMPLATE QUAL::DynamicArray(const DynamicArray& pArray)
{
	mNumChunks = 0;
	mChunk = 0;

	*this = pArray;
}

TEMPLATE QUAL::~DynamicArray()
{
	Clear();
}

TEMPLATE void QUAL::Clear()
{
	for (unsigned i = 0; i < mNumChunks; i++)
	{
		delete (mChunk[i]);
		mChunk[i] = 0;
	}

	if (mChunk != 0)
	{
		delete[] (mChunk);
		mChunk = 0;
	}
}

TEMPLATE void QUAL::operator=(const DynamicArray& pArray)
{
	Clear();

	mChunkArraySize = pArray.mChunkArraySize;
	mNumChunks = pArray.mNumChunks;
	mObjectCount = pArray.mObjectCount;

	mChunk = new Chunk*[mChunkArraySize];

	for (unsigned i = 0; i < mNumChunks; i++)
	{
		mChunk[i] = new Chunk(*pArray.mChunk[i]);
	}
}

TEMPLATE void QUAL::CheckObjectInc()
{
	if (mChunk == 0)
	{
		CheckChunkArrayInc();
	}

	Chunk* lChunk = mChunk[mNumChunks - 1];

	if (lChunk->mObjectCount == CHUNKSIZE)
	{
		// We need a new chunk.
		CheckChunkArrayInc();

		mChunk[mNumChunks] = new Chunk();
		mNumChunks++;
	}
}

TEMPLATE void QUAL::CheckChunkArrayInc()
{
	// Check if there is room for one more chunk.

	if (mNumChunks >= mChunkArraySize)
	{
		// We have to allocate a larger chunk array.
		Chunk** lChunkArray = new Chunk*[mChunkArraySize + 16];

		if (mChunk != 0)
		{
			for (unsigned i = 0; i < mChunkArraySize; i++)
			{
				lChunkArray[i] = mChunk[i];
			}

			delete[] mChunk;
		}

		mChunk = lChunkArray;
		mChunkArraySize += 16;
	}

	// We need at least one chunk.
	if (mNumChunks == 0)
	{
		mChunk[0] = new Chunk();
		mNumChunks++;
	}
}

TEMPLATE typename QUAL::Iterator QUAL::First() const
{
	if (mObjectCount == 0)
	{
		return Iterator(0, 0, 0, 0);
	}
	else
	{
		return Iterator(mChunk, &mNumChunks, 0, 0);
	}
}

TEMPLATE typename QUAL::Iterator QUAL::Last() const
{
	if (mObjectCount == 0)
	{
		return Iterator(0, 0, 0, 0);
	}
	else
	{
		return Iterator(mChunk, &mNumChunks, mNumChunks - 1, mChunk[mNumChunks - 1]->mObjectCount - 1);
	}
}

TEMPLATE typename QUAL::Iterator QUAL::End() const
{
	return Iterator(0, 0, 0, 0);
}

TEMPLATE bool QUAL::IsEmpty() const
{
	return (mObjectCount == 0);
}

TEMPLATE int QUAL::GetCount() const
{
	return mObjectCount;
}

TEMPLATE void QUAL::Remove(const Iterator& pItem)
{
	int lCurrentChunk = pItem.mCurrentChunk;
	int lCurrentIndex = pItem.mCurrentIndex;
	// Now move all elements one step towards the beginning of the array...

	int lObjectCount = mChunk[mNumChunks - 1]->mObjectCount;
	T lCarry = mChunk[mNumChunks - 1]->mTArray[lObjectCount - 1];

	int i;
	for (i = mNumChunks - 1; i > lCurrentChunk; i--)
	{
		lObjectCount = mChunk[i]->mObjectCount;
		for (int j = lObjectCount - 1; j >= 0; j--)
		{
			T lTemp = mChunk[i]->mTArray[j];
			mChunk[i]->mTArray[j] = lCarry;
			lCarry = lTemp;
		}
	}

	lObjectCount = (int)mChunk[i]->mObjectCount;
	for (int j = lObjectCount - 1; j >= lCurrentIndex; j--)
	{
		T lTemp = mChunk[i]->mTArray[j];
		mChunk[i]->mTArray[j] = lCarry;
		lCarry = lTemp;
	}

	mChunk[mNumChunks - 1]->mObjectCount--;

	if (mChunk[mNumChunks - 1]->mObjectCount == 0)
	{
		delete mChunk[mNumChunks - 1];
		mNumChunks--;
	}

	mObjectCount--;
}

TEMPLATE void QUAL::Remove(T pItem)
{
	for (Iterator lIter = First(); lIter != End(); ++lIter)
	{
		if (lIter.mNode == &pItem)
		{
			Remove(lIter);
			return;
		}
	}
}

TEMPLATE void QUAL::RemoveN(unsigned pIndex)
{
	unsigned lChunk = (pIndex >> CHUNKSIZE_EXP);
	unsigned lIndex = (pIndex & (CHUNKSIZE - 1));

	Iterator lIter(mChunk, &mNumChunks, lChunk, lIndex);
	Remove(lIter);
}

TEMPLATE void QUAL::RemoveAll()
{
	for (unsigned i = 0; i < mNumChunks; i++)
	{
		delete mChunk[i];
	}

	delete[] mChunk;

	mNumChunks = 0;
	mChunkArraySize = 0;
	mChunk = 0;
	mObjectCount = 0;
}

TEMPLATE typename QUAL::Iterator QUAL::PushFront(const T& pObject)
{
	CheckObjectInc();

	// Put this object first in the array... Not a very fast thing
	// to do. You are really naughty if you call this functions!
	T lCarry = pObject;
	for (unsigned i = 0; i < mNumChunks; i++)
	{
		int lObjectCount = mChunk[i]->mObjectCount;
		for (int j = 0; j < lObjectCount; j++)
		{
			T lTemp = mChunk[i]->mTArray[j];
			mChunk[i]->mTArray[j] = lCarry;
			lCarry = lTemp;
		}
	}

	mChunk[mNumChunks - 1]->mObjectCount++;
	mObjectCount++;

	return Iterator(mChunk, &mNumChunks, 0, 0);
}

TEMPLATE typename QUAL::Iterator QUAL::PushBack(const T& pObject)
{
	CheckObjectInc();

	Chunk* lChunk = mChunk[mNumChunks - 1];
	lChunk->mTArray[lChunk->mObjectCount++] = pObject;

	mObjectCount++;

	return Iterator(mChunk, &mNumChunks, mNumChunks-1, lChunk->mObjectCount-1);
}

TEMPLATE typename QUAL::Iterator QUAL::InsertBefore(const Iterator& pBeforeHere, const T& pObject)
{
	if (pBeforeHere.mArray != this)
	{
		return End();
	}

	CheckObjectInc();

	int lStartIndex = pBeforeHere.mCurrentIndex;
	T lCarry = pObject;
	for (unsigned i = (unsigned)pBeforeHere.mCurrentChunk; i < mNumChunks; i++)
	{
		int lObjectCount = mChunk[i]->mObjectCount;
		for (int j = lStartIndex; j < lObjectCount; j++)
		{
			T lTemp = mChunk[i]->mTArray[j];
			mChunk[i]->mTArray[j] = lCarry;
			lCarry = lTemp;
		}

		lStartIndex = 0;
	}

	mChunk[mNumChunks - 1]->mObjectCount++;
	mObjectCount++;

	// The iterator pBeforeHere is now invalid.
	return Iterator(this, pBeforeHere.mCurrentChunk, pBeforeHere.mCurrentIndex);
}

TEMPLATE typename QUAL::Iterator QUAL::InsertAfter(const Iterator& pAfterHere, const T& pObject)
{
	if (pAfterHere.mArray != this)
	{
		return End();
	}

	CheckObjectInc();

	int lStartIndex = pAfterHere.mCurrentIndex + 1;
	unsigned lStartChunk = (unsigned)pAfterHere.mCurrentChunk;

	if (lStartIndex >= CHUNKSIZE)
	{
		lStartChunk++;
		lStartIndex = 0;
	}

	int lStartIndex2 = lStartIndex;
	T lCarry = pObject;
	for (unsigned i = lStartChunk; i < mNumChunks; i++)
	{
		int lObjectCount = mChunk[i]->mObjectCount;
		for (int j = lStartIndex2; j < lObjectCount; j++)
		{
			T lTemp = mChunk[i]->mTArray[j];
			mChunk[i]->mTArray[j] = lCarry;
			lCarry = lTemp;
		}

		lStartIndex2 = 0;
	}

	mChunk[mNumChunks - 1]->mObjectCount++;
	mObjectCount++;

	return Iterator(this, lStartChunk, lStartIndex);
}

TEMPLATE T QUAL::PopFront()
{
	T lReturnValue = mChunk[0]->mTArray[0];

	// Now move all elements one step towards the beginning of the array...

	T lCarry;
	for (unsigned i = mNumChunks - 1; i >= 0; i--)
	{
		int lObjectCount = mChunk[i]->mObjectCount;
		for (int j = lObjectCount - 1; j >= 0; j--)
		{
			T lTemp = mChunk[i]->mTArray[j];
			mChunk[i]->mTArray[j] = lCarry;
			lCarry = lTemp;
		}
	}

	mChunk[mNumChunks - 1]->mObjectCount--;

	if (mChunk[mNumChunks - 1]->mObjectCount == 0)
	{
		delete mChunk[mNumChunks - 1];
		mNumChunks--;
	}

	mObjectCount--;

	return lReturnValue;
}

TEMPLATE T QUAL::PopBack()
{
	Chunk* lChunk = mChunk[mNumChunks - 1];
	T lReturnValue = lChunk->mTArray[lChunk->mObjectCount - 1];

	lChunk->mObjectCount--;

	if (lChunk->mObjectCount == 0)
	{
		delete lChunk;
		mNumChunks--;
	}

	return lReturnValue;
}

TEMPLATE typename QUAL::Iterator QUAL::Find(const T& pObject)
{
	Iterator lIter = First();
	Iterator lEndIter = End();

	while (lIter != lEndIter)
	{
		if (*lIter == pObject)
		{
			return lIter;
		}

		++lIter;
	}

	return lIter;
}

TEMPLATE bool QUAL::Exists(const T& pObject)
{
	return (Find(pObject) != End());
}

TEMPLATE unsigned QUAL::GetIndex(const Iterator& pIter)
{
	return pIter.mCurrentChunk * CHUNKSIZE + pIter.mCurrentIndex;
}

} // End namespace.

#undef TEMPLATE
#undef QUAL
#undef CHUNKSIZE
#undef CHUNKSIZE_EXP

#endif
*/