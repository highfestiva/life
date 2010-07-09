
// Author: Jonas Byströ
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include <locale>
#include <vector>
#include "String.h"



#define STR_UTIL_TEMPLATE template<class _String>



namespace Lepra
{



STR_UTIL_TEMPLATE class StringUtilityTemplate
{
public:
	typedef std::vector<_String> strvec;
	static strvec Split(const _String& pString, const _String& pCharDelimitors, int pSplitMaxCount = 10000);
	// Splits strings, but also keeps track of quotes. Strings within quotes are left untouched from the delimitors.
	static strvec BlockSplit(const _String& pString, const _String& pCharDelimitors, bool pKeepQuotes, bool pIsCString, int pSplitMaxCount = 10000);
	static _String Join(const strvec& pStringVector, const _String& pJoinString, size_t pStartIndex = 0, size_t pEndIndex = _String::npos);

	static _String StringToCString(const _String& pString);
	static bool CStringToString(const _String& pCString, _String& _pString);

	static _String StripLeft(const _String& pString, const _String& pCharDelimitors);

	static _String ReplaceAll(const _String& pString, typename _String::value_type pFromChar, typename _String::value_type pToChar);
	static _String ReplaceAll(const _String& pString, const _String& pFrom, const _String& pTo);

	static _String Format(const typename _String::value_type* ppFormat, ...);
	static _String VFormat(const typename _String::value_type* ppFormat, va_list& pArguments);

	static bool StringToInt(const _String& pString, int& pValue, int pRadix = 10);
	static _String IntToString(int64 pValue, int pRadix);
	static bool StringToDouble(const _String& pString, double& pValue);
	static void DoubleToString(double pValue, int pNumDecimals, _String& pString);
	static _String DoubleToString(double pValue, int pNumDecimals);
	static _String FastDoubleToString(double pValue);
	static bool StringToBool(const _String& pString, bool& pValue);
	static _String BoolToString(bool pValue);

	static _String Reverse(const _String& pString);
	static const _String Right(const _String& pString, size_t pCharCount);
	static bool StartsWith(const _String& pString, const _String& pStart);
	static bool EndsWith(const _String& pString, const _String& pEnd);

	static size_t FindPreviousWord(const _String& pLine, const _String& pDelimitors, size_t pStartIndex);
	static size_t FindNextWord(const _String& pLine, const _String& pDelimitors, size_t pStartIndex);
	static int FindFirstWhiteSpace(const _String& pString, size_t pOffset, int pSearchDirection);
	static void StripWhiteSpaces(_String& pString);
	static bool IsWhiteSpace(typename _String::value_type pChar);
	static _String ReplaceCtrlChars(const _String& pString, typename _String::value_type pReplacement);
	static _String DumpData(const uint8* pData, size_t pLength);

	static const _String Encode(const astr& pString);
	static const _String Encode(const wstr& pString);

	static void ToLower(_String& pString);
	static void ToUpper(_String& pString);

	static int CompareIgnoreCase(const _String& pString1, const _String& pString2);

	static long StrToL(const typename _String::value_type* pString, typename _String::value_type** pEndPtr, int pRadix);
	static int StrLen(const typename _String::value_type* pString);
	static int StrCmp(const typename _String::value_type* pString1, const typename _String::value_type* pString2);
	static int StrCmpI(const typename _String::value_type* pString1, const typename _String::value_type* pString2);
	static int StrNCmp(const typename _String::value_type* pString1, const typename _String::value_type* pString2, int pMaxCount);
	static const typename _String::value_type* StrStr(const typename _String::value_type* pString, const typename _String::value_type* pStringSearch);

private:
	static std::locale mLocale;
};

typedef StringUtilityTemplate<astr>	astrutil;
typedef StringUtilityTemplate<wstr>	wstrutil;
typedef StringUtilityTemplate<str>	strutil;
extern const astr gEmptyAnsiString;
extern const wstr gEmptyUnicodeString;
#ifdef LEPRA_UNICODE
#define EmptyString		gEmptyUnicodeString
#else // !LEPRA_UNICODE
#define EmptyString		gEmptyAnsiString
#endif // LEPRA_UNICODE/!LEPRA_UNICODE
#define EmptyAnsiString		gEmptyAnsiString
#define EmptyUnicodeString	gEmptyUnicodeString



}



#include "StringUtility.inl"

#undef STR_UTIL_TEMPLATE
