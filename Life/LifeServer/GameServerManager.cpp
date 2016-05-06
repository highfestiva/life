
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "gameservermanager.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/cppcontextobject.h"
#include "../../cure/include/contextobjectattribute.h"
#include "../../cure/include/elevator.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/network.h"
#include "../../lepra/include/path.h"
#include "../../lepra/include/systemmanager.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../tbc/include/physicsengine.h"
#include "../../tbc/include/physicsspawner.h"
#include "../lifemaster/masterserver.h"
#include "../lifeapplication.h"
#include "../lifestring.h"
#include "../spawner.h"
#include "bullettime.h"
#include "masterserverconnection.h"
#include "racetimer.h"
#include "rtvar.h"
#include "serverconsolemanager.h"
#include "serverdelegate.h"
#include "servermessageprocessor.h"



namespace life {



const int NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE = PHYSICS_FPS/2;
const int ID_OWNERSHIP_LOAN_EXPIRES = 1;



GameServerManager::GameServerManager(const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager):
	Parent(time, variable_scope, resource_manager),
	user_account_manager_(new cure::MemoryUserAccountManager()),
	delegate_(0),
	message_processor_(0),
	movement_array_list_(NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE),
	master_connection_(0),
	physics_fps_shadow_(24),
	physics_rtr_shadow_(1),
	physics_halt_shadow_(false) {
	v_set(GetVariableScope(), kRtvarGameAutoflipenabled, true);
	v_set(GetVariableScope(), kRtvarGameSpawnpart, 1.0);
	v_set(GetVariableScope(), kRtvarNetworkLogingreeting, _("echo 4 \"Welcome to my server! Enjoy the ride!\""));
	v_set(GetVariableScope(), kRtvarNetworkServername, strutil::Format(_("%s's server"), SystemManager::GetLoginName().c_str()));
	v_set(cure::GetSettings(), kRtvarNetworkPublishserver, false);	// Make it overridable and possible to change between dedicated/local by using cure::GetSettings().

	SetNetworkAgent(new cure::NetworkServer(variable_scope, this));
}

GameServerManager::~GameServerManager() {
	if (master_connection_) {
		if (!master_connection_->CloseUnlessUploaded()) {
			bool skip_quit_for_now = (SystemManager::GetQuitRequest() == 1);
			if (skip_quit_for_now) {
				SystemManager::AddQuitRequest(-1);
			}
			log_.Info("Unregistering server with master. Might take a few secs...");
			master_connection_->AppendLocalInfo(" --remove true");
			master_connection_->WaitUntilDone(10.0, true);
			master_connection_->GraceClose(1.0, false);
			master_connection_ = 0;	// Not owned by us, deleted elsewhere.
			if (skip_quit_for_now) {
				SystemManager::AddQuitRequest(+1);
			}
		}
	}

	DeleteAllClients();

	delete message_processor_;
	message_processor_ = 0;
	delete delegate_;
	delegate_ = 0;

	delete (user_account_manager_);
	user_account_manager_ = 0;

	if (GetConsoleManager()) {
		GetConsoleManager()->ExecuteCommand("save-application-config-file " + Application::GetIoFile("ServerApplication", "lsh"));
	}
}



bool GameServerManager::BeginTick() {
	AccountClientTable::Iterator x = account_client_table_.First();
	for (; x != account_client_table_.End(); ++x) {
		const Client* _client = x.GetObject();
		cure::CppContextObject* _object = (cure::CppContextObject*)GetContext()->GetObject(_client->GetAvatarId());
		if (!_object) {
			continue;
		}
		if (_object->IsAttributeTrue("float_childishness") ||
			_object->GetGuideMode() == tbc::ChunkyPhysics::kGuideAlways) {
			_object->StabilizeTick();
		}
	}

	return Parent::BeginTick();
}

void GameServerManager::PreEndTick() {
	Parent::PreEndTick();
	delegate_->PreEndTick();
}



void GameServerManager::StartConsole(InteractiveConsoleLogListener* console_logger, ConsolePrompt* console_prompt) {
	if (!GetConsoleManager()) {
		SetConsoleManager(new ServerConsoleManager(GetResourceManager(), this, GetVariableScope(), console_logger, console_prompt));
	}
	GetConsoleManager()->PushYieldCommand("execute-file -i " + Application::GetIoFile("ServerApplication", "lsh"));
	GetConsoleManager()->Start();
}

bool GameServerManager::Initialize(MasterServerConnection* master_connection, const str& address) {
	bool _ok = true;

	if (_ok) {
		strutil::strvec file_array;
		int x;
		for (x = 0; _ok && x < 100; ++x) {
			const str user_name = strutil::Format("User%i", x);
			str readable_password("CarPassword");
			cure::MangledPassword password(readable_password);
			_ok = user_account_manager_->AddUserAccount(cure::LoginId(user_name, password));
			if (_ok) {
				if (file_array.empty()) {
					file_array = GetResourceManager()->ListFiles("*.class");
				}
				strutil::strvec::const_iterator x = file_array.begin();
				for (; _ok && x != file_array.end(); ++x) {
					if (x->find("level_") != str::npos ||
						x->find("road_sign") != str::npos) {
						continue;
					}
					cure::UserAccount::AvatarId id(Path::GetFileBase(*x));
					_ok = user_account_manager_->AddUserAvatarId(user_name, id);
				}
			}
			deb_assert(!user_account_manager_->GetUserAvatarIdSet(user_name)->empty());
		}
	}

	str accept_address = address;
	if (_ok) {
		SocketAddress _address;
		if (!_address.Resolve(accept_address)) {
			log_.Warningf("Could not resolve address '%s'.", accept_address.c_str());
			accept_address = ":16650";
			if (!_address.Resolve(accept_address)) {
				log_.Errorf("Could not resolve address '%s', defaulting to 'localhost'.", accept_address.c_str());
				accept_address = "localhost:16650";
			}
		}
	}
	if (_ok) {
		_ok = GetNetworkServer()->Start(accept_address);
		if (!_ok) {
			log_.Fatalf("Is a server already running on '%s'?", accept_address.c_str());
		}
	}
	if (_ok) {
		master_connection_ = master_connection;
		TickMasterServer();
		delegate_->OnOpen();
	}
	return (_ok);
}

float GameServerManager::GetPowerSaveAmount() const {
	power_save_timer_.UpdateTimer();
	float power_save;
	// TODO: if there are logged-in clients, check if all have been idle lately.
	if (GetLoggedInClientCount() > 0) {
		power_save = 0;	// Users are currently playing = no power save.
		power_save_timer_.ClearTimeDiff();
	} else if (power_save_timer_.GetTimeDiff() < 10.0) {
		power_save = 0;	// Users played until very recently = no power save yet.
	} else if (GetNetworkAgent()->GetConnectionCount() > 0) {
		power_save = 0.1f;	// Someone is currently logging in, but not logged in yet.
	} else {
		power_save = 1.0f;	// None logged in, none connected.
	}
	return (power_save);
}



#ifdef LEPRA_DEBUG
tbc::PhysicsManager* GameServerManager::GetPhysicsManager() const {
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner() ||
		(GetNetworkAgent()->GetLock()->IsOwner() && GetTickLock()->IsOwner()));
	return Parent::GetPhysicsManager();
}
#endif // Debug mode



