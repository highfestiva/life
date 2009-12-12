
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



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
	static String GetHostname();
	static bool ResolveHostname(const String& pHostname, IPAddress& pIPAddress);

private:
	static bool mStarted;
	LOG_CLASS_DECLARE();
};



}
