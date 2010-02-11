
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameServerManager.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/CppContextObject.h"
#include "../../Cure/Include/NetworkServer.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/PositionalData.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/AntiCrack.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../Life.h"
#include "../LifeApplication.h"
#include "../RtVar.h"
#include "Client.h"
#include "ServerConsoleManager.h"



// Run before main() is started.
AntiCrack _r__;



namespace Life
{



const int NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE = PHYSICS_FPS/2;



GameServerManager::GameServerManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	InteractiveConsoleLogListener* pConsoleLogger):
	Cure::GameManager(pVariableScope, pResourceManager),
	mUserAccountManager(new Cure::MemoryUserAccountManager()),
	mTerrainObject(0),
	mBoxObject(0),
	mMovementArrayList(NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE)
{
	ConsoleManager lConsole(0, Cure::GetSettings(), 0, 0);
	lConsole.Init();
	lConsole.ExecuteCommand(_T("execute-file -i ServerDefault.lsh"));
	lConsole.ExecuteCommand(_T("execute-file -i ") + Application::GetIoFile(_T("Base"), _T("lsh")));

	GetResourceManager()->InitDefault();

	//GetContext()->SetIsObjectOwner(false);

	SetNetworkAgent(new Cure::NetworkServer(pVariableScope, this));

	SetConsoleManager(new ServerConsoleManager(this, GetVariableScope(), pConsoleLogger, new StdioConsolePrompt()));
	GetConsoleManager()->PushYieldCommand(_T("execute-file -i ") + Application::GetIoFile(_T("Application"), _T("lsh")));
	GetConsoleManager()->Start();
}

GameServerManager::~GameServerManager()
{
	{
		ConsoleManager lConsole(0, Cure::GetSettings(), 0, 0);
		lConsole.Init();
		lConsole.ExecuteCommand(_T("save-system-config-file 0 ") + Application::GetIoFile(_T("Base"), _T("lsh")));
	}

	DeleteAllClients();

	delete (mUserAccountManager);
	mUserAccountManager = 0;

	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ") + Application::GetIoFile(_T("Application"), _T("lsh")));
}



bool GameServerManager::Tick()
{
	assert(!GetNetworkAgent()->GetLock()->IsOwner());
	// Don't do this here! Done explicitly in parent.
	//          ==>  ScopeLock lTickLock(GetTickLock());
	bool lOk = Parent::BeginTick();
	lOk = (lOk && Parent::EndTick());
	GetResourceManager()->Tick();

	if (CURE_RTVAR_GETSET(GetVariableScope(), RTVAR_ALLLOGGEDOUT_AUTOSHUTDOWN, false))
	{
		static size_t lMaxLoginCount = 0;
		size_t lUserCount = ListUsers().size();
		lMaxLoginCount = (lUserCount > lMaxLoginCount)? lUserCount : lMaxLoginCount;
		if (lMaxLoginCount > 0 && lUserCount == 0)
		{
			mLog.AWarning("Server automatically shuts down since rtvar active and all users now logged off.");
			SystemManager::AddQuitRequest(+1);
		}
	}

	return (lOk);
}



wstrutil::strvec GameServerManager::ListUsers()
{
	wstrutil::strvec lVector;
	{
		assert(!GetNetworkAgent()->GetLock()->IsOwner());
		ScopeLock lTickLock(GetTickLock());
		ScopeLock lNetLock(GetNetworkAgent()->GetLock());

		AccountClientTable::Iterator x = mAccountClientTable.First();
		for (; x != mAccountClientTable.End(); ++x)
		{
			const Client* lClient = x.GetObject();
			wstr lUserInfo = lClient->GetUserConnection()->GetLoginName();
			Cure::ContextObject* lObject = GetContext()->GetObject(lClient->GetAvatarId());
			if (lObject)
			{
				Vector3DF lPosition = lObject->GetPosition();
				lUserInfo += wstrutil::Format(L" at (%f, %f, %f)", lPosition.x, lPosition.y, lPosition.z);
			}
			else
			{
				lUserInfo += L" [not loaded]";
			}
			lVector.push_back(lUserInfo);
		}
	}
	return (lVector);
}