void GameServerManager::DeleteContextObject(cure::GameObjectId instance_id) {
	deb_assert(GetTickLock()->IsOwner());

	cure::ContextObject* _object = GetContext()->GetObject(instance_id);
	delegate_->OnDeleteObject(_object);
	if (_object && _object->GetNetworkObjectType() != cure::kNetworkObjectLocalOnly) {
		BroadcastDeleteObject(instance_id);
	}
	DeleteMovements(instance_id);
	GetContext()->DeleteObject(instance_id);
}



ServerDelegate* GameServerManager::GetDelegate() const {
	return delegate_;
}

void GameServerManager::SetDelegate(ServerDelegate* delegate) {
	deb_assert(!delegate_);
	delegate_ = delegate;
}

void GameServerManager::SetMessageProcessor(ServerMessageProcessor* message_processor) {
	deb_assert(!message_processor_);
	message_processor_ = message_processor;
}

void GameServerManager::AdjustClientSimulationSpeed(Client* client, int client_frame_index) {
	const int current_frame_diff = GetTimeManager()->GetCurrentPhysicsFrameDelta(client_frame_index);
	// Calculate client network latency and jitter.
	client->StoreFrameDiff(current_frame_diff);

	// Send physics enginge speed-up of slow-down to client if necessary.
	client->QuerySendStriveTimes();
}

void GameServerManager::StoreMovement(int client_frame_index, cure::MessageObjectMovement* movement) {
	const int current_physics_frame = GetTimeManager()->GetCurrentPhysicsFrame();
	const int frame_offset = GetTimeManager()->GetPhysicsFrameDelta(client_frame_index, current_physics_frame);
	if (frame_offset >= 0 && frame_offset < NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE) {
		cure::MessageObjectMovement* _movement = movement->CloneToStandalone();
		const int frame_cycle_index = (current_physics_frame+frame_offset)%NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE;
		//log_.Infof("Storing position with %u children.", ((cure::MessageObjectPosition*)_movement)->GetPositionalData().body_position_array_.size());
		movement_array_list_[frame_cycle_index].push_back(_movement);
	} else {
		// This input data is already old or too much ahead! Skip it.
		log_volatile(log_.Debugf("Skipping store of movement (%i frames ahead).", frame_offset));
	}
}

void GameServerManager::OnSelectAvatar(Client* client, const cure::UserAccount::AvatarId& avatar_id) {
	ScopeLock tick_lock(GetTickLock());
	delegate_->OnSelectAvatar(client, avatar_id);
}

void GameServerManager::LoanObject(Client* client, cure::GameObjectId instance_id) {
	cure::ContextObject* _object = GetContext()->GetObject(instance_id);
	if (_object) {
		if (_object->GetBorrowerInstanceId() == client->GetAvatarId() ||	// Reloan?
			(_object->GetBorrowerInstanceId() == 0 &&
			_object->GetNetworkObjectType() == cure::kNetworkObjectLocallyControlled)) {
			if (delegate_->IsObjectLendable(client, _object)) {
				const float client_ownership_time = 10.0f;
				const float server_ownership_time = client_ownership_time * 1.3f;
				int end_frame = GetTimeManager()->GetCurrentPhysicsFrameAddSeconds(client_ownership_time);
				_object->SetNetworkObjectType(cure::kNetworkObjectRemoteControlled);
				GetNetworkServer()->SendNumberMessage(true, client->GetUserConnection()->GetSocket(),
					cure::MessageNumber::kInfoGrantLoan, instance_id, (float)end_frame);
				_object->SetBorrowerInstanceId(client->GetAvatarId());
				GetContext()->CancelPendingAlarmCallbacksById(_object, ID_OWNERSHIP_LOAN_EXPIRES);
				GetContext()->AddAlarmExternalCallback(_object, cure::ContextManager::AlarmExternalCallback(this, &GameServerManager::OnIdOwnershipExpired), ID_OWNERSHIP_LOAN_EXPIRES, server_ownership_time, 0);
			}
		}
	} else {
		log_.Warningf("User %s requested ownership of object with ID %i (just unloaded?).", client->GetUserConnection()->GetLoginName().c_str(), instance_id);
	}
}

