/*
	Class:  File
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/File.h"
#include "../Include/Random.h"

namespace Lepra
{

void File::SetEndian(Endian::EndianType pEndian)
{
	Reader::SetReaderEndian(pEndian);
	Writer::SetWriterEndian(pEndian);
}

IOError File::Skip(unsigned pSize)
{
	IOError lStatus = IO_OK;
	int64 lTargetPos = Tell() + (int64)GetSize();
	if (SeekCur(pSize) != lTargetPos)
	{
		lStatus = IO_ERROR_READING_FROM_STREAM;
	}
	return (lStatus);
}

int64 File::SeekSet(int64 pOffset)
{
	return (Seek(pOffset, FSEEK_SET));
}

int64 File::SeekCur(int64 pOffset)
{
	return (Seek(pOffset, FSEEK_CUR));
}

int64 File::SeekEnd(int64 pOffset)
{
	return (Seek(pOffset, FSEEK_END));
}

int64 File::ReadString(String& pString)
{
	if (GetMode(READ_MODE) == false)
	{
		return -1;
	}

	pString = _T("");

	bool lDone = false;
	while (!lDone)
	{
		int lReadSize = 128*sizeof(tchar);
		int64 lFileSize = GetSize();

		int64 lPrevPos = Tell();
		if (lPrevPos + (int64)lReadSize > lFileSize)
		{
			lReadSize = (int)(lFileSize - Tell());
		}

		tchar lData[128];
		ReadData(lData, lReadSize);

		// Get the string length..
		int lLength;
		for (lLength = 0; lLength < lReadSize && lData[lLength] != 0; ++lLength)
			;	// TRICKY: loop expression does the checking = no body.

		// If we found the null character, we are done.
		if (lData[lLength] == 0)
		{
			SeekSet(lPrevPos + (int64)lLength + 1);
			lDone = true;
		}
		else
		{
			SeekSet(lPrevPos + (int64)lLength);
		}

		pString.assign(lData, lLength);
	}

	return pString.length();
}

bool File::WriteString(const String& pString)
{
	if (GetMode(WRITE_MODE) == false)
		return false;

	return (WriteData(pString.c_str(), pString.length()*sizeof(Lepra::tchar) + 1) == (int)pString.length() + 1);
}

/*
JB: doesn't work: (currently only used under Posix).
If ever put back: place it in Lepra::Path instead!

bool File::CompareFileName(const String& pFileName, const String& pCompareString)
{
	static bool slInitialized = false;
	static String slSpecialTokens;

	if (slInitialized == false)
	{
		slSpecialTokens.AddToken(_T("?"));
		slSpecialTokens.AddToken(_T("*"));
		slInitialized = true;
	}

	int lCSIndex = 0;
	int lFNIndex = 0;
	bool lAsterisk = false;
	bool lReturn = false;
	String lToken;

	while (lCSIndex >= 0)
	{
		lCSIndex = pCompareString.GetToken(lToken, lCSIndex, _T(""), slSpecialTokens, true);

		if (lCSIndex >= 0)
		{
			if (lToken == _T("?"))
			{
				if (lAsterisk == false)
				{
					lFNIndex++;
				}
			}
			else if(lToken == _T("*"))
			{
				lAsterisk = true;
			}
			else
			{
				lFNIndex = pFileName.Find(lToken.c_str(), lFNIndex);
				if (lAsterisk == false && lFNIndex != 0)
				{
					lReturn = false;
					break;
				}

				lAsterisk = false;
			}
		}
	}

	return lReturn;
}
*/

} // End namespace.
