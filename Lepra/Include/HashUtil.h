
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once



namespace Lepra
{



class HashUtil
{
public:
	template<class _HashSetType> static typename _HashSetType::key_type
		FindSetObject(const _HashSetType& pHashSetType, typename _HashSetType::key_type pKey)
	{
		typename _HashSetType::const_iterator x = pHashSetType.find(pKey);
		if (x != pHashSetType.end())
		{
			return (*x);	// TRICKY: RAII simplifies here.
		}
		return (_HashSetType::key_type(0));
	}

	template<class _HashMapType> static typename _HashMapType::data_type
		FindMapObject(const _HashMapType& pHashMapType, typename _HashMapType::key_type pKey)
	{
		typename _HashMapType::const_iterator x = pHashMapType.find(pKey);
		if (x != pHashMapType.end())
		{
			return (x->second);	// TRICKY: RAII simplifies here.
		}
		return (_HashMapType::data_type(0));
	}
};



}