strutil::strvec GameServerManager::ListUsers() {
	strutil::strvec __vector;
	{
		deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
		ScopeLock tick_lock(GetTickLock());
		ScopeLock net_lock(GetNetworkAgent()->GetLock());

		AccountClientTable::Iterator x = account_client_table_.First();
		for (; x != account_client_table_.End(); ++x) {
			const Client* _client = x.GetObject();
			str user_info = _client->GetUserConnection()->GetLoginName();
			cure::ContextObject* _object = GetContext()->GetObject(_client->GetAvatarId());
			if (_object) {
				vec3 _position = _object->GetPosition();
				user_info += strutil::Format(" at (%f, %f, %f)", _position.x, _position.y, _position.z);
			} else {
				user_info += " [not loaded]";
			}
			__vector.push_back(user_info);
		}
	}
	return (__vector);
}

cure::NetworkServer* GameServerManager::GetNetworkServer() const {
	return ((cure::NetworkServer*)GetNetworkAgent());
}

void GameServerManager::SendObjects(Client* client, bool create, const ContextTable& object_table) {
	// TODO: restrict to visible, concrete objects.

	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	cure::MessageCreateObject* create_message = 0;
	cure::MessageCreateOwnedObject* create_owned_message = 0;
	if (create) {
		create_message = (cure::MessageCreateObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeCreateObject);
		create_owned_message = (cure::MessageCreateOwnedObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeCreateOwnedObject);
	}
	cure::MessageObjectPosition* position_message = (cure::MessageObjectPosition*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeObjectPosition);

	ContextTable::const_iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		// Don't send local objects.
		if (_object->GetNetworkObjectType() == cure::kNetworkObjectLocalOnly) {
			continue;
		}

		_packet->Clear();

		if (create) {	// Store creation info?
			xform transformation(_object->GetOrientation(), _object->GetPosition());
			if (_object->GetOwnerInstanceId() != 0) {
				_packet->AddMessage(create_owned_message);
				create_owned_message->Store(_packet, _object->GetInstanceId(), transformation,
					_object->GetClassId(), _object->GetOwnerInstanceId());
			} else {
				_packet->AddMessage(create_message);
				create_message->Store(_packet, _object->GetInstanceId(),
					 transformation, _object->GetClassId());
			}
		}

		const cure::ObjectPositionalData* _position;
		if (_object->UpdateFullPosition(_position)) {
			_packet->AddMessage(position_message);
			position_message->Store(_packet, _object->GetInstanceId(),
				GetTimeManager()->GetCurrentPhysicsFrame(), *_position);
		}

		// Send.
		GetNetworkAgent()->PlaceInSendBuffer(true, client->GetUserConnection()->GetSocket(), _packet);
	}

	_packet->Clear();
	if (create) {	// Sent creation info?
		GetNetworkAgent()->GetPacketFactory()->GetMessageFactory()->Release(create_owned_message);
		GetNetworkAgent()->GetPacketFactory()->GetMessageFactory()->Release(create_message);
	}
	GetNetworkAgent()->GetPacketFactory()->GetMessageFactory()->Release(position_message);
	GetNetworkAgent()->GetPacketFactory()->Release(_packet);
}

void GameServerManager::BroadcastCreateObject(cure::GameObjectId instance_id, const xform& transform, const str& class_id, cure::GameObjectId owner_instance_id) {
	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	if (owner_instance_id) {
		cure::MessageCreateOwnedObject* _create = (cure::MessageCreateOwnedObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeCreateOwnedObject);
		_packet->AddMessage(_create);
		_create->Store(_packet, instance_id, transform, class_id, owner_instance_id);
	} else {
		cure::MessageCreateObject* _create = (cure::MessageCreateObject*)GetNetworkAgent()->
			GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeCreateObject);
		_packet->AddMessage(_create);
		_create->Store(_packet, instance_id, transform, class_id);
	}
	BroadcastPacket(0, _packet, true);
	GetNetworkAgent()->GetPacketFactory()->Release(_packet);
}

void GameServerManager::BroadcastObjectPosition(cure::GameObjectId instance_id,
	const cure::ObjectPositionalData& position, Client* exclude_client, bool safe) {
	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	cure::MessageObjectPosition* _position = (cure::MessageObjectPosition*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeObjectPosition);
	_packet->AddMessage(_position);
	_position->Store(_packet, instance_id, GetTimeManager()->GetCurrentPhysicsFrame(), position);

	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
	ScopeLock tick_lock(GetTickLock());
	BroadcastPacket(exclude_client, _packet, safe);

	GetNetworkAgent()->GetPacketFactory()->Release(_packet);
}

bool GameServerManager::BroadcastChatMessage(const str& message) {
	return BroadcastStatusMessage(cure::MessageStatus::kInfoChat, message);
}

bool GameServerManager::BroadcastStatusMessage(cure::MessageStatus::InfoType type, const str& s) {
	bool _ok = false;
	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	{
		deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
		ScopeLock tick_lock(GetTickLock());
		ScopeLock net_lock(GetNetworkAgent()->GetLock());
		AccountClientTable::Iterator x = account_client_table_.First();
		for (; x != account_client_table_.End(); ++x) {
			const Client* _client = x.GetObject();
			_ok |= GetNetworkAgent()->SendStatusMessage(_client->GetUserConnection()->GetSocket(), 0,
				cure::kRemoteOk, type, s, _packet);
		}
	}
	GetNetworkAgent()->GetPacketFactory()->Release(_packet);
	return (_ok);
}

void GameServerManager::BroadcastNumberMessage(Client* exclude_client, bool safe, cure::MessageNumber::InfoType info, int32 integer, float32 _f) {
	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	cure::MessageNumber* number = (cure::MessageNumber*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeNumber);
	_packet->AddMessage(number);
	number->Store(_packet, info, integer, _f);
	BroadcastPacket(exclude_client, _packet, safe);
	GetNetworkAgent()->GetPacketFactory()->Release(_packet);
}

