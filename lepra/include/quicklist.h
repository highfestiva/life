/*
	Class:  QuickList
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	The quicklist is a time efficient data structure which combines
	the advantages of both the linked list and the hash table. Due
	to the combination of these two data structures, the result is
	not very memory efficient.
*/

#pragma once

#include <list>
#include "hashtable.h"

#define TEMPLATE template<class T, class HashFunc>
#define QUAL QuickList<T, HashFunc>


namespace lepra {

template<class T, class HashFunc = HashTableHash<T>>
class QuickList {
private:
	typedef std::list<T> TList;
	typedef HashTable<T, typename std::list<T>::iterator, HashFunc, 32> TTable;
public:

	class ConstIterator;

	class Iterator {
	public:
		friend class QuickList<T, HashFunc>;

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
		bool operator==(T other_item) const				{ return (*iter_) == other_item; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& other) const		{ return iter_ != other.iter_; }
		bool operator!=(const ConstIterator& other) const	{ return iter_ != other.iter_; }
		bool operator!=(T other_item) const				{ return (*iter_) != other_item; }

		// Gets the contents of the iterator.
		T& operator*() const	{ return (*iter_); }

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

		Iterator(const typename TList::Iterator& iter) : iter_(iter){}
		typename TList::Iterator iter_;
	};

	class ConstIterator {
	public:
		friend class QuickList<T, HashFunc>;

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
		bool operator==(T other_item) const				{ return (*iter_) == other_item; }

		// Logical inequality operator for iterator comparisons.
		bool operator!=(const Iterator& other) const		{ return iter_ != other.iter_; }
		bool operator!=(const ConstIterator& other) const	{ return iter_ != other.iter_; }
		bool operator!=(T other_item) const				{ return (*iter_) != other_item; }

		// Gets the contents of the iterator.
		T& operator*() const	{ return (*iter_); }

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

		ConstIterator(const typename TList::ConstIterator& iter) : iter_(iter){}
		typename TList::ConstIterator iter_;
	};

	inline QuickList();
	inline QuickList(const QuickList& other);
	inline ~QuickList();

	inline Iterator PushFront(T item);
	inline Iterator PushBack(T item);

	inline Iterator PushFrontUnique(T item);
	inline Iterator PushBackUnique(T item);

	inline T PopFront();
	inline T PopBack();

	inline Iterator InsertBefore(const Iterator& before_here, T item);
	inline Iterator InsertAfter(const Iterator& after_here, T item);

	inline void Remove(const Iterator& item);
	inline void Remove(T item);

	inline void RemoveAll();
	inline void DeleteAll();

	inline Iterator Find(T item);
	inline Iterator FindN(int num);
	inline bool Exists(T item);
	inline int GetIndex(T item); // Returns -1 if the item doesn't exist.

	// Will swap node position in the list. The iterators will also swap their contents,
	// so the same position in the list is conserved.
	inline void Swap(Iterator& it1, Iterator& it2);

	// Swaps if not already first/last in the list.
	inline void SwapWithNext(Iterator& it);
	inline void SwapWithPrevious(Iterator& it);

	// A special function used in caches. If there is a hit on an item in the cache,
	// it should be moved towards the beginning or the end of the list, to be able to
	// remove rarely used items first when the cache grows too large.
	inline void CompeteToFront(Iterator& it);
	inline void CompeteToLast(Iterator& it);

	inline Iterator First();
	inline Iterator Last();
	inline Iterator End();

	inline ConstIterator First() const;
	inline ConstIterator Last() const;
	inline ConstIterator End() const;

	inline int GetCount() const;
	inline bool IsEmpty() const;

	inline const QuickList& operator=(const QuickList& other);

private:

