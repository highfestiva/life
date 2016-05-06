
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"
#include "fastallocator.h"



#define TEMPLATE template<class _TKey, class _TObject>
#define QUAL BinTree<_TKey, _TObject>



namespace lepra {



template<class _TKey, class _TObject>
class BinTree {
public:
	class Node;
private:
	typedef FastAllocator<Node> NodeAllocator;
public:
	class Node {
	public:

		Node();
		Node(_TKey key, _TObject object, Node* parent);
		~Node();

		void Init(_TKey key, _TObject object, Node* parent);
		void Destroy(NodeAllocator* allocator);

		Node* Insert(Node* node);
		Node* Find(_TKey key);
		const Node* GetLeftMostNode() const;
		Node* GetLeftMostNode();
		const Node* GetRightMostNode() const;
		Node* GetRightMostNode();

		void Relink(Node* old_node, Node* new_node);
		Node* Unlink();

		Node*    parent_;
		Node*    left_;
		Node*    right_;

		_TKey    key_;
		_TObject object_;
	};

public:

	class ConstIterator;

	class Iterator {
	public:
		friend class BinTree;

		Iterator() : node_(0), prev_(0){}
		Iterator(const Iterator& iterator) : node_(iterator.node_), prev_(iterator.prev_){}

		Iterator& operator = (const Iterator& other) {
			node_   = other.node_;
			prev_   = other.prev_;

			return *this;
		}

		Iterator& operator = (const Iterator* other) {
			node_   = other->node_;
			prev_   = other->prev_;

			return *this;
		}

		// Logical equality operator for iterator comparisons.
		int  operator==(const Iterator& other) const		{ return node_ == other.node_; }
		bool   operator==(const ConstIterator& other) const	{ return node_ == other.node_; }

		// Logical inequality operator for iterator comparisons.
		int  operator!=(const Iterator& other) const		{ return !operator==(other); }
		bool   operator!=(const ConstIterator& other) const	{ return !operator==(other); }

		// Gets the contents of the iterator.
		_TObject& operator*() const	{ return node_->object_; }

		// Pre- and postfix increment operators for traversing a tree.
		Iterator& operator++() {
			if (node_ == 0) {
				return *this;
			}

			if (prev_ == 0 || prev_ == node_->left_) {
				// Try to go right.
				if (node_->right_ != 0) {
					node_ = node_->right_->GetLeftMostNode();
					prev_ = 0;
				} else {
					// Go up as long as we are coming from the right.
					do {
						prev_ = node_;
						node_ = node_->parent_;
					} while(node_ != 0 && prev_ == node_->right_);
				}
			} else {
				// Go up as long as we are coming from the right.
				do {
					prev_ = node_;
					node_ = node_->parent_;
				} while(node_ != 0 && prev_ == node_->right_);
			}

			return *this;
		}
		Iterator  operator++(int) {
			Iterator i(node_, prev_); ++(*this); return i;
		}

		// Pre- and postfix decrement operators for traversing a tree.
		Iterator& operator--() {
			if (node_ == 0) {
				return *this;
			}

			if (prev_ == 0 || prev_ == node_->right_) {
				// Try to go left.
				if (node_->left_ != 0) {
					node_ = node_->left_->GetRightMostNode();
					prev_ = 0;
				} else {
					// Go up as long as we are coming from the left.
					do {
						prev_ = node_;
						node_ = node_->parent_;
					} while(node_ != 0 && prev_ == node_->left_);
				}
			} else {
				// Go up as long as we are coming from the left.
				do {
					prev_ = node_;
					node_ = node_->parent_;
				} while(node_ != 0 && prev_ == node_->left_);
			}

			return *this;
		}
		Iterator  operator--(int) {
			Iterator i(node_, prev_); --(*this); return i;
		}

	private:

		Iterator(Node* node) : node_(node), prev_(0){}
		Iterator(Node* node, Node* prev) : node_(node), prev_(prev){}

		Node* node_;
		Node* prev_;
	};

	class ConstIterator {
	public:
		friend class BinTree;

		ConstIterator() : node_(0){}
		ConstIterator(const Iterator& iterator) : node_(iterator.node_){}

		ConstIterator& operator = (const ConstIterator& other) {
			node_ = other.node_;

			return *this;
		}

		ConstIterator& operator = (const ConstIterator* other) {
			node_ = other->node_;

			return *this;
		}

		ConstIterator& operator = (const Iterator& other) {
			node_ = other.node_;

			return *this;
		}

		ConstIterator& operator = (const Iterator* other) {
			node_ = other->node_;

			return *this;
		}


