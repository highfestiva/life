
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/LepraAssert.h"
#include "../Include/Network.h"

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
		mLog.AFatal("Could not startup networking!");
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
		return (strutil::Encode(astr(lName)));
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
		lHostent = ::gethostbyname(astrutil::Encode(pHostname).c_str());
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

bool Network::IsLocalAddress(const str& pAddress)
{
	bool lIsLocalAddress = false;
	const str lUrl = strutil::Split(pAddress, _T(":"), 1)[0];
	IPAddress lIpAddress;
	if (ResolveHostname(lUrl, lIpAddress))
	{
		IPAddress lExternalIpAddress;
		ResolveHostname(_T(""), lExternalIpAddress);
		const str lIp = lIpAddress.GetAsString();
		if (lIp == _T("127.0.0.1") ||
			lIp == _T("0.0.0.0") ||
			lIpAddress == lExternalIpAddress)
		{
			lIsLocalAddress = true;
		}
	}
	return lIsLocalAddress;
}



bool Network::mStarted = false;
LOG_CLASS_DEFINE(NETWORK, Network);



}
