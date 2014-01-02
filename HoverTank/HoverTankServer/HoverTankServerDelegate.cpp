
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "HoverTankServerDelegate.h"
#include "../../Cure/Include/ConsoleManager.h"
#include "../../Cure/Include/Health.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Cure/Include/IntAttribute.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Cure/Include/Spawner.h"
#include "../../Lepra/Include/Random.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "../../Life/LifeServer/ServerFastProjectile.h"
#include "../../Life/LifeServer/ServerMine.h"
#include "../../Life/LifeServer/ServerProjectile.h"
#include "../../Life/Explosion.h"
#include "../../Life/ProjectileUtil.h"
#include "../RtVar.h"
#include "../RtVar.h"
#include "../Version.h"
#include "BombPlane.h"
#include "Npc.h"
#include "HoverTankServerConsole.h"

#define KILLS	_T("int_kills:")
#define DEATHS	_T("int_deaths:")
#define PING	_T("int_ping:")



namespace HoverTank
{



HoverTankServerDelegate::HoverTankServerDelegate(Life::GameServerManager* pGameServerManager):
	Parent(pGameServerManager),
	mLevelId(0),
	mScoreInfoId(0)
{
	CURE_RTVAR_SET(mGameServerManager->GetVariableScope(), RTVAR_GAME_NPCSKILL, 0.5);
	CURE_RTVAR_SET(mGameServerManager->GetVariableScope(), RTVAR_DEBUG_SERVERINDICATEHIT, 0.0);
}

HoverTankServerDelegate::~HoverTankServerDelegate()
{
	mScoreInfoId = 0;
	mLevelId = 0;
}



void HoverTankServerDelegate::SetLevel(const str& pLevelName)
{
	ScopeLock lLock(mGameServerManager->GetTickLock());
	if (mLevelId)
	{
		mGameServerManager->DeleteContextObject(mLevelId);
	}

	Cure::ContextObject* lLevel = mGameServerManager->GameManager::CreateContextObject(pLevelName, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	mLevelId = lLevel->GetInstanceId();
	lLevel->StartLoading();
}



Cure::ContextObject* HoverTankServerDelegate::CreateContextObject(const str& pClassId) const
{
	if (strutil::StartsWith(pClassId, _T("mine_")))
	{
		return new Life::ServerMine(mGameServerManager->GetResourceManager(), pClassId, (HoverTankServerDelegate*)this);
	}
	else if (strutil::StartsWith(pClassId, _T("deltawing")))
	{
		return new BombPlane(mGameServerManager->GetResourceManager(), pClassId, (HoverTankServerDelegate*)this, Vector3DF());
	}
	return new Cure::CppContextObject(mGameServerManager->GetResourceManager(), pClassId);
}

void HoverTankServerDelegate::OnOpen()
{
	new HoverTankServerConsole(this, mGameServerManager->GetConsoleManager()->GetConsoleCommandManager());

	SetLevel(_T("level_02"));

	Cure::ContextObject* lScoreInfo = mGameServerManager->GameManager::CreateContextObject(_T("score_info"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	mScoreInfoId = lScoreInfo->GetInstanceId();
	lScoreInfo->SetLoadResult(true);
}

void HoverTankServerDelegate::OnLogin(Life::Client* pClient)
{
#ifdef LEPRA_DEBUG
	if (mGameServerManager->GetLoggedInClientCount() == 1)
	{
		deb_assert(mNpcSet.empty());
	}
#endif // Debug

	// Create scores.
	const str lLoginName = strutil::Encode(pClient->GetUserConnection()->GetLoginName());
	CreateScore(lLoginName, true);

	// Create another computer opponent, to balance teams.
	CreateNpc();
}

void HoverTankServerDelegate::OnLogout(Life::Client* pClient)
{
	// Drop scores.
	const str lLoginName = strutil::Encode(pClient->GetUserConnection()->GetLoginName());
	DeleteScore(lLoginName);

	// Drop a computer opponent, to balance teams.
	DeleteNpc();

#ifdef LEPRA_DEBUG
	if (mGameServerManager->GetLoggedInClientCount() == 0)
	{
		deb_assert(mNpcSet.empty());
	}
#endif // Debug
}



void HoverTankServerDelegate::OnSelectAvatar(Life::Client* pClient, const Cure::UserAccount::AvatarId& pAvatarId)
{
	const Cure::GameObjectId lPreviousAvatarId = pClient->GetAvatarId();
	if (lPreviousAvatarId)
	{
		mLog.Info(_T("User ")+strutil::Encode(pClient->GetUserConnection()->GetLoginName())+_T(" had an avatar, replacing it."));
		pClient->SetAvatarId(0);
		Cure::ContextObject* lObject = mGameServerManager->GetContext()->GetObject(lPreviousAvatarId);
		if (lObject)
		{
			TransformationF lTransform;
			lTransform.SetPosition(lObject->GetPosition());
			lTransform.GetPosition() += Vector3DF(0, 0, 2);
			Vector3DF lEulerAngles;
			lObject->GetOrientation().GetEulerAngles(lEulerAngles);
			QuaternionF q;
			q.SetEulerAngles(lEulerAngles.x, 0, 0);
			lTransform.SetOrientation(q * lTransform.GetOrientation());
		}
		mGameServerManager->DeleteContextObject(lPreviousAvatarId);
	}

	Cure::Spawner* lSpawner = mGameServerManager->GetAvatarSpawner(mLevelId);
	if (!lSpawner)
	{
		mLog.AError("No player spawner in level!");
		return;
	}
	mLog.Info(_T("Loading avatar '")+pAvatarId+_T("' for user ")+strutil::Encode(pClient->GetUserConnection()->GetLoginName())+_T("."));
	Cure::ContextObject* lObject = mGameServerManager->GameServerManager::Parent::CreateContextObject(pAvatarId, Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
	lSpawner->PlaceObject(lObject, -1);
	pClient->SetAvatarId(lObject->GetInstanceId());
	lObject->SetExtraData((void*)(intptr_t)pClient->GetUserConnection()->GetAccountId());
	lObject->StartLoading();
}

void HoverTankServerDelegate::OnLoadAvatar(Life::Client* pClient, Cure::ContextObject* pAvatar)
{
	Cure::Health::Set(pAvatar, 1);
	if (pClient)
	{
		// User avatar.
		AddAvatarToTeam(pAvatar, 0);
	}
	else
	{
		// NPC avatar.
		AddAvatarToTeam(pAvatar, 1);
	}
}

void HoverTankServerDelegate::OnLoadObject(Cure::ContextObject* pObject)
{
	(void)pObject;
}

void HoverTankServerDelegate::OnDeleteObject(Cure::ContextObject* pObject)
{
	if (pObject)
	{
		RemoveAvatar(pObject);
	}
}



bool HoverTankServerDelegate::IsObjectLendable(Life::Client* pClient, Cure::ContextObject* pObject)
{
	(void)pClient;
	return !strutil::StartsWith(pObject->GetClassId(), _T("hover_tank"));
}



void HoverTankServerDelegate::PreEndTick()
{
	TickNpcGhosts();

	if (mPingUpdateTimer.QueryTimeDiff() > 5.0)
	{
		mPingUpdateTimer.ClearTimeDiff();
		UpdatePing();
	}
}



void HoverTankServerDelegate::OrderAirStrike(const Vector3DF& pPosition, float pFlyInAngle)
{
	const float lPlaneDistance = 1000;

	Cure::ContextObject* lPlane = new BombPlane(mGameServerManager->GetResourceManager(), _T("deltawing"), this, pPosition);
	mGameServerManager->AddContextObject(lPlane, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	TransformationF t;
	t.GetPosition().Set(lPlaneDistance*::sin(pFlyInAngle), lPlaneDistance*::cos(pFlyInAngle), 30);
	t.GetPosition().x += pPosition.x;
	t.GetPosition().y += pPosition.y;
	t.GetOrientation().RotateAroundOwnX(PIF/2);
	t.GetOrientation().RotateAroundWorldZ(-pFlyInAngle);
	lPlane->SetInitialTransform(t);
	lPlane->StartLoading();
}



void HoverTankServerDelegate::Shoot(Cure::ContextObject* pAvatar, int pWeapon)
{
	str lAmmo;
	bool lIsFast = true;
	Cure::NetworkObjectType lNetworkType = Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED;
	switch (pWeapon)
	{
		case 0:		lAmmo = _T("bullet");	lNetworkType = Cure::NETWORK_OBJECT_LOCAL_ONLY;	break;
		case 1:		lAmmo = _T("grenade");							break;
		case 2:		lAmmo = _T("rocket");							break;
		case -10:	lAmmo = _T("bomb");	lIsFast = false;				break;
		default: deb_assert(false); return;
	}
	Cure::ContextObject* lProjectile;
	if (lIsFast)
	{
		lProjectile = new Life::ServerFastProjectile(mGameServerManager->GetResourceManager(), lAmmo, this);
	}
	else
	{
		lProjectile = new Life::ServerProjectile(mGameServerManager->GetResourceManager(), lAmmo, 0, this);
	}
	mGameServerManager->AddContextObject(lProjectile, lNetworkType, 0);
	log_volatile(mLog.Debugf(_T("Shooting projectile with ID %i!"), (int)lProjectile->GetInstanceId()));
	lProjectile->SetOwnerInstanceId(pAvatar->GetInstanceId());
	TransformationF t;
	Vector3DF v;
	if (Life::ProjectileUtil::GetBarrel(lProjectile, t, v))
	{
		lProjectile->SetInitialTransform(t);
		lProjectile->StartLoading();
	}

	if (lNetworkType == Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		// Transmit the shoot event rather than the projectile itself.
		Life::Client* lClient = mGameServerManager->GetClientByObject(pAvatar);
		mGameServerManager->BroadcastNumberMessage(lClient, false, Cure::MessageNumber::INFO_TOOL_0, pAvatar->GetInstanceId(), (float)pWeapon);
	}
}

void HoverTankServerDelegate::Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength)
{
	(void)pExplosiveGeometry;
	(void)pVelocity;
	(void)pNormal;

	float lIndicateHit;
	CURE_RTVAR_GET(lIndicateHit, =(float), mGameServerManager->GetVariableScope(), RTVAR_DEBUG_SERVERINDICATEHIT, 0.0);
	mGameServerManager->IndicatePosition(pPosition, lIndicateHit);

	ScopeLock lLock(mGameServerManager->GetTickLock());
	TBC::PhysicsManager* lPhysicsManager = mGameServerManager->GetPhysicsManager();
	Cure::ContextManager::ContextObjectTable lObjectTable = mGameServerManager->GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (!lObject->IsLoaded())
		{
			continue;
		}
		const str& lClassId = lObject->GetClassId();
		if (strutil::StartsWith(lClassId, _T("bomb")))	// Prevent bombs from hover_tanking each other away from the target!
		{
			continue;
		}
		float lEnduranceReciproc = 1;
		if (strutil::StartsWith(lClassId, _T("deltawing")))	// Prevent bombers from getting their noses hover_tanked upwards when bombs go off!
		{
			lEnduranceReciproc = 0.1f;
		}
		const float lForce = Life::Explosion::CalculateForce(lPhysicsManager, lObject, pPosition, pStrength * lEnduranceReciproc);
		if (lForce > 0 && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			Cure::FloatAttribute* lHealth = Cure::Health::GetAttribute(lObject);
			if (lHealth)
			{
				DrainHealth(pExplosive, lObject, lHealth, lForce*Random::Normal(0.51f, 0.05f, 0.3f, 0.5f));
			}
			x->second->ForceSend();
		}
		Life::Explosion::PushObject(lPhysicsManager, lObject, pPosition, pStrength * lEnduranceReciproc);
	}
}

void HoverTankServerDelegate::OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject)
{
	float lIndicateHit;
	CURE_RTVAR_GET(lIndicateHit, =(float), mGameServerManager->GetVariableScope(), RTVAR_DEBUG_SERVERINDICATEHIT, 0.0);
	mGameServerManager->IndicatePosition(pBullet->GetPosition(), lIndicateHit);

	Cure::FloatAttribute* lHealth = Cure::Health::GetAttribute(pHitObject);
	if (lHealth)
	{
		DrainHealth(pBullet, pHitObject, lHealth, Random::Normal(0.17f, 0.01f, 0.1f, 0.3f));
		const float lIncomingAngle = 2*PIF * Random::Uniform(0.0f, 1.0f);
		OrderAirStrike(pHitObject->GetPosition(), lIncomingAngle);
		Vector3DF v(27*::sin(lIncomingAngle), 27*::cos(lIncomingAngle), 2);
		QuaternionF q = QuaternionF();
		q.RotateAroundWorldZ(PIF/4);
		const Vector3DF r = RNDVEC(3.0f);
		OrderAirStrike(pHitObject->GetPosition() + q*v+r, lIncomingAngle);
		q.RotateAroundWorldZ(-PIF/2);
		OrderAirStrike(pHitObject->GetPosition() + q*v+r, lIncomingAngle);
	}
}



Cure::ContextObject* HoverTankServerDelegate::CreateAvatarForNpc(Npc* pNpc)
{
	double lSpawnPart;
	CURE_RTVAR_GET(lSpawnPart, =, mGameServerManager->GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	if (Random::Uniform(0.0, 0.999) >= lSpawnPart)
	{
		return 0;
	}

	Cure::Spawner* lSpawner = mGameServerManager->GetAvatarSpawner(mLevelId);
	if (!lSpawner)
	{
		mLog.AError("No NPC spawner in level!");
		return 0;
	}

	Cure::ContextObject* lAvatar = mGameServerManager->GameManager::CreateContextObject(_T("hover_tank_01"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	lSpawner->PlaceObject(lAvatar, -1);
	lAvatar->SetExtraData((void*)-1);
	lAvatar->StartLoading();
	pNpc->SetAvatarId(lAvatar->GetInstanceId());
	return lAvatar;
}

void HoverTankServerDelegate::AddAvatarToTeam(Cure::ContextObject* pAvatar, int pTeam)
{
	deb_assert(pAvatar);
	deb_assert(pTeam == 0 || pTeam == 1);
	deb_assert(!IsAvatarObject(pAvatar));
	mAvatarTeamSets[pTeam].insert(pAvatar->GetInstanceId());
	Cure::IntAttribute* lTeam = (Cure::IntAttribute*)pAvatar->GetAttribute(_T("int_team"));
	if (lTeam)
	{
		lTeam->SetValue(pTeam);
	}
	else
	{
		new Cure::IntAttribute(pAvatar, _T("int_team"), pTeam);
	}
}

void HoverTankServerDelegate::RemoveAvatar(Cure::ContextObject* pAvatar)
{
	AvatarIdSet::iterator x;
	bool lFound0 = ((x = mAvatarTeamSets[0].find(pAvatar->GetInstanceId())) != mAvatarTeamSets[0].end());
	if (lFound0)
	{
		mAvatarTeamSets[0].erase(x);
	}
	bool lFound1 = ((x = mAvatarTeamSets[1].find(pAvatar->GetInstanceId())) != mAvatarTeamSets[1].end());
	if (lFound1)
	{
		mAvatarTeamSets[1].erase(x);
	}
	if (!lFound0 && !lFound1)
	{
		return;
	}


	Npc* lNpc = GetNpcByAvatar(pAvatar->GetInstanceId());
	if (lNpc)
	{
			lNpc->SetAvatarId(0);
	}
}

const HoverTankServerDelegate::AvatarIdSet& HoverTankServerDelegate::GetAvatarsInTeam(int pTeam)
{
	deb_assert(pTeam == 0 || pTeam == 1);
	return mAvatarTeamSets[pTeam];
}



void HoverTankServerDelegate::CreateNpc()
{
	Npc* lNpc = new Npc(this);
	mGameServerManager->GetContext()->AddLocalObject(lNpc);
	mNpcSet.insert(lNpc->GetInstanceId());
	lNpc->StartCreateAvatar(0.1f);

	const str lPlayerName = strutil::Format(_T("NPC %u"), lNpc->GetInstanceId());
	CreateScore(lPlayerName, false);
}

void HoverTankServerDelegate::DeleteNpc()
{
	deb_assert(!mNpcSet.empty());
	AvatarIdSet::iterator x = mNpcSet.begin();
	mLog.Headlinef(_T("Deleting NPC %u."), *x);
	mGameServerManager->GetContext()->PostKillObject(*x);

	const str lPlayerName = strutil::Format(_T("NPC %u"), *x);
	DeleteScore(lPlayerName);

	mNpcSet.erase(x);
}

Npc* HoverTankServerDelegate::GetNpcByAvatar(Cure::GameObjectId pAvatarId) const
{
	for (AvatarIdSet::const_iterator x = mNpcSet.begin(); x != mNpcSet.end(); ++x)
	{
		Npc* lNpc = (Npc*)mGameServerManager->GetContext()->GetObject(*x);
		if (lNpc && lNpc->GetAvatarId() == pAvatarId)
		{
			return lNpc;
		}
	}
	return 0;
}

void HoverTankServerDelegate::CreateScore(const str& pPlayerName, bool pCreatePing)
{
	deb_assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	deb_assert(lScoreInfo);
	new Cure::IntAttribute(lScoreInfo, KILLS + pPlayerName, 0);
	new Cure::IntAttribute(lScoreInfo, DEATHS + pPlayerName, 0);
	if (pCreatePing)
	{
		new Cure::IntAttribute(lScoreInfo, PING + pPlayerName, 0);
	}
}

void HoverTankServerDelegate::DeleteScore(const str& pPlayerName)
{
	deb_assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	deb_assert(lScoreInfo);
	lScoreInfo->DeleteAttribute(KILLS + pPlayerName);
	lScoreInfo->DeleteAttribute(DEATHS + pPlayerName);
	lScoreInfo->DeleteAttribute(PING + pPlayerName);
}

void HoverTankServerDelegate::UpdatePing()
{
	const Cure::TimeManager* lTimeManager = mGameServerManager->GetTimeManager();
	typedef Life::GameServerManager::AccountClientTable ClientTable;
	const ClientTable& lClients = mGameServerManager->GetAccountClientTable();
	ClientTable::ConstIterator x = lClients.First();
	for (; x != lClients.End(); ++x)
	{
		int lPing = (int)(lTimeManager->ConvertPhysicsFramesToSeconds((int)x.GetObject()->GetPhysicsFrameAheadCount()) * 1000);
		lPing = std::abs(lPing) * 2;
		SetPoints(PING, x.GetObject(), lPing);
	}
}

void HoverTankServerDelegate::AddPoint(const str& pPrefix, const Cure::ContextObject* pAvatar, int pPoints)
{
	if (!pAvatar)
	{
		return;
	}
	Cure::ContextObject* lAvatar = (Cure::ContextObject*)pAvatar;
	Life::Client* lClient = mGameServerManager->GetClientByObject(lAvatar);
	str lPlayerName;
	if (lClient)
	{
		lPlayerName = strutil::Encode(lClient->GetUserConnection()->GetLoginName());
	}
	else
	{
		Npc* lNpc = GetNpcByAvatar(pAvatar->GetInstanceId());
		if (!lNpc)
		{
			return;
		}
		lPlayerName = strutil::Format(_T("NPC %u"), lNpc->GetInstanceId());
	}
	deb_assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	deb_assert(lScoreInfo);
	Cure::IntAttribute* lAttribute = (Cure::IntAttribute*)lScoreInfo->GetAttribute(pPrefix+lPlayerName);
	if (lAttribute)
	{
		lAttribute->SetValue(lAttribute->GetValue() + pPoints);
	}
}

void HoverTankServerDelegate::SetPoints(const str& pPrefix, const Life::Client* pClient, int pPoints)
{
	const str lPlayerName = strutil::Encode(pClient->GetUserConnection()->GetLoginName());
	deb_assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	deb_assert(lScoreInfo);
	Cure::IntAttribute* lAttribute = (Cure::IntAttribute*)lScoreInfo->GetAttribute(pPrefix+lPlayerName);
	deb_assert(lAttribute);
	if (lAttribute)
	{
		lAttribute->SetValue(pPoints);
	}
}

void HoverTankServerDelegate::DrainHealth(Cure::ContextObject* pExplosive, Cure::ContextObject* pObject, Cure::FloatAttribute* pHealth, float pDamage)
{
	deb_assert(pHealth);
	const float lPriorHealth = pHealth->GetValue();
	float lRemainingHealth = lPriorHealth - pDamage;
	if (lPriorHealth > 0)
	{
		pHealth->SetValue(lRemainingHealth);
	}
	if (lPriorHealth > 0 && lRemainingHealth <= 0 && IsAvatarObject(pObject))
	{
		Cure::ContextObject* lAvatar = pObject;
		AddPoint(DEATHS, lAvatar, +1);
		if (pExplosive->GetOwnerInstanceId())
		{
			const int lPoints = (pExplosive->GetOwnerInstanceId() == lAvatar->GetInstanceId()) ? -1 : +1;	// Kills oneself?
			AddPoint(KILLS, mGameServerManager->GetContext()->GetObject(pExplosive->GetOwnerInstanceId()), lPoints);
		}
		Die(lAvatar);
	}
}

void HoverTankServerDelegate::Die(Cure::ContextObject* pAvatar)
{
	Life::Explosion::FallApart(mGameServerManager->GetPhysicsManager(), (Cure::CppContextObject*)pAvatar);
	mGameServerManager->DeleteContextObjectDelay(pAvatar, 2);
	mGameServerManager->BroadcastNumberMessage(0, true, Cure::MessageNumber::INFO_FALL_APART, pAvatar->GetInstanceId(), 0);
}

bool HoverTankServerDelegate::IsAvatarObject(const Cure::ContextObject* pObject) const
{
	return mAvatarTeamSets[0].find(pObject->GetInstanceId()) != mAvatarTeamSets[0].end() ||
		mAvatarTeamSets[1].find(pObject->GetInstanceId()) != mAvatarTeamSets[1].end();
}

void HoverTankServerDelegate::TickNpcGhosts()
{
	ScopeLock lLock(mGameServerManager->GetTickLock());
	const Cure::ContextManager* lContextManager = mGameServerManager->GetContext();
	const int lStepCount = mGameServerManager->GetTimeManager()->GetAffordedPhysicsStepCount();
	const float lPhysicsFrameTime = mGameServerManager->GetTimeManager()->GetAffordedPhysicsStepTime();
	for (int lTeam = 0; lTeam <= 1; ++lTeam)
	{
		AvatarIdSet::iterator x = mAvatarTeamSets[lTeam].begin();
		for (; x != mAvatarTeamSets[lTeam].end(); ++x)
		{
			Cure::ContextObject* lAvatar = lContextManager->GetObject(*x);
			if (lAvatar && lAvatar->GetExtraData() == 0)	// Exists && NPC check.
			{
				lAvatar->GetNetworkOutputGhost()->GhostStep(lStepCount, lPhysicsFrameTime);
				const Cure::ObjectPositionalData* lPositionalData = 0;
				if (!lAvatar->UpdateFullPosition(lPositionalData))
				{
					continue;
				}
				if (!lPositionalData->IsSameStructure(*lAvatar->GetNetworkOutputGhost()))
				{
					lAvatar->GetNetworkOutputGhost()->CopyData(lPositionalData);
					continue;
				}
				float lResyncOnDiff;
				CURE_RTVAR_GET(lResyncOnDiff, =(float), mGameServerManager->GetVariableScope(), RTVAR_NETPHYS_RESYNCONDIFFGT, 0.2);
				if (lPositionalData->GetScaledDifference(lAvatar->GetNetworkOutputGhost()) > lResyncOnDiff)
				{
					log_volatile(mLog.Debugf(_T("NPC avatar %s (%u) sending pos due to deviation."), lAvatar->GetClassId().c_str(), lAvatar->GetInstanceId()));
					lAvatar->GetNetworkOutputGhost()->CopyData(lPositionalData);
					mGameServerManager->GetContext()->AddPhysicsSenderObject(lAvatar);
				}
			}
		}
	}
}



LOG_CLASS_DEFINE(GAME, HoverTankServerDelegate);



}
