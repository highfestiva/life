
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "View.h"



namespace Cure
{
class LoginId;
class RuntimeVariableScope;
}



namespace Life
{



class ServerSelectObserver;



class ServerListView: public View
{
public:
	ServerListView(ServerSelectObserver* pSelectObserver);

private:
	void OnExit();
	void OnSelect(UiTbc::Button*);

	ServerSelectObserver* mSelectObserver;
};



class ServerSelectObserver
{
public:
	virtual void CancelJoinServer() = 0;
	virtual void RequestJoinServer(const str& pServerAddress) = 0;
};



}