bool GameServerManager::BroadcastChatMessage(const wstr& pMessage)
{
	bool lOk = false;

	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	{
		assert(!GetNetworkAgent()->GetLock()->IsOwner());
		ScopeLock lTickLock(GetTickLock());
		ScopeLock lNetLock(GetNetworkAgent()->GetLock());
		AccountClientTable::Iterator x = mAccountClientTable.First();
		for (; x != mAccountClientTable.End(); ++x)
		{
			const Client* lClient = x.GetObject();
			lOk |= GetNetworkAgent()->SendStatusMessage(lClient->GetUserConnection()->GetSocket(), 0, Cure::REMOTE_OK, pMessage, lPacket);
		}
	}
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
	return (lOk);
}

bool GameServerManager::SendChatMessage(const wstr& pClientUserName, const wstr& pMessage)
{
	assert(!GetNetworkAgent()->GetLock()->IsOwner());
	ScopeLock lTickLock(GetTickLock());
	ScopeLock lNetLock(GetNetworkAgent()->GetLock());

	Cure::UserAccount::AccountId lAccountId;
	bool lOk = mUserAccountManager->GetUserAccountId(pClientUserName, lAccountId);
	const Client* lClient = 0;
	if (lOk)
	{
		lClient = GetClientByAccount(lAccountId);
		lOk = (lClient != 0);
	}
	if (lOk)
	{
		Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
		lOk = GetNetworkAgent()->SendStatusMessage(lClient->GetUserConnection()->GetSocket(), 0, Cure::REMOTE_OK, pMessage, lPacket);
		GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
	}
	return (lOk);
}



int GameServerManager::GetLoggedInClientCount() const
{
	assert(!GetNetworkAgent()->GetLock()->IsOwner());
	ScopeLock lTickLock(GetTickLock());
	ScopeLock lNetLock(GetNetworkAgent()->GetLock());
	return (mAccountClientTable.GetCount());
}



void GameServerManager::Logout(Cure::UserAccount::AccountId pAccountId, const str& pReason)
{
	GetNetworkServer()->Disconnect(pAccountId, pReason, true);
}

void GameServerManager::DeleteAllClients()
{
	while (!mAccountClientTable.IsEmpty())
	{
		Cure::UserAccount::AccountId lAccountId = mAccountClientTable.First().GetKey();
		Logout(lAccountId, _T("Server shutdown"));
	}
}



Client* GameServerManager::GetClientByAccount(Cure::UserAccount::AccountId pAccountId) const
{
	Client* lClient = mAccountClientTable.FindObject(pAccountId);
	return (lClient);
}



bool GameServerManager::Initialize()
{
	bool lOk = InitializeTerrain();

	// TODO: remove entirely! The accounts should be created elsewhere:
	//    Online game)     typically in a database via a webservice.
	//    Anonymous game)  in RAM by the implementing server-side class.
	if (lOk)
	{
		int x;
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Ball%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("sphere_002")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Car%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("car_001")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Monster%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("monster_001")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Excavator%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("excavator_703")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Crane%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("crane_whatever")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Tractor%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("tractor_02")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"OriginalTractor%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("tractor_01")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Fjask%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("fjask")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"RoadRoller%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("road_roller_01")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Truck%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("truck_01")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Heli%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("helicopter_01")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Loader%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("frontloader")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"TowTruck%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("towtruck_01")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"M%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("monster_02")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"Hovercraft%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName, Cure::UserAccount::AvatarId(_T("hovercraft_01")));
			}
		}
	}

	/*if (lOk)
	{
		assert(mBoxObject == 0);
		mBoxObject = Parent::CreateContextObject(_T("box_002"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
		mBoxObject->StartLoading();
	}*/

	// TODO: remove!!!
	//Cure::ContextObject* lTractor = Parent::CreateContextObject(_T("tractor_01"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	//lTractor->StartLoading();

	str lAcceptAddress = CURE_RTVAR_GETSET(GetVariableScope(), RTVAR_NETWORK_LISTEN_ADDRESS, _T("0.0.0.0:16650"));
	if (lOk)
	{
		SocketAddress lAddress;
		if (!lAddress.Resolve(lAcceptAddress))
		{
			mLog.Warningf(_T("Could not resolve address '%s'."), lAcceptAddress.c_str());
			lAcceptAddress = _T(":16650");
			if (!lAddress.Resolve(lAcceptAddress))
			{
				mLog.Errorf(_T("Could not resolve address '%s', defaulting to 'localhost'."), lAcceptAddress.c_str());
				lAcceptAddress = _T("localhost:16650");
			}
		}
	}
	if (lOk)
	{
		lOk = GetNetworkServer()->Start(lAcceptAddress);
		if (!lOk)
		{
			mLog.Fatalf(_T("Is a server already running on '%s'?"), lAcceptAddress.c_str());
		}
	}
	return (lOk);
}

