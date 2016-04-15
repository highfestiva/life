
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Socket.h"
#include "../Life.h"



namespace Life
{



struct ServerInfo;



class MasterServerNetworkParser
{
public:
	static unsigned StrToRaw(uint8* pRawData, const str& pStr);
	static bool RawToStr(str& pStr, const uint8* pRawData, unsigned pRawLength);

	static bool ExtractServerInfo(const str& pServerString, ServerInfo& pInfo, const SocketAddress* pRemoteAddress);

	logclass();
};



}
