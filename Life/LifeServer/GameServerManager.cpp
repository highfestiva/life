
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameServerManager.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/CppContextObject.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Path.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../LifeApplication.h"
#include "Elevator.h"
#include "MasterServerConnection.h"
#include "RtVar.h"
#include "ServerConsoleManager.h"
#include "Spawner.h"



namespace Life
{



const int NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE = PHYSICS_FPS/2;



GameServerManager::GameServerManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Cure::GameManager(pVariableScope, pResourceManager),
	mUserAccountManager(new Cure::MemoryUserAccountManager()),
	mTerrainObject(0),
	mMovementArrayList(NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE),
	mMasterConnection(0)
{
	CURE_RTVAR_SET_IF_NOT_SET(Cure::GetSettings(), RTVAR_APPLICATION_AUTOEXITONEMPTYSERVER, false);
	CURE_RTVAR_SET_IF_NOT_SET(Cure::GetSettings(), RTVAR_GAME_SPAWNPART, 1.0);
	CURE_RTVAR_SET_IF_NOT_SET(Cure::GetSettings(), RTVAR_NETWORK_SERVERNAME, _T("My Server"));

	SetNetworkAgent(new Cure::NetworkServer(pVariableScope, this));

}

GameServerManager::~GameServerManager()
{
	if (mMasterConnection)
	{
		mMasterConnection->AppendLocalInfo(_T(" --remove true"));
		mMasterConnection->WaitUntilDone(50.0, true);
		mMasterConnection = 0;	// Not owned by us, deleted elsewhere.
	}

	DeleteAllClients();

	delete (mUserAccountManager);
	mUserAccountManager = 0;

	if (GetConsoleManager())
	{
		GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ") + Application::GetIoFile(_T("ServerApplication"), _T("lsh")));
	}
}



void GameServerManager::StartConsole(InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt)
{
	if (!GetConsoleManager())
	{
		SetConsoleManager(new ServerConsoleManager(this, GetVariableScope(), pConsoleLogger, pConsolePrompt));
	}
	GetConsoleManager()->PushYieldCommand(_T("execute-file -i ") + Application::GetIoFile(_T("ServerApplication"), _T("lsh")));
	GetConsoleManager()->Start();
}

bool GameServerManager::Initialize(MasterServerConnection* pMasterConnection)
{
	bool lOk = InitializeTerrain();

	if (lOk)
	{
		int x;
		for (x = 0; lOk && x < 100; ++x)
		{
			const wstr lUserName = wstrutil::Format(L"User%i", x);
			wstr lReadablePassword(L"CarPassword");
			Cure::MangledPassword lPassword(lReadablePassword);
			lOk = mUserAccountManager->AddUserAccount(Cure::LoginId(lUserName, lPassword));
			if (lOk)
			{
				DiskFile::FindData lFindData;
				if (DiskFile::FindFirst(_T("Data/*.class"), lFindData))
				{
					do
					{
						if (lFindData.GetName().find(_T("level_")) != str::npos ||
							lFindData.GetName().find(_T("road_sign")) != str::npos)
						{
							continue;
						}
						Cure::UserAccount::AvatarId lId(Path::GetFileBase(lFindData.GetName()));
						lOk = mUserAccountManager->AddUserAvatarId(lUserName, lId);
					}
					while (lOk && DiskFile::FindNext(lFindData));
				}
			}
		}
	}

	str lAcceptAddress;
	CURE_RTVAR_GET(lAcceptAddress, =, GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, _T("0.0.0.0:16650"));
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
		UploadServerInfo();
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
			lOk |= GetNetworkAgent()->SendStatusMessage(lClient->GetUserConnection()->GetSocket(), 0,
				Cure::REMOTE_OK, Cure::MessageStatus::INFO_CHAT, pMessage, lPacket);
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
		lOk = GetNetworkAgent()->SendStatusMessage(lClient->GetUserConnection()->GetSocket(), 0, Cure::REMOTE_OK,
			Cure::MessageStatus::INFO_CHAT, pMessage, lPacket);
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



void GameServerManager::TickInput()
{
	UploadServerInfo();

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



Client* GameServerManager::GetClientByAccount(Cure::UserAccount::AccountId pAccountId) const
{
	Client* lClient = mAccountClientTable.FindObject(pAccountId);
	return (lClient);
}



bool GameServerManager::InitializeTerrain()
{
	assert(mTerrainObject == 0);
	mTerrainObject = Parent::CreateContextObject(_T("level_01"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	mTerrainObject->StartLoading();
	return (true);
}



void GameServerManager::ProcessNetworkInputMessage(Client* pClient, Cure::Message* pMessage)
{
	Cure::MessageType lType = pMessage->GetType();
	switch (lType)
	{
		case Cure::MESSAGE_TYPE_STATUS:
		{
			Cure::MessageStatus* lStatus = (Cure::MessageStatus*)pMessage;
			if (lStatus->GetRemoteStatus() == Cure::REMOTE_OK)
			{
				switch (lStatus->GetInfo())
				{
					case Cure::MessageStatus::INFO_CHAT:
					{
						log_adebug("Chat...");
					}
					break;
					case Cure::MessageStatus::INFO_AVATAR:
					{
						log_adebug("Avatar selected...");
						wstr lAvatarName;
						lStatus->GetMessageString(lAvatarName);
						const Cure::UserAccount::AvatarId lAvatarId = strutil::Encode(lAvatarName);
						OnSelectAvatar(pClient, lAvatarId);
					}
					break;
					default:
					{
						assert(false);
					}
					break;
				}
			}
			else //if (lStatus->GetRemoteStatus() == Cure::REMOTE_NO_CONNECTION)
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
			if (pClient->GetAvatarId() == lInstanceId)
			{
				int32 lClientFrameIndex = lMovement->GetFrameIndex();
				AdjustClientSimulationSpeed(pClient, lClientFrameIndex);
				// Pass on to other clients.
				const Cure::ObjectPositionalData& lPosition = lMovement->GetPositionalData();
				BroadcastObjectPosition(lInstanceId, lPosition, pClient, false);
				StoreMovement(lClientFrameIndex, lMovement);
			}
			else
			{
				mLog.Warningf(_T("Client %i tried to control instance ID %i."),
					strutil::Encode(pClient->GetUserConnection()->GetLoginName()).c_str(),
					lInstanceId);
			}
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
			else if (lNumber->GetInfo() == Cure::MessageNumber::INFO_RECREATE_OBJECT)
			{
				const Cure::GameObjectId lInstanceId = lNumber->GetInteger();
				Cure::ContextObject* lObject = GetContext()->GetObject(lInstanceId);
				if (lObject)
				{
					
					ContextTable lTable;
					lTable.insert(ContextTable::value_type(lInstanceId, lObject));
					SendObjects(pClient, true, lTable);
					log_volatile(mLog.Debugf(_T("Recreating %s (%i) on client."),
						lObject->GetClassId().c_str(),
						lInstanceId));
				}
				else
				{
					mLog.Warningf(_T("User %s tried to fetch unknown object with ID %i."), pClient->GetUserConnection()->GetLoginName().c_str(), lInstanceId);
				}
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

		for (AvatarIdSet::const_iterator x = lAvatarIdSet->begin(); x != lAvatarIdSet->end(); ++x)
		{
			const Cure::UserAccount::AvatarId& lAvatarId = *x;
			lClient->SendAvatar(lAvatarId, lPacket);
		}

		GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

		SendObjects(lClient, true, GetContext()->GetObjectTable());
	}
	else
	{
		mLog.Error(_T("User ") + strutil::Encode(pUserConnection->GetLoginName()) + _T(" does not exist or is not allowed avatars!"));
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
	}
	DeleteObject(lAvatarId);

	mLog.Info(_T("User ") + strutil::Encode(pUserConnection->GetLoginName()) + _T(" logged out."));
}

void GameServerManager::OnSelectAvatar(Client* pClient, const Cure::UserAccount::AvatarId& pAvatarId)
{
	ScopeLock lTickLock(GetTickLock());

	TransformationF lTransform;
	const Cure::GameObjectId lPreviousAvatarId = pClient->GetAvatarId();
	if (lPreviousAvatarId)
	{
		mLog.Info(_T("User ")+strutil::Encode(pClient->GetUserConnection()->GetLoginName())+_T(" had an avatar, replacing it."));
		pClient->SetAvatarId(0);
		Cure::ContextObject* lObject = GetContext()->GetObject(lPreviousAvatarId);
		if (lObject)
		{
			lTransform.SetPosition(lObject->GetPosition());
			lTransform.GetPosition() += Vector3DF(0, 0, 2);
			Vector3DF lEulerAngles;
			lObject->GetOrientation().GetEulerAngles(lEulerAngles);
			lTransform.GetOrientation().SetEulerAngles(lEulerAngles.x, 0, 0);
		}
		DeleteObject(lPreviousAvatarId);
	}

	mLog.Info(_T("Loading avatar '")+pAvatarId+_T("' for user ")+strutil::Encode(pClient->GetUserConnection()->GetLoginName())+_T("."));
	Cure::ContextObject* lObject = Parent::CreateContextObject(pAvatarId,
		Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
	lObject->SetInitialTransform(lTransform);
	pClient->SetAvatarId(lObject->GetInstanceId());
	lObject->SetExtraData((void*)(intptr_t)pClient->GetUserConnection()->GetAccountId());
	lObject->StartLoading();
}

void GameServerManager::DeleteObject(Cure::GameObjectId pInstanceId)
{
	DeleteMovements(pInstanceId);
	GetContext()->DeleteObject(pInstanceId);
	BroadcastDeleteObject(pInstanceId);
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
		mLog.Warningf(_T("Skipping store of movement (%i frames ahead)."), lFrameOffset);
	}
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
		assert(lMovementCount == lMovementList.size());
		MovementList::iterator x = lMovementList.begin();
		for (; x != lMovementList.end(); ++x)
		{
			Cure::MessageObjectMovement* lMovement = *x;
			Cure::GameObjectId lInstanceId = lMovement->GetObjectId();
			if (lCurrentPhysicsFrame != lMovement->GetFrameIndex())
			{
				mLog.AWarning("Throwing away network movement.");
			}
			else
			{
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
	return (new Cure::CppContextObject(GetResourceManager(), pClassId));
}

void GameServerManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	Client* lClient = 0;
	{
		Cure::UserAccount::AccountId lAccountId = (Cure::UserAccount::AccountId)(intptr_t)pObject->GetExtraData();
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
		DeleteMovements(pObject->GetInstanceId());
		if (lClient)
		{
			mLog.Infof(_T("Loaded avatar for %s with instance id %i."),
				strutil::Encode(lClient->GetUserConnection()->GetLoginName()).c_str(),
				pObject->GetInstanceId());
			BroadcastAvatar(lClient);
		}
		else
		{
			log_volatile(mLog.Debugf(_T("Loaded object %s."), pObject->GetClassId().c_str()));
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
			/*if (!lSendCollision && lAreBothDynamic)
			{
				// If the other object thinks it's a high impact, we go. This is to not
				// replicate when another - heavier - object is standing on top of us.
				lSendCollision = IsHighImpact(7.0f, pObject2, pForce, pTorque);
			}*/
		}
		else if (lAreBothDynamic)
		{
			lSendCollision = IsHighImpact(12.0f, pObject1, pForce, pTorque);
		}
		if (lSendCollision)
		{
			// We have found a collision. Asynchronously inform all viewers, including the colliding client.
			if (pObject1->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED)
			{
				pObject1->SetSendCount(3);
			}
			GetContext()->AddPhysicsSenderObject(pObject1);
		}
	}
}

void GameServerManager::OnStopped(Cure::ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId)
{
#ifdef LEPRA_DEBUG
	const unsigned lRootIndex = 0;
	assert(pObject->GetStructureGeometry(lRootIndex));
	assert(pObject->GetStructureGeometry(lRootIndex)->GetBodyId() == pBodyId);
#else // !Debug
	pBodyId;
#endif // Debug / !Debug
	if (pObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		log_volatile(mLog.Debugf(_T("Object %u/%s stopped, sending position."), pObject->GetInstanceId(), pObject->GetClassId().c_str()));
		GetContext()->AddPhysicsSenderObject(pObject);
	}
}

bool GameServerManager::OnPhysicsSend(Cure::ContextObject* pObject)
{
	bool lLastSend = false;
	if (pObject->QueryResendTime(0.6f, false))
	{
		lLastSend = true;
		log_volatile(mLog.Debugf(_T("Sending pos for %s."), pObject->GetClassId().c_str()));
		const Cure::ObjectPositionalData* lPosition = 0;
		if (pObject->UpdateFullPosition(lPosition))
		{
			BroadcastObjectPosition(pObject->GetInstanceId(), *lPosition, 0, false);
			lLastSend = (pObject->PopSendCount() == 0);
		}

	}
	return (lLastSend);
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

void GameServerManager::HandleWorldBoundaries()
{
	std::vector<Cure::GameObjectId> lLostObjectArray;
	const ContextTable& lObjectTable = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		const TBC::ChunkyPhysics* lPhysics = lObject->GetPhysics();
		if (lObject->IsLoaded() && lPhysics && lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)
		{
			const Vector3DF lPosition = lObject->GetPosition();
			if (lPosition.z < -1000 || lPosition.z > +800)
			{
				lLostObjectArray.push_back(lObject->GetInstanceId());
			}
		}
	}
	std::vector<Cure::GameObjectId>::const_iterator y = lLostObjectArray.begin();
	for (; y != lLostObjectArray.end(); ++y)
	{
		DeleteObject(*y);
	}
}



Cure::ContextObject* GameServerManager::CreateLogicHandler(const str& pType) const
{
	if (pType == _T("trig_elevator"))
	{
		return new Elevator(GetContext());
	}
	else if (pType == _T("spawner"))
	{
		return new Spawner(GetContext());
	}
	assert(false);
	return (0);
}



void GameServerManager::BroadcastCreateObject(Cure::ContextObject* pObject)
{
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageCreateObject* lCreate = (Cure::MessageCreateObject*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OBJECT);
	lPacket->AddMessage(lCreate);
	lCreate->Store(lPacket, pObject->GetInstanceId(), pObject->GetInitialTransform(),
		wstrutil::Encode(pObject->GetClassId()));
	BroadcastPacket(0, lPacket, true);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
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

void GameServerManager::SendObjects(Client* pClient, bool pCreate, const ContextTable& pObjectTable)
{
	// TODO: restrict to visible, concrete objects.

	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::MessageCreateObject* lCreateMessage = 0;
	if (pCreate)
	{
		lCreateMessage = (Cure::MessageCreateObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_CREATE_OBJECT);
	}
	Cure::MessageObjectPosition* lPositionMessage = (Cure::MessageObjectPosition*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_POSITION);

	ContextTable::const_iterator x = pObjectTable.begin();
	for (; x != pObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		// Don't send local objects.
		if (lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			lPacket->Clear();

			if (lCreateMessage)	// Store creation info?
			{
				lPacket->AddMessage(lCreateMessage);
				TransformationF lTransformation(lObject->GetOrientation(), lObject->GetPosition());
				lCreateMessage->Store(lPacket, lObject->GetInstanceId(),
					 lTransformation, wstrutil::Encode(lObject->GetClassId()));
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
	}

	lPacket->Clear();
	if (lCreateMessage)	// Store creation info?
	{
		GetNetworkAgent()->GetPacketFactory()->GetMessageFactory()->Release(lCreateMessage);
	}
	GetNetworkAgent()->GetPacketFactory()->GetMessageFactory()->Release(lPositionMessage);
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



void GameServerManager::UploadServerInfo()
{
	strutil::strvec lAddressParts = strutil::Split(GetNetworkServer()->GetLocalAddress(), _T(":"), 1);
	if (lAddressParts.size() == 2)
	{
		const str lPort = lAddressParts[1];
		str lServerName;
		CURE_RTVAR_GET(lServerName, =, Cure::GetSettings(), RTVAR_NETWORK_SERVERNAME, _T("?"));
		const str lPlayerCount = strutil::IntToString(GetLoggedInClientCount(), 10);
		const str lId = strutil::ReplaceAll(strutil::Encode(SystemManager::GetSystemPseudoId()), _T("\""), _T("''\\''"));
		const str lLocalServerInfo = _T("--name \"")+lServerName + _T("\" --player-count ")+lPlayerCount
			+ _T(" --port ")+lPort + _T(" --id \"")+lId+_T("\"");
		mMasterConnection->SendLocalInfo(lLocalServerInfo);
	}
}

LOG_CLASS_DEFINE(GAME, GameServerManager);



}
