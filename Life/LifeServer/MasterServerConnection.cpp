
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "masterserverconnection.h"
#include "../../lepra/include/socket.h"
#include "../../lepra/include/systemmanager.h"
#include "../lifemaster/masterserver.h"
#include "../lifemaster/masterservernetworkparser.h"



namespace life {



MasterServerConnection::MasterServerConnection(const str& master_address):
	master_server_address_(master_address),
	state_(kDisconnected),
	socket_io_handler_(0),
	mux_socket_(0),
	v_socket_(0),
	connecter_(new MemberThread<MasterServerConnection>("MasterServerConnecter")),
	disconnect_counter_(0),
	is_connect_error_(false),
	last_firewall_open_status_(kFirewallError) {
}

MasterServerConnection::~MasterServerConnection() {
	GraceClose(2.0, true);
	delete connecter_;
	connecter_ = 0;
}

const str& MasterServerConnection::GetMasterAddress() const {
	return master_server_address_;
}


void MasterServerConnection::SetSocketInfo(cure::SocketIoHandler* socket_io_handler, double connect_timeout) {
	connect_timeout_ = connect_timeout;
	deb_assert(!socket_io_handler_ || socket_io_handler_ == socket_io_handler);	// If this is not valid, you must at least remove *this* as a FilterIo listener from the socket before replacing it.
	socket_io_handler_ = socket_io_handler;
	if (!socket_io_handler) {
		mux_socket_ = 0;
		OnDropSocket(v_socket_);
	} else {
		cure::SocketIoHandler::MuxIoSocket* mux_socket = socket_io_handler->GetMuxIoSocket();
		if (!mux_socket_ || !mux_socket || mux_socket_->GetSysSocket() != mux_socket->GetSysSocket() ||
			!mux_socket_->IsOpen()) {
			OnDropSocket(v_socket_);
		}
		mux_socket_ = mux_socket;
	}
}



void MasterServerConnection::SendLocalInfo(const str& local_server_info) {
	deb_assert(!local_server_info.empty());
	if (local_server_info_ != local_server_info || upload_timeout_.QueryTimeDiff() >= server_info_timeout_) {
		local_server_info_ = local_server_info;
		upload_timeout_.ClearTimeDiff();
		QueryAddState(kUploadInfo);
	}
}

void MasterServerConnection::AppendLocalInfo(const str& extra_server_info) {
	if (!uploaded_server_info_.empty()) {
		local_server_info_ += extra_server_info;
		QueryAddState(kUploadInfo);
		TriggerConnectTimer();
	}
}

void MasterServerConnection::RequestServerList(const str& server_criterias) {
	server_sort_criterias_ = server_criterias;
	QueryAddState(kDownloadList);
	TriggerConnectTimer();
}

void MasterServerConnection::RequestOpenFirewall(const str& game_server_connect_address) {
	last_firewall_open_status_ = kFirewallError;
	lan_game_server_address_.clear();
	game_server_connect_address_ = game_server_connect_address;
	QueryAddState(kOpenFirewall);
	TriggerConnectTimer();
	Tick();	// We want this to happen real fast.
}

bool MasterServerConnection::UpdateServerList(ServerInfoList& server_list) const {
	strutil::strvec servers = strutil::Split(server_list_, "\n");
	if (servers.size() >= 1) {
		if (servers.back() != "OK") {
			bool is_updated = !server_list.empty();
			server_list.clear();
			return is_updated;
		}
	}
	bool is_updated = false;
	ServerInfoList new_server_list;
	strutil::strvec::const_iterator x = servers.begin();
	ServerInfoList::const_iterator y = server_list.begin();
	for (; x != servers.end(); ++x) {
		ServerInfo info;
		MasterServerNetworkParser::ExtractServerInfo(*x, info, 0);
		new_server_list.push_back(info);
		if (!is_updated && (y != server_list.end() && info != *y)) {
			is_updated = true;
		}
		if (y != server_list.end()) {
			++y;
		} else {
			is_updated = true;
		}
	}
	is_updated |= (new_server_list.size() != server_list.size());
	if (is_updated) {
		server_list = new_server_list;
	}
	return is_updated;
}

str MasterServerConnection::GetServerListAsText() const {
	return server_list_;
}

bool MasterServerConnection::IsConnectError() const {
	return is_connect_error_;
}

MasterServerConnection::FirewallStatus MasterServerConnection::GetFirewallOpenStatus() const {
	return last_firewall_open_status_;
}

const str& MasterServerConnection::GetLanServerConnectAddress() const {
	return lan_game_server_address_;
}

double MasterServerConnection::WaitUntilDone(double timeout, bool allow_reconnect) {
	const int start_disonnect_counter = disconnect_counter_;
	HiResTimer timer;
	while (timer.QueryTimeDiff() < timeout && (allow_reconnect || v_socket_) &&
		start_disonnect_counter == disconnect_counter_ && !state_list_.empty()) {
		Tick();
		Thread::Sleep(0.001);
	}
	if (timer.QueryTimeDiff() < timeout && v_socket_ && start_disonnect_counter == disconnect_counter_) {
		Tick();
	}
	return timer.QueryTimeDiff();
}

void MasterServerConnection::GraceClose(double timeout, bool wait_until_done) {
	const double waited_time = wait_until_done? WaitUntilDone(timeout, false) : 0;
	connecter_->Join(timeout - waited_time);
	Close(false);
}

bool MasterServerConnection::CloseUnlessUploaded() {
	if (uploaded_server_info_.empty()) {
		GraceClose(1.0, false);
	}
	return uploaded_server_info_.empty();	// TRICKY: some time has gone by, might have been updated.
}

void MasterServerConnection::Tick() {
	switch (state_) {
		case kUploadInfo: {
			if (!UploadServerInfo()) {
				Close(true);
			}
		} break;
		case kDownloadList: {
			if (!DownloadServerList()) {
				Close(true);
			}
		} break;
		case kOpenFirewall: {
			last_firewall_open_status_ = OpenFirewall();
		} break;
	}
	StepState();
}

bool MasterServerConnection::TickReceive(ServerInfo& server_info) {
	if (state_ != kConnected || !v_socket_ || !QueryMuxValid()) {
		return false;
	}
	if (!v_socket_->NeedInputPeek()) {
		return false;
	}
	str command_line;
	if (!Receive(command_line)) {
		log_.Error("Someone snatched data received from master server!");
		deb_assert(false);
		return false;
	}
	if (!MasterServerNetworkParser::ExtractServerInfo(command_line, server_info, &v_socket_->GetTargetAddress())) {
		log_.Error("Got bad formatted command from master server!");
		deb_assert(false);
		return false;
	}
	return true;
}



void MasterServerConnection::QueryAddState(State state) {
	if (state_ == state) {
		return;
	}
	std::list<State>::iterator x = state_list_.begin();
	for (; x != state_list_.end(); ++x) {
		if (*x == state) {
			return;
		}
	}
	idle_timer_.PopTimeDiff();
	state_list_.push_back(state);
}

void MasterServerConnection::TriggerConnectTimer() {
	idle_timer_.PopTimeDiff();
	idle_timer_.ReduceTimeDiff(-1.1*disconnected_idle_timeout_);
}

void MasterServerConnection::StepState() {
	switch (state_) {
		case kConnect: {
			if (!connecter_->IsRunning() && QueryMuxValid()) {
				state_ = kConnecting;
				if (!connecter_->Start(this, &MasterServerConnection::ConnectEntry)) {
					log_.Warning("Could not start connecter.");
					is_connect_error_ = true;
				}
			} else {
				log_.Warning("Trying to connect while connecter still running!");
				deb_assert(!v_socket_);
				deb_assert(false);
			}
		} break;
		case kConnected: {
			if (!state_list_.empty()) {
				state_ = state_list_.front();
				state_list_.pop_front();
				idle_timer_.PopTimeDiff();
			} else if (idle_timer_.QueryTimeDiff() >= connected_idle_timeout_) {
				// This may happen if the player idles too long at the server list GUI.
				Close(false);
			}
		} break;
		case kUploadInfo:
		case kDownloadList:
		case kOpenFirewall: {
			state_ = kConnected;
		} break;
		case kDisconnected: {
			if (idle_timer_.QueryTimeDiff() >= disconnected_idle_timeout_ && !state_list_.empty()) {
				// Anything we want done, we must be connected.
				state_ = kConnect;
			}
		} break;
	}
}

void MasterServerConnection::ConnectEntry() {
	if (state_ != kConnecting || v_socket_ != 0) {
		log_.Warning("Starting connector thread while already working/connected!");
		deb_assert(false);
		return;
	}

	SocketAddress target_address;
	if (!target_address.Resolve(master_server_address_)) {
		log_.Warningf("Could not resolve master server address '%s'.", master_server_address_.c_str());
		Close(true);
		return;
	}
	if (connecter_->GetStopRequest() || !QueryMuxValid()) {
		log_.Warningf("Connect to master server '%s' was aborted.", master_server_address_.c_str());
		Close(true);
		return;
	}
	const std::string connection_id = SystemManager::GetRandomId();
	cure::SocketIoHandler::DropFilterCallback drop_callback(this, &MasterServerConnection::OnDropSocket);
	socket_io_handler_->AddFilterIoSocket(0, drop_callback);
	v_socket_ = mux_socket_->Connect(target_address, connection_id, connect_timeout_);
	if (!v_socket_) {
		log_.Warningf("Could not connect to master server address '%s'.", master_server_address_.c_str());
		Close(true);
		return;
	}
	socket_io_handler_->AddFilterIoSocket(v_socket_, drop_callback);
	deb_assert(state_ == kConnecting);
	state_ = kConnected;
	idle_timer_.PopTimeDiff();
	log_.Headlinef("Connected to master server @ %s.", master_server_address_.c_str());
}

bool MasterServerConnection::UploadServerInfo() {
	if (!SendAndAck(kMasterServerUsi " " + local_server_info_)) {
		return false;
	}
	log_.Info("Uploaded server info...");
	uploaded_server_info_ = local_server_info_;
	is_connect_error_ = false;
	return true;
}

bool MasterServerConnection::DownloadServerList() {
	if (!SendAndRecv(kMasterServerDsl " " + server_sort_criterias_, server_list_)) {
		return false;
	}
	is_connect_error_ = false;
	return true;
}

MasterServerConnection::FirewallStatus MasterServerConnection::OpenFirewall() {
	strutil::strvec vector = strutil::Split(game_server_connect_address_, ":");
	if (vector.size() != 2) {
		return kFirewallError;
	}
	str _reply;
	if (!SendAndRecv(kMasterServerOf " --address " + vector[0] + " --port " + vector[1], _reply)) {
		return kFirewallError;
	}
	ServerInfo info;
	if (_reply == "OK") {
		is_connect_error_ = false;
		return kFirewallOpened;
	} else if (MasterServerNetworkParser::ExtractServerInfo(_reply, info, 0)) {
		if (info.command_ == kMasterServerUl) {
			lan_game_server_address_ = info.internal_ip_address_ + strutil::Format(":%i", info.internal_port_);
			return kFirewallUseLan;
		}
	}
	return kFirewallError;
}

bool MasterServerConnection::SendAndAck(const str& data) {
	str _reply;
	if (!SendAndRecv(data, _reply)) {
		return false;
	}
	return _reply == "OK";
}

bool MasterServerConnection::SendAndRecv(const str& data, str& reply) {
	if (!Send(data)) {
		return false;
	}
	return Receive(reply);
}

bool MasterServerConnection::Send(const str& data) {
	if (!v_socket_ || !QueryMuxValid()) {
		log_.Warning("Trying to send to master server even though unconnected.");
		Close(false);
		return false;
	}
	if (data.size() > 300) {
		log_.Warning("Trying to send too big chunk to master server.");
		return false;
	}
	uint8 raw_data[1024];
	unsigned send_byte_count = MasterServerNetworkParser::StrToRaw(raw_data, data);
	if ((unsigned)v_socket_->DirectSend(raw_data, send_byte_count) != send_byte_count) {
		log_.Warning("Transmission to master server failed.");
		return false;
	}
	return true;
}

bool MasterServerConnection::Receive(str& data) {
	if (!v_socket_ || !QueryMuxValid()) {
		log_.Warning("Trying to receive master server data even though unconnected.");
		return false;
	}
	v_socket_->WaitAvailable(4.0);
	uint8 raw_data[1024];
	int raw_size = v_socket_->Receive(raw_data, sizeof(raw_data));
	if (raw_size <= 0) {
		log_.Warning("Tried to receive data from master server, but it didn't send us any!");
		return false;
	}
	str _data;
	if (!MasterServerNetworkParser::RawToStr(_data, raw_data, raw_size)) {
		log_.Error("Got garbled data from master server - something is seriously wrong!");
		return false;
	}
	data = _data;
	return true;
}

void MasterServerConnection::Close(bool error) {
	if (error) {
		is_connect_error_ = true;
	}
	++disconnect_counter_;
	if (mux_socket_ && v_socket_) {
		Send(kMasterServerDc);
		if (socket_io_handler_) {
			socket_io_handler_->KillIoSocket(v_socket_);
		}
	}
	if (socket_io_handler_) {
		socket_io_handler_->RemoveAllFilterIoSockets();
	}
	deb_assert(v_socket_ == 0);
	state_ = kDisconnected;
	state_list_.clear();	// A) we lost connection, no use trying for a while, or B) nothing more to do (=already empty).
}

bool MasterServerConnection::QueryMuxValid() {
	bool invalid = (!mux_socket_ || !mux_socket_->IsOpen());
	if (invalid) {
		OnDropSocket(v_socket_);
	}
	return !invalid;
}



void MasterServerConnection::OnDropSocket(cure::SocketIoHandler::VIoSocket* socket) {
	//deb_assert(socket == v_socket_);
	deb_assert(state_ == kConnect || state_ == kConnected || state_ == kUploadInfo || state_ == kDownloadList ||
		state_ == kOpenFirewall || state_ == kDisconnected || state_ == kConnecting);	// Just to make sure we're not deleted and gets callbacked anyhoo.
	if (socket == v_socket_ || socket == 0) {	// NULL means MUX socket is closing down!
		if (state_ == kConnecting) {
			connecter_->Join(0.5);
		}
		v_socket_ = 0;
		state_ = kDisconnected;
	}
}



const double MasterServerConnection::connected_idle_timeout_ = kMasterServerTimeout;
const double MasterServerConnection::disconnected_idle_timeout_ = 10.0;
const double MasterServerConnection::server_info_timeout_ = kMasterServerTimeout/3-1;

loginstance(kNetworkServer, MasterServerConnection);



}
