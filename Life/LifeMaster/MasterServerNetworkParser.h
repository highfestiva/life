
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/socket.h"
#include "../life.h"



namespace life {



struct ServerInfo;



class MasterServerNetworkParser {
public:
	static unsigned StrToRaw(uint8* raw_data, const str& s);
	static bool RawToStr(str& s, const uint8* raw_data, unsigned raw_length);

	static bool ExtractServerInfo(const str& server_string, ServerInfo& info, const SocketAddress* remote_address);

	logclass();
};



}
