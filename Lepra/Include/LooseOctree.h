/*
	Class:  LooseOctree
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:
	
	The LooseOctree class is used to store objects in three dimensions
	spatially. It can be used as a loose octree as well as a normal octree.
	When searching for objects in a volumetric space this is the optimal
	storage class to do that.

	Each object is combined with a volume to tell the octree how much space
	this very object requires. Each object is also identified by an ID (unsigned).
	This ID is then used to access the object in the octree.

	The volume that is associated with the object must derive from the class
	LOVolume.
*/

#ifndef LOOSEOCTREE_H
#define LOOSEOCTREE_H

#include "LepraTypes.h"
#include "Vector3D.h"
#include "Sphere.h"
#include "AABB.h"
#include "CollisionDetector3D.h"

#include <list>
#include "HashTable.h"

// _TObject is the object class itself.
// _TVarType is a primitive variable type (i.e. float or double).

#define TEMPLATE template<class _TKey, class _TObject, class _TVarType, class _THashFunc>
#define QUAL LooseOctree<_TKey, _TObject, _TVarType, _THashFunc>

namespace Lepra
{

template<class _TVarType>
class LOVolume
{
public:
	virtual bool IsAABBEnclosingVolume(const AABB<_TVarType>& pAABB)   = 0;
	virtual bool IsAABCEnclosingVolume(const Vector3D<_TVarType>& pAABCCenter,
						_TVarType pHalfAABCSize)   = 0;
	virtual bool IsBSOverlappingVolume(const Sphere<_TVarType>& pBS)   = 0;
	virtual bool IsAABBOverlappingVolume(const AABB<_TVarType>& pAABB) = 0;
	virtual Vector3D<_TVarType> GetPosition()                          = 0;
	virtual void SetPosition(const Vector3D<_TVarType>& pPos)          = 0;
};

// Default area types.
template<class _TVarType>
class LOSphereVolume : public Sphere<_TVarType>, public LOVolume<_TVarType>
{
public:
	LOSphereVolume() {}
	LOSphereVolume(const Vector3D<_TVarType>& pPosition, _TVarType pRadius) : Sphere<_TVarType>(pPosition, pRadius) {}

	bool IsAABBEnclosingVolume(const AABB<_TVarType>& pAABB) { return mCD.IsAABBEnclosingSphere(pAABB, *this); }
	bool IsAABCEnclosingVolume(const Vector3D<_TVarType>& pAABCCenter, _TVarType pHalfAABCSize) { return mCD.IsAABBEnclosingSphere(AABB<_TVarType>(pAABCCenter, Vector3D<_TVarType>(pHalfAABCSize, pHalfAABCSize, pHalfAABCSize)), *this); }
	bool IsBSOverlappingVolume(const Sphere<_TVarType>& pBS) { return mCD.IsSphereOverlappingSphere(pBS, *this); }
	bool IsAABBOverlappingVolume(const AABB<_TVarType>& pAABB) { return mCD.IsAABBOverlappingSphere(pAABB, *this); }
	Vector3D<_TVarType> GetPosition() { return Sphere<_TVarType>::GetPosition(); }
	void SetPosition(const Vector3D<_TVarType>& pPos) { Sphere<_TVarType>::SetPosition(pPos); }
private:
	CollisionDetector3D<_TVarType> mCD;
};

template<class _TVarType>
class LOAABBVolume : public AABB<_TVarType>, public LOVolume<_TVarType>
{
public:
	LOAABBVolume() {}
	LOAABBVolume(const Vector3D<_TVarType>& pPosition, const Vector3D<_TVarType>& pSize) : AABB<_TVarType>(pPosition, pSize) {}

