/*
	Class:  LQArea,
		CircleArea,
		LooseQuadtree
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:
	
	The LooseQuadtree class is used to store objects in two dimensions
	spatially. It can be used as a loose quadtree as well as a normal quadtree.
	When searching for objects in an area this is the optimal storage class 
	to do that.

	Each object is combined with an area to tell the octree how much space
	this very object requires. Each object is also identified by an ID (unsigned).
	This ID is then used to access the object in the quadtree.

	The area that is associated with the object must derive from the class
	LQArea.
*/

#ifndef LOOSEQUADTREE_H
#define LOOSEQUADTREE_H

#include "AABR.h"
#include "Circle.h"
#include "HashTable.h"
#include "LepraTypes.h"
#include "String.h"
#include "Vector2D.h"
#include "CollisionDetector2D.h"
#include <list>

// _TObject is the object class itself.
// _TVarType is a primitive variable type (i.e. float or double).

#define LQ_TEMPLATE template<class _TKey, class _TObject, class _TVarType, class _THashFunc>
#define LQ_QUAL LooseQuadtree<_TKey, _TObject, _TVarType, _THashFunc>

namespace Lepra
{

template<class _TVarType>
class LQArea
{
public:
	// AABR  = Axis Aligned Bounding Rect
	// AABSQ = Axis Aligned Bounding Square
	// BC    = Bounding Circle
	virtual bool IsAABREnclosingArea(const AABR<_TVarType>& pAABR) = 0;
	virtual bool IsAABSQEnclosingArea(const Vector2D<_TVarType>& pAABSQCenter, _TVarType pHalfAABSQSize) = 0;
	virtual bool IsBCOverlappingArea(const Circle<_TVarType>& pBC) = 0;
	virtual bool IsAABROverlappingArea(const AABR<_TVarType>& pAABR) = 0;
	virtual Vector2D<_TVarType> GetPosition() = 0;
	virtual void SetPosition(const Vector2D<_TVarType>& pPos) = 0;
};

// Default area types.
template<class _TVarType>
class LQCircleArea: public Circle<_TVarType>, public LQArea<_TVarType>
{
public:
	LQCircleArea():
		Circle<_TVarType>() 
	{
		mCD.SetPointOfCollisionEnabled(false);
	}
	LQCircleArea(const Vector2D<_TVarType>& pPosition, _TVarType pRadius):
		Circle<_TVarType>(pPosition, pRadius) 
	{
		mCD.SetPointOfCollisionEnabled(false);
	}

	bool IsAABREnclosingArea(const AABR<_TVarType>& pAABR) { return mCD.IsAABREnclosingCircle(pAABR, *this); }
	bool IsAABSQEnclosingArea(const Vector2D<_TVarType>& pAABSQCenter, _TVarType pHalfAABSQSize) { 	return mCD.IsAABREnclosingCircle(AABR<_TVarType>(pAABSQCenter, Vector2D<_TVarType>(pHalfAABSQSize, pHalfAABSQSize)), *this); }
	bool IsBCOverlappingArea(const Circle<_TVarType>& pBC) { return mCD.StaticCircleToCircleTest(pBC, *this); }
	bool IsAABROverlappingArea(const AABR<_TVarType>& pAABR) { return mCD.StaticAABRToCircleTest(pAABR, *this); }
	Vector2D<_TVarType> GetPosition() { return Circle<_TVarType>::GetPosition(); }
	void SetPosition(const Vector2D<_TVarType>& pPos) { Circle<_TVarType>::SetPosition(pPos); }

private:
	CollisionDetector2D<_TVarType> mCD;
};

template<class _TVarType>
class LQRectArea : public AABR<_TVarType>, public LQArea<_TVarType>
{
public:
	LQRectArea() : 
		AABR<_TVarType>() 
	{
		mCD.SetPointOfCollisionEnabled(false);
	}
	LQRectArea(const Vector2D<_TVarType>& pPosition, const Vector2D<_TVarType>& pSize) : 
		AABR<_TVarType>(pPosition, pSize) 
	{
		mCD.SetPointOfCollisionEnabled(false);
	}