bool GameServerManager::InitializeTerrain()
{
	assert(mTerrainObject == 0);
	mTerrainObject = Parent::CreateContextObject(_T("level_01"), Cure::NETWORK_OBJECT_LOCAL_ONLY);
	mTerrainObject->StartLoading();
	return (true);
}



float GameServerManager::GetPowerSaveAmount() const
{
	mPowerSaveTimer.UpdateTimer();
	float lPowerSave;
	// TODO: if there are logged-in clients, check if all have been idle lately.
	if (GetLoggedInClientCount() > 0)
	{
		lPowerSave = 0;	// Users are currently playing = no power save.
		mPowerSaveTimer.ClearTimeDiff();
	}
	else if (mPowerSaveTimer.GetTimeDiffF() < 10.0)
	{
		lPowerSave = 0;	// Users played until very recently = no power save yet.
	}
	else if (GetNetworkAgent()->GetConnectionCount() > 0)
	{
		lPowerSave = 0.1f;	// Someone is currently logging in, but not logged in yet.
	}
	else
	{
		lPowerSave = 1.0f;	// None logged in, none connected.
	}
	return (lPowerSave);
}



void GameServerManager::TickInput()
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::NetworkAgent::ReceiveStatus lReceived = Cure::NetworkAgent::RECEIVE_OK;
	while (lReceived != Cure::NetworkAgent::RECEIVE_NO_DATA)
	{
		Cure::UserAccount::AccountId lAccountId;
		lReceived = GetNetworkServer()->ReceiveFirstPacket(lPacket, lAccountId);
		switch (lReceived)
		{
			case Cure::NetworkAgent::RECEIVE_OK:
			{
				Client* lClient = GetClientByAccount(lAccountId);
				assert (lClient);
				//log_volatile(mLog.Debugf(_T("Received data from %s."), lClient->GetUserConnection()->GetLoginName().c_str()));
				Cure::Packet::ParseResult lParseResult;
				// Walk packets.
				do
				{
					// Walk messages.
					const int lMessageCount = lPacket->GetMessageCount();
					for (int x = 0; x < lMessageCount; ++x)
					{
						Cure::Message* lMessage = lPacket->GetMessageAt(x);
						ProcessNetworkInputMessage(lClient, lMessage);
					}
					lParseResult = lPacket->ParseMore();
				}
				while (lParseResult == Cure::Packet::PARSE_OK);
				if (lParseResult != Cure::Packet::PARSE_NO_DATA)
				{
					mLog.AError("Problem with receiving crap extra packet!");
				}
			}
			break;
			case Cure::NetworkAgent::RECEIVE_PARSE_ERROR:
			{
				mLog.AError("Problem with receiving crap data!");
			}
			break;
			case Cure::NetworkAgent::RECEIVE_CONNECTION_BROKEN:
			{
				mLog.AError("Disconnected from client!");
			}
			break;
			case Cure::NetworkAgent::RECEIVE_NO_DATA:
			{
				// Nothing, really.
			}
			break;
		}
	}
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

	// Apply buffered prediction movement.
	ApplyStoredMovement();
}

