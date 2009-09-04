
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games
 


#include "../../Lepra/Include/Endian.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Lepra/Include/Log.h"
#include "../Include/ContextObject.h"
#include "../Include/NetworkServer.h"
#include "../Include/Packet.h"
#include "../Include/RuntimeVariable.h"
#include "../Include/UserConnection.h"



namespace Cure
{



NetworkServer::LoginListener::LoginListener()
{
}

NetworkServer::LoginListener::~LoginListener()
{
}



NetworkServer::NetworkServer(RuntimeVariableScope* pVariableScope, LoginListener* pLoginListener):
	NetworkAgent(pVariableScope),
	mUserConnectionFactory(new UserConnectionFactory()),
	mLoginListener(pLoginListener)
{
}

NetworkServer::~NetworkServer()
{
	Stop();

	mLoginListener = 0;	// TRICKY: caller owns login listener.
	delete (mUserConnectionFactory);
	mUserConnectionFactory = 0;
}

bool NetworkServer::Start(const Lepra::String& pHostAddress)
{
	bool lOk = true;
	Lepra::SocketAddress lAddress;
	if (lOk)
	{
		lOk = lAddress.Resolve(pHostAddress);
	}
	if (lOk)
	{
		mLog.Info(_T("Server listening to address ") + lAddress.GetAsString() + _T("."));
		Lepra::ScopeLock lLock(&mLock);
		SetMuxSocket(new Lepra::GameMuxSocket(_T("Srv "), lAddress, true, 100, 1000));
		mMuxSocket->SetCloseCallback(this, &NetworkServer::OnCloseSocket);
		lOk = mMuxSocket->IsOpen();
	}
	return (lOk);
}

void NetworkServer::Stop()
{
	Lepra::ScopeLock lLock(&mLock);
	while (!mLoggedInIdUserTable.empty())
	{
		LoggedInIdUserTable::iterator x = mLoggedInIdUserTable.begin();
		UserConnection* lUser = x->second;
		RemoveUser(lUser->GetAccountId(), true);
	}
	Parent::Stop();
}



void NetworkServer::Disconnect(UserAccount::AccountId pAccountId, const Lepra::String& pReason, bool pSendDisconnect)
{
	UserConnection* lUser = GetUser(pAccountId);
	if (lUser)
	{
		if (pSendDisconnect)
		{
			Cure::Packet* lPacket = GetPacketFactory()->Allocate();
			Parent::SendStatusMessage(lUser->GetSocket(), 0, Cure::REMOTE_NO_CONNECTION, Lepra::UnicodeStringUtility::ToOwnCode(pReason), lPacket);
			GetPacketFactory()->Release(lPacket);
		}
		else if (lUser->GetSocket())
		{
			lUser->GetSocket()->ClearOutputData();
		}
		RemoveUser(pAccountId, true);
	}
}



bool NetworkServer::PlaceInSendBuffer(bool pSafe, Packet* pPacket, UserAccount::AccountId pAccountId)
{
	bool lOk = false;
	bool lRemoveUser = false;
	UserConnection* lUser = GetUser(pAccountId);
	if (lUser)
	{
		lOk = Parent::PlaceInSendBuffer(pSafe, lUser->GetSocket(), pPacket);
		lRemoveUser = !lOk;
	}
	if (lRemoveUser)
	{
		RemoveUser(pAccountId, true);
	}
	return (lOk);
}

bool NetworkServer::SendAll()
{
	bool lAllSendsOk = true;
	Lepra::GameSocket* lSocket;
	while ((lSocket = mMuxSocket->PopSenderSocket()) != 0)
	{
		int lSendCount = lSocket->SendBuffer();
		bool lOk = (lSendCount > 0);
		if (!lOk)
		{
			lAllSendsOk = false;
			if (lSendCount == 0)
			{
				mLog.AWarning("Disconnecting socket since no data available for sending, though listed as such.");
			}
			else
			{
				mLog.AWarning("Disconnecting socket since send failed.");
			}

			// Socket disconnected: drop user or socket.
			UserConnection* lUser;
			{
				lUser = Lepra::HashUtil::FindMapObject(mSocketUserTable, lSocket);
			}
			if (lUser)
			{
				RemoveUser(lUser->GetAccountId(), true);
			}
			else
			{
				// A pending login or similar.
				DropSocket(lSocket);
			}
		}
	}
	return (lAllSendsOk);
}

NetworkServer::ReceiveStatus NetworkServer::ReceiveFirstPacket(Packet* pPacket, UserAccount::AccountId& pAccountId)
{
	PollAccept();
	KillDeadSockets();

	ReceiveStatus lStatus = RECEIVE_NO_DATA;
	Lepra::GameSocket* lSocket;
	// Walk through TCP and UDP connections.
	for (int x = 0; lStatus == RECEIVE_NO_DATA && x < 2; ++x)
	{
		while ((lSocket = mMuxSocket->PopReceiverSocket(x == 0)) != 0)
		{
			int lDataLength = lSocket->Receive(x == 0, pPacket->GetWriteBuffer(), pPacket->GetBufferSize());
			UserConnection* lUser;
			{
				lUser = Lepra::HashUtil::FindMapObject(mSocketUserTable, lSocket);
				if (lUser)
				{
					pAccountId = lUser->GetAccountId();
				}
			}
			if (lDataLength == 0)
			{
				lStatus = RECEIVE_NO_DATA;
				// TRICKY: no break here, but in sibling scopes.
			}
			else if (lDataLength > 0)
			{
				pPacket->SetPacketSize(lDataLength);
				if (pPacket->Parse() == Packet::PARSE_OK)
				{
					lStatus = RECEIVE_OK;

					// Good boy, you will not be kicked this run!
					mSocketTimeoutTable.erase(lSocket);
				}
				else
				{
					lStatus = RECEIVE_PARSE_ERROR;
					// TODO: take action on bad network data?
				}
				lSocket->TryAddReceiverSocket();
				// TRICKY: break here, but not in all sibling scopes.
				if (lUser)
				{
					break;	// We have parsed towards a logged-in user. Done.
				}
			}
			else
			{
				lStatus = RECEIVE_CONNECTION_BROKEN;
				// TRICKY: break here, but not in all sibling scopes.
				if (lUser)
				{
					break;	// We have broken pipe towards a logged-in user. Done.
				}
			}
			if (!lUser)
			{
				TryLogin(lSocket, pPacket, lDataLength);
				pPacket->Release();
				lStatus = RECEIVE_NO_DATA;
			}
		}
	}
	if (lStatus == RECEIVE_CONNECTION_BROKEN)
	{
		RemoveUser(pAccountId, true);
	}
	return (lStatus);
}



void NetworkServer::PollAccept()
{
	if (mPendingLoginTable.size() < 1000)
	{
		Lepra::GameSocket* lSocket = mMuxSocket->PollAccept();
		// TODO: add banning techniques to avoid DoS attacks.
		if (lSocket)
		{
			mPendingLoginTable.insert(lSocket);
		}
	}
}

void NetworkServer::TryLogin(Lepra::GameSocket* pSocket, Packet* pPacket, int pDataLength)
{
	{
		PendingSocketTable::iterator x = mPendingLoginTable.find(pSocket);
		if (x == mPendingLoginTable.end())
		{
			// If user hasn't connected properly, we just cut the line.
			DropSocket(pSocket);
			return;	// TRICKY: returning here simplifies.
		}
		mPendingLoginTable.erase(x);
	}

	if (pDataLength >= 1+4)
	{
		if (pPacket->GetMessageCount() == 1)
		{
			if (pPacket->GetMessageAt(0)->GetType() == MESSAGE_TYPE_LOGIN_REQUEST)
			{
				ManageLogin(pSocket, pPacket);
			}
			else
			{
				// This login packet was shit. Ignore it.
				mLog.Warning(_T("Yucky hack packet from ")+
					pSocket->GetTargetAddress().GetAsString()+_T("."));
				DropSocket(pSocket);
			}
		}
		else
		{
			// Too many login packets. Throo avaj.
			mLog.Warning(_T("Too many (login?) packets or crappy data from ")+
				pSocket->GetTargetAddress().GetAsString()+_T("."));
			DropSocket(pSocket);
		}
	}
	else if (pDataLength != 0)
	{
		mLog.Error(_T("Login connection broken to ")+
			pSocket->GetTargetAddress().GetAsString()+_T("."));
		DropSocket(pSocket);
	}
	// TODO: add timeout for sockets in pending state.
}

RemoteStatus NetworkServer::ManageLogin(Lepra::GameSocket* pSocket, Packet* pPacket)
{
	Message* lMessage = pPacket->GetMessageAt(0);
	MessageLoginRequest* lLoginMessage = (MessageLoginRequest*)lMessage;
	Lepra::UnicodeString lPacketLoginName;
	lLoginMessage->GetLoginName(lPacketLoginName);
	Lepra::UnicodeString lLoginName(lPacketLoginName);
	UserAccount::AccountId lAccountId;
	RemoteStatus lStatus = QueryLogin(lLoginName, lLoginMessage, lAccountId);
	switch (lStatus)
	{
		case REMOTE_OK:
		{
			mLog.Info(_T("Logging in user ")+Lepra::UnicodeStringUtility::ToCurrentCode(lLoginName)+_T("."));
			Login(lLoginName, lAccountId, pSocket, pPacket);
		}
		break;
		case REMOTE_LOGIN_ALREADY:
		{
			mLog.Warning(_T("User ")+Lepra::UnicodeStringUtility::ToCurrentCode(lLoginName)+_T(" already logged in."));
			Parent::SendStatusMessage(pSocket, 0, lStatus, L"You have already been logged in.", pPacket);
			DropSocket(pSocket);
		}
		break;
		case REMOTE_LOGIN_ERRONOUS_DATA:
		{
			mLog.Warning(_T("User ")+Lepra::UnicodeStringUtility::ToCurrentCode(lLoginName)+_T(" attempted with wrong username or password."));
			Parent::SendStatusMessage(pSocket, 0, lStatus, L"Wrong username or password. Try again.", pPacket);
			DropSocket(pSocket);
		}
		break;
		case REMOTE_LOGIN_BAN:
		{
			mLog.Warning(_T("User ")+Lepra::UnicodeStringUtility::ToCurrentCode(lLoginName)+_T(" tried logging in, but was banned."));
			Parent::SendStatusMessage(pSocket, 0, lStatus, L"Sorry, you are banned. Try again later.", pPacket);
			DropSocket(pSocket);
		}
		break;
		case REMOTE_NO_CONNECTION:	// TODO: check me out!
		case REMOTE_UNKNOWN:
		{
			mLog.Error(_T("An unknown error occurred when user ")+Lepra::UnicodeStringUtility::ToCurrentCode(lLoginName)+_T(" tried logging in."));
			Parent::SendStatusMessage(pSocket, 0, lStatus, L"Unknown login error, please contact support.", pPacket);
			DropSocket(pSocket);
		}
		break;
	}
	return (lStatus);
}

RemoteStatus NetworkServer::QueryLogin(const Lepra::UnicodeString& pLoginName, MessageLoginRequest* pLoginRequest, UserAccount::AccountId& pAccountId)
{
	RemoteStatus lLoginResult = REMOTE_UNKNOWN;

	UserConnection* lAccount = Lepra::HashUtil::FindMapObject(mLoggedInNameUserTable, pLoginName);
	if (lAccount == 0)
	{
		MangledPassword lMangledPassword = pLoginRequest->GetPassword();
		LoginId lLoginId(pLoginName, lMangledPassword);
		UserAccount::Availability lStatus = mLoginListener->QueryLogin(lLoginId, pAccountId);
		switch (lStatus)
		{
			case UserAccount::STATUS_OK:
			{
				lLoginResult = REMOTE_OK;
			}
			break;
			case UserAccount::STATUS_NOT_PRESENT:
			{
				// Wrong username or password.
				lLoginResult = REMOTE_LOGIN_ERRONOUS_DATA;
			}
			break;
			case UserAccount::STATUS_BANNED:
			case UserAccount::STATUS_TEMPORARY_BANNED:
			{
				// Banned for now or forever.
				lLoginResult = REMOTE_LOGIN_BAN;
			}
			break;
		}
	}
	else
	{
		// User already logged in.
		lLoginResult = REMOTE_LOGIN_ALREADY;
	}

	return (lLoginResult);
}

void NetworkServer::Login(const Lepra::UnicodeString& pLoginName, UserAccount::AccountId pAccountId, Lepra::GameSocket* pSocket, Packet* pPacket)
{
	UserConnection* lUser = mUserConnectionFactory->AllocateUserConnection();
	lUser->SetLoginName(pLoginName);
	lUser->SetAccountId(pAccountId);
	lUser->SetSocket(pSocket);

	AddUser(lUser, pAccountId);

	mLog.Infof(_T("Sending login OK with account ID %i"), pAccountId);
	SendStatusMessage(pAccountId, pAccountId, REMOTE_OK, L"Welcome.", pPacket);

	mLoginListener->OnLogin(lUser);
}

void NetworkServer::AddUser(UserConnection* pUser, UserAccount::AccountId& pAccountId)
{
	mLoggedInIdUserTable.insert(LoggedInIdUserPair(pAccountId, pUser));
	mLoggedInNameUserTable.insert(LoggedInNameUserPair(pUser->GetLoginName(), pUser));
	mSocketUserTable.insert(SocketUserPair(pUser->GetSocket(), pUser));
}

bool NetworkServer::RemoveUser(UserAccount::AccountId pAccountId, bool pDestroy)
{
	UserConnection* lUser = 0;
	{
		LoggedInIdUserTable::iterator x = mLoggedInIdUserTable.find(pAccountId);
		if (x != mLoggedInIdUserTable.end())
		{
			lUser = x->second;
			if (pDestroy)
			{
				mLoggedInIdUserTable.erase(x);
				mLoggedInNameUserTable.erase(lUser->GetLoginName());
			}
			mSocketUserTable.erase(lUser->GetSocket());
			mSocketTimeoutTable.erase(lUser->GetSocket());
		}
	}
	if (lUser)
	{
		Lepra::GameSocket* lSocket = lUser->GetSocket();
		if (lSocket)
		{
			lUser->SetSocket(0);
			DropSocket(lSocket);
		}
		if (pDestroy)
		{
			mLoginListener->OnLogout(lUser);
			mUserConnectionFactory->FreeUserConnection(lUser);
		}
	}
	return (lUser != 0);
}

void NetworkServer::KillDeadSockets()
{
	{
		Lepra::ScopeLock lLock(&mLock);
		AccountIdSet::iterator x = mDropUserList.begin();
		for (; x != mDropUserList.end(); ++x)
		{
			UserAccount::AccountId lUserAccountId = *x;
			mLog.Infof(_T("Kicking user ID %i in drop zone silently."), lUserAccountId);
			Disconnect(lUserAccountId, Lepra::EmptyString, false);
		}
		mDropUserList.clear();
	}

	mKeepaliveTimer.UpdateTimer();
	if (mKeepaliveTimer.GetTimeDiffF() > CURE_RTVAR_GET(mVariableScope, RTVAR_NETWORK_KEEPALIVE_KILLINTERVAL, 20.0))
	{
		// Reset the keepalive timer.
		mKeepaliveTimer.ClearTimeDiff();

		// Kill all old and dead connections.
		while (!mSocketTimeoutTable.empty())
		{
			Lepra::GameSocket* lSocket = *mSocketTimeoutTable.begin();
			SocketUserTable::iterator y = mSocketUserTable.find(lSocket);
			if (y != mSocketUserTable.end())
			{
				UserConnection* lUser = y->second;
				UserAccount::AccountId lUserAccountId = lUser->GetAccountId();
				mLog.Infof(_T("Dropping user %s (ID %i) due to network keepalive timeout."),
					Lepra::UnicodeStringUtility::ToCurrentCode(lUser->GetLoginName()).c_str(), lUserAccountId);
				Disconnect(lUserAccountId, _T("Network timeout"), true);
			}
			else
			{
				// A not-logged-in pending socket. Probably just a spoofer or similar. Close silently.
				mLog.Infof(_T("Dropping connected, but not logged in socket on %s due to network keepalive timeout."),
					lSocket->GetTargetAddress().GetAsString().c_str());
				DropSocket(lSocket);
			}
		}

		// Put all current connections in the "old and dead" bin. If they won't talk within the given
		// keepalive timeout, we kill them off.
		// First copy all pending connections.
		mSocketTimeoutTable = mPendingLoginTable;
		// Then extend with established user connections.
		SocketUserTable::iterator y = mSocketUserTable.begin();
		for (; y != mSocketUserTable.end(); ++y)
		{
			mSocketTimeoutTable.insert(y->second->GetSocket());
		}
	}
}

void NetworkServer::DropSocket(Lepra::GameSocket* pSocket)
{
	pSocket->SendBuffer();
	mSocketTimeoutTable.erase(pSocket);
	mPendingLoginTable.erase(pSocket);
	mMuxSocket->CloseSocket(pSocket);
}

UserConnection* NetworkServer::GetUser(UserAccount::AccountId pAccountId)
{
	UserConnection* lUser = Lepra::HashUtil::FindMapObject(mLoggedInIdUserTable, pAccountId);
	return (lUser);
}



bool NetworkServer::SendStatusMessage(UserAccount::AccountId pAccountId, Lepra::int32 pInteger, RemoteStatus pStatus, Lepra::UnicodeString pMessage, Packet* pPacket)
{
	pPacket->Release();
	MessageStatus* lStatus = (MessageStatus*)mPacketFactory->GetMessageFactory()->Allocate(MESSAGE_TYPE_STATUS);
	pPacket->AddMessage(lStatus);
	lStatus->Store(pPacket, pStatus, pInteger, pMessage);
	bool lOk = PlaceInSendBuffer(true, pPacket, pAccountId);
	return (lOk);
}



void NetworkServer::OnCloseSocket(Lepra::GameSocket* pSocket)
{
	Lepra::ScopeLock lLock(&mLock);
	SocketUserTable::iterator x = mSocketUserTable.find(pSocket);
	if (x != mSocketUserTable.end())
	{
		UserConnection* lUser = x->second;
		UserAccount::AccountId lUserAccountId = lUser->GetAccountId();
		mLog.Infof(_T("Placing user %s (ID %i) in drop zone (due to OOB? kill). Will drop next frame."),
			Lepra::UnicodeStringUtility::ToCurrentCode(lUser->GetLoginName()).c_str(), lUserAccountId);
		RemoveUser(lUserAccountId, false);
		mDropUserList.insert(lUserAccountId);
	}
	else
	{
		mLog.Infof(_T("Network drop (OOB?) of socket on %s."),
			pSocket->GetTargetAddress().GetAsString().c_str());
		DropSocket(pSocket);
	}
}



LOG_CLASS_DEFINE(NETWORK_SERVER, NetworkServer);



}