bool GameServerManager::SendChatMessage(const str& client_user_name, const str& message) {
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
	ScopeLock tick_lock(GetTickLock());
	ScopeLock net_lock(GetNetworkAgent()->GetLock());

	cure::UserAccount::AccountId _account_id;
	bool _ok = user_account_manager_->GetUserAccountId(client_user_name, _account_id);
	const Client* _client = 0;
	if (_ok) {
		_client = GetClientByAccount(_account_id);
		_ok = (_client != 0);
	}
	if (_ok) {
		cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
		_ok = GetNetworkAgent()->SendStatusMessage(_client->GetUserConnection()->GetSocket(), 0, cure::kRemoteOk,
			cure::MessageStatus::kInfoChat, message, _packet);
		GetNetworkAgent()->GetPacketFactory()->Release(_packet);
	}
	return (_ok);
}

void GameServerManager::IndicatePosition(const vec3 position, float time) {
	if (time <= 0) {
		return;
	}
	cure::ContextObject* _object = Parent::CreateContextObject("indicator", cure::kNetworkObjectLocallyControlled);
	_object->SetInitialTransform(xform(kIdentityQuaternionF, position));
	_object->StartLoading();
	GetContext()->DelayKillObject(_object, time);
}



int GameServerManager::GetLoggedInClientCount() const {
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner() || GetTickLock()->IsOwner());
	ScopeLock tick_lock(GetTickLock());
	ScopeLock net_lock(GetNetworkAgent()->GetLock());
	return (account_client_table_.GetCount());
}

Client* GameServerManager::GetClientByAccount(cure::UserAccount::AccountId account_id) const {
	Client* _client = account_client_table_.FindObject(account_id);
	return (_client);
}

Client* GameServerManager::GetClientByObject(cure::ContextObject*& object) const {
	Client* _client = 0;
	{
		cure::UserAccount::AccountId _account_id = (cure::UserAccount::AccountId)(intptr_t)object->GetExtraData();
		if (_account_id != 0 && _account_id != (cure::UserAccount::AccountId)-1) {
			_client = GetClientByAccount(_account_id);
			if (!_client) {
				log_.Errorf("Error: client seems to have logged off before avatar %s got loaded.",
					object->GetClassId().c_str());
				delete (object);
				object = 0;
			}
		}
	}
	return _client;
}

const GameServerManager::AccountClientTable& GameServerManager::GetAccountClientTable() const {
	return account_client_table_;
}


void GameServerManager::Build(const str& what) {
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
	ScopeLock tick_lock(GetTickLock());
	ScopeLock net_lock(GetNetworkAgent()->GetLock());

	for (int x = 0; x < 400 && !IsThreadSafe(); ++x) {
		if (x > 3) net_lock.Release();
		if (x > 7) tick_lock.Release();
		Thread::Sleep(0.005);
		if (x > 7) tick_lock.Acquire();
		if (x > 3) net_lock.Acquire();
	}
	if (!IsThreadSafe()) {
		log_.Error("Could never reach a thread-safe slice. Aborting construction.");
		return;
	}

	AccountClientTable::Iterator x = account_client_table_.First();
	for (; x != account_client_table_.End(); ++x) {
		const Client* _client = x.GetObject();
		cure::ContextObject* _object = GetContext()->GetObject(_client->GetAvatarId());
		if (_object) {
			vec3 _position = _object->GetPosition() + vec3(10, 0, 0);
			log_.Info("Building object '"+what+"' near user "+_client->GetUserConnection()->GetLoginName()+".");
			cure::ContextObject* _object = Parent::CreateContextObject(what, cure::kNetworkObjectLocallyControlled);
			_object->SetInitialTransform(xform(kIdentityQuaternionF, _position));
			_object->StartLoading();
		}
	}
}



void GameServerManager::TickInput() {
	TickMasterServer();
	MonitorRtvars();

	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	cure::NetworkAgent::ReceiveStatus received = cure::NetworkAgent::kReceiveOk;
	while (received != cure::NetworkAgent::kReceiveNoData) {
		cure::UserAccount::AccountId _account_id;
		received = GetNetworkServer()->ReceiveFirstPacket(_packet, _account_id);
		switch (received) {
			case cure::NetworkAgent::kReceiveOk: {
				Client* _client = GetClientByAccount(_account_id);
				deb_assert (_client);
				//log_volatile(log_.Debugf("Received data from %s.", _client->GetUserConnection()->GetLoginName().c_str()));
				cure::Packet::ParseResult parse_result;
				// Walk packets.
				do {
					// Walk messages.
					const int message_count = _packet->GetMessageCount();
					for (int x = 0; x < message_count; ++x) {
						cure::Message* _message = _packet->GetMessageAt(x);
						message_processor_->ProcessNetworkInputMessage(_client, _message);
					}
					parse_result = _packet->ParseMore();
					if (parse_result == cure::Packet::kParseShift) {
						received = GetNetworkServer()->ReceiveMore(_account_id, _packet);
						if (received == cure::NetworkAgent::kReceiveOk) {
							parse_result = _packet->ParseMore();
						} else {
							parse_result = cure::Packet::kParseError;
						}
					}
				} while (parse_result == cure::Packet::kParseOk);
				if (parse_result != cure::Packet::kParseNoData) {
					log_.Error("Problem with receiving crap extra packet!");
				}
			} break;
			case cure::NetworkAgent::kReceiveParseError: {
				log_.Error("Problem with receiving crap data!");
			} break;
			case cure::NetworkAgent::kReceiveConnectionBroken: {
				log_.Error("Disconnected from client!");
			} break;
			case cure::NetworkAgent::kReceiveNoData: {
				// Nothing, really.
			} break;
		}
	}
	GetNetworkAgent()->GetPacketFactory()->Release(_packet);

	// Apply buffered prediction movement.
	ApplyStoredMovement();
}



void GameServerManager::Logout(cure::UserAccount::AccountId account_id, const str& reason) {
	GetNetworkServer()->Disconnect(account_id, reason, true);
}

void GameServerManager::DeleteAllClients() {
	while (!account_client_table_.IsEmpty()) {
		cure::UserAccount::AccountId _account_id = account_client_table_.First().GetKey();
		Logout(_account_id, "Server shutdown");
	}
}



