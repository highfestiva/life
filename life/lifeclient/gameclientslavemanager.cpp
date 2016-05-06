
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "gameclientslavemanager.h"
#include <algorithm>
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/contextobjectattribute.h"
#include "../../cure/include/networkclient.h"
#include "../../cure/include/resourcemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/network.h"
#include "../../lepra/include/number.h"
#include "../../lepra/include/stringutility.h"
#include "../../uicure/include/uicppcontextobject.h"
#include "../../uicure/include/uidebugrenderer.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uicure/include/uiruntimevariablename.h"
#include "../lifeserver/masterserverconnection.h"
#include "../lifeapplication.h"
#include "clientconsolemanager.h"
#include "gameclientmasterticker.h"
#include "rtvar.h"
#include "uiconsole.h"
#include "uigameservermanager.h"



namespace life {



const int ID_OWNERSHIP_LOAN_EXPIRES = 1;



GameClientSlaveManager::GameClientSlaveManager(GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	cure::GameManager(time, variable_scope, resource_manager),
	ui_manager_(ui_manager),
	slave_index_(slave_index),
	render_area_(render_area),
	master_server_connection_(0),
	is_reset_(false),
	is_reset_complete_(false),
	quit_(false),
	last_sent_byte_count_(0),
	ping_attempt_count_(0),
	allow_movement_input_(true),
	options_(variable_scope, slave_index) {
	v_set(GetVariableScope(), kRtvarCtrlMousesensitivity, 4.0f);
	v_set(GetVariableScope(), kRtvarCtrlMousefilter, -1.0f);	// Disable (optimization).
	v_set(GetVariableScope(), kRtvarGameChildishness, 1.0);

	SetTicker(pMaster);

	SetNetworkAgent(new cure::NetworkClient(GetVariableScope()));
}

GameClientSlaveManager::~GameClientSlaveManager() {
	Close();

	SetMasterServerConnection(0);
	SetTicker(0);

	if (GetConsoleManager()) {
		GetConsoleManager()->Join();
	}
}

void GameClientSlaveManager::SetMasterServerConnection(MasterServerConnection* connection) {
	delete master_server_connection_;
	master_server_connection_ = connection;
}

void GameClientSlaveManager::Resume(bool hard) {
	(void)hard;
}

void GameClientSlaveManager::Suspend(bool hard) {
	(void)hard;
}

void GameClientSlaveManager::LoadSettings() {
	str external_server_address;
	v_get(external_server_address, =, UiCure::GetSettings(), kRtvarNetworkServeraddress, "localhost:16650");
	GetConsoleManager()->ExecuteCommand("alias gfx-lo \"#" kRtvarUi3DPixelshaders " false; #" kRtvarUi3DShadows " No; #" kRtvarUi3DEnablemassobjects " false; #" kRtvarUi3DEnablemassobjectfading " false; #" kRtvarUi3DEnableparticles " false\"");
	GetConsoleManager()->ExecuteCommand("alias gfx-hi \"#" kRtvarUi3DPixelshaders " true; #" kRtvarUi3DShadows " Force:Volumes; #" kRtvarUi3DEnablemassobjects " true; #" kRtvarUi3DEnablemassobjectfading " true; #" kRtvarUi3DEnableparticles " true\"");
	GetConsoleManager()->ExecuteCommand("alias iphone4-settings \"#" kRtvarUiDisplayWidth " 960; #" kRtvarUiDisplayHeight " 640; #" kRtvarCtrlEmulatetouch " true; start-reset-ui\"");
	GetConsoleManager()->ExecuteCommand("alias iphone5-settings \"#" kRtvarUiDisplayWidth " 1136; #" kRtvarUiDisplayHeight " 640; #" kRtvarCtrlEmulatetouch " true; start-reset-ui\"");
	GetConsoleManager()->ExecuteCommand("alias ipad2-settings \"#" kRtvarUiDisplayWidth " 1024; #" kRtvarUiDisplayHeight " 768; #" kRtvarCtrlEmulatetouch " true; start-reset-ui\"");
	GetConsoleManager()->ExecuteCommand("alias ipad3-settings \"#" kRtvarUiDisplayWidth " 2048; #" kRtvarUiDisplayHeight " 1536; #" kRtvarCtrlEmulatetouch " true; start-reset-ui\"");
	GetConsoleManager()->ExecuteCommand("alias computer-settings \"#" kRtvarUiDisplayWidth " 960; #" kRtvarUiDisplayHeight " 540; #" kRtvarCtrlEmulatetouch " false; start-reset-ui\"");
#ifdef LEPRA_WINDOWS
#define kShellRebuildData		"cd .. & Tools\\build\\rgo.py --pause-on-error builddata"
#else
#define kShellRebuildData		"cd ..; Tools/build/rgo.py --pause-on-error builddata"
#endif // Windows shell / Posix shell
	GetConsoleManager()->ExecuteCommand("alias rebuild-data shell-execute \"" kShellRebuildData "\"");
	GetConsoleManager()->ExecuteCommand("alias zzz \"zombie nop\"");
#ifdef LEPRA_DEBUG
	GetConsoleManager()->ExecuteCommand("bind-key F5 zzz");
#endif // Debug
	GetConsoleManager()->ExecuteCommand("execute-file -i "+GetApplicationCommandFilename());
	// Always default these settings, to avoid that the user can't get rid of undesired behavior.
	v_set(UiCure::GetSettings(), kRtvarDebugEnable, false);
	v_set(UiCure::GetSettings(), kRtvarDebugInputPrint, false);
	bool is_server_selected;
	v_tryget(is_server_selected, =, UiCure::GetSettings(), kRtvarLoginIsserverselected, false);
	if (is_server_selected) {
		v_set(UiCure::GetSettings(), kRtvarNetworkServeraddress, external_server_address);
	} else {
		str _server_address;
		v_get(_server_address, =, UiCure::GetSettings(), kRtvarNetworkServeraddress, "localhost:16650");
		if (Network::IsLocalAddress(_server_address)) {
			bool is_open_server;
			v_get(is_open_server, =, GetVariableScope(), kRtvarNetworkEnableopenserver, false);
			const bool is_currently_localhost = (strutil::StartsWith(_server_address, "localhost:") || strutil::StartsWith(_server_address, "127.0.0.1:"));
			if (is_open_server) {
				if (is_currently_localhost) {
					v_set(UiCure::GetSettings(), kRtvarNetworkServeraddress, "0.0.0.0:16650");
				}
			} else {
				if (!is_currently_localhost) {
					v_set(UiCure::GetSettings(), kRtvarNetworkServeraddress, "localhost:16650");
				}
			}
		}
	}
	v_set(UiCure::GetSettings(), kRtvarPhysicsFps, PHYSICS_FPS);
	v_set(UiCure::GetSettings(), kRtvarPhysicsRtr, 1.0);
	v_set(UiCure::GetSettings(), kRtvarPhysicsHalt, false);
}

void GameClientSlaveManager::RefreshOptions() {
	options_.DoRefreshConfiguration();
}

void GameClientSlaveManager::SetRenderArea(const PixelRect& render_area) {
	render_area_ = render_area;
	((ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->SetRenderArea(render_area);
}

bool GameClientSlaveManager::Open() {
	Close();
	ScopeLock lock(GetTickLock());
	bool ok = Reset();
	if (ok) {
		ok = GetConsoleManager()->Start();
	}
	return (ok);
}

void GameClientSlaveManager::Close() {
	ScopeLock lock(GetTickLock());
	// Drop all physics and renderer objects.
	GetContext()->ClearObjects();
	if (GetConsoleManager()) {
		GetConsoleManager()->Join();
	}
}

bool GameClientSlaveManager::IsQuitting() const {
	return (quit_);
}

void GameClientSlaveManager::SetIsQuitting() {
	log_.Headlinef("Slave %i will quit.", GetSlaveIndex());
	if (Thread::GetCurrentThread()->GetThreadName() == "MainThread") {
		GetResourceManager()->Tick();
	}
	quit_ = true;
	v_internal(UiCure::GetSettings(), kRtvarLoginIsserverselected, false);
}



GameClientMasterTicker* GameClientSlaveManager::GetMaster() const {
	return (GameClientMasterTicker*)GetTicker();
}



bool GameClientSlaveManager::Render() {
	ScopeLock lock(GetTickLock());

	UpdateCameraPosition(true);

	float _fov;
	v_get(_fov, =(float), GetVariableScope(), kRtvarUi3DFov, 45.0);
	UpdateFrustum(_fov);

	LEPRA_MEASURE_SCOPE(SlaveRender);
	bool outline;
	bool wire_frame;
	float line_width;
	v_get(outline, =, GetVariableScope(), kRtvarUi3DOutlinemode, false);
	v_get(wire_frame, =, GetVariableScope(), kRtvarUi3DWireframemode, false);
	v_get(line_width, =(float), GetVariableScope(), kRtvarUi3DLinewidth, 3.0);
	SetLocalRender(true);
	ui_manager_->GetRenderer()->EnableOutlineRendering(outline);
	ui_manager_->GetRenderer()->EnableWireframe(wire_frame);
	ui_manager_->GetRenderer()->SetLineWidth(line_width);
	ui_manager_->Render(render_area_);
	SetLocalRender(false);	// Hide sun and mass objects from other cameras.

	return (true);
}

bool GameClientSlaveManager::Paint() {
	return true;
}

bool GameClientSlaveManager::IsPrimaryManager() const {
	if (GetMaster()->IsLocalServer()) {
		return false;
	}
	return GetMaster()->IsFirstSlave(this);
}

void GameClientSlaveManager::PreEndTick() {
	Parent::PreEndTick();

	HandleUnusedRelativeAxis();
}

bool GameClientSlaveManager::EndTick() {
	bool is_debug_drawing = ui_manager_->CanRender();
	v_get(is_debug_drawing, &=, GetVariableScope(), kRtvarDebugEnable, false);

	if (is_debug_drawing) {
		DrawAsyncDebugInfo();
	}

	bool ok = Parent::EndTick();
	if (ok) {
		if (is_debug_drawing) {
			DrawSyncDebugInfo();
		}
		TickUiUpdate();
	}
	return (ok);
}

void GameClientSlaveManager::TickNetworkInput() {
	if (GetNetworkClient()->GetSocket() == 0) {
		if (!GetNetworkClient()->IsConnecting() && !is_reset_) {
			Reset();
		}
		return;	// TRICKY: easy way out.
	}

	cure::Packet* packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	cure::NetworkAgent::ReceiveStatus received = GetNetworkClient()->ReceiveNonBlocking(packet);
	switch (received) {
		case cure::NetworkAgent::kReceiveOk: {
			//log_volatile(log_.Debugf("%s received data from server.", GetName().c_str()));
			cure::Packet::ParseResult parse_result;
			// Walk packets.
			do {
				// Walk messages.
				const int message_count = packet->GetMessageCount();
				for (int x = 0; x < message_count; ++x) {
					cure::Message* _message = packet->GetMessageAt(x);
					//log_volatile(log_.Tracef("Received message of type %i.", _message->GetType()));
					ProcessNetworkInputMessage(_message);
				}
				parse_result = packet->ParseMore();
				if (parse_result == cure::Packet::kParseShift) {
					received = GetNetworkClient()->ReceiveMore(packet);
					if (received == cure::NetworkAgent::kReceiveOk) {
						parse_result = packet->ParseMore();
					} else {
						parse_result = cure::Packet::kParseError;
					}
				}
			} while (parse_result == cure::Packet::kParseOk);
			if (parse_result == cure::Packet::kParseNoData) {
				last_unsafe_receive_time_.ClearTimeDiff();
			} else {
				log_.Error("Problem with receiving crap extra packet!");
			}
		} break;
		case cure::NetworkAgent::kReceiveParseError: {
			log_.Error("Problem with receiving crap data!");
		} break;
		case cure::NetworkAgent::kReceiveConnectionBroken: {
			disconnect_reason_ = "Server abrubtly disconnected!";
			log_.Error("Disconnected from server!");
			is_reset_ = false;
			GetNetworkClient()->Disconnect(false);
		} break;
		case cure::NetworkAgent::kReceiveNoData: {
			// Nothing, really.
		} break;
	}
	GetNetworkAgent()->GetPacketFactory()->Release(packet);
}

bool GameClientSlaveManager::TickNetworkOutput() {
	bool send_ok = true;
	bool is_sent = false;
	if (GetNetworkClient()->GetSocket()) {
		// Check if we should send client keepalive (keepalive is simply a position update).
		bool force_send_unsafe_client_keepalive = false;
		last_send_time_.UpdateTimer();
		double keepalive_interval;
		v_get(keepalive_interval, =, GetVariableScope(), kRtvarNetworkKeepaliveSendinterval, 1.0);
		if (last_sent_byte_count_ != GetNetworkAgent()->GetSentByteCount()) {
			last_sent_byte_count_ = GetNetworkAgent()->GetSentByteCount();
			last_send_time_.ClearTimeDiff();
		} else if (last_send_time_.GetTimeDiff() >= keepalive_interval) {
			force_send_unsafe_client_keepalive = true;
		}

		// Check if we should send updates. Send all owned objects at the same time to avoid penetration.
		float pos_send_interval_limit;
		v_get(pos_send_interval_limit, =(float), GetVariableScope(), kRtvarNetphysPossendintervallimit, 0.5);
		pos_send_interval_limit *= 0.5f;	// Sampling theorem.
		bool send = false;
		ObjectIdSet::iterator x = owned_object_list_.begin();
		for (; x != owned_object_list_.end(); ++x) {
			cure::ContextObject* _object = GetContext()->GetObject(*x);
			if (_object) {
				_object->SetNetworkObjectType(cure::kNetworkObjectLocallyControlled);
				if (!_object->QueryResendTime(pos_send_interval_limit, false)) {
					continue;
				}
				const cure::ObjectPositionalData* positional_data = 0;
				if (_object->UpdateFullPosition(positional_data)) {
					if (!positional_data->IsSameStructure(*_object->GetNetworkOutputGhost())) {
						_object->GetNetworkOutputGhost()->CopyData(positional_data);
					}
					const bool is_collision_expired = collision_expire_alarm_.PopExpired(0.6);
					const bool is_input_expired = input_expire_alarm_.PopExpired(0.0);
					const bool is_position_expired = (is_collision_expired || is_input_expired);
					if (is_position_expired) {
						log_trace("Position expires.");
					}
					const bool is_allwed_diff_send = send_expire_alarm_.IsExpired(0.5);

					float resync_on_diff;
					v_get(resync_on_diff, =(float), GetVariableScope(), kRtvarNetphysResyncondiffgt, 0.2);
					if (force_send_unsafe_client_keepalive ||
						is_position_expired ||
						(is_allwed_diff_send &&
						positional_data->GetScaledDifference(_object->GetNetworkOutputGhost()) > resync_on_diff)) {
						//if (force_send_unsafe_client_keepalive)	log_.Info("POS_SEND: Force pos send!");
						//if (is_position_expired)			log_.Info("POS_SEND: Pos expired causing pos send!");
						//if (is_allwed_diff_send)			log_.Info("POS_SEND: Diff causing pos send!");
						send_expire_alarm_.Set();
						send = true;
						break;
					}
				}
			}
		}
		if (send) {
			ObjectIdSet::iterator x = owned_object_list_.begin();
			for (; x != owned_object_list_.end(); ++x) {
				cure::ContextObject* _object = GetContext()->GetObject(*x);
				if (_object) {
					const cure::ObjectPositionalData* positional_data = 0;
					if (_object->UpdateFullPosition(positional_data)) {
						_object->GetNetworkOutputGhost()->CopyData(positional_data);
						send_ok = GetNetworkAgent()->SendObjectFullPosition(GetNetworkClient()->GetSocket(),
							_object->GetInstanceId(), GetTimeManager()->GetCurrentPhysicsFrame(), *_object->GetNetworkOutputGhost());
						is_sent = true;

						v_internal_arithmetic(GetVariableScope(), kRtvarDebugNetSendposcnt, int, +, 1, 0, 1000000);

						/*for (int x = 0; x < _object->GetPhysics()->GetEngineCount(); ++x) {
							tbc::PhysicsEngine* engine = _object->GetPhysics()->GetEngine(x);
							log_volatile(log_.Debugf("Sync'ed engine of type %i with value %f.", engine->GetEngineType(), engine->GetValue()));
						}*/
					}
				}
			}
		}

		// Check if we should send server check-up (uses message similar to ping).
		if (send_ok && !GetNetworkClient()->IsLoggingIn()) {
			last_unsafe_receive_time_.UpdateTimer();
			double ping_interval;
			v_get(ping_interval, =, GetVariableScope(), kRtvarNetworkKeepalivePinginterval, 7.0);
			if ((!is_sent && force_send_unsafe_client_keepalive) ||
				last_unsafe_receive_time_.GetTimeDiff() >= ping_interval) {
				int ping_retry_count;
				v_get(ping_retry_count, =, GetVariableScope(), kRtvarNetworkKeepalivePingretrycount, 4);
				if (++ping_attempt_count_ <= ping_retry_count) {
					last_unsafe_receive_time_.ReduceTimeDiff(ping_interval);
					log_volatile(log_.Debugf("Slave %i sending ping.", slave_index_));
					send_ok = GetNetworkAgent()->SendNumberMessage(false, GetNetworkClient()->GetSocket(),
						cure::MessageNumber::kInfoPing, GetTimeManager()->GetCurrentPhysicsFrame(), 0);
				} else {
					disconnect_reason_ = "Server not responding!";
					log_.Error("Server is not responding to ping! Disconnecting now!");
					GetNetworkClient()->Disconnect(true);
				}
			}
		}
	}
	if (send_ok) {
		send_ok = Parent::TickNetworkOutput();
	}

	if (master_server_connection_) {
		float connect_timeout;
		v_get(connect_timeout, =(float), GetVariableScope(), kRtvarNetworkConnectTimeout, 3.0);
		master_server_connection_->SetSocketInfo(send_ok? GetNetworkClient() : 0, connect_timeout);
		master_server_connection_->Tick();
	}

	// If we were unable to send to server, we conclude that it has silently died.
	if (!send_ok) {
		disconnect_reason_ = "Connection to server died!";
		log_.Error("Server seems dead! Disconnecting silently.");
		GetNetworkClient()->Disconnect(false);
	}
	return (send_ok);
}

void GameClientSlaveManager::TickNetworkOutputGhosts() {
	const int step_count = GetTimeManager()->GetAffordedPhysicsStepCount();
	const float physics_frame_time = GetTimeManager()->GetAffordedPhysicsStepTime();
	ObjectIdSet::iterator x = owned_object_list_.begin();
	for (; x != owned_object_list_.end(); ++x) {
		cure::ContextObject* _object = GetContext()->GetObject(*x);
		if (_object) {
			_object->GetNetworkOutputGhost()->GhostStep(step_count, physics_frame_time);
		}
	}
}



void GameClientSlaveManager::ToggleConsole() {
	allow_movement_input_ = !((ClientConsoleManager*)GetConsoleManager())->ToggleVisible();
}



void GameClientSlaveManager::RequestLogin(const str& server_address, const cure::LoginId& login_token) {
	GetMaster()->PreLogin(server_address);

	ScopeLock lock(GetTickLock());

	is_reset_ = false;

	str port_range = v_slowget(GetVariableScope(), kRtvarNetworkConnectLocalportrange, "1025-65535");
	str local_name;
	if (strutil::StartsWith(server_address, "localhost:") || strutil::StartsWith(server_address, "127.0.0.1:")) {
		local_name = "localhost";
	}
	str local_address = local_name + ':' + port_range;
	if (!GetNetworkClient()->Open(local_address)) {
		disconnect_reason_ = "Could not use local sockets.";
		return;
	}

	float connect_timeout;
	v_get(connect_timeout, =(float), GetVariableScope(), kRtvarNetworkConnectTimeout, 3.0);
	str _server_address = server_address;

	// Open firewall path from server.
	if (master_server_connection_) {
		master_server_connection_->SetSocketInfo(GetNetworkClient(), connect_timeout);
		bool is_local_address = false;
		{
			SocketAddress resolved_address;
			if (resolved_address.Resolve(_server_address)) {
				str ip = resolved_address.GetIP().GetAsString();
				if (ip == "127.0.0.1" || ip == "::1" || ip == "0:0:0:0:0:0:0:1") {
					is_local_address = true;
				}
			}
		}
		if (!GetMaster()->IsLocalServer() && !is_local_address) {
			master_server_connection_->RequestOpenFirewall(server_address);
			const double timeout = 1.2;
			const double wait_time = master_server_connection_->WaitUntilDone(timeout, true);
			if (wait_time < timeout) {
				if (master_server_connection_->GetFirewallOpenStatus() == MasterServerConnection::kFirewallOpened) {
					log_.Info("Master seems to have asked game server to open firewall OK.");
					Thread::Sleep(wait_time+0.02);	// Let's pray. Since we got through in a certain time, perhaps the server will too.
				} else if (master_server_connection_->GetFirewallOpenStatus() == MasterServerConnection::kFirewallUseLan) {
					log_.Info("Master seems to think the game server is on our LAN. Using that instead.");
					_server_address = master_server_connection_->GetLanServerConnectAddress();
				}
			} else {
				log_.Warning("Master did not reply in time to if it asked game server to open firewall. Trying anyway.");
			}
		}
	}

	connect_user_name_ = login_token.GetName();
	connect_server_address_ = server_address;
	disconnect_reason_ = "Connect failed.";
	GetNetworkClient()->StartConnectLogin(_server_address, connect_timeout, login_token);
}

void GameClientSlaveManager::OnLoginSuccess() {
}

void GameClientSlaveManager::Logout() {
	if (GetNetworkClient()->IsActive()) {
		disconnect_reason_ = "User requested logout.";
		log_.Warning(disconnect_reason_);
		is_reset_complete_ = false;
		is_reset_ = false;
		GetNetworkClient()->Disconnect(true);
		for (int x = 0; !is_reset_complete_ && x < 10; ++x) {
			Thread::Sleep(0.05);
		}
	}
}

bool GameClientSlaveManager::IsLoggingIn() const {
	return (GetNetworkClient()->IsConnecting() || GetNetworkClient()->IsLoggingIn());
}

bool GameClientSlaveManager::IsUiMoveForbidden(cure::GameObjectId object_id) const {
	if (GetContext()->IsLocalGameObjectId(object_id)) {	// Optimization, this is the fastest method.
		return false;
	}
	const bool move_allowed = (IsOwned(object_id) || (!GetMaster()->IsLocalObject(object_id) && GetMaster()->IsFirstSlave(this)));
	return !move_allowed;
}

void GameClientSlaveManager::AddLocalObjects(std::unordered_set<cure::GameObjectId>& local_object_set) {
	local_object_set.insert(owned_object_list_.begin(), owned_object_list_.end());
}

bool GameClientSlaveManager::IsOwned(cure::GameObjectId object_id) const {
	return (owned_object_list_.find(object_id) != owned_object_list_.end());
}



bool GameClientSlaveManager::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	options_.RefreshConfiguration();

	options_.options::OptionsManager::UpdateInput(key_code, true);
	if (options_.IsToggleConsole()) {
		options_.ResetToggles();
		ToggleConsole();
		return (true);	// This key ends here.
	}
	return (false);
}

bool GameClientSlaveManager::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	options_.options::OptionsManager::UpdateInput(key_code, false);
	((ConsoleManager*)GetConsoleManager())->OnKey(uilepra::InputManager::GetKeyName(key_code));
	return (false);
}

void GameClientSlaveManager::OnInput(uilepra::InputElement* element) {
	bool output_input;
	v_get(output_input, =, GetVariableScope(), kRtvarDebugInputPrint, false);
	if (output_input) {
		log_.Infof("Input %s: %f.", element->GetFullName().c_str(), element->GetValue());
	}

	options_.RefreshConfiguration();

	if (options_.UpdateInput(element)) {
		if (element->GetInterpretation() == uilepra::InputElement::kRelativeAxis) {
			relative_axis_.insert(element);
			unused_relative_axis_.erase(element);
		}
	}
	if (options_.IsToggleConsole()) {
		options_.ResetToggles();
		ToggleConsole();
	}
}

void GameClientSlaveManager::HandleUnusedRelativeAxis() {
	float mouse_filter;
	v_get(mouse_filter, =(float), GetVariableScope(), kRtvarCtrlMousefilter, -1.0f);
	if (mouse_filter < 0) {
		return;
	}

	InputElementSet unused_set = unused_relative_axis_;
	InputElementSet::iterator x = unused_set.begin();
	for (; x != unused_set.end(); ++x) {
		uilepra::InputElement* axis = (*x);

		const str suffixes[] = { "", "+", "-" };
		for (int y = 0; y < LEPRA_ARRAY_COUNT(suffixes); ++y) {
			str axis_name = axis->GetFullName() + suffixes[y];
			bool is_steering = false;
			options::OptionsManager::ValueArray* value_pointers = options_.GetValuePointers(axis_name, is_steering);
			if (!value_pointers) {
				continue;
			}
			options::OptionsManager::ValueArray::iterator x = value_pointers->begin();
			for (; x != value_pointers->end(); ++x) {
				if (std::abs(**x) > 0.02f) {
					**x *= mouse_filter;
				} else if (**x != 0) {
					**x = 0;
					relative_axis_.erase(axis);
				}
			}
		}
	}

	// Done. For next loop we assume all relative axis' haven't been triggered.
	unused_relative_axis_ = relative_axis_;
}



int GameClientSlaveManager::GetSlaveIndex() const {
	return (slave_index_);
}



PixelRect GameClientSlaveManager::GetRenderArea() const {
	return (render_area_);
}

float GameClientSlaveManager::UpdateFrustum(float fov) {
	return GetMaster()->UpdateFrustum(fov);
}



str GameClientSlaveManager::GetApplicationCommandFilename() const {
	return (Application::GetIoFile(
		"ClientApplication"+strutil::IntToString(slave_index_, 10),
		"lsh"));
}



bool GameClientSlaveManager::Reset() {	// Run when disconnected.
	ScopeLock lock(GetTickLock());

	is_reset_ = true;

	GetNetworkClient()->Disconnect(true);
	ping_attempt_count_ = 0;

	object_frame_index_map_.clear();

	GetContext()->ClearObjects();
	bool ok = InitializeUniverse();

	is_reset_complete_ = true;

	return (ok);
}



void GameClientSlaveManager::ProcessNetworkInputMessage(cure::Message* message) {
	cure::MessageType _type = message->GetType();
	switch (_type) {
		case cure::kMessageTypeStatus: {
			cure::MessageStatus* message_status = (cure::MessageStatus*)message;
			cure::RemoteStatus remote_status = message_status->GetRemoteStatus();
			//log_.Infof("Got remote status %i with ID %u.", remote_status, message_status->GetInteger());
			if (remote_status != cure::kRemoteOk) {
				str error_message;
				message_status->GetMessageString(error_message);
				disconnect_reason_ = error_message;
				log_.Warning(disconnect_reason_);
				GetNetworkClient()->Disconnect(false);
			} else if (GetNetworkClient()->IsLoggingIn()) {
				GetNetworkClient()->SetLoginAccountId(message_status->GetInteger());
				disconnect_reason_.clear();
				// A successful login: lets store these parameters for next time!
				v_override(GetVariableScope(), kRtvarLoginUsername, connect_user_name_);
				v_set(GetVariableScope(), kRtvarNetworkServeraddress, connect_server_address_);
				master_server_connection_->GraceClose(0.1, false);

				OnLoginSuccess();
			} else {
				ProcessNetworkStatusMessage(message_status);
			}
		} break;
		case cure::kMessageTypeNumber: {
			cure::MessageNumber* message_number = (cure::MessageNumber*)message;
			ProcessNumber(message_number->GetInfo(), message_number->GetInteger(), message_number->GetFloat());
		} break;
		case cure::kMessageTypeCreateObject:
		case cure::kMessageTypeCreateOwnedObject: {
			cure::MessageCreateObject* message_create_object = (cure::MessageCreateObject*)message;
			str _class_id;
			lepra::xform transformation;
			message_create_object->GetTransformation(transformation);
			//const float a = 1.0f/::sqrt(2.0f);
			//transformation.SetOrientation(quat(0, 0, -a, -a));
			message_create_object->GetClassId(_class_id);
			/*log_.Infof("Creating network instance %u of type %s at pos (%f; %f; %f, q (%f, %f, %f, %f)."),
				message_create_object->GetObjectId(), _class_id.c_str(),
				transformation.GetPosition().x, transformation.GetPosition().y, transformation.GetPosition().z,
				transformation.GetOrientation().a, transformation.GetOrientation().b, transformation.GetOrientation().c, transformation.GetOrientation().d);*/
			cure::ContextObject* _object = CreateObject(message_create_object->GetObjectId(),
				_class_id, cure::kNetworkObjectRemoteControlled, &transformation);
			if (_type == cure::kMessageTypeCreateOwnedObject) {
				cure::MessageCreateOwnedObject* message_create_owned_object = (cure::MessageCreateOwnedObject*)message_create_object;
				_object->SetOwnerInstanceId(message_create_owned_object->GetOwnerInstanceId());
				//deb_assert(GetContext()->GetObject(_object->GetOwnerInstanceId(), true));	Owning object may have been destroyed.
			}

		} break;
		case cure::kMessageTypeDeleteObject: {
			cure::MessageDeleteObject* message_delete_object = (cure::MessageDeleteObject*)message;
			cure::GameObjectId id = message_delete_object->GetObjectId();
			GetContext()->DeleteObject(id);
		} break;
		case cure::kMessageTypeObjectPosition: {
			cure::MessageObjectPosition* message_movement = (cure::MessageObjectPosition*)message;
			cure::GameObjectId _instance_id = message_movement->GetObjectId();
			int32 _frame_index = message_movement->GetFrameIndex();
			cure::ObjectPositionalData& _data = message_movement->GetPositionalData();
			cure::ContextObject* _object = GetContext()->GetObject(_instance_id, true);
			if (!_object) {
				log_volatile(log_.Debugf("Object %u is missing, can't set pos from network data. Asking server for re-creation.", _instance_id));
				GetNetworkAgent()->SendNumberMessage(true, GetNetworkClient()->GetSocket(),
					cure::MessageNumber::kInfoRecreateObject, _instance_id, 0);
			} else {
				if (!_object->IsLoaded()) {
					// Aha! Set positional information so it's there when the object gets loaded.
					_object->SetInitialPositionalData(_data);
				}

				if (!GetMaster()->IsLocalServer()) {	// Server will have set the position already.
					if (GetMaster()->IsLocalObject(_instance_id)) {
						// Only set changes to locally controlled objects if the change is big.
						SetMovement(_instance_id, _frame_index, _data, 0.5f);
					} else {
						SetMovement(_instance_id, _frame_index, _data, 0);
					}
				}
			}
			v_internal_arithmetic(GetVariableScope(), kRtvarDebugNetRecvposcnt, int, +, 1, 0, 1000000);
		} break;
		case cure::kMessageTypeObjectAttach: {
			cure::MessageObjectAttach* message_attach = (cure::MessageObjectAttach*)message;
			cure::GameObjectId _object1_id = message_attach->GetObjectId();
			cure::GameObjectId _object2_id = message_attach->GetObject2Id();
			unsigned _body1_index = message_attach->GetBody1Index();
			unsigned _body2_index = message_attach->GetBody2Index();
			AttachObjects(_object1_id, _body1_index, _object2_id, _body2_index);
		} break;
		case cure::kMessageTypeObjectDetach: {
			cure::MessageObjectDetach* message_detach = (cure::MessageObjectDetach*)message;
			cure::GameObjectId _object1_id = message_detach->GetObjectId();
			cure::GameObjectId _object2_id = message_detach->GetObject2Id();
			DetachObjects(_object1_id, _object2_id);
		} break;
		case cure::kMessageTypeObjectAttribute: {
			cure::MessageObjectAttribute* message_attrib = (cure::MessageObjectAttribute*)message;
			cure::GameObjectId _object_id = message_attrib->GetObjectId();
			unsigned byte_size = 0;
			const uint8* buffer = message_attrib->GetReadBuffer(byte_size);
			GetContext()->UnpackObjectAttribute(_object_id, buffer, byte_size);
		} break;
		default: {
			log_.Error("Got bad message type from server.");
		} break;
	}
}

void GameClientSlaveManager::ProcessNetworkStatusMessage(cure::MessageStatus* message) {
	switch (message->GetInfo()) {
		case cure::MessageStatus::kInfoChat: {
			str chat_message;
			message->GetMessageString(chat_message);
			if (message->GetInteger() == 0) {
				chat_message = "ServerAdmin: "+chat_message;
			} else {
				chat_message = "<Player?>: "+chat_message;
			}
			log_.Headline(chat_message);
		} break;
		case cure::MessageStatus::kInfoLogin: {
			deb_assert(false);
		} break;
		case cure::MessageStatus::kInfoCommand: {
			if (message->GetRemoteStatus() == cure::kRemoteOk) {
				str command;
				message->GetMessageString(command);
				ClientConsoleManager* console = ((ClientConsoleManager*)GetConsoleManager());
				const int previous_security_level = console->GetSecurityLevel();
				console->SetSecurityLevel(1);
				int result = console->FilterExecuteCommand(command);
				console->SetSecurityLevel(previous_security_level);
				if (result != 0) {
					disconnect_reason_ = "Server not safe! Please join some other game.";
					GetNetworkClient()->Disconnect(false);
				}
			}
		} break;
	}
}

void GameClientSlaveManager::ProcessNumber(cure::MessageNumber::InfoType type, int32 integer, float32 _f) {
	switch (type) {
		case cure::MessageNumber::kInfoSetTime: {
			log_volatile(log_.Tracef("Setting physics frame to %i.", integer));
			if (!GetMaster()->IsLocalServer()) {
				GetMaster()->GetTimeManager()->SetCurrentPhysicsFrame(integer);
			}
		} break;
		case cure::MessageNumber::kInfoAdjustTime: {
			log_trace("Adjusting time.");
			if (!GetMaster()->IsLocalServer()) {
				GetMaster()->GetTimeManager()->SetPhysicsSpeedAdjustment(_f, integer);
			}
		} break;
		case cure::MessageNumber::kInfoPong: {
			if (GetNetworkClient()->GetSocket()) {
				ping_attempt_count_ = 0;
				log_volatile(const float ping_time = GetTimeManager()->ConvertPhysicsFramesToSeconds(GetTimeManager()->GetCurrentPhysicsFrameDelta(integer)));
				log_volatile(const float server_strive_time = GetTimeManager()->ConvertPhysicsFramesToSeconds((int)_f)*2);
				log_volatile(log_.Debugf("Pong: this=%ss, server sim strives to be x2=%ss ahead, (self=%s).",
					Number::ConvertToPostfixNumber(ping_time, 2).c_str(),
					Number::ConvertToPostfixNumber(server_strive_time, 2).c_str(),
					GetNetworkClient()->GetSocket()->GetLocalAddress().GetAsString().c_str()));
			}
		} break;
		case cure::MessageNumber::kInfoGrantLoan: {
			const cure::GameObjectId _instance_id = integer;
			owned_object_list_.insert(_instance_id);
			cure::ContextObject* _object = GetContext()->GetObject(_instance_id);
			if (_object) {
				const int ownership_frames = GetTimeManager()->GetPhysicsFrameDelta((int)_f, GetTimeManager()->GetCurrentPhysicsFrame());
				const float ownership_seconds = GetTimeManager()->ConvertPhysicsFramesToSeconds(ownership_frames-2);
				_object->SetNetworkObjectType(cure::kNetworkObjectLocallyControlled);
				GetContext()->AddAlarmExternalCallback(_object, cure::ContextManager::AlarmExternalCallback(this, &GameClientSlaveManager::OnIdOwnershipExpired), ID_OWNERSHIP_LOAN_EXPIRES, ownership_seconds, 0);
				log_volatile(log_.Debugf("Got control over object with ID %i for %f seconds.", integer, ownership_seconds));
			}
		} break;
		default: {
			deb_assert(false);
		} break;
	}
}

cure::ContextObject* GameClientSlaveManager::CreateObject(cure::GameObjectId instance_id, const str& class_id,
	cure::NetworkObjectType network_type, xform* transform) {
	cure::ContextObject* _object = GetContext()->GetObject(instance_id, true);
	if (_object && _object->GetClassId() != class_id) {
		// Ouch, this object has been killed and we weren't informed.
		log_.Warningf("Ouch, slave %i creating context object %s, but already had old instance of type %s (deleting that first).",
			slave_index_, class_id.c_str(), _object->GetClassId().c_str());
		GetContext()->DeleteObject(instance_id);
		_object = 0;
	}
	if (!_object) {
		log_volatile(log_.Debugf("Slave %i creating context object %s.", slave_index_, class_id.c_str()));
		_object = Parent::CreateContextObject(class_id, network_type, instance_id);
		if (transform) {
			_object->SetInitialTransform(*transform);
		}
		if (!_object->IsLoaded()) {
			_object->StartLoading();
		}
	}
	return _object;
}

void GameClientSlaveManager::SetMovement(cure::GameObjectId instance_id, int32 frame_index, cure::ObjectPositionalData& data, float delta_threshold) {
	ObjectFrameIndexMap::iterator x = object_frame_index_map_.find(instance_id);
	if (x == object_frame_index_map_.end()) {
		object_frame_index_map_.insert(ObjectFrameIndexMap::value_type(instance_id, frame_index-1));
		x = object_frame_index_map_.find(instance_id);
	}
	const int last_set_frame_index = x->second;	// Last set frame index.
	const int delta_frames = GetTimeManager()->GetPhysicsFrameDelta(frame_index, last_set_frame_index);
	if (delta_frames >= 0 || delta_frames < -1000) {	// Either it's newer, or it's long, long ago (meaning time wrap).
		x->second = frame_index;

		//str s = strutil::Format("client %i at frame %i", client_index, frame_index);
		//log_debug("Client set pos of other client", s);
		UiCure::CppContextObject* _object = (UiCure::CppContextObject*)GetContext()->GetObject(instance_id, true);
		if (_object) {
			// Client has moved forward in time since the server sent us this positional info
			// some frames ago. Extrapolate forward the number of micro-frames that diff.
			float extrapolation_factor;
			v_get(extrapolation_factor, =(float), GetVariableScope(), kRtvarNetphysExtrapolationfactor, 0.0);
			if (extrapolation_factor) {
				int micro_steps;
				v_get(micro_steps, =, GetVariableScope(), kRtvarPhysicsMicrosteps, 3);
				const int future_step_count = GetTimeManager()->GetCurrentPhysicsFrameDelta(frame_index) * micro_steps;
				const float step_increment = GetTimeManager()->GetAffordedPhysicsStepTime() / micro_steps;
				data.GhostStep(future_step_count, step_increment*extrapolation_factor);
			}
			/*bool lSetPosition = true;
			if (instance_id == avatar_id_) {
				const cure::ObjectPositionalData* lCurrentPos;
				if (_object->UpdateFullPosition(lCurrentPos)) {
					float resync_on_diff;
					v_get(resync_on_diff, =(float), GetVariableScope(), kRtvarNetphysResyncondiffgt, 0.2);
					if (data.GetScaledDifference(lCurrentPos) < resync_on_diff) {
						lSetPosition = false;	// Not enough change to take notice. Would just yield a jerky movement, not much more.
					}
				}
			}*/
			_object->SetFullPosition(data, delta_threshold);
			bool enable_smoothing;
			v_get(enable_smoothing, =, GetVariableScope(), kRtvarNetphysEnablesmoothing, true);
			if (enable_smoothing) {
				_object->ActivateLerp();
			}
		} else {
			log_.Warningf("Slave %i could not set position for object %i.", slave_index_, instance_id);
		}
	} else {
		log_volatile(log_.Debugf("Throwing away out-of-order positional data (%i frames late).", last_set_frame_index-frame_index));
	}
}

bool GameClientSlaveManager::OnPhysicsSend(cure::ContextObject*) {
	return (true);	// Say true to drop us from sender list.
}

bool GameClientSlaveManager::OnAttributeSend(cure::ContextObject* object) {
	log_debug("Sending attribute(s) for a context object...");
	typedef cure::ContextObject::AttributeArray AttributeArray;
	const AttributeArray& attributes = object->GetAttributes();
	AttributeArray::const_iterator x = attributes.begin();
	for (; x != attributes.end(); ++x) {
		cure::ContextObjectAttribute* attribute = *x;
		const int send_size = attribute->QuerySend();
		const bool is_allowed_send = (attribute->GetNetworkType() == cure::ContextObjectAttribute::kTypeBoth ||
				attribute->GetNetworkType() == cure::ContextObjectAttribute::TYPE_BOTH_BROADCAST);
		if (send_size > 0 && is_allowed_send) {
			cure::Packet* packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
			cure::MessageObjectAttribute* attrib_message = (cure::MessageObjectAttribute*)GetNetworkAgent()->
				GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeObjectAttribute);
			packet->AddMessage(attrib_message);
			attribute->Pack(attrib_message->GetWriteBuffer(packet, object->GetInstanceId(), send_size));

			deb_assert(!GetNetworkAgent()->GetLock()->IsOwner());
			ScopeLock tick_lock(GetTickLock());
			GetNetworkAgent()->PlaceInSendBuffer(true, GetNetworkClient()->GetSocket(), packet);

			GetNetworkAgent()->GetPacketFactory()->Release(packet);
		}
	}
	return true;	// Say true to drop us from sender list.
}

bool GameClientSlaveManager::IsServer() {
	return (false);
}

void GameClientSlaveManager::SendAttach(cure::ContextObject*, unsigned, cure::ContextObject*, unsigned) {
	// Server manages this.
	deb_assert(false);
}

void GameClientSlaveManager::SendDetach(cure::ContextObject*, cure::ContextObject*) {
	// Server manages this.
}

void GameClientSlaveManager::OnIdOwnershipExpired(int, cure::ContextObject* object, void*) {
	deb_assert(IsOwned(object->GetInstanceId()));
	owned_object_list_.erase(object->GetInstanceId());
	object->SetNetworkObjectType(cure::kNetworkObjectRemoteControlled);
	object->DeleteNetworkOutputGhost();
}

void GameClientSlaveManager::AttachObjects(cure::GameObjectId object1_id, unsigned body1_index,
	cure::GameObjectId object2_id, unsigned body2_index) {
	cure::ContextObject* object1 = GetContext()->GetObject(object1_id);
	cure::ContextObject* object2 = GetContext()->GetObject(object2_id);
	if (object1 && object2) {
		object1->AttachToObjectByBodyIndices(body1_index, object2, body2_index);
	} else {
		deb_assert(false);
	}
}

void GameClientSlaveManager::DetachObjects(cure::GameObjectId object1_id, cure::GameObjectId object2_id) {
	cure::ContextObject* object1 = GetContext()->GetObject(object1_id);
	cure::ContextObject* object2 = GetContext()->GetObject(object2_id);
	if (object1 && object2) {
		if (!object1->DetachFromObject(object2)) {
			deb_assert(false);
		}
	} else {
		deb_assert(false);
	}
}



cure::NetworkClient* GameClientSlaveManager::GetNetworkClient() const {
	return ((cure::NetworkClient*)GetNetworkAgent());
}



void GameClientSlaveManager::DrawAsyncDebugInfo() {
	ui_manager_->GetPainter()->ResetClippingRect();
	ui_manager_->GetPainter()->SetClippingRect(render_area_);

	// Draw send and receive staples.
	int send_count;
	v_tryget(send_count, =, GetVariableScope(), kRtvarDebugNetSendposcnt, 0);
	if (send_count > 0) {
		v_internal(GetVariableScope(), kRtvarDebugNetSendposcnt, 0);
	}
	DrawDebugStaple(0, send_count*10, Color(255, 0, 0));
	int recv_count;
	v_tryget(recv_count, =, GetVariableScope(), kRtvarDebugNetRecvposcnt, 0);
	if (recv_count > 0) {
		v_internal(GetVariableScope(), kRtvarDebugNetRecvposcnt, 0);
	}
	DrawDebugStaple(1, recv_count*10, Color(0, 255, 0));
}

void GameClientSlaveManager::DrawDebugStaple(int index, int height, const Color& color) {
	if (height > 0) {
		ui_manager_->GetPainter()->SetColor(color, 0);
		const int unit = 10;
		const int x = render_area_.left_ + unit + index*unit*2;
		const int y = render_area_.bottom_ - unit - height;
		ui_manager_->GetPainter()->FillRect(x, y, x+unit, y+height);
	}
}

void GameClientSlaveManager::DrawSyncDebugInfo() {
	UpdateCameraPosition(false);
	float _fov;
	v_get(_fov, =(float), GetVariableScope(), kRtvarUi3DFov, 45.0);
	UpdateFrustum(_fov);

	bool draw_local_server;
	v_get(draw_local_server, =, GetVariableScope(), kRtvarDebug3DDrawlocalserver, true);
	const cure::ContextManager* server_context = (draw_local_server && GetMaster()->IsLocalServer())? GetMaster()->GetLocalServer()->GetContext() : 0;
	UiCure::DebugRenderer debug_renderer(GetVariableScope(), ui_manager_, GetContext(), server_context, GetTickLock());
	debug_renderer.Render(ui_manager_, render_area_);
}



loginstance(kGame, GameClientSlaveManager);



}
