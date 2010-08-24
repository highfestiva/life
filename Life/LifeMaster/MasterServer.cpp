
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games
 


#include "MasterServer.h"
#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/SystemManager.h"
#include "MasterServerNetworkParser.h"



namespace Life
{



MasterServer::MasterServer():
	mSocket(0)
{
}

MasterServer::~MasterServer()
{
	SystemManager::AddQuitRequest(+1);
	delete (mSocket);
	mSocket = 0;
}

bool MasterServer::Run()
{
	SocketAddress lAddress;
	str lAcceptAddress = _T("0.0.0.0:") _T(MASTER_SERVER_PORT);
	if (!lAddress.Resolve(_T("0.0.0.0:") _T(MASTER_SERVER_PORT)))
	{
		mLog.Warningf(_T("Could not resolve address '%s'."), lAcceptAddress.c_str());
		lAcceptAddress = _T(":") _T(MASTER_SERVER_PORT);
		if (!lAddress.Resolve(lAcceptAddress))
		{
			mLog.Errorf(_T("Could not resolve address '%s'!"), lAcceptAddress.c_str());
			return false;
		}
	}
	mSocket = new TcpListenerSocket(lAddress);
	if (!mSocket->IsOpen())
	{
		return false;
	}
	mLog.Headline(_T("Master server up and running. Awaiting connections."));
	SystemManager::SetQuitRequestCallback(SystemManager::QuitRequestCallback(this, &MasterServer::OnQuitRequest));
	TcpSocket* lConnectedSocket;
	while (SystemManager::GetQuitRequest() == 0 && (lConnectedSocket = mSocket->Accept()) != 0)
	{
		CmdHandlerThread* lThread = new CmdHandlerThread(lConnectedSocket);
		lThread->RequestSelfDestruct();
		if (!lThread->Start(this, &MasterServer::CommandEntry))
		{
			mLog.Error(_T("Could not start worker thread!"));
			return false;
		}
	}
	mLog.Headline(_T("Terminating master server..."));
	return false;
}



void MasterServer::OnQuitRequest(int)
{
	TcpListenerSocket* lSocket = mSocket;
	mSocket = 0;
	delete lSocket;
}

void MasterServer::CommandEntry()
{
	TcpSocket* lSocket = ((CmdHandlerThread*)Thread::GetCurrentThread())->mSocket;
	uint8 lRawData[1024];
	for (;;)
	{
		const int lRawSize = lSocket->Receive(lRawData, sizeof(lRawData));
		if (lRawSize < 0)
		{
			break;
		}
		else if (lRawSize == 0)
		{
			mLog.Error(_T("Got no data from game server!"));
			break;
		}
		wstr lWideData;
		if (!MasterServerNetworkParser::RawToStr(lWideData, lRawData, lRawSize))
		{
			mLog.Error(_T("Got garbled data from game server!"));
			break;
		}
		const str lCommandLine = strutil::Encode(lWideData);
		if (!HandleCommandLine(lSocket, lCommandLine))
		{
			mLog.Error(_T("Got invalid command from game server!"));
			break;
		}
	}
	delete lSocket;
}

bool MasterServer::HandleCommandLine(TcpSocket* pSocket, const str& pCommandLine)
{
	strutil::strvec lCommandList = strutil::BlockSplit(pCommandLine, _T(" \t\r\n"), false, false);
	const str lCommand = lCommandList[0];
	str lName;
	int lPort = -1;
	str lId;
	bool lRemove = false;
	for (unsigned x = 1; x < lCommandList.size(); x += 2)
	{
		if (lCommandList.size() < x+2)
		{
			mLog.Error(_T("Got too few parameters from game server!"));
			return false;
		}
		if (lCommandList[x] == _T("--name"))
		{
			lName = lCommandList[x+1];
		}
		else if (lCommandList[x] == _T("--port"))
		{
			if (!strutil::StringToInt(lCommandList[x+1], lPort))
			{
				mLog.Error(_T("Got non-integer port parameter from game server!"));
				return false;
			}
			if (lPort < 0 || lPort > 65535)
			{
				mLog.Errorf(_T("Got invalid port number (%i) from game server!"), lPort);
				return false;
			}
		}
		else if (lCommandList[x] == _T("--id"))
		{
			lId = lCommandList[x+1];
		}
		else if (lCommandList[x] == _T("--remove"))
		{
			if (lCommandList[x+1] != _T("true"))
			{
				mLog.Errorf(_T("Got bad --remove argument (%s) from game server!"), lCommandList[x+1].c_str());
				return false;
			}
			lRemove = true;
		}
		else
		{
			mLog.Errorf(_T("Got bad parameter (%s) from game server!"), lCommandList[x].c_str());
			return false;
		}
	}
	if (lCommand == _T(MASTER_SERVER_USI))
	{
		return RegisterGameServer(!lRemove, pSocket, lName, lPort, lId);
	}
	else if (lCommand == _T(MASTER_SERVER_DSL))
	{
		return SendServerList(pSocket);
	}
	else
	{
		mLog.Errorf(_T("Got bad command (%s) from game server!"), lCommand.c_str());
	}
	return false;
}

bool MasterServer::RegisterGameServer(bool pActivate, TcpSocket* pSocket, const str& pName, int pPort, const str& pId)
{
	bool lOk = false;
	const str lAddress = pSocket->GetTargetAddress().GetIP().GetAsString() + _T(":") + strutil::IntToString(pPort, 10);

	{
		ScopeLock lLock(&mLock);
		GameServerTable::iterator x = mGameServerTable.find(lAddress);
		if (x != mGameServerTable.end())
		{
			GameServerInfo& lInfo = x->second;
			if (pActivate)
			{
				if (lInfo.mId == pId)
				{
					lInfo.mName = pName;
					lInfo.mIdleTime.PopTimeDiffF();
					lOk = true;
				}
				else
				{
					mLog.Errorf(_T("Got bad ID (%s) from game server %s!"), pId.c_str(), lInfo.mName.c_str());
				}
			}
			else
			{
				if (lInfo.mName == pName && lInfo.mId == pId)
				{
					mGameServerTable.erase(x);
					lOk = true;
				}
				else
				{
					mLog.Errorf(_T("Could not drop game server %s, due to mismatching name and/or id."), lInfo.mName.c_str());
				}
			}
		}
		else if (pActivate)
		{
			GameServerInfo lInfo(pName, pPort, pId);
			mGameServerTable.insert(GameServerTable::value_type(lAddress, lInfo));
			lOk = true;
		}
		else
		{
			mLog.Errorf(_T("Could not drop game server %s, not found."), pName.c_str());
		}
		if (lOk)
		{
			mLog.RawPrint(Log::LEVEL_INFO, _T("----------------------------------------\nServer list:\n"));
			GameServerTable::iterator x = mGameServerTable.begin();
			for (; x != mGameServerTable.end(); ++x)
			{
				mLog.RawPrint(Log::LEVEL_INFO, x->second.mName + _T(" @ ") + x->first + _T("\n"));
			}
		}
	}

	if (lOk)
	{
		Send(pSocket, _T("OK"));
		return true;
	}
	return false;
}

bool MasterServer::SendServerList(TcpSocket* pSocket)
{
	str lServerList;
	{
		ScopeLock lLock(&mLock);
		GameServerTable::iterator x = mGameServerTable.begin();
		for (; x != mGameServerTable.end(); ++x)
		{
			lServerList += x->second.mName + _T(": ") + x->first + _T("\n");
		}
	}
	return Send(pSocket, lServerList);
}

bool MasterServer::Send(TcpSocket* pSocket, const str& pData)
{
	uint8 lRawData[1024];
	if (pData.size() > sizeof(lRawData)/3)
	{
		mLog.Warning(_T("Trying to send too big chunk to game server."));
		return false;
	}
	unsigned lSendByteCount = MasterServerNetworkParser::StrToRaw(lRawData, wstrutil::Encode(pData));
	if ((unsigned)pSocket->Send(lRawData, lSendByteCount) != lSendByteCount)
	{
		mLog.Warning(_T("Transmission to game server failed."));
		return false;
	}
	return true;
}



MasterServer::CmdHandlerThread::CmdHandlerThread(TcpSocket* pSocket):
	Parent(_T("MasterServerCmdSlave")),
	mSocket(pSocket)
{
}



MasterServer::GameServerInfo::GameServerInfo(const str& pName, int pPort, const str& pId):
	mName(pName),
	mPort(pPort),
	mId(pId)
{
}



LOG_CLASS_DEFINE(NETWORK_SERVER, MasterServer);



}
