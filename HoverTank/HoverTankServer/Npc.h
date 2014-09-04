
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../HoverTank.h"



namespace HoverTank
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

	logclass();
};



}
