
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_map>
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/Timer.h"
#include "../Life.h"

//#define MASTER_SERVER_NAME	"rg.servegame.org"
#define MASTER_SERVER_NAME	"localhost"
#define MASTER_SERVER_PORT	"35749"
#define MASTER_SERVER_USI	"UploadServerInfo"
#define MASTER_SERVER_DSL	"DownloadServerList"



namespace Lepra
{
class SocketAddress;
class UdpSocket;
}



namespace Life
{



class MasterServer
{
public:
	MasterServer();
	virtual ~MasterServer();
	bool Run();

private:
	void OnQuitRequest(int pLevel);
	void HandleReceive(const SocketAddress& pRemoteAddress, const uint8* pCommand, unsigned pCommandLength);
	bool HandleCommandLine(const SocketAddress& pRemoteAddress, const str& pCommandLine);
	bool RegisterGameServer(bool pActivate, const SocketAddress& pRemoteAddress, const str& pName, int pPort, int pPlayerCount, const str& pId);
	bool SendServerList(const SocketAddress& pRemoteAddress);
	bool Send(const SocketAddress& pRemoteAddress, const str& pData);

	struct GameServerInfo
	{
		str mName;
		int mPort;
		int mPlayerCount;
		str mId;
		Timer mIdleTime;

		GameServerInfo(const str& pName, int pPort, int pPlayerCount, const str& pId);
	};

	typedef std::hash_map<str, GameServerInfo> GameServerTable;

	UdpMuxSocket* mMuxSocket;
	Lock mLock;
	GameServerTable mGameServerTable;

	LOG_CLASS_DECLARE();
};



}
