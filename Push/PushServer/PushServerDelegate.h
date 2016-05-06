
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../life/lifeserver/serverdelegate.h"
#include "../../lepra/include/hirestimer.h"
#include "../../life/launcher.h"
#include "../push.h"



namespace cure {
class FloatAttribute;
}



namespace Push {



class PushServerDelegate: public life::ServerDelegate {
	typedef life::ServerDelegate Parent;
public:
	PushServerDelegate(life::GameServerManager* game_server_manager);
	virtual ~PushServerDelegate();

	virtual void SetLevel(const str& level_name);

private:
	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;

	virtual void OnOpen();
	virtual void OnLogin(life::Client* client);
	virtual void OnLogout(life::Client* client);

	virtual void OnSelectAvatar(life::Client* client, const cure::UserAccount::AvatarId& avatar_id);
	virtual void OnLoadAvatar(life::Client* client, cure::ContextObject* avatar);
	virtual void OnLoadObject(cure::ContextObject* object);
	virtual void OnDeleteObject(cure::ContextObject* object);

	virtual bool IsObjectLendable(life::Client* client, cure::ContextObject* object);

	virtual void PreEndTick();

	cure::GameObjectId level_id_;

	logclass();
};



}
