
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once



#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/NetworkServer.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/UserAccountManager.h"
#include "../../Lepra/Include/LogListener.h"
#include "ServerContextResource.h"
#include "Client.h"



namespace Life
{



class GameServerManager: public Cure::GameTicker, public Cure::GameManager, public Cure::NetworkServer::LoginListener
{
public:
	typedef Cure::GameManager Parent;

	GameServerManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		Lepra::InteractiveConsoleLogListener* pConsoleLogger);
	virtual ~GameServerManager();

	bool Tick();

	Lepra::UnicodeStringUtility::StringVector ListUsers();
	bool BroadcastChatMessage(const Lepra::UnicodeString& pMessage);
	bool SendChatMessage(const Lepra::UnicodeString& pClientUserName, const Lepra::UnicodeString& pMessage);

	int GetLoggedInClientCount() const;

private:
	Lepra::String GetName() const;
	Lepra::String GetApplicationCommandFilename() const;

	void Logout(Cure::UserAccount::AccountId pAccountId, const Lepra::String& pReason);
	void DeleteAllClients();

	void ContextObjectLoadCallback(UserContextObjectInfoResource* pResource);

	Client* GetClientByAccount(Cure::UserAccount::AccountId pAccountId) const;

	bool Initialize();
	bool InitializeTerrain();

	float GetPowerSaveAmount() const;

	void TickInput();
	void ProcessNetworkInputMessage(Client* pClient, Cure::Message* pMessage);

	Cure::UserAccount::Availability QueryLogin(const Cure::LoginId& pLoginId, Cure::UserAccount::AccountId& pAccountId);
	void OnLogin(Cure::UserConnection* pUserConnection);
	void OnLogout(Cure::UserConnection* pUserConnection);

	void AdjustClientSimulationSpeed(Client* pClient, int pClientFrameIndex);
	void StoreMovement(int pClientFrameIndex, Cure::MessageObjectMovement* pMovement);
	void ApplyStoredMovement();

	void AvatarLoadCallback(UserContextObjectAccountInfoResource* pResource);
	void OnCollision(const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2);
	void OnStopped(Cure::ContextObject* pObject, TBC::PhysicsEngine::BodyID pBodyId);
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

	Cure::NetworkServer* GetNetworkServer() const;

	typedef std::list<Cure::MessageObjectMovement*> MovementList;
	typedef std::vector<MovementList> MovementArrayList;
	typedef Lepra::HashTable<Cure::UserAccount::AccountId, Client*> AccountClientTable;
	typedef Lepra::HashTable<Cure::UserAccount::AccountId, Client*> AvatarClientTable;

	Cure::UserAccountManager* mUserAccountManager;
	AccountClientTable mAccountClientTable;
	UserContextObjectInfoResource mTerrainResource;	// TODO: remove when applicable.
	UserContextObjectInfoResource mBoxResource;	// TODO: remove when applicable.
	MovementArrayList mMovementArrayList;
	mutable Lepra::Timer mPowerSaveTimer;

	LOG_CLASS_DECLARE();
};



}
