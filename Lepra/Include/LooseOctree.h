/*
	Class:  LooseOctree
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

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

#pragma once

#include "lepratypes.h"
#include "vector3d.h"
#include "sphere.h"
#include "aabb.h"
#include "collisiondetector3d.h"

#include <list>
#include "hashtable.h"

// _TObject is the object class itself.
// _TVarType is a primitive variable type (i.e. float or double).

#define TEMPLATE template<class _TKey, class _TObject, class _TVarType, class _THashFunc>
#define QUAL LooseOctree<_TKey, _TObject, _TVarType, _THashFunc>

namespace lepra {

template<class _TVarType>
class LOVolume {
public:
	virtual bool IsAABBEnclosingVolume(const AABB<_TVarType>& aabb)   = 0;
	virtual bool IsAABCEnclosingVolume(const Vector3D<_TVarType>& aabc_center,
						_TVarType half_aabc_size)   = 0;
	virtual bool IsBSOverlappingVolume(const Sphere<_TVarType>& bs)   = 0;
	virtual bool IsAABBOverlappingVolume(const AABB<_TVarType>& aabb) = 0;
	virtual Vector3D<_TVarType> GetPosition()                          = 0;
	virtual void SetPosition(const Vector3D<_TVarType>& pos)          = 0;
};

// Default area types.
template<class _TVarType>
class LOSphereVolume : public Sphere<_TVarType>, public LOVolume<_TVarType>
{
public:
	LOSphereVolume() {}
	LOSphereVolume(const Vector3D<_TVarType>& position, _TVarType radius) : Sphere<_TVarType>(position, radius) {}

	bool IsAABBEnclosingVolume(const AABB<_TVarType>& aabb) { return cd_.IsAABBEnclosingSphere(aabb, *this); }
	bool IsAABCEnclosingVolume(const Vector3D<_TVarType>& aabc_center, _TVarType half_aabc_size) { return cd_.IsAABBEnclosingSphere(AABB<_TVarType>(aabc_center, Vector3D<_TVarType>(half_aabc_size, half_aabc_size, half_aabc_size)), *this); }
	bool IsBSOverlappingVolume(const Sphere<_TVarType>& bs) { return cd_.IsSphereOverlappingSphere(bs, *this); }
	bool IsAABBOverlappingVolume(const AABB<_TVarType>& aabb) { return cd_.IsAABBOverlappingSphere(aabb, *this); }
	Vector3D<_TVarType> GetPosition() { return Sphere<_TVarType>::GetPosition(); }
	void SetPosition(const Vector3D<_TVarType>& pos) { Sphere<_TVarType>::SetPosition(pos); }
private:
	CollisionDetector3D<_TVarType> cd_;
};

template<class _TVarType>
class LOAABBVolume : public AABB<_TVarType>, public LOVolume<_TVarType>
{
public:
	LOAABBVolume() {}
	LOAABBVolume(const Vector3D<_TVarType>& position, const Vector3D<_TVarType>& size) : AABB<_TVarType>(position, size) {}

	bool IsAABBEnclosingVolume(const AABB<_TVarType>& aabb) { return cd_.IsAABB1EnclosingAABB2(aabb, *this); }
	bool IsAABCEnclosingVolume(const Vector3D<_TVarType>& aabc_center, _TVarType half_aabc_size) { return cd_.IsAABB1EnclosingAABB2(AABB<_TVarType>(aabc_center, Vector3D<_TVarType>(half_aabc_size, half_aabc_size, half_aabc_size)), *this); }
	bool IsBSOverlappingVolume(const Sphere<_TVarType>& bs) { return cd_.IsAABBOverlappingSphere(*this, bs); }
	bool IsAABBOverlappingVolume(const AABB<_TVarType>& aabb) { return cd_.IsAABBOverlappingAABB(aabb, *this); }
	Vector3D<_TVarType> GetPosition() { return AABB<_TVarType>::GetPosition(); }
	void SetPosition(const Vector3D<_TVarType>& pos) { AABB<_TVarType>::SetPosition(pos); }
private:
	CollisionDetector3D<_TVarType> cd_;
};


template<class _TKey, class _TObject, class _TVarType, class _THashFunc = std::hash<_TKey> >
class LooseOctree {
public:

	typedef std::list<_TObject>	        ObjectList;
	typedef std::list<AABB<_TVarType> > AABBList;

	LooseOctree(_TObject error_object,			// An object to return when an error occurs.
			_TVarType total_tree_size = 65536,	// Length of one dimension the entire 3D-space.
			_TVarType minimum_cell_size = 16,	// Length of one dimension of the smallest allowed node.
			_TVarType k = 2);			// Node expansion factor.
	~LooseOctree();

	void InsertObject(_TKey key, LOVolume<_TVarType>* volume, _TObject object);
	_TObject RemoveObject(_TKey key);
	_TObject FindObject(_TKey key) const;

	bool MoveObject(_TKey key, LOVolume<_TVarType>* new_volume);
	_TObject MoveObject(_TKey key, const Vector3D<_TVarType>& to_pos); // Slightly slower...

	// Get list with objects within the specified volume. The list is not cleared,
	// objects are appended.
	void GetObjects(ObjectList& objects, const Sphere<_TVarType>& bs);
	void GetObjects(ObjectList& objects, const AABB<_TVarType>& aabb);

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
	void GetNodeBoxes(AABBList& boxes) const;

private:

	// One node of the octree... A node is a cube containing objects,
	// and since the objects are 3D, they take up some space, defined by
	// the volume. Each object- and volume-pair makes one entry (look at class Entry below).
	// All entries are stored in a hash table, typedef:ed as EntryTable.
	class Node {
		public:

			// A node entry. Contains the object and its associated volume.
			class Entry {
				public:
					inline Entry(LOVolume<_TVarType>* volume, const _TObject& object) :
						volume_(volume),
						object_(object) {
					}

					inline Entry(const Entry& entry) {
						volume_ = entry.volume_;
						object_  = entry.object_;
					}

					LOVolume<_TVarType>*	volume_;
					_TObject		object_;
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
				children_[4] = 0;
				children_[5] = 0;
				children_[6] = 0;
				children_[7] = 0;

				// The index tells us which of the parents children this node is.
				index_ = index;

				// Childmask is a bitfield containing 1's where there is a child node,
				// and 0's where there isn't.
				child_mask_ = 0;
				object_count_ = 0;
			}

			const Vector3D<_TVarType>& GetPosition() const {
				return node_box_.GetPosition();
			}

			_TVarType GetSizeHalf() const {
				return node_box_.GetSize().x;
			}

			_TVarType GetFixedSizeHalf() const {
				return fixed_size_half_;
			}

			inline void DeleteChildren(LooseOctree* octree) {
				for (int i = 0; i < 8; i++) {
					if (children_[i] != 0) {
						children_[i]->DeleteChildren(octree);

						if (octree != 0) {
							octree->RecycleNode(children_[i]);
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

				for (i = 0; i < 8; i++) {
					if (children_[i] != NULL) {
						children_[i]->GetNodeInfoAsText(s, level + 1, i);
					}
				}
			}
*/

			void GetNodeBoxRecursive(AABBList& boxes) const {
				boxes.PushBack(node_box_);

				for (int i = 0; i < 8; i++) {
					if (children_[i] != NULL) {
						children_[i]->GetNodeBoxRecursive(boxes);
					}
				}
			}

			_TVarType fixed_size_half_;
			AABB<_TVarType>	node_box_;

			Node* parent_;
			Node* children_[8];

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

	_TObject MoveObject(_TKey key, typename NodeTable::Iterator& node_iter);

	typename Node::EntryTable::Iterator FindObject(_TKey key, Node* object_node) const;

	inline unsigned GetOverlaps( const Vector3D<_TVarType>& pos_rel_parent,
				    _TVarType bounding_radius,
				    _TVarType child_node_size,
				    _TVarType parent_node_size) const;
	inline unsigned GetOverlaps( const Vector3D<_TVarType>& pos_rel_parent,
				     _TVarType size_x,
				     _TVarType size_y,
				     _TVarType size_z,
				     _TVarType child_node_size,
				     _TVarType parent_node_size) const;

	// BS = Bounding Sphere.
	void GetObjectsInBS(ObjectList& objects, const Sphere<_TVarType>& bs, Node* node);
	void GetObjectsInAABB(ObjectList& objects, const AABB<_TVarType>& aabb, Node* node);

	uint8 GetChild(const Vector3D<_TVarType>& pos, const Node* node);
	uint8 GetChild(const Vector3D<_TVarType>& pos, const Node* node, Vector3D<_TVarType>& child_pos);

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
	if ( ( pos_rel_parent.z - child_node_size ) > min_box_separation || \
	    ( child_node_size - pos_rel_parent.z ) > min_box_separation ) \
	{	\
		/* Remove top. */	\
		overlap_mask &= ~(2 + 8 + 32 + 128);	\
	}	\
	/* Test bottom. */	\
	else if( ( pos_rel_parent.z + child_node_size ) > min_box_separation || \
		-( child_node_size + pos_rel_parent.z ) > min_box_separation ) \
	{	\
		/* Remove bottom. */	\
		overlap_mask &= ~(1 + 4 + 16 + 64);	\
	}	\
}

