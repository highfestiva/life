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

#pragma once

#include "aabr.h"
#include "circle.h"
#include "hashtable.h"
#include "lepratypes.h"
#include "string.h"
#include "vector2d.h"
#include "collisiondetector2d.h"
#include <list>

// _TObject is the object class itself.
// _TVarType is a primitive variable type (i.e. float or double).

#define LQ_TEMPLATE template<class _TKey, class _TObject, class _TVarType, class _THashFunc>
#define LQ_QUAL LooseQuadtree<_TKey, _TObject, _TVarType, _THashFunc>

namespace lepra {

template<class _TVarType>
class LQArea {
public:
	// AABR  = Axis Aligned Bounding Rect
	// AABSQ = Axis Aligned Bounding Square
	// BC    = Bounding Circle
	virtual bool IsAABREnclosingArea(const AABR<_TVarType>& aabr) = 0;
	virtual bool IsAABSQEnclosingArea(const Vector2D<_TVarType>& aabsq_center, _TVarType half_aabsq_size) = 0;
	virtual bool IsBCOverlappingArea(const Circle<_TVarType>& bc) = 0;
	virtual bool IsAABROverlappingArea(const AABR<_TVarType>& aabr) = 0;
	virtual Vector2D<_TVarType> GetPosition() = 0;
	virtual void SetPosition(const Vector2D<_TVarType>& pos) = 0;
};

// Default area types.
template<class _TVarType>
class LQCircleArea: public Circle<_TVarType>, public LQArea<_TVarType>
{
public:
	LQCircleArea():
		Circle<_TVarType>() {
		cd_.SetPointOfCollisionEnabled(false);
	}
	LQCircleArea(const Vector2D<_TVarType>& position, _TVarType radius):
		Circle<_TVarType>(position, radius) {
		cd_.SetPointOfCollisionEnabled(false);
	}

	bool IsAABREnclosingArea(const AABR<_TVarType>& aabr) { return cd_.IsAABREnclosingCircle(aabr, *this); }
	bool IsAABSQEnclosingArea(const Vector2D<_TVarType>& aabsq_center, _TVarType half_aabsq_size) { 	return cd_.IsAABREnclosingCircle(AABR<_TVarType>(aabsq_center, Vector2D<_TVarType>(half_aabsq_size, half_aabsq_size)), *this); }
	bool IsBCOverlappingArea(const Circle<_TVarType>& bc) { return cd_.StaticCircleToCircleTest(bc, *this); }
	bool IsAABROverlappingArea(const AABR<_TVarType>& aabr) { return cd_.StaticAABRToCircleTest(aabr, *this); }
	Vector2D<_TVarType> GetPosition() { return Circle<_TVarType>::GetPosition(); }
	void SetPosition(const Vector2D<_TVarType>& pos) { Circle<_TVarType>::SetPosition(pos); }

private:
	CollisionDetector2D<_TVarType> cd_;
};

template<class _TVarType>
class LQRectArea : public AABR<_TVarType>, public LQArea<_TVarType>
{
public:
	LQRectArea() :
		AABR<_TVarType>() {
		cd_.SetPointOfCollisionEnabled(false);
	}
	LQRectArea(const Vector2D<_TVarType>& position, const Vector2D<_TVarType>& size) :
		AABR<_TVarType>(position, size) {
		cd_.SetPointOfCollisionEnabled(false);
	}

	bool IsAABREnclosingArea(const AABR<_TVarType>& aabr) { return cd_.IsAABR1EnclosingAABR2(aabr, *this); }
	bool IsAABSQEnclosingArea(const Vector2D<_TVarType>& aabsq_center, _TVarType half_aabsq_size) { return cd_.IsAABR1EnclosingAABR2(AABR<_TVarType>(aabsq_center, Vector2D<_TVarType>(half_aabsq_size, half_aabsq_size)), *this); }
	bool IsBCOverlappingArea(const Circle<_TVarType>& bc) { return cd_.StaticAABRToCircleTest(*this, bc); }
	bool IsAABROverlappingArea(const AABR<_TVarType>& aabr) { return cd_.StaticAABRToAABRTest(aabr, *this); }
	Vector2D<_TVarType> GetPosition() { return AABR<_TVarType>::GetPosition(); }
	void SetPosition(const Vector2D<_TVarType>& pos) { AABR<_TVarType>::SetPosition(pos); }

private:
	CollisionDetector2D<_TVarType> cd_;
};

template<class _TKey, class _TObject, class _TVarType, class _THashFunc = std::hash<_TKey> >
class LooseQuadtree {
public:
	typedef AABR<_TVarType>     AABR_; // Axis Aligned Bounding Rect.
	typedef Circle<_TVarType>   BC;   // Bounding Circle
	typedef std::list<_TObject> ObjectList;
	typedef std::list<AABR_>     AABRList;

