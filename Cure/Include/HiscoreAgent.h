
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "cure.h"
#include "../../lepra/include/memberthread.h"



namespace happyhttp {
class Connection;
class Response;
}



namespace cure {



// Interface towards my GAE (Google App Engine) Python 2.5 app called gamehiscore (which can be found at
// ssh://rg@pixeldoctrine.dyndns.org:2202/~/hiscore.git). The app automagically redirects from
// gamehiscore.pixeldoctrine.com to some other free hosting site in the cloud.
class HiscoreAgent {
public:
	enum Action {
		kActionNone = 1,
		kActionDownloadList,
		kActionUploadScore,
	};

	struct Entry {
		str name_;
		int score_;
	};

	struct List {
		int offset_;
		int total_count_;
		typedef std::vector<Entry> EntryList;
		EntryList entry_list_;
	};

	HiscoreAgent(const str& host, const int port, const str& game_name);
	virtual ~HiscoreAgent();
	void Close();
	ResourceLoadState Poll();

	ResourceLoadState GetLoadState() const;
	Action GetAction() const;
	void SetLoadState(ResourceLoadState load_state);
	bool StartDownloadingList(const str& platform, const str& level, const str& avatar, int offset, int limit);
	const List& GetDownloadedList() const;
	bool StartUploadingScore(const str& platform, const str& level, const str& avatar, const str& name, int score);
	int GetUploadedPlace() const;

	bool ParseList(str& data);	// For testing 'JSON parsing'...
	bool ParseScore(str& data);	// For testing 'JSON parsing'...

private:
	void Reopen();
	void AppendData(const str& data);
	bool CompleteList();
	bool CompleteScore();
	static void OnData(const happyhttp::Response* response, void* user_data, const unsigned char* data, int byte_count);
	static void OnListComplete(const happyhttp::Response* response, void* user_data);
	static void OnScoreComplete(const happyhttp::Response* response, void* user_data);
	str Hypnotize(const str& platform, const str& level, const str& avatar, const str& name, int score, int time_stamp);	// A sorta shuffle
	void DownloadThreadEntry();
	void UploadThreadEntry();

	const str server_host_;
	const int server_port_;
	happyhttp::Connection* connection_;
	const str game_name_;
	ResourceLoadState load_state_;
	List downloaded_list_;
	int uploaded_place_;
	str response_data_;
	Action action_;
	MemberThread<HiscoreAgent> connector_thread_;
	str connector_path_;
	str connector_body_;
	str connector_hash_;

	void operator=(const HiscoreAgent&);

	logclass();
};



}