	bool IsAABBEnclosingVolume(const AABB<_TVarType>& pAABB) { return mCD.IsAABB1EnclosingAABB2(pAABB, *this); }
	bool IsAABCEnclosingVolume(const Vector3D<_TVarType>& pAABCCenter, _TVarType pHalfAABCSize) { return mCD.IsAABB1EnclosingAABB2(AABB<_TVarType>(pAABCCenter, Vector3D<_TVarType>(pHalfAABCSize, pHalfAABCSize, pHalfAABCSize)), *this); }
	bool IsBSOverlappingVolume(const Sphere<_TVarType>& pBS) { return mCD.IsAABBOverlappingSphere(*this, pBS); }
	bool IsAABBOverlappingVolume(const AABB<_TVarType>& pAABB) { return mCD.IsAABBOverlappingAABB(pAABB, *this); }
	Vector3D<_TVarType> GetPosition() { return AABB<_TVarType>::GetPosition(); }
	void SetPosition(const Vector3D<_TVarType>& pPos) { AABB<_TVarType>::SetPosition(pPos); }
private:
	CollisionDetector3D<_TVarType> mCD;
};


template<class _TKey, class _TObject, class _TVarType, class _THashFunc = std::hash<_TKey> >
class LooseOctree
{
public:

	typedef std::list<_TObject>	        ObjectList;
	typedef std::list<AABB<_TVarType> > AABBList;

	LooseOctree(_TObject pErrorObject,			// An object to return when an error occurs.
			_TVarType pTotalTreeSize = 65536,	// Length of one dimension the entire 3D-space.
			_TVarType pMinimumCellSize = 16,	// Length of one dimension of the smallest allowed node.
			_TVarType pK = 2);			// Node expansion factor.
	~LooseOctree();

	void InsertObject(_TKey pKey, LOVolume<_TVarType>* pVolume, _TObject pObject);
	_TObject RemoveObject(_TKey pKey);
	_TObject FindObject(_TKey pKey) const;

	bool MoveObject(_TKey pKey, LOVolume<_TVarType>* pNewVolume);
	_TObject MoveObject(_TKey pKey, const Vector3D<_TVarType>& pToPos); // Slightly slower...
																					
	// Get list with objects within the specified volume. The list is not cleared, 
	// objects are appended.
	void GetObjects(ObjectList& pObjects, const Sphere<_TVarType>& pBS);
	void GetObjects(ObjectList& pObjects, const AABB<_TVarType>& pAABB);

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

	str ToString() const;
	void GetNodeBoxes(AABBList& pBoxes) const;

private:

	// One node of the octree... A node is a cube containing objects,
	// and since the objects are 3D, they take up some space, defined by
	// the volume. Each object- and volume-pair makes one entry (look at class Entry below).
	// All entries are stored in a hash table, typedef:ed as EntryTable.
	class Node
	{
		public:

			// A node entry. Contains the object and its associated volume.
			class Entry
			{
				public:
					inline Entry(LOVolume<_TVarType>* pVolume, const _TObject& pObject) :
						mVolume(pVolume),
						mObject(pObject)
					{
					}

					inline Entry(const Entry& pEntry)	
					{
						mVolume = pEntry.mVolume;
						mObject  = pEntry.mObject;
					}

					LOVolume<_TVarType>*	mVolume;
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
				// that this is the size of the node as it should be in a normal octree.
				mFixedSizeHalf = pFixedSizeHalf;

				mParent = pParent;

				mChildren[0] = 0;
				mChildren[1] = 0;
				mChildren[2] = 0;
				mChildren[3] = 0;
				mChildren[4] = 0;
				mChildren[5] = 0;
				mChildren[6] = 0;
				mChildren[7] = 0;

				// The index tells us which of the parents children this node is.
				mIndex = pIndex;

				// Childmask is a bitfield containing 1's where there is a child node,
				// and 0's where there isn't.
				mChildMask = 0;
				mObjectCount = 0;
			}

			const Vector3D<_TVarType>& GetPosition() const
			{
				return mNodeBox.GetPosition();
			}

