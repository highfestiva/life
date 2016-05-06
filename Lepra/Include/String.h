
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
#include "hasher.h"
#include "lepratypes.h"



namespace lepra {
typedef std::string		str;
typedef std::wstring		wstr;
typedef std::stringstream	strstream;
typedef std::wstringstream	wstrstream;

inline size_t HashString(const char* s);
inline size_t HashString(const wchar_t* w);

class HashedString: public str {
public:
	inline HashedString() {
	}
	inline HashedString(const str& data):
		str(data),
		hash_(HashString(c_str())),
		value_(0),
		value_expire_count_(0) {
	}
	inline HashedString(const str::value_type* data):
		str(data),
		hash_(HashString(c_str())),
		value_(0),
		value_expire_count_(0) {
	}
	size_t hash_;
	mutable void* value_;
	mutable int value_expire_count_;
};

struct HashedStringHasher {
	size_t operator()(const HashedString& k) const {
		return k.hash_;
	}
};

inline bool operator==(const HashedString& a, const HashedString& b) {
	return (a.hash_ == b.hash_) && ((str&)a == (str&)b);
}
}



// We add our own STL hashers for string and wide.
namespace std {
inline size_t __stl_hash_astring(const char* s) {
	size_t h = 0;
	for (; *s; ++s)
		h = 101*h + (size_t)*s;
	return h;
}
inline size_t __stl_hash_wstring(const wchar_t* s) {
	size_t h = 0;
	for (; *s; ++s)
		h = 101*h + (size_t)*s;
	return h;
}
template<> struct LEPRA_STD_HASHER<wchar_t*>
{
	size_t operator()(const wchar_t* s) const {
		return __stl_hash_wstring(s);
	}
};
template<> struct LEPRA_STD_HASHER<const wchar_t*>
{
	size_t operator()(const wchar_t* s) const {
		return __stl_hash_wstring(s);
	}
};
/*template<> struct LEPRA_STD_HASHER<lepra::astr>
{
public:
	size_t operator() (const lepra::astr& s) const {
		return (priv::__stl_hash_string(s.c_str()));
	}
};
template<> struct LEPRA_STD_HASHER<lepra::wstr>
{
public:
	size_t operator() (const lepra::wstr& w) const {
		return (__stl_hash_wstring(w.c_str()));
	}
};*/
template<> struct LEPRA_STD_HASHER<lepra::HashedString>
{
	size_t operator()(const lepra::HashedString& s) const {
		return s.hash_;
	}
};
}



namespace lepra {
inline size_t HashString(const char* s) {
	return std::__stl_hash_astring(s);
}
inline size_t HashString(const wchar_t* w) {
	return std::__stl_hash_wstring(w);
}
}



#include "stringutility.h"
