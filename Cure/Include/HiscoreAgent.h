
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#pragma once

#include "Cure.h"
#include "../../Lepra/Include/MemberThread.h"



namespace happyhttp
{
class Connection;
class Response;
}



namespace Cure
{



// Interface towards my GAE (Google App Engine) Python 2.5 app called gamehiscore (which can be found at
// ssh://rg@pixeldoctrine.dyndns.org:2202/~/hiscore.git). The app automagically redirects from
// gamehiscore.pixeldoctrine.com to some other free hosting site in the cloud.
class HiscoreAgent
{
public:
	enum Action
	{
		ACTION_NONE = 1,
		ACTION_DOWNLOAD_LIST,
		ACTION_UPLOAD_SCORE,
	};

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
	Action GetAction() const;
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
	str Hypnotize(const str& pPlatform, const str& pLevel, const str& pAvatar, const str& pName, int pScore, int pTimeStamp);	// A sorta shuffle
	void DownloadThreadEntry();
	void UploadThreadEntry();

	const str mServerHost;
	const int mServerPort;
	happyhttp::Connection* mConnection;
	const str mGameName;
	ResourceLoadState mLoadState;
	List mDownloadedList;
	int mUploadedPlace;
	astr mResponseData;
	Action mAction;
	MemberThread<HiscoreAgent> mConnectorThread;
	str mConnectorPath;
	str mConnectorBody;
	str mConnectorHash;

	void operator=(const HiscoreAgent&);

	LOG_CLASS_DECLARE();
};



}
