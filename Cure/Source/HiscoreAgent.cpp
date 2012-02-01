
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/HiscoreAgent.h"
#include <assert.h>
#include <happyhttp.h>
#include "../../Lepra/Include/Obfuxator.h"
#include "../../Lepra/Include/SHA1.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"



namespace Cure
{



HiscoreAgent::HiscoreAgent(const str& pHost, const int pPort, const str& pGameName):
	mServerHost(pHost),
	mServerPort(pPort),
	mConnection(0),
	mGameName(pGameName)
{
	Close();
}

HiscoreAgent::~HiscoreAgent()
{
	Close();
}

void HiscoreAgent::Close()
{
	delete mConnection;
	mConnection = 0;

	mLoadState = RESOURCE_UNLOADED;
	mDownloadedList.mOffset = -1;
	mDownloadedList.mTotalCount = -1;
	mDownloadedList.mEntryList.clear();
	mUploadedPlace = -1;
	mResponseData.clear();
}

ResourceLoadState HiscoreAgent::Poll()
{
	if (GetLoadState() == RESOURCE_LOAD_IN_PROGRESS && mConnection->outstanding())
	{
		try
		{
			mConnection->pump();
		}
		catch (happyhttp::Wobbly&)
		{
			SetLoadState(RESOURCE_LOAD_ERROR);
		}
	}
	if (GetLoadState() == RESOURCE_LOAD_ERROR && mConnection)
	{
		// Bail if something trashed us earlier.
		Close();
		SetLoadState(RESOURCE_LOAD_ERROR);	// Reset load state after close.
	}
	return GetLoadState();
}



ResourceLoadState HiscoreAgent::GetLoadState() const
{
	return mLoadState;
}

void HiscoreAgent::SetLoadState(ResourceLoadState pLoadState)
{
	mLoadState = pLoadState;
}

bool HiscoreAgent::StartDownloadingList(const str& pPlatform, const str& pLevel, const str& pAvatar, int pOffset, int pLimit)
{
	try
	{
		Reopen();
		mConnection->setcallbacks(0, &HiscoreAgent::OnData, &HiscoreAgent::OnListComplete, this);
		const str lFormat = _O("o|_7=1=9ay+x.2=*8/,:1ay+Mx29(92ay+x=(<=*=,)ay+x/88+9*ay]5x25=15*ay5", "/?game=%s&platform=%s&level=%s&avatar=%s&offset=%i&limit=%i");
		str lPath = strutil::Format(lFormat.c_str(),
			mGameName.c_str(), pPlatform.c_str(), pLevel.c_str(), pAvatar.c_str(), pOffset, pLimit);
		astr lMethod = _OA("WzYJ", "GET");
		astr lAPath = astrutil::Encode(lPath);
		mConnection->request(lMethod.c_str(), lAPath.c_str());
		return true;
	}
	catch (happyhttp::Wobbly&)
	{
		return false;
	}
}

const HiscoreAgent::List& HiscoreAgent::GetDownloadedList() const
{
	return mDownloadedList;
}

bool HiscoreAgent::StartUploadingScore(const str& pPlatform, const str& pLevel, const str& pAvatar, const str& pName, int pScore)
{
	try
	{
		Reopen();
		const str lFormat = _O("7L=19aWy+x.2=*8/,1aoy+x2V9(92ay+x=(=*|=,ay?+x0=19ay+x+;}/,9a!y5", "game=%s&platform=%s&level=%s&avatar=%s&name=%s&score=%i");
		str lBody = strutil::Format(lFormat.c_str(),
			mGameName.c_str(), pPlatform.c_str(), pLevel.c_str(), pAvatar.c_str(), pName.c_str(), pScore);
		mConnection->setcallbacks(0, &HiscoreAgent::OnData, &HiscoreAgent::OnScoreComplete, this);
		const astr lClient = _OA("[\"RUYP_J", "CLIENT");
		const astr lHash = astrutil::Encode(Hypnotize(pPlatform, pLevel, pAvatar, pName, pScore));
		const astr lAccept = _OA("]x;;9.$*", "Accept");
		const astr lPlain = _OA("*{9&*o&.2=50", "text/plain");
		const char* lHeaders[] = 
		{
			lClient.c_str(), lHash.c_str(),
			lAccept.c_str(), lPlain.c_str(),
			0
		};
		const astr lMethod = _OA("NaOKJ", "POST");
		const astr lPath = _OA("oH=::?$90*,%", "/add_entry");
		const astr lUtf8Body = astrutil::Encode(lBody);
		mConnection->request(lMethod.c_str(), lPath.c_str(), lHeaders, (const unsigned char*)lUtf8Body.c_str(), lUtf8Body.length());
		return true;
	}
	catch (happyhttp::Wobbly&)
	{
		return false;
	}
}

int HiscoreAgent::GetUploadedPlace() const
{
	return mUploadedPlace;
}



bool HiscoreAgent::ParseList(astr& pData)
{
	// Well, hrm... parse the JSON. Quick and dirty was the theme of the day.
	mLoadState = RESOURCE_LOAD_ERROR;
	if (pData.length() < 10 || pData[0] != '{' || pData[pData.length()-1] != '}')
	{
		return false;
	}
	pData.resize(pData.length()-1);
	str lData = strutil::Encode(&pData[1]);
	size_t p;
	if ((p = lData.find(_T("[]"))) != str::npos)
	{
		// Check for [] (empty list) and add a space for lousy parsing.
		lData.replace(p, 2, _T("[ ]"), 3);
	}
	str lList;
	typedef strutil::strvec sv;
	sv lStrings = strutil::BlockSplit(lData, _T("[]"), true, true);
	str lOther;
	for (sv::iterator x = lStrings.begin(); x != lStrings.end(); ++x)
	{
		lOther += *x;
		size_t y = strutil::FindPreviousWord(*x, _T("\": ,"), x->length()-1);
		if (x->substr(y, 4) == _T("list"))
		{
			++x;
			lList = *x;
		}
	}
	if (lOther.empty() || lList.empty())
	{
		return false;
	}
	lStrings = strutil::BlockSplit(lOther, _T(","), true, true);
	for (sv::iterator x = lStrings.begin(); x != lStrings.end(); ++x)
	{
		sv lTagValue = strutil::BlockSplit(*x, _T(": \t\r\n"), true, true);
		const size_t lBase = lTagValue[0].empty()? 1 : 0;
		if (lTagValue.size() < lBase+2)
		{
			continue;
		}
		if (lTagValue[lBase+0] == _T("\"offset\""))
		{
			if (!strutil::StringToInt(lTagValue[lBase+1], mDownloadedList.mOffset))
			{
				return false;
			}
		}
		else if (lTagValue[lBase+0] == _T("\"total_count\""))
		{
			if (!strutil::StringToInt(lTagValue[lBase+1], mDownloadedList.mTotalCount))
			{
				return false;
			}
		}
	}
	Entry lEntry;
	int lFlags = 0;
	lStrings = strutil::BlockSplit(lList, _T("{},"), true, true);
	for (sv::iterator x = lStrings.begin(); x != lStrings.end(); ++x)
	{
		sv lTagValue = strutil::BlockSplit(*x, _T(": \t\r\n"), true, true);
		if (lTagValue.empty())
		{
			continue;
		}
		const size_t lBase = lTagValue[0].empty()? 1 : 0;
		if (lTagValue[lBase+0] == _T("\"name\""))
		{
			if (lTagValue[lBase+1].empty())
			{
				return false;
			}
			lEntry.mName = lTagValue[lBase+1].substr(1, lTagValue[lBase+1].length()-2);
			lFlags |= 1;
		}
		else if (lTagValue[lBase+0] == _T("\"score\""))
		{
			if (!strutil::StringToInt(lTagValue[lBase+1], lEntry.mScore))
			{
				return false;
			}
			lFlags |= 2;
		}
		if ((lFlags&3) == 3)
		{
			mDownloadedList.mEntryList.push_back(lEntry);
			lFlags = 0;
		}
	}
	if (mDownloadedList.mOffset < 0 ||
		mDownloadedList.mTotalCount < 0 ||
		(int)mDownloadedList.mEntryList.size() > mDownloadedList.mOffset+mDownloadedList.mTotalCount)
	{
		return false;
	}
	mLoadState = RESOURCE_LOAD_COMPLETE;
	return true;
}

bool HiscoreAgent::ParseScore(astr& pData)
{
	mLoadState = RESOURCE_LOAD_ERROR;
	if (pData.length() < 10 || pData[0] != '{' || pData[pData.length()-1] != '}')
	{
		return false;
	}
	pData.resize(pData.length()-1);
	str lData = strutil::Encode(&pData[1]);
	typedef strutil::strvec sv;
	sv lStrings = strutil::BlockSplit(lData, _T(","), true, true);
	for (sv::iterator x = lStrings.begin(); x != lStrings.end(); ++x)
	{
		sv lTagValue = strutil::BlockSplit(*x, _T(": \t\r\n"), true, true);
		const size_t lBase = lTagValue[0].empty()? 1 : 0;
		if (lTagValue.size() < 2)
		{
			return false;
		}
		if (lTagValue[lBase+0] == _T("\"offset\""))
		{
			if (!strutil::StringToInt(lTagValue[lBase+1], mUploadedPlace))
			{
				return false;
			}
		}
	}
	if (mUploadedPlace < 0)
	{
		return false;
	}
	mLoadState = RESOURCE_LOAD_COMPLETE;
	return true;
}



void HiscoreAgent::Reopen()
{
	Close();
	mConnection = new happyhttp::Connection(astrutil::Encode(mServerHost).c_str(), mServerPort);
	mLoadState = RESOURCE_LOAD_IN_PROGRESS;
}

void HiscoreAgent::AppendData(const astr& pData)
{
	mResponseData += pData;
}

bool HiscoreAgent::CompleteList()
{
	return ParseList(mResponseData);
}

bool HiscoreAgent::CompleteScore()
{
	// Well, hrm... parse the JSON. Quick and dirty was the theme of the day.
	return ParseScore(mResponseData);
}

void HiscoreAgent::OnData(const happyhttp::Response* pResponse, void* pUserData, const unsigned char* pData, int pByteCount)
{
	HiscoreAgent* lThis = (HiscoreAgent*)pUserData;
	if (pResponse->getstatus() != 200)
	{
		lThis->SetLoadState(RESOURCE_LOAD_ERROR);
		return;
	}
	lThis->AppendData(astr((const char*)pData, pByteCount));
}

void HiscoreAgent::OnListComplete(const happyhttp::Response* pResponse, void* pUserData)
{
	HiscoreAgent* lThis = (HiscoreAgent*)pUserData;
	if (pResponse->getstatus() != 200)
	{
		lThis->SetLoadState(RESOURCE_LOAD_ERROR);
		return;
	}
	lThis->CompleteList();
}

void HiscoreAgent::OnScoreComplete(const happyhttp::Response* pResponse, void* pUserData)
{
	HiscoreAgent* lThis = (HiscoreAgent*)pUserData;
	if (pResponse->getstatus() != 200)
	{
		lThis->SetLoadState(RESOURCE_LOAD_ERROR);
		return;
	}
	lThis->CompleteScore();
}

str HiscoreAgent::Hypnotize(const str& pPlatform, const str& pLevel, const str& pAvatar, const str& pName, int pScore)
{
	const str lFormat = _O("v+|}XV2^y+ y+?y+?y+7??y+aqy5q={9w1:i7i", "(\"!FH@%s~%s_%s_%s__%s-%i-a#e'md5g");
	str lOrigin = strutil::Format(lFormat.c_str(),
		mGameName.c_str(), pPlatform.c_str(), pLevel.c_str(), pAvatar.c_str(), pName.c_str(), pScore);
	astr lUtfString = astrutil::Encode(lOrigin);
	uint8 lSha1Hash[20];
	SHA1::Hash((uint8*)lUtfString.c_str(), lUtfString.length(), lSha1Hash);
	str lInputHexdigest = strutil::DumpData(lSha1Hash, sizeof(lSha1Hash));
	strutil::ToLower(lInputHexdigest);

	// Shuffle:
	//input_hexdigest = list(input_hexdigest)
	int i = 0;
	int j = lInputHexdigest.length()-2;
	int chk = (int)'B';
	while (i < j)
	{
		std::swap(lInputHexdigest[i], lInputHexdigest[j]);
		chk ^= (int)lInputHexdigest[i] + j;
		i += 3;
		j -= 5;
	}
	chk = (chk % 25) + (int)'a';
	lInputHexdigest = lInputHexdigest.substr(0, 15) + (tchar)chk + lInputHexdigest.substr(15) + (tchar)(chk+1);
	int ints[] = { 0, 0, 0, 0, 0, 0, 0 };
	for (int x = 0; x < 6; ++x)
	{
		for (int y = 0; y < 7; ++y)
		{
			ints[y] += (int)lInputHexdigest[x*7+y] - 73 - y - x*5;
		}
	}
	str lShuffledHash;
	for (int y = 0; y < 7; ++y)
	{
		lShuffledHash += strutil::IntToString(::abs(ints[y]), (y&1)? 9 : 10);
	}
	return lShuffledHash;
}



void HiscoreAgent::operator=(const HiscoreAgent&)
{
	assert(false);
}



}
