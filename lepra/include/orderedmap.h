
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "hashtable.h"

#define TEMPLATE template<class KeyType, class ObjectType, class HashFunc>
#define QUAL OrderedMap<KeyType, ObjectType, HashFunc>



namespace lepra {



template<class KeyType, class ObjectType, class HashFunc = std::hash<KeyType> >
class OrderedMap {
private:
	class Node {
	public:
		Node():
			object_(0) {
		};
		Node(KeyType key, ObjectType object) :
			key_(key),
			object_(object) {
		};

		KeyType key_;
		ObjectType object_;
	};

	// Used to avoid warning C4503 (truncated type names).
	struct IteratorContainer {
		IteratorContainer(const typename std::list<Node>::iterator& it) : it_(it) {}
		typename std::list<Node>::iterator it_;
	};

	typedef HashTable<KeyType, IteratorContainer, HashFunc, 32> TTable;
	typedef std::list<Node> NodeList;
public:

	class ConstIterator;

	class Iterator {
	public:
		friend class OrderedMap<KeyType, ObjectType, HashFunc>;

		Iterator(){}
		Iterator(const Iterator& _iterator) : iter_(_iterator.iter_){}

		Iterator& operator = (const Iterator& other) {
			iter_ = other.iter_;
			return *this;
		}

		Iterator& operator = (const Iterator* other) {
			iter_ = other->iter_;
			return *this;
		}

		// Logical equality operator for iterator comparisons.
		bool operator==(const Iterator& other) const		{ return iter_ == other.iter_; }
		bool operator==(const ConstIterator& other) const	{ return iter_ == other.iter_; }
		bool operator==(ObjectType other_object) const			{ return (*iter_) == other_object; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& other) const		{ return iter_ != other.iter_; }
		bool operator!=(const ConstIterator& other) const	{ return iter_ != other.iter_; }
		bool operator!=(ObjectType other_object) const			{ return (*iter_) != other_object; }

		// Gets the contents of the iterator.
		KeyType& GetKey() const { return (*iter_).key_; }
		ObjectType& GetObject() const { return (*iter_).object_; }

		// Pre- and postfix increment operators for traversing a list.
		Iterator& operator++()		{ ++iter_; return *this; }
		Iterator  operator++(int)	{ return Iterator(iter_++);}

		// Pre- and postfix decrement operators for traversing a list.
		Iterator& operator--()		{ --iter_; return *this; }
		Iterator  operator--(int)	{ return Iterator(iter_--); }

		Iterator& operator+=(int count) {
			iter_ += count;
			return *this;
		}

		Iterator& operator-=(int count) {
			iter_ -= count;
			return *this;
		}

		Iterator operator + (int count) {
			return Iterator(iter_ + count);
		}

		Iterator operator - (int count) {
			return Iterator(iter_ - count);
		}

	private:

		Iterator(const typename NodeList::iterator& iter) : iter_(iter){}
		typename NodeList::iterator iter_;
	};

	class ConstIterator {
	public:
		friend class OrderedMap<ObjectType, HashFunc>;

		ConstIterator(){}
		ConstIterator(const Iterator& _iterator) : iter_(_iterator.iter_){}
		ConstIterator(const ConstIterator& _iterator) : iter_(_iterator.iter_){}

		ConstIterator& operator = (const ConstIterator& other) {
			iter_ = other.iter_;
			return *this;
		}

		ConstIterator& operator = (const ConstIterator* other) {
			iter_ = other->iter_;
			return *this;
		}

		// Logical equality operator for iterator comparisons.
		bool operator==(const Iterator& other) const		{ return iter_ == other.iter_; }
		bool operator==(const ConstIterator& other) const	{ return iter_ == other.iter_; }
		bool operator==(ObjectType other_object) const				{ return (*iter_) == other_object; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& other) const		{ return iter_ != other.iter_; }
		bool operator!=(const ConstIterator& other) const	{ return iter_ != other.iter_; }
		bool operator!=(ObjectType other_object) const				{ return (*iter_) != other_object; }

