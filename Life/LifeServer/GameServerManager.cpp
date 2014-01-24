
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "GameServerManager.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/CppContextObject.h"
#include "../../Cure/Include/ContextObjectAttribute.h"
#include "../../Cure/Include/Elevator.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Path.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsSpawner.h"
#include "../LifeMaster/MasterServer.h"
#include "../LifeApplication.h"
#include "../LifeString.h"
#include "../Spawner.h"
#include "BulletTime.h"
#include "MasterServerConnection.h"
#include "RaceTimer.h"
#include "RtVar.h"
#include "ServerConsoleManager.h"
#include "ServerDelegate.h"
#include "ServerMessageProcessor.h"



namespace Life
{



const int NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE = PHYSICS_FPS/2;



GameServerManager::GameServerManager(const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Parent(pTime, pVariableScope, pResourceManager),
	mUserAccountManager(new Cure::MemoryUserAccountManager()),
	mDelegate(0),
	mMessageProcessor(0),
	mMovementArrayList(NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE),
	mMasterConnection(0),
	mPhysicsFpsShadow(24),
	mPhysicsRtrShadow(1),
	mPhysicsHaltShadow(false)
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_AUTOFLIPENABLED, true);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_LOGINGREETING, _("echo 4 \"Welcome to my server! Enjoy the ride!\""));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_SERVERNAME, strutil::Format(_("%s's server"), SystemManager::GetLoginName().c_str()));
	CURE_RTVAR_SET(Cure::GetSettings(), RTVAR_NETWORK_PUBLISHSERVER, false);	// Make it overridable and possible to change between dedicated/local by using Cure::GetSettings().

	SetNetworkAgent(new Cure::NetworkServer(pVariableScope, this));
}

GameServerManager::~GameServerManager()
{
	if (mMasterConnection)
	{
		if (!mMasterConnection->CloseUnlessUploaded())
		{
			bool lSkipQuitForNow = (SystemManager::GetQuitRequest() == 1);
			if (lSkipQuitForNow)
			{
				SystemManager::AddQuitRequest(-1);
			}
			mLog.Info(_T("Unregistering server with master. Might take a few secs..."));
			mMasterConnection->AppendLocalInfo(_T(" --remove true"));
			mMasterConnection->WaitUntilDone(10.0, true);
			mMasterConnection->GraceClose(1.0, false);
			mMasterConnection = 0;	// Not owned by us, deleted elsewhere.
			if (lSkipQuitForNow)
			{
				SystemManager::AddQuitRequest(+1);
			}
		}
	}

	DeleteAllClients();

	delete mMessageProcessor;
	mMessageProcessor = 0;
	delete mDelegate;
	mDelegate = 0;

	delete (mUserAccountManager);
	mUserAccountManager = 0;

	if (GetConsoleManager())
	{
		GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ") + Application::GetIoFile(_T("ServerApplication"), _T("lsh")));
	}
}



bool GameServerManager::BeginTick()
{
	AccountClientTable::Iterator x = mAccountClientTable.First();
	for (; x != mAccountClientTable.End(); ++x)
	{
		const Client* lClient = x.GetObject();
		Cure::CppContextObject* lObject = (Cure::CppContextObject*)GetContext()->GetObject(lClient->GetAvatarId());
		if (!lObject)
		{
			continue;
		}
		if (lObject->IsAttributeTrue(_T("float_childishness")) ||
			lObject->GetGuideMode() == TBC::ChunkyPhysics::GUIDE_ALWAYS)
		{
			lObject->StabilizeTick();
		}
	}

	return Parent::BeginTick();
}

void GameServerManager::PreEndTick()
{
	Parent::PreEndTick();
	mDelegate->PreEndTick();
}



void GameServerManager::StartConsole(InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt)
{
	if (!GetConsoleManager())
	{
		SetConsoleManager(new ServerConsoleManager(GetResourceManager(), this, GetVariableScope(), pConsoleLogger, pConsolePrompt));
	}
	GetConsoleManager()->PushYieldCommand(_T("execute-file -i ") + Application::GetIoFile(_T("ServerApplication"), _T("lsh")));
	GetConsoleManager()->Start();
}

bool GameServerManager::Initialize(MasterServerConnection* pMasterConnection, const str& pAddress)
{
	bool lOk = true;

	if (lOk)
	{
		strutil::strvec lFileArray;
		int x;
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"User%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				if (lFileArray.empty())
				{
					lFileArray = GetResourceManager()->ListFiles(_T("*.class"));
				}
				strutil::strvec::const_iterator x = lFileArray.begin();
				for (; lOk && x != lFileArray.end(); ++x)
				{
					if (x->find(_T("level_")) != str::npos ||
						x->find(_T("road_sign")) != str::npos)
					{
						continue;
					}
					Cure::UserAccount::AvatarId lId(Path::GetFileBase(*x));
					lOk = mUserAccountManager->AddUserAvatarId(lUserName, lId);
				}
			}
			deb_assert(!mUserAccountManager->GetUserAvatarIdSet(lUserName)->empty());
		}
	}

	str lAcceptAddress = pAddress;
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
	if (lOk)
	{
		mMasterConnection = pMasterConnection;
		TickMasterServer();
		mDelegate->OnOpen();
	}
	return (lOk);
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
	else if (mPowerSaveTimer.GetTimeDiff() < 10.0)
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



