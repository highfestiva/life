
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "LepraOS.h"
#include "SocketAddress.h"

#ifdef LEPRA_NETWORK_IPV6
typedef sockaddr_in6 RawSocketAddress;
#else // IPv4
typedef sockaddr_in RawSocketAddress;
#endif // IPv6/IPV4



namespace Lepra
{



class SocketAddressGetter
{
public:
	static sockaddr* GetRaw(SocketAddress& pAddress)
	{
		return (sockaddr*)pAddress.GetRawData();
	}

	static const sockaddr* GetRaw(const SocketAddress& pAddress)
	{
		return (const sockaddr*)pAddress.GetRawData();
	}

	static const RawSocketAddress& Get(const SocketAddress& pAddress)
	{
		return *(RawSocketAddress*)pAddress.GetRawData();
	}
};



}
