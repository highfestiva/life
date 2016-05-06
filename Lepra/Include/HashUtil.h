
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



namespace lepra {



class HashUtil {
public:
	template<class _HashSetType> static typename _HashSetType::key_type
		FindSetObject(const _HashSetType& hash_set_type, typename _HashSetType::key_type key) {
		return FindSetObjectDefault(hash_set_type, key, typename _HashSetType::key_type(0));
	}

	template<class _HashSetType> static typename _HashSetType::key_type
		FindSetObjectDefault(const _HashSetType& hash_set_type, typename _HashSetType::key_type key,
			typename _HashSetType::key_type _default) {
		typename _HashSetType::const_iterator x = hash_set_type.find(key);
		if (x != hash_set_type.end()) {
			return *x;
		}
		return _default;
	}

	template<class _HashMapType> static typename _HashMapType::mapped_type
		FindMapObject(const _HashMapType& hash_map_type, typename _HashMapType::key_type key) {
		typename _HashMapType::const_iterator x = hash_map_type.find(key);
		if (x != hash_map_type.end()) {
			return x->second;
		}
		return typename _HashMapType::mapped_type(0);
	}

	template<class _HashMapType> static bool
		TryFindMapObject(const _HashMapType& hash_map_type, typename _HashMapType::key_type key,
			typename _HashMapType::mapped_type& value) {
		typename _HashMapType::const_iterator x = hash_map_type.find(key);
		if (x != hash_map_type.end()) {
			value = x->second;
			return true;
		}
		return false;
	}
};



}