cure::UserAccount::Availability GameServerManager::QueryLogin(const cure::LoginId& login_id, cure::UserAccount::AccountId& account_id) {
	ScopeLock lock(GetNetworkAgent()->GetLock());
	return (user_account_manager_->GetUserAccountStatus(login_id, account_id));
}

void GameServerManager::OnLogin(cure::UserConnection* user_connection) {
	ScopeLock tick_lock(GetTickLock());
	ScopeLock lock(GetNetworkAgent()->GetLock());

	Client* _client = GetClientByAccount(user_connection->GetAccountId());
	deb_assert(!_client);
	if (_client) {
		log_.Errorf("user %s already has an account!", user_connection->GetLoginName().c_str());
		return;
	}

	typedef cure::UserAccount::AvatarIdSet AvatarIdSet;
	const AvatarIdSet* avatar_id_set = user_account_manager_->GetUserAvatarIdSet(user_connection->GetLoginName());
	if (avatar_id_set) {
		cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();

		_client = new Client(GetTimeManager(), GetNetworkAgent(), user_connection);
		account_client_table_.Insert(user_connection->GetAccountId(), _client);
		_client->SendPhysicsFrame(GetTimeManager()->GetCurrentPhysicsFrameAddFrames(2), _packet);	// TODO: adjust physics frame diff by ping-ponging some.
		str server_greeting;
		v_get(server_greeting, =, GetVariableScope(), kRtvarNetworkLogingreeting, "");
		_client->SendLoginCommands(_packet, server_greeting);

		for (AvatarIdSet::const_iterator x = avatar_id_set->begin(); x != avatar_id_set->end(); ++x) {
			const cure::UserAccount::AvatarId& _avatar_id = *x;
			_client->SendAvatar(_avatar_id, _packet);
		}

		GetNetworkAgent()->GetPacketFactory()->Release(_packet);

		SendObjects(_client, true, GetContext()->GetObjectTable());

		delegate_->OnLogin(_client);
	} else {
		log_.Error("User " + user_connection->GetLoginName() + " does not exist or is not allowed avatars!");
	}
}

void GameServerManager::OnLogout(cure::UserConnection* user_connection) {
	deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
	cure::GameObjectId _avatar_id = 0;
	ScopeLock tick_lock(GetTickLock());
	{
		ScopeLock net_lock(GetNetworkAgent()->GetLock());

		// TODO: logout with some timer, and also be able to reconnect the
		// client with his/her avatar again if logged in within the time frame.
		Client* _client = GetClientByAccount(user_connection->GetAccountId());
		deb_assert(_client);
		_avatar_id = _client->GetAvatarId();
		deb_assert(IsThreadSafe());
		account_client_table_.Remove(user_connection->GetAccountId());
		delegate_->OnLogout(_client);
		delete (_client);
	}
	DeleteContextObject(_avatar_id);

	log_.Info("User " + user_connection->GetLoginName() + " logged out.");
}



void GameServerManager::DeleteMovements(cure::GameObjectId instance_id) {
	for (size_t x = 0; x < (size_t)NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE; ++x) {
		MovementList& list = movement_array_list_[x];
		MovementList::iterator y = list.begin();
		for (; y != list.end(); ++y) {
			if ((*y)->GetObjectId() == instance_id) {
				delete (*y);
				list.erase(y);
				break;
			}
		}
	}
}

void GameServerManager::ApplyStoredMovement() {
	// Walk through all steps (including missed steps due to slow computer).
	int current_physics_steps = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (current_physics_steps >= NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE) {
		if (GetLoggedInClientCount() > 0) {
			log_.Warningf("Network positional buffer overrun: had to skip %i steps!", current_physics_steps-NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE+1);
		}
		current_physics_steps = NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE-1;
	}
	int current_physics_frame = GetTimeManager()->GetCurrentPhysicsFrameAddFrames(-current_physics_steps);
	if (current_physics_frame < 0) {
		return;
	}
	//log_.Debugf("[frame %i to %i]", current_physics_frame, GetTimeManager()->GetCurrentPhysicsFrame());
	for (; GetTimeManager()->GetCurrentPhysicsFrameDelta(current_physics_frame) >= 0;
		current_physics_frame = GetTimeManager()->GetPhysicsFrameAddFrames(current_physics_frame, 1)) {
		const int frame_cycle_index = current_physics_frame%NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE;
		MovementList movement_list = movement_array_list_[frame_cycle_index];
		LEPRA_DEBUG_CODE(size_t movement_count = movement_list.size();)
		movement_array_list_[frame_cycle_index].clear();
		LEPRA_DEBUG_CODE(deb_assert(movement_count == movement_list.size()));
		MovementList::iterator x = movement_list.begin();
		for (; x != movement_list.end(); ++x) {
			cure::MessageObjectMovement* _movement = *x;
			cure::GameObjectId _instance_id = _movement->GetObjectId();
			/*if (current_physics_frame != _movement->GetFrameIndex()) {
				log_.Warning("Throwing away network movement.");
			} else*/
			{
				cure::ContextObject* context_object = GetContext()->GetObject(_instance_id);
				if (context_object) {
					if (_movement->GetType() == cure::kMessageTypeObjectPosition) {
						const cure::ObjectPositionalData& data =
							((cure::MessageObjectPosition*)_movement)->GetPositionalData();
						context_object->SetFullPosition(data, 0);
					} else {
						log_.Error("Not implemented!");
					}
				}
			}
			delete _movement;
		}
	}
}



