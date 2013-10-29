
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../HoverTank.h"



namespace Cure
{
class ContextObject;
}



namespace HoverTank
{



class Npc;



class GameServerLogic
{
public:
	typedef std::unordered_set<Cure::GameObjectId> AvatarIdSet;

	GameServerLogic();
	virtual ~GameServerLogic();

	virtual void Shoot(Cure::ContextObject* pAvatar, int pWeapon) = 0;

	virtual Cure::ContextObject* CreateAvatarForNpc(Npc* pNpc) = 0;
	virtual void AddAvatarToTeam(Cure::ContextObject* pAvatar, int pTeam) = 0;
	virtual void RemoveAvatar(Cure::ContextObject* pAvatar) = 0;
	virtual const AvatarIdSet& GetAvatarsInTeam(int pTeam) = 0;
};



}