void GameServerManager::ProcessNetworkInputMessage(Client* pClient, Cure::Message* pMessage)
{
	Cure::MessageType lType = pMessage->GetType();
	switch (lType)
	{
		case Cure::MESSAGE_TYPE_STATUS:
		{
			Cure::MessageStatus* lStatus = (Cure::MessageStatus*)pMessage;
			if (lStatus->GetRemoteStatus() == Cure::REMOTE_NO_CONNECTION)
			{
				log_atrace("User disconnects.");
				GetNetworkServer()->Disconnect(pClient->GetUserConnection()->GetAccountId(), _T(""), false);
			}
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_POSITION:
		{
			Cure::MessageObjectPosition* lMovement = (Cure::MessageObjectPosition*)pMessage;
			// TODO: make sure client is authorized to control object with given ID.
			Cure::GameObjectId lInstanceId = lMovement->GetObjectId();
			int32 lClientFrameIndex = lMovement->GetFrameIndex();
			AdjustClientSimulationSpeed(pClient, lClientFrameIndex);

			// TODO:
			//if (pClient->IsInControlOf(lInstanceId))
			{
				// Pass on to other clients.
				const Cure::ObjectPositionalData& lPosition = lMovement->GetPositionalData();
				BroadcastObjectPosition(lInstanceId, lPosition, pClient, false);
				StoreMovement(lClientFrameIndex, lMovement);
			}
			/* TODO:
			else
			{
				mLog.Warningf(_T("Client %i tried to control instance ID %i."),
					strutil::ToCurrentCode(pClient->GetUserConnection()->GetLoginName()).c_str(),
					lInstanceId);
			}*/
		}
		break;
		case Cure::MESSAGE_TYPE_NUMBER:
		{
			Cure::MessageNumber* lNumber = (Cure::MessageNumber*)pMessage;
			if (lNumber->GetInfo() == Cure::MessageNumber::INFO_PING)
			{
				GetNetworkAgent()->SendNumberMessage(false, pClient->GetUserConnection()->GetSocket(),
					Cure::MessageNumber::INFO_PONG, lNumber->GetInteger(), pClient->GetPhysicsFrameAheadCount());
				log_volatile(mLog.Debugf(_T("Replying to PING (PONG) to %s (ptr=%p)."),
					pClient->GetUserConnection()->GetSocket()->GetTargetAddress().GetAsString().c_str(),
					pClient->GetUserConnection()->GetSocket()));
			}
			else
			{
				mLog.AError("Received an invalid MessageNumber from client.");
			}
		}
		break;
		default:
		{
			mLog.AError("Got bad message type from client.");
		}
		break;
	}
}



Cure::UserAccount::Availability GameServerManager::QueryLogin(const Cure::LoginId& pLoginId, Cure::UserAccount::AccountId& pAccountId)
{
	ScopeLock lLock(GetNetworkAgent()->GetLock());
	return (mUserAccountManager->GetUserAccountStatus(pLoginId, pAccountId));
}

void GameServerManager::OnLogin(Cure::UserConnection* pUserConnection)
{
	ScopeLock lLock(GetNetworkAgent()->GetLock());

	Client* lClient = GetClientByAccount(pUserConnection->GetAccountId());
	assert(!lClient);

	const Cure::UserAccount::AvatarIdSet* lAvatarIdSet = mUserAccountManager->GetUserAvatarIdSet(pUserConnection->GetLoginName());
	if (lAvatarIdSet && !lAvatarIdSet->empty())
	{
		lClient = new Client(GetTimeManager(), GetNetworkAgent(), pUserConnection);
		mAccountClientTable.Insert(pUserConnection->GetAccountId(), lClient);
		lClient->SendPhysicsFrame(GetTimeManager()->GetCurrentPhysicsFrame());

		const Cure::UserAccount::AvatarId& lAvatarId = *lAvatarIdSet->begin();
		mLog.Info(_T("Loading avatar '")+lAvatarId+_T("' for user ")+wstrutil::ToCurrentCode(lClient->GetUserConnection()->GetLoginName())+_T("."));
		Cure::ContextObject* lObject = Parent::CreateContextObject(lAvatarId,
			Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
		lClient->SetAvatarId(lObject->GetInstanceId());
		lObject->SetExtraData((void*)(size_t)lClient->GetUserConnection()->GetAccountId());
		lObject->StartLoading();
	}
	else
	{
		mLog.Error(_T("User ") + wstrutil::ToCurrentCode(pUserConnection->GetLoginName()) + _T(" does not exist or has no avatar!"));
	}
}

void GameServerManager::OnLogout(Cure::UserConnection* pUserConnection)
{
	assert(!GetNetworkAgent()->GetLock()->IsOwner());
	Cure::GameObjectId lAvatarId = 0;
	ScopeLock lTickLock(GetTickLock());
	{
		ScopeLock lNetLock(GetNetworkAgent()->GetLock());

		// TODO: logout with some timer, and also be able to reconnect the
		// client with his/her avatar again if logged in within the time frame.
		Client* lClient = GetClientByAccount(pUserConnection->GetAccountId());
		assert(lClient);
		lAvatarId = lClient->GetAvatarId();
		assert(IsThreadSafe());
		mAccountClientTable.Remove(pUserConnection->GetAccountId());
		delete (lClient);
		BroadcastDeleteObject(lAvatarId);
	}
	GetContext()->DeleteObject(lAvatarId);

	mLog.Info(_T("User ") + wstrutil::ToCurrentCode(pUserConnection->GetLoginName()) + _T(" logged out."));
}



void GameServerManager::AdjustClientSimulationSpeed(Client* pClient, int pClientFrameIndex)
{
	int lCurrentFrameDiff = GetTimeManager()->GetCurrentPhysicsFrame() - pClientFrameIndex;
	// Calculate client network latency and jitter.
	pClient->StoreFrameDiff(lCurrentFrameDiff);

	// Send physics enginge speed-up of slow-down to client if necessary.
	pClient->QuerySendStriveTimes();
}

void GameServerManager::StoreMovement(int pClientFrameIndex, Cure::MessageObjectMovement* pMovement)
{
	const int lCurrentPhysicsFrame = GetTimeManager()->GetCurrentPhysicsFrame();
	const int lFrameOffset = pClientFrameIndex-lCurrentPhysicsFrame;
	if (lFrameOffset >= 0 && lFrameOffset < NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE)
	{
		Cure::MessageObjectMovement* lMovement = pMovement->CloneToStandalone();
		const int lFrameCycleIndex = (lCurrentPhysicsFrame+lFrameOffset)%NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE;
		//mLog.Infof(_T("Storing position with %u children."), ((Cure::MessageObjectPosition*)lMovement)->GetPositionalData().mBodyPositionArray.size());
		mMovementArrayList[lFrameCycleIndex].push_back(lMovement);
	}
	else
	{
		// This input data is already old or too much ahead! Skip it.
		mLog.Warningf(_T("Skipping store of movement (%i frames ahead)."), lFrameOffset);
	}
}

void GameServerManager::ApplyStoredMovement()
{
	// Walk through all steps (including missed steps due to slow computer).
	int lCurrentPhysicsSteps = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lCurrentPhysicsSteps >= NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE)
	{
		if (GetLoggedInClientCount() > 0)
		{
			mLog.Warningf(_T("Network positional buffer overrun: had to skip %i steps!"), lCurrentPhysicsSteps-NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE+1);
		}
		lCurrentPhysicsSteps = NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE-1;
	}
	int lCurrentPhysicsFrame = GetTimeManager()->GetCurrentPhysicsFrame() - lCurrentPhysicsSteps;
	//mLog.Debugf(_T("[frame %i to %i]"), lCurrentPhysicsFrame, GetTimeManager()->GetCurrentPhysicsFrame());
	for (; lCurrentPhysicsFrame <= GetTimeManager()->GetCurrentPhysicsFrame(); ++lCurrentPhysicsFrame)
	{
		const int lFrameCycleIndex = lCurrentPhysicsFrame%NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE;
		MovementList lMovementList = mMovementArrayList[lFrameCycleIndex];
		LEPRA_DEBUG_CODE(size_t lMovementCount = lMovementList.size();)
		mMovementArrayList[lFrameCycleIndex].clear();
		assert(lMovementCount == lMovementList.size());
		MovementList::iterator x = lMovementList.begin();
		for (; x != lMovementList.end(); ++x)
		{
			Cure::MessageObjectMovement* lMovement = *x;
			Cure::GameObjectId lInstanceId = lMovement->GetObjectId();
			if (lCurrentPhysicsFrame != lMovement->GetFrameIndex())
			{
				mLog.AWarning("Throwing away network movement.");
				continue;	// TRICKY: RAII to keep things simple.
			}
			Cure::ContextObject* lContextObject = GetContext()->GetObject(lInstanceId);
			if (lContextObject)
			{
				if (lMovement->GetType() == Cure::MESSAGE_TYPE_OBJECT_POSITION)
				{
					const Cure::ObjectPositionalData& lData =
						((Cure::MessageObjectPosition*)lMovement)->GetPositionalData();
					lContextObject->SetFullPosition(lData);
				}
				else
				{
					mLog.AError("Not implemented!");
				}
			}
		}
	}
}



void GameServerManager::BroadcastAvatar(Client* pClient)
{
	Cure::ContextObject* lObject = GetContext()->GetObject(pClient->GetAvatarId());
	Cure::GameObjectId lInstanceId = lObject->GetInstanceId();
	mLog.Info(_T("User ")+wstrutil::ToCurrentCode(pClient->GetUserConnection()->GetLoginName())+_T(" login complete (avatar loaded)."));

	// TODO: this is hard-coded. Use a general replication-mechanism instead (where visible and added/updated objects gets replicated automatically).
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	GetNetworkAgent()->SendNumberMessage(true, pClient->GetUserConnection()->GetSocket(),
		Cure::MessageNumber::INFO_AVATAR, lInstanceId, 0);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

	BroadcastCreateObject(lObject);
	SendCreateAllObjects(pClient);
}



Cure::ContextObject* GameServerManager::CreateContextObject(const str& pClassId) const
{
	return (new Cure::CppContextObject(pClassId));
}

void GameServerManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	Client* lClient = 0;
	{
		Cure::UserAccount::AccountId lAccountId = (Cure::UserAccount::AccountId)(size_t)pObject->GetExtraData();
		if (lAccountId)
		{
			lClient = GetClientByAccount(lAccountId);
			if (!lClient)
			{
				mLog.Errorf(_T("Error: client seems to have logged off before avatar %s got loaded."),
					pObject->GetClassId().c_str());
				delete (pObject);
				return;
			}
		}
	}
	if (pOk)
	{
		if (lClient)
		{
			mLog.Infof(_T("Yeeha! Loaded avatar for %s."),
				wstrutil::ToCurrentCode(lClient->GetUserConnection()->GetLoginName()).c_str());
			BroadcastAvatar(lClient);
		}
		else
		{
			mLog.Infof(_T("Loaded object %s."), pObject->GetClassId().c_str());
			OnPhysicsSend(pObject);
		}
	}
	else
	{
		if (lClient)
		{
			mLog.Errorf(_T("Could not load avatar of type %s for user %s."),
				pObject->GetClassId().c_str(), lClient->GetUserConnection()->GetLoginName().c_str());
		}
		else
		{
			mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		}
		assert(false);
		delete (pObject);
	}
}

void GameServerManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2)
{
	if (pObject1 != pObject2 &&	// I.e. car where a wheel collides with the body.
		pObject1->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)	// We only handle network object collisions.
	{
		bool lSendCollision = false;
		const bool lAreBothDynamic = (pObject2 != 0 && pObject2->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY);
		const bool lIsServerControlled = (pObject1->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
		if (lIsServerControlled)
		{
			lSendCollision = IsHighImpact(12.0f, pObject1, pForce, pTorque);
			if (!lSendCollision && lAreBothDynamic)
			{
				// If the other object thinks it's a high impact, we go. This is to not
				// replicate when another - heavier - object is standing on top of us.
				lSendCollision = IsHighImpact(7.0f, pObject2, pForce, pTorque);
			}
		}
		else if (lAreBothDynamic)
		{
			lSendCollision = IsHighImpact(12.0f, pObject1, pForce, pTorque);
		}
		if (lSendCollision)
		{
			// We have found a collision. Asynchronously inform all viewers, including the colliding client.
			pObject1->SetSendCount(2);
			GetContext()->AddPhysicsSenderObject(pObject1);
		}
	}
}

void GameServerManager::OnStopped(Cure::ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId)
{
	const unsigned lRootIndex = 0;
	assert(pObject->GetStructureGeometry(lRootIndex));
	if (pObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY &&
		pObject->GetStructureGeometry(lRootIndex)->GetBodyId() == pBodyId)
	{
		log_volatile(mLog.Debugf(_T("Object %u/%s stopped, sending position."), pObject->GetInstanceId(), pObject->GetClassId().c_str()));
		const Cure::ObjectPositionalData* lPosition = 0;
		if (pObject->UpdateFullPosition(lPosition))
		{
			BroadcastObjectPosition(pObject->GetInstanceId(), *lPosition, 0, false);
		}
	}
}

bool GameServerManager::OnPhysicsSend(Cure::ContextObject* pObject)
{
	bool lResend = false;
	if (pObject->QueryResendTime(0.3f, false))
	{
		log_adebug("Sending collision.");
		const Cure::ObjectPositionalData* lPosition = 0;
		lResend = pObject->UpdateFullPosition(lPosition);
		if (lResend)
		{
			BroadcastObjectPosition(pObject->GetInstanceId(), *lPosition, 0, false);
			lResend = (pObject->PopSendCount() > 0);
		}

	}
	return (lResend);
}

bool GameServerManager::IsConnectAuthorized()
{
	return (true);
}

void GameServerManager::SendAttach(Cure::ContextObject* pObject1, unsigned pId1,
	Cure::ContextObject* pObject2, unsigned pId2)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageObjectAttach* lAttach = (Cure::MessageObjectAttach*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_ATTACH);
	lPacket->AddMessage(lAttach);
	lAttach->Store(lPacket, pObject1->GetInstanceId(), pObject2->GetInstanceId(), (uint16)pId1, (uint16)pId2);

	BroadcastPacket(0, lPacket, true);

	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

void GameServerManager::SendDetach(Cure::ContextObject* pObject1, Cure::ContextObject* pObject2)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageObjectDetach* lDetach = (Cure::MessageObjectDetach*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_DETACH);
	lPacket->AddMessage(lDetach);
	lDetach->Store(lPacket, pObject1->GetInstanceId(), pObject2->GetInstanceId());

	BroadcastPacket(0, lPacket, true);

	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}



