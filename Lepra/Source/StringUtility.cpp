
// Author: Jonas Bystrröm
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/StringUtility.h"
#include <assert.h>
#ifdef _MSC_VER
#pragma warning(disable: 4127)	// Conditional if (sizeof()...) results in constant expression.
#pragma warning(disable: 4244)	// Converting UTF-32 code point to wchar_t on 16-bit system.
#endif // VC++.
#include <utf8.h>



namespace Lepra
{



template<> AnsiString AnsiStringUtility::VFormat(const char* pFormat, va_list pArguments)
{
	char lBuffer[1024];
#ifdef LEPRA_WINDOWS
#if _MSC_VER > 1310 // <VS 2005+>
	::vsprintf_s(lBuffer, sizeof(lBuffer), pFormat, pArguments);
#else // <VS 2003 .NET->
	::vsprintf(lBuffer, pFormat, pArguments);
#endif // <VS 2005+> / <VS 2003 .NET->
#else // !LEPRA_WINDOWS
	::vsprintf(lBuffer, pFormat, pArguments);
#endif // LEPRA_WINDOWS/!LEPRA_WINDOWS
	return (AnsiString(lBuffer));
}

template<> UnicodeString UnicodeStringUtility::VFormat(const wchar_t* pFormat, va_list pArguments)
{
	wchar_t lBuffer[1024];
	WIDE_SPRINTF(lBuffer, sizeof(lBuffer)/sizeof(wchar_t), pFormat, pArguments);
	return (UnicodeString(lBuffer));
}



template<> long AnsiStringUtility::StrToL(const char* pString, char** pEndPtr, int pRadix)
{
	return (::strtol(pString, pEndPtr, pRadix));
}

template<> long UnicodeStringUtility::StrToL(const wchar_t* pString, wchar_t** pEndPtr, int pRadix)
{
	return (::wcstol(pString, pEndPtr, pRadix));
}

template<> bool AnsiStringUtility::StringToDouble(const AnsiString& pString, double& pValue)
{
	if (pString.empty())
	{
		return (false);
	}
	char* lEndPtr;
	pValue = ::strtod(pString.c_str(), &lEndPtr);
	return (pString.c_str()+pString.length() == lEndPtr);
}

template<> bool UnicodeStringUtility::StringToDouble(const UnicodeString& pString, double& pValue)
{
	if (pString.empty())
	{
		return (false);
	}
	wchar_t* lEndPtr;
	pValue = ::wcstod(pString.c_str(), &lEndPtr);
	return (pString.c_str()+pString.length() == lEndPtr);
}

template<> void AnsiStringUtility::DoubleToString(double pValue, int pNumDecimals, AnsiString& pString)
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

template<> void UnicodeStringUtility::DoubleToString(double pValue, int pNumDecimals, UnicodeString& pString)
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



template<> int AnsiStringUtility::StrLen(const char* pString)
{
	return (int)::strlen(pString);
}

template<> int UnicodeStringUtility::StrLen(const wchar_t* pString)
{
	return (int)::wcslen(pString);
}

template<> int AnsiStringUtility::StrCmp(const char* pString1, const char* pString2)
{
	return ::strcmp(pString1, pString2);
}

template<> int UnicodeStringUtility::StrCmp(const wchar_t* pString1, const wchar_t* pString2)
{
	return ::wcscmp(pString1, pString2);
}

#ifndef LEPRA_POSIX // Implemented in String.inl instead.

template<> int AnsiStringUtility::StrCmpI(const char* pString1, const char* pString2)
{
#ifdef LEPRA_MSVC
	return ::_stricmp(pString1, pString2);
#else
	return ::stricmpt(pString1, pString2);
	
#endif
}

template<> int UnicodeStringUtility::StrCmpI(const wchar_t* pString1, const wchar_t* pString2)
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

template<> int AnsiStringUtility::StrNCmp(const char* pString1, const char* pString2, int pMaxCount)
{
	return ::strncmp(pString1, pString2, pMaxCount);
}

template<> int UnicodeStringUtility::StrNCmp(const wchar_t* pString1, const wchar_t* pString2, int pMaxCount)
{
	return ::wcsncmp(pString1, pString2, pMaxCount);
}

template<> const char* AnsiStringUtility::StrStr(const char* pString, const char* pStringSearch)
{
	return ::strstr(pString, pStringSearch);
}

template<> const wchar_t* UnicodeStringUtility::StrStr(const wchar_t* pString, const wchar_t* pStringSearch)
{
	return ::wcsstr(pString, pStringSearch);
}



template<> bool AnsiStringUtility::IsWhiteSpace(char pChar)
{
	return (pChar == ' ' || pChar == '\t' || pChar == '\v' || pChar == '\r' || pChar == '\n');
}

template<> bool UnicodeStringUtility::IsWhiteSpace(wchar_t pChar)
{
	return (pChar == L' ' || pChar == L'\t' || pChar == L'\v' || pChar == L'\r' || pChar == L'\n');
}



template<> const AnsiString AnsiStringUtility::ToOwnCode(const AnsiString& pString)
{
	return (pString);
}

template<> const AnsiString AnsiStringUtility::ToOwnCode(const UnicodeString& pString)
{
	// Convert to UTF-8.
	AnsiString lAnsi;
	if (sizeof(wchar_t) == 2)
	{
		utf8::utf16to8(pString.begin(), pString.end(), back_inserter(lAnsi));
	}
	else
	{
		assert(sizeof(wchar_t) == 4);
		utf8::utf32to8(pString.begin(), pString.end(), back_inserter(lAnsi));
	}
	return (lAnsi);
}

template<> const UnicodeString UnicodeStringUtility::ToOwnCode(const AnsiString& pString)
{
	// Convert to UTF-16 or UTF-32.
	UnicodeString lUnicode;
	if (sizeof(wchar_t) == 2)
	{
		utf8::utf8to16(pString.begin(), pString.end(), back_inserter(lUnicode));
	}
	else
	{
		assert(sizeof(wchar_t) == 4);
		utf8::utf8to32(pString.begin(), pString.end(), back_inserter(lUnicode));
	}
	return (lUnicode);
}

template<> const UnicodeString UnicodeStringUtility::ToOwnCode(const UnicodeString& pString)
{
	return (pString);
}



template<> std::locale AnsiStringUtility::mLocale("");
template<> std::locale UnicodeStringUtility::mLocale("");

const AnsiString gEmptyAnsiString;
const UnicodeString gEmptyUnicodeString;



}
