
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "unordered.h"	// To get ahold of std::hash<>.

//#pragma once
#pragma once

// JB: extending STL with const void* hashing. This will probably not work
// equally well on all STL implementations, so make sure to port it properly.
#define LEPRA_STD_HASHER hash
namespace std {

template<> struct LEPRA_STD_HASHER<const void *>
{
	enum { bucket_size = 1 };	// MSVC2010 compatibility. (Perhaps some type of optimization, unsure.)
	union __vp {
		size_t s;
		const void* p;
	};
	size_t operator()(const void *__x) const {
		__vp vp;
		vp.p = __x;
		return (vp.s);
	}
	bool operator()(const void* k1, const void* k2) const {	// MSVC2010 compatibility. (Perhaps some type of ordering, unsure.)
		return k1 == k2;
	}
};

}

typedef std::LEPRA_STD_HASHER<const void*> LEPRA_VOIDP_HASHER;
typedef std::LEPRA_STD_HASHER<int> LEPRA_ENUM_HASHER;
