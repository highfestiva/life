
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <algorithm>



namespace lepra {



class ListUtil {
public:
	template<class _ListType> static void DeleteAll(_ListType& list) {
		while(!list.empty()) {
			delete list.front();
			list.pop_front();
		}
	}
	template<class _ListType> static typename _ListType::iterator FindByIndex(_ListType& list, size_t index) {
		if(index < 0 || index >= list.size())
			return list.end();

		typename _ListType::iterator it;
		if(index <= list.size() / 2) {
			size_t i;
			for(i = 0, it = list.begin(); i < index; ++i, ++it)
			{}
		} else {
			size_t i;
			for(i = list.size() - 1, it = --list.end(); i > index; --i, --it)
			{}
		}

		return it;
	}
	// Returns -1 if not found.
	template<class _ListType> static int FindIndexOf(_ListType& list, const typename _ListType::value_type& value) {
		int _index = 0;
		typename _ListType::iterator it;
		for(it = list.begin(); it != list.end(); ++it, ++_index) {
			if(*it == value)
				return _index;
		}

		return -1;
	}

	template<class _ListType> static bool Contains(const _ListType& list, const typename _ListType::value_type& value) {
		return std::find(list.begin(), list.end(), value) != list.end();
	}
};


template<class _Type, class _ListType = std::vector<_Type*> >
class VectorUtil {
public:
	static void CloneList(_ListType& target, const _ListType& source) {
		const size_t cnt = source.size();
		target.resize(cnt);
		for (size_t x = 0; x < cnt; ++x) {
			target[x] = new _Type(*source[x]);
		}
	}

	static void CloneListFactoryMethod(_ListType& target, const _ListType& source) {
		const size_t cnt = source.size();
		target.resize(cnt);
		for (size_t x = 0; x < cnt; ++x) {
			target[x] = _Type::Create(*source[x]);
		}
	}
};



}
