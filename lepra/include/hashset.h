/*
	Class:  HashSet
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A hash set. (Which is about the same as a hash table, but the objects inserted
	are their own keys).

	To be able to index the table, the inserted object must be converted to a
	size_t. This is done through a hash function, which is defined like this:

	struct HashFunction {
		size_t operator()(const _TSetObject& object);
	};

	The objects must also be comparable using the equal-operator ==.
*/

#pragma once

#include "fastallocator.h"
#include "hasher.h"
#include "lepratypes.h"
#include "unordered.h"

#define TEMPLATE template<class _TSetObject, class _THash, unsigned SIZE>
#define QUAL HashSet<_TSetObject, _THash, SIZE>

namespace lepra {

template<class _TSetObject, class _THash = std::hash<_TSetObject>, unsigned SIZE = 256>
class HashSet {
public:
	friend class Iterator;

	typedef std::unordered_set<_TSetObject, _THash> _HashSet;

	class Iterator {
	public:

		friend class HashSet<_TSetObject, _THash, SIZE>;

		Iterator(){}

		Iterator& operator = (const Iterator& other) {
			iterator_ = other.iterator_;
			return *this;
		}

		// Logical equality/inequality operator for iterator comparisons.
		bool operator==(const Iterator& other) const		{ return iterator_ == other.iterator_; }
		bool operator!=(const Iterator& other) const		{ return !operator==(other); }

		// Gets the contents of the iterator.
		const _TSetObject& operator*() const	{ return *iterator_; }

		_TSetObject GetObject() const { return *iterator_; }

		// Prefix increment operator for traversing a list.
		Iterator& operator++() {
			++iterator_;
			return *this;
		}

		// Postfix increment operator for traversing a hash table.
		Iterator  operator++(int) {
			Iterator i(iterator_);
			++iterator_;
			return i;
		}

	private:
		Iterator(const typename _HashSet::iterator& _iterator) :
			iterator_(_iterator) {
		}
		typename _HashSet::iterator iterator_;
	};

	HashSet(unsigned _size = SIZE);
	~HashSet();

	inline bool IsEmpty() const;

	bool Insert(const _TSetObject& object);
	bool Remove(const _TSetObject& object);
	void Remove(const Iterator& iter);

	void RemoveAll();

	Iterator Find(const _TSetObject& object);
	inline bool Exist(const _TSetObject& object);

	Iterator First();
	Iterator End();

	int GetCount() const;

private:
	_HashSet hash_set_;
};

TEMPLATE int QUAL::GetCount() const {
	return ((int)hash_set_.size());
}

TEMPLATE QUAL::HashSet(unsigned _size) :
	hash_set_(_size) {
}

TEMPLATE QUAL::~HashSet() {
}

TEMPLATE bool QUAL::IsEmpty() const {
	return hash_set_.empty();
}

TEMPLATE void QUAL::RemoveAll() {
	hash_set_.clear();
}


TEMPLATE bool QUAL::Insert(const _TSetObject& object) {
	return hash_set_.insert(object).second;
}

TEMPLATE bool QUAL::Remove(const _TSetObject& object) {
	typename _HashSet::iterator _iter(hash_set_.find(object));
	bool removed = (_iter != hash_set_.end());
	if (removed == true) {
		hash_set_.erase(_iter);
	}

	return removed;
}

TEMPLATE void QUAL::Remove(const Iterator& iter) {
	hash_set_.erase(iter.iterator_);
}

TEMPLATE typename QUAL::Iterator QUAL::Find(const _TSetObject& object) {
	return Iterator(hash_set_.find(object));
}

TEMPLATE bool QUAL::Exist(const _TSetObject& object) {
	return (Find(object) != End());
}

TEMPLATE typename QUAL::Iterator QUAL::First() {
	return Iterator(hash_set_.begin());
}

TEMPLATE typename QUAL::Iterator QUAL::End() {
	return Iterator(hash_set_.end());
}

}

#undef TEMPLATE
#undef QUAL