	bool IsAABREnclosingArea(const AABR<_TVarType>& pAABR) { return mCD.IsAABR1EnclosingAABR2(pAABR, *this); }
	bool IsAABSQEnclosingArea(const Vector2D<_TVarType>& pAABSQCenter, _TVarType pHalfAABSQSize) { return mCD.IsAABR1EnclosingAABR2(AABR<_TVarType>(pAABSQCenter, Vector2D<_TVarType>(pHalfAABSQSize, pHalfAABSQSize)), *this); }
	bool IsBCOverlappingArea(const Circle<_TVarType>& pBC) { return mCD.StaticAABRToCircleTest(*this, pBC); }
	bool IsAABROverlappingArea(const AABR<_TVarType>& pAABR) { return mCD.StaticAABRToAABRTest(pAABR, *this); }
	Vector2D<_TVarType> GetPosition() { return AABR<_TVarType>::GetPosition(); }
	void SetPosition(const Vector2D<_TVarType>& pPos) { AABR<_TVarType>::SetPosition(pPos); }

private:
	CollisionDetector2D<_TVarType> mCD;
};

template<class _TKey, class _TObject, class _TVarType, class _THashFunc = std::hash<_TKey> >
class LooseQuadtree
{
public:
	typedef AABR<_TVarType>     AABR_; // Axis Aligned Bounding Rect.
	typedef Circle<_TVarType>   BC;   // Bounding Circle
	typedef std::list<_TObject> ObjectList;
	typedef std::list<AABR_>     AABRList;

	LooseQuadtree(_TObject pErrorObject,		// An object to return when an error occurs.
		      _TVarType pTotalTreeSize = 65536,	// Length of one dimension of the entire 2D-space.
		      _TVarType pMinimumCellSize = 16,	// Length of one dimension of the smallest allowed node.
		      _TVarType pK = 2);		// Node expansion factor.
	~LooseQuadtree();

	void InsertObject(_TKey pKey, LQArea<_TVarType>* pArea, _TObject pObject);
	_TObject RemoveObject(_TKey pKey);
	_TObject FindObject(_TKey pKey) const;

	bool MoveObject(_TKey pKey, LQArea<_TVarType>* pNewArea);
	_TObject MoveObject(_TKey pKey, const Vector2D<_TVarType>& pToPos); // Slightly slower...

	// Get list with objects within the specified area. The list is not cleared, 
	// objects are appended.
	void GetObjects(ObjectList& pObjects, const BC& pBC);
	void GetObjects(ObjectList& pObjects, const AABR_& pAABR);

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
	void GetNodeRects(AABRList& pRects) const;

private:

	// One node of the quadtree... A node is a square containing objects,
	// and since the objects are 2D, they take up some space, defined by
	// the area. Each object- and area-pair makes one entry (look at class Entry below).
	// All entries are stored in a hash table, typedef:ed as EntryTable.
	class Node
	{
		public:

			// A node entry. Contains the object and its associated volume.
			class Entry
			{
			public:
				inline Entry(LQArea<_TVarType>* pArea, const _TObject& pObject) :
					mArea(pArea),
					mObject(pObject)
				{
				}

				inline Entry(const Entry& pEntry)	
				{
					mArea = pEntry.mArea;
					mObject  = pEntry.mObject;
				}

				LQArea<_TVarType>* mArea;
				_TObject mObject;
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

				// The index tells us which of the parents children this node is.
				mIndex = pIndex;

				// Childmask is a bitfield containing 1's where there is a child node,
				// and 0's where there isn't.
				mChildMask = 0;
				mObjectCount = 0;
			}

			const Vector2D<_TVarType>& GetPosition() const
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

