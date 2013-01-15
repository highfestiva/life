
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Life/LifeServer/ServerDelegate.h"
#include "../Push.h"
#include "GameServerLogic.h"
#include "ServerLauncher.h"



namespace Push
{



class PushServerDelegate: public Life::ServerDelegate, public GameServerLogic, public ServerLauncher
{
	typedef Life::ServerDelegate Parent;
public:
	PushServerDelegate(Life::GameServerManager* pGameServerManager);
	virtual ~PushServerDelegate();

private:
	virtual void OnOpen();
	virtual void OnLogin(Life::Client* pClient);
	virtual void OnLogout(Life::Client* pClient);

	virtual void OnLoadAvatar(Life::Client* pClient, Cure::ContextObject* pAvatar);
	virtual void OnLoadObject(Cure::ContextObject* pObject);
	virtual void OnDeleteObject(Cure::ContextObject* pObject);

	virtual bool IsObjectLendable(Life::Client* pClient, Cure::ContextObject* pObject);

	virtual void PreEndTick();

	virtual void GetBarrel(Cure::GameObjectId pOwnerId, TransformationF& pTransform, Vector3DF& pVelocity) const;
	virtual void Shoot(Cure::ContextObject* pAvatar);
	virtual void Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pExplosive, Cure::ContextObject* pHitObject,
		TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pHitBodyId);

	virtual Cure::ContextObject* CreateAvatarForNpc(Npc* pNpc);
	virtual void AddAvatarToTeam(Cure::ContextObject* pAvatar, int pTeam);
	virtual void RemoveAvatar(Cure::ContextObject* pAvatar);
	virtual const AvatarIdSet& GetAvatarsInTeam(int pTeam);

	void CreateNpc();
	void DeleteNpc();
	Npc* GetNpcByAvatar(Cure::GameObjectId pAvatarId) const;
	void CreateScore(const str& pPlayerName);
	void DeleteScore(const str& pPlayerName);
	void AddPoint(const str& pPrefix, const Cure::ContextObject* pAvatar, int pPoints);
	void Die(Cure::ContextObject* pAvatar);
	bool IsAvatarObject(const Cure::ContextObject* pObject) const;
	void TickNpcGhosts();

	Cure::GameObjectId mScoreInfoId;
	AvatarIdSet mAvatarTeamSets[2];
	AvatarIdSet mNpcSet;

	LOG_CLASS_DECLARE();
};



}
