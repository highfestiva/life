
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Hasher.h"
#include "LepraTypes.h"



namespace Lepra
{
typedef std::string	astr;
typedef std::wstring	wstr;
#ifdef LEPRA_UNICODE
typedef wstr		str;
#else // !LEPRA_UNICODE
typedef astr		str;
#endif // LEPRA_UNICODE/!LEPRA_UNICODE
}



// We add our own STL hashers for string and wide.
namespace std
{
inline size_t __stl_hash_wstring(const wchar_t* __w)
{
	unsigned long __h = 0;
	for ( ; *__w; ++__w)
	__h = 5*__h + *__w;
	return ((size_t)(__h));
}
template<> struct LEPRA_STD_HASHER<wchar_t*>
{
	size_t operator()(const wchar_t* __s) const
	{
		return (__stl_hash_wstring(__s));
	}
};
template<> struct LEPRA_STD_HASHER<const wchar_t*>
{
	size_t operator()(const wchar_t* __s) const
	{
		return (__stl_hash_wstring(__s));
	}
};
template<> struct LEPRA_STD_HASHER<Lepra::astr>
{
public:
	size_t operator() (const Lepra::astr& __s) const
	{
		return (priv::__stl_hash_string(__s.c_str()));
	}
};
template<> struct LEPRA_STD_HASHER<Lepra::wstr>
{
public:
	size_t operator() (const Lepra::wstr& __w) const
	{
		return (__stl_hash_wstring(__w.c_str()));
	}
};
}



namespace Lepra
{
inline size_t HashString(const char* __s)
{
	return (std::priv::__stl_hash_string(__s));
}
inline size_t HashString(const wchar_t* __w)
{
	return (std::__stl_hash_wstring(__w));
}
}



#include "StringUtility.h"
