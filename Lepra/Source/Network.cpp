
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/lepraassert.h"
#include "../include/lepraos.h"
#include "../include/network.h"
#include "../include/socket.h"

#ifdef LEPRA_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#endif // LEPRA_WINDOWS



namespace lepra {




bool Network::Start() {
	deb_assert(!started_);
	if (started_ == false) {
#ifdef LEPRA_WINDOWS
		WSADATA socket_data;
		started_ = (::WSAStartup(MAKEWORD(2, 2), &socket_data) == 0);
#else
		started_ = true;
#endif // Windows / other.
	}
	if (!started_) {
		log_.Fatal("Could not startup networking!");
	}
	return (started_);
}

bool Network::Stop() {
	if (started_ == true) {
		started_ = false;
#ifdef LEPRA_WINDOWS
		return (::WSACleanup() == 0);
#endif // LEPRA_WINDOWS
	}
	return !started_;
}

str Network::GetHostname() {
	char name[256];
	if (::gethostname(name, sizeof(name)) == 0) {
		return (str(name));
	}
	return (str());
}

bool Network::ResolveHostname(const str& hostname, IPAddress& ip_address) {
	hostent* __hostent;
	if (hostname.length() == 0) {
		char _hostname[256] = "";
		::gethostname(_hostname, sizeof(_hostname));
		__hostent = ::gethostbyname(_hostname);
	} else {
		__hostent = ::gethostbyname(hostname.c_str());
	}

	if (__hostent) {
		if (__hostent->h_addr_list[0]) {
			ip_address.Set((const uint8*)__hostent->h_addr_list[0], __hostent->h_length);
			return true;
		}
	}
	return false;
}

bool Network::ResolveIp(const IPAddress& ip_address, str& hostname) {
	uint8 raw_address[16];
	ip_address.Get(raw_address);
	hostent* __hostent = ::gethostbyaddr((const char*)raw_address, ip_address.GetNumBytes(), AF_INET);
	if (__hostent && __hostent->h_name && __hostent->h_name[0]) {
		hostname = __hostent->h_name;
		return true;
	}
	return false;
}

bool Network::IsLocalAddress(const str& address) {
	bool is_local_address = false;
	const str url = strutil::Split(address, ":", 1)[0];
	IPAddress _ip_address;
	if (ResolveHostname(url, _ip_address)) {
		IPAddress external_ip_address;
		ResolveHostname("", external_ip_address);
		const str ip = _ip_address.GetAsString();
		if (ip == "127.0.0.1" ||
			ip == "0.0.0.0" ||
			_ip_address == external_ip_address) {
			is_local_address = true;
		}
	}
	return is_local_address;
}



bool Network::started_ = false;
loginstance(kNetwork, Network);



}
