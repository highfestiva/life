
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "view.h"
#include "../life/serverinfo.h"



namespace Push {



class ServerSelectObserver;



class ServerListView: public View {
public:
	ServerListView(ServerSelectObserver* select_observer);

	void Tick();

private:
	void OnExit();
	void OnSelect(uitbc::Button*);

	ServerSelectObserver* select_observer_;
	life::ServerInfoList server_list_;
	bool is_master_connect_error_;
};



class ServerSelectObserver {
public:
	virtual void OnCancelJoinServer() = 0;
	virtual void OnRequestJoinServer(const str& server_address) = 0;
	virtual bool UpdateServerList(life::ServerInfoList& server_list) const = 0;
	virtual bool IsMasterServerConnectError() const = 0;
};



}
