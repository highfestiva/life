
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "masterserver.h"
#include "../../lepra/include/socket.h"
#include "../../lepra/include/systemmanager.h"
#include "../serverinfo.h"
#include "masterservernetworkparser.h"



namespace life {



MasterServer::MasterServer(const str& port):
	mux_socket_(0),
	port_(port) {
}

MasterServer::~MasterServer() {
	SystemManager::AddQuitRequest(+1);
	delete (mux_socket_);
	mux_socket_ = 0;
}



bool MasterServer::Initialize() {
	SocketAddress address;
	str accept_address = "0.0.0.0:" + port_;
	if (!address.Resolve(accept_address)) {
		log_.Warningf("Could not resolve address '%s'.", accept_address.c_str());
		accept_address = ":" + port_;
		if (!address.Resolve(accept_address)) {
			log_.Errorf("Could not resolve address '%s'!", accept_address.c_str());
			return false;
		}
	}
	mux_socket_ = new UdpMuxSocket("MasterServer", address, true);
	if (!mux_socket_->IsOpen()) {
		log_.Errorf("Address '%s' seems busy. Terminating.", accept_address.c_str());
		return false;
	}
	log_.Headline("Up and running, awaiting connections.");
	SystemManager::SetQuitRequestCallback(SystemManager::QuitRequestCallback(this, &MasterServer::OnQuitRequest));
	return true;
}

bool MasterServer::Tick() {
	UdpVSocket* socket;
	if (mux_socket_->GetConnectionCount() == 0) {
		socket = mux_socket_->Accept();
	} else {
		socket = mux_socket_->PollAccept();
	}
	if (socket) {
		socket_table_.insert(socket);
	}
	KillDeadSockets();
	KillDeadServers();
	while ((socket = mux_socket_->PopReceiverSocket()) != 0) {
		uint8 receive_buffer[1024];
		int received_bytes = socket->Receive(receive_buffer, sizeof(receive_buffer));
		if (received_bytes > 0) {
			socket_timeout_table_.erase(socket);
			HandleReceive(socket, receive_buffer, received_bytes);
		}
	}
	return true;
}

void MasterServer::PollRoundTrip() {
	Tick();
}

float MasterServer::GetTickTimeReduction() const {
	return 0;
}

float MasterServer::GetPowerSaveAmount() const {
	if (!game_server_table_.empty()) {
		return 0;
	}
	if (!socket_timeout_table_.empty()) {
		return 0.1f;
	}
	return 1;
}



void MasterServer::KillDeadServers() {
	if (server_idle_timer_.QueryTimeDiff() < kMasterServerTimeout) {
		return;
	}
	server_idle_timer_.ClearTimeDiff();

	GameServerTable::iterator x = game_server_table_.begin();
	if (x != game_server_table_.end()) {
		GameServerInfo& info = x->second;
		if (info.idle_time_.QueryTimeDiff() > kMasterServerTimeout) {
			game_server_table_.erase(x++);
		} else {
			++x;
		}
	}
}

void MasterServer::KillDeadSockets() {
	if (keepalive_timer_.QueryTimeDiff() < kMasterServerTimeout) {
		return;
	}
	keepalive_timer_.ClearTimeDiff();

	// Kill all old and dead connections.
	while (!socket_timeout_table_.empty()) {
		UdpVSocket* socket = *socket_timeout_table_.begin();
		DropSocket(socket);
	}
	socket_timeout_table_.insert(socket_table_.begin(), socket_table_.end());
}

void MasterServer::OnQuitRequest(int) {
	if (mux_socket_) {
		mux_socket_->Close();
		mux_socket_->ReleaseSocketThreads();
	}
}

void MasterServer::HandleReceive(UdpVSocket* remote, const uint8* command, unsigned command_length) {
	str wide_data;
	if (!MasterServerNetworkParser::RawToStr(wide_data, command, command_length)) {
		log_.Error("Got garbled data from game server!");
		DropSocket(remote);
		return;
	}
	const str _command_line = wide_data;
	if (!HandleCommandLine(remote, _command_line)) {
		log_.Error("Got invalid command from game server!");
	}
}

bool MasterServer::HandleCommandLine(UdpVSocket* remote, const str& command_line) {
	ServerInfo _server_info;
	if (!MasterServerNetworkParser::ExtractServerInfo(command_line, _server_info, &remote->GetTargetAddress())) {
		return false;
	}
	log_volatile(log_.Debugf("Got command: '%s'.", command_line.c_str()));
	if (_server_info.command_ == kMasterServerUsi) {
		if (_server_info.remote_port_ < 0 || _server_info.player_count_ < 0 || _server_info.id_.empty()) {
			log_.Errorf("Got bad parameters to command (%s from game server)!", _server_info.command_.c_str());
			return false;
		}
		return RegisterGameServer(_server_info, remote);
	} else if (_server_info.command_ == kMasterServerDsl) {
		return SendServerList(remote);
	} else if (_server_info.command_ == kMasterServerOf) {
		return OpenFirewall(remote, _server_info);
	} else if (_server_info.command_ == kMasterServerDc) {
		DropSocket(remote);
		return true;
	} else {
		log_.Errorf("Got bad command (%s from game server)!", _server_info.command_.c_str());
	}
	return false;
}

bool MasterServer::RegisterGameServer(const ServerInfo& server_info, UdpVSocket* remote) {
	const bool activate = !server_info.remove_;

	bool ok = false;
	const str address = remote->GetTargetAddress().GetAsString();

	{
		ScopeLock lock(&lock_);
		GameServerTable::iterator x = game_server_table_.find(address);
		if (x != game_server_table_.end()) {
			GameServerInfo& info = x->second;
			if (activate) {
				if (info.id_ == server_info.id_) {
					info = server_info;
					info.idle_time_.PopTimeDiff();
					ok = true;
				} else {
					log_.Errorf("Got bad ID (%s from game server %s)!", server_info.id_.c_str(), info.name_.c_str());
				}
			} else {
				if (info.name_ == server_info.name_ && info.id_ == server_info.id_) {
					game_server_table_.erase(x);
					ok = true;
				} else {
					log_.Errorf("Could not drop game server %s, due to mismatching name and/or id.", info.name_.c_str());
				}
			}
		} else if (activate) {
			GameServerInfo info(server_info);
			game_server_table_.insert(GameServerTable::value_type(address, info));
			ok = true;
		} else {
			log_.Errorf("Could not drop game server %s, not found.", server_info.name_.c_str());
		}
		if (ok) {
			log_.RawPrint(kLevelDebug, "----------------------------------------\nServer list:\n");
			GameServerTable::iterator x = game_server_table_.begin();
			for (; x != game_server_table_.end(); ++x) {
				log_.RawPrint(kLevelDebug, x->second.name_ + " @ " + x->first + "\n");
			}
		}
	}

	if (ok) {
		Send(remote, "OK");
		return true;
	}
	return false;
}

bool MasterServer::SendServerList(UdpVSocket* remote) {
	str server_list;
	server_list.reserve(1024);
	{
		ScopeLock lock(&lock_);
		GameServerTable::iterator x = game_server_table_.begin();
		for (; x != game_server_table_.end(); ++x) {
			const strutil::strvec full_address = strutil::Split(x->first, ":");
			server_list += "--name \"" + x->second.name_ + "\" --address " + full_address[0] +
				" --port " + full_address[1] +
				" --player-count " + strutil::IntToString(x->second.player_count_, 10) + "\n";
		}
	}
	server_list += "OK";
	return Send(remote, server_list);
}

bool MasterServer::OpenFirewall(UdpVSocket* remote, const ServerInfo& server_info) {
	const str address = server_info.given_ip_address_ + strutil::Format(":%u", server_info.given_port_);
	SocketAddress socket_address;
	if (socket_address.Resolve(address)) {
		ScopeLock lock(&lock_);
		GameServerTable::iterator x = game_server_table_.find(address);
		if (x != game_server_table_.end()) {
			if (server_info.given_ip_address_ == server_info.remote_ip_address_) {
				// The client has the same IP as the server. This means it should try LAN connect instead of
				// going through the firewall. At least the risk of getting caught in a NAT without hairpin
				// is significantly decreased.
				const GameServerInfo& _server_info = x->second;
				log_.Info("Asking game client to use LAN instead, since they share IP.");
				log_volatile(log_.Debugf("Internal address is '%s'.", _server_info.internal_ip_address_.c_str()));
				return Send(remote, kMasterServerUl " --internal-address " + _server_info.internal_ip_address_ +
					" --internal-port " + strutil::IntToString(_server_info.internal_port_, 10));
			}

			UdpVSocket* game_server_socket = mux_socket_->GetVSocket(socket_address);
			if (game_server_socket) {
				if (Send(game_server_socket, kMasterServerOf " --address " + server_info.remote_ip_address_ + " --port " + strutil::IntToString(server_info.remote_port_, 10))) {
					log_.Info("Asked game server to open firewall!");
					return Send(remote, "OK");
				}
			}
		}
	}
	log_.Error("Got request for connecting to offline game server!");
	Send(remote, "Server offline.");
	return false;
}
bool MasterServer::Send(UdpVSocket* remote, const str& data) {
	uint8 raw_data[1024];
	if (data.size() > sizeof(raw_data)/3) {
		log_.Warning("Trying to send too big chunk to game server.");
		return false;
	}
	unsigned send_byte_count = MasterServerNetworkParser::StrToRaw(raw_data, data);
	if ((unsigned)remote->DirectSend(raw_data, send_byte_count) != send_byte_count) {
		log_.Warning("Transmission to game server failed.");
		return false;
	}
	return true;
}

void MasterServer::DropSocket(UdpVSocket* remote) {
	deb_assert(socket_table_.find(remote) != socket_table_.end());
	socket_timeout_table_.erase(remote);
	socket_table_.erase(remote);
	mux_socket_->CloseSocket(remote);
}



MasterServer::GameServerInfo::GameServerInfo(const ServerInfo& server_info):
	ServerInfo(server_info)
/*	command_(server_info.command_),
	name_(server_info.name_),
	id_(server_info.id_),
	given_ip_address_(server_info.given_ip_address_),
	internal_ip_address_(server_info.internal_ip_address_),
	remote_ip_address_(server_info.remote_ip_address_),
	given_port_(server_info.given_port_),
	internal_port_(server_info.internal_port_),
	remote_port_(server_info.remote_port_),
	player_count_(server_info.player_count_),
	remove_(server_info.remove_),
	ping_(server_info.ping_)*/
{
	idle_time_.PopTimeDiff();
}



loginstance(kNetworkServer, MasterServer);



}