		// Logical equality operator for iterator comparisons.
		bool operator==(const ConstIterator& other) const		{ return node_ == other.node_; }
		bool operator==(const Iterator& other) const			{ return node_ == other.node_; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const ConstIterator& other) const		{ return !operator==(other); }
		bool operator!=(const Iterator& other) const			{ return !operator==(other); }

		// Gets the contents of the iterator.
		_TObject& operator*() const	{ return node_->object_; }

		ConstIterator& operator++() {
			if (node_ == 0) {
				return *this;
			}

			if (prev_ == 0 || prev_ == node_->left_) {
				// Try to go right.
				if (node_->right_ != 0) {
					node_ = node_->right_->GetLeftMostNode();
					prev_ = 0;
				} else {
					// Go up as long as we are coming from the right.
					do {
						prev_ = node_;
						node_ = node_->parent_;
					} while(node_ != 0 && prev_ == node_->right_);
				}
			} else {
				// Go up as long as we are coming from the right.
				do {
					prev_ = node_;
					node_ = node_->parent_;
				} while(node_ != 0 && prev_ == node_->right_);
			}

			return *this;
		}
		ConstIterator  operator++(int) {
			ConstIterator i(node_, prev_); ++(*this); return i;
		}

		// Pre- and postfix decrement operators for traversing a tree.
		ConstIterator& operator--() {
			if (node_ == 0) {
				return *this;
			}

			if (prev_ == 0 || prev_ == node_->right_) {
				// Try to go left.
				if (node_->left_ != 0) {
					node_ = node_->left_->GetRightMostNode();
					prev_ = 0;
				} else {
					// Go up as long as we are coming from the left.
					do {
						prev_ = node_;
						node_ = node_->parent_;
					} while(node_ != 0 && prev_ == node_->left_);
				}
			} else {
				// Go up as long as we are coming from the left.
				do {
					prev_ = node_;
					node_ = node_->parent_;
				} while(node_ != 0 && prev_ == node_->left_);
			}

			return *this;
		}
		ConstIterator  operator--(int) {
			ConstIterator i(node_, prev_); --(*this); return i;
		}

	private:

		ConstIterator(Node* node) : node_(node){}
		ConstIterator(Node* node, Node* prev) : node_(node), prev_(prev){}

		const Node* node_;
		const Node* prev_;
	};

	inline BinTree();
	inline ~BinTree();

	void Insert(_TKey key, _TObject object);

	void Remove(const Iterator& item);
	void Remove(_TKey key);
	void RemoveAll();

	// Will rearrange the nodes in the tree to make it perfectly balanced.
	void Balance();

	inline Iterator Find(_TKey key);
	inline Iterator FindN(int num);
	inline bool Exists(_TKey key);

	inline Iterator First();
	inline Iterator Last();
	inline Iterator End();

	inline ConstIterator First() const;
	inline ConstIterator Last() const;
	inline ConstIterator End() const;

	inline int GetCount() const;
	inline bool IsEmpty() const;

	// Copies the pointer to the node allocator of tree. tree is
	// considered the "owner" of the allocator, and is responsible of
	// deleting it. This means that you can't delete the owner tree
	// before the others.
	//
	// You can only call this function on empty trees. If the tree isn't
	// empty, the function has no effect.
	inline void ShareNodePool(BinTree* tree);

private:
	inline NodeAllocator* GetNodeAllocator();
	inline void SetNodeAllocator(NodeAllocator* node_allocator);

	inline Node* NewNode(_TKey key, _TObject object, Node* parent);
	inline void RecycleNode(Node* node);

	void Balance(int start, int end, Node* new_top_node);

	bool allocator_owner_;
	NodeAllocator* node_allocator_;

