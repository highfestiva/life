
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_map>
#include <set>
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/Timer.h"
#include "../Life.h"

//#define MASTER_SERVER_NAME	"rg.servegame.org"
//#define MASTER_SERVER_NAME	"localhost"
//#define MASTER_SERVER_NAME	"192.168.2.1"
#define MASTER_SERVER_NAME	"192.168.1.21"
#define MASTER_SERVER_PORT	"35749"
#define MASTER_SERVER_TIMEOUT	2*60
#define MASTER_SERVER_USI	"UploadServerInfo"
#define MASTER_SERVER_DSL	"DownloadServerList"
#define MASTER_SERVER_IC	"InitiateConnect"
#define MASTER_SERVER_DC	"Disconnect"



namespace Lepra
{
class SocketAddress;
class UdpMuxSocket;
class UdpVSocket;
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
	void KillDeadServers();
	void KillDeadSockets();
	void OnQuitRequest(int pLevel);
	void HandleReceive(UdpVSocket* pRemote, const uint8* pCommand, unsigned pCommandLength);
	bool HandleCommandLine(UdpVSocket* pRemote, const str& pCommandLine);
	bool RegisterGameServer(bool pActivate, UdpVSocket* pRemote, const str& pName, int pPort, int pPlayerCount, const str& pId);
	bool SendServerList(UdpVSocket* pRemote);
	bool Send(UdpVSocket* pRemote, const str& pData);
	void DropSocket(UdpVSocket* pRemote);

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