void GameServerManager::BroadcastAvatar(Client* client) {
	cure::ContextObject* _object = GetContext()->GetObject(client->GetAvatarId());
	cure::GameObjectId _instance_id = _object->GetInstanceId();
	log_.Info("User "+client->GetUserConnection()->GetLoginName()+" login complete (avatar loaded).");

	// TODO: this is hard-coded. Use a general replication-mechanism instead (where visible and added/updated objects gets replicated automatically).
	GetNetworkAgent()->SendNumberMessage(true, client->GetUserConnection()->GetSocket(),
		cure::MessageNumber::kInfoAvatar, _instance_id, 0);

	BroadcastCreateObject(_object);
}



cure::ContextObject* GameServerManager::CreateContextObject(const str& class_id) const {
	return delegate_->CreateContextObject(class_id);
}

void GameServerManager::OnLoadCompleted(cure::ContextObject* object, bool ok) {
	Client* _client = GetClientByObject(object);
	if (!object) {
		return;
	}
	if (ok) {
		DeleteMovements(object->GetInstanceId());
		if (_client || object->GetExtraData() == (void*)-1L) {
			delegate_->OnLoadAvatar(_client, object);
			if (_client) {
				log_.Infof("Loaded avatar for %s with instance id %i.",
					_client->GetUserConnection()->GetLoginName().c_str(),
					object->GetInstanceId());
				/*const quat q = object->GetOrientation();
				log_.Infof("Avatar %s/%i has q=(%f, %f, %f, %f."),
					_client->GetUserConnection(->GetLoginName()).c_str(),
					object->GetInstanceId(),
					q.a, q.b, q.c, q.d);*/
				BroadcastAvatar(_client);
			} else {
				object->SetExtraData(0);
			}
		} else {
			log_volatile(log_.Debugf("Loaded object %s.", object->GetClassId().c_str()));
			delegate_->OnLoadObject(object);
			BroadcastCreateObject(object);
		}
	} else {
		if (_client) {
			log_.Errorf("Could not load avatar of type %s for user %s.",
				object->GetClassId().c_str(), _client->GetUserConnection()->GetLoginName().c_str());
		} else {
			log_.Errorf("Could not load object of type %s.", object->GetClassId().c_str());
		}
		GetContext()->PostKillObject(object->GetInstanceId());
	}
}

void GameServerManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) {
	ScopeLock lock(GetTickLock());

	(void)position;
	(void)body1_id;
	(void)body2_id;

	const bool object1_dynamic = (object1->GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic);
	const bool object2_dynamic = (object2->GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic);
	const bool both_are_dynamic = (object1_dynamic && object2_dynamic);
	if (!both_are_dynamic) {
		if (object1_dynamic && object1->GetPhysics()->GetGuideMode() >= tbc::ChunkyPhysics::kGuideExternal) {
			FlipCheck(object1);
		}
		return;
	}

	if (object1 != object2 &&	// I.e. car where a wheel collides with the body.
		object1->GetNetworkObjectType() != cure::kNetworkObjectLocalOnly) {	// We only handle network object collisions.
		if (object1->GetBorrowerInstanceId() == object2->GetInstanceId() ||
			object2->GetBorrowerInstanceId() == object1->GetInstanceId()) {
			return;	// The client knows best what collisions have happened.
		}

		bool send_collision = false;
		const bool are_both_controlled = (object2 != 0 && object2->GetNetworkObjectType() != cure::kNetworkObjectLocalOnly);
		const bool is_server_controlled = (object1->GetNetworkObjectType() == cure::kNetworkObjectLocallyControlled);
		if (is_server_controlled) {
			send_collision = (object1->GetImpact(GetPhysicsManager()->GetGravity(), force, torque) >= 2.0f);
		} else if (are_both_controlled) {
			if (object1->GetBorrowerInstanceId() == object2->GetInstanceId() ||
				object2->GetBorrowerInstanceId() == object1->GetInstanceId()) {
				// An avatar collides against an object that she owns, let remote end handle!
			} else {
				send_collision = (object1->GetImpact(GetPhysicsManager()->GetGravity(), force, torque) >= 2.0f);
			}
		}
		if (send_collision) {
			/*// We have found a collision. Asynchronously inform all viewers, including the colliding client.
			if (object1->GetNetworkObjectType() == cure::kNetworkObjectLocallyControlled) {
				object1->SetSendCount(3);
			}*/
			GetContext()->AddPhysicsSenderObject(object1);
		}
	}
}

void GameServerManager::FlipCheck(cure::ContextObject* object) const {
	bool auto_flip_enabled;
	v_get(auto_flip_enabled, =, GetVariableScope(), kRtvarGameAutoflipenabled, true);
	if (!auto_flip_enabled) {
		return;
	}

	// Check if we've landed on our side.
	vec3 up(0, 0, 1);
	up = object->GetOrientation() * up;
	if (up.z > 0.2f ||
		object->GetVelocity().GetLengthSquared() > 1*1 ||
		object->GetAngularVelocity().GetLengthSquared() > 1*1) {
		// Nope, still standing, or at least moving. Might be drunken style,
		// but at least not on it's head yet.
		return;
	}
	// A grown-up still activating an engine = leave 'em be.
	if (!object->IsAttributeTrue("float_childishness")) {
		const int engine_count = object->GetPhysics()->GetEngineCount();
		for (int x = 0; x < engine_count; ++x) {
			if (::fabs(object->GetPhysics()->GetEngine(x)->GetValue()) > 0.6f) {
				return;
			}
		}
	}

	// Yup, reset vehicle in the direction it was heading.
	const cure::ObjectPositionalData* original_position_data;
	if (object->UpdateFullPosition(original_position_data)) {
		cure::ObjectPositionalData position_data;
		position_data.CopyData(original_position_data);
		position_data.Stop();
		xform& _transform = position_data.position_.transformation_;
		_transform.SetPosition(object->GetPosition() + vec3(0, 0, 5));
		vec3 euler_angles;
		object->GetOrientation().GetEulerAngles(euler_angles);
		_transform.GetOrientation().SetEulerAngles(euler_angles.x, 0, 0);
		_transform.GetOrientation() *= object->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		object->SetFullPosition(position_data, 0);
		GetContext()->AddPhysicsSenderObject(object);
	} else {
		log_.Errorf("Error: vehicle %i could not be flipped!", object->GetInstanceId());
	}
}

