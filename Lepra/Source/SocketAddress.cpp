
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../Include/SocketAddress.h"
#include "../Include/Endian.h"
#include "../Include/Network.h"
#include "../Include/SocketAddressGetter.h"
#include "../Include/StringUtility.h"



namespace Lepra
{



SocketAddress::SocketAddress()
{
	memset(&mSockAddr, 0, sizeof(mSockAddr));
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	lSockAddr->sin_family = AF_INET;
#ifdef LEPRA_MAC
	lSockAddr->sin_len = sizeof(RawSocketAddress);
#endif // Macintosh
}

#ifdef LEPRA_NETWORK_IPV6

SocketAddress::SocketAddress(const IPAddress& pIP, uint16 pPort)
{
	memset(&mSockAddr, 0, sizeof(mSockAddr));
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	lSockAddr->sin6_family = AF_INET6;
	lSockAddr->sin6_port = Endian::HostToBig(pPort);
	pIP.Get((uint8*)&lSockAddr->sin6_addr);
}

bool SocketAddress::operator==(const SocketAddress pAddr) const
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	return ((unsigned*)(&pAddr.lSockAddr->sin6_addr))[0] == 
			((unsigned*)(&lSockAddr->sin6_addr))[0] &&
			((unsigned*)(&pAddr.lSockAddr->sin6_addr))[1] == ;
			((unsigned*)(&lSockAddr->sin6_addr))[1] &&
			((unsigned*)(&pAddr.lSockAddr->sin6_addr))[2] == ;
			((unsigned*)(&lSockAddr->sin6_addr))[2] &&
			((unsigned*)(&pAddr.lSockAddr->sin6_addr))[3] == ;
			((unsigned*)(&lSockAddr->sin6_addr))[3] &&
			lSockAddr->sin6_port == pAddr.lSockAddr->sin6_port;
}

size_t SocketAddress::operator() (const SocketAddress& pAddr) const
{
	size_t lHash = 0;
	RawSocketAddress* lSockAddr = (RawSocketAddress*)pAddr.mSockAddr;
	for (int x = 0; x < sizeof(lSockAddr->sin6_addr); ++x)
	{
		lHash += lSockAddr->sin6_addr[x];	// TODO: don't hash byte by byte, instead do size_t by size_t.
	}
	lHash += pAddr.lSockAddr->sin6_port;
	return lHash;
}

void SocketAddress::SetPort(uint16 pPort)
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	lSockAddr->sin6_port = Endian::HostToBig(pPort);
}

uint16 SocketAddress::GetPort() const
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	return Endian::BigToHost(lSockAddr->sin6_port);
}

void SocketAddress::SetIP(const IPAddress& pIP)
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	pIP.Get((uint8*)&lSockAddr->sin6_addr);
}

IPAddress SocketAddress::GetIP() const
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	return IPAddress((const uint8*)&lSockAddr->sin6_addr, 16);
}

#else // !LEPRA_NETWORK_IPV6

SocketAddress::SocketAddress(const IPAddress& pIP, uint16 pPort)
{
	memset(&mSockAddr, 0, sizeof(mSockAddr));
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
#ifdef LEPRA_MAC
	lSockAddr->sin_len = sizeof(RawSocketAddress);
#endif // Macintosh
	lSockAddr->sin_family = AF_INET;
	lSockAddr->sin_port = Endian::HostToBig(pPort);
	pIP.Get((uint8*)&lSockAddr->sin_addr);
}

bool SocketAddress::operator==(const SocketAddress& pAddr) const
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	RawSocketAddress* lOtherSockAddr = (RawSocketAddress*)pAddr.mSockAddr;
	return (lOtherSockAddr->sin_addr.s_addr == lSockAddr->sin_addr.s_addr &&
		lOtherSockAddr->sin_port == lSockAddr->sin_port);
}

size_t SocketAddress::operator() (const SocketAddress& pAddr) const
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)pAddr.mSockAddr;
	return (lSockAddr->sin_addr.s_addr + lSockAddr->sin_port);
}

