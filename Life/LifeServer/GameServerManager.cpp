
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



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
#include "Client.h"
#include "GameServerManager.h"
#include "ServerConsoleManager.h"



// Run before main() is started.
AntiCrack _r__;



namespace Life
{



const int NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE = PHYSICS_FPS/2;



GameServerManager::GameServerManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	Lepra::InteractiveConsoleLogListener* pConsoleLogger):
	Cure::GameManager(pVariableScope, pResourceManager, false),
	mUserAccountManager(new Cure::MemoryUserAccountManager()),
	mTerrainResource(ContextObjectInfo(GetContext(), 0, 0, Cure::NETWORK_OBJECT_LOCAL_ONLY)),
	mBoxResource(ContextObjectInfo(GetContext(), 0, 0, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED)),
	mMovementArrayList(NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE)
{
	unsigned lPhysicsFps = CURE_RTVAR_GET(pVariableScope, RTVAR_PHYSICS_FPS, 60);
	GetResourceManager()->InitDefault(new Cure::CppContextObjectFactory(lPhysicsFps));

	GetContext()->SetIsObjectOwner(false);

	mTerrainResource.GetExtraData().mInstanceId = GetContext()->AllocateGameObjectId(mTerrainResource.GetExtraData().mNetworkType);
	mTerrainResource.GetExtraData().mLoadingResource = &mTerrainResource;
	mBoxResource.GetExtraData().mInstanceId = GetContext()->AllocateGameObjectId(mBoxResource.GetExtraData().mNetworkType);
	mBoxResource.GetExtraData().mLoadingResource = &mBoxResource;

	SetNetworkAgent(new Cure::NetworkServer(pVariableScope, this));

	SetConsoleManager(new ServerConsoleManager(this, GetVariableScope(), pConsoleLogger, new Lepra::StdioConsolePrompt()));
	GetConsoleManager()->Start();
	GetConsoleManager()->ExecuteCommand(_T("execute-file ")+GetSystemCommandFilename());
	GetConsoleManager()->ExecuteCommand(_T("execute-file ")+GetApplicationCommandFilename());
}

GameServerManager::~GameServerManager()
{
	DeleteAllClients();

	delete (mUserAccountManager);
	mUserAccountManager = 0;

	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file -no ")+GetApplicationCommandFilename());
	GetConsoleManager()->ExecuteCommand(_T("save-system-config-file -no ")+GetSystemCommandFilename());
}



bool GameServerManager::Tick()
{
	// Don't do this here! Done explicitly in parent.
	//          ==>  Lepra::ScopeLock lTickLock(GetTickLock());
	bool lOk = Parent::BeginTick();
	lOk = (lOk && Parent::EndTick());
	GetResourceManager()->Tick();
	return (lOk);
}



Lepra::UnicodeStringUtility::StringVector GameServerManager::ListUsers()
{
	Lepra::UnicodeStringUtility::StringVector lVector;
	{
		Lepra::ScopeLock lTickLock(GetTickLock());
		Lepra::ScopeLock lNetLock(GetNetworkAgent()->GetLock());

		AccountClientTable::Iterator x = mAccountClientTable.First();
		for (; x != mAccountClientTable.End(); ++x)
		{
			const Client* lClient = x.GetObject();
			lVector.push_back(lClient->GetUserConnection()->GetLoginName());
		}
	}
	return (lVector);
}

