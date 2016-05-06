
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../include/socketaddress.h"
#include "../include/endian.h"
#include "../include/network.h"
#include "../include/socketaddressgetter.h"
#include "../include/stringutility.h"



namespace lepra {



SocketAddress::SocketAddress() {
	memset(&sock_addr_, 0, sizeof(sock_addr_));
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	sock_addr->sin_family = AF_INET;
#ifdef LEPRA_MAC
	sock_addr->sin_len = sizeof(RawSocketAddress);
#endif // Macintosh
}

#ifdef LEPRA_NETWORK_IPV6

SocketAddress::SocketAddress(const IPAddress& ip, uint16 port) {
	memset(&sock_addr_, 0, sizeof(sock_addr_));
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	sock_addr->sin6_family = AF_INET6;
	sock_addr->sin6_port = Endian::HostToBig(port);
	ip.Get((uint8*)&sock_addr->sin6_addr);
}

bool SocketAddress::operator==(const SocketAddress addr) const {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	return ((unsigned*)(&addr.sock_addr->sin6_addr))[0] ==
			((unsigned*)(&sock_addr->sin6_addr))[0] &&
			((unsigned*)(&addr.sock_addr->sin6_addr))[1] == ;
			((unsigned*)(&sock_addr->sin6_addr))[1] &&
			((unsigned*)(&addr.sock_addr->sin6_addr))[2] == ;
			((unsigned*)(&sock_addr->sin6_addr))[2] &&
			((unsigned*)(&addr.sock_addr->sin6_addr))[3] == ;
			((unsigned*)(&sock_addr->sin6_addr))[3] &&
			sock_addr->sin6_port == addr.sock_addr->sin6_port;
}

size_t SocketAddress::operator() (const SocketAddress& addr) const {
	size_t hash = 0;
	RawSocketAddress* sock_addr = (RawSocketAddress*)addr.sock_addr_;
	for (int x = 0; x < sizeof(sock_addr->sin6_addr); ++x) {
		hash += sock_addr->sin6_addr[x];	// TODO: don't hash byte by byte, instead do size_t by size_t.
	}
	hash += addr.sock_addr->sin6_port;
	return hash;
}

void SocketAddress::SetPort(uint16 port) {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	sock_addr->sin6_port = Endian::HostToBig(port);
}

uint16 SocketAddress::GetPort() const {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	return Endian::BigToHost(sock_addr->sin6_port);
}

void SocketAddress::SetIP(const IPAddress& ip) {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	ip.Get((uint8*)&sock_addr->sin6_addr);
}

IPAddress SocketAddress::GetIP() const {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	return IPAddress((const uint8*)&sock_addr->sin6_addr, 16);
}

#else // !LEPRA_NETWORK_IPV6

SocketAddress::SocketAddress(const IPAddress& ip, uint16 port) {
	memset(&sock_addr_, 0, sizeof(sock_addr_));
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
#ifdef LEPRA_MAC
	sock_addr->sin_len = sizeof(RawSocketAddress);
#endif // Macintosh
	sock_addr->sin_family = AF_INET;
	sock_addr->sin_port = Endian::HostToBig(port);
	ip.Get((uint8*)&sock_addr->sin_addr);
}

bool SocketAddress::operator==(const SocketAddress& addr) const {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	RawSocketAddress* other_sock_addr = (RawSocketAddress*)addr.sock_addr_;
	return (other_sock_addr->sin_addr.s_addr == sock_addr->sin_addr.s_addr &&
		other_sock_addr->sin_port == sock_addr->sin_port);
}

size_t SocketAddress::operator() (const SocketAddress& addr) const {
	RawSocketAddress* sock_addr = (RawSocketAddress*)addr.sock_addr_;
	return (sock_addr->sin_addr.s_addr + sock_addr->sin_port);
}

void SocketAddress::SetPort(uint16 port) {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	sock_addr->sin_port = Endian::HostToBig(port);
}

uint16 SocketAddress::GetPort() const {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	return Endian::BigToHost(sock_addr->sin_port);
}

void SocketAddress::SetIP(const IPAddress& ip) {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	ip.Get((uint8*)&sock_addr->sin_addr);
}

IPAddress SocketAddress::GetIP() const {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	return IPAddress((const uint8*)&sock_addr->sin_addr, 4);
}

#endif // LEPRA_NETWORK_IPV6/!LEPRA_NETWORK_IPV6

void SocketAddress::Set(const IPAddress& ip, uint16 port) {
	RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
	sock_addr->sin_family = AF_INET;
	SetIP(ip);
	SetPort(port);
}

bool SocketAddress::operator!=(const SocketAddress& addr) const {
	return (!(*this == addr));
}

void SocketAddress::operator=(const SocketAddress& addr) {
	::memcpy(&sock_addr_, &addr.sock_addr_, sizeof(sock_addr_));
}

str SocketAddress::GetAsString() const {
	str s = GetIP().GetAsString();
	s += strutil::Format(":%i", GetPort());
	return (s);
}

void* SocketAddress::GetRawData() const {
	return (void*)sock_addr_;
}

bool SocketAddress::Resolve(const str& address) {
	bool ok = true;
	strutil::strvec vector;
	int _port = 0;
	if (ok) {
		vector = strutil::Split(address, ":");
		ok = (vector.size() == 2);
		if (ok) {
			ok = (vector[1].length() > 0 &&
				strutil::StringToInt(vector[1], _port));
			if (!ok && _port > 1024) {
				ok = true;
			}
		}
	}
	if (ok) {
		ok = (_port >= 0 && _port <= 65535);
	}
	if (ok) {
		ok = ResolveHost(vector[0]);
	}
	if (ok) {
		SetPort((uint16)_port);
	}
	return (ok);
}

bool SocketAddress::ResolveRange(const str& address, uint16& end_port) {
	strutil::strvec local_address_components = strutil::Split(address, "-");
	bool ok = (local_address_components.size() >= 1 && local_address_components.size() <= 2);
	if (ok) {
		ok = Resolve(local_address_components[0]);
	}

	if (ok) {
		end_port = GetPort();
		if (local_address_components.size() == 2) {
			int end_port_int;
			ok = (strutil::StringToInt(local_address_components[1], end_port_int) && end_port_int >= 0);
			if (ok) {
				end_port = (uint16)end_port_int;
			}
		}
	}
	return (ok);
}

bool SocketAddress::ResolveHost(const str& hostname) {
#ifdef LEPRA_NETWORK_IPV6
	// TODO: fixme!
	unsigned ip;
#else // IPV4
	unsigned ip;
#endif // IPv6/IPV4
	IPAddress ip_address;
	bool ok = false;
	if (Network::ResolveHostname(hostname, ip_address)) {
		ok = true;
		SetIP(ip_address);
	}
#ifdef LEPRA_NETWORK_IPV6
	// TODO: fixme!
	else if ((ip = inet_addr(hostname.c_str())) != 0xFFFFFFFF)
#else // IPV4
	else if ((ip = inet_addr(hostname.c_str())) != 0xFFFFFFFF)
#endif // IPv6/IPV4
	{
		ok = true;
		RawSocketAddress* sock_addr = (RawSocketAddress*)sock_addr_;
		sock_addr->sin_addr.s_addr = ip;
	}
	return (ok);
}

bool SocketAddress::ResolveIpToHostname(str& hostname) const {
	return Network::ResolveIp(GetIP(), hostname);
}



}
