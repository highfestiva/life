
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../Life.h"



namespace Cure
{
class ContextObject;
}



namespace Life
{



class Client;
class GameServerManager;



class ServerDelegate
{
public:
	ServerDelegate(GameServerManager* pGameServerManager);
	virtual ~ServerDelegate();

	virtual void OnOpen() = 0;
	virtual void OnLogin(Client* pClient) = 0;
	virtual void OnLogout(Client* pClient) = 0;

	virtual void OnLoadAvatar(Client* pClient, Cure::ContextObject* pAvatar) = 0;
	virtual void OnLoadObject(Cure::ContextObject* pObject) = 0;
	virtual void OnDeleteObject(Cure::ContextObject* pObject) = 0;

	virtual void PreEndTick() = 0;

protected:
	GameServerManager* mGameServerManager;

	LOG_CLASS_DECLARE();
};



}
