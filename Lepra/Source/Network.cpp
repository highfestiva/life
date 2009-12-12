
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../Include/Network.h"

#ifdef LEPRA_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#endif // LEPRA_WINDOWS



namespace Lepra
{




bool Network::Start()
{
	assert(!mStarted);
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
	assert(mStarted);
	if (mStarted == true)
	{
		mStarted = false;
#ifdef LEPRA_WINDOWS
		return (::WSACleanup() == 0);
#endif // LEPRA_WINDOWS
	}
	return !mStarted;
}

String Network::GetHostname()
{
	char lName[256];
	const int lNameLength = ::gethostname(lName, sizeof(lName));
	return (AnsiStringUtility::ToCurrentCode(AnsiString(lName, lNameLength)));
}

bool Network::ResolveHostname(const String& pHostname, IPAddress& pIPAddress)
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
		lHostent = ::gethostbyname(AnsiStringUtility::ToOwnCode(pHostname).c_str());
	}

	if (lHostent != 0)
	{
		pIPAddress.Set((const uint8*)lHostent->h_addr, lHostent->h_length);
		return true;
	}
	return false;
}



bool Network::mStarted = false;
LOG_CLASS_DEFINE(NETWORK, Network);



}
