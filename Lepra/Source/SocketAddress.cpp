
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#include "../Include/Endian.h"
#include "../Include/Network.h"
#include "../Include/SocketAddress.h"
#include "../Include/StringUtility.h"

namespace Lepra
{

SocketAddress::SocketAddress()
{
	memset(&mSockAddr, 0, sizeof(mSockAddr));
	mSockAddr.sin_family = AF_INET;
#ifdef LEPRA_MAC
	mSockAddr.sin_len = sizeof(sockaddr_in);
#endif // Macintosh
}

#ifdef LEPRA_NETWORK_IPV6

SocketAddress::SocketAddress(sockaddr_in6 pSockAddr) :
	mSockAddr(pSockAddr)
{
}

SocketAddress::SocketAddress(const IPAddress& pIP, uint16 pPort)
{
	memset(&mSockAddr, 0, sizeof(mSockAddr));
	mSockAddr.sin6_family = AF_INET6;
	mSockAddr.sin6_port = Endian::HostToBig(pPort);
	pIP.Get((uint8*)&mSockAddr.sin6_addr);
}

const sockaddr_in6& SocketAddress::GetAddr() const
{
	return mSockAddr;
}

bool SocketAddress::operator==(const SocketAddress pAddr) const
{
	return ((unsigned*)(&pAddr.mSockAddr.sin6_addr))[0] == 
			((unsigned*)(&mSockAddr.sin6_addr))[0] &&
			((unsigned*)(&pAddr.mSockAddr.sin6_addr))[1] == ;
			((unsigned*)(&mSockAddr.sin6_addr))[1] &&
			((unsigned*)(&pAddr.mSockAddr.sin6_addr))[2] == ;
			((unsigned*)(&mSockAddr.sin6_addr))[2] &&
			((unsigned*)(&pAddr.mSockAddr.sin6_addr))[3] == ;
			((unsigned*)(&mSockAddr.sin6_addr))[3] &&
			mSockAddr.sin6_port == pAddr.mSockAddr.sin6_port;
}

size_t SocketAddress::operator() (const SocketAddress& pAddr) const
{
	size_t lHash = 0;
	const size_t* lAddress = (const size_t*)&pAddr.mSockAddr.sin6_addr;
	for (lAddress < sizeof(pAddr.mSockAddr.sin6_addr); ++lAddress)
	{
		lHash += *lAddress;
	}
	lHash += pAddr.mSockAddr.sin6_port;
	return (lHash);
}

void SocketAddress::SetPort(uint16 pPort)
{
	mSockAddr.sin6_port = Endian::HostToBig(pPort);
}

uint16 SocketAddress::GetPort() const
{
	return Endian::BigToHost(mSockAddr.sin6_port);
}

void SocketAddress::SetIP(const IPAddress& pIP)
{
	pIP.Get((uint8*)&mSockAddr.sin6_addr);
}

IPAddress SocketAddress::GetIP() const
{
	return IPAddress((const uint8*)&pAddr.mSockAddr.sin6_addr, 16);
}

#else // !LEPRA_NETWORK_IPV6

SocketAddress::SocketAddress(sockaddr_in pSockAddr) :
	mSockAddr(pSockAddr)
{
}

SocketAddress::SocketAddress(const IPAddress& pIP, uint16 pPort)
{
	memset(&mSockAddr, 0, sizeof(mSockAddr));
#ifdef LEPRA_MAC
	mSockAddr.sin_len = sizeof(sockaddr_in);
#endif // Macintosh
	mSockAddr.sin_family = AF_INET;
	mSockAddr.sin_port = Endian::HostToBig(pPort);
	pIP.Get((uint8*)&mSockAddr.sin_addr);
}

const sockaddr_in& SocketAddress::GetAddr() const
{
	return mSockAddr;
}

bool SocketAddress::operator==(const SocketAddress& pAddr) const
{
	return (pAddr.mSockAddr.sin_addr.s_addr == mSockAddr.sin_addr.s_addr &&
		pAddr.mSockAddr.sin_port == mSockAddr.sin_port);
}

size_t SocketAddress::operator() (const SocketAddress& pAddr) const
{
	return (pAddr.mSockAddr.sin_addr.s_addr + pAddr.mSockAddr.sin_port);
}

void SocketAddress::SetPort(uint16 pPort)
{
	mSockAddr.sin_port = Endian::HostToBig(pPort);
}

uint16 SocketAddress::GetPort() const
{
	return Endian::BigToHost(mSockAddr.sin_port);
}

void SocketAddress::SetIP(const IPAddress& pIP)
{
	pIP.Get((uint8*)&mSockAddr.sin_addr);
}

IPAddress SocketAddress::GetIP() const
{
	return IPAddress((const uint8*)&mSockAddr.sin_addr, 4);
}

#endif // LEPRA_NETWORK_IPV6/!LEPRA_NETWORK_IPV6

void SocketAddress::Set(const IPAddress& pIP, uint16 pPort)
{
	mSockAddr.sin_family = AF_INET;
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

bool SocketAddress::Resolve(const str& pAddress)
{
	bool lOk = true;
	strutil::strvec lVector;
	int lPort = 0;
	if (lOk)
	{
		lVector = strutil::Split(pAddress, _T(":"));
		lOk = (lVector.size() == 2 &&
			lVector[1].length() > 0 &&
			strutil::StringToInt(lVector[1], lPort));
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
		lIpAddress.Get((uint8*)&mSockAddr.sin_addr);
	}
#ifdef LEPRA_NETWORK_IPV6
	// TODO: fixme!
	else if ((lIp = inet_addr(astrutil::Encode(pHostname).c_str())) != 0xFFFFFFFF)
#else // IPV4
	else if ((lIp = inet_addr(astrutil::Encode(pHostname).c_str())) != 0xFFFFFFFF)
#endif // IPv6/IPV4
	{
		lOk = true;
		mSockAddr.sin_addr.s_addr = lIp;
	}
	return (lOk);
}

}
