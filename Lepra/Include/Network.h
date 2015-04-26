
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "IPAddress.h"
#include "Log.h"



namespace Lepra
{



class Network
{
public:
	static bool Start();
	static bool Stop();
	static str GetHostname();
	static bool ResolveHostname(const str& pHostname, IPAddress& pIPAddress);
	static bool ResolveIp(const IPAddress& pIpAddress, str& pHostname);
	static bool IsLocalAddress(const str& pAddress);

private:
	static bool mStarted;
	logclass();
};



}