bool GameServerManager::BroadcastChatMessage(const Lepra::UnicodeString& pMessage)
{
	bool lOk = false;

	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	{
		Lepra::ScopeLock lTickLock(GetTickLock());
		Lepra::ScopeLock lNetLock(GetNetworkAgent()->GetLock());
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

bool GameServerManager::SendChatMessage(const Lepra::UnicodeString& pClientUserName, const Lepra::UnicodeString& pMessage)
{
	Lepra::ScopeLock lTickLock(GetTickLock());
	Lepra::ScopeLock lNetLock(GetNetworkAgent()->GetLock());

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
	Lepra::ScopeLock lTickLock(GetTickLock());
	Lepra::ScopeLock lNetLock(GetNetworkAgent()->GetLock());
	return (mAccountClientTable.GetCount());
}



Lepra::String GameServerManager::GetName() const
{
	return (_T("Server"));
}

Lepra::String GameServerManager::GetSystemCommandFilename() const
{
	return (Lepra::SystemManager::GetLoginName()+GetName()+(_T("System") _TEXT_ALTERNATIVE("", L"U") _T(".LifeBatch")));
}

Lepra::String GameServerManager::GetApplicationCommandFilename() const
{
	return (Lepra::SystemManager::GetLoginName()+GetName()+(_T("Application") _TEXT_ALTERNATIVE("", L"U") _T(".LifeBatch")));
}



void GameServerManager::Logout(Cure::UserAccount::AccountId pAccountId, const Lepra::String& pReason)
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



void GameServerManager::ContextObjectLoadCallback(UserContextObjectInfoResource* pResource)
{
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		Cure::ContextObject* lObject = pResource->GetData();
		GetContext()->AddObject(lObject);
		GetContext()->EnableTickCallback(lObject);
	}
	else
	{
		mLog.Errorf(_T("Problem loading context object %s."), pResource->GetName().c_str());
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
			const Lepra::UnicodeString lUserName = Lepra::UnicodeStringUtility::Format(L"Ball%i", x);
			Lepra::UnicodeString lReadablePassword(L"BallPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName,
					Lepra::StringUtility::IntToString(x, 10) + _T(":sphere_002"));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const Lepra::UnicodeString lUserName = Lepra::UnicodeStringUtility::Format(L"Car%i", x);
			Lepra::UnicodeString lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName,
					Lepra::StringUtility::IntToString(x, 10) + Cure::UserAccount::AvatarId(_T(":car_001")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const Lepra::UnicodeString lUserName = Lepra::UnicodeStringUtility::Format(L"Monster%i", x);
			Lepra::UnicodeString lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName,
					Lepra::StringUtility::IntToString(x, 10) + Cure::UserAccount::AvatarId(_T(":monster_001")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const Lepra::UnicodeString lUserName = Lepra::UnicodeStringUtility::Format(L"Excavator%i", x);
			Lepra::UnicodeString lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName,
					Lepra::StringUtility::IntToString(x, 10) + Cure::UserAccount::AvatarId(_T(":excavator_703")));
			}
		}
		for (x = 0; lOk && x < 100; ++x)
		{
			const Lepra::UnicodeString lUserName = Lepra::UnicodeStringUtility::Format(L"Crane%i", x);
			Lepra::UnicodeString lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				lOk = mUserAccountManager->AddUserAvatarId(lUserName,
					Lepra::StringUtility::IntToString(x, 10) + Cure::UserAccount::AvatarId(_T(":crane_whatever")));
			}
		}
	}

	if (lOk)
	{
		mBoxResource.Load(GetResourceManager(), _T("x:box_002"),
			UserContextObjectInfoResource::TypeLoadCallback(this, &GameServerManager::ContextObjectLoadCallback));
	}

	Lepra::String lAcceptAddress = CURE_RTVAR_GETSET(GetVariableScope(), "Login.AcceptAddress", _T(":16650"));
	if (lOk)
	{
		Lepra::SocketAddress lAddress;
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
	mTerrainResource.Load(GetResourceManager(), _T("ground_002"),
		UserContextObjectInfoResource::TypeLoadCallback(this, &GameServerManager::ContextObjectLoadCallback));
	return (true);
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
					const size_t lMessageCount = lPacket->GetMessageCount();
					for (size_t x = 0; x < lMessageCount; ++x)
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
			Lepra::int32 lClientFrameIndex = lMovement->GetFrameIndex();
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
					Lepra::StringUtility::ToCurrentCode(pClient->GetUserConnection()->GetLoginName()).c_str(),
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
	Lepra::ScopeLock lLock(GetNetworkAgent()->GetLock());
	return (mUserAccountManager->GetUserAccountStatus(pLoginId, pAccountId));
}

void GameServerManager::OnLogin(Cure::UserConnection* pUserConnection)
{
	Lepra::ScopeLock lLock(GetNetworkAgent()->GetLock());

	Client* lClient = GetClientByAccount(pUserConnection->GetAccountId());
	assert(!lClient);

	const Cure::UserAccount::AvatarIdSet* lAvatarIdSet = mUserAccountManager->GetUserAvatarIdSet(pUserConnection->GetLoginName());
	if (lAvatarIdSet && !lAvatarIdSet->empty())
	{
		lClient = new Client(GetNetworkAgent(), pUserConnection);
		mAccountClientTable.Insert(pUserConnection->GetAccountId(), lClient);
		lClient->SendPhysicsFrame(GetTimeManager()->GetCurrentPhysicsFrame());

		const Cure::UserAccount::AvatarId& lAvatarId = *lAvatarIdSet->begin();
		mLog.Info(_T("Loading avatar '")+lAvatarId+_T("' for user ")+Lepra::UnicodeStringUtility::ToCurrentCode(lClient->GetUserConnection()->GetLoginName())+_T("."));
		lClient->GetAvatarResource()->GetExtraData().mContextManager = GetContext();
		lClient->GetAvatarResource()->GetExtraData().mInstanceId = GetContext()->AllocateGameObjectId(lClient->GetAvatarResource()->GetExtraData().mNetworkType);
		lClient->GetAvatarResource()->Load(GetResourceManager(), lAvatarId,
			UserContextObjectAccountInfoResource::TypeLoadCallback(this, &GameServerManager::AvatarLoadCallback));
	}
	else
	{
		mLog.Error(_T("User ") + Lepra::UnicodeStringUtility::ToCurrentCode(pUserConnection->GetLoginName()) + _T(" does not exist or has no avatar!"));
	}
}

