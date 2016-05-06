
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include <locale>
#include <vector>
#include "string.h"
#include "lepraassert.h"



#define STR_UTIL_TEMPLATE template<class _String>



namespace lepra {



STR_UTIL_TEMPLATE class StringUtilityTemplate {
public:
	typedef std::vector<_String> strvec;
	static strvec Split(const _String& s, const _String& char_delimitors, int split_max_count = 10000);
	// Splits strings, but also keeps track of quotes. Strings within quotes are left untouched from the delimitors.
	static strvec BlockSplit(const _String& s, const _String& char_delimitors, bool keep_quotes, bool is_c_string, int split_max_count = 10000);
	static void FastBlockSplit(strvec& token_vector, const _String& s, const _String& char_delimitors, bool keep_quotes, bool is_c_string, int split_max_count = 10000);
	static _String Join(const strvec& string_vector, const _String& join_string, size_t start_index = 0, size_t end_index = _String::npos);
	static void Append(strvec& destination, const strvec& append);

	static _String StringToCString(const _String& s);
	static bool CStringToString(const _String& c_string, _String& _pString);

	static _String StripLeft(const _String& s, const _String& char_delimitors);
	static _String StripRight(const _String& s, const _String& char_delimitors);
	static _String Strip(const _String& s, const _String& char_delimitors);

	static _String ReplaceAll(const _String& s, typename _String::value_type from_char, typename _String::value_type to_char);
	static _String ReplaceAll(const _String& s, const _String& from, const _String& to);

	static _String Format(const typename _String::value_type* format, ...);
	static _String VFormat(const typename _String::value_type* format, va_list& arguments);

	static bool StringToInt(const _String& s, int& value, int radix = 10);
	static _String IntToString(int64 value, int radix);
	static bool StringToDouble(const _String& s, double& value);
	static void DoubleToString(double value, int num_decimals, _String& s);
	static _String DoubleToString(double value, int num_decimals);
	static _String FastDoubleToString(double value);
	static bool StringToBool(const _String& s, bool& value);
	static _String BoolToString(bool value);

	static _String Reverse(const _String& s);
	static const _String Right(const _String& s, size_t char_count);
	static bool StartsWith(const _String& s, const _String& start);
	static bool EndsWith(const _String& s, const _String& end);

	static size_t FindPreviousWord(const _String& line, const _String& delimitors, size_t start_index);
	static size_t FindNextWord(const _String& line, const _String& delimitors, size_t start_index);
	static int FindFirstWhiteSpace(const _String& s, size_t offset, int search_direction);
	static void StripAllWhiteSpaces(_String& s);
	static bool IsWhiteSpace(typename _String::value_type c);
	static _String ReplaceCtrlChars(const _String& s, typename _String::value_type replacement);
	static _String DumpData(const uint8* data, size_t length);

	static const _String Encode(const str& s);
	static const _String Encode(const wstr& s);

	static void ToLower(_String& s);
	static void ToUpper(_String& s);

	static int CompareIgnoreCase(const _String& string1, const _String& string2);

	static long StrToL(const typename _String::value_type* s, typename _String::value_type** end_ptr, int radix);
	static int StrLen(const typename _String::value_type* s);
	static int StrCmp(const typename _String::value_type* string1, const typename _String::value_type* string2);
	static int StrCmpI(const typename _String::value_type* string1, const typename _String::value_type* string2);
	static int StrNCmp(const typename _String::value_type* string1, const typename _String::value_type* string2, int max_count);
	static const typename _String::value_type* StrStr(const typename _String::value_type* s, const typename _String::value_type* string_search);

private:
	static std::locale locale_;
};

typedef StringUtilityTemplate<str>	strutil;
typedef StringUtilityTemplate<wstr>	wstrutil;
extern const str kEmptyAnsiString;
extern const wstr kEmptyUnicodeString;
#ifdef LEPRA_UTF32
#define EmptyString		kEmptyUnicodeString
#else // !LEPRA_UTF32
#define EmptyString		kEmptyAnsiString
#endif // LEPRA_UTF32/!LEPRA_UTF32
#define EmptyAnsiString		kEmptyAnsiString
#define EmptyUnicodeString	kEmptyUnicodeString



}



#include "stringutility.inl"

#undef STR_UTIL_TEMPLATE