void GameServerManager::BroadcastCreateObject(Cure::ContextObject* pObject)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageCreateObject* lCreate = (Cure::MessageCreateObject*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OBJECT);
	lPacket->AddMessage(lCreate);
	lCreate->Store(lPacket, pObject->GetInstanceId(), wstrutil::ToOwnCode(pObject->GetClassId()));
	BroadcastPacket(0, lPacket, true);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

	const Cure::ObjectPositionalData* lPosition = 0;
	if (pObject->UpdateFullPosition(lPosition))
	{
		BroadcastObjectPosition(pObject->GetInstanceId(), *lPosition, 0, true);
	}
}

void GameServerManager::BroadcastDeleteObject(Cure::GameObjectId pInstanceId)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageDeleteObject* lDelete = (Cure::MessageDeleteObject*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_DELETE_OBJECT);
	lPacket->AddMessage(lDelete);
	lDelete->Store(lPacket, pInstanceId);
	AccountClientTable::Iterator x = mAccountClientTable.First();
	for (; x != mAccountClientTable.End(); ++x)
	{
		const Client* lClient = x.GetObject();
		UdpVSocket* lSocket = lClient->GetUserConnection()->GetSocket();
		if (lSocket)
		{
			GetNetworkAgent()->PlaceInSendBuffer(true, lSocket, lPacket);
		}
	}
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

