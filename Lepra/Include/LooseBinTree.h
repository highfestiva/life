/*
	Class:  LooseBinTree
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A binary space tree. This is the 1D version of a space partition tree.
*/

#pragma once

#include "FastAllocator.h"
#include "HashTable.h"
#include "LepraTypes.h"
#include "Log.h"
#include <list>

#define TEMPLATE template<class _TKey, class _TObject, class _TVarType, class _THashFunc>
#define QUAL LooseBinTree<_TKey, _TObject, _TVarType, _THashFunc>

namespace Lepra
{

template<class _TKey, class _TObject, class _TVarType, class _THashFunc = std::hash<_TKey> >
class LooseBinTree
{
public:

	typedef std::list<_TObject> ObjectList;

	LooseBinTree(_TObject pErrorObject,			// An object to return when an error occurs.
		     _TVarType pTotalTreeSize = 65536,	// Length of the entire 1D-space.
		     _TVarType pMinimumCellSize = 16,		// Length of the smallest allowed node.
		     _TVarType pK = 2);			// Node expansion factor.

	virtual ~LooseBinTree();

	void InsertObject(_TKey pKey, _TObject pObject, _TVarType pPos, _TVarType pSize);
	_TObject RemoveObject(_TKey pKey);
	_TObject FindObject(_TKey pKey) const;

	bool MoveObject(_TKey pKey, _TVarType pNewPos, _TVarType pNewSizeHalf);
	_TObject MoveObject(_TKey pKey, _TVarType pNewPos); // Slightly slower...

	bool GetObjectSizeAndPos(_TKey pKey, _TVarType& pPos, _TVarType& pSizeHalf);
																					
	// Get list with objects within the specified area. The list is not cleared, 
	// objects are appended.
	void GetObjects(ObjectList& pObjects, _TVarType pPos, _TVarType pSizeHalf);

	//
	// Debug utilities.
	//

	inline unsigned	GetNumObjects() const {return mNumObjects;}
	inline unsigned	GetNumNodes() const {return mNumNodes;}

	// Get number of bytes currently allocated by tree (only the nodes, not the objects).
	inline unsigned	GetMemUsage() const {return mNumNodes * sizeof(Node);}
	// Get number of bytes that the octree would need if all nodes were occupied.
	// (Only the nodes, not the objects).
	unsigned GetFullTreeMemSize() const;

private:

	// One node of the LooseBinTree... A node is a line segment containing objects,
	// and since the objects are 1D, they take up some space, defined by
	// the position and size. Each object- pos- and size-triple makes one entry 
	// (look at class Entry below).
	// All entries are stored in a hash table, typedef'ed as EntryTable.
	class Node
	{
		public:

			// A node entry. Contains the object and its associated size.
			class Entry
			{
				public:
					inline Entry()
					{
					}

					inline Entry(const Entry& pEntry)	
					{
						mPos      = pEntry.mPos;
						mSizeHalf = pEntry.mSizeHalf;
						mObject   = pEntry.mObject;
					}

					inline bool IsEnclosing(_TVarType pPos, _TVarType pSizeHalf) const
					{
						_TVarType lMin1(pPos - pSizeHalf);
						_TVarType lMax1(pPos + pSizeHalf);
						_TVarType lMin2(mPos - mSizeHalf);
						_TVarType lMax2(mPos + mSizeHalf);

						return(lMax2 > lMax1 && lMin2 < lMin1);
					}

					inline bool IsEnclosed(_TVarType pPos, _TVarType pSizeHalf) const
					{
						_TVarType lMin1(pPos - pSizeHalf);
						_TVarType lMax1(pPos + pSizeHalf);
						_TVarType lMin2(mPos - mSizeHalf);
						_TVarType lMax2(mPos + mSizeHalf);

						return(lMax1 > lMax2 && lMin1 < lMin2);
					}

					inline bool IsOverlapping(_TVarType pPos, _TVarType pSizeHalf) const
					{
						_TVarType lMinSeparationDist(pSizeHalf + mSizeHalf);
						_TVarType lDist(pPos - mPos);

						if (lDist < 0.0f)
						{
							lDist = -lDist;
						}

						return (lDist < lMinSeparationDist);
					}

					_TVarType		mPos;
					_TVarType		mSizeHalf;
					_TObject		mObject;
			};
			
			typedef HashTable<_TKey, Entry, _THashFunc, 32> EntryTable;

			inline Node(Node* pParent, uint8 pIndex, _TVarType pFixedSizeHalf)
			{
				Init(pParent, pIndex, pFixedSizeHalf);
			}

