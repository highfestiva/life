
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/hiscoreagent.h"
#include "../../lepra/include/lepraassert.h"
#include <happyhttp.h>
#include "../../lepra/include/obfuxator.h"
#include "../../lepra/include/jsonstring.h"
#include "../../lepra/include/sha1.h"
#include "../../tbc/include/chunkybonegeometry.h"



namespace cure {



HiscoreAgent::HiscoreAgent(const str& host, const int port, const str& game_name):
	server_host_(host),
	server_port_(port),
	connection_(0),
	game_name_(game_name),
	action_(kActionNone),
	connector_thread_("sxnt") {
	Close();
}

HiscoreAgent::~HiscoreAgent() {
	Close();
}

void HiscoreAgent::Close() {
	log_debug("Closing and resetting state.");
	delete connection_;
	connection_ = 0;

	load_state_ = kResourceUnloaded;
	downloaded_list_.offset_ = -1;
	downloaded_list_.total_count_ = -1;
	downloaded_list_.entry_list_.clear();
	uploaded_place_ = -1;
	response_data_.clear();
}

ResourceLoadState HiscoreAgent::Poll() {
	if (connector_thread_.IsRunning()) {
		// Still connecting.
		return kResourceLoadInProgress;
	}
	if (!connection_) {
		// Connector thread had some problem.
		return kResourceLoadError;
	}

	log_debug("Polling connection.");
	if (GetLoadState() == kResourceLoadInProgress && connection_->outstanding()) {
		try {
			connection_->pump();
		} catch (happyhttp::Wobbly& e) {
			(void)e;
			log_volatile(log_.Warning(str("Problem polling connection: ") + e.what()));
			SetLoadState(kResourceLoadError);
		}
	}
	if (GetLoadState() == kResourceLoadError && connection_) {
		// Bail if something trashed us earlier.
		Close();
		SetLoadState(kResourceLoadError);	// Reset load state after close.
	}
	return GetLoadState();
}



ResourceLoadState HiscoreAgent::GetLoadState() const {
	return load_state_;
}

HiscoreAgent::Action HiscoreAgent::GetAction() const {
	return action_;
}

void HiscoreAgent::SetLoadState(ResourceLoadState load_state) {
	load_state_ = load_state;
}

bool HiscoreAgent::StartDownloadingList(const str& platform, const str& level, const str& avatar, int offset, int limit) {
	if (!connector_thread_.Join(0.1)) {
		deb_assert(false);
		return false;
	}
	const str format = _O("o@y+_.R2=*8/,1ay+x2&9(92Qay+x=(=*=,ayL+x/8e8+9*ay5x2515&*ay5g", "/%s?platform=%s&level=%s&avatar=%s&offset=%i&limit=%i");
	connector_path_ = strutil::Format(format.c_str(),
		game_name_.c_str(), platform.c_str(), level.c_str(), avatar.c_str(), offset, limit);
	return connector_thread_.Start(this, &HiscoreAgent::DownloadThreadEntry);
}

const HiscoreAgent::List& HiscoreAgent::GetDownloadedList() const {
	return downloaded_list_;
}

bool HiscoreAgent::StartUploadingScore(const str& platform, const str& level, const str& avatar, const str& name, int score) {
	if (!connector_thread_.Join(0.1)) {
		deb_assert(false);
		return false;
	}
	str json_name = JsonString::ToJson(name);
	if (json_name.empty()) {
		deb_assert(false);
		return false;
	}
	json_name = json_name.substr(1, json_name.length()-2);
	const int _time_stamp = (int32)::time(0);
	connector_hash_ = Hypnotize(platform, level, avatar, json_name, score, _time_stamp);
	const str format = _O(".}2=*8?/,1ay+x29(92-ay+x(=(=*=,ay+x0=,19ayw+x+;/,9ay5x* 519apy5", "platform=%s&level=%s&avatar=%s&name=%s&score=%i&time=%i");
	json_name = JsonString::UrlEncode(json_name);
	connector_body_ = strutil::Format(format.c_str(),
		platform.c_str(), level.c_str(), avatar.c_str(), json_name.c_str(), score, _time_stamp);
	connector_path_ = _O("oe=::?`90*,%o", "/add_entry/") + game_name_;
	return connector_thread_.Start(this, &HiscoreAgent::UploadThreadEntry);
}

int HiscoreAgent::GetUploadedPlace() const {
	return uploaded_place_;
}



bool HiscoreAgent::ParseList(str& data) {
	// Well, hrm... parse the JSON. Quick and dirty was the theme of the day.
	load_state_ = kResourceLoadError;
	log_debug("Parsing downloaded list: " + data);
	if (data.length() < 10 || data[0] != '{' || data[data.length()-1] != '}') {
		log_volatile(log_.Warning("Problem parsing list, not our JSON: " + data));
		return false;
	}
	data.resize(data.length()-1);
	str _data = data.substr(1);
	size_t p;
	if ((p = _data.find("[]")) != str::npos) {
		// Check for [] (empty list) and add a space for lousy parsing.
		_data.replace(p, 2, "[ ]", 3);
	}
	str list;
	typedef strutil::strvec sv;
	sv strings = strutil::BlockSplit(_data, "[]", true, true);
	str other;
	for (sv::iterator x = strings.begin(); x != strings.end(); ++x) {
		other += *x;
		size_t y = strutil::FindPreviousWord(*x, "\": ,", x->length()-1);
		if (x->substr(y, 4) == "list") {
			++x;
			list = *x;
		}
	}
	if (other.empty() || list.empty()) {
		log_volatile(log_.Warning("Problem parsing list; list or other data missing: " + _data));
		return false;
	}
	strings = strutil::BlockSplit(other, ",", true, true);
	for (sv::iterator x = strings.begin(); x != strings.end(); ++x) {
		sv tag_value = strutil::BlockSplit(*x, ": \t\r\n", true, true);
		const size_t base = tag_value[0].empty()? 1 : 0;
		if (tag_value.size() < base+2) {
			continue;
		}
		if (tag_value[base+0] == "\"offset\"") {
			if (!strutil::StringToInt(tag_value[base+1], downloaded_list_.offset_)) {
				log_volatile(log_.Warning("Problem parsing download list offset (int): " + tag_value[base+1]));
				return false;
			}
		} else if (tag_value[base+0] == "\"total_count\"") {
			if (!strutil::StringToInt(tag_value[base+1], downloaded_list_.total_count_)) {
				log_volatile(log_.Warning("Problem parsing download list total_count (int): " + tag_value[base+1]));
				return false;
			}
		}
	}
	Entry entry;
	int flags = 0;
	strings = strutil::BlockSplit(list, "{},", true, true);
	for (sv::iterator x = strings.begin(); x != strings.end(); ++x) {
		sv tag_value = strutil::BlockSplit(*x, ": \t\r\n", true, true);
		if (tag_value.size() < 2) {
			continue;
		}
		const size_t base = tag_value[0].empty()? 1 : 0;
		if (tag_value[base+0] == "\"name\"") {
			if (tag_value[base+1].empty()) {
				log_volatile(log_.Warning("Problem parsing download list name (empty)."));
				return false;
			}
			entry.name_ = JsonString::FromJson(tag_value[base+1]);
			flags |= 1;
		} else if (tag_value[base+0] == "\"score\"") {
			if (!strutil::StringToInt(tag_value[base+1], entry.score_)) {
				log_volatile(log_.Warning("Problem parsing download list score (int): " + tag_value[base+1]));
				return false;
			}
			flags |= 2;
		}
		if ((flags&3) == 3) {
			downloaded_list_.entry_list_.push_back(entry);
			flags = 0;
		}
	}
	if (downloaded_list_.offset_ < 0 ||
		downloaded_list_.total_count_ < 0 ||
		(int)downloaded_list_.entry_list_.size() > downloaded_list_.offset_+downloaded_list_.total_count_) {
		log_volatile(log_.Warning("Problem parsing hiscore list data: " + _data));
		return false;
	}
	load_state_ = kResourceLoadComplete;
	log_volatile(log_.Info("Hiscore list data parsed OK."));
	return true;
}

bool HiscoreAgent::ParseScore(str& data) {
	load_state_ = kResourceLoadError;
	log_debug("Parsing uploaded score: " + data);
	if (data.length() < 10 || data[0] != '{' || data[data.length()-1] != '}') {
		log_volatile(log_.Warning("Problem parsing uploaded score, not our JSON: " + data));
		return false;
	}
	data.resize(data.length()-1);
	str _data = data.substr(1);
	typedef strutil::strvec sv;
	sv strings = strutil::BlockSplit(_data, ",", true, true);
	for (sv::iterator x = strings.begin(); x != strings.end(); ++x) {
		sv tag_value = strutil::BlockSplit(*x, ": \t\r\n", true, true);
		const size_t base = tag_value[0].empty()? 1 : 0;
		if (tag_value.size() < 2) {
			log_volatile(log_.Warning("Problem parsing tags of uploaded score: " + _data));
			return false;
		}
		if (tag_value[base+0] == "\"offset\"") {
			if (!strutil::StringToInt(tag_value[base+1], uploaded_place_)) {
				log_volatile(log_.Warning("Problem parsing uploaded score offset (int): " + tag_value[base+1]));
				return false;
			}
		}
	}
	if (uploaded_place_ < 0) {
		log_volatile(log_.Warning("Problem parsing own placement when uploaded score: " + _data));
		return false;
	}
	load_state_ = kResourceLoadComplete;
	log_volatile(log_.Info("Hiscore list data parsed OK."));
	return true;
}



void HiscoreAgent::Reopen() {
	Close();
	log_debug("Reopening connection.");
	connection_ = new happyhttp::Connection(server_host_.c_str(), server_port_);
	load_state_ = kResourceLoadInProgress;
}

void HiscoreAgent::AppendData(const str& data) {
	response_data_ += data;
}

bool HiscoreAgent::CompleteList() {
	return ParseList(response_data_);
}

bool HiscoreAgent::CompleteScore() {
	// Well, hrm... parse the JSON. Quick and dirty was the theme of the day.
	return ParseScore(response_data_);
}

void HiscoreAgent::OnData(const happyhttp::Response* response, void* user_data, const unsigned char* data, int byte_count) {
	HiscoreAgent* value = (HiscoreAgent*)user_data;
	if (response->getstatus() != 200) {
		log_volatile(log_.Warningf("HTTP error %i when receiving data.", response->getstatus()));
		value->SetLoadState(kResourceLoadError);
		return;
	}
	value->AppendData(str((const char*)data, byte_count));
}

void HiscoreAgent::OnListComplete(const happyhttp::Response* response, void* user_data) {
	HiscoreAgent* value = (HiscoreAgent*)user_data;
	if (response->getstatus() != 200) {
		log_volatile(log_.Warningf("HTTP error %i when completing download list data reception.", response->getstatus()));
		value->SetLoadState(kResourceLoadError);
		return;
	}
	value->CompleteList();
}

void HiscoreAgent::OnScoreComplete(const happyhttp::Response* response, void* user_data) {
	HiscoreAgent* value = (HiscoreAgent*)user_data;
	if (response->getstatus() != 200) {
		log_volatile(log_.Warningf("HTTP error %i when completing upload score data reception.", response->getstatus()));
		value->SetLoadState(kResourceLoadError);
		return;
	}
	value->CompleteScore();
}

str HiscoreAgent::Hypnotize(const str& platform, const str& level, const str& avatar, const str& name, int score, int time_stamp) {
	const str format = _O("vF|}XV^^y5y+ y+?y+?Uy+??ry+qy5q={9w1:Ci7", "(\"!FH@%i%s~%s_%s_%s__%s-%i-a#e'md5g");
	str origin = strutil::Format(format.c_str(),
		time_stamp+1, game_name_.c_str(), platform.c_str(),
		level.c_str(), avatar.c_str(), name.c_str(), score-1);
	log_.Infof("data = %s", origin.c_str());
	str utf_string = origin;
	//::memset((void*)origin.c_str(), 0, origin.size()*sizeof(char));
	uint8 sha1_hash[20];
	SHA1::Hash((const uint8*)utf_string.c_str(), utf_string.length(), sha1_hash);
	//::memset((void*)utf_string.c_str(), 0, utf_string.length());
	str input_hexdigest = strutil::DumpData(sha1_hash, sizeof(sha1_hash));
	strutil::ToLower(input_hexdigest);
	//log_.Infof("sha1 = %s", input_hexdigest.c_str());

	// Shuffle:
	//input_hexdigest = list(input_hexdigest)
	int i = 0;
	int j = input_hexdigest.length()-2;
	int chk = (int)'B';
	while (i < j) {
		std::swap(input_hexdigest[i], input_hexdigest[j]);
		chk ^= (int)input_hexdigest[i] + j;
		i += 3;
		j -= 5;
	}
	chk = (chk % 25) + (int)'a';
	input_hexdigest = input_hexdigest.substr(0, 15) + (char)chk + input_hexdigest.substr(15) + (char)(chk+1);
	int ints[] = { 0, 0, 0, 0, 0, 0, 0 };
	for (int x = 0; x < 6; ++x) {
		for (int y = 0; y < 7; ++y) {
			ints[y] += (int)input_hexdigest[x*7+y] - 73 - y - x*5;
		}
	}
	str shuffled_hash;
	for (int y = 0; y < 7; ++y) {
		shuffled_hash += strutil::IntToString(::abs(ints[y]), (y&1)? 9 : 10);
	}
	log_.Infof("output hash = %s", shuffled_hash.c_str());
	return shuffled_hash;
}

void HiscoreAgent::DownloadThreadEntry() {
	try {
		action_ = kActionDownloadList;
		Reopen();
		connection_->setcallbacks(0, &HiscoreAgent::OnData, &HiscoreAgent::OnListComplete, this);
		str method = _OA("WzYJ", "GET");
		str a_path = connector_path_;
		connection_->request(method.c_str(), a_path.c_str());
		log_volatile(log_.Info("Downloading highscore list."));
	} catch (happyhttp::Wobbly& e) {
		(void)e;
		log_volatile(log_.Warning(str("Problem retrieving list: ") + e.what()));
		delete connection_;
		connection_ = 0;
	}
}

void HiscoreAgent::UploadThreadEntry() {
	try {
		action_ = kActionUploadScore;
		Reopen();
		connection_->setcallbacks(0, &HiscoreAgent::OnData, &HiscoreAgent::OnScoreComplete, this);
		const str client = _OA("[\"RUYP_J", "CLIENT");
		const str hash = connector_hash_;
		const str accept = _OA("]x;;9.$*", "Accept");
		const str plain = _OA("*{9&*o&.2=50", "text/plain");
		const char* headers[] =
		{
			client.c_str(), hash.c_str(),
			accept.c_str(), plain.c_str(),
			0
		};
		const str method = _OA("NaOKJ", "POST");
		const str utf8_body = strutil::ReplaceAll(connector_body_, "\\", "%5C");
		connection_->request(method.c_str(), connector_path_.c_str(), headers, (const unsigned char*)utf8_body.c_str(), utf8_body.length());
		log_volatile(log_.Info("Uploading score."));
	} catch (happyhttp::Wobbly& e) {
		(void)e;
		log_volatile(log_.Warning(str("Problem uploading score: ") + e.what()));
		delete connection_;
		connection_ = 0;
	}
}



void HiscoreAgent::operator=(const HiscoreAgent&) {
	deb_assert(false);
}



loginstance(kNetworkClient, HiscoreAgent);



}