#ifdef LEPRA_DEBUG
TBC::PhysicsManager* GameServerManager::GetPhysicsManager() const
{
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner() ||
		(GetNetworkAgent()->GetLock()->IsOwner() && GetTickLock()->IsOwner()));
	return Parent::GetPhysicsManager();
}
#endif // Debug mode



void GameServerManager::DeleteContextObject(Cure::GameObjectId pInstanceId)
{
	deb_assert(GetTickLock()->IsOwner());

	Cure::ContextObject* lObject = GetContext()->GetObject(pInstanceId);
	mDelegate->OnDeleteObject(lObject);
	if (lObject && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		BroadcastDeleteObject(pInstanceId);
	}
	DeleteMovements(pInstanceId);
	GetContext()->DeleteObject(pInstanceId);
}



ServerDelegate* GameServerManager::GetDelegate() const
{
	return mDelegate;
}

void GameServerManager::SetDelegate(ServerDelegate* pDelegate)
{
	deb_assert(!mDelegate);
	mDelegate = pDelegate;
}

void GameServerManager::SetMessageProcessor(ServerMessageProcessor* pMessageProcessor)
{
	deb_assert(!mMessageProcessor);
	mMessageProcessor = pMessageProcessor;
}

void GameServerManager::AdjustClientSimulationSpeed(Client* pClient, int pClientFrameIndex)
{
	const int lCurrentFrameDiff = GetTimeManager()->GetCurrentPhysicsFrameDelta(pClientFrameIndex);
	// Calculate client network latency and jitter.
	pClient->StoreFrameDiff(lCurrentFrameDiff);

	// Send physics enginge speed-up of slow-down to client if necessary.
	pClient->QuerySendStriveTimes();
}

void GameServerManager::StoreMovement(int pClientFrameIndex, Cure::MessageObjectMovement* pMovement)
{
	const int lCurrentPhysicsFrame = GetTimeManager()->GetCurrentPhysicsFrame();
	const int lFrameOffset = GetTimeManager()->GetPhysicsFrameDelta(pClientFrameIndex, lCurrentPhysicsFrame);
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
		log_volatile(mLog.Debugf(_T("Skipping store of movement (%i frames ahead)."), lFrameOffset));
	}
}

void GameServerManager::OnSelectAvatar(Client* pClient, const Cure::UserAccount::AvatarId& pAvatarId)
{
	ScopeLock lTickLock(GetTickLock());
	mDelegate->OnSelectAvatar(pClient, pAvatarId);
}

void GameServerManager::LoanObject(Client* pClient, Cure::GameObjectId pInstanceId)
{
	Cure::ContextObject* lObject = GetContext()->GetObject(pInstanceId);
	if (lObject)
	{
		if (lObject->GetBorrowerInstanceId() == pClient->GetAvatarId() ||	// Reloan?
			(lObject->GetBorrowerInstanceId() == 0 &&
			lObject->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED))
		{
			if (mDelegate->IsObjectLendable(pClient, lObject))
			{
				const float lClientOwnershipTime = 10.0f;
				const float lServerOwnershipTime = lClientOwnershipTime * 1.3f;
				int lEndFrame = GetTimeManager()->GetCurrentPhysicsFrameAddSeconds(lClientOwnershipTime);
				lObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
				GetNetworkServer()->SendNumberMessage(true, pClient->GetUserConnection()->GetSocket(),
					Cure::MessageNumber::INFO_GRANT_LOAN, pInstanceId, (float)lEndFrame);
				lObject->SetBorrowerInstanceId(pClient->GetAvatarId());
				GetContext()->CancelPendingAlarmCallbacksById(lObject, Cure::ContextManager::SYSTEM_ALARM_ID_OWNERSHIP_LOAN_EXPIRES);
				GetContext()->AddAlarmCallback(lObject, Cure::ContextManager::SYSTEM_ALARM_ID_OWNERSHIP_LOAN_EXPIRES, lServerOwnershipTime, 0);
			}
		}
	}
	else
	{
		mLog.Warningf(_T("User %s requested ownership of object with ID %i (just unloaded?)."), pClient->GetUserConnection()->GetLoginName().c_str(), pInstanceId);
	}
}