			inline ~Node()
			{
				DeleteChildren(0);
			}

			void Init(Node* pParent, uint8 pIndex, _TVarType pFixedSizeHalf)
			{
				// Half the size of this node, "unloose", which means
				// that this is the size of the node as it should be in a normal LooseBinTree.
				mFixedSizeHalf = pFixedSizeHalf;

				mParent = pParent;

				mChildren[0] = 0;
				mChildren[1] = 0;

				// The index tells us which of the parents children this node is.
				mIndex = pIndex;

				// Childmask is a bitfield containing 1's where there is a child node,
				// and 0's where there isn't.
				mChildMask = 0;
				mObjectCount = 0;
			}

			_TVarType GetPosition() const
			{
				return mPos;
			}

			_TVarType GetSizeHalf() const
			{
				return mSizeHalf;
			}

			_TVarType GetFixedSizeHalf() const
			{
				return mFixedSizeHalf;
			}

			inline void DeleteChildren(LooseBinTree* pLooseBinTree)
			{
				for (int i = 0; i < 2; i++)
				{
					if (mChildren[i] != 0)
					{
						mChildren[i]->DeleteChildren(pLooseBinTree);

						if (pLooseBinTree != 0)
						{
							pLooseBinTree->RecycleNode(mChildren[i]);
						}
						else
						{
							delete mChildren[i];
						}
						mChildren[i] = 0;
					}
				}
			}

			inline bool IsEmpty() const
			{
				return (mObjectCount == 0 && mChildMask == 0);
			}

			inline bool IsEnclosing(_TVarType pPos, _TVarType pSizeHalf) const
			{
				_TVarType lMin1(pPos - pSizeHalf);
				_TVarType lMax1(pPos + pSizeHalf);
				_TVarType lMin2(mPos - mSizeHalf);
				_TVarType lMax2(mPos + mSizeHalf);

				return(lMax2 > lMax1 && lMin2 < lMin1);
			}

			inline bool IsEnclosed(_TVarType pPos, _TVarType pSizeHalf) const
			{
				_TVarType lMin1(pPos - pSizeHalf);
				_TVarType lMax1(pPos + pSizeHalf);
				_TVarType lMin2(mPos - mSizeHalf);
				_TVarType lMax2(mPos + mSizeHalf);

				return(lMax1 > lMax2 && lMin1 < lMin2);
			}

			_TVarType	mFixedSizeHalf;
			_TVarType	mPos;
			_TVarType	mSizeHalf;

			Node*		mParent;
			Node*		mChildren[2];

			uint8		mIndex;
			
			uint16		mChildMask;
			uint16		mObjectCount;

			EntryTable	mEntryTable;
	};

	friend class Node;

	// Used to quickly find the object that we search for.
	typedef HashTable<_TKey, Node*, _THashFunc> NodeTable;

	void InsertObject(_TKey pKey,
			  typename Node::Entry pEntry,
			  Node* pNode,
			  unsigned pDepth);

	typename Node::Entry RemoveObject(_TKey pKey, typename NodeTable::Iterator& pNodeIter);

	_TObject MoveObject(_TKey pKey,
			    typename NodeTable::Iterator& pNodeIter,
			    typename Node::EntryTable::Iterator& pObjectIter);

	typename Node::EntryTable::Iterator FindObject(_TKey pKey, Node* pObjectNode) const;

	inline unsigned GetOverlaps(_TVarType pPosRelParent, 
				    _TVarType pSizeHalf, 
				    _TVarType pChildNodeSize,
				    _TVarType pParentNodeSize) const;

	void GetObjects(ObjectList& pObjects, 
			_TVarType pPos, 
			_TVarType pSizeHalf, 
			Node* pNode);

	uint8 GetChild(_TVarType pPos, const Node* pNode);
	uint8 GetChild(_TVarType pPos, const Node* pNode, _TVarType& pChildPos);
	
	void RecycleNode(Node* pNode);		// Used to minimize the use of new and delete.
	Node* NewNode(Node* pParent, uint8 pIndex, _TVarType pFixedSizeHalf);

	enum
	{
		MAX_RECYCLED_NODES = 1024,
	};

	typedef std::list<Node*> NodeList;

	NodeList mRecycledNodeList;
	NodeTable mNodeTable;

	Node* mRootNode;

	unsigned mMaxTreeDepth;
	_TVarType mK;		// A constant node scaling factor.

	unsigned mNumObjects;
	unsigned mNumNodes;

	_TObject mErrorObject;

	logclass();
};

#include "LooseBinTree.inl"

}

#undef TEMPLATE
#undef QUAL