void GameServerManager::SendCreateAllObjects(Client* pClient)
{
	// TODO: restrict to visible, concrete objects.

	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageCreateObject* lCreateMessage = (Cure::MessageCreateObject*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OBJECT);
	Cure::MessageObjectPosition* lPositionMessage = (Cure::MessageObjectPosition*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_POSITION);

	const Cure::ContextManager::ContextObjectTable& lObjectTable = GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::const_iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		// Don't send local objects, such as terrain.
		if (lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			lPacket->Clear();

			// Store creation info.
			lPacket->AddMessage(lCreateMessage);
			lCreateMessage->Store(lPacket, lObject->GetInstanceId(),
				wstrutil::ToOwnCode(lObject->GetClassId()));

			// Store positional info.
			lPacket->AddMessage(lPositionMessage);
			const Cure::ObjectPositionalData* lPosition = 0;
			if (lObject->UpdateFullPosition(lPosition))
			{
				lPositionMessage->Store(lPacket, lObject->GetInstanceId(),
					GetTimeManager()->GetCurrentPhysicsFrame(), *lPosition);
				// Send.
				GetNetworkAgent()->PlaceInSendBuffer(true, pClient->GetUserConnection()->GetSocket(), lPacket);
			}
			else
			{
			}
		}
	}

	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

