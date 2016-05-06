
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../cure/include/useraccount.h"
#include "../life.h"



namespace cure {
class ContextObject;
}



namespace life {



class Client;
class GameServerManager;



class ServerDelegate {
public:
	ServerDelegate(GameServerManager* game_server_manager);
	virtual ~ServerDelegate();

	virtual cure::ContextObject* CreateContextObject(const str& class_id) const = 0;

	virtual void OnOpen() = 0;
	virtual void OnLogin(Client* client) = 0;
	virtual void OnLogout(Client* client) = 0;

	virtual void OnSelectAvatar(Client* client, const cure::UserAccount::AvatarId& avatar_id) = 0;
	virtual void OnLoadAvatar(Client* client, cure::ContextObject* avatar) = 0;
	virtual void OnLoadObject(cure::ContextObject* object) = 0;
	virtual void OnDeleteObject(cure::ContextObject* object) = 0;

	virtual bool IsObjectLendable(Client* client, cure::ContextObject* object) = 0;

	virtual void PreEndTick() = 0;

protected:
	GameServerManager* game_server_manager_;

	logclass();
};



}
