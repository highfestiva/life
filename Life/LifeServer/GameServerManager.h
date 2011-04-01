
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/NetworkServer.h"
#include "Client.h"



namespace Lepra
{
class InteractiveConsoleLogListener;
class ConsolePrompt;
}



namespace Life
{



class MasterServerConnection;
struct ServerInfo;



class GameServerManager: public Cure::GameManager, public Cure::NetworkServer::LoginListener
{
public:
	typedef Cure::GameManager Parent;

	GameServerManager(Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager);
	virtual ~GameServerManager();

	virtual void StartConsole(InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt);
	bool Initialize(MasterServerConnection* pMasterConnection);
	float GetPowerSaveAmount() const;

	wstrutil::strvec ListUsers();
	bool BroadcastChatMessage(const wstr& pMessage);
	bool BroadcastStatusMessage(Cure::MessageStatus::InfoType pType, const wstr& pString);
	bool SendChatMessage(const wstr& pClientUserName, const wstr& pMessage);

	int GetLoggedInClientCount() const;

	void Build(const str& pWhat);

	void TickInput();

private:
	typedef Cure::ContextManager::ContextObjectTable ContextTable;

	void Logout(Cure::UserAccount::AccountId pAccountId, const str& pReason);
	void DeleteAllClients();

	Client* GetClientByAccount(Cure::UserAccount::AccountId pAccountId) const;

	virtual bool InitializeTerrain();

	void ProcessNetworkInputMessage(Client* pClient, Cure::Message* pMessage);

	Cure::UserAccount::Availability QueryLogin(const Cure::LoginId& pLoginId, Cure::UserAccount::AccountId& pAccountId);
	void OnLogin(Cure::UserConnection* pUserConnection);
	void OnLogout(Cure::UserConnection* pUserConnection);
	void OnSelectAvatar(Client* pClient, const Cure::UserAccount::AvatarId& pAvatarId);
	void DeleteObject(Cure::GameObjectId pInstanceId);

	void AdjustClientSimulationSpeed(Client* pClient, int pClientFrameIndex);
	void StoreMovement(int pClientFrameIndex, Cure::MessageObjectMovement* pMovement);
	void DeleteMovements(Cure::GameObjectId pInstanceId);
	void ApplyStoredMovement();

	void BroadcastAvatar(Client* pClient);

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);
	void FlipCheck(Cure::ContextObject* pObject) const;
	virtual bool OnPhysicsSend(Cure::ContextObject* pObject);
	virtual bool OnAttributeSend(Cure::ContextObject* pObject);
	bool IsServer();
	void SendAttach(Cure::ContextObject* pObject1, unsigned pId1, Cure::ContextObject* pObject2, unsigned pId2);
	void SendDetach(Cure::ContextObject* pObject1, Cure::ContextObject* pObject2);
	virtual void OnAlarm(int pAlarmId, Cure::ContextObject* pObject, void* pExtraData);
	virtual void HandleWorldBoundaries();

	virtual Cure::ContextObject* CreateLogicHandler(const str& pType) const;

	void BroadcastCreateObject(Cure::ContextObject* pObject);
	void BroadcastDeleteObject(Cure::GameObjectId pInstanceId);
	void SendObjects(Client* pClient, bool pCreate, const ContextTable& pObjectTable);
	void BroadcastObjectPosition(Cure::GameObjectId pInstanceId, const Cure::ObjectPositionalData& pPosition,
		Client* pExcludeClient, bool pSafe);
	void BroadcastPacket(const Client* pExcludeClient, Cure::Packet* pPacket, bool pSafe);

	LEPRA_DEBUG_CODE(virtual TBC::PhysicsManager* GetPhysicsManager() const);
	Cure::NetworkServer* GetNetworkServer() const;

	void TickMasterServer();
	bool HandleMasterCommand(const ServerInfo& pServerInfo);
	void MonitorRtvars();

	typedef std::list<Cure::MessageObjectMovement*> MovementList;
	typedef std::vector<MovementList> MovementArrayList;
	typedef HashTable<Cure::UserAccount::AccountId, Client*> AccountClientTable;
	typedef HashTable<Cure::UserAccount::AccountId, Client*> AvatarClientTable;

	Cure::UserAccountManager* mUserAccountManager;
	AccountClientTable mAccountClientTable;
	Cure::ContextObject* mTerrainObject;	// TODO: remove when applicable.
	MovementArrayList mMovementArrayList;
	mutable Timer mPowerSaveTimer;
	MasterServerConnection* mMasterConnection;
	int mPhysicsFpsShadow;
	float mPhysicsRtrShadow;

	LOG_CLASS_DECLARE();
};



}