void GameServerManager::BroadcastObjectPosition(Cure::GameObjectId pInstanceId,
	const Cure::ObjectPositionalData& pPosition, Client* pExcludeClient, bool pSafe)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageObjectPosition* lPosition = (Cure::MessageObjectPosition*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_POSITION);
	lPacket->AddMessage(lPosition);
	lPosition->Store(lPacket, pInstanceId, GetTimeManager()->GetCurrentPhysicsFrame(), pPosition);

	assert(!GetNetworkAgent()->GetLock()->IsOwner());
	ScopeLock lTickLock(GetTickLock());
	BroadcastPacket(pExcludeClient, lPacket, pSafe);

	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

void GameServerManager::BroadcastPacket(const Client* pExcludeClient, Cure::Packet* pPacket, bool pSafe)
{
	AccountClientTable::Iterator x = mAccountClientTable.First();
	for (; x != mAccountClientTable.End(); ++x)
	{
		const Client* lClient = x.GetObject();
		if (lClient != pExcludeClient)
		{
			GetNetworkAgent()->PlaceInSendBuffer(pSafe, lClient->GetUserConnection()->GetSocket(), pPacket);
		}
	}
}



#ifdef LEPRA_DEBUG
TBC::PhysicsManager* GameServerManager::GetPhysicsManager() const
{
	assert(!GetNetworkAgent()->GetLock()->IsOwner() ||
		(GetNetworkAgent()->GetLock()->IsOwner() && GetTickLock()->IsOwner()));
	return Parent::GetPhysicsManager();
}
#endif // Debug mode

Cure::NetworkServer* GameServerManager::GetNetworkServer() const
{
	return ((Cure::NetworkServer*)GetNetworkAgent());
}



LOG_CLASS_DEFINE(GAME, GameServerManager);



}
