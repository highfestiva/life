/*
	Class:  SocketAddress
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	A network socket address representation.

	The main reason that this class is implemented is the operator == and the
	size_t operator(). They are both required in order to be able to put
	socket addresses in a HashTable.
*/

#ifndef LEPRA_SOCKADDR_H
#define LEPRA_SOCKADDR_H

#include "IPAddress.h"
#include "LepraTypes.h"
#include "String.h"

namespace Lepra
{

class SocketAddress
{
public:

	SocketAddress();

	#ifdef LEPRA_NETWORK_IPV6
		SocketAddress(sockaddr_in6 pSockAddr);
		const sockaddr_in6& GetAddr() const;
	#else // IPv4
		SocketAddress(sockaddr_in pSockAddr);
		const sockaddr_in& GetAddr() const;
	#endif // IPv6/IPV4

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

	String GetAsString() const;

	bool Resolve(const String& pAddress);
	bool ResolveRange(const String& pAddress, uint16& pEndPort);

protected:
	bool ResolveIp(const String& pIp);

private:
	#ifdef LEPRA_NETWORK_IPV6
		sockaddr_in6 mSockAddr;
	#else // IPV4
		sockaddr_in mSockAddr;
	#endif // IPv6/IPV4
};

}

#endif // LEPRA_SOCKADDR_H
