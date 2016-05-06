
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/unordered.h"
#include <set>
#include "../../cure/include/gameticker.h"
#include "../../lepra/include/memberthread.h"
#include "../../lepra/include/timer.h"
#include "../life.h"
#include "../serverinfo.h"

#define kMasterServerTimeout	2*60
#define kMasterServerUsi	"UploadServerInfo"
#define kMasterServerDsl	"DownloadServerList"
#define kMasterServerOf	"OpenFirewall"
#define kMasterServerUl	"UseLAN"
#define kMasterServerDc	"Disconnect"



namespace lepra {
class SocketAddress;
class UdpMuxSocket;
class UdpVSocket;
}



namespace life {



struct ServerInfo;



class MasterServer: public cure::ApplicationTicker {
public:
	MasterServer(const str& port);
	virtual ~MasterServer();

	virtual bool Initialize();
	virtual bool Tick();
	virtual void PollRoundTrip();
	virtual float GetTickTimeReduction() const;
	virtual float GetPowerSaveAmount() const;

private:
	void KillDeadServers();
	void KillDeadSockets();
	void OnQuitRequest(int level);
	void HandleReceive(UdpVSocket* remote, const uint8* command, unsigned command_length);
	bool HandleCommandLine(UdpVSocket* remote, const str& command_line);
	bool RegisterGameServer(const ServerInfo& server_info, UdpVSocket* remote);
	bool SendServerList(UdpVSocket* remote);
	bool OpenFirewall(UdpVSocket* remote, const ServerInfo& server_info);
	bool Send(UdpVSocket* remote, const str& data);
	void DropSocket(UdpVSocket* remote);

	struct GameServerInfo: public ServerInfo {
		Timer idle_time_;

		GameServerInfo(const ServerInfo& server_info);
	};

	typedef std::unordered_map<str, GameServerInfo> GameServerTable;
	typedef std::set<UdpVSocket*> SocketTable;

	UdpMuxSocket* mux_socket_;
	str port_;
	SocketTable socket_table_;
	SocketTable socket_timeout_table_;
	Lock lock_;
	GameServerTable game_server_table_;
	Timer keepalive_timer_;
	Timer server_idle_timer_;

	logclass();
};



}
