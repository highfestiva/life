
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../hovertank.h"



namespace cure {
class ContextObject;
}



namespace HoverTank {



class Npc;



class GameServerLogic {
public:
	typedef std::unordered_set<cure::GameObjectId> AvatarIdSet;

	GameServerLogic();
	virtual ~GameServerLogic();

	virtual void Shoot(cure::ContextObject* avatar, int weapon) = 0;

	virtual cure::ContextObject* CreateAvatarForNpc(Npc* npc) = 0;
	virtual void AddAvatarToTeam(cure::ContextObject* avatar, int team) = 0;
	virtual void RemoveAvatar(cure::ContextObject* avatar) = 0;
	virtual const AvatarIdSet& GetAvatarsInTeam(int team) = 0;
};



}
