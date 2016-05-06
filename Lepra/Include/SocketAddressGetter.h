
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepraos.h"
#include "socketaddress.h"

#ifdef LEPRA_NETWORK_IPV6
typedef sockaddr_in6 RawSocketAddress;
#else // IPv4
typedef sockaddr_in RawSocketAddress;
#endif // IPv6/IPV4



namespace lepra {



class SocketAddressGetter {
public:
	static sockaddr* GetRaw(SocketAddress& address) {
		return (sockaddr*)address.GetRawData();
	}

	static const sockaddr* GetRaw(const SocketAddress& address) {
		return (const sockaddr*)address.GetRawData();
	}

	static const RawSocketAddress& Get(const SocketAddress& address) {
		return *(RawSocketAddress*)address.GetRawData();
	}
};



}
