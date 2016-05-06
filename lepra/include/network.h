
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "ipaddress.h"
#include "log.h"



namespace lepra {



class Network {
public:
	static bool Start();
	static bool Stop();
	static str GetHostname();
	static bool ResolveHostname(const str& hostname, IPAddress& ip_address);
	static bool ResolveIp(const IPAddress& ip_address, str& hostname);
	static bool IsLocalAddress(const str& address);

private:
	static bool started_;
	logclass();
};



}
