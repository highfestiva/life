
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "../../cure/include/socketiohandler.h"
#include "../../lepra/include/hirestimer.h"
#include "../../lepra/include/memberthread.h"
#include "../serverinfo.h"



namespace life {



class MasterServerConnection {
public:
	enum FirewallStatus {
		kFirewallError,
		kFirewallOpened,
		kFirewallUseLan,
	};

	MasterServerConnection(const str& master_address);
	virtual ~MasterServerConnection();
	const str& GetMasterAddress() const;

	void SetSocketInfo(cure::SocketIoHandler* mux_socket, double connect_timeout);

	void SendLocalInfo(const str& local_server_info);
	void AppendLocalInfo(const str& extra_server_info);
	void RequestServerList(const str& criterias);
	void RequestOpenFirewall(const str& game_server_connect_address);
	bool UpdateServerList(ServerInfoList& server_list) const;
	str GetServerListAsText() const;
	bool IsConnectError() const;
	FirewallStatus GetFirewallOpenStatus() const;
	const str& GetLanServerConnectAddress() const;
	double WaitUntilDone(double timeout, bool allow_reconnect);
	void GraceClose(double timeout, bool wait_until_done);
	bool CloseUnlessUploaded();
	void Tick();
	bool TickReceive(ServerInfo& server_info);

private:
	enum State {
		kConnect,
		kConnected,
		kUploadInfo,
		kDownloadList,
		kOpenFirewall,
		kDisconnected,
		kConnecting,
	};

	void QueryAddState(State state);
	void TriggerConnectTimer();
	void StepState();
	void ConnectEntry();
	bool UploadServerInfo();
	bool DownloadServerList();
	FirewallStatus OpenFirewall();
	bool SendAndAck(const str& data);
	bool SendAndRecv(const str& data, str& reply);
	bool Send(const str& data);
	bool Receive(str& data);
	void Close(bool error);
	bool QueryMuxValid();

	void OnDropSocket(cure::SocketIoHandler::VIoSocket* socket);

	str master_server_address_;
	State state_;
	std::list<State> state_list_;
	str local_server_info_;
	str uploaded_server_info_;
	str server_sort_criterias_;
	str server_list_;
	str game_server_connect_address_;
	str lan_game_server_address_;
	double connect_timeout_;
	cure::SocketIoHandler* socket_io_handler_;
	cure::SocketIoHandler::MuxIoSocket* mux_socket_;
	cure::SocketIoHandler::VIoSocket* v_socket_;
	MemberThread<MasterServerConnection>* connecter_;
	volatile int disconnect_counter_;
	HiResTimer idle_timer_;
	HiResTimer upload_timeout_;
	bool is_connect_error_;
	FirewallStatus last_firewall_open_status_;
	const static double connected_idle_timeout_;
	const static double disconnected_idle_timeout_;
	const static double server_info_timeout_;

	logclass();
};



}
