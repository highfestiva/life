
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

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

typedef std::LEPRA_STD_HASHER<const void*> LEPRA_VOIDP_HASHER;
typedef std::hash<int> LEPRA_ENUM_HASHER;

#endif