	LooseQuadtree(_TObject error_object,		// An object to return when an error occurs.
		      _TVarType total_tree_size = 65536,	// Length of one dimension of the entire 2D-space.
		      _TVarType minimum_cell_size = 16,	// Length of one dimension of the smallest allowed node.
		      _TVarType k = 2);		// Node expansion factor.
	~LooseQuadtree();

	void InsertObject(_TKey key, LQArea<_TVarType>* area, _TObject object);
	_TObject RemoveObject(_TKey key);
	_TObject FindObject(_TKey key) const;

	bool MoveObject(_TKey key, LQArea<_TVarType>* new_area);
	_TObject MoveObject(_TKey key, const Vector2D<_TVarType>& to_pos); // Slightly slower...

	// Get list with objects within the specified area. The list is not cleared,
	// objects are appended.
	void GetObjects(ObjectList& objects, const BC& bc);
	void GetObjects(ObjectList& objects, const AABR_& aabr);

	//
	// Debug utilities.
	//

	inline unsigned	GetNumObjects() const {return num_objects_;}
	inline unsigned	GetNumNodes() const {return num_nodes_;}

	// Get number of bytes currently allocated by tree (only the nodes, not the objects).
	inline unsigned	GetMemUsage() const {return num_nodes_ * sizeof(Node);}
	// Get number of bytes that the octree would need if all nodes were occupied.
	// (Only the nodes, not the objects).
	unsigned GetFullTreeMemSize() const;

	str ToString() const;
	void GetNodeRects(AABRList& rects) const;

private:

	// One node of the quadtree... A node is a square containing objects,
	// and since the objects are 2D, they take up some space, defined by
	// the area. Each object- and area-pair makes one entry (look at class Entry below).
	// All entries are stored in a hash table, typedef:ed as EntryTable.
	class Node {
		public:

			// A node entry. Contains the object and its associated volume.
			class Entry {
			public:
				inline Entry(LQArea<_TVarType>* area, const _TObject& object) :
					area_(area),
					object_(object) {
				}

				inline Entry(const Entry& entry) {
					area_ = entry.area_;
					object_  = entry.object_;
				}

				LQArea<_TVarType>* area_;
				_TObject object_;
			};

			typedef HashTable<_TKey, Entry, _THashFunc, 32> EntryTable;

			inline Node(Node* parent, uint8 index, _TVarType fixed_size_half) {
				Init(parent, index, fixed_size_half);
			}

			inline ~Node() {
				DeleteChildren(0);
			}

			void Init(Node* parent, uint8 index, _TVarType fixed_size_half) {
				// Half the size of this node, "unloose", which means
				// that this is the size of the node as it should be in a normal octree.
				fixed_size_half_ = fixed_size_half;

				parent_ = parent;

				children_[0] = 0;
				children_[1] = 0;
				children_[2] = 0;
				children_[3] = 0;

				// The index tells us which of the parents children this node is.
				index_ = index;

				// Childmask is a bitfield containing 1's where there is a child node,
				// and 0's where there isn't.
				child_mask_ = 0;
				object_count_ = 0;
			}

			const Vector2D<_TVarType>& GetPosition() const {
				return node_box_.GetPosition();
			}

			_TVarType GetSizeHalf() const {
				return node_box_.GetSize().x;
			}

			_TVarType GetFixedSizeHalf() const {
				return fixed_size_half_;
			}

			inline void DeleteChildren(LooseQuadtree* quadtree) {
				for (int i = 0; i < 4; i++) {
					if (children_[i] != 0) {
						children_[i]->DeleteChildren(quadtree);

						if (quadtree != 0) {
							quadtree->RecycleNode(children_[i]);
						} else {
							delete children_[i];
						}
						children_[i] = 0;
					}
				}
			}

