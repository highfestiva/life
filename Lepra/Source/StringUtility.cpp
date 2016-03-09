
// Author: Jonas Bystrröm
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/StringUtility.h"
#include "../Include/LepraAssert.h"
#include "../Include/LepraOS.h"
#ifdef _MSC_VER
#pragma warning(disable: 4127)	// Conditional if (sizeof()...) results in constant expression.
#pragma warning(disable: 4244)	// Converting UTF-32 code point to wchar_t on 16-bit system.
#endif // VC++.
#include <utf8.h>



namespace Lepra
{



template<> astr astrutil::VFormat(const char* pFormat, va_list& pArguments)
{
	char lBuffer[1024];
#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // <VS 2005+>
	::vsprintf_s(lBuffer, sizeof(lBuffer), pFormat, pArguments);
#else // <VS 2003 .NET->
	::vsprintf(lBuffer, pFormat, pArguments);
#endif // <VS 2005+> / <VS 2003 .NET->
#else // !LEPRA_WINDOWS
	::vsnprintf(lBuffer, sizeof(lBuffer), pFormat, pArguments);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
	return (astr(lBuffer));
}

template<> wstr wstrutil::VFormat(const wchar_t* pFormat, va_list& pArguments)
{
	wchar_t lBuffer[1024];
#ifndef LEPRA_WINDOWS
	wchar_t lFormat[1024];
	int x = 0;
	bool lIsPercent = false;
	do
	{
		wchar_t ch = pFormat[x];
		if (lIsPercent)
		{
			if (ch == '%' || ch == 'd' || ch == 'i' || ch == 'f' || ch == 'F' ||
				ch == 'x' || ch == 'X' || ch == 'c' || ch == 'C' || ch == 'p' ||
				ch == 'g' || ch == 'G' || ch == 'e' || ch == 'E')
			{
				lIsPercent = false;
			}
			else if (ch == 's')
			{
				ch = 'S';
				lIsPercent = false;
			}
		}
		else if (!lIsPercent && ch == '%')
		{
			lIsPercent = true;
		}
		lFormat[x] = ch;
	}
	while (pFormat[x++]);
	pFormat = lFormat;
#endif // !Windows
	WIDE_SPRINTF(lBuffer, sizeof(lBuffer)/sizeof(wchar_t), pFormat, pArguments);
	return (wstr(lBuffer));
}



template<> long astrutil::StrToL(const char* pString, char** pEndPtr, int pRadix)
{
	return (::strtol(pString, pEndPtr, pRadix));
}

template<> long wstrutil::StrToL(const wchar_t* pString, wchar_t** pEndPtr, int pRadix)
{
	return (::wcstol(pString, pEndPtr, pRadix));
}

template<> bool astrutil::StringToDouble(const astr& pString, double& pValue)
{
	if (pString.empty())
	{
		return (false);
	}
	char* lEndPtr;
	pValue = ::strtod(pString.c_str(), &lEndPtr);
	return (pString.c_str()+pString.length() == lEndPtr);
}

template<> bool wstrutil::StringToDouble(const wstr& pString, double& pValue)
{
	if (pString.empty())
	{
		return (false);
	}
	wchar_t* lEndPtr;
	pValue = ::wcstod(pString.c_str(), &lEndPtr);
	return (pString.c_str()+pString.length() == lEndPtr);
}

template<> void astrutil::DoubleToString(double pValue, int pNumDecimals, astr& pString)
{
	char lFormat[32];
	char lResult[64];

#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // = MSVC 2005
	::sprintf_s(lFormat, sizeof(lFormat), "%%.%if", pNumDecimals);
	::sprintf_s(lResult, sizeof(lResult), lFormat, pValue);
#else // _MSC_VER <= 1310 = MSVC 2003 or earlier
	::sprintf(lFormat, "%%.%if", pNumDecimals);
	::sprintf(lResult, lFormat, pValue);
#endif // MSVC 2005+/MSVC 2003-
#else // !LEPRA_WINDOWS
	::sprintf(lFormat, "\%%.%if", pNumDecimals);
	::sprintf(lResult, lFormat, pValue);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS

	pString = lResult;
}

template<> astr astrutil::BoolToString(bool pValue)
{
	return pValue? "true" : "false";
}

template<> wstr wstrutil::BoolToString(bool pValue)
{
	return pValue? L"true" : L"false";
}

template<> void wstrutil::DoubleToString(double pValue, int pNumDecimals, wstr& pString)
{
	wchar_t lFormat[32];
	wchar_t lResult[64];

#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // = MSVC 2005
	::swprintf_s(lFormat, sizeof(lFormat)/sizeof(wchar_t), L"%%.%if", pNumDecimals);
	::swprintf_s(lResult, sizeof(lResult)/sizeof(wchar_t), lFormat, pValue);
#else // _MSC_VER <= 1310 = MSVC 2003 or earlier
	::swprintf(lFormat, L"%%.%if", pNumDecimals);
	::swprintf(lResult, lFormat, pValue);
#endif // MSVC 2005+/MSVC 2003-
#else // !LEPRA_WINDOWS
	::swprintf(lFormat, sizeof(lFormat) / sizeof(wchar_t), L"%%.%if", pNumDecimals);
	::swprintf(lResult, sizeof(lResult) / sizeof(wchar_t), lFormat, pValue);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS

	pString = lResult;
}



template<> int astrutil::StrLen(const char* pString)
{
	return (int)::strlen(pString);
}

template<> int wstrutil::StrLen(const wchar_t* pString)
{
	return (int)::wcslen(pString);
}

template<> int astrutil::StrCmp(const char* pString1, const char* pString2)
{
	return ::strcmp(pString1, pString2);
}

template<> int wstrutil::StrCmp(const wchar_t* pString1, const wchar_t* pString2)
{
	return ::wcscmp(pString1, pString2);
}

#ifndef LEPRA_POSIX // Implemented in str.inl instead.

template<> int astrutil::StrCmpI(const char* pString1, const char* pString2)
{
#ifdef LEPRA_MSVC
	return ::_stricmp(pString1, pString2);
#else
	return ::stricmpt(pString1, pString2);
	
#endif
}

template<> int wstrutil::StrCmpI(const wchar_t* pString1, const wchar_t* pString2)
{
#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // = MSVC 2005
	return ::_wcsicmp(pString1, pString2);
#else // _MSC_VER <= 1310 = MSVC 2003 or earlier
	return ::wcsicmp(pString1, pString2);
#endif // MSVC 2005+/MSVC 2003-
#else // !LEPRA_WINDOWS
	return ::stricmpt(pString1, pString2);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
}

#endif

template<> int astrutil::StrNCmp(const char* pString1, const char* pString2, int pMaxCount)
{
	return ::strncmp(pString1, pString2, pMaxCount);
}

template<> int wstrutil::StrNCmp(const wchar_t* pString1, const wchar_t* pString2, int pMaxCount)
{
	return ::wcsncmp(pString1, pString2, pMaxCount);
}

template<> const char* astrutil::StrStr(const char* pString, const char* pStringSearch)
{
	return ::strstr(pString, pStringSearch);
}

template<> const wchar_t* wstrutil::StrStr(const wchar_t* pString, const wchar_t* pStringSearch)
{
	return ::wcsstr(pString, pStringSearch);
}



template<> bool astrutil::IsWhiteSpace(char pChar)
{
	return (pChar == ' ' || pChar == '\t' || pChar == '\v' || pChar == '\r' || pChar == '\n');
}

template<> bool wstrutil::IsWhiteSpace(wchar_t pChar)
{
	return (pChar == L' ' || pChar == L'\t' || pChar == L'\v' || pChar == L'\r' || pChar == L'\n');
}



template<> const astr astrutil::Encode(const astr& pString)
{
	return (pString);
}

template<> const astr astrutil::Encode(const wstr& pString)
{
	// Convert to UTF-8.
	astr lAnsi;
	lAnsi.reserve(((pString.size()*9)>>3) + 6);	// UTF-8 might be around 9/8ths and then some in many languages.
	if (sizeof(wchar_t) == 2)
	{
		utf8::unchecked::utf16to8(pString.begin(), pString.end(), back_inserter(lAnsi));
	}
	else
	{
		deb_assert(sizeof(wchar_t) == 4);
		utf8::unchecked::utf32to8(pString.begin(), pString.end(), back_inserter(lAnsi));
	}
	return (lAnsi);
}

template<> const wstr wstrutil::Encode(const astr& pString)
{
	// Convert to UTF-16 or UTF-32.
	wstr lUnicode;
	lUnicode.reserve(pString.size());
	if (sizeof(wchar_t) == 2)
	{
		utf8::unchecked::utf8to16(pString.begin(), pString.end(), back_inserter(lUnicode));
	}
	else
	{
		deb_assert(sizeof(wchar_t) == 4);
		utf8::unchecked::utf8to32(pString.begin(), pString.end(), back_inserter(lUnicode));
	}
	return (lUnicode);
}

template<> const wstr wstrutil::Encode(const wstr& pString)
{
	return (pString);
}



template<> std::locale astrutil::mLocale("");
template<> std::locale wstrutil::mLocale("");

const astr gEmptyAnsiString;
const wstr gEmptyUnicodeString;



}
