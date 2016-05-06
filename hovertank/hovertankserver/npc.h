
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/cppcontextobject.h"
#include "../../lepra/include/hirestimer.h"
#include "../hovertank.h"



namespace HoverTank {



class GameServerLogic;



class Npc: public cure::CppContextObject {
public:
	typedef cure::CppContextObject Parent;

	Npc(GameServerLogic* logic);
	virtual ~Npc();

	cure::GameObjectId GetAvatarId() const;
	void SetAvatarId(cure::GameObjectId avatar_id);
	void StartCreateAvatar(float time);

private:
	virtual void OnTick();
	virtual void OnAlarm(int alarm_id, void* extra_data);

	GameServerLogic* logic_;
	float intelligence_;
	cure::GameObjectId avatar_id_;
	HiResTimer shoot_wait_;

	logclass();
};



}