			_TVarType GetSizeHalf() const
			{
				return mNodeBox.GetSize().x;
			}

			_TVarType GetFixedSizeHalf() const
			{
				return mFixedSizeHalf;
			}

			inline void DeleteChildren(LooseOctree* pOctree)
			{
				for (int i = 0; i < 8; i++)
				{
					if (mChildren[i] != 0)
					{
						mChildren[i]->DeleteChildren(pOctree);

						if (pOctree != 0)
						{
							pOctree->RecycleNode(mChildren[i]);
						}
						else
						{
							delete mChildren[i];
						}
						mChildren[i] = 0;
					}
				}
			}

			inline bool IsEmpty()
			{
				return (mObjectCount == 0 && mChildMask == 0);
			}

/*
			void GetNodeInfoAsText(std::stringstream& pStr, int pLevel, int pIndex) const
			{
				int i;
				for (i = 0; i < pLevel; i++)
				{
					pStr << "\t";
				}

				pStr << "[" << pIndex << "]\t" << mObjectCount << std::endl;

				for (i = 0; i < 8; i++)
				{
					if (mChildren[i] != NULL)
					{
						mChildren[i]->GetNodeInfoAsText(pStr, pLevel + 1, i);
					}
				}
			}
*/

			void GetNodeBoxRecursive(AABBList& pBoxes) const
			{
				pBoxes.PushBack(mNodeBox);

				for (int i = 0; i < 8; i++)
				{
					if (mChildren[i] != NULL)
					{
						mChildren[i]->GetNodeBoxRecursive(pBoxes);
					}
				}
			}

			_TVarType mFixedSizeHalf;
			AABB<_TVarType>	mNodeBox;

			Node* mParent;
			Node* mChildren[8];

			uint8 mIndex;
			
			uint16 mChildMask;
			uint16 mObjectCount;

			EntryTable mEntryTable;
	};





	friend class Node;

	// Used to quickly find the object that we search for.
	typedef HashTable<_TKey, Node*, _THashFunc> NodeTable;

	void InsertObject(_TKey pKey,
			  typename Node::Entry pEntry,
			  Node* pNode,
			  uint16 pDepth);

	typename Node::Entry RemoveObject(_TKey pKey, typename NodeTable::Iterator& pNodeIter);

	_TObject MoveObject(_TKey pKey, typename NodeTable::Iterator& pNodeIter);

	typename Node::EntryTable::Iterator FindObject(_TKey pKey, Node* pObjectNode) const;

	inline unsigned GetOverlaps( const Vector3D<_TVarType>& pPosRelParent, 
				    _TVarType pBoundingRadius, 
				    _TVarType pChildNodeSize,
				    _TVarType pParentNodeSize) const;
	inline unsigned GetOverlaps( const Vector3D<_TVarType>& pPosRelParent, 
				     _TVarType pSizeX, 
				     _TVarType pSizeY, 
				     _TVarType pSizeZ, 
				     _TVarType pChildNodeSize,
				     _TVarType pParentNodeSize) const;

	// BS = Bounding Sphere.
	void GetObjectsInBS(ObjectList& pObjects, const Sphere<_TVarType>& pBS, Node* pNode);
	void GetObjectsInAABB(ObjectList& pObjects, const AABB<_TVarType>& pAABB, Node* pNode);

	uint8 GetChild(const Vector3D<_TVarType>& pPos, const Node* pNode);
	uint8 GetChild(const Vector3D<_TVarType>& pPos, const Node* pNode, Vector3D<_TVarType>& pChildPos);
	
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

	uint16 mMaxTreeDepth;
	_TVarType mK;				// A constant node scaling factor.

	unsigned mNumObjects;
	unsigned mNumNodes;