bool GameServerManager::OnPhysicsSend(cure::ContextObject* object) {
	if (object->GetNetworkObjectType() == cure::kNetworkObjectLocalOnly) {
		return true;
	}

	bool last_send = false;
	float send_interval_limit;
	v_get(send_interval_limit, =(float), GetVariableScope(), kRtvarNetphysPossendintervallimit, 0.5);
	if (object->QueryResendTime(send_interval_limit, false)) {
		last_send = true;
		log_volatile(log_.Debugf("Sending pos for %s (%u).", object->GetClassId().c_str(), object->GetInstanceId()));
		const cure::ObjectPositionalData* _position = 0;
		if (object->UpdateFullPosition(_position)) {
			BroadcastObjectPosition(object->GetInstanceId(), *_position, 0, false);
			last_send = (object->PopSendCount() == 0);
		}

	}
	return (last_send);
}

bool GameServerManager::OnAttributeSend(cure::ContextObject* object) {
	log_debug("Sending attribute(s) for a context object...");
	typedef cure::ContextObject::AttributeArray AttributeArray;
	const AttributeArray& attributes = object->GetAttributes();
	AttributeArray::const_iterator x = attributes.begin();
	for (; x != attributes.end(); ++x) {
		cure::ContextObjectAttribute* attribute = *x;
		const int send_size = attribute->QuerySend();
		if (send_size > 0) {
			cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
			cure::MessageObjectAttribute* attrib_message = (cure::MessageObjectAttribute*)GetNetworkAgent()->
				GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeObjectAttribute);
			_packet->AddMessage(attrib_message);
			attribute->Pack(attrib_message->GetWriteBuffer(_packet, object->GetInstanceId(), send_size));

			deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
			ScopeLock tick_lock(GetTickLock());
			switch (attribute->GetNetworkType()) {
				case cure::ContextObjectAttribute::kTypeServerBroadcast:
				case cure::ContextObjectAttribute::TYPE_BOTH_BROADCAST: {
					BroadcastPacket(0, _packet, true);
				} break;
				default: {
					cure::UserAccount::AccountId _account_id = (cure::UserAccount::AccountId)(intptr_t)object->GetExtraData();
					if (!_account_id) {
						log_.Error("Error: trying to attribute sync to avatar without avatar!");
						break;
					}
					const Client* _client = GetClientByAccount(_account_id);
					if (!_client) {
						log_.Error("Error: client seems to have logged off before attribute sync happened.");
						break;
					}
					cure::NetworkAgent::VSocket* socket = _client->GetUserConnection()->GetSocket();
					if (socket) {
						GetNetworkAgent()->PlaceInSendBuffer(true, socket, _packet);
					}
				} break;
			}
			GetNetworkAgent()->GetPacketFactory()->Release(_packet);
		}
	}
	return true;
}

bool GameServerManager::IsServer() {
	return (true);
}

void GameServerManager::SendAttach(cure::ContextObject* object1, unsigned id1,
	cure::ContextObject* object2, unsigned id2) {
	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	cure::MessageObjectAttach* attach = (cure::MessageObjectAttach*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeObjectAttach);
	_packet->AddMessage(attach);
	attach->Store(_packet, object1->GetInstanceId(), object2->GetInstanceId(), (uint16)id1, (uint16)id2);

	BroadcastPacket(0, _packet, true);

	GetNetworkAgent()->GetPacketFactory()->Release(_packet);
}

void GameServerManager::SendDetach(cure::ContextObject* object1, cure::ContextObject* object2) {
	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	cure::MessageObjectDetach* detach = (cure::MessageObjectDetach*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeObjectDetach);
	_packet->AddMessage(detach);
	detach->Store(_packet, object1->GetInstanceId(), object2->GetInstanceId());

	BroadcastPacket(0, _packet, true);

	GetNetworkAgent()->GetPacketFactory()->Release(_packet);
}

void GameServerManager::OnIdOwnershipExpired(int, cure::ContextObject* object, void*) {
	object->SetNetworkObjectType(cure::kNetworkObjectLocallyControlled);
	object->SetBorrowerInstanceId(0);
}

void GameServerManager::HandleWorldBoundaries() {
	std::vector<cure::GameObjectId> lost_object_array;
	const ContextTable& _object_table = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = _object_table.begin();
	for (; x != _object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		if (_object->IsLoaded() && _object->GetPhysics()) {
			const vec3 _position = _object->GetPosition();
			if (!Math::IsInRange(_position.x, -2000.0f, +2000.0f) ||
				!Math::IsInRange(_position.y, -2000.0f, +2000.0f) ||
				!Math::IsInRange(_position.z, -1000.0f, +800.0f)) {
				lost_object_array.push_back(_object->GetInstanceId());
			}
		}
	}
	if (!lost_object_array.empty()) {
		ScopeLock lock(GetTickLock());
		std::vector<cure::GameObjectId>::const_iterator y = lost_object_array.begin();
		for (; y != lost_object_array.end(); ++y) {
			DeleteContextObject(*y);
		}
	}
}



cure::ContextObject* GameServerManager::CreateLogicHandler(const str& type) {
	if (type == "trig_elevator") {
		return new cure::Elevator(GetContext());
	} else if (type == "spawner") {
		return new Spawner(GetContext());
	} else if (type == "real_time_ratio") {
		return new BulletTime(GetContext());
	} else if (type == "race_timer") {
		return new RaceTimer(GetContext());
	}
	return (0);
}



