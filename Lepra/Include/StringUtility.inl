
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#define STR_UTIL_QUAL StringUtilityTemplate<_String>



namespace Lepra
{



STR_UTIL_TEMPLATE typename STR_UTIL_QUAL::strvec STR_UTIL_QUAL::Split(const _String& pString, const _String& pCharDelimitors, int pSplitMaxCount)
{
	strvec lTokenVector;
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
	else if (pString.length() > 0 && pCharDelimitors.find_first_of(pString[pString.length()-1]) != str::npos)
	{
		lTokenVector.push_back(_String());
	}
	return (lTokenVector);
}

STR_UTIL_TEMPLATE typename STR_UTIL_QUAL::strvec STR_UTIL_QUAL::BlockSplit(const _String& pString, const _String& pCharDelimitors, bool pKeepQuotes, bool pIsCString, int pSplitMaxCount)
{
	strvec lTokenVector;
	FastBlockSplit(lTokenVector, pString, pCharDelimitors, pKeepQuotes, pIsCString, pSplitMaxCount);
	return lTokenVector;
}

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::FastBlockSplit(strvec& pTokenVector, const _String& pString, const _String& pCharDelimitors, bool pKeepQuotes, bool pIsCString, int pSplitMaxCount)
{
	typename _String::value_type lCurrentToken[32*1024];
	int y = 0;
	bool lTakeNextString = true;
	bool lInsideString = false;
	size_t x = 0;
	for (int lSplitCount = 0; x < pString.length(); ++x)
	{
		if (pIsCString && pString[x] == '\\' && x+1 < pString.length())
		{
			lCurrentToken[y++] = pString[x];
			lCurrentToken[y++] = pString[++x];
		}
		else if (pString[x] == '"')
		{
			lInsideString = !lInsideString;
			if (pKeepQuotes)
			{
				lCurrentToken[y++] = pString[x];
			}
			else if (y == 0 && !lInsideString)
			{
				// Push empty token.
				pTokenVector.push_back(_String());
			}
		}
		else if (!lInsideString && pCharDelimitors.find_first_of(pString[x]) != str::npos)
		{
			if (lTakeNextString)
			{
				pTokenVector.push_back(_String(lCurrentToken, y));
				y = 0;
				if (++lSplitCount >= pSplitMaxCount)
				{
					x = pString.find_first_not_of(pCharDelimitors, x);
					break;
				}
			}
			lTakeNextString = false;
		}
		else
		{
			lCurrentToken[y++] = pString[x];
			lTakeNextString = true;
		}
		deb_assert(y < sizeof(lCurrentToken));
	}
	// If we reached end of string while parsing.
	if (y != 0)
	{
		pTokenVector.push_back(_String(lCurrentToken, y));
	}
	if (x < pString.length())	// If we stopped splitting due to split maximum count.
	{
		pTokenVector.push_back(pString.substr(x));
	}
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Join(const strvec& pStringVector, const _String& pJoinString, size_t pStartIndex, size_t pEndIndex)
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

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::Append(strvec& pDestination, const strvec& pAppend)
{
		pDestination.insert(pDestination.end(), pAppend.begin(), pAppend.end());
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::StringToCString(const _String& pString)
{
	_String lCString;
	size_t lLength = pString.length();
	for (size_t x = 0; x < lLength; ++x)
	{
		switch (pString[x])
		{
			case '\\':	lCString += "\\\\";	break;
			case '\t':	lCString += "\\t";	break;
			case '\v':	lCString += "\\v";	break;
			case '\r':	lCString += "\\r";	break;
			case '\n':	lCString += "\\n";	break;
			case '"':	lCString += "\\\"";	break;
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
		if (pCString[x] == '\\')
		{
			if (x+1 >= lLength)
			{
				lOk = false;
				break;
			}
			switch (pCString[x+1])
			{
				case 'n':
				{
					pString += '\n';
					++x;
				}
				break;
				case 'r':
				{
					pString += '\r';
					++x;
				}
				break;
				case 't':
				{
					pString += '\t';
					++x;
				}
				break;
				case 'v':
				{
					pString += '\v';
					++x;
				}
				break;
				case '\\':
				{
					pString += '\\';
					++x;
				}
				break;
				case '"':
				{
					pString += '"';
					++x;
				}
				break;
				case 'x':
				{
					const char* lNumber = pCString.c_str()+x+2;
					char* lEnd = 0;
					int lCharacter = StrToL(lNumber, &lEnd, 16);
					pString += (char)lCharacter;
					x += 1+(unsigned)(lEnd-lNumber);
				}
				break;
				case '0':
				{
					const char* lNumber = pCString.c_str()+x+2;
					char* lEnd = 0;
					int lCharacter = StrToL(lNumber, &lEnd, 8);
					pString += (char)lCharacter;
					x += 1+(unsigned)(lEnd-lNumber);
				}
				break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					const char* lNumber = pCString.c_str()+x+1;
					char* lEnd = 0;
					int lCharacter = StrToL(lNumber, &lEnd, 10);
					pString += (char)lCharacter;
					x += 1+(unsigned)(lEnd-lNumber);
				}
				break;
				default:
				{
					pString += '\\';
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
	if (lTokenStartPosition == 0)
	{
		return pString;
	}
	if (lTokenStartPosition != _String::npos)
	{
		return (pString.substr(lTokenStartPosition));
	}
	return (_String());
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::StripRight(const _String& pString, const _String& pCharDelimitors)
{
	// Skip delimitor characters at end, find end of last token.
	size_t lTokenEndPosition = pString.find_last_not_of(pCharDelimitors, pString.size()-1);
	if (lTokenEndPosition != _String::npos)
	{
		return (pString.substr(0, lTokenEndPosition+1));
	}
	return (_String());
}

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Strip(const _String& pString, const _String& pCharDelimitors)
{
	return StripRight(StripLeft(pString, pCharDelimitors), pCharDelimitors);
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

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::ReplaceAll(const _String& pString, const _String& pFrom, const _String& pTo)
{
	_String lString = pString;
	typename _String::size_type lPosition = 0;
	while ((lPosition = lString.find(pFrom, lPosition)) != str::npos)
	{
		lString.replace(lPosition, pFrom.length(), pTo);
		lPosition += pTo.length();
	}
	return (lString);
}



STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::Format(const typename _String::value_type* pFormat, ...)
{
	va_list	lArguments;
	va_start(lArguments, pFormat);
	_String lResult = VFormat(pFormat, lArguments);
	va_end(lArguments);
	return (lResult);
}



STR_UTIL_TEMPLATE size_t STR_UTIL_QUAL::FindPreviousWord(const _String& pLine, const _String& pDelimitors, size_t pStartIndex)
{
	_String lReverseLine = Reverse(pLine);
	size_t lReverseIndex = pLine.length()-pStartIndex;
	size_t lIndex = lReverseLine.find_first_not_of(pDelimitors, lReverseIndex);
	lIndex = lReverseLine.find_first_of(pDelimitors, lIndex);
	if (lIndex != str::npos)
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
	if (lIndex != str::npos)
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

STR_UTIL_TEMPLATE void STR_UTIL_QUAL::StripAllWhiteSpaces(_String& pString)
{
	int lDestIndex = 0;
	for (size_t i = 0; i < pString.length(); i++)
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
	str lDataString(pLength*2, '?');
	for (size_t x = 0; x < pLength; ++x)
	{
		lDataString[x*2+0] = lHexTable[pData[x]>>4];
		lDataString[x*2+1] = lHexTable[pData[x]&0xF];
	}
	return lDataString;
}



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
	if (lSign)
	{
		pValue = -pValue;
	}

	static typename _String::value_type lString[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i = 62;	// Leave a zero at the end.
	if (pRadix == 10)
	{
		while (pValue > 0)
		{
			const int lDigit = (int)(pValue % 10);
			lString[i--] = (typename _String::value_type)('0' + lDigit);
			pValue /= 10;
		}
	}
	else
	{
		while (pValue > 0)
		{
			int lDigit = (int)(pValue % pRadix);
			pValue /= pRadix;

			if (lDigit >= 0 && lDigit <= 9)
			{
				lString[i--] = (typename _String::value_type)('0' + lDigit);
			}
			else if(lDigit > 9)
			{
				lString[i--] = (typename _String::value_type)('a' + (lDigit - 10));
			}
		}
	}
	if (lSign)
	{
		lString[i--] = '-';
	}
	if (i == 62)	// Empty string means zero.
	{
		lString[i--] = '0';
	}
	return &lString[i+1];
}

STR_UTIL_TEMPLATE bool STR_UTIL_QUAL::StringToBool(const _String& pString, bool& pValue)
{
	bool lOk = false;
	if (pString == "false")
	{
		pValue = false;
		lOk = true;
	}
	else if (pString == "true")
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

STR_UTIL_TEMPLATE _String STR_UTIL_QUAL::FastDoubleToString(double pValue)
{
	if (pValue > -1000 && pValue < 1000)
	{
		int lValue = (int)(pValue*1000000);
		bool lPositive;
		if (lValue >= 0)
		{
			lPositive = true;
		}
		else
		{
			lPositive = false;
			lValue = -lValue;
		}
		const size_t lBufferSize = 11;
		typename _String::value_type lString[lBufferSize];
		size_t lIndex = lBufferSize-1;
		int x;
		for (x = 0; x < 5; ++x, lValue /= 10)
		{
			int lDigit;
			if ((lDigit = lValue%10) != 0)
			{
				lString[lIndex--] = '0' + typename _String::value_type(lDigit);
			}
		}
		for (; x < 6; ++x, lValue /= 10)
		{
			lString[lIndex--] = '0' + typename _String::value_type(lValue%10);
		}
		lString[lIndex--] = '.';
		lString[lIndex--] = '0' + typename _String::value_type(lValue%10);
		lValue /= 10;
		for (x = 0; lValue && x < 2; ++x, lValue /= 10)
		{
			lString[lIndex--] = '0' + typename _String::value_type(lValue%10);
		}
		if (lPositive)
		{
			++lIndex;
		}
		else
		{
			lString[lIndex] = '-';
		}
		const size_t lCount = lBufferSize-lIndex;
		return _String(&lString[lIndex], lCount);
	}
	else
	{
		return DoubleToString(pValue, 6);
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
