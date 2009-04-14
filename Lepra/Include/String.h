
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

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
typedef std::string AnsiString;
typedef std::wstring UnicodeString;
#ifdef LEPRA_UNICODE
typedef UnicodeString		String;
#else // !LEPRA_UNICODE
typedef AnsiString		String;
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
template<> struct LEPRA_STD_HASHER<Lepra::AnsiString>
{
public:
	size_t operator() (const Lepra::AnsiString& __s) const
	{
		return (priv::__stl_hash_string(__s.c_str()));
	}
};
template<> struct LEPRA_STD_HASHER<Lepra::UnicodeString>
{
public:
	size_t operator() (const Lepra::UnicodeString& __w) const
	{
		return (__stl_hash_wstring(__w.c_str()));
	}
};
}

#include "StringUtility.h"