void GameServerManager::BroadcastCreateObject(cure::ContextObject* object) {
	if (object->GetNetworkObjectType() == cure::kNetworkObjectLocalOnly) {
		return;
	}

	bool is_engine_controlled = false;
	xform _transform;
	if (object->GetPhysics()) {
		tbc::ChunkyBoneGeometry* structure_geometry = object->GetPhysics()->GetBoneGeometry(object->GetPhysics()->GetRootBone());
		tbc::PhysicsManager::BodyID body = structure_geometry->GetBodyId();
		GetPhysicsManager()->GetBodyTransform(body, _transform);
		is_engine_controlled = (object->GetPhysics()->GetEngineCount() > 0);
	}
	BroadcastCreateObject(object->GetInstanceId(), _transform, object->GetClassId(), object->GetOwnerInstanceId());
	OnAttributeSend(object);
	if (is_engine_controlled || object->GetVelocity().GetLengthSquared() > 0.1f) {
		OnPhysicsSend(object);
	}
}

void GameServerManager::BroadcastDeleteObject(cure::GameObjectId instance_id) {
	cure::Packet* _packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	cure::MessageDeleteObject* __delete = (cure::MessageDeleteObject*)GetNetworkAgent()->
		GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeDeleteObject);
	_packet->AddMessage(__delete);
	__delete->Store(_packet, instance_id);
	AccountClientTable::Iterator x = account_client_table_.First();
	for (; x != account_client_table_.End(); ++x) {
		const Client* _client = x.GetObject();
		cure::NetworkAgent::VSocket* socket = _client->GetUserConnection()->GetSocket();
		if (socket) {
			GetNetworkAgent()->PlaceInSendBuffer(true, socket, _packet);
		}
	}
	GetNetworkAgent()->GetPacketFactory()->Release(_packet);
}

void GameServerManager::BroadcastPacket(const Client* exclude_client, cure::Packet* packet, bool safe) {
	AccountClientTable::Iterator x = account_client_table_.First();
	for (; x != account_client_table_.End(); ++x) {
		const Client* _client = x.GetObject();
		if (_client != exclude_client) {
			GetNetworkAgent()->PlaceInSendBuffer(safe, _client->GetUserConnection()->GetSocket(), packet);
		}
	}
}



void GameServerManager::TickMasterServer() {
	bool is_open_server;
	v_get(is_open_server, =, GetVariableScope(), kRtvarNetworkEnableopenserver, false);
	bool publish_server;
	v_get(publish_server, =, GetVariableScope(), kRtvarNetworkPublishserver, false);
	if (!master_connection_ || !is_open_server || !publish_server) {
		return;
	}

	static str local_ip_address;
	if (local_ip_address.empty()) {
		IPAddress ip_address;
		if (Network::ResolveHostname("", ip_address)) {
			local_ip_address = ip_address.GetAsString();
		} else {
			local_ip_address = GetNetworkServer()->GetLocalAddress().GetIP().GetAsString();
		}
	}

	str server_name;
	v_get(server_name, =, GetVariableScope(), kRtvarNetworkServername, "?");
	const str player_count = strutil::IntToString(GetLoggedInClientCount(), 10);
	const str id = strutil::ReplaceAll(SystemManager::GetSystemPseudoId(), "\"", "''\\''");
	const str local_server_info = "--name \"" + server_name + "\" --player-count " + player_count
		+ " --id \"" + id + "\" --internal-address " + local_ip_address +
		" --internal-port " + strutil::IntToString(GetNetworkServer()->GetLocalAddress().GetPort(), 10);
	float connect_timeout;
	v_get(connect_timeout, =(float), GetVariableScope(), kRtvarNetworkConnectTimeout, 3.0);
	master_connection_->SetSocketInfo(GetNetworkServer(), connect_timeout);
	master_connection_->SendLocalInfo(local_server_info);

	ServerInfo _server_info;
	if (master_connection_->TickReceive(_server_info)) {
		HandleMasterCommand(_server_info);
	}
}


bool GameServerManager::HandleMasterCommand(const ServerInfo& server_info) {
	if (server_info.command_ == kMasterServerOf) {
		const str _address = server_info.given_ip_address_ + strutil::Format(":%u", server_info.given_port_);
		SocketAddress socket_address;
		if (socket_address.Resolve(_address)) {
			log_.Infof("Attempting to open local firewall to allow remote client connect from %s.", _address.c_str());
			cure::SocketIoHandler* socket_io_handler = GetNetworkServer();
			return socket_io_handler->GetMuxIoSocket()->SendOpenFirewallData(socket_address);
		}
	} else {
		log_.Errorf("Got bad command (%s from master server)!", server_info.command_.c_str());
		deb_assert(false);
	}
	return false;
}

void GameServerManager::MonitorRtvars() {
	{
		int physics_fps;
		v_get(physics_fps, =, GetVariableScope(), kRtvarPhysicsFps, PHYSICS_FPS);
		if (physics_fps != physics_fps_shadow_) {
			physics_fps_shadow_ = physics_fps;
			BroadcastStatusMessage(cure::MessageStatus::kInfoCommand,
				strutil::Format("#" kRtvarPhysicsFps " %i;", physics_fps));
		}
	}
	{
		float physics_rtr;
		v_get(physics_rtr, =(float), GetVariableScope(), kRtvarPhysicsRtr, 1.0);
		if (!Math::IsEpsEqual(physics_rtr_shadow_, physics_rtr, 0.01f)) {
			physics_rtr_shadow_ = physics_rtr;
			BroadcastStatusMessage(cure::MessageStatus::kInfoCommand,
				strutil::Format("#" kRtvarPhysicsRtr " %f;", physics_rtr));
		}
	}
	{
		bool physics_halt;
		v_get(physics_halt, =, GetVariableScope(), kRtvarPhysicsHalt, false);
		if (physics_halt != physics_halt_shadow_) {
			physics_halt_shadow_ = physics_halt;
			BroadcastStatusMessage(cure::MessageStatus::kInfoCommand,
				strutil::Format("#" kRtvarPhysicsRtr " %s;", strutil::BoolToString(physics_halt).c_str()));
		}
	}
}



loginstance(kGame, GameServerManager);



}
