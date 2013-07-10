
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../Life/LifeServer/ServerDelegate.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Life/Launcher.h"
#include "../Push.h"



namespace Cure
{
class FloatAttribute;
}



namespace Push
{



class PushServerDelegate: public Life::ServerDelegate
{
	typedef Life::ServerDelegate Parent;
public:
	PushServerDelegate(Life::GameServerManager* pGameServerManager);
	virtual ~PushServerDelegate();

	virtual void SetLevel(const str& pLevelName);

private:
	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;

	virtual void OnOpen();
	virtual void OnLogin(Life::Client* pClient);
	virtual void OnLogout(Life::Client* pClient);

	virtual void OnSelectAvatar(Life::Client* pClient, const Cure::UserAccount::AvatarId& pAvatarId);
	virtual void OnLoadAvatar(Life::Client* pClient, Cure::ContextObject* pAvatar);
	virtual void OnLoadObject(Cure::ContextObject* pObject);
	virtual void OnDeleteObject(Cure::ContextObject* pObject);

	virtual bool IsObjectLendable(Life::Client* pClient, Cure::ContextObject* pObject);

	virtual void PreEndTick();

	Cure::GameObjectId mLevelId;

	LOG_CLASS_DECLARE();
};



}
