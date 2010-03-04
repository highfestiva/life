
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/NetworkServer.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/UserAccountManager.h"
#include "../../Lepra/Include/LogListener.h"
#include "Client.h"



namespace Life
{



class GameServerManager: public Cure::GameTicker, public Cure::GameManager, public Cure::NetworkServer::LoginListener
{
public:
	typedef Cure::GameManager Parent;

	GameServerManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		InteractiveConsoleLogListener* pConsoleLogger);
	virtual ~GameServerManager();

	bool Tick();

	wstrutil::strvec ListUsers();
	bool BroadcastChatMessage(const wstr& pMessage);
	bool SendChatMessage(const wstr& pClientUserName, const wstr& pMessage);

	int GetLoggedInClientCount() const;

private:
	void Logout(Cure::UserAccount::AccountId pAccountId, const str& pReason);
	void DeleteAllClients();

	Client* GetClientByAccount(Cure::UserAccount::AccountId pAccountId) const;

	bool Initialize();
	bool InitializeTerrain();

	float GetPowerSaveAmount() const;

	void TickInput();
	void ProcessNetworkInputMessage(Client* pClient, Cure::Message* pMessage);

	Cure::UserAccount::Availability QueryLogin(const Cure::LoginId& pLoginId, Cure::UserAccount::AccountId& pAccountId);
	void OnLogin(Cure::UserConnection* pUserConnection);
	void OnLogout(Cure::UserConnection* pUserConnection);
	void OnSelectAvatar(Client* pClient, const Cure::UserAccount::AvatarId& pAvatarId);
	void DropAvatar(Cure::GameObjectId pAvatarId);

	void AdjustClientSimulationSpeed(Client* pClient, int pClientFrameIndex);
	void StoreMovement(int pClientFrameIndex, Cure::MessageObjectMovement* pMovement);
	void DeleteMovements(Cure::GameObjectId pInstanceId);
	void ApplyStoredMovement();

	void BroadcastAvatar(Client* pClient);

	Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2);
	void OnStopped(Cure::ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId);
	bool OnPhysicsSend(Cure::ContextObject* pObject);
	bool IsConnectAuthorized();
	void SendAttach(Cure::ContextObject* pObject1, unsigned pId1, Cure::ContextObject* pObject2, unsigned pId2);
	void SendDetach(Cure::ContextObject* pObject1, Cure::ContextObject* pObject2);

	void BroadcastCreateObject(Cure::ContextObject* pObject);
	void BroadcastDeleteObject(Cure::GameObjectId pInstanceId);
	void SendCreateAllObjects(Client* pClient);
	void BroadcastObjectPosition(Cure::GameObjectId pInstanceId, const Cure::ObjectPositionalData& pPosition,
		Client* pExcludeClient, bool pSafe);
	void BroadcastPacket(const Client* pExcludeClient, Cure::Packet* pPacket, bool pSafe);

	LEPRA_DEBUG_CODE(virtual TBC::PhysicsManager* GetPhysicsManager() const);
	Cure::NetworkServer* GetNetworkServer() const;

	typedef std::list<Cure::MessageObjectMovement*> MovementList;
	typedef std::vector<MovementList> MovementArrayList;
	typedef HashTable<Cure::UserAccount::AccountId, Client*> AccountClientTable;
	typedef HashTable<Cure::UserAccount::AccountId, Client*> AvatarClientTable;

	Cure::UserAccountManager* mUserAccountManager;
	AccountClientTable mAccountClientTable;
	Cure::ContextObject* mTerrainObject;	// TODO: remove when applicable.
	Cure::ContextObject* mBoxObject;	// TODO: remove when applicable.
	MovementArrayList mMovementArrayList;
	mutable Timer mPowerSaveTimer;

	LOG_CLASS_DECLARE();
};



}
