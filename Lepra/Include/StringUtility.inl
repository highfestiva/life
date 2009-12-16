
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#define STR_UTIL_QUAL StringUtilityTemplate<_String>



namespace Lepra
{



STR_UTIL_TEMPLATE typename STR_UTIL_QUAL::StringVector STR_UTIL_QUAL::Split(const _String& pString, const _String& pCharDelimitors, int pSplitMaxCount)
{
	StringVector lTokenVector;
	size_t lLastPosition = 0;
	// Find next delimiter at end of token
	size_t lCurrentPosition = pString.find_first_of(pCharDelimitors, lLastPosition);
	for (int lSplitCount = 0; lSplitCount < pSplitMaxCount &&
		(lCurrentPosition != _String::npos || lLastPosition != _String::npos); ++lSplitCount)
	{
		// Found a token, add it to the vector.
		lTokenVector.push_back(pString.substr(lLastPosition, lCurrentPosition - lLastPosition));
		// Skip delimitor characters after token.  Note the "not_of". this is beginning of token
		lLastPosition = pString.find_first_not_of(pCharDelimitors, lCurrentPosition);
		// Find next delimiter at end of token.
		lCurrentPosition = pString.find_first_of(pCharDelimitors, lLastPosition);
	}
	// If not all has been added.
	if (lLastPosition != _String::npos)
	{
		lTokenVector.push_back(pString.substr(lLastPosition));
	}
	// If the string ends with a delimitor.
	else if (pString.length() > 0 && pCharDelimitors.find_first_of(pString[pString.length()-1]) != String::npos)
	{
		lTokenVector.push_back(String());
	}
	return (lTokenVector);
}

STR_UTIL_TEMPLATE typename STR_UTIL_QUAL::StringVector STR_UTIL_QUAL::BlockSplit(const _String& pString, const _String& pCharDelimitors, bool pKeepQuotes, bool pIsCString, int pSplitMaxCount)
{
	StringVector lTokenVector;
	_String lCurrentToken;
	bool lTakeNextString = true;
	bool lInsideString = false;
	size_t x = 0;
	for (int lSplitCount = 0; x < pString.length(); ++x)
	{
		if (pIsCString && pString[x] == _T('\\') && x+1 < pString.length())
		{
			lCurrentToken.push_back(pString[x]);
			lCurrentToken.push_back(pString[++x]);
		}
		else if (pString[x] == _T('"'))
		{
			lInsideString = !lInsideString;
			if (pKeepQuotes)
			{
				lCurrentToken.push_back(pString[x]);
			}
		}
		else if (!lInsideString && pCharDelimitors.find_first_of(pString[x]) != String::npos)
		{
			if (lTakeNextString)
			{
				lTokenVector.push_back(lCurrentToken);
				lCurrentToken.clear();
				if (++lSplitCount >= pSplitMaxCount)
				{
					break;
				}
			}
			lTakeNextString = false;
		}
		else
		{
			lCurrentToken.push_back(pString[x]);
			lTakeNextString = true;
		}
	}
	// If we reached end of string while parsing.
	if (!lCurrentToken.empty())
	{
		lTokenVector.push_back(lCurrentToken);
	}
	if (x < pString.length())	// If we stopped splitting due to split maximum count.
	{
		lTokenVector.push_back(pString.substr(x));
	}
	return (lTokenVector);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Join(const StringVector& pStringVector, const _String& pJoinString, size_t pStartIndex, size_t pEndIndex)
{
	_String lResultString;
	for (size_t x = pStartIndex; x < pStringVector.size() && x < pEndIndex; ++x)
	{
		if (x > pStartIndex)
		{
			lResultString += pJoinString;
		}
		lResultString += pStringVector[x];
	}
	return (lResultString);
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::StringToCString(const _String& pString)
{
	_String lCString;
	size_t lLength = pString.length();
	for (size_t x = 0; x < lLength; ++x)
	{
		switch (pString[x])
		{
			case _T('\\'):	lCString += _T("\\\\");	break;
			case _T('\t'):	lCString += _T("\\t");	break;
			case _T('\v'):	lCString += _T("\\v");	break;
			case _T('\r'):	lCString += _T("\\r");	break;
			case _T('\n'):	lCString += _T("\\n");	break;
			case _T('"'):	lCString += _T("\\\"");	break;
			default:	lCString += pString[x];	break;
		}
	}
	return (lCString);
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::CStringToString(const _String& pCString, _String& pString)
{
	pString.clear();
	bool lOk = true;
	size_t lLength = pCString.length();
	for (size_t x = 0; x < lLength; ++x)
	{
		if (pCString[x] == _T('\\'))
		{
			if (x+1 >= lLength)
			{
				lOk = false;
				break;
			}
			switch (pCString[x+1])
			{
				case _T('n'):
				{
					pString += _T('\n');
					++x;
				}
				break;
				case _T('r'):
				{
					pString += _T('\r');
					++x;
				}
				break;
				case _T('t'):
				{
					pString += _T('\t');
					++x;
				}
				break;
				case _T('v'):
				{
					pString += _T('\v');
					++x;
				}
				break;
				case _T('\\'):
				{
					pString += _T('\\');
					++x;
				}
				break;
				case _T('"'):
				{
					pString += _T('"');
					++x;
				}
				break;
				case _T('x'):
				{
					const tchar* lNumber = pCString.c_str()+x+2;
					tchar* lEnd = 0;
					int lCharacter = StrToL(lNumber, &lEnd, 16);
					pString += (tchar)lCharacter;
					x += 1+(unsigned)(lEnd-lNumber);
				}
				break;
				case _T('0'):
				{
					const tchar* lNumber = pCString.c_str()+x+2;
					tchar* lEnd = 0;
					int lCharacter = StrToL(lNumber, &lEnd, 8);
					pString += (tchar)lCharacter;
					x += 1+(unsigned)(lEnd-lNumber);
				}
				break;
				case _T('1'):
				case _T('2'):
				case _T('3'):
				case _T('4'):
				case _T('5'):
				case _T('6'):
				case _T('7'):
				case _T('8'):
				case _T('9'):
				{
					const tchar* lNumber = pCString.c_str()+x+1;
					tchar* lEnd = 0;
					int lCharacter = StrToL(lNumber, &lEnd, 10);
					pString += (tchar)lCharacter;
					x += 1+(unsigned)(lEnd-lNumber);
				}
				break;
				default:
				{
					pString += _T('\\');
					lOk = false;
				}
				break;

			}
		}
		else
		{
			pString += pCString[x];
		}
	}
	return (lOk);
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::StripLeft(const _String& pString, const _String& pCharDelimitors)
{
	// Skip delimitor characters at beginning, find start of first token.
	size_t lTokenStartPosition = pString.find_first_not_of(pCharDelimitors, 0);
	if (lTokenStartPosition != _String::npos)
	{
		return (pString.substr(lTokenStartPosition));
	}
	return (_String());
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::ReplaceAll(const _String& pString, typename _String::value_type pFromChar, typename _String::value_type pToChar)
{
	_String lString = pString;
	int length = (int)pString.length();
	for (int x = 0; x < length; ++x)
	{
		if (lString[x] == pFromChar)
		{
			lString[x] = pToChar;
		}
	}
	return (lString);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::ReplaceAll(const _String& pString, const _String& pFrom, const String& pTo)
{
	_String lString = pString;
	_String::size_type lPosition;
	while ((lPosition = lString.find(pFrom)) != Lepra::String::npos)
	{
		lString.replace(lPosition, pFrom.length(), pTo);
	}
	return (lString);
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Format(const typename _String::value_type* pFormat, ...)
{
	va_list	lArguments;
	va_start(lArguments, pFormat);
	return (VFormat(pFormat, lArguments));
}



STR_UTIL_TEMPLATE size_t STR_UTIL_QUAL::FindPreviousWord(const _String& pLine, const _String& pDelimitors, size_t pStartIndex)
{
	_String lReverseLine = Reverse(pLine);
	size_t lReverseIndex = pLine.length()-pStartIndex;
	size_t lIndex = lReverseLine.find_first_not_of(pDelimitors, lReverseIndex);
	lIndex = lReverseLine.find_first_of(pDelimitors, lIndex);
	if (lIndex != Lepra::String::npos)
	{
		lReverseIndex = lIndex;
	}
	else
	{
		lReverseIndex = pLine.length();
	}
	pStartIndex = pLine.length()-lReverseIndex;
	return (pStartIndex);
}

STR_UTIL_TEMPLATE size_t STR_UTIL_QUAL::FindNextWord(const _String& pLine, const _String& pDelimitors, size_t pStartIndex)
{
	size_t lIndex = pLine.find_first_of(pDelimitors, pStartIndex);
	lIndex = pLine.find_first_not_of(pDelimitors, lIndex);
	if (lIndex != Lepra::String::npos)
	{
		pStartIndex = lIndex;
	}
	else
	{
		pStartIndex = pLine.length();
	}
	return (pStartIndex);
}

STR_UTIL_TEMPLATE int STR_UTIL_QUAL::FindFirstWhiteSpace(const _String& pString, size_t pOffset, int pSearchDirection)
{
	int lLength = (int)pString.length();
	int x = (int)pOffset;
	for (; x >= 0 && x < lLength; x += pSearchDirection)
	{
		if (IsWhiteSpace(pString[x]))
		{
			break;
		}
	}
	if (x >= lLength)
	{
		x = -1;
	}
	return (x);
}

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::StripWhiteSpaces(_String& pString)
{
	int lDestIndex = 0;
	for (int i = 0; pString[i] != 0; i++)
	{
		if (!IsWhiteSpace(pString[i]))
		{
			pString[lDestIndex++] = pString[i];
		}
	}
	pString.resize(lDestIndex);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::ReplaceCtrlChars(const _String& pString, typename _String::value_type pReplacement)
{
	_String lString(pString);
	for (size_t x = 0; x < lString.length(); ++x)
	{
		if (lString[x] < ' ')
		{
			lString[x] = pReplacement;
		}
	}
	return (lString);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::DumpData(const uint8* pData, size_t pLength)
{
	const char lHexTable[] = "0123456789ABCDEF";
	AnsiString lDataString((const char*)pData, pLength*2);
	for (size_t x = 0; x < pLength; ++x)
	{
		lDataString[x*2+0] = lHexTable[pData[x]>>4];
		lDataString[x*2+1] = lHexTable[pData[x]&0xF];
	}
	return (ToOwnCode(lDataString));
}

#ifdef LEPRA_UNICODE

STR_UTIL_TEMPLATE const UnicodeString STR_UTIL_QUAL::ToCurrentCode(const _String& pString)
{
	return (UnicodeStringUtility::ToOwnCode(pString));
}

#else // !LEPRA_UNICODE

STR_UTIL_TEMPLATE const AnsiString STR_UTIL_QUAL::ToCurrentCode(const _String& pString)
{
	return (AnsiStringUtility::ToOwnCode(pString));
}

#endif // LEPRA_UNICODE/!LEPRA_UNICODE



#ifdef LEPRA_POSIX

STR_UTIL_TEMPLATE int STR_UTIL_QUAL::StrCmpI(const typename _String::value_type* pString1, const typename _String::value_type* pString2)
{
	typename _String::value_type lC1;
	typename _String::value_type lC2;

	while (true)
	{
		lC1 = *pString1++;
		lC2 = *pString2++;

		if (!lC1 || !lC2)
		{
			break;
		}

		if (lC1 == lC2)
		{
			continue;
		}

		lC1 = std::tolower(lC1, mLocale);
		lC2 = std::tolower(lC2, mLocale);

		if (lC1 != lC2)
		{
			break;
		}
	}
	return (int)lC1 - (int)lC2;
}

#endif // !LEPRA_POSIX



STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::StringToInt(const _String& pString, int& pValue, int pRadix)
{
	if (pString.empty())
	{
		return (false);
	}
	typename _String::value_type* lEndPtr;
	pValue = StrToL(pString.c_str(), &lEndPtr, pRadix);
	return (pString.c_str()+pString.length() == lEndPtr);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::IntToString(int64 pValue, int pRadix)
{
	bool lSign = (pValue < 0);
	if (lSign == true)
	{
		pValue = -pValue;
	}

	typename _String::value_type lString[64];
	int i = 0;

	while (pValue > 0)
	{
		int lDigit = (int)(pValue % pRadix);
		pValue /= pRadix;

		if (lDigit >= 0 && lDigit <= 9)
		{
			lString[i] = (typename _String::value_type)('0' + lDigit);
		}
		else if(lDigit > 9)
		{
			lString[i] = (typename _String::value_type)('a' + (lDigit - 10));
		}
		i++;
	}

	if (lSign == true)
	{
		lString[i] = '-';
		i++;
	}

	if (i == 0)
	{
		lString[i] = '0';
		i++;
	}
	lString[i] = 0;

	// Reverse string.
	for (int j = 0; j < i; j++, i--)
	{
		typename _String::value_type lTemp = lString[i - 1];
		lString[i - 1] = lString[j];
		lString[j] = lTemp;
	}

	return (lString);
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::StringToBool(const _String& pString, bool& pValue)
{
	bool lOk = false;
	if (pString == _T("false"))
	{
		pValue = false;
		lOk = true;
	}
	else if (pString == _T("true"))
	{
		pValue = true;
		lOk = true;
	}
	return (lOk);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::DoubleToString(double pValue, int pNumDecimals)
{
	_String lValue;
	DoubleToString(pValue, pNumDecimals, lValue);
	return (lValue);
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::BoolToString(bool pValue)
{
	if (pValue)
	{
		return (_T("true"));	// RAII simplifies here.
	}
	else
	{
		return (_T("false"));	// RAII simplifies here.
	}
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Reverse(const _String& pString)
{
	_String lReverse(pString);
	for (size_t x = 0; x < pString.length(); ++x)
	{
		lReverse[pString.length()-x-1] = pString[x];
	}
	return (lReverse);
}

STR_UTIL_TEMPLATE const _String STR_UTIL_QUAL::Right(const _String& pString, size_t pCharCount)
{
	if (pCharCount <= pString.length())
	{
		return (pString.substr(pString.length()-pCharCount, pCharCount));
	}
	return (pString);
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::StartsWith(const _String& pString, const _String& pStart)
{
	if (pString.length() >= pStart.length())
	{
		return (pString.substr(0, pStart.length()) == pStart);
	}
	return (false);
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::EndsWith(const _String& pString, const _String& pEnd)
{
	return (Right(pString, pEnd.length()) == pEnd);
}



STR_UTIL_TEMPLATE void STR_UTIL_QUAL::ToLower(_String& pString)
{
	for (int i = 0; i < (int)pString.length(); i++)
	{
		pString[i] = std::tolower(pString[i], mLocale);
	}
}

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::ToUpper(_String& pString)
{
	for (int i = 0; i < (int)pString.length(); i++)
	{
		pString[i] = std::toupper(pString[i], mLocale);
	}
}



STR_UTIL_TEMPLATE int STR_UTIL_QUAL::CompareIgnoreCase(const _String& pString1, const _String& pString2)
{
	return (StrCmpI(pString1.c_str(), pString2.c_str()));
}



}



#undef STR_UTIL_QUAL
