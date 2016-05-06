
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "ipaddress.h"
#include "string.h"



namespace lepra {



class SocketAddress {
public:

	SocketAddress();
	SocketAddress(const IPAddress& ip, uint16 port);

	void Set(const IPAddress& ip, uint16 port);
	void SetPort(uint16 port);
	uint16 GetPort() const;
	void SetIP(const IPAddress& ip);
	IPAddress GetIP() const;
	size_t operator() (const SocketAddress& socket_address) const;
	bool operator==(const SocketAddress& addr) const;
	bool operator!=(const SocketAddress& addr) const;
	void operator=(const SocketAddress& addr);

	str GetAsString() const;
	void* GetRawData() const;

	bool Resolve(const str& address);
	bool ResolveRange(const str& address, uint16& end_port);
	bool ResolveHost(const str& hostname);	// Only changes address, not port.
	bool ResolveIpToHostname(str& hostname) const;

private:
	uint8 sock_addr_[64];
};



}
