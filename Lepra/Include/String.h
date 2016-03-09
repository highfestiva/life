
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Hasher.h"
#include "LepraTypes.h"



namespace Lepra
{
typedef std::string		astr;
typedef std::wstring		wstr;
typedef std::stringstream	astrstream;
typedef std::wstringstream	wstrstream;
#ifdef LEPRA_UNICODE
typedef wstr			str;
typedef wstrstream		strstream;
#else // !LEPRA_UNICODE
typedef astr			str;
typedef astrstream		strstream;
#endif // LEPRA_UNICODE/!LEPRA_UNICODE

inline size_t HashString(const char* s);
inline size_t HashString(const wchar_t* w);

class HashedString: public str
{
public:
	inline HashedString()
	{
	}
	inline HashedString(const str& pData):
		str(pData),
		mHash(HashString(c_str())),
		mValue(0),
		mValueExpireCount(0)
	{
	}
	inline HashedString(const str::value_type* pData):
		str(pData),
		mHash(HashString(c_str())),
		mValue(0),
		mValueExpireCount(0)
	{
	}
	size_t mHash;
	mutable void* mValue;
	mutable int mValueExpireCount;
};

struct HashedStringHasher
{
	size_t operator()(const HashedString& k) const
	{
		return k.mHash;
	}
};

inline bool operator==(const HashedString& a, const HashedString& b)
{
	return (a.mHash == b.mHash) && ((*(str*)&a) == (*(str*)&b));
}
}



// We add our own STL hashers for string and wide.
namespace std
{
inline size_t __stl_hash_astring(const char* s)
{
	size_t h = 0;
	for (; *s; ++s)
		h = 101*h + (size_t)*s;
	return h;
}
inline size_t __stl_hash_wstring(const wchar_t* s)
{
	size_t h = 0;
	for (; *s; ++s)
		h = 101*h + (size_t)*s;
	return h;
}
template<> struct LEPRA_STD_HASHER<wchar_t*>
{
	size_t operator()(const wchar_t* s) const
	{
		return __stl_hash_wstring(s);
	}
};
template<> struct LEPRA_STD_HASHER<const wchar_t*>
{
	size_t operator()(const wchar_t* s) const
	{
		return __stl_hash_wstring(s);
	}
};
/*template<> struct LEPRA_STD_HASHER<Lepra::astr>
{
public:
	size_t operator() (const Lepra::astr& s) const
	{
		return (priv::__stl_hash_string(s.c_str()));
	}
};
template<> struct LEPRA_STD_HASHER<Lepra::wstr>
{
public:
	size_t operator() (const Lepra::wstr& w) const
	{
		return (__stl_hash_wstring(w.c_str()));
	}
};*/
template<> struct LEPRA_STD_HASHER<Lepra::HashedString>
{
	size_t operator()(const Lepra::HashedString& s) const
	{
		return s.mHash;
	}
};
}



namespace Lepra
{
inline size_t HashString(const char* s)
{
	return std::__stl_hash_astring(s);
}
inline size_t HashString(const wchar_t* w)
{
	return std::__stl_hash_wstring(w);
}
}



#include "StringUtility.h"
