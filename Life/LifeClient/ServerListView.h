
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "View.h"
#include "../ServerInfo.h"



namespace Life
{



class ServerSelectObserver;



class ServerListView: public View
{
public:
	ServerListView(ServerSelectObserver* pSelectObserver);

	void Tick();

private:
	void OnExit();
	void OnSelect(UiTbc::Button*);

	ServerSelectObserver* mSelectObserver;
	ServerInfoList mServerList;
	bool mIsMasterConnectError;
};



class ServerSelectObserver
{
public:
	virtual void OnCancelJoinServer() = 0;
	virtual void OnRequestJoinServer(const str& pServerAddress) = 0;
	virtual bool UpdateServerList(ServerInfoList& pServerList) const = 0;
	virtual bool IsMasterServerConnectError() const = 0;
};



}