void SocketAddress::SetPort(uint16 pPort)
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	lSockAddr->sin_port = Endian::HostToBig(pPort);
}

uint16 SocketAddress::GetPort() const
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	return Endian::BigToHost(lSockAddr->sin_port);
}

void SocketAddress::SetIP(const IPAddress& pIP)
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	pIP.Get((uint8*)&lSockAddr->sin_addr);
}

IPAddress SocketAddress::GetIP() const
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	return IPAddress((const uint8*)&lSockAddr->sin_addr, 4);
}

#endif // LEPRA_NETWORK_IPV6/!LEPRA_NETWORK_IPV6

void SocketAddress::Set(const IPAddress& pIP, uint16 pPort)
{
	RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
	lSockAddr->sin_family = AF_INET;
	SetIP(pIP);
	SetPort(pPort);
}

bool SocketAddress::operator!=(const SocketAddress& pAddr) const
{
	return (!(*this == pAddr));
}

void SocketAddress::operator=(const SocketAddress& pAddr)
{
	::memcpy(&mSockAddr, &pAddr.mSockAddr, sizeof(mSockAddr));
}

str SocketAddress::GetAsString() const
{
	str s = GetIP().GetAsString();
	s += strutil::Format(_T(":%i"), GetPort());
	return (s);
}

void* SocketAddress::GetRawData() const
{
	return (void*)mSockAddr;
}

bool SocketAddress::Resolve(const str& pAddress)
{
	bool lOk = true;
	strutil::strvec lVector;
	int lPort = 0;
	if (lOk)
	{
		lVector = strutil::Split(pAddress, _T(":"));
		lOk = (lVector.size() == 2);
		if (lOk)
		{
			lOk = (lVector[1].length() > 0 &&
				strutil::StringToInt(lVector[1], lPort));
			if (!lOk && lPort > 1024)
			{
				lOk = true;
			}
		}
	}
	if (lOk)
	{
		lOk = (lPort >= 0 && lPort <= 65535);
	}
	if (lOk)
	{
		lOk = ResolveHost(lVector[0]);
	}
	if (lOk)
	{
		SetPort((uint16)lPort);
	}
	return (lOk);
}

bool SocketAddress::ResolveRange(const str& pAddress, uint16& pEndPort)
{
	strutil::strvec lLocalAddressComponents = strutil::Split(pAddress, _T("-"));
	bool lOk = (lLocalAddressComponents.size() >= 1 && lLocalAddressComponents.size() <= 2);
	if (lOk)
	{
		lOk = Resolve(lLocalAddressComponents[0]);
	}

	if (lOk)
	{
		pEndPort = GetPort();
		if (lLocalAddressComponents.size() == 2)
		{
			int lEndPortInt;
			lOk = (strutil::StringToInt(lLocalAddressComponents[1], lEndPortInt) && lEndPortInt >= 0);
			if (lOk)
			{
				pEndPort = (uint16)lEndPortInt;
			}
		}
	}
	return (lOk);
}

bool SocketAddress::ResolveHost(const str& pHostname)
{
#ifdef LEPRA_NETWORK_IPV6
	// TODO: fixme!
	unsigned lIp;
#else // IPV4
	unsigned lIp;
#endif // IPv6/IPV4
	IPAddress lIpAddress;
	bool lOk = false;
	if (Network::ResolveHostname(pHostname, lIpAddress))
	{
		lOk = true;
		SetIP(lIpAddress);
	}
#ifdef LEPRA_NETWORK_IPV6
	// TODO: fixme!
	else if ((lIp = inet_addr(astrutil::Encode(pHostname).c_str())) != 0xFFFFFFFF)
#else // IPV4
	else if ((lIp = inet_addr(astrutil::Encode(pHostname).c_str())) != 0xFFFFFFFF)
#endif // IPv6/IPV4
	{
		lOk = true;
		RawSocketAddress* lSockAddr = (RawSocketAddress*)mSockAddr;
		lSockAddr->sin_addr.s_addr = lIp;
	}
	return (lOk);
}



}
