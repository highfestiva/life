
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "IPAddress.h"
#include "String.h"



namespace Lepra
{



class SocketAddress
{
public:

	SocketAddress();
	SocketAddress(const IPAddress& pIP, uint16 pPort);

	void Set(const IPAddress& pIP, uint16 pPort);
	void SetPort(uint16 pPort);
	uint16 GetPort() const;
	void SetIP(const IPAddress& pIP);
	IPAddress GetIP() const;
	size_t operator() (const SocketAddress& pSocketAddress) const;
	bool operator==(const SocketAddress& pAddr) const;
	bool operator!=(const SocketAddress& pAddr) const;
	void operator=(const SocketAddress& pAddr);

	str GetAsString() const;
	void* GetRawData() const;

	bool Resolve(const str& pAddress);
	bool ResolveRange(const str& pAddress, uint16& pEndPort);
	bool ResolveHost(const str& pHostname);	// Only changes address, not port.

private:
	uint8 mSockAddr[64];
};



}
