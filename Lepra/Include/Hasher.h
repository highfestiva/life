
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#include <hash_set>	// To get ahold of std::hash<>.

//#pragma once
#ifndef LEPRAHASHER_H
#define LEPRAHASHER_H

// JB: extending STL with const void* hashing. This will probably not work
// equally well on all STL implementations, so make sure to port it properly.
#define LEPRA_STD_HASHER hash
namespace std
{

template<> struct LEPRA_STD_HASHER<const void *>
{
	union __vp
	{
		size_t s;
		const void* p;
	};
	size_t operator()(const void *__x) const
	{
		__vp vp;
		vp.p = __x;
		return (vp.s);
	}
};

}

#endif

