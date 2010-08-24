
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games
 


#include "MasterServerConnection.h"
#include "../../Lepra/Include/Socket.h"
#include "../LifeMaster/MasterServer.h"
#include "../LifeMaster/MasterServerNetworkParser.h"



namespace Life
{



MasterServerConnection::MasterServerConnection():
	mState(DISCONNECTED),
	mSocket(0),
	mConnecter(new MemberThread<MasterServerConnection>(_T("MasterServerConnecter"))),
	mDisconnectCounter(0)
{
}

MasterServerConnection::~MasterServerConnection()
{
	GraceClose(2.0);
	delete mConnecter;
	mConnecter = 0;
}


void MasterServerConnection::SendLocalInfo(const str& pLocalServerInfo)
{
	if (mLocalServerInfo != pLocalServerInfo)
	{
		mLocalServerInfo = pLocalServerInfo;
		QueryAddState(UPLOAD_INFO);
	}
}

void MasterServerConnection::AppendLocalInfo(const str& pExtraServerInfo)
{
	if (!mLocalServerInfo.empty())
	{
		mLocalServerInfo += pExtraServerInfo;
		QueryAddState(UPLOAD_INFO);
		mIdleTimer.ReduceTimeDiff(-1.1*mDisconnectedIdleTimeout);
	}
}

void MasterServerConnection::RequestServerList(const str& pServerCriterias)
{
	mServerSortCriterias = pServerCriterias;
	QueryAddState(DOWNLOAD_LIST);
}

str MasterServerConnection::GetServerList() const
{
	return str();
}

double MasterServerConnection::WaitUntilDone(double pTimeout, bool pAllowReconnect)
{
	const int lStartDisonnectCounter = mDisconnectCounter;
	HiResTimer lTimer;
	while (lTimer.QueryTimeDiff() < pTimeout && (pAllowReconnect || mSocket) &&
		lStartDisonnectCounter == mDisconnectCounter && !mStateList.empty())
	{
		Tick();
		Thread::Sleep(0.001);
	}
	if (lTimer.QueryTimeDiff() < pTimeout && mSocket && lStartDisonnectCounter == mDisconnectCounter)
	{
		Tick();
	}
	return lTimer.QueryTimeDiff();
}

void MasterServerConnection::GraceClose(double pTimeout)
{
	const double lWaitedTime = WaitUntilDone(pTimeout, false);
	mConnecter->RequestStop();
	if (mSocket)
	{
		mSocket->Close();
	}
	mConnecter->Join(pTimeout - lWaitedTime);
	Close();
}

void MasterServerConnection::Tick()
{
	switch (mState)
	{
		case UPLOAD_INFO:
		{
			if (!UploadServerInfo())
			{
				Close();
			}
		}
		break;
		case DOWNLOAD_LIST:
		{
			if (!DownloadServerList())
			{
				Close();
			}
		}
		break;
	}
	StepState();
}



void MasterServerConnection::QueryAddState(State pState)
{
	if (mState == pState)
	{
		return;
	}
	std::list<State>::iterator x = mStateList.begin();
	for (; x != mStateList.end(); ++x)
	{
		if (*x == pState)
		{
			return;
		}
	}
	if (mStateList.empty())
	{
		mIdleTimer.PopTimeDiff();
	}
	mStateList.push_back(pState);
}

void MasterServerConnection::StepState()
{
	switch (mState)
	{
		case CONNECT:
		{
			if (!mConnecter->IsRunning())
			{
				mState = WORKING;
				if (!mConnecter->Start(this, &MasterServerConnection::ConnectEntry))
				{
					mLog.Warning(_T("Could not start connecter."));
				}
			}
			else
			{
				mLog.Warning(_T("Trying to connect while connecter still running!"));
				assert(!mSocket);
				assert(false);
			}
		}
		break;
		case CONNECTED:
		{
			if (!mStateList.empty())
			{
				mState = mStateList.front();
				mStateList.pop_front();
				mIdleTimer.PopTimeDiff();
			}
			else if (mIdleTimer.QueryTimeDiff() >= mConnectedIdleTimeout)
			{
				Close();
			}
		}
		break;
		case UPLOAD_INFO:
		case DOWNLOAD_LIST:
		{
			mState = CONNECTED;
		}
		break;
		case DISCONNECTED:
		{
			if (mIdleTimer.QueryTimeDiff() >= mDisconnectedIdleTimeout && !mStateList.empty())
			{
				// Anything we want done, we must be connected.
				mState = CONNECT;
			}
		}
		break;
	}
}

void MasterServerConnection::ConnectEntry()
{
	if (mState != WORKING || mSocket != 0)
	{
		mLog.Warning(_T("Starting connector thread while already working/connected!"));
		assert(false);
		return;
	}

	mSocket = new TcpSocket(0);
	if (!mSocket->IsOpen() || mConnecter->GetStopRequest())
	{
		mLog.Warning(_T("Could open TCP socket."));
		Close();
		return;
	}
	const str lMasterServerAddress(_T(MASTER_SERVER_NAME) _T(":") _T(MASTER_SERVER_PORT));
	SocketAddress lTargetAddress;
	if (!lTargetAddress.Resolve(lMasterServerAddress))
	{
		mLog.Warningf(_T("Could not resolve master server address '%s'."), lMasterServerAddress.c_str());
		Close();
		return;
	}
	if (!mSocket->Connect(lTargetAddress) || mConnecter->GetStopRequest())
	{
		mLog.Warningf(_T("Could not connect to master server address '%s'."), lMasterServerAddress.c_str());
		Close();
		return;
	}
	assert(mState == WORKING);
	mState = CONNECTED;
	mIdleTimer.PopTimeDiff();
}

bool MasterServerConnection::UploadServerInfo()
{
	if (!SendAndAck(_T(MASTER_SERVER_USI) _T(" ") + mLocalServerInfo))
	{
		mLocalServerInfo.clear();	// TRICKY: this will make sure that we don't try to drop our server on shutdown.
		return false;
	}
	mLog.Info(_T("Seems to have uploaded server info..."));
	return true;
}

bool MasterServerConnection::DownloadServerList()
{
	str lReply;
	if (!Send(_T(MASTER_SERVER_DSL) _T(" ") + mServerSortCriterias, lReply))
	{
		return false;
	}
	mLog.Info(lReply);	// TODO: fix, and move the console text interface.
	return true;
}

bool MasterServerConnection::SendAndAck(const str& pData)
{
	str lReply;
	if (!Send(pData, lReply))
	{
		return false;
	}
	return lReply == _T("OK");
}

bool MasterServerConnection::Send(const str& pData, str& pReply)
{
	if (!mSocket || !mSocket->IsConnected())
	{
		mLog.Warning(_T("Trying to send to master server even though unconnected."));
		return false;
	}
	if (pData.size() > 300)
	{
		mLog.Warning(_T("Trying to send too big chunk to master server."));
		return false;
	}
	uint8 lRawData[1024];
	unsigned lSendByteCount = MasterServerNetworkParser::StrToRaw(lRawData, wstrutil::Encode(pData));
	if ((unsigned)mSocket->Send(lRawData, lSendByteCount) != lSendByteCount)
	{
		mLog.Warning(_T("Transmission to master server failed."));
		return false;
	}
	return Receive(pReply);
}

bool MasterServerConnection::Receive(str& pData)
{
	if (!mSocket || !mSocket->IsConnected())
	{
		mLog.Warning(_T("Trying to receive master server data even though unconnected."));
		return false;
	}
	uint8 lRawData[1024];
	int lRawSize = mSocket->Receive(lRawData, sizeof(lRawData));
	if (lRawSize <= 0)
	{
		mLog.Warning(_T("Tried to receive data from master server, but it didn't send us any!"));
		return false;
	}
	wstr lWideData;
	if (!MasterServerNetworkParser::RawToStr(lWideData, lRawData, lRawSize))
	{
		mLog.Error(_T("Got garbled data from master server - something is seriously wrong!"));
		return false;
	}
	pData = strutil::Encode(lWideData);
	return true;
}

void MasterServerConnection::Close()
{
	++mDisconnectCounter;
	delete mSocket;
	mSocket = 0;
	mState = DISCONNECTED;
	mStateList.clear();	// A) we lost connection, no use trying for a while, or B) nothing more to do (=already empty).
}



const double MasterServerConnection::mConnectedIdleTimeout = 10.0;
const double MasterServerConnection::mDisconnectedIdleTimeout = 1.0;

LOG_CLASS_DEFINE(NETWORK_SERVER, MasterServerConnection);



}