			inline bool IsEmpty() {
				return (object_count_ == 0 && child_mask_ == 0);
			}

/*
			void GetNodeInfoAsText(std::stringstream& s, int level, int index) const {
				int i;
				for (i = 0; i < level; i++) {
					s << "\t";
				}

				s << "[" << index << "]\t" << object_count_ << std::endl;

				for (i = 0; i < 4; i++) {
					if (children_[i] != NULL) {
						children_[i]->GetNodeInfoAsText(s, level + 1, i);
					}
				}
			}
*/

			void GetNodeBoxRecursive(AABRList& boxes) const {
				boxes.PushBack(node_box_);

				for (int i = 0; i < 4; i++) {
					if (children_[i] != NULL) {
						children_[i]->GetNodeBoxRecursive(boxes);
					}
				}
			}

			_TVarType fixed_size_half_;
			AABR_ node_box_;

			Node* parent_;
			Node* children_[4];

			uint8 index_;

			uint16 child_mask_;
			uint16 object_count_;

			EntryTable entry_table_;
	};





	friend class Node;

	// Used to quickly find the object that we search for.
	typedef HashTable<_TKey, Node*, _THashFunc> NodeTable;

	void InsertObject(_TKey key,
			  typename Node::Entry entry,
			  Node* node,
			  uint16 depth);

	typename Node::Entry RemoveObject(_TKey key, typename NodeTable::Iterator& node_iter);

	_TObject MoveObject(_TKey key,
			    typename NodeTable::Iterator& node_iter);

	typename Node::EntryTable::Iterator FindObject(_TKey key, Node* object_node) const;

	inline unsigned GetOverlaps( const Vector2D<_TVarType>& pos_rel_parent,
				    _TVarType bounding_radius,
				    _TVarType child_node_size,
				    _TVarType parent_node_size) const;
	inline unsigned GetOverlaps( const Vector2D<_TVarType>& pos_rel_parent,
				     _TVarType size_x,
				     _TVarType size_y,
				     _TVarType child_node_size,
				     _TVarType parent_node_size) const;

	void GetObjectsInBC(Node* node, ObjectList& objects, const BC& bc);
	void GetObjectsInAABR(Node* node, ObjectList& objects, const AABR_& aabr);

	uint8 GetChild(const Vector2D<_TVarType>& pos, const Node* node);
	uint8 GetChild(const Vector2D<_TVarType>& pos, const Node* node, Vector2D<_TVarType>& child_pos);

	void RecycleNode(Node* node);		// Used to minimize the use of new and delete.
	Node* NewNode(Node* parent, uint8 index, _TVarType fixed_size_half);


	enum {
		kMaxRecycledNodes = 1024,
	};

	typedef std::list<Node*> NodeList;

	NodeList recycled_node_list_;
	NodeTable node_table_;

	Node* root_node_;

	uint16 max_tree_depth_;
	_TVarType k_;				// A constant node scaling factor.

	unsigned num_objects_;
	unsigned num_nodes_;

	_TObject error_object_;
};




// LO = LooseOctree
// GO = GetOverlaps

#define kMacroLoGo1TestTopAndBottom	\
{	\
	/* Test top. */	\
	if ( ( pos_rel_parent.y - child_node_size ) > min_box_separation || \
	    ( child_node_size - pos_rel_parent.y ) > min_box_separation ) \
	{	\
		/* Remove top. */	\
		overlap_mask &= ~(2 + 8);	\
	}	\
	/* Test bottom. */	\
	else if( ( pos_rel_parent.y + child_node_size ) > min_box_separation || \
		-( child_node_size + pos_rel_parent.y ) > min_box_separation ) \
	{	\
		/* Remove bottom. */	\
		overlap_mask &= ~(1 + 4);	\
	}	\
}

#define kMacroLoGo2TestTopAndBottom	\
{	\
	/* Test top. */	\
	if ( ( pos_rel_parent.y - child_node_size ) > min_box_separation_y || \
	    ( child_node_size - pos_rel_parent.y ) > min_box_separation_y ) \
	{	\
		/* Remove top. */	\
		overlap_mask &= ~(2 + 8);	\
	}	\
	/* Test bottom. */	\
	else if( ( pos_rel_parent.y + child_node_size ) > min_box_separation_y || \
		-( child_node_size + pos_rel_parent.y ) > min_box_separation_y ) \
	{	\
		/* Remove bottom. */	\
		overlap_mask &= ~(1 + 4);	\
	}	\
}

#include "loosequadtree.inl"

}

#undef LQ_TEMPLATE
#undef LQ_QUAL
#undef kMacroLoGo1TestTopAndBottom
#undef kMacroLoGo2TestTopAndBottom