	int count_;
	Node* top_node_;
};

TEMPLATE QUAL::BinTree() :
	allocator_owner_(true),
	count_(0),
	top_node_(0) {
	node_allocator_ = new NodeAllocator;
}

TEMPLATE QUAL::~BinTree() {
	if (top_node_ != 0) {
		RecycleNode(top_node_);
	}

	if (allocator_owner_ == true) {
		delete node_allocator_;
	}
}

TEMPLATE int QUAL::GetCount() const {
	return count_;
}

TEMPLATE typename QUAL::Iterator QUAL::First() {
	if (top_node_ == 0) {
		return End();
	}

	Iterator _iter(top_node_->GetLeftMostNode());
	return _iter;
}

TEMPLATE typename QUAL::Iterator QUAL::Last() {
	if (top_node_ == 0) {
		return End();
	}

	// Keep going right until you reach the bottom.
	Iterator _iter(top_node_->GetRightMostNode());
	return _iter;
}

TEMPLATE typename QUAL::Iterator QUAL::End() {
	return Iterator(0);
}

TEMPLATE typename QUAL::ConstIterator QUAL::First() const {
	if (top_node_ == 0) {
		return End();
	}

	// Keep going left until you reach the bottom.
	ConstIterator _iter(top_node_);
	while (_iter.node_->left_ != 0) {
		_iter.node_ = _iter.node_->left_;
	}

	return _iter;
}

TEMPLATE typename QUAL::ConstIterator QUAL::Last() const {
	if (top_node_ == 0) {
		return End();
	}

	// Keep going right until you reach the bottom.
	ConstIterator _iter(top_node_);
	while (_iter.node_->right_ != 0) {
		_iter.node_ = _iter.node_->right_;
	}

	return _iter;
}

TEMPLATE typename QUAL::ConstIterator QUAL::End() const {
	return ConstIterator(0);
}

TEMPLATE bool QUAL::IsEmpty() const {
	return (top_node_ == 0);
}

TEMPLATE void QUAL::Insert(_TKey key, _TObject object) {
	if (top_node_ == 0) {
		top_node_ = NewNode(key, object, 0);
		count_ = 1;
	} else {
		Node* _node = NewNode(key, object, 0);
		if (top_node_->Insert(_node) != 0) {
			count_++;
		} else {
			// The node wasn't inserted since the key value already exist.
			RecycleNode(_node);
		}
	}
}

TEMPLATE void QUAL::Remove(const Iterator& iter) {
	Node* _node = iter.node_;

	if (_node != 0) {
		if (_node == top_node_) {
			top_node_ = _node->Unlink();
		} else {
			_node->Unlink();
		}

		_node->parent_ = 0;
		_node->left_   = 0;
		_node->right_  = 0;
		RecycleNode(_node);

		count_--;
	}
}

TEMPLATE void QUAL::Remove(_TKey key) {
	Node* _node = top_node_->Find(key);

	if (_node != 0) {
		if (_node == top_node_) {
			top_node_ = _node->Unlink();
		} else {
			_node->Unlink();
		}

		_node->parent_ = 0;
		_node->left_   = 0;
		_node->right_  = 0;
		RecycleNode(_node);

		count_--;
	}
}

TEMPLATE void QUAL::RemoveAll() {
	top_node_->Destroy(node_allocator_);
	top_node_ = 0;
	count_ = 0;
}

TEMPLATE typename QUAL::Iterator QUAL::Find(_TKey key) {
	if (top_node_ == 0) {
		return End();
	}

	Node* _node = top_node_->Find(key);

	if (_node == 0) {
		return End();
	}

	return Iterator(_node);
}

TEMPLATE typename QUAL::Iterator QUAL::FindN(int num) {
	Iterator _iter = First();
	for (int i = 0; i < num && _iter != End(); ++i, ++_iter){}
	return _iter;
}

TEMPLATE bool QUAL::Exists(_TKey key) {
	return (Find(key) != End());
}

TEMPLATE void QUAL::Balance() {
	if (count_ <= 2) {
		return;
	}

	int count = count_;

	int middle = count_ / 2;
	Iterator _iter = FindN(middle);
	Node* top_node = NewNode(_iter.node_->key_,
							   _iter.node_->object_,
							   0);
	Remove(_iter);

	Balance(0, count_, top_node);
	RemoveAll();
	top_node_ = top_node;
	count_ = count;
}

TEMPLATE void QUAL::Balance(int start, int end, Node* new_top_node) {
	if (end == start) {
		return;
	}

	int middle = (start + end) / 2;

	Iterator _iter = FindN(middle);
	Node* _node = NewNode(_iter.node_->key_,
							_iter.node_->object_,
							0);
	new_top_node->Insert(_node);

	Balance(start, middle, new_top_node);
	Balance(middle + 1, end, new_top_node);
}

TEMPLATE void QUAL::ShareNodePool(BinTree* tree) {
	if (IsEmpty() == true) {
		SetNodeAllocator(tree->GetNodeAllocator());
	}
}

TEMPLATE typename QUAL::NodeAllocator* QUAL::GetNodeAllocator() {
	return node_allocator_;
}

TEMPLATE void QUAL::SetNodeAllocator(NodeAllocator* node_allocator) {
	if (node_allocator_ != 0) {
		delete node_allocator_;
	}

	node_allocator_ = node_allocator;
	allocator_owner_ = false;
}

TEMPLATE typename QUAL::Node* QUAL::NewNode(_TKey key, _TObject object, Node* parent) {
	Node* _node = node_allocator_->Alloc();
	_node->Init(key, object, parent);
	return _node;
}

TEMPLATE void QUAL::RecycleNode(Node* node) {
	node_allocator_->Free(node);
}












//
// class Node
//

TEMPLATE QUAL::Node::Node() :
	parent_(0),
	left_(0),
	right_(0) {
}

TEMPLATE QUAL::Node::Node(_TKey key, _TObject object, Node* parent) :
	parent_(parent),
	left_(0),
	right_(0),
	key_(key),
	object_(object) {
}

TEMPLATE QUAL::Node::~Node() {
}

TEMPLATE void QUAL::Node::Init(_TKey key, _TObject object, Node* parent) {
	parent_ = parent;
	left_ = 0;
	right_ = 0;
	key_ = key;
	object_ = object;
}

TEMPLATE void QUAL::Node::Destroy(NodeAllocator* allocator) {
	if (left_ != 0) {
		left_->Destroy(allocator);
		left_ = 0;
	}
	if (right_ != 0) {
		right_->Destroy(allocator);
		right_ = 0;
	}

	allocator->Free(this);
}

TEMPLATE typename QUAL::Node* QUAL::Node::Insert(Node* node) {
	if (key_ == node->key_) {
		object_ = node->object_;

		// No new node was created.
		return 0;
	}

	if (node->key_ < key_) {
		if (left_ == 0) {
			left_ = node;
			node->parent_ = this;
			return left_;
		} else {
			return left_->Insert(node);
		}
	} else {
		if (right_ == 0) {
			right_ = node;
			node->parent_ = this;
			return right_;
		} else {
			return right_->Insert(node);
		}
	}
}

TEMPLATE typename QUAL::Node* QUAL::Node::Find(_TKey key) {
	if (key_ == key) {
		return this;
	} else if(key < key_) {
		if (left_ == 0) {
			return 0;
		}

		return left_->Find(key);
	} else {
		if (right_ == 0) {
			return 0;
		}

		return right_->Find(key);
	}
}

TEMPLATE const typename QUAL::Node* QUAL::Node::GetLeftMostNode() const {
	if (left_ == 0) {
		return this;
	} else {
		return left_->GetLeftMostNode();
	}
}

TEMPLATE typename QUAL::Node* QUAL::Node::GetLeftMostNode() {
	if (left_ == 0) {
		return this;
	} else {
		return left_->GetLeftMostNode();
	}
}

TEMPLATE const typename QUAL::Node* QUAL::Node::GetRightMostNode() const {
	if (right_ == 0) {
		return this;
	} else {
		return right_->GetRightMostNode();
	}
}

TEMPLATE typename QUAL::Node* QUAL::Node::GetRightMostNode() {
	if (right_ == 0) {
		return this;
	} else {
		return right_->GetRightMostNode();
	}
}

TEMPLATE void QUAL::Node::Relink(Node* old_node, Node* new_node) {
	if (left_ == old_node) {
		left_ = new_node;
	} else if(right_ == old_node) {
		right_ = new_node;
	}

	if (new_node != 0) {
		new_node->parent_ = this;
	}
}

TEMPLATE typename QUAL::Node* QUAL::Node::Unlink() {
	if (left_ == 0 && right_ == 0) {
		if (parent_ != 0) {
			parent_->Relink(this, 0);
		}
		return 0;
	} else if(left_ == 0 && right_ != 0) {
		if (parent_ != 0) {
			parent_->Relink(this, right_);
		} else {
			right_->parent_ = 0;
		}

		return right_;
	} else if(left_ != 0 && right_ == 0) {
		if (parent_ != 0) {
			parent_->Relink(this, left_);
		} else {
			left_->parent_ = 0;
		}

		return left_;
	} else {
		// This algorithm will keep the tree somewhat balanced,
		// if it's balanced already.

		Node* rl = right_->GetLeftMostNode();

		rl->left_ = left_;
		left_->parent_ = rl;


/*
		// This code is the original algorithm. If you uncomment this,
		// you should remove all the code below.
		if (parent_ != 0) {
			parent_->Relink(this, right_);
		}
		right_->parent_ = parent_;
*/



		if (rl != right_) {
			rl->parent_->Relink(rl, 0);
		}

		rl->parent_ = parent_;

		if (parent_ != 0) {
			parent_->Relink(this, rl);
		}

		if (rl != right_) {
			Node* rr = rl->GetRightMostNode();
			rr->right_ = right_;
			right_->parent_ = rr;
		}

		return right_;
	}
}



}



#undef TEMPLATE
#undef QUAL
