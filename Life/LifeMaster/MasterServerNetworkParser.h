
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Lepra/Include/Socket.h"
#include "../Life.h"



namespace Life
{



struct ServerInfo;



class MasterServerNetworkParser
{
public:
	static unsigned StrToRaw(uint8* pRawData, const wstr& pStr);
	static bool RawToStr(wstr& pStr, const uint8* pRawData, unsigned pRawLength);

	static bool ExtractServerInfo(const str& pServerString, ServerInfo& pInfo, const SocketAddress* pRemoteAddress);

	LOG_CLASS_DECLARE();
};



}
