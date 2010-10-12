
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_map>
#include <set>
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/Timer.h"
#include "../Life.h"
#include "../ServerInfo.h"

#define MASTER_SERVER_NAME	"rg.servegame.org"
//#define MASTER_SERVER_NAME	"localhost"
//#define MASTER_SERVER_NAME	"192.168.2.1"
//#define MASTER_SERVER_NAME	"192.168.1.20"
#define MASTER_SERVER_PORT	"35749"
#define MASTER_SERVER_TIMEOUT	2*60
#define MASTER_SERVER_USI	"UploadServerInfo"
#define MASTER_SERVER_DSL	"DownloadServerList"
#define MASTER_SERVER_OF	"OpenFirewall"
#define MASTER_SERVER_UL	"UseLAN"
#define MASTER_SERVER_DC	"Disconnect"



namespace Lepra
{
class SocketAddress;
class UdpMuxSocket;
class UdpVSocket;
}



namespace Life
{



struct ServerInfo;



class MasterServer
{
public:
	MasterServer();
	virtual ~MasterServer();
	bool Run();

private:
	void KillDeadServers();
	void KillDeadSockets();
	void OnQuitRequest(int pLevel);
	void HandleReceive(UdpVSocket* pRemote, const uint8* pCommand, unsigned pCommandLength);
	bool HandleCommandLine(UdpVSocket* pRemote, const str& pCommandLine);
	bool RegisterGameServer(const ServerInfo& pServerInfo, UdpVSocket* pRemote);
	bool SendServerList(UdpVSocket* pRemote);
	bool OpenFirewall(UdpVSocket* pRemote, const ServerInfo& pServerInfo);
	bool Send(UdpVSocket* pRemote, const str& pData);
	void DropSocket(UdpVSocket* pRemote);

	struct GameServerInfo: public ServerInfo
	{
		Timer mIdleTime;

		GameServerInfo(const ServerInfo& pServerInfo);
	};

	typedef std::hash_map<str, GameServerInfo> GameServerTable;
	typedef std::set<UdpVSocket*> SocketTable;

	UdpMuxSocket* mMuxSocket;
	SocketTable mSocketTable;
	SocketTable mSocketTimeoutTable;
	Lock mLock;
	GameServerTable mGameServerTable;
	Timer mKeepaliveTimer;
	Timer mServerIdleTimer;

	LOG_CLASS_DECLARE();
};



}
