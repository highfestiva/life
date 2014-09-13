
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
class ServerDelegate;
struct ServerInfo;
class ServerMessageProcessor;



class GameServerManager: public Cure::GameManager, public Cure::NetworkServer::LoginListener
{
public:
	typedef Cure::GameManager Parent;
	typedef Cure::ContextManager::ContextObjectTable ContextTable;
	typedef HashTable<Cure::UserAccount::AccountId, Client*> AccountClientTable;

	GameServerManager(const Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager);
	virtual ~GameServerManager();

	virtual bool BeginTick();
	virtual void PreEndTick();

	virtual void StartConsole(InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt);
	bool Initialize(MasterServerConnection* pMasterConnection, const str& pAddress);
	float GetPowerSaveAmount() const;

	LEPRA_DEBUG_CODE(virtual Tbc::PhysicsManager* GetPhysicsManager() const);

	virtual void DeleteContextObject(Cure::GameObjectId pInstanceId);

	ServerDelegate* GetDelegate() const;
	void SetDelegate(ServerDelegate* pDelegate);
	void SetMessageProcessor(ServerMessageProcessor* pMessageProcessor);
	void AdjustClientSimulationSpeed(Client* pClient, int pClientFrameIndex);
	virtual void StoreMovement(int pClientFrameIndex, Cure::MessageObjectMovement* pMovement);
	void OnSelectAvatar(Client* pClient, const Cure::UserAccount::AvatarId& pAvatarId);
	void LoanObject(Client* pClient, Cure::GameObjectId pInstanceId);
	wstrutil::strvec ListUsers();
	Cure::NetworkServer* GetNetworkServer() const;
	void SendObjects(Client* pClient, bool pCreate, const ContextTable& pObjectTable);
	void BroadcastCreateObject(Cure::GameObjectId pInstanceId, const xform& pTransform, const str& pClassId, Cure::GameObjectId pOwnerInstanceId);
	void BroadcastObjectPosition(Cure::GameObjectId pInstanceId, const Cure::ObjectPositionalData& pPosition,
		Client* pExcludeClient, bool pSafe);
	bool BroadcastChatMessage(const wstr& pMessage);
	bool BroadcastStatusMessage(Cure::MessageStatus::InfoType pType, const wstr& pString);
	void BroadcastNumberMessage(Client* pExcludeClient, bool pSafe, Cure::MessageNumber::InfoType pInfo, int32 pInteger, float32 pFloat);
	bool SendChatMessage(const wstr& pClientUserName, const wstr& pMessage);
	void IndicatePosition(const vec3 pPosition, float pTime);

	int GetLoggedInClientCount() const;
	Client* GetClientByAccount(Cure::UserAccount::AccountId pAccountId) const;
	Client* GetClientByObject(Cure::ContextObject*& pObject) const;
	const AccountClientTable& GetAccountClientTable() const;	// Use with caution!

	void Build(const str& pWhat);

	void TickInput();

protected:
	void Logout(Cure::UserAccount::AccountId pAccountId, const str& pReason);
	void DeleteAllClients();

	Cure::UserAccount::Availability QueryLogin(const Cure::LoginId& pLoginId, Cure::UserAccount::AccountId& pAccountId);
	void OnLogin(Cure::UserConnection* pUserConnection);
	void OnLogout(Cure::UserConnection* pUserConnection);

	void DeleteMovements(Cure::GameObjectId pInstanceId);
	void ApplyStoredMovement();

	void BroadcastAvatar(Client* pClient);

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id);
	void FlipCheck(Cure::ContextObject* pObject) const;
	virtual bool OnPhysicsSend(Cure::ContextObject* pObject);
	virtual bool OnAttributeSend(Cure::ContextObject* pObject);
	bool IsServer();
	void SendAttach(Cure::ContextObject* pObject1, unsigned pId1, Cure::ContextObject* pObject2, unsigned pId2);
	void SendDetach(Cure::ContextObject* pObject1, Cure::ContextObject* pObject2);
	void OnIdOwnershipExpired(int, Cure::ContextObject* pObject, void*);
	virtual void HandleWorldBoundaries();

	virtual Cure::ContextObject* CreateLogicHandler(const str& pType);

	void BroadcastCreateObject(Cure::ContextObject* pObject);
	void BroadcastDeleteObject(Cure::GameObjectId pInstanceId);
	void BroadcastPacket(const Client* pExcludeClient, Cure::Packet* pPacket, bool pSafe);

	void TickMasterServer();
	bool HandleMasterCommand(const ServerInfo& pServerInfo);
	void MonitorRtvars();

private:
	typedef std::list<Cure::MessageObjectMovement*> MovementList;
	typedef std::vector<MovementList> MovementArrayList;

	Cure::UserAccountManager* mUserAccountManager;
	AccountClientTable mAccountClientTable;
	ServerDelegate* mDelegate;
	ServerMessageProcessor* mMessageProcessor;
	MovementArrayList mMovementArrayList;
	mutable Timer mPowerSaveTimer;
	MasterServerConnection* mMasterConnection;
	int mPhysicsFpsShadow;
	float mPhysicsRtrShadow;
	bool mPhysicsHaltShadow;

	logclass();
};



}
