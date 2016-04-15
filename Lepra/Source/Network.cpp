
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/LepraAssert.h"
#include "../Include/LepraOS.h"
#include "../Include/Network.h"
#include "../Include/Socket.h"

#ifdef LEPRA_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#endif // LEPRA_WINDOWS



namespace Lepra
{




bool Network::Start()
{
	deb_assert(!mStarted);
	if (mStarted == false)
	{
#ifdef LEPRA_WINDOWS
		WSADATA lSocketData;
		mStarted = (::WSAStartup(MAKEWORD(2, 2), &lSocketData) == 0);
#else
		mStarted = true;
#endif // Windows / other.
	}
	if (!mStarted)
	{
		mLog.Fatal("Could not startup networking!");
	}
	return (mStarted);
}

bool Network::Stop()
{
	if (mStarted == true)
	{
		mStarted = false;
#ifdef LEPRA_WINDOWS
		return (::WSACleanup() == 0);
#endif // LEPRA_WINDOWS
	}
	return !mStarted;
}

str Network::GetHostname()
{
	char lName[256];
	if (::gethostname(lName, sizeof(lName)) == 0)
	{
		return (str(lName));
	}
	return (str());
}

bool Network::ResolveHostname(const str& pHostname, IPAddress& pIPAddress)
{
	hostent* lHostent;
	if (pHostname.length() == 0)
	{
		char lHostname[256] = "";
		::gethostname(lHostname, sizeof(lHostname));
		lHostent = ::gethostbyname(lHostname);
	}
	else
	{
		lHostent = ::gethostbyname(pHostname.c_str());
	}

	if (lHostent)
	{
		if (lHostent->h_addr_list[0])
		{
			pIPAddress.Set((const uint8*)lHostent->h_addr_list[0], lHostent->h_length);
			return true;
		}
	}
	return false;
}

bool Network::ResolveIp(const IPAddress& pIpAddress, str& pHostname)
{
	uint8 lRawAddress[16];
	pIpAddress.Get(lRawAddress);
	hostent* lHostent = ::gethostbyaddr((const char*)lRawAddress, pIpAddress.GetNumBytes(), AF_INET);
	if (lHostent && lHostent->h_name && lHostent->h_name[0])
	{
		pHostname = lHostent->h_name;
		return true;
	}
	return false;
}

bool Network::IsLocalAddress(const str& pAddress)
{
	bool lIsLocalAddress = false;
	const str lUrl = strutil::Split(pAddress, ":", 1)[0];
	IPAddress lIpAddress;
	if (ResolveHostname(lUrl, lIpAddress))
	{
		IPAddress lExternalIpAddress;
		ResolveHostname("", lExternalIpAddress);
		const str lIp = lIpAddress.GetAsString();
		if (lIp == "127.0.0.1" ||
			lIp == "0.0.0.0" ||
			lIpAddress == lExternalIpAddress)
		{
			lIsLocalAddress = true;
		}
	}
	return lIsLocalAddress;
}



bool Network::mStarted = false;
loginstance(NETWORK, Network);



}
