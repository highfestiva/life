/*
	Class:  LooseBinTree
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A binary space tree. This is the 1D version of a space partition tree.
*/

#pragma once

#include "fastallocator.h"
#include "hashtable.h"
#include "lepratypes.h"
#include "log.h"
#include <list>

#define TEMPLATE template<class _TKey, class _TObject, class _TVarType, class _THashFunc>
#define QUAL LooseBinTree<_TKey, _TObject, _TVarType, _THashFunc>

namespace lepra {

template<class _TKey, class _TObject, class _TVarType, class _THashFunc = std::hash<_TKey> >
class LooseBinTree {
public:

	typedef std::list<_TObject> ObjectList;

	LooseBinTree(_TObject error_object,			// An object to return when an error occurs.
		     _TVarType total_tree_size = 65536,	// Length of the entire 1D-space.
		     _TVarType minimum_cell_size = 16,		// Length of the smallest allowed node.
		     _TVarType k = 2);			// Node expansion factor.

	virtual ~LooseBinTree();

	void InsertObject(_TKey key, _TObject object, _TVarType pos, _TVarType size);
	_TObject RemoveObject(_TKey key);
	_TObject FindObject(_TKey key) const;

	bool MoveObject(_TKey key, _TVarType new_pos, _TVarType new_size_half);
	_TObject MoveObject(_TKey key, _TVarType new_pos); // Slightly slower...

	bool GetObjectSizeAndPos(_TKey key, _TVarType& pos, _TVarType& size_half);

	// Get list with objects within the specified area. The list is not cleared,
	// objects are appended.
	void GetObjects(ObjectList& objects, _TVarType pos, _TVarType size_half);

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

private:

	// One node of the LooseBinTree... A node is a line segment containing objects,
	// and since the objects are 1D, they take up some space, defined by
	// the position and size. Each object- pos- and size-triple makes one entry
	// (look at class Entry below).
	// All entries are stored in a hash table, typedef'ed as EntryTable.
	class Node {
		public:

			// A node entry. Contains the object and its associated size.
			class Entry {
				public:
					inline Entry() {
					}

					inline Entry(const Entry& entry) {
						pos_      = entry.pos_;
						size_half_ = entry.size_half_;
						object_   = entry.object_;
					}

					inline bool IsEnclosing(_TVarType pos, _TVarType size_half) const {
						_TVarType min1(pos - size_half);
						_TVarType max1(pos + size_half);
						_TVarType min2(pos_ - size_half_);
						_TVarType max2(pos_ + size_half_);

						return(max2 > max1 && min2 < min1);
					}

					inline bool IsEnclosed(_TVarType pos, _TVarType size_half) const {
						_TVarType min1(pos - size_half);
						_TVarType max1(pos + size_half);
						_TVarType min2(pos_ - size_half_);
						_TVarType max2(pos_ + size_half_);

						return(max1 > max2 && min1 < min2);
					}

					inline bool IsOverlapping(_TVarType pos, _TVarType size_half) const {
						_TVarType min_separation_dist(size_half + size_half_);
						_TVarType dist(pos - pos_);

						if (dist < 0.0f) {
							dist = -dist;
						}

						return (dist < min_separation_dist);
					}

					_TVarType		pos_;
					_TVarType		size_half_;
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
				// that this is the size of the node as it should be in a normal LooseBinTree.
				fixed_size_half_ = fixed_size_half;

				parent_ = parent;

				children_[0] = 0;
				children_[1] = 0;

				// The index tells us which of the parents children this node is.
				index_ = index;

				// Childmask is a bitfield containing 1's where there is a child node,
				// and 0's where there isn't.
				child_mask_ = 0;
				object_count_ = 0;
			}

			_TVarType GetPosition() const {
				return pos_;
			}

			_TVarType GetSizeHalf() const {
				return size_half_;
			}

			_TVarType GetFixedSizeHalf() const {
				return fixed_size_half_;
			}

			inline void DeleteChildren(LooseBinTree* loose_bin_tree) {
				for (int i = 0; i < 2; i++) {
					if (children_[i] != 0) {
						children_[i]->DeleteChildren(loose_bin_tree);

						if (loose_bin_tree != 0) {
							loose_bin_tree->RecycleNode(children_[i]);
						} else {
							delete children_[i];
						}
						children_[i] = 0;
					}
				}
			}

			inline bool IsEmpty() const {
				return (object_count_ == 0 && child_mask_ == 0);
			}

			inline bool IsEnclosing(_TVarType pos, _TVarType size_half) const {
				_TVarType min1(pos - size_half);
				_TVarType max1(pos + size_half);
				_TVarType min2(pos_ - size_half_);
				_TVarType max2(pos_ + size_half_);

				return(max2 > max1 && min2 < min1);
			}

			inline bool IsEnclosed(_TVarType pos, _TVarType size_half) const {
				_TVarType min1(pos - size_half);
				_TVarType max1(pos + size_half);
				_TVarType min2(pos_ - size_half_);
				_TVarType max2(pos_ + size_half_);

				return(max1 > max2 && min1 < min2);
			}

			_TVarType	fixed_size_half_;
			_TVarType	pos_;
			_TVarType	size_half_;

			Node*		parent_;
			Node*		children_[2];

			uint8		index_;

			uint16		child_mask_;
			uint16		object_count_;

			EntryTable	entry_table_;
	};

	friend class Node;

	// Used to quickly find the object that we search for.
	typedef HashTable<_TKey, Node*, _THashFunc> NodeTable;

	void InsertObject(_TKey key,
			  typename Node::Entry entry,
			  Node* node,
			  unsigned depth);

	typename Node::Entry RemoveObject(_TKey key, typename NodeTable::Iterator& node_iter);

	_TObject MoveObject(_TKey key,
			    typename NodeTable::Iterator& node_iter,
			    typename Node::EntryTable::Iterator& object_iter);

	typename Node::EntryTable::Iterator FindObject(_TKey key, Node* object_node) const;

	inline unsigned GetOverlaps(_TVarType pos_rel_parent,
				    _TVarType size_half,
				    _TVarType child_node_size,
				    _TVarType parent_node_size) const;

	void GetObjects(ObjectList& objects,
			_TVarType pos,
			_TVarType size_half,
			Node* node);

	uint8 GetChild(_TVarType pos, const Node* node);
	uint8 GetChild(_TVarType pos, const Node* node, _TVarType& child_pos);

	void RecycleNode(Node* node);		// Used to minimize the use of new and delete.
	Node* NewNode(Node* parent, uint8 index, _TVarType fixed_size_half);

	enum {
		kMaxRecycledNodes = 1024,
	};

	typedef std::list<Node*> NodeList;

	NodeList recycled_node_list_;
	NodeTable node_table_;

	Node* root_node_;

	unsigned max_tree_depth_;
	_TVarType k_;		// A constant node scaling factor.

	unsigned num_objects_;
	unsigned num_nodes_;

	_TObject error_object_;

	logclass();
};

#include "loosebintree.inl"

}

#undef TEMPLATE
#undef QUAL