wstrutil::strvec GameServerManager::ListUsers()
{
	wstrutil::strvec lVector;
	{
		deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
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

Cure::NetworkServer* GameServerManager::GetNetworkServer() const
{
	return ((Cure::NetworkServer*)GetNetworkAgent());
}

void GameServerManager::SendObjects(Client* pClient, bool pCreate, const ContextTable& pObjectTable)
{
	// TODO: restrict to visible, concrete objects.

	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageCreateObject* lCreateMessage = 0;
	Cure::MessageCreateOwnedObject* lCreateOwnedMessage = 0;
	if (pCreate)
	{
		lCreateMessage = (Cure::MessageCreateObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OBJECT);
		lCreateOwnedMessage = (Cure::MessageCreateOwnedObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OWNED_OBJECT);
	}
	Cure::MessageObjectPosition* lPositionMessage = (Cure::MessageObjectPosition*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_POSITION);

	ContextTable::const_iterator x = pObjectTable.begin();
	for (; x != pObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		// Don't send local objects.
		if (lObject->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			continue;
		}

		lPacket->Clear();

		if (pCreate)	// Store creation info?
		{
			TransformationF lTransformation(lObject->GetOrientation(), lObject->GetPosition());
			if (lObject->GetOwnerInstanceId() != 0)
			{
				lPacket->AddMessage(lCreateOwnedMessage);
				lCreateOwnedMessage->Store(lPacket, lObject->GetInstanceId(), lTransformation,
					wstrutil::Encode(lObject->GetClassId()), lObject->GetOwnerInstanceId());
			}
			else
			{
				lPacket->AddMessage(lCreateMessage);
				lCreateMessage->Store(lPacket, lObject->GetInstanceId(),
					 lTransformation, wstrutil::Encode(lObject->GetClassId()));
			}
		}

		const Cure::ObjectPositionalData* lPosition;
		if (lObject->UpdateFullPosition(lPosition))
		{
			lPacket->AddMessage(lPositionMessage);
			lPositionMessage->Store(lPacket, lObject->GetInstanceId(),
				GetTimeManager()->GetCurrentPhysicsFrame(), *lPosition);
		}

		// Send.
		GetNetworkAgent()->PlaceInSendBuffer(true, pClient->GetUserConnection()->GetSocket(), lPacket);
	}

	lPacket->Clear();
	if (pCreate)	// Sent creation info?
	{
		GetNetworkAgent()->GetPacketFactory()->GetMessageFactory()->Release(lCreateOwnedMessage);
		GetNetworkAgent()->GetPacketFactory()->GetMessageFactory()->Release(lCreateMessage);
	}
	GetNetworkAgent()->GetPacketFactory()->GetMessageFactory()->Release(lPositionMessage);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

void GameServerManager::BroadcastCreateObject(Cure::GameObjectId pInstanceId, const TransformationF& pTransform, const str& pClassId, Cure::GameObjectId pOwnerInstanceId)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	if (pOwnerInstanceId)
	{
		Cure::MessageCreateOwnedObject* lCreate = (Cure::MessageCreateOwnedObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OWNED_OBJECT);
		lPacket->AddMessage(lCreate);
		lCreate->Store(lPacket, pInstanceId, pTransform, wstrutil::Encode(pClassId), pOwnerInstanceId);
	}
	else
	{
		Cure::MessageCreateObject* lCreate = (Cure::MessageCreateObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OBJECT);
		lPacket->AddMessage(lCreate);
		lCreate->Store(lPacket, pInstanceId, pTransform, wstrutil::Encode(pClassId));
	}
	BroadcastPacket(0, lPacket, true);
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

	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
	ScopeLock lTickLock(GetTickLock());
	BroadcastPacket(pExcludeClient, lPacket, pSafe);

	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

bool GameServerManager::BroadcastChatMessage(const wstr& pMessage)
{
	return BroadcastStatusMessage(Cure::MessageStatus::INFO_CHAT, pMessage);
}

bool GameServerManager::BroadcastStatusMessage(Cure::MessageStatus::InfoType pType, const wstr& pString)
{
	bool lOk = false;
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	{
		deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
		ScopeLock lTickLock(GetTickLock());
		ScopeLock lNetLock(GetNetworkAgent()->GetLock());
		AccountClientTable::Iterator x = mAccountClientTable.First();
		for (; x != mAccountClientTable.End(); ++x)
		{
			const Client* lClient = x.GetObject();
			lOk |= GetNetworkAgent()->SendStatusMessage(lClient->GetUserConnection()->GetSocket(), 0,
				Cure::REMOTE_OK, pType, pString, lPacket);
		}
	}
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
	return (lOk);
}

void GameServerManager::BroadcastNumberMessage(Client* pExcludeClient, bool pSafe, Cure::MessageNumber::InfoType pInfo, int32 pInteger, float32 pFloat)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageNumber* lNumber = (Cure::MessageNumber*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_NUMBER);
	lPacket->AddMessage(lNumber);
	lNumber->Store(lPacket, pInfo, pInteger, pFloat);
	BroadcastPacket(pExcludeClient, lPacket, pSafe);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

bool GameServerManager::SendChatMessage(const wstr& pClientUserName, const wstr& pMessage)
{
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
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
		lOk = GetNetworkAgent()->SendStatusMessage(lClient->GetUserConnection()->GetSocket(), 0, Cure::REMOTE_OK,
			Cure::MessageStatus::INFO_CHAT, pMessage, lPacket);
		GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
	}
	return (lOk);
}

void GameServerManager::IndicatePosition(const Vector3DF pPosition, float pTime)
{
	if (pTime <= 0)
	{
		return;
	}
	Cure::ContextObject* lObject = Parent::CreateContextObject(_T("indicator"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	lObject->SetInitialTransform(TransformationF(gIdentityQuaternionF, pPosition));
	lObject->StartLoading();
	DeleteContextObjectDelay(lObject, pTime);
}



int GameServerManager::GetLoggedInClientCount() const
{
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner() || GetTickLock()->IsOwner());
	ScopeLock lTickLock(GetTickLock());
	ScopeLock lNetLock(GetNetworkAgent()->GetLock());
	return (mAccountClientTable.GetCount());
}

Client* GameServerManager::GetClientByAccount(Cure::UserAccount::AccountId pAccountId) const
{
	Client* lClient = mAccountClientTable.FindObject(pAccountId);
	return (lClient);
}

Client* GameServerManager::GetClientByObject(Cure::ContextObject*& pObject) const
{
	Client* lClient = 0;
	{
		Cure::UserAccount::AccountId lAccountId = (Cure::UserAccount::AccountId)(intptr_t)pObject->GetExtraData();
		if (lAccountId != 0 && lAccountId != (Cure::UserAccount::AccountId)-1)
		{
			lClient = GetClientByAccount(lAccountId);
			if (!lClient)
			{
				mLog.Errorf(_T("Error: client seems to have logged off before avatar %s got loaded."),
					pObject->GetClassId().c_str());
				delete (pObject);
				pObject = 0;
			}
		}
	}
	return lClient;
}

const GameServerManager::AccountClientTable& GameServerManager::GetAccountClientTable() const
{
	return mAccountClientTable;
}


void GameServerManager::Build(const str& pWhat)
{
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
	ScopeLock lTickLock(GetTickLock());
	ScopeLock lNetLock(GetNetworkAgent()->GetLock());

	for (int x = 0; x < 400 && !IsThreadSafe(); ++x)
	{
		if (x > 3) lNetLock.Release();
		if (x > 7) lTickLock.Release();
		Thread::Sleep(0.005);
		if (x > 7) lTickLock.Acquire();
		if (x > 3) lNetLock.Acquire();
	}
	if (!IsThreadSafe())
	{
		mLog.AError("Could never reach a thread-safe slice. Aborting construction.");
		return;
	}

	AccountClientTable::Iterator x = mAccountClientTable.First();
	for (; x != mAccountClientTable.End(); ++x)
	{
		const Client* lClient = x.GetObject();
		Cure::ContextObject* lObject = GetContext()->GetObject(lClient->GetAvatarId());
		if (lObject)
		{
			Vector3DF lPosition = lObject->GetPosition() + Vector3DF(10, 0, 0);
			mLog.Info(_T("Building object '")+pWhat+_T("' near user ")+strutil::Encode(lClient->GetUserConnection()->GetLoginName())+_T("."));
			Cure::ContextObject* lObject = Parent::CreateContextObject(pWhat, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
			lObject->SetInitialTransform(TransformationF(gIdentityQuaternionF, lPosition));
			lObject->StartLoading();
		}
	}
}



void GameServerManager::TickInput()
{
	TickMasterServer();
	MonitorRtvars();

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
				deb_assert (lClient);
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
						mMessageProcessor->ProcessNetworkInputMessage(lClient, lMessage);
					}
					lParseResult = lPacket->ParseMore();
					if (lParseResult == Cure::Packet::PARSE_SHIFT)
					{
						lReceived = GetNetworkServer()->ReceiveMore(lAccountId, lPacket);
						if (lReceived == Cure::NetworkAgent::RECEIVE_OK)
						{
							lParseResult = lPacket->ParseMore();
						}
						else
						{
							lParseResult = Cure::Packet::PARSE_ERROR;
						}
					}
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



Cure::UserAccount::Availability GameServerManager::QueryLogin(const Cure::LoginId& pLoginId, Cure::UserAccount::AccountId& pAccountId)
{
	ScopeLock lLock(GetNetworkAgent()->GetLock());
	return (mUserAccountManager->GetUserAccountStatus(pLoginId, pAccountId));
}

void GameServerManager::OnLogin(Cure::UserConnection* pUserConnection)
{
	ScopeLock lTickLock(GetTickLock());
	ScopeLock lLock(GetNetworkAgent()->GetLock());

	Client* lClient = GetClientByAccount(pUserConnection->GetAccountId());
	deb_assert(!lClient);
	if (lClient)
	{
		mLog.Errorf(_T("user %s already has an account!"), pUserConnection->GetLoginName().c_str());
		return;
	}

	typedef Cure::UserAccount::AvatarIdSet AvatarIdSet;
	const AvatarIdSet* lAvatarIdSet = mUserAccountManager->GetUserAvatarIdSet(pUserConnection->GetLoginName());
	if (lAvatarIdSet)
	{
		Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();

		lClient = new Client(GetTimeManager(), GetNetworkAgent(), pUserConnection);
		mAccountClientTable.Insert(pUserConnection->GetAccountId(), lClient);
		lClient->SendPhysicsFrame(GetTimeManager()->GetCurrentPhysicsFrameAddFrames(2), lPacket);	// TODO: adjust physics frame diff by ping-ponging some.
		str lServerGreeting;
		CURE_RTVAR_GET(lServerGreeting, =, GetVariableScope(), RTVAR_NETWORK_LOGINGREETING, _T(""));
		lClient->SendLoginCommands(lPacket, lServerGreeting);

		for (AvatarIdSet::const_iterator x = lAvatarIdSet->begin(); x != lAvatarIdSet->end(); ++x)
		{
			const Cure::UserAccount::AvatarId& lAvatarId = *x;
			lClient->SendAvatar(lAvatarId, lPacket);
		}

		GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

		SendObjects(lClient, true, GetContext()->GetObjectTable());

		mDelegate->OnLogin(lClient);
	}
	else
	{
		mLog.Error(_T("User ") + strutil::Encode(pUserConnection->GetLoginName()) + _T(" does not exist or is not allowed avatars!"));
	}
}

void GameServerManager::OnLogout(Cure::UserConnection* pUserConnection)
{
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
	Cure::GameObjectId lAvatarId = 0;
	ScopeLock lTickLock(GetTickLock());
	{
		ScopeLock lNetLock(GetNetworkAgent()->GetLock());

		// TODO: logout with some timer, and also be able to reconnect the
		// client with his/her avatar again if logged in within the time frame.
		Client* lClient = GetClientByAccount(pUserConnection->GetAccountId());
		deb_assert(lClient);
		lAvatarId = lClient->GetAvatarId();
		deb_assert(IsThreadSafe());
		mAccountClientTable.Remove(pUserConnection->GetAccountId());
		mDelegate->OnLogout(lClient);
		delete (lClient);
	}
	DeleteContextObject(lAvatarId);

	mLog.Info(_T("User ") + strutil::Encode(pUserConnection->GetLoginName()) + _T(" logged out."));
}



void GameServerManager::DeleteMovements(Cure::GameObjectId pInstanceId)
{
	for (size_t x = 0; x < (size_t)NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE; ++x)
	{
		MovementList& lList = mMovementArrayList[x];
		MovementList::iterator y = lList.begin();
		for (; y != lList.end(); ++y)
		{
			if ((*y)->GetObjectId() == pInstanceId)
			{
				delete (*y);
				lList.erase(y);
				break;
			}
		}
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
	int lCurrentPhysicsFrame = GetTimeManager()->GetCurrentPhysicsFrameAddFrames(-lCurrentPhysicsSteps);
	if (lCurrentPhysicsFrame < 0)
	{
		return;
	}
	//mLog.Debugf(_T("[frame %i to %i]"), lCurrentPhysicsFrame, GetTimeManager()->GetCurrentPhysicsFrame());
	for (; GetTimeManager()->GetCurrentPhysicsFrameDelta(lCurrentPhysicsFrame) >= 0;
		lCurrentPhysicsFrame = GetTimeManager()->GetPhysicsFrameAddFrames(lCurrentPhysicsFrame, 1))
	{
		const int lFrameCycleIndex = lCurrentPhysicsFrame%NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE;
		MovementList lMovementList = mMovementArrayList[lFrameCycleIndex];
		LEPRA_DEBUG_CODE(size_t lMovementCount = lMovementList.size();)
		mMovementArrayList[lFrameCycleIndex].clear();
		LEPRA_DEBUG_CODE(deb_assert(lMovementCount == lMovementList.size()));
		MovementList::iterator x = lMovementList.begin();
		for (; x != lMovementList.end(); ++x)
		{
			Cure::MessageObjectMovement* lMovement = *x;
			Cure::GameObjectId lInstanceId = lMovement->GetObjectId();
			/*if (lCurrentPhysicsFrame != lMovement->GetFrameIndex())
			{
				mLog.AWarning("Throwing away network movement.");
			}
			else*/
			{
				Cure::ContextObject* lContextObject = GetContext()->GetObject(lInstanceId);
				if (lContextObject)
				{
					if (lMovement->GetType() == Cure::MESSAGE_TYPE_OBJECT_POSITION)
					{
						const Cure::ObjectPositionalData& lData =
							((Cure::MessageObjectPosition*)lMovement)->GetPositionalData();
						lContextObject->SetFullPosition(lData, 0);
					}
					else
					{
						mLog.AError("Not implemented!");
					}
				}
			}
			delete lMovement;
		}
	}
}



void GameServerManager::BroadcastAvatar(Client* pClient)
{
	Cure::ContextObject* lObject = GetContext()->GetObject(pClient->GetAvatarId());
	Cure::GameObjectId lInstanceId = lObject->GetInstanceId();
	mLog.Info(_T("User ")+strutil::Encode(pClient->GetUserConnection()->GetLoginName())+_T(" login complete (avatar loaded)."));

	// TODO: this is hard-coded. Use a general replication-mechanism instead (where visible and added/updated objects gets replicated automatically).
	GetNetworkAgent()->SendNumberMessage(true, pClient->GetUserConnection()->GetSocket(),
		Cure::MessageNumber::INFO_AVATAR, lInstanceId, 0);

	BroadcastCreateObject(lObject);
}



Cure::ContextObject* GameServerManager::CreateContextObject(const str& pClassId) const
{
	return mDelegate->CreateContextObject(pClassId);
}

void GameServerManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	Client* lClient = GetClientByObject(pObject);
	if (!pObject)
	{
		return;
	}
	if (pOk)
	{
		DeleteMovements(pObject->GetInstanceId());
		if (lClient || pObject->GetExtraData() == (void*)-1L)
		{
			mDelegate->OnLoadAvatar(lClient, pObject);
			if (lClient)
			{
				mLog.Infof(_T("Loaded avatar for %s with instance id %i."),
					strutil::Encode(lClient->GetUserConnection()->GetLoginName()).c_str(),
					pObject->GetInstanceId());
				/*const QuaternionF q = pObject->GetOrientation();
				mLog.Infof(_T("Avatar %s/%i has q=(%f, %f, %f, %f)."),
					strutil::Encode(lClient->GetUserConnection()->GetLoginName()).c_str(),
					pObject->GetInstanceId(),
					q.a, q.b, q.c, q.d);*/
				BroadcastAvatar(lClient);
			}
			else
			{
				pObject->SetExtraData(0);
			}
		}
		else
		{
			log_volatile(mLog.Debugf(_T("Loaded object %s."), pObject->GetClassId().c_str()));
			mDelegate->OnLoadObject(pObject);
			BroadcastCreateObject(pObject);
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
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void GameServerManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	ScopeLock lLock(GetTickLock());

	(void)pPosition;
	(void)pBody1Id;
	(void)pBody2Id;

	const bool lObject1Dynamic = (pObject1->GetPhysics()->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC);
	const bool lObject2Dynamic = (pObject2->GetPhysics()->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC);
	const bool lBothAreDynamic = (lObject1Dynamic && lObject2Dynamic);
	if (!lBothAreDynamic)
	{
		if (lObject1Dynamic && pObject1->GetPhysics()->GetGuideMode() >= TBC::ChunkyPhysics::GUIDE_EXTERNAL)
		{
			FlipCheck(pObject1);
		}
		return;
	}

	if (pObject1 != pObject2 &&	// I.e. car where a wheel collides with the body.
		pObject1->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)	// We only handle network object collisions.
	{
		if (pObject1->GetBorrowerInstanceId() == pObject2->GetInstanceId() ||
			pObject2->GetBorrowerInstanceId() == pObject1->GetInstanceId())
		{
			return;	// The client knows best what collisions have happened.
		}

		bool lSendCollision = false;
		const bool lAreBothControlled = (pObject2 != 0 && pObject2->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY);
		const bool lIsServerControlled = (pObject1->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
		if (lIsServerControlled)
		{
			lSendCollision = (pObject1->GetImpact(GetPhysicsManager()->GetGravity(), pForce, pTorque) >= 2.0f);
		}
		else if (lAreBothControlled)
		{
			if (pObject1->GetBorrowerInstanceId() == pObject2->GetInstanceId() ||
				pObject2->GetBorrowerInstanceId() == pObject1->GetInstanceId())
			{
				// An avatar collides against an object that she owns, let remote end handle!
			}
			else
			{
				lSendCollision = (pObject1->GetImpact(GetPhysicsManager()->GetGravity(), pForce, pTorque) >= 2.0f);
			}
		}
		if (lSendCollision)
		{
			/*// We have found a collision. Asynchronously inform all viewers, including the colliding client.
			if (pObject1->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED)
			{
				pObject1->SetSendCount(3);
			}*/
			GetContext()->AddPhysicsSenderObject(pObject1);
		}
	}
}

void GameServerManager::FlipCheck(Cure::ContextObject* pObject) const
{
	bool lAutoFlipEnabled;
	CURE_RTVAR_GET(lAutoFlipEnabled, =, GetVariableScope(), RTVAR_GAME_AUTOFLIPENABLED, true);
	if (!lAutoFlipEnabled)
	{
		return;
	}

	// Check if we've landed on our side.
	Vector3DF lUp(0, 0, 1);
	lUp = pObject->GetOrientation() * lUp;
	if (lUp.z > 0.2f ||
		pObject->GetVelocity().GetLengthSquared() > 1*1 ||
		pObject->GetAngularVelocity().GetLengthSquared() > 1*1)
	{
		// Nope, still standing, or at least moving. Might be drunken style,
		// but at least not on it's head yet.
		return;
	}
	// A grown-up still activating an engine = leave 'em be.
	if (!pObject->IsAttributeTrue(_T("float_childishness")))
	{
		const int lEngineCount = pObject->GetPhysics()->GetEngineCount();
		for (int x = 0; x < lEngineCount; ++x)
		{
			if (::fabs(pObject->GetPhysics()->GetEngine(x)->GetValue()) > 0.6f)
			{
				return;
			}
		}
	}

	// Yup, reset vehicle in the direction it was heading.
	const Cure::ObjectPositionalData* lOriginalPositionData;
	if (pObject->UpdateFullPosition(lOriginalPositionData))
	{
		Cure::ObjectPositionalData lPositionData;
		lPositionData.CopyData(lOriginalPositionData);
		lPositionData.Stop();
		TransformationF& lTransform = lPositionData.mPosition.mTransformation;
		lTransform.SetPosition(pObject->GetPosition() + Vector3DF(0, 0, 5));
		Vector3DF lEulerAngles;
		pObject->GetOrientation().GetEulerAngles(lEulerAngles);
		lTransform.GetOrientation().SetEulerAngles(lEulerAngles.x, 0, 0);
		lTransform.GetOrientation() *= pObject->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		pObject->SetFullPosition(lPositionData, 0);
		GetContext()->AddPhysicsSenderObject(pObject);
	}
	else
	{
		mLog.Errorf(_T("Error: vehicle %i could not be flipped!"), pObject->GetInstanceId());
	}
}

bool GameServerManager::OnPhysicsSend(Cure::ContextObject* pObject)
{
	if (pObject->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		return true;
	}

	bool lLastSend = false;
	float lSendIntervalLimit;
	CURE_RTVAR_GET(lSendIntervalLimit, =(float), GetVariableScope(), RTVAR_NETPHYS_POSSENDINTERVALLIMIT, 0.5);
	if (pObject->QueryResendTime(lSendIntervalLimit, false))
	{
		lLastSend = true;
		log_volatile(mLog.Debugf(_T("Sending pos for %s (%u)."), pObject->GetClassId().c_str(), pObject->GetInstanceId()));
		const Cure::ObjectPositionalData* lPosition = 0;
		if (pObject->UpdateFullPosition(lPosition))
		{
			BroadcastObjectPosition(pObject->GetInstanceId(), *lPosition, 0, false);
			lLastSend = (pObject->PopSendCount() == 0);
		}

	}
	return (lLastSend);
}

bool GameServerManager::OnAttributeSend(Cure::ContextObject* pObject)
{
	log_adebug("Sending attribute(s) for a context object...");
	typedef Cure::ContextObject::AttributeArray AttributeArray;
	const AttributeArray& lAttributes = pObject->GetAttributes();
	AttributeArray::const_iterator x = lAttributes.begin();
	for (; x != lAttributes.end(); ++x)
	{
		Cure::ContextObjectAttribute* lAttribute = *x;
		const int lSendSize = lAttribute->QuerySend();
		if (lSendSize > 0)
		{
			Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
			Cure::MessageObjectAttribute* lAttribMessage = (Cure::MessageObjectAttribute*)GetNetworkAgent()->
				GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_ATTRIBUTE);
			lPacket->AddMessage(lAttribMessage);
			lAttribute->Pack(lAttribMessage->GetWriteBuffer(lPacket, pObject->GetInstanceId(), lSendSize));

			deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
			ScopeLock lTickLock(GetTickLock());
			switch (lAttribute->GetNetworkType())
			{
				case Cure::ContextObjectAttribute::TYPE_SERVER_BROADCAST:
				case Cure::ContextObjectAttribute::TYPE_BOTH_BROADCAST:
				{
					BroadcastPacket(0, lPacket, true);
				}
				break;
				default:
				{
					Cure::UserAccount::AccountId lAccountId = (Cure::UserAccount::AccountId)(intptr_t)pObject->GetExtraData();
					if (!lAccountId)
					{
						mLog.AError("Error: trying to attribute sync to avatar without avatar!");
						break;
					}
					const Client* lClient = GetClientByAccount(lAccountId);
					if (!lClient)
					{
						mLog.AError("Error: client seems to have logged off before attribute sync happened.");
						break;
					}
					Cure::NetworkAgent::VSocket* lSocket = lClient->GetUserConnection()->GetSocket();
					if (lSocket)
					{
						GetNetworkAgent()->PlaceInSendBuffer(true, lSocket, lPacket);
					}
				}
				break;
			}
			GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
		}
	}
	return true;
}

bool GameServerManager::IsServer()
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

void GameServerManager::OnAlarm(int pAlarmId, Cure::ContextObject* pObject, void* pExtraData)
{
	if (pAlarmId == Cure::ContextManager::SYSTEM_ALARM_ID_OWNERSHIP_LOAN_EXPIRES)
	{
		pObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
		pObject->SetBorrowerInstanceId(0);
	}
	else
	{
		Parent::OnAlarm(pAlarmId, pObject, pExtraData);
	}
}

void GameServerManager::HandleWorldBoundaries()
{
	std::vector<Cure::GameObjectId> lLostObjectArray;
	const ContextTable& lObjectTable = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (lObject->IsLoaded() && lObject->GetPhysics())
		{
			const Vector3DF lPosition = lObject->GetPosition();
			if (!Math::IsInRange(lPosition.x, -2000.0f, +2000.0f) ||
				!Math::IsInRange(lPosition.y, -2000.0f, +2000.0f) ||
				!Math::IsInRange(lPosition.z, -1000.0f, +800.0f))
			{
				lLostObjectArray.push_back(lObject->GetInstanceId());
			}
		}
	}
	if (!lLostObjectArray.empty())
	{
		ScopeLock lLock(GetTickLock());
		std::vector<Cure::GameObjectId>::const_iterator y = lLostObjectArray.begin();
		for (; y != lLostObjectArray.end(); ++y)
		{
			DeleteContextObject(*y);
		}
	}
}



Cure::ContextObject* GameServerManager::CreateLogicHandler(const str& pType)
{
	if (pType == _T("trig_elevator"))
	{
		return new Cure::Elevator(GetContext());
	}
	else if (pType == _T("spawner"))
	{
		return new Spawner(GetContext());
	}
	else if (pType == _T("real_time_ratio"))
	{
		return new BulletTime(GetContext());
	}
	else if (pType == _T("race_timer"))
	{
		return new RaceTimer(GetContext());
	}
	return (0);
}



void GameServerManager::BroadcastCreateObject(Cure::ContextObject* pObject)
{
	if (pObject->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		return;
	}

	bool lIsEngineControlled = false;
	TransformationF lTransform;
	if (pObject->GetPhysics())
	{
		TBC::ChunkyBoneGeometry* lStructureGeometry = pObject->GetPhysics()->GetBoneGeometry(pObject->GetPhysics()->GetRootBone());
		TBC::PhysicsManager::BodyID lBody = lStructureGeometry->GetBodyId();
		if (!lBody)
		{
			lBody = lStructureGeometry->GetTriggerId();
		}
		GetPhysicsManager()->GetBodyTransform(lBody, lTransform);
		lIsEngineControlled = (pObject->GetPhysics()->GetEngineCount() > 0);
	}
	BroadcastCreateObject(pObject->GetInstanceId(), lTransform, pObject->GetClassId(), pObject->GetOwnerInstanceId());
	OnAttributeSend(pObject);
	if (lIsEngineControlled || pObject->GetVelocity().GetLengthSquared() > 0.1f)
	{
		OnPhysicsSend(pObject);
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
		Cure::NetworkAgent::VSocket* lSocket = lClient->GetUserConnection()->GetSocket();
		if (lSocket)
		{
			GetNetworkAgent()->PlaceInSendBuffer(true, lSocket, lPacket);
		}
	}
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



void GameServerManager::TickMasterServer()
{
	bool lIsOpenServer;
	CURE_RTVAR_GET(lIsOpenServer, =, GetVariableScope(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	bool lPublishServer;
	CURE_RTVAR_GET(lPublishServer, =, GetVariableScope(), RTVAR_NETWORK_PUBLISHSERVER, false);
	if (!mMasterConnection || !lIsOpenServer || !lPublishServer)
	{
		return;
	}

	static str lLocalIpAddress;
	if (lLocalIpAddress.empty())
	{
		IPAddress lIpAddress;
		if (Network::ResolveHostname(_T(""), lIpAddress))
		{
			lLocalIpAddress = lIpAddress.GetAsString();
		}
		else
		{
			lLocalIpAddress = GetNetworkServer()->GetLocalAddress().GetIP().GetAsString();
		}
	}

	str lServerName;
	CURE_RTVAR_GET(lServerName, =, GetVariableScope(), RTVAR_NETWORK_SERVERNAME, _T("?"));
	const str lPlayerCount = strutil::IntToString(GetLoggedInClientCount(), 10);
	const str lId = strutil::ReplaceAll(strutil::Encode(SystemManager::GetSystemPseudoId()), _T("\""), _T("''\\''"));
	const str lLocalServerInfo = _T("--name \"") + lServerName + _T("\" --player-count ") + lPlayerCount
		+ _T(" --id \"") + lId + _T("\" --internal-address ") + lLocalIpAddress +
		_T(" --internal-port ") + strutil::IntToString(GetNetworkServer()->GetLocalAddress().GetPort(), 10);
	float lConnectTimeout;
	CURE_RTVAR_GET(lConnectTimeout, =(float), GetVariableScope(), RTVAR_NETWORK_CONNECT_TIMEOUT, 3.0);
	mMasterConnection->SetSocketInfo(GetNetworkServer(), lConnectTimeout);
	mMasterConnection->SendLocalInfo(lLocalServerInfo);

	ServerInfo lServerInfo;
	if (mMasterConnection->TickReceive(lServerInfo))
	{
		HandleMasterCommand(lServerInfo);
	}
}


bool GameServerManager::HandleMasterCommand(const ServerInfo& pServerInfo)
{
	if (pServerInfo.mCommand == _T(MASTER_SERVER_OF))
	{
		const str lAddress = pServerInfo.mGivenIpAddress + strutil::Format(_T(":%u"), pServerInfo.mGivenPort);
		SocketAddress lSocketAddress;
		if (lSocketAddress.Resolve(lAddress))
		{
			mLog.Infof(_T("Attempting to open local firewall to allow remote client connect from %s."), lAddress.c_str());
			Cure::SocketIoHandler* lSocketIoHandler = GetNetworkServer();
			return lSocketIoHandler->GetMuxIoSocket()->SendOpenFirewallData(lSocketAddress);
		}
	}
	else
	{
		mLog.Errorf(_T("Got bad command (%s) from master server!"), pServerInfo.mCommand.c_str());
		deb_assert(false);
	}
	return false;
}

void GameServerManager::MonitorRtvars()
{
	{
		int lPhysicsFps;
		CURE_RTVAR_GET(lPhysicsFps, =, GetVariableScope(), RTVAR_PHYSICS_FPS, PHYSICS_FPS);
		if (lPhysicsFps != mPhysicsFpsShadow)
		{
			mPhysicsFpsShadow = lPhysicsFps;
			BroadcastStatusMessage(Cure::MessageStatus::INFO_COMMAND,
				wstrutil::Format(L"#" _WIDE(RTVAR_PHYSICS_FPS) L" %i;", lPhysicsFps));
		}
	}
	{
		float lPhysicsRtr;
		CURE_RTVAR_GET(lPhysicsRtr, =(float), GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0);
		if (!Math::IsEpsEqual(mPhysicsRtrShadow, lPhysicsRtr, 0.01f))
		{
			mPhysicsRtrShadow = lPhysicsRtr;
			BroadcastStatusMessage(Cure::MessageStatus::INFO_COMMAND,
				wstrutil::Format(L"#" _WIDE(RTVAR_PHYSICS_RTR) L" %f;", lPhysicsRtr));
		}
	}
	{
		bool lPhysicsHalt;
		CURE_RTVAR_GET(lPhysicsHalt, =, GetVariableScope(), RTVAR_PHYSICS_HALT, false);
		if (lPhysicsHalt != mPhysicsHaltShadow)
		{
			mPhysicsHaltShadow = lPhysicsHalt;
			BroadcastStatusMessage(Cure::MessageStatus::INFO_COMMAND,
				wstrutil::Format(L"#" _WIDE(RTVAR_PHYSICS_RTR) L" %s;", wstrutil::BoolToString(lPhysicsHalt).c_str()));
		}
	}
}



LOG_CLASS_DEFINE(GAME, GameServerManager);



}
