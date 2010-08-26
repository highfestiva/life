
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
class TcpSocket;
class TcpListenerSocket;
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
	void CommandEntry();
	bool HandleCommandLine(TcpSocket* pSocket, const str& pCommandLine);
	bool RegisterGameServer(bool pActivate, TcpSocket* pSocket, const str& pName, int pPort, int pPlayerCount, const str& pId);
	bool SendServerList(TcpSocket* pSocket);
	static bool Send(TcpSocket* pSocket, const str& pData);

	struct CmdHandlerThread: public MemberThread<MasterServer>
	{
		typedef MemberThread<MasterServer> Parent;
		TcpSocket* mSocket;
		CmdHandlerThread(TcpSocket* pSocket);
	};

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

	TcpListenerSocket* mSocket;
	Lock mLock;
	GameServerTable mGameServerTable;

	LOG_CLASS_DECLARE();
};



}
