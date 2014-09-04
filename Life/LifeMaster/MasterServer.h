
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Unordered.h"
#include <set>
#include "../../Cure/Include/GameTicker.h"
#include "../../Lepra/Include/Lock.h"
#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/Timer.h"
#include "../Life.h"
#include "../ServerInfo.h"

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



class MasterServer: public Cure::ApplicationTicker
{
public:
	MasterServer(const str& pPort);
	virtual ~MasterServer();

	virtual bool Initialize();
	virtual bool Tick();
	virtual void PollRoundTrip();
	virtual float GetTickTimeReduction() const;
	virtual float GetPowerSaveAmount() const;

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

	typedef std::unordered_map<str, GameServerInfo> GameServerTable;
	typedef std::set<UdpVSocket*> SocketTable;

	UdpMuxSocket* mMuxSocket;
	str mPort;
	SocketTable mSocketTable;
	SocketTable mSocketTimeoutTable;
	Lock mLock;
	GameServerTable mGameServerTable;
	Timer mKeepaliveTimer;
	Timer mServerIdleTimer;

	logclass();
};



}
