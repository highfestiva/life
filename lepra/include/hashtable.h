
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


// To be able to index the table, the key must be converted to a size_t.
// The keys must be comparable using the equal-operator ==.



#pragma once

#include "string.h"	// Pulls in both basich hashing and narrow and wide string hashing.
#include "unordered.h"

#define HASHTABLE_TEMPLATE template<class _TTableKey, class _TTableObject, class _THash, unsigned SIZE>
#define HASHTABLE_QUAL HashTable<_TTableKey, _TTableObject, _THash, SIZE>



namespace lepra {



template<class _TTableKey, class _TTableObject, class _THash = std::hash<_TTableKey>, unsigned SIZE = 64>
class HashTable {
public:
	friend class Iterator;

	typedef std::unordered_map<_TTableKey, _TTableObject, _THash> HashMap;

	class Iterator {
	public:

		friend class HashTable<_TTableKey, _TTableObject, _THash, SIZE>;

		Iterator(){}

		Iterator& operator = (const Iterator& other) {
			iterator_ = other.iterator_;
			return *this;
		}

		// Logical equality/inequality operator for iterator comparisons.
		bool operator==(const Iterator& other) const		{ return iterator_ == other.iterator_; }
		bool operator!=(const Iterator& other) const		{ return !operator==(other); }

		// Gets the contents of the iterator.
		_TTableObject& operator*() const	{ return (*iterator_).second; }

		_TTableObject& GetObject() const { return (*iterator_).second; }
		_TTableKey    GetKey() const { return (*iterator_).first; }

		// Prefix increment operator for traversing a hash table.
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
		Iterator(const typename HashMap::iterator& _iterator) :
			iterator_(_iterator) {
		}
		typename HashMap::iterator iterator_;
	};

	class ConstIterator {
	public:

		friend class HashTable<_TTableKey, _TTableObject, _THash, SIZE>;

		ConstIterator(){}

		ConstIterator& operator = (const ConstIterator& other) {
			iterator_ = other.iterator_;
			return *this;
		}

		// Logical equality/inequality operator for iterator comparisons.
		bool operator==(const ConstIterator& other) const		{ return iterator_ == other.iterator_; }
		bool operator!=(const ConstIterator& other) const		{ return !operator==(other); }

		// Gets the contents of the iterator.
		const _TTableObject& operator*() const	{ return (*iterator_).second; }

		const _TTableObject& GetObject() const { return (*iterator_).second; }
		_TTableKey    GetKey() const { return (*iterator_).first; }

		// Prefix increment operator for traversing a hash table.
		ConstIterator& operator++() {
			++iterator_;
			return *this;
		}

		// Postfix increment operator for traversing a hash table.
		ConstIterator  operator++(int) {
			Iterator i(iterator_);
			++iterator_;
			return i;
		}

	private:
		ConstIterator(const typename HashMap::const_iterator& _iterator) :
			iterator_(_iterator) {
		}
		typename HashMap::const_iterator iterator_;
	};

	HashTable(unsigned _size = SIZE);
	HashTable(const HashTable& other);
	~HashTable();

	void Insert(const _TTableKey& key, const _TTableObject& object);
	void Remove(const _TTableKey& key);
	void Remove(const Iterator& iter);

	void RemoveAll();

	_TTableObject FindObject(const _TTableKey& key) const;
	Iterator Find(const _TTableKey& key);
	ConstIterator Find(const _TTableKey& key) const;

	inline Iterator First();
	inline Iterator End();

	inline ConstIterator First() const;
	inline ConstIterator End() const;

	inline int GetCount() const;
	inline bool IsEmpty() const;
	inline bool Contains(const _TTableKey& key) const;

	const HashTable& operator=(const HashTable& other);

private:
	HashMap hash_map_;
};

HASHTABLE_TEMPLATE int HASHTABLE_QUAL::GetCount() const {
	return (int)hash_map_.size();
}

HASHTABLE_TEMPLATE bool HASHTABLE_QUAL::IsEmpty() const {
	return hash_map_.empty();
}

HASHTABLE_TEMPLATE HASHTABLE_QUAL::HashTable(unsigned _size) :
	hash_map_(_size) {
}

HASHTABLE_TEMPLATE HASHTABLE_QUAL::HashTable(const HashTable& other) :
	hash_map_(other.hash_map_) {
}

HASHTABLE_TEMPLATE HASHTABLE_QUAL::~HashTable() {
}


HASHTABLE_TEMPLATE void HASHTABLE_QUAL::RemoveAll() {
	hash_map_.clear();
}


HASHTABLE_TEMPLATE void HASHTABLE_QUAL::Insert(const _TTableKey& key, const _TTableObject& object) {
	hash_map_.insert(typename HashMap::value_type(key, object));
}

HASHTABLE_TEMPLATE void HASHTABLE_QUAL::Remove(const _TTableKey& key) {
	hash_map_.erase(key);
}

HASHTABLE_TEMPLATE void HASHTABLE_QUAL::Remove(const Iterator& iter) {
	hash_map_.erase(iter.iterator_);
}

HASHTABLE_TEMPLATE _TTableObject HASHTABLE_QUAL::FindObject(const _TTableKey& key) const {
	typename HashMap::const_iterator _iter(hash_map_.find(key));
	if (_iter != hash_map_.end()) {
		return (*_iter).second;
	}
	return(0);
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::Iterator HASHTABLE_QUAL::Find(const _TTableKey& key) {
	return Iterator(hash_map_.find(key));
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::ConstIterator HASHTABLE_QUAL::Find(const _TTableKey& key) const {
	return ConstIterator(hash_map_.find(key));
}

HASHTABLE_TEMPLATE bool HASHTABLE_QUAL::Contains(const _TTableKey& key) const {
	return hash_map_.find(key) != hash_map_.end();
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::Iterator HASHTABLE_QUAL::First() {
	return Iterator(hash_map_.begin());
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::Iterator HASHTABLE_QUAL::End() {
	return Iterator(hash_map_.end());
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::ConstIterator HASHTABLE_QUAL::First() const {
	return ConstIterator(hash_map_.begin());
}

HASHTABLE_TEMPLATE typename HASHTABLE_QUAL::ConstIterator HASHTABLE_QUAL::End() const {
	return ConstIterator(hash_map_.end());
}

HASHTABLE_TEMPLATE const HASHTABLE_QUAL& HASHTABLE_QUAL::operator=(const HashTable& other) {
	hash_map_ = other.hash_map_;
	return *this;
}



}



#undef HASHTABLE_TEMPLATE
#undef HASHTABLE_QUAL
