
// Author: Jonas Bystrröm
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/stringutility.h"
#include "../include/lepraassert.h"
#include "../include/lepraos.h"
#ifdef _MSC_VER
#pragma warning(disable: 4127)	// Conditional if (sizeof()...) results in constant expression.
#pragma warning(disable: 4244)	// Converting UTF-32 code point to wchar_t on 16-bit system.
#endif // VC++.
#include <utf8.h>



namespace lepra {



template<> str strutil::VFormat(const char* format, va_list& arguments) {
	char buffer[1024];
#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // <VS 2005+>
	::vsprintf_s(buffer, sizeof(buffer), format, arguments);
#else // <VS 2003 .NET->
	::vsprintf(buffer, format, arguments);
#endif // <VS 2005+> / <VS 2003 .NET->
#else // !LEPRA_WINDOWS
	::vsnprintf(buffer, sizeof(buffer), format, arguments);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
	return (str(buffer));
}

template<> wstr wstrutil::VFormat(const wchar_t* format, va_list& arguments) {
	wchar_t buffer[1024];
#ifndef LEPRA_WINDOWS
	wchar_t _format[1024];
	int x = 0;
	bool is_percent = false;
	do {
		wchar_t ch = format[x];
		if (is_percent) {
			if (ch == '%' || ch == 'd' || ch == 'i' || ch == 'f' || ch == 'F' ||
				ch == 'x' || ch == 'X' || ch == 'c' || ch == 'C' || ch == 'p' ||
				ch == 'g' || ch == 'G' || ch == 'e' || ch == 'E') {
				is_percent = false;
			} else if (ch == 's') {
				ch = 'S';
				is_percent = false;
			}
		} else if (!is_percent && ch == '%') {
			is_percent = true;
		}
		_format[x] = ch;
	} while (format[x++]);
	format = _format;
#endif // !Windows
	WIDE_SPRINTF(buffer, sizeof(buffer)/sizeof(wchar_t), format, arguments);
	return (wstr(buffer));
}



template<> long strutil::StrToL(const char* s, char** end_ptr, int radix) {
	return (::strtol(s, end_ptr, radix));
}

template<> long wstrutil::StrToL(const wchar_t* s, wchar_t** end_ptr, int radix) {
	return (::wcstol(s, end_ptr, radix));
}

template<> bool strutil::StringToDouble(const str& s, double& value) {
	if (s.empty()) {
		return (false);
	}
	char* _end_ptr;
	value = ::strtod(s.c_str(), &_end_ptr);
	return (s.c_str()+s.length() == _end_ptr);
}

template<> bool wstrutil::StringToDouble(const wstr& s, double& value) {
	if (s.empty()) {
		return (false);
	}
	wchar_t* _end_ptr;
	value = ::wcstod(s.c_str(), &_end_ptr);
	return (s.c_str()+s.length() == _end_ptr);
}

template<> void strutil::DoubleToString(double value, int num_decimals, str& s) {
	char _format[32];
	char result[64];

#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // = MSVC 2005
	::sprintf_s(_format, sizeof(_format), "%%.%if", num_decimals);
	::sprintf_s(result, sizeof(result), _format, value);
#else // _MSC_VER <= 1310 = MSVC 2003 or earlier
	::sprintf(_format, "%%.%if", num_decimals);
	::sprintf(result, _format, value);
#endif // MSVC 2005+/MSVC 2003-
#else // !LEPRA_WINDOWS
	::sprintf(_format, "\%%.%if", num_decimals);
	::sprintf(result, _format, value);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS

	s = result;
}

template<> str strutil::BoolToString(bool value) {
	return value? "true" : "false";
}

template<> wstr wstrutil::BoolToString(bool value) {
	return value? L"true" : L"false";
}

template<> void wstrutil::DoubleToString(double value, int num_decimals, wstr& s) {
	wchar_t _format[32];
	wchar_t result[64];

#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // = MSVC 2005
	::swprintf_s(_format, sizeof(_format)/sizeof(wchar_t), L"%%.%if", num_decimals);
	::swprintf_s(result, sizeof(result)/sizeof(wchar_t), _format, value);
#else // _MSC_VER <= 1310 = MSVC 2003 or earlier
	::swprintf(_format, L"%%.%if", num_decimals);
	::swprintf(result, _format, value);
#endif // MSVC 2005+/MSVC 2003-
#else // !LEPRA_WINDOWS
	::swprintf(_format, sizeof(_format) / sizeof(wchar_t), L"%%.%if", num_decimals);
	::swprintf(result, sizeof(result) / sizeof(wchar_t), _format, value);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS

	s = result;
}



template<> int strutil::StrLen(const char* s) {
	return (int)::strlen(s);
}

template<> int wstrutil::StrLen(const wchar_t* s) {
	return (int)::wcslen(s);
}

template<> int strutil::StrCmp(const char* string1, const char* string2) {
	return ::strcmp(string1, string2);
}

template<> int wstrutil::StrCmp(const wchar_t* string1, const wchar_t* string2) {
	return ::wcscmp(string1, string2);
}

#ifndef LEPRA_POSIX // Implemented in str.inl instead.

template<> int strutil::StrCmpI(const char* string1, const char* string2) {
#ifdef LEPRA_MSVC
	return ::_stricmp(string1, string2);
#else
	return ::stricmpt(string1, string2);

#endif
}

template<> int wstrutil::StrCmpI(const wchar_t* string1, const wchar_t* string2) {
#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // = MSVC 2005
	return ::_wcsicmp(string1, string2);
#else // _MSC_VER <= 1310 = MSVC 2003 or earlier
	return ::wcsicmp(string1, string2);
#endif // MSVC 2005+/MSVC 2003-
#else // !LEPRA_WINDOWS
	return ::stricmpt(string1, string2);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
}

#endif

template<> int strutil::StrNCmp(const char* string1, const char* string2, int max_count) {
	return ::strncmp(string1, string2, max_count);
}

template<> int wstrutil::StrNCmp(const wchar_t* string1, const wchar_t* string2, int max_count) {
	return ::wcsncmp(string1, string2, max_count);
}

template<> const char* strutil::StrStr(const char* s, const char* string_search) {
	return ::strstr(s, string_search);
}

template<> const wchar_t* wstrutil::StrStr(const wchar_t* s, const wchar_t* string_search) {
	return ::wcsstr(s, string_search);
}



template<> bool strutil::IsWhiteSpace(char c) {
	return (c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n');
}

template<> bool wstrutil::IsWhiteSpace(wchar_t c) {
	return (c == L' ' || c == L'\t' || c == L'\v' || c == L'\r' || c == L'\n');
}



template<> const str strutil::Encode(const str& s) {
	deb_assert(false);
	return (s);
}

template<> const str strutil::Encode(const wstr& s) {
	// Convert to UTF-8.
	str __utf8;
	__utf8.reserve(((s.size()*9)>>3) + 6);	// UTF-8 might be around 9/8ths and then some in many languages.
	if (sizeof(wchar_t) == 2) {
		utf8::unchecked::utf16to8(s.begin(), s.end(), back_inserter(__utf8));
	} else {
		deb_assert(sizeof(wchar_t) == 4);
		utf8::unchecked::utf32to8(s.begin(), s.end(), back_inserter(__utf8));
	}
	return (__utf8);
}

template<> const wstr wstrutil::Encode(const str& s) {
	// Convert to UTF-16 or UTF-32.
	wstr utf_n;
	utf_n.reserve(s.size());
	if (sizeof(wchar_t) == 2) {
		utf8::unchecked::utf8to16(s.begin(), s.end(), back_inserter(utf_n));
	} else {
		deb_assert(sizeof(wchar_t) == 4);
		utf8::unchecked::utf8to32(s.begin(), s.end(), back_inserter(utf_n));
	}
	return (utf_n);
}

template<> const wstr wstrutil::Encode(const wstr& s) {
	deb_assert(false);
	return (s);
}



template<> std::locale strutil::locale_("");
template<> std::locale wstrutil::locale_("");

const str kEmptyAnsiString;
const wstr kEmptyUnicodeString;



}