#define kMacroLoGo2TestTopAndBottom	\
{	\
	/* Test top. */	\
	if ( ( pos_rel_parent.z - child_node_size ) > min_box_separation_z || \
	    ( child_node_size - pos_rel_parent.z ) > min_box_separation_z ) \
	{	\
		/* Remove top. */	\
		overlap_mask &= ~(2 + 8 + 32 + 128);	\
	}	\
	/* Test bottom. */	\
	else if( ( pos_rel_parent.z + child_node_size ) > min_box_separation_z || \
		-( child_node_size + pos_rel_parent.z ) > min_box_separation_z ) \
	{	\
		/* Remove bottom. */	\
		overlap_mask &= ~(1 + 4 + 16 + 64);	\
	}	\
}


#define kMacroLoGo1TestFrontBackTopBottom	\
{	\
	/* Test front. */	\
	if ( ( pos_rel_parent.y - child_node_size ) > min_box_separation || \
	    ( child_node_size - pos_rel_parent.y ) > min_box_separation ) \
	{	\
		/* Remove front. */	\
		overlap_mask &= ~(4 + 8 + 64 + 128);	\
		kMacroLoGo1TestTopAndBottom	\
	}	\
	/* Test back. */	\
	else if( ( pos_rel_parent.y + child_node_size ) > min_box_separation || \
		-( child_node_size + pos_rel_parent.y ) > min_box_separation ) \
	{	\
		/* Remove back. */	\
		overlap_mask &= ~(1 + 2 + 16 + 32);	\
		kMacroLoGo1TestTopAndBottom	\
	}	\
}


#define kMacroLoGo2TestFrontBackTopBottom	\
{	\
	/* Test front. */	\
	if ( ( pos_rel_parent.y - child_node_size ) > min_box_separation_y || \
	    ( child_node_size - pos_rel_parent.y ) > min_box_separation_y ) \
	{	\
		/* Remove front. */	\
		overlap_mask &= ~(4 + 8 + 64 + 128);	\
		kMacroLoGo2TestTopAndBottom	\
	}	\
	/* Test back. */	\
	else if( ( pos_rel_parent.y + child_node_size ) > min_box_separation_y || \
		-( child_node_size + pos_rel_parent.y ) > min_box_separation_y ) \
	{	\
		/* Remove back. */	\
		overlap_mask &= ~(1 + 2 + 16 + 32);	\
		kMacroLoGo2TestTopAndBottom	\
	}	\
}

#include "looseoctree.inl"

}

#undef TEMPLATE
#undef QUAL
#undef kMacroLoGo1TestTopAndBottom
#undef kMacroLoGo2TestFrontBackTopBottom
