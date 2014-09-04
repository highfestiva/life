
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/HiscoreAgent.h"
#include "../../Lepra/Include/LepraAssert.h"
#include <happyhttp.h>
#include "../../Lepra/Include/Obfuxator.h"
#include "../../Lepra/Include/JsonString.h"
#include "../../Lepra/Include/SHA1.h"
#include "../../Tbc/Include/ChunkyBoneGeometry.h"



namespace Cure
{



HiscoreAgent::HiscoreAgent(const str& pHost, const int pPort, const str& pGameName):
	mServerHost(pHost),
	mServerPort(pPort),
	mConnection(0),
	mGameName(pGameName),
	mAction(ACTION_NONE),
	mConnectorThread("sxnt")
{
	Close();
}

HiscoreAgent::~HiscoreAgent()
{
	Close();
}

void HiscoreAgent::Close()
{
	log_adebug("Closing and resetting state.");
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
	if (mConnectorThread.IsRunning())
	{
		// Still connecting.
		return RESOURCE_LOAD_IN_PROGRESS;
	}
	if (!mConnection)
	{
		// Connector thread had some problem.
		return RESOURCE_LOAD_ERROR;
	}

	log_adebug("Polling connection.");
	if (GetLoadState() == RESOURCE_LOAD_IN_PROGRESS && mConnection->outstanding())
	{
		try
		{
			mConnection->pump();
		}
		catch (happyhttp::Wobbly& e)
		{
			(void)e;
			log_volatile(mLog.Warning(_T("Problem polling connection: ") + strutil::Encode(e.what())));
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

HiscoreAgent::Action HiscoreAgent::GetAction() const
{
	return mAction;
}

void HiscoreAgent::SetLoadState(ResourceLoadState pLoadState)
{
	mLoadState = pLoadState;
}

bool HiscoreAgent::StartDownloadingList(const str& pPlatform, const str& pLevel, const str& pAvatar, int pOffset, int pLimit)
{
	if (!mConnectorThread.Join(0.1))
	{
		deb_assert(false);
		return false;
	}
	const str lFormat = _O("o@y+_.R2=*8/,1ay+x2&9(92Qay+x=(=*=,ayL+x/8e8+9*ay5x2515&*ay5g", "/%s?platform=%s&level=%s&avatar=%s&offset=%i&limit=%i");
	mConnectorPath = strutil::Format(lFormat.c_str(),
		mGameName.c_str(), pPlatform.c_str(), pLevel.c_str(), pAvatar.c_str(), pOffset, pLimit);
	return mConnectorThread.Start(this, &HiscoreAgent::DownloadThreadEntry);
}

const HiscoreAgent::List& HiscoreAgent::GetDownloadedList() const
{
	return mDownloadedList;
}

bool HiscoreAgent::StartUploadingScore(const str& pPlatform, const str& pLevel, const str& pAvatar, const str& pName, int pScore)
{
	if (!mConnectorThread.Join(0.1))
	{
		deb_assert(false);
		return false;
	}
	str lJsonName = JsonString::ToJson(pName);
	if (lJsonName.empty())
	{
		deb_assert(false);
		return false;
	}
	lJsonName = lJsonName.substr(1, lJsonName.length()-2);
	const int lTimeStamp = (int32)::time(0);
	mConnectorHash = Hypnotize(pPlatform, pLevel, pAvatar, lJsonName, pScore, lTimeStamp);
	const str lFormat = _O(".}2=*8?/,1ay+x29(92-ay+x(=(=*=,ay+x0=,19ayw+x+;/,9ay5x* 519apy5", "platform=%s&level=%s&avatar=%s&name=%s&score=%i&time=%i");
	lJsonName = JsonString::UrlEncode(lJsonName);
	mConnectorBody = strutil::Format(lFormat.c_str(),
		pPlatform.c_str(), pLevel.c_str(), pAvatar.c_str(), lJsonName.c_str(), pScore, lTimeStamp);
	mConnectorPath = _O("oe=::?`90*,%o", "/add_entry/") + mGameName;
	return mConnectorThread.Start(this, &HiscoreAgent::UploadThreadEntry);
}

int HiscoreAgent::GetUploadedPlace() const
{
	return mUploadedPlace;
}



bool HiscoreAgent::ParseList(astr& pData)
{
	// Well, hrm... parse the JSON. Quick and dirty was the theme of the day.
	mLoadState = RESOURCE_LOAD_ERROR;
	log_debug(_T("Parsing downloaded list: ") + strutil::Encode(pData));
	if (pData.length() < 10 || pData[0] != '{' || pData[pData.length()-1] != '}')
	{
		log_volatile(mLog.Warning(_T("Problem parsing list, not our JSON: ") + strutil::Encode(pData)));
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
		log_volatile(mLog.Warning(_T("Problem parsing list; list or other data missing: ") + lData));
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
				log_volatile(mLog.Warning(_T("Problem parsing download list offset (int): ") + lTagValue[lBase+1]));
				return false;
			}
		}
		else if (lTagValue[lBase+0] == _T("\"total_count\""))
		{
			if (!strutil::StringToInt(lTagValue[lBase+1], mDownloadedList.mTotalCount))
			{
				log_volatile(mLog.Warning(_T("Problem parsing download list total_count (int): ") + lTagValue[lBase+1]));
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
		if (lTagValue.size() < 2)
		{
			continue;
		}
		const size_t lBase = lTagValue[0].empty()? 1 : 0;
		if (lTagValue[lBase+0] == _T("\"name\""))
		{
			if (lTagValue[lBase+1].empty())
			{
				log_volatile(mLog.Warning(_T("Problem parsing download list name (empty).")));
				return false;
			}
			lEntry.mName = JsonString::FromJson(lTagValue[lBase+1]);
			lFlags |= 1;
		}
		else if (lTagValue[lBase+0] == _T("\"score\""))
		{
			if (!strutil::StringToInt(lTagValue[lBase+1], lEntry.mScore))
			{
				log_volatile(mLog.Warning(_T("Problem parsing download list score (int): ") + lTagValue[lBase+1]));
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
		log_volatile(mLog.Warning(_T("Problem parsing hiscore list data: ") + lData));
		return false;
	}
	mLoadState = RESOURCE_LOAD_COMPLETE;
	log_volatile(mLog.AInfo("Hiscore list data parsed OK."));
	return true;
}

bool HiscoreAgent::ParseScore(astr& pData)
{
	mLoadState = RESOURCE_LOAD_ERROR;
	log_debug(_T("Parsing uploaded score: ") + strutil::Encode(pData));
	if (pData.length() < 10 || pData[0] != '{' || pData[pData.length()-1] != '}')
	{
		log_volatile(mLog.Warning(_T("Problem parsing uploaded score, not our JSON: ") + strutil::Encode(pData)));
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
			log_volatile(mLog.Warning(_T("Problem parsing tags of uploaded score: ") + lData));
			return false;
		}
		if (lTagValue[lBase+0] == _T("\"offset\""))
		{
			if (!strutil::StringToInt(lTagValue[lBase+1], mUploadedPlace))
			{
				log_volatile(mLog.Warning(_T("Problem parsing uploaded score offset (int): ") + lTagValue[lBase+1]));
				return false;
			}
		}
	}
	if (mUploadedPlace < 0)
	{
		log_volatile(mLog.Warning(_T("Problem parsing own placement when uploaded score: ") + lData));
		return false;
	}
	mLoadState = RESOURCE_LOAD_COMPLETE;
	log_volatile(mLog.AInfo("Hiscore list data parsed OK."));
	return true;
}



void HiscoreAgent::Reopen()
{
	Close();
	log_adebug("Reopening connection.");
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
		log_volatile(mLog.Warningf(_T("HTTP error %i when receiving data."), pResponse->getstatus()));
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
		log_volatile(mLog.Warningf(_T("HTTP error %i when completing download list data reception."), pResponse->getstatus()));
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
		log_volatile(mLog.Warningf(_T("HTTP error %i when completing upload score data reception."), pResponse->getstatus()));
		lThis->SetLoadState(RESOURCE_LOAD_ERROR);
		return;
	}
	lThis->CompleteScore();
}

str HiscoreAgent::Hypnotize(const str& pPlatform, const str& pLevel, const str& pAvatar, const str& pName, int pScore, int pTimeStamp)
{
	const str lFormat = _O("vF|}XV^^y5y+ y+?y+?Uy+??ry+qy5q={9w1:Ci7", "(\"!FH@%i%s~%s_%s_%s__%s-%i-a#e'md5g");
	str lOrigin = strutil::Format(lFormat.c_str(),
		pTimeStamp+1, mGameName.c_str(), pPlatform.c_str(),
		pLevel.c_str(), pAvatar.c_str(), pName.c_str(), pScore-1);
	mLog.Infof(_T("data = %s"), lOrigin.c_str());
	astr lUtfString = astrutil::Encode(lOrigin);
	//::memset((void*)lOrigin.c_str(), 0, lOrigin.size()*sizeof(tchar));
	uint8 lSha1Hash[20];
	SHA1::Hash((const uint8*)lUtfString.c_str(), lUtfString.length(), lSha1Hash);
	//::memset((void*)lUtfString.c_str(), 0, lUtfString.length());
	astr lInputHexdigest = astrutil::DumpData(lSha1Hash, sizeof(lSha1Hash));
	astrutil::ToLower(lInputHexdigest);
	//mLog.Infof(_T("sha1 = %s"), strutil::Encode(lInputHexdigest).c_str());

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
	lInputHexdigest = lInputHexdigest.substr(0, 15) + (char)chk + lInputHexdigest.substr(15) + (char)(chk+1);
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
	mLog.Infof(_T("output hash = %s"), lShuffledHash.c_str());
	return lShuffledHash;
}

void HiscoreAgent::DownloadThreadEntry()
{
	try
	{
		mAction = ACTION_DOWNLOAD_LIST;
		Reopen();
		mConnection->setcallbacks(0, &HiscoreAgent::OnData, &HiscoreAgent::OnListComplete, this);
		astr lMethod = _OA("WzYJ", "GET");
		astr lAPath = astrutil::Encode(mConnectorPath);
		mConnection->request(lMethod.c_str(), lAPath.c_str());
		log_volatile(mLog.AInfo("Downloading highscore list."));
	}
	catch (happyhttp::Wobbly& e)
	{
		(void)e;
		log_volatile(mLog.Warning(_T("Problem retrieving list: ") + strutil::Encode(e.what())));
		delete mConnection;
		mConnection = 0;
	}
}

void HiscoreAgent::UploadThreadEntry()
{
	try
	{
		mAction = ACTION_UPLOAD_SCORE;
		Reopen();
		mConnection->setcallbacks(0, &HiscoreAgent::OnData, &HiscoreAgent::OnScoreComplete, this);
		const astr lClient = _OA("[\"RUYP_J", "CLIENT");
		const astr lHash = astrutil::Encode(mConnectorHash);
		const astr lAccept = _OA("]x;;9.$*", "Accept");
		const astr lPlain = _OA("*{9&*o&.2=50", "text/plain");
		const char* lHeaders[] = 
		{
			lClient.c_str(), lHash.c_str(),
			lAccept.c_str(), lPlain.c_str(),
			0
		};
		const astr lMethod = _OA("NaOKJ", "POST");
		const astr lUtf8Body = astrutil::ReplaceAll(astrutil::Encode(mConnectorBody), "\\", "%5C");
		mConnection->request(lMethod.c_str(), astrutil::Encode(mConnectorPath).c_str(), lHeaders, (const unsigned char*)lUtf8Body.c_str(), lUtf8Body.length());
		log_volatile(mLog.AInfo("Uploading score."));
	}
	catch (happyhttp::Wobbly& e)
	{
		(void)e;
		log_volatile(mLog.Warning(_T("Problem uploading score: ") + strutil::Encode(e.what())));
		delete mConnection;
		mConnection = 0;
	}
}



void HiscoreAgent::operator=(const HiscoreAgent&)
{
	deb_assert(false);
}



loginstance(NETWORK_CLIENT, HiscoreAgent);



}