void GameServerManager::OnLogout(Cure::UserConnection* pUserConnection)
{
	Lepra::ScopeLock lTickLock(GetTickLock());
	Lepra::ScopeLock lNetLock(GetNetworkAgent()->GetLock());

	// TODO: logout with some timer, and also be able to reconnect the
	// client with his/her avatar again if logged in within the time frame.
	Client* lClient = GetClientByAccount(pUserConnection->GetAccountId());
	assert(lClient);
	Cure::GameObjectId lAvatarId = lClient->GetAvatarId();
	assert(IsThreadSafe());
	mAccountClientTable.Remove(pUserConnection->GetAccountId());
	delete (lClient);
	BroadcastDeleteObject(lAvatarId);

	mLog.Info(_T("User ") + Lepra::UnicodeStringUtility::ToCurrentCode(pUserConnection->GetLoginName()) + _T(" logged out."));
}



void GameServerManager::AdjustClientSimulationSpeed(Client* pClient, int pClientFrameIndex)
{
	int lCurrentFrameDiff = GetTimeManager()->GetCurrentPhysicsFrame() -  pClientFrameIndex;
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
		//mLog.Warningf(_T("Skipping store of movement (%i frames ahead)."), lFrameOffset);
	}
}

void GameServerManager::ApplyStoredMovement()
{
	// Walk through all steps (including missed steps due to slow computer).
	int lCurrentPhysicsSteps = GetTimeManager()->GetCurrentPhysicsStepCount();
	if (lCurrentPhysicsSteps >= NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE)
	{
		mLog.Errorf(_T("Too small network positional buffer: had to skip %i steps!"), lCurrentPhysicsSteps-NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE+1);
		lCurrentPhysicsSteps = NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE-1;
	}
	int lCurrentPhysicsFrame = GetTimeManager()->GetCurrentPhysicsFrame() - lCurrentPhysicsSteps;
	for (; lCurrentPhysicsFrame <= GetTimeManager()->GetCurrentPhysicsFrame(); ++lCurrentPhysicsFrame)
	{
		const int lFrameCycleIndex = lCurrentPhysicsFrame%NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE;
		MovementList lMovementList = mMovementArrayList[lFrameCycleIndex];
		mMovementArrayList[lFrameCycleIndex].clear();
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



void GameServerManager::AvatarLoadCallback(UserContextObjectAccountInfoResource* pResource)
{
	Cure::UserAccount::AccountId lAccountId = pResource->GetExtraData().mAccountId;
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		Client* lClient = GetClientByAccount(lAccountId);
		if (lClient)
		{
			Cure::ContextObject* lObject = pResource->GetData();
			Cure::GameObjectId lInstanceId = lObject->GetInstanceId();
			lClient->SetAvatarId(lInstanceId);
			GetContext()->AddObject(lObject);
			GetContext()->EnableTickCallback(lObject);
			mLog.Info(_T("User ")+Lepra::UnicodeStringUtility::ToCurrentCode(lClient->GetUserConnection()->GetLoginName())+_T(" login complete (avatar loaded)."));

			// TODO: this is hard-coded. Use a general replication-mechanism instead (where visible and added/updated objects gets replicated automatically).
			Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
			GetNetworkAgent()->SendNumberMessage(true, lClient->GetUserConnection()->GetSocket(),
				Cure::MessageNumber::INFO_AVATAR, lInstanceId, 0);
			GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

			BroadcastCreateObject(lObject);
			SendCreateAllObjects(lClient);
		}
		else
		{
			// The client has logged out during load, we need to dispose of context object.
			mLog.Errorf(_T("User logged out while loading avatar resource? AccountId=%u. TODO: implement!"), lAccountId);
			// TODO: what? Why hasn't this resource been dropped before we end up here?
		}
	}
	else
	{
		Logout(lAccountId, _T("Error when loading avatar."));
	}
}

void GameServerManager::OnCollision(const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2)
{
	if (pObject1 != pObject2 &&	// I.e. car where a wheel collides with the body.
		pObject1->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)	// We only handle network object collisions.
	{
		// Are two dynamic objects colliding?
		float lForceSquare = pForce.GetLengthSquared();
		float lTorqueSquare = pTorque.GetLengthSquared();
		bool lSendCollision;
		bool lAreBothDynamic = (pObject2 != 0 && pObject2->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY);
		if (lAreBothDynamic)
		{
			// Are two dynamic object colliding with high enough force (threshold keeps trafic low).
			lSendCollision = (lForceSquare > 400.0f || lTorqueSquare > 800.0f);
			if (lSendCollision)
			{
				log_volatile(mLog.Tracef(_T("Collision between dynamic objects. f^2=%f, t^2=%f."), lForceSquare, lTorqueSquare));
			}
		}
		else
		{
			// Is a server controlled object colliding with a static object with high enough force?
			lSendCollision = (pObject1->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED &&
				(lForceSquare > 400.0f || lTorqueSquare > 800.0f));
			if (lSendCollision)
			{
				log_volatile(mLog.Tracef(_T("Collision with static object. f^2=%f, t^2=%f."), lForceSquare, lTorqueSquare));
			}
		}
		if (lSendCollision)
		{
			// We have found a collision. Inform all viewers, including the colliding client.
			log_adebug("Sending collision.");
			const Cure::ObjectPositionalData* lPosition = 0;
			if (pObject1->UpdateFullPosition(lPosition))
			{
				BroadcastObjectPosition(pObject1->GetInstanceId(), *lPosition, 0, true);
			}
		}
	}
}

void GameServerManager::OnStopped(Cure::ContextObject* pObject)
{
	if (pObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		log_volatile(mLog.Debugf(_T("Object %u stopped, sending position."), pObject->GetInstanceId()));
		const Cure::ObjectPositionalData* lPosition = 0;
		if (pObject->UpdateFullPosition(lPosition))
		{
			BroadcastObjectPosition(pObject->GetInstanceId(), *lPosition, 0, true);
		}
	}
}



void GameServerManager::BroadcastCreateObject(Cure::ContextObject* pObject)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageCreateObject* lCreate = (Cure::MessageCreateObject*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OBJECT);
	lPacket->AddMessage(lCreate);
	Lepra::StringUtility::StringVector lInstanceClass = Lepra::StringUtility::Split(pObject->GetClassId(), _T(":"), 1);
	assert(lInstanceClass.size() == 2);
	assert(lInstanceClass[1].find(_T(":")) == Lepra::String::npos);
	lCreate->Store(lPacket, pObject->GetInstanceId(), Lepra::UnicodeStringUtility::ToOwnCode(lInstanceClass[1]));
	AccountClientTable::Iterator x = mAccountClientTable.First();
	for (; x != mAccountClientTable.End(); ++x)
	{
		const Client* lClient = x.GetObject();
		GetNetworkAgent()->PlaceInSendBuffer(true, lClient->GetUserConnection()->GetSocket(), lPacket);
	}
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
		GetNetworkAgent()->PlaceInSendBuffer(true, lClient->GetUserConnection()->GetSocket(), lPacket);
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
			Lepra::StringUtility::StringVector lInstanceClass = Lepra::StringUtility::Split(lObject->GetClassId(), _T(":"), 1);
			assert(lInstanceClass.size() == 2);
			assert(lInstanceClass[1].find(_T(":")) == Lepra::String::npos);
			lCreateMessage->Store(lPacket, lObject->GetInstanceId(),
				Lepra::UnicodeStringUtility::ToOwnCode(lInstanceClass[1]));

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

	Lepra::ScopeLock lTickLock(GetTickLock());
	AccountClientTable::Iterator x = mAccountClientTable.First();
	for (; x != mAccountClientTable.End(); ++x)
	{
		const Client* lClient = x.GetObject();
		if (lClient != pExcludeClient)
		{
			//mLog.Debugf(_T("Sending position (%f; %f; %f)."), pPosition.mTransformation.GetPosition().x,
			//	pPosition.mTransformation.GetPosition().y, pPosition.mTransformation.GetPosition().z);
			GetNetworkAgent()->PlaceInSendBuffer(pSafe, lClient->GetUserConnection()->GetSocket(), lPacket);
		}
	}

	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

Cure::NetworkServer* GameServerManager::GetNetworkServer() const
{
	return ((Cure::NetworkServer*)GetNetworkAgent());
}



LOG_CLASS_DEFINE(GAME, GameServerManager);



}
