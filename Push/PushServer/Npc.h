
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../Push.h"



namespace Push
{



class GameServerLogic;



class Npc: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	Npc(GameServerLogic* pLogic);
	virtual ~Npc();

	Cure::GameObjectId GetAvatarId() const;
	void SetAvatarId(Cure::GameObjectId pAvatarId);
	void StartCreateAvatar(float pTime);

private:
	virtual void OnTick();
	virtual void OnAlarm(int pAlarmId, void* pExtraData);

	GameServerLogic* mLogic;
	float mIntelligence;
	Cure::GameObjectId mAvatarId;
	HiResTimer mShootWait;

	LOG_CLASS_DECLARE();
};



}
