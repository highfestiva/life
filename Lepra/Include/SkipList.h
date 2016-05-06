/*
JB: use std::map instead. It has a balanced red-black tree in the bottom and thus also a complexity of O(log n),
    however it is faster in many ways, such as the assignment operator (which is not even implemented here).



	Class:  SkipList
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A skip list (which is a Map, just like a hash table).

	Never heard of skip lists before? Well, you are not alone.

	It is a fast and SORTED(!) list (map) of key and value pairs. Each
	key maps to exactly one value (or object). Thus, there cannot be
	more than one value per key.

	The list is sorted in ascending key order.

	All operations (insert, remove and find) have a time complexity
	of O(log n).

	The skip list stores redundant data, and requires about twice as
	much memory as a binary tree for instance.

	So what's the point using a skip list instead of a binary tree?

	Well... I have no idea.

	In the worst case, both a binary tree and a skip list will perform
	just as bad as a linked list, which is O(n). But since a skip list
	is using a probabilistic approach, there is always a high probability
	that it will perform at O(log n).

	If you can guarantee that your binary tree is always perfecly balanced,
	and if you can do that in constant time, then using a skip list is just
	plain stupid. :)
/

#pragma once

#include "fastallocator.h"
#include "lepratypes.h"
#include "Random.h"

#define TEMPLATE template<class _TKey, class _TObject>
#define QUAL SkipList<_TKey, _TObject>

namespace lepra {

template<class _TKey, class _TObject>
class SkipList {
private:
	class ListNode {
	public:

		inline ListNode() : next_(0), prev_(0), mUp(0), mDown(0) {}

		void Init() {
			next_ = 0;
			prev_ = 0;
			mUp   = 0;
			mDown = 0;
		}

		ListNode* next_;
		ListNode* prev_;
		ListNode* mUp;
		ListNode* mDown;

		_TKey     key_;
		_TObject  object_;
	};

public:
	class ConstIterator;

	class Iterator {
	public:
		friend class SkipList;

		Iterator() : node_(0){}
		Iterator(const Iterator& iterator) : node_(iterator.node_){}

		Iterator& operator = (const Iterator& other) {
			node_ = other.node_;

			return *this;
		}

		Iterator& operator = (const Iterator* other) {
			node_ = other->node_;

			return *this;
		}

		// Logical equality operator for iterator comparisons.
		inline bool operator==(const Iterator& other) const		{ return node_ == other.node_; }
		inline bool operator==(const ConstIterator& other) const	{ return node_ == other.node_; }

		// Logical inequality operator for iterator comparisons.
		inline bool operator!=(const Iterator& other) const		{ return !operator==(other); }
		inline bool operator!=(const ConstIterator& other) const	{ return !operator==(other); }

		// Gets the contents of the iterator.
		_TKey& GetKey() const		{ return (node_->key_); }
		_TObject& operator*() const	{ return node_->object_; }

		// Pre- and postfix increment operators for traversing a list.
		inline Iterator& operator++() {
			node_ = node_->next_;
			if (node_->next_ == 0)	// Set to end.
				node_ = 0;
			return *this;
		}
		inline Iterator  operator++(int) {
			ListNode* temp = node_;
			node_ = node_->next_;
			if (node_->next_ == 0)	// Set to end.
				node_ = 0;
			return Iterator(temp);
		}

		// Pre- and postfix decrement operators for traversing a list.
		inline Iterator& operator--() {
			node_ = node_->prev_;
			if (node_->prev_ == 0)	// Set to end.
				node_ = 0;
			return *this;
		}
		inline Iterator operator--(int) {
			ListNode* temp = node_;
			node_ = node_->prev_;
			if (node_->prev_ == 0)	// Set to end.
				node_ = 0;
			return Iterator(temp);
		}

		Iterator& operator+=(int count) {
			for (int i = 0; i < count; i++, ++*this);
			return *this;
		}

		Iterator& operator-=(int count) {
			for (int i = 0; i < count; i++, --*this);
			return *this;
		}

		Iterator operator + (int count) {
			Iterator i(node_);
			i += count;
			return i;
		}

		Iterator operator - (int count) {
			Iterator i(node_);
			i -= count;
			return i;
		}

	private:

		Iterator(ListNode* node) : node_(node){}
		ListNode* node_;
	};

	class ConstIterator {
	public:
		friend class SkipList;

		ConstIterator() : node_(0){}
		ConstIterator(const ConstIterator& iterator) : node_(iterator.node_){}

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
		bool operator==(const Iterator& other) const		{ return node_ == other.node_; }
		bool operator==(const ConstIterator& other) const	{ return node_ == other.node_; }
		bool operator==(_TObject other_object) const			{ return node_->object_ == other_object; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& other) const		{ return !operator==(other); }
		bool operator!=(const ConstIterator& other) const	{ return !operator==(other); }
		bool operator!=(_TObject other_object) const			{ return !operator==(other_object); }

		// Gets the contents of the iterator.
		_TKey& GetKey() const		{ return (node_->key_); }
		_TObject& operator*() const	{ return node_->object_; }

		// Pre- and postfix increment operators for traversing a list.
		inline ConstIterator& operator++() {
			node_ = node_->next_;
			if (node_->next_ == 0)	// Set to end.
				node_ = 0;
			return *this;
		}
		inline ConstIterator  operator++(int) {
			ListNode* temp = node_;
			node_ = node_->next_;
			if (node_->next_ == 0)	// Set to end.
				node_ = 0;
			return ConstIterator(temp);
		}

		// Pre- and postfix decrement operators for traversing a list.
		inline ConstIterator& operator--() {
			node_ = node_->prev_;
			if (node_->prev_ == 0)	// Set to end.
				node_ = 0;
			return *this;
		}
		inline ConstIterator  operator--(int) {
			ListNode* temp = node_;
			node_ = node_->prev_;
			if (node_->prev_ == 0)	// Set to end.
				node_ = 0;
			return ConstIterator(temp);
		}

		ConstIterator& operator+=(int count) {
			for (int i = 0; i < count; i++, ++*this);
			return *this;
		}

		ConstIterator& operator-=(int count) {
			for (int i = 0; i < count; i++, --*this);
			return *this;
		}

		ConstIterator operator + (int count) {
			ConstIterator i(node_);
			i += count;
			return i;
		}

		ConstIterator operator - (int count) {
			ConstIterator i(node_);
			i -= count;
			return i;
		}

	private:

		ConstIterator(const ListNode* node) : node_(node){}
		const ListNode* node_;
	};

	SkipList();
	virtual ~SkipList();

	void Insert(const _TKey& key, const _TObject& object);
	void Remove(const _TKey& key);
	void Remove(const Iterator& iter);

	void RemoveAll();

	_TObject FindObject(const _TKey& key) const;
	Iterator Find(const _TKey& key) const;

	inline Iterator First() const;
	inline Iterator Last() const;
	inline Iterator End() const;

	inline int GetCount() const;
	inline bool IsEmpty() const;

	// Copies the pointer to the node allocator of list. list is
	// considered the "owner" of the allocator, and is responsible of
	// deleting it. This means that you can't delete the owner list
	// before the others.
	//
	// You can only call this function on empty lists. If the list isn't
	// empty, the function has no effect.
	inline void ShareNodePool(SkipList* list);

protected:
private:
	typedef FastAllocator<ListNode> NodeAllocator;

	inline NodeAllocator* GetNodeAllocator();
	inline void SetNodeAllocator(NodeAllocator* node_allocator);

	inline ListNode* NewNode();
	inline void RecycleNode(ListNode* node);

	void AddNewTopLevel();
	void RemoveTopLevel();

	ListNode* FindClosestNode(ListNode* left, const _TKey& key) const;
	ListNode* SearchLevel(ListNode* left, const _TKey& key) const;
	ListNode* Insert(ListNode* left, const _TKey& key, const _TObject& object);

	inline bool IsEndNode(ListNode* node) const;

	bool allocator_owner_;
	NodeAllocator* node_allocator_;

	ListNode* mTopLeft;
	ListNode* mBottomLeft;
	ListNode* mBottomRight;

	int count_;
};

TEMPLATE QUAL::SkipList() :
	allocator_owner_(true),
	node_allocator_(0),
	mTopLeft(0),
	mBottomLeft(0),
	mBottomRight(0),
	count_(0) {
	node_allocator_ = new NodeAllocator();
	AddNewTopLevel();
}

TEMPLATE QUAL::~SkipList() {
	// Delete all nodes...
	while (mTopLeft != 0) {
		RemoveTopLevel();
	}

	if (allocator_owner_ == true) {
		delete node_allocator_;
	}
}

TEMPLATE void QUAL::Insert(const _TKey& key, const _TObject& object) {
	ListNode* lNodeDown = Insert(mTopLeft, key, object);

	while (lNodeDown != 0) {
		AddNewTopLevel();

		// Create the new node to insert.
		ListNode* new_node = NewNode();
		new_node->key_ = key;
		new_node->object_ = object;

		// Insert horizontally at the current level.
		new_node->prev_   = mTopLeft;
		new_node->next_   = mTopLeft->next_;
		mTopLeft->next_   = new_node;
		new_node->next_->prev_ = new_node;

		// Link up and down.
		new_node->mDown = lNodeDown;
		lNodeDown->mUp  = new_node;

		// Randomize with 50% probability if we should add this node
		// at the higher level.
		if (Random::GetRandomNumber()&0x400) {	// 50-50: bit is 1 or 0.
			lNodeDown = new_node;
		} else {
			lNodeDown = 0;
		}
	}
}

TEMPLATE typename QUAL::ListNode* QUAL::Insert(ListNode* left,
					       const _TKey& key,
					       const _TObject& object) {
	ListNode* node = SearchLevel(left, key);

	if (node->prev_ != 0 && node->key_ == key) {
		// The key already exist, so just update the contents (the object).
		while (node != 0) {
			node->object_ = object;
			node = node->mDown;
		}
		return 0;
	}

	bool lCreateNewNode = false;
	ListNode* lNodeDown = 0;

	// If we are at the lowest level.
	if (node->mDown == 0) {
		lCreateNewNode = true;
		count_++;
	} else {
		// Continue searching down.
		lNodeDown = Insert(node->mDown, key, object);
		lCreateNewNode = (lNodeDown != 0);
	}

	if (lCreateNewNode == true) {
		// Create the new node to insert.
		ListNode* new_node = NewNode();
		new_node->key_ = key;
		new_node->object_ = object;

		// Insert horizontally at the current level.
		new_node->prev_   = node;
		new_node->next_   = node->next_;
		node->next_      = new_node;
		new_node->next_->prev_ = new_node;

		if (lNodeDown != 0) {
			// Link up and down.
			new_node->mDown = lNodeDown;
			lNodeDown->mUp  = new_node;
		}

		// Randomize with 50% probability if we should add this node
		// at the higher level.
		if (Random::GetRandomNumber()&0x10) {	// 50-50: bit is 1 or 0.
			return new_node;
		}
	}

	return 0;
}

TEMPLATE void QUAL::Remove(const _TKey& key) {
	ListNode* node = FindClosestNode(mTopLeft, key);

	if (IsEndNode(node) == false && node->key_ == key) {
		while (node != 0) {
			node->prev_->next_ = node->next_;
			node->next_->prev_ = node->prev_;

			ListNode* temp = node;
			node = node->mDown;
			// Hugge-TODO for JB: This fix is wrong. Try to find your bug again.
			//if (node)	// JB-TODO for Hugge: verify that this check is valid.
			{
				RecycleNode(temp);
			}
		}

		count_--;
	}
}

TEMPLATE void QUAL::Remove(const Iterator& iter) {
	ListNode* node = iter.node_;

	if (node == 0 || IsEndNode(node)) {
		return;
	}

	count_--;

	while (node != 0) {
		node->prev_->next_ = node->next_;
		node->next_->prev_ = node->prev_;

		ListNode* temp = node;
		node = node->mUp;

		// Hugge: Fixed the bug. TODO for JB: Verify that it works.
		//if (lNode)	// JB-TODO for Hugge: verify that this code is correct. At least it doesn't crash anymore. :)
		{
			RecycleNode(temp);
		}
	}
}

TEMPLATE void QUAL::RemoveAll() {
	// Delete all nodes...
	while (mTopLeft != 0) {
		RemoveTopLevel();
	}
	count_ = 0;

	AddNewTopLevel();
}

TEMPLATE _TObject QUAL::FindObject(const _TKey& key) const {
	ListNode* node = FindClosestNode(mTopLeft, key);

	if (node->key_ == key) {
		return node->object_;
	} else {
		return 0;
	}
}

TEMPLATE typename QUAL::Iterator QUAL::Find(const _TKey& key) const {
	ListNode* node = FindClosestNode(mTopLeft, key);

	if (IsEndNode(node) == true) {
		return End();
	}

	if (node->key_ == key) {
		return Iterator(node);
	} else {
		return End();
	}
}

TEMPLATE typename QUAL::Iterator QUAL::First() const {
	if (IsEndNode(mBottomLeft->next_) == true) {
		return End();
	}
	return Iterator(mBottomLeft->next_);
}

TEMPLATE typename QUAL::Iterator QUAL::Last() const {
	if (IsEndNode(mBottomRight->prev_) == true) {
		return End();
	}
	return Iterator(mBottomRight->prev_);
}

TEMPLATE typename QUAL::Iterator QUAL::End() const {
	return Iterator(0);
}

TEMPLATE int QUAL::GetCount() const {
	return count_;
}

TEMPLATE bool QUAL::IsEmpty() const {
	return (count_ == 0);
}

TEMPLATE void QUAL::AddNewTopLevel() {
	ListNode* top_left  = NewNode();
	ListNode* top_right = NewNode();

	top_left->next_  = top_right;
	top_right->prev_ = top_left;

	if (mTopLeft != 0) {
		top_left->mDown = mTopLeft;
		mTopLeft->mUp   = top_left;

		// Find the rightmost node.
		ListNode* temp = mTopLeft;
		while (temp->next_ != 0) {
			temp = temp->next_;
		}

		top_right->mDown = temp;
		temp->mUp = top_right;
	}

	mTopLeft = top_left;

	if (mBottomLeft == 0 || mBottomRight == 0) {
		mBottomLeft  = top_left;
		mBottomRight = top_right;
	}
}

TEMPLATE void QUAL::RemoveTopLevel() {
	if (mTopLeft != 0) {
		ListNode* node = mTopLeft;
		mTopLeft = mTopLeft->mDown;

		while (node != 0) {
			if (node->mDown != 0) {
				node->mDown->mUp = 0;
			}

			ListNode* temp = node;
			node = node->next_;
			RecycleNode(temp);
		}
	}

	if (mTopLeft == 0) {
		mBottomLeft  = 0;
		mBottomRight = 0;
	}
}

TEMPLATE typename QUAL::ListNode* QUAL::FindClosestNode(ListNode* left, const _TKey& key) const {
	// Search this level...
	ListNode* node = SearchLevel(left, key);

	if (node->key_ == key ||
	   node->mDown == 0) {
		// We have found the node.
		return node;
	}

	if (node->mDown != 0) {
		// Step back and down one level, and continue searching.
		return FindClosestNode(node->mDown, key);
	} else {
		return node;
	}
}

TEMPLATE typename QUAL::ListNode* QUAL::SearchLevel(ListNode* left, const _TKey& key) const {
	ListNode* current_node = left;

	while (current_node->next_ != 0 &&
		  (current_node->prev_ == 0 ||
		  current_node->key_ < key)) {
		current_node = current_node->next_;
	}

	// Have we found the node?
	if (current_node->key_ == key) {
		return current_node;
	} else {
		return current_node->prev_;
	}
}

TEMPLATE void QUAL::ShareNodePool(SkipList* list) {
	if (IsEmpty() == true) {
		SetNodeAllocator(list->GetNodeAllocator());
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

TEMPLATE typename QUAL::ListNode* QUAL::NewNode() {
	ListNode* node = node_allocator_->Alloc();
	node->Init();
	return node;
}

TEMPLATE void QUAL::RecycleNode(ListNode* node) {
	node_allocator_->Free(node);
}

TEMPLATE bool QUAL::IsEndNode(ListNode* node) const {
	return ((node->next_ == 0) || (node->prev_ == 0));
}

}

#undef TEMPLATE
#undef QUAL
*/