			inline void DeleteChildren(LooseQuadtree* pQuadtree)
			{
				for (int i = 0; i < 4; i++)
				{
					if (mChildren[i] != 0)
					{
						mChildren[i]->DeleteChildren(pQuadtree);

						if (pQuadtree != 0)
						{
							pQuadtree->RecycleNode(mChildren[i]);
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

				for (i = 0; i < 4; i++)
				{
					if (mChildren[i] != NULL)
					{
						mChildren[i]->GetNodeInfoAsText(pStr, pLevel + 1, i);
					}
				}
			}
*/

			void GetNodeBoxRecursive(AABRList& pBoxes) const
			{
				pBoxes.PushBack(mNodeBox);

				for (int i = 0; i < 4; i++)
				{
					if (mChildren[i] != NULL)
					{
						mChildren[i]->GetNodeBoxRecursive(pBoxes);
					}
				}
			}

			_TVarType mFixedSizeHalf;
			AABR_ mNodeBox;

			Node* mParent;
			Node* mChildren[4];

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

	_TObject MoveObject(_TKey pKey,
			    typename NodeTable::Iterator& pNodeIter);

	typename Node::EntryTable::Iterator FindObject(_TKey pKey, Node* pObjectNode) const;

	inline unsigned GetOverlaps( const Vector2D<_TVarType>& pPosRelParent, 
				    _TVarType pBoundingRadius, 
				    _TVarType pChildNodeSize,
				    _TVarType pParentNodeSize) const;
	inline unsigned GetOverlaps( const Vector2D<_TVarType>& pPosRelParent, 
				     _TVarType pSizeX, 
				     _TVarType pSizeY, 
				     _TVarType pChildNodeSize,
				     _TVarType pParentNodeSize) const;

	void GetObjectsInBC(Node* pNode, ObjectList& pObjects, const BC& pBC);
	void GetObjectsInAABR(Node* pNode, ObjectList& pObjects, const AABR_& pAABR);

	uint8 GetChild(const Vector2D<_TVarType>& pPos, const Node* pNode);
	uint8 GetChild(const Vector2D<_TVarType>& pPos, const Node* pNode, Vector2D<_TVarType>& pChildPos);
	
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
	if ( ( pPosRelParent.y - pChildNodeSize ) > lMinBoxSeparation || \
	    ( pChildNodeSize - pPosRelParent.y ) > lMinBoxSeparation ) \
	{	\
		/* Remove top. */	\
		lOverlapMask &= ~(2 + 8);	\
	}	\
	/* Test bottom. */	\
	else if( ( pPosRelParent.y + pChildNodeSize ) > lMinBoxSeparation || \
		-( pChildNodeSize + pPosRelParent.y ) > lMinBoxSeparation ) \
	{	\
		/* Remove bottom. */	\
		lOverlapMask &= ~(1 + 4);	\
	}	\
}

#define MACRO_LO_GO2_TEST_TOP_AND_BOTTOM	\
{	\
	/* Test top. */	\
	if ( ( pPosRelParent.y - pChildNodeSize ) > lMinBoxSeparationY || \
	    ( pChildNodeSize - pPosRelParent.y ) > lMinBoxSeparationY ) \
	{	\
		/* Remove top. */	\
		lOverlapMask &= ~(2 + 8);	\
	}	\
	/* Test bottom. */	\
	else if( ( pPosRelParent.y + pChildNodeSize ) > lMinBoxSeparationY || \
		-( pChildNodeSize + pPosRelParent.y ) > lMinBoxSeparationY ) \
	{	\
		/* Remove bottom. */	\
		lOverlapMask &= ~(1 + 4);	\
	}	\
}

#include "LooseQuadtree.inl"

} // End namespace.

#undef LQ_TEMPLATE
#undef LQ_QUAL
#undef MACRO_LO_GO1_TEST_TOP_AND_BOTTOM
#undef MACRO_LO_GO2_TEST_TOP_AND_BOTTOM

#endif // LOOSEQUADTREE_H