	_TObject mErrorObject;
};




// LO = LooseOctree
// GO = GetOverlaps

#define MACRO_LO_GO1_TEST_TOP_AND_BOTTOM	\
{	\
	/* Test top. */	\
	if ( ( pPosRelParent.z - pChildNodeSize ) > lMinBoxSeparation || \
	    ( pChildNodeSize - pPosRelParent.z ) > lMinBoxSeparation ) \
	{	\
		/* Remove top. */	\
		lOverlapMask &= ~(2 + 8 + 32 + 128);	\
	}	\
	/* Test bottom. */	\
	else if( ( pPosRelParent.z + pChildNodeSize ) > lMinBoxSeparation || \
		-( pChildNodeSize + pPosRelParent.z ) > lMinBoxSeparation ) \
	{	\
		/* Remove bottom. */	\
		lOverlapMask &= ~(1 + 4 + 16 + 64);	\
	}	\
}

#define MACRO_LO_GO2_TEST_TOP_AND_BOTTOM	\
{	\
	/* Test top. */	\
	if ( ( pPosRelParent.z - pChildNodeSize ) > lMinBoxSeparationZ || \
	    ( pChildNodeSize - pPosRelParent.z ) > lMinBoxSeparationZ ) \
	{	\
		/* Remove top. */	\
		lOverlapMask &= ~(2 + 8 + 32 + 128);	\
	}	\
	/* Test bottom. */	\
	else if( ( pPosRelParent.z + pChildNodeSize ) > lMinBoxSeparationZ || \
		-( pChildNodeSize + pPosRelParent.z ) > lMinBoxSeparationZ ) \
	{	\
		/* Remove bottom. */	\
		lOverlapMask &= ~(1 + 4 + 16 + 64);	\
	}	\
}


#define MACRO_LO_GO1_TEST_FRONT_BACK_TOP_BOTTOM	\
{	\
	/* Test front. */	\
	if ( ( pPosRelParent.y - pChildNodeSize ) > lMinBoxSeparation || \
	    ( pChildNodeSize - pPosRelParent.y ) > lMinBoxSeparation ) \
	{	\
		/* Remove front. */	\
		lOverlapMask &= ~(4 + 8 + 64 + 128);	\
		MACRO_LO_GO1_TEST_TOP_AND_BOTTOM	\
	}	\
	/* Test back. */	\
	else if( ( pPosRelParent.y + pChildNodeSize ) > lMinBoxSeparation || \
		-( pChildNodeSize + pPosRelParent.y ) > lMinBoxSeparation ) \
	{	\
		/* Remove back. */	\
		lOverlapMask &= ~(1 + 2 + 16 + 32);	\
		MACRO_LO_GO1_TEST_TOP_AND_BOTTOM	\
	}	\
}


#define MACRO_LO_GO2_TEST_FRONT_BACK_TOP_BOTTOM	\
{	\
	/* Test front. */	\
	if ( ( pPosRelParent.y - pChildNodeSize ) > lMinBoxSeparationY || \
	    ( pChildNodeSize - pPosRelParent.y ) > lMinBoxSeparationY ) \
	{	\
		/* Remove front. */	\
		lOverlapMask &= ~(4 + 8 + 64 + 128);	\
		MACRO_LO_GO2_TEST_TOP_AND_BOTTOM	\
	}	\
	/* Test back. */	\
	else if( ( pPosRelParent.y + pChildNodeSize ) > lMinBoxSeparationY || \
		-( pChildNodeSize + pPosRelParent.y ) > lMinBoxSeparationY ) \
	{	\
		/* Remove back. */	\
		lOverlapMask &= ~(1 + 2 + 16 + 32);	\
		MACRO_LO_GO2_TEST_TOP_AND_BOTTOM	\
	}	\
}

#include "LooseOctree.inl"

} // End namespace.

#undef TEMPLATE
#undef QUAL
#undef MACRO_LO_GO1_TEST_TOP_AND_BOTTOM
#undef MACRO_LO_GO2_TEST_FRONT_BACK_TOP_BOTTOM

#endif // LOOSEOCTREE_H
