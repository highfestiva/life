
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#pragma once

#include "Cure.h"



namespace happyhttp
{
class Connection;
class Response;
}



namespace Cure
{



class HiscoreAgent
{
public:
	struct Entry
	{
		str mName;
		int mScore;
	};

	struct List
	{
		int mOffset;
		int mTotalCount;
		typedef std::vector<Entry> EntryList;
		EntryList mEntryList;
	};

	HiscoreAgent(const str& pHost, const int pPort, const str& pGameName);
	virtual ~HiscoreAgent();
	void Close();
	ResourceLoadState Poll();

	ResourceLoadState GetLoadState() const;
	void SetLoadState(ResourceLoadState pLoadState);
	bool StartDownloadingList(const str& pPlatform, const str& pLevel, const str& pAvatar, int pOffset, int pLimit);
	const List& GetDownloadedList() const;
	bool StartUploadingScore(const str& pPlatform, const str& pLevel, const str& pAvatar, const str& pName, int pScore);
	int GetUploadedPlace() const;

	bool ParseList(astr& pData);	// For testing 'JSON parsing'...
	bool ParseScore(astr& pData);	// For testing 'JSON parsing'...

private:
	void Reopen();
	void AppendData(const astr& pData);
	bool CompleteList();
	bool CompleteScore();
	static void OnData(const happyhttp::Response* pResponse, void* pUserData, const unsigned char* pData, int pByteCount);
	static void OnListComplete(const happyhttp::Response* pResponse, void* pUserData);
	static void OnScoreComplete(const happyhttp::Response* pResponse, void* pUserData);
	str Hypnotize(const str& pPlatform, const str& pLevel, const str& pAvatar, const str& pName, int pScore);	// A sorta shuffle

	const str mServerHost;
	const int mServerPort;
	happyhttp::Connection* mConnection;
	const str mGameName;
	ResourceLoadState mLoadState;
	List mDownloadedList;
	int mUploadedPlace;
	astr mResponseData;

	void operator=(const HiscoreAgent&);

	LOG_CLASS_DECLARE();
};



}
