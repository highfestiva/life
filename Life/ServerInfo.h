
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <vector>
#include "life.h"



namespace life {



struct ServerInfo {
	str command_;
	str name_;
	str id_;
	str given_ip_address_;
	str internal_ip_address_;
	str remote_ip_address_;
	int given_port_;
	int internal_port_;
	int remote_port_;
	int player_count_;
	bool remove_;
	double ping_;

	ServerInfo():
		given_port_(-1),
		internal_port_(-1),
		remote_port_(-1),
		player_count_(-1),
		remove_(false),
		ping_(-1) {
	}

	bool operator!=(const ServerInfo& other) {
		return command_ != other.command_ ||
			name_ != other.name_ ||
			id_ != other.id_ ||
			given_ip_address_ != other.given_ip_address_ ||
			remote_ip_address_ != other.remote_ip_address_ ||
			given_port_ != other.given_port_ ||
			remote_port_ != other.remote_port_ ||
			player_count_ != other.player_count_ ||
			remove_ != other.remove_ ||
			ping_ != other.ping_;
	};
};

typedef std::vector<ServerInfo> ServerInfoList;



}