	TList list_;
	TTable table_;
};

TEMPLATE QUAL::QuickList() {
}

TEMPLATE QUAL::QuickList(const QuickList& other) :
	list_(other.list_),
	table_(other.table_) {
}

TEMPLATE QUAL::~QuickList() {
}

TEMPLATE typename QUAL::Iterator QUAL::PushFront(T item) {
	TList::Iterator _iter = list_.PushFront(item);
	table_.Insert(item, _iter);
	return Iterator(_iter);
}

TEMPLATE typename QUAL::Iterator QUAL::PushBack(T item) {
	TList::Iterator _iter = list_.PushBack(item);
	table_.Insert(item, _iter);
	return Iterator(_iter);
}

TEMPLATE typename QUAL::Iterator QUAL::PushFrontUnique(T item) {
	if (table_.Find(item) == table_.End()) {
		return PushFront(item);
	} else {
		return End();
	}
}

TEMPLATE typename QUAL::Iterator QUAL::PushBackUnique(T item) {
	if (table_.Find(item) == table_.End()) {
		return PushBack(item);
	} else {
		return End();
	}
}

TEMPLATE T QUAL::PopFront() {
	if (list_.IsEmpty() == true) {
		// Well... We have to return /something/.
		return list_.PopFront();
	} else {
		T object = list_.PopFront();
		table_.Remove(object);
		return object;
	}
}

TEMPLATE T QUAL::PopBack() {
	if (list_.IsEmpty() == true) {
		// Well... We have to return /something/.
		return list_.PopBack();
	} else {
		T object = list_.PopBack();
		table_.Remove(object);
		return object;
	}
}

TEMPLATE typename QUAL::Iterator QUAL::InsertBefore(const Iterator& before_here, T item) {
	TList::Iterator _iter(list_.InsertBefore(before_here.iter_, item));
	table_.Insert(item, _iter);
	return _iter;
}

TEMPLATE typename QUAL::Iterator QUAL::InsertAfter(const Iterator& after_here, T item) {
	TList::Iterator _iter(list_.InsertAfter(after_here.iter_, item));
	table_.Insert(item, _iter);
	return _iter;
}

TEMPLATE void QUAL::Remove(const Iterator& item) {
	list_.Remove(item.iter_);
	table_.Remove(*item.iter_);
}

TEMPLATE void QUAL::Remove(T item) {
	TTable::Iterator _iter(table_.Find(item));
	list_.Remove(*_iter);
	table_.Remove(_iter);
}

TEMPLATE void QUAL::RemoveAll() {
	list_.RemoveAll();
	table_.RemoveAll();
}

TEMPLATE void QUAL::DeleteAll() {
	list_.DeleteAll();
	table_.RemoveAll();
}

TEMPLATE typename QUAL::Iterator QUAL::Find(T item) {
	TTable::Iterator _iter(table_.Find(item));
	if (_iter == table_.End()) {
		return End();
	} else {
		return Iterator(*_iter);
	}
}

TEMPLATE typename QUAL::Iterator QUAL::FindN(int num) {
	// This function is unoptimized.
	return Iterator(list_.FindN(num));
}

TEMPLATE bool QUAL::Exists(T item) {
	return table_.Contains(item);
}

TEMPLATE int QUAL::GetIndex(T item) {
	// This function is unoptimized.
	return list_.GetIndex(item);
}

TEMPLATE void QUAL::Swap(Iterator& it1, Iterator& it2) {
	list_.Swap(it1.iter_, it2.iter_);
}

TEMPLATE void QUAL::SwapWithNext(Iterator& it) {
	list_.SwapWithNext(it.iter_);
}

TEMPLATE void QUAL::SwapWithPrevious(Iterator& it) {
	list_.SwapWithPrevious(it.iter_);
}

TEMPLATE void QUAL::CompeteToFront(Iterator& it) {
	list_.CompeteToFront(it.iter_);
}

TEMPLATE void QUAL::CompeteToLast(Iterator& it) {
	list_.CompeteToLast(it.iter_);
}

TEMPLATE typename QUAL::Iterator QUAL::First() {
	return Iterator(list_.First());
}

TEMPLATE typename QUAL::Iterator QUAL::Last() {
	return Iterator(list_.Last());
}

TEMPLATE typename QUAL::Iterator QUAL::End() {
	return Iterator(list_.End());
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

TEMPLATE int QUAL::GetCount() const {
	return list_.GetCount();
}

TEMPLATE bool QUAL::IsEmpty() const {
	return list_.IsEmpty();
}

TEMPLATE const QuickList<T, HashFunc>& QUAL::operator=(const QuickList& other) {
	list_ = other.list_;
	table_ = other.table_;
}

}

#undef TEMPLATE
#undef QUAL
