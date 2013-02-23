
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Life/LifeServer/ServerDelegate.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Life/Launcher.h"
#include "../Push.h"
#include "GameServerLogic.h"



namespace Cure
{
class FloatAttribute;
}



namespace Push
{



class PushServerDelegate: public Life::ServerDelegate, public GameServerLogic, public Life::Launcher
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

	virtual void OnLoadAvatar(Life::Client* pClient, Cure::ContextObject* pAvatar);
	virtual void OnLoadObject(Cure::ContextObject* pObject);
	virtual void OnDeleteObject(Cure::ContextObject* pObject);

	virtual bool IsObjectLendable(Life::Client* pClient, Cure::ContextObject* pObject);

	virtual void PreEndTick();

	void OrderAirStrike(const Vector3DF& pPosition, float pFlyInAngle);

	virtual void Shoot(Cure::ContextObject* pAvatar, int pWeapon);
	virtual void Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength);
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject);

	virtual Cure::ContextObject* CreateAvatarForNpc(Npc* pNpc);
	virtual void AddAvatarToTeam(Cure::ContextObject* pAvatar, int pTeam);
	virtual void RemoveAvatar(Cure::ContextObject* pAvatar);
	virtual const AvatarIdSet& GetAvatarsInTeam(int pTeam);

	void CreateNpc();
	void DeleteNpc();
	Npc* GetNpcByAvatar(Cure::GameObjectId pAvatarId) const;
	void CreateScore(const str& pPlayerName, bool pCreatePing);
	void DeleteScore(const str& pPlayerName);
	void UpdatePing();
	void AddPoint(const str& pPrefix, const Cure::ContextObject* pAvatar, int pPoints);
	void SetPoints(const str& pPrefix, const Life::Client* pClient, int pPoints);
	void DrainHealth(Cure::ContextObject* pExplosive, Cure::ContextObject* pAvatar, Cure::FloatAttribute* pHealth, float pDamage);
	void Die(Cure::ContextObject* pAvatar);
	bool IsAvatarObject(const Cure::ContextObject* pObject) const;
	void TickNpcGhosts();

	enum Command
	{
		COMMAND_SET_LEVEL,
	};

	Cure::GameObjectId mLevelId;
	Cure::GameObjectId mScoreInfoId;
	AvatarIdSet mAvatarTeamSets[2];
	AvatarIdSet mNpcSet;
	HiResTimer mPingUpdateTimer;

	LOG_CLASS_DECLARE();
};



}