		// Gets the contents of the iterator.
		KeyType& GetKey() const { return (*iter_).key_; }
		ObjectType& GetObject() const { return (*iter_).object_; }

		// Pre- and postfix increment operators for traversing a list.
		ConstIterator& operator++()		{ ++iter_; return *this; }
		ConstIterator  operator++(int)	{ return ConstIterator(iter_++);}

		// Pre- and postfix decrement operators for traversing a list.
		ConstIterator& operator--()		{ --iter_; return *this; }
		ConstIterator  operator--(int)	{ return ConstIterator(iter_--); }

		ConstIterator& operator+=(int count) {
			iter_ += count;
			return *this;
		}

		ConstIterator& operator-=(int count) {
			iter_ -= count;
			return *this;
		}

		ConstIterator operator + (int count) {
			return ConstIterator(iter_ + count);
		}

		ConstIterator operator - (int count) {
			return ConstIterator(iter_ - count);
		}

	private:

		ConstIterator(const typename NodeList::const_iterator& iter) : iter_(iter){}
		typename NodeList::const_iterator iter_;
	};

	inline OrderedMap();
	inline OrderedMap(const OrderedMap& other);
	inline ~OrderedMap();

	inline Iterator PushFront(KeyType key, ObjectType object);
	inline Iterator PushBack(KeyType key, ObjectType object);

	// Returns key and Object. Make sure the quick list isn't empty.
	inline void PopFront(KeyType& key, ObjectType& object);
	inline void PopBack(KeyType& key, ObjectType& object);

	inline Iterator InsertBefore(const Iterator& before_here, KeyType key, ObjectType object);
	inline Iterator InsertAfter(const Iterator& after_here, KeyType key, ObjectType object);

	inline void Remove(const Iterator& iter);
	inline bool Remove(KeyType key);

	inline void RemoveAll();

	inline Iterator Find(KeyType key);
	inline bool Exists(KeyType key);
	inline int GetIndex(KeyType key); // Returns -1 if the Object doesn't exist.

	// Will swap node position in the list. The iterators will also swap their contents,
	// so the same position in the list is conserved.
	inline void Swap(Iterator& it1, Iterator& it2);

	inline Iterator First();
	inline Iterator Last();
	inline Iterator End();

	inline ConstIterator First() const;
	inline ConstIterator Last() const;
	inline ConstIterator End() const;

	inline size_t GetCountSafe() const;
	inline size_t GetCount() const;
	inline bool IsEmpty() const;

	inline const OrderedMap& operator=(const OrderedMap& other);

private:

	NodeList list_;
	TTable table_;
};

TEMPLATE QUAL::OrderedMap() {
}

TEMPLATE QUAL::OrderedMap(const OrderedMap& other) :
	list_(other.list_),
	table_(other.table_) {
}

TEMPLATE QUAL::~OrderedMap() {
}

TEMPLATE typename QUAL::Iterator QUAL::PushFront(KeyType key, ObjectType object) {
	deb_assert(table_.Find(key) == table_.End());
	list_.push_front(Node(key, object));
	typename NodeList::iterator _iter = list_.begin();
	table_.Insert(key, _iter);
	deb_assert(table_.GetCount() == (int)list_.size());
	return Iterator(_iter);
}

TEMPLATE typename QUAL::Iterator QUAL::PushBack(KeyType key, ObjectType object) {
	deb_assert(table_.Find(key) == table_.End());
	list_.push_back(Node(key, object));
	typename NodeList::iterator _iter = list_.end();
	--_iter;
	table_.Insert(key, _iter);
	deb_assert(table_.GetCount() == (int)list_.size());
	return Iterator(_iter);
}

TEMPLATE void QUAL::PopFront(KeyType& key, ObjectType& object) {
	if (list_.empty() == false) {
		Node& node = list_.front();
		key = node.key_;
		object = node.object_;
		table_.Remove(node.key_);
		list_.pop_front();
	}
	deb_assert(table_.GetCount() == (int)list_.size());
}

TEMPLATE void QUAL::PopBack(KeyType& key, ObjectType& object) {
	if (list_.empty() == false) {
		Node& node = list_.back();
		key = node.key_;
		object = node.object_;
		table_.Remove(node.key_);
		list_.pop_back();
	}
	deb_assert(table_.GetCount() == (int)list_.size());
}

TEMPLATE typename QUAL::Iterator QUAL::InsertBefore(const Iterator& before_here, KeyType key, ObjectType object) {
	typename NodeList::Iterator _iter(list_.InsertBefore(before_here.iter_, Node(key, object)));
	table_.Insert(key, _iter);
	return _iter;
}

TEMPLATE typename QUAL::Iterator QUAL::InsertAfter(const Iterator& after_here, KeyType key, ObjectType object) {
	typename NodeList::Iterator _iter(list_.InsertAfter(after_here.iter_, Node(key, object)));
	table_.Insert(key, _iter);
	return _iter;
}

TEMPLATE void QUAL::Remove(const Iterator& iter) {
	deb_assert(table_.GetCount() == (int)list_.size());
	table_.Remove(iter.GetKey());
	list_.erase(iter.iter_);
	deb_assert(table_.GetCount() == (int)list_.size());
}

TEMPLATE bool QUAL::Remove(KeyType key) {
	deb_assert(table_.GetCount() == (int)list_.size());
	typename TTable::Iterator _iter(table_.Find(key));
	const bool erased = (_iter != table_.End());
	if (erased) {
		list_.erase((*_iter).it_);
		table_.Remove(_iter);
	}
	deb_assert(table_.GetCount() == (int)list_.size());
	return (erased);
}

TEMPLATE void QUAL::RemoveAll() {
	deb_assert(table_.GetCount() == (int)list_.size());
	list_.clear();
	table_.RemoveAll();
	deb_assert(table_.GetCount() == (int)list_.size());
}

TEMPLATE typename QUAL::Iterator QUAL::Find(KeyType key) {
	typename TTable::Iterator _iter(table_.Find(key));
	if (_iter == table_.End()) {
		return End();
	} else {
		return Iterator((*_iter).it_);
	}
}

TEMPLATE bool QUAL::Exists(KeyType key) {
	return table_.Contains(key);
}

TEMPLATE int QUAL::GetIndex(KeyType key) {
	int i = 0;
	int index = -1;

	typename NodeList::Iterator _iter;
	for(_iter = list_.First(); _iter != list_.End(); ++_iter, ++i) {
		if((*_iter).key_ == key) {
			index = i;
			break;
		}
	}

	return index;
}

TEMPLATE void QUAL::Swap(Iterator& it1, Iterator& it2) {
	list_.Swap(it1.iter_, it2.iter_);
}

TEMPLATE typename QUAL::Iterator QUAL::First() {
	return Iterator(list_.begin());
}

TEMPLATE typename QUAL::Iterator QUAL::Last() {
	return Iterator(--list_.end());
}

TEMPLATE typename QUAL::Iterator QUAL::End() {
	return Iterator(list_.end());
}

TEMPLATE typename QUAL::ConstIterator QUAL::First() const {
	return ConstIterator(list_.First());
}

TEMPLATE typename QUAL::ConstIterator QUAL::Last() const {
	return ConstIterator(list_.Last());
}

TEMPLATE typename QUAL::ConstIterator QUAL::End() const {
	return ConstIterator(list_.End());
}

TEMPLATE size_t QUAL::GetCountSafe() const {
	return (table_.GetCount());	// This is safe as long as STLport is using member integer for size() for unordered_map type.
}

TEMPLATE size_t QUAL::GetCount() const {
	deb_assert(table_.GetCount() == (int)list_.size());
	return (GetCountSafe());
}

TEMPLATE bool QUAL::IsEmpty() const {
	deb_assert(table_.GetCount() == (int)list_.size());
	return (table_.IsEmpty());
}

TEMPLATE const OrderedMap<KeyType, ObjectType, HashFunc>& QUAL::operator=(const OrderedMap& other) {
	list_ = other.list_;
	table_ = other.table_;
	return (*this);
}



}



#undef TEMPLATE
#undef QUAL
