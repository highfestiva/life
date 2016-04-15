
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/LepraAssert.h"
#include <utf8.h>
#include "../Include/JsonString.h"
#include "../Include/StringUtility.h"



namespace Lepra
{



str JsonString::ToJson(const str& pString)
{
	const str lUtf8String = pString;
	str lJsonString = "\"";
	for (str::const_iterator x = lUtf8String.begin(); x != lUtf8String.end(); ++x)
	{
		const char ch = *x;
		switch (ch)
		{
			case '\"':	lJsonString += "\\\"";	break;
			case '\\':	lJsonString += "\\\\";	break;
			case '\b':	lJsonString += "\\b";	break;
			case '\f':	lJsonString += "\\f";	break;
			case '\n':	lJsonString += "\\n";	break;
			case '\r':	lJsonString += "\\r";	break;
			case '\t':	lJsonString += "\\t";	break;
			default:
			{
				if (ch >= 31)
				{
					lJsonString += (char)ch;
				}
				else
				{
					// Convert UTF-8 to UTF-16 and write out each short as "\uXXXX".
					try
					{
						str::const_iterator y = x;
						utf8::next(y, lUtf8String.end());	// Advance y.
						utf8::uint16_t lUtf16String[8];
						const utf8::uint16_t* lUtf16EndIndex = utf8::utf8to16(x, y, &lUtf16String[0]);
						for (utf8::uint16_t* z = &lUtf16String[0]; z < lUtf16EndIndex; ++z)
						{
							lJsonString += strutil::Format("\\u%.4X", (unsigned)*z);
						}
						x = y-1;
					}
					catch (std::exception e)
					{
						lJsonString += '?';
					}
				}
			}
			break;
		}
	}
	lJsonString += "\"";
	return lJsonString;
}

str JsonString::FromJson(const str& pJsonString)
{
	if (pJsonString.size() < 2 || pJsonString[0] != '"' || pJsonString[pJsonString.size()-1] != '"')
	{
		deb_assert(false);
		return str();
	}
	str::const_iterator lEnd = pJsonString.end()-1;
	str lString;
	for (str::const_iterator x = pJsonString.begin()+1; x != lEnd; ++x)
	{
		const char ch = *x;
		if (ch == '\\')
		{
			if (x+1 == lEnd)	// Fluke ending?
			{
				lString += '?';
				break;
			}
			++x;
			const char ch2 = *x;
			switch (ch2)
			{
				case 'b':	lString += '\b';	break;
				case 'f':	lString += '\f';	break;
				case 'n':	lString += '\n';	break;
				case 'r':	lString += '\r';	break;
				case 't':	lString += '\t';	break;
				case 'u':
				{
					// Convert JSON's UTF-16 encoded characters to UTF-8, then to str.
					utf8::uint16_t lUtf16String[128];
					size_t lUtf16Index = 0;
					for (;;)
					{
						if (x+5 > lEnd)
						{
							lString += '?';
							return lString;
						}
						str lNumber = str(x+1, x+5);
						int lValue = 0;
						if (strutil::StringToInt(lNumber, lValue, 16))
						{
							lUtf16String[lUtf16Index++] = (utf8::uint16_t)lValue;
						}
						x += 5;
						if (x > lEnd-6 || *x != '\\' || *(x+1) != 'u')
						{
							--x;
							break;
						}
						++x;
					}
					char lUtf8String[sizeof(lUtf16String)*2];
					char* lUtf8End = utf8::utf16to8(&lUtf16String[0], &lUtf16String[lUtf16Index], &lUtf8String[0]);
					lString += str(lUtf8String, lUtf8End-&lUtf8String[0]);
				}
				break;
				default:
				{
					lString += ch2;
				}
				break;
			}
		}
		else
		{
			lString += ch;
		}
	}
	return lString;
}



str JsonString::UrlEncode(const str& pUrl)
{
	str lEscaped;
	const size_t l = pUrl.size();
	for (size_t x = 0; x < l; ++x)
	{
		const char c = pUrl[x];
		if ((c >= '0' && c <= '9') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			c == '~' || c == '!' || c == '*' || c == '(' || c == ')' || c == '\'')
		{
			lEscaped += c;
		}
		else
		{
			lEscaped += strutil::Format("%%%.2X", (unsigned)c);
		}
	}
	return lEscaped;
}



}
