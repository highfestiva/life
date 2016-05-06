
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/networkserver.h"
#include "client.h"



namespace lepra {
class InteractiveConsoleLogListener;
class ConsolePrompt;
}



namespace life {



class MasterServerConnection;
class ServerDelegate;
struct ServerInfo;
class ServerMessageProcessor;



class GameServerManager: public cure::GameManager, public cure::NetworkServer::LoginListener {
public:
	typedef cure::GameManager Parent;
	typedef cure::ContextManager::ContextObjectTable ContextTable;
	typedef HashTable<cure::UserAccount::AccountId, Client*> AccountClientTable;

	GameServerManager(const cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
		cure::ResourceManager* resource_manager);
	virtual ~GameServerManager();

	virtual bool BeginTick();
	virtual void PreEndTick();

	virtual void StartConsole(InteractiveConsoleLogListener* console_logger, ConsolePrompt* console_prompt);
	bool Initialize(MasterServerConnection* master_connection, const str& address);
	float GetPowerSaveAmount() const;

	LEPRA_DEBUG_CODE(virtual tbc::PhysicsManager* GetPhysicsManager() const);

	virtual void DeleteContextObject(cure::GameObjectId instance_id);

	ServerDelegate* GetDelegate() const;
	void SetDelegate(ServerDelegate* delegate);
	void SetMessageProcessor(ServerMessageProcessor* message_processor);
	void AdjustClientSimulationSpeed(Client* client, int client_frame_index);
	virtual void StoreMovement(int client_frame_index, cure::MessageObjectMovement* movement);
	void OnSelectAvatar(Client* client, const cure::UserAccount::AvatarId& avatar_id);
	void LoanObject(Client* client, cure::GameObjectId instance_id);
	strutil::strvec ListUsers();
	cure::NetworkServer* GetNetworkServer() const;
	void SendObjects(Client* client, bool create, const ContextTable& object_table);
	void BroadcastCreateObject(cure::GameObjectId instance_id, const xform& transform, const str& class_id, cure::GameObjectId owner_instance_id);
	void BroadcastObjectPosition(cure::GameObjectId instance_id, const cure::ObjectPositionalData& position,
		Client* exclude_client, bool safe);
	bool BroadcastChatMessage(const str& message);
	bool BroadcastStatusMessage(cure::MessageStatus::InfoType type, const str& s);
	void BroadcastNumberMessage(Client* exclude_client, bool safe, cure::MessageNumber::InfoType info, int32 integer, float32 f);
	bool SendChatMessage(const str& client_user_name, const str& message);
	void IndicatePosition(const vec3 position, float time);

	int GetLoggedInClientCount() const;
	Client* GetClientByAccount(cure::UserAccount::AccountId account_id) const;
	Client* GetClientByObject(cure::ContextObject*& object) const;
	const AccountClientTable& GetAccountClientTable() const;	// Use with caution!

	void Build(const str& what);

	void TickInput();

protected:
	void Logout(cure::UserAccount::AccountId account_id, const str& reason);
	void DeleteAllClients();

	cure::UserAccount::Availability QueryLogin(const cure::LoginId& login_id, cure::UserAccount::AccountId& account_id);
	void OnLogin(cure::UserConnection* user_connection);
	void OnLogout(cure::UserConnection* user_connection);

	void DeleteMovements(cure::GameObjectId instance_id);
	void ApplyStoredMovement();

	void BroadcastAvatar(Client* client);

	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* object1, cure::ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id);
	void FlipCheck(cure::ContextObject* object) const;
	virtual bool OnPhysicsSend(cure::ContextObject* object);
	virtual bool OnAttributeSend(cure::ContextObject* object);
	bool IsServer();
	void SendAttach(cure::ContextObject* object1, unsigned id1, cure::ContextObject* object2, unsigned id2);
	void SendDetach(cure::ContextObject* object1, cure::ContextObject* object2);
	void OnIdOwnershipExpired(int, cure::ContextObject* object, void*);
	virtual void HandleWorldBoundaries();

	virtual cure::ContextObject* CreateLogicHandler(const str& type);

	void BroadcastCreateObject(cure::ContextObject* object);
	void BroadcastDeleteObject(cure::GameObjectId instance_id);
	void BroadcastPacket(const Client* exclude_client, cure::Packet* packet, bool safe);

	void TickMasterServer();
	bool HandleMasterCommand(const ServerInfo& server_info);
	void MonitorRtvars();

private:
	typedef std::list<cure::MessageObjectMovement*> MovementList;
	typedef std::vector<MovementList> MovementArrayList;

	cure::UserAccountManager* user_account_manager_;
	AccountClientTable account_client_table_;
	ServerDelegate* delegate_;
	ServerMessageProcessor* message_processor_;
	MovementArrayList movement_array_list_;
	mutable Timer power_save_timer_;
	MasterServerConnection* master_connection_;
	int physics_fps_shadow_;
	float physics_rtr_shadow_;
	bool physics_halt_shadow_;

	logclass();
};



}
