
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "LepraTypes.h"
#include "Socket.h"



namespace Lepra
{



class Network
{
public:
	static bool Start();
	static bool Stop();
	static str GetHostname();
	static bool ResolveHostname(const str& pHostname, IPAddress& pIPAddress);
	static bool IsLocalAddress(const str& pAddress);

private:
	static bool mStarted;
	LOG_CLASS_DECLARE();
};



}
