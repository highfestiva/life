
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerDelegate.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Cure/Include/IntAttribute.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "../../Life/ProjectileUtil.h"
#include "../RtVar.h"
#include "../RtVar.h"
#include "../Explosion.h"
#include "../Version.h"
#include "Npc.h"
#include "ServerFastProjectile.h"
#include "ServerProjectile.h"

#define KILLS	_T("int_kills:")
#define DEATHS	_T("int_deaths:")
#define PING	_T("int_ping:")



namespace Push
{



PushServerDelegate::PushServerDelegate(Life::GameServerManager* pGameServerManager):
	Parent(pGameServerManager),
	mScoreInfoId(0)
{
	CURE_RTVAR_SET(mGameServerManager->GetVariableScope(), RTVAR_GAME_NPCSKILL, 0.5);
	CURE_RTVAR_SET(mGameServerManager->GetVariableScope(), RTVAR_DEBUG_SERVERINDICATEHIT, 0.0);
}

PushServerDelegate::~PushServerDelegate()
{
	mScoreInfoId = 0;
}



void PushServerDelegate::OnOpen()
{
	Cure::ContextObject* lScoreInfo = mGameServerManager->GameManager::CreateContextObject(_T("score_info"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	lScoreInfo->SetLoadResult(true);
	mScoreInfoId = lScoreInfo->GetInstanceId();
}

void PushServerDelegate::OnLogin(Life::Client* pClient)
{
#ifdef LEPRA_DEBUG
	if (mGameServerManager->GetLoggedInClientCount() == 1)
	{
		assert(mNpcSet.empty());
	}
#endif // Debug

	// Create scores.
	const str lLoginName = strutil::Encode(pClient->GetUserConnection()->GetLoginName());
	CreateScore(lLoginName, true);

	// Create another computer opponent, to balance teams.
	CreateNpc();
}

void PushServerDelegate::OnLogout(Life::Client* pClient)
{
	// Drop scores.
	const str lLoginName = strutil::Encode(pClient->GetUserConnection()->GetLoginName());
	DeleteScore(lLoginName);

	// Drop a computer opponent, to balance teams.
	DeleteNpc();

#ifdef LEPRA_DEBUG
	if (mGameServerManager->GetLoggedInClientCount() == 0)
	{
		assert(mNpcSet.empty());
	}
#endif // Debug
}



void PushServerDelegate::OnLoadAvatar(Life::Client* pClient, Cure::ContextObject* pAvatar)
{
	new Cure::FloatAttribute(pAvatar, _T("float_health"), 1);
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

void PushServerDelegate::OnLoadObject(Cure::ContextObject* pObject)
{
	(void)pObject;
}

void PushServerDelegate::OnDeleteObject(Cure::ContextObject* pObject)
{
	if (pObject)
	{
		RemoveAvatar(pObject);
	}
}



bool PushServerDelegate::IsObjectLendable(Life::Client* pClient, Cure::ContextObject* pObject)
{
	(void)pClient;
	return !strutil::StartsWith(pObject->GetClassId(), _T("hover_tank"));
}



void PushServerDelegate::PreEndTick()
{
	TickNpcGhosts();

	if (mPingUpdateTimer.QueryTimeDiff() > 5.0)
	{
		mPingUpdateTimer.ClearTimeDiff();
		UpdatePing();
	}
}



void PushServerDelegate::Shoot(Cure::ContextObject* pAvatar, int pWeapon)
{
	str lAmmo;
	Cure::NetworkObjectType lNetworkType = Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED;
	switch (pWeapon)
	{
		case 0:	lAmmo = _T("bullet");	lNetworkType = Cure::NETWORK_OBJECT_LOCAL_ONLY;	break;
		case 1:	lAmmo = _T("grenade");							break;
		case 2:	lAmmo = _T("rocket");							break;
		default: assert(false); return;
	}
	Cure::ContextObject* lProjectile = new ServerFastProjectile(mGameServerManager->GetResourceManager(), lAmmo, this);
	mGameServerManager->AddContextObject(lProjectile, lNetworkType, 0);
	log_volatile(mLog.Debugf(_T("Shooting projectile with ID %i!"), (int)lProjectile->GetInstanceId()));
	lProjectile->SetOwnerInstanceId(pAvatar->GetInstanceId());
	TransformationF t;
	Vector3DF v;
	Life::ProjectileUtil::GetBarrel(lProjectile, t, v);
	lProjectile->SetInitialTransform(t);
	lProjectile->StartLoading();

	if (lNetworkType == Cure::NETWORK_OBJECT_LOCAL_ONLY)
	{
		// Transmit the shoot event rather than the projectile itself.
		Life::Client* lClient = mGameServerManager->GetClientByObject(pAvatar);
		mGameServerManager->BroadcastNumberMessage(lClient, false, Cure::MessageNumber::INFO_TOOL_0, pAvatar->GetInstanceId(), (float)pWeapon);
	}
}

void PushServerDelegate::Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition)
{
	(void)pExplosiveGeometry;

	float lIndicateHit;
	CURE_RTVAR_GET(lIndicateHit, =(float), mGameServerManager->GetVariableScope(), RTVAR_DEBUG_SERVERINDICATEHIT, 0.0);
	mGameServerManager->IndicatePosition(pPosition, lIndicateHit);

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
		const float lForce = Explosion::PushObject(lPhysicsManager, lObject, pPosition, 1.0f);
		if (lForce > 0 && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			if (IsAvatarObject(lObject))
			{
				DrainHealth(pExplosive, lObject, lForce*(float)Random::Normal(0.51, 0.05, 0.3, 0.5));
			}
			x->second->ForceSend();
		}
	}
}

void PushServerDelegate::OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject)
{
	float lIndicateHit;
	CURE_RTVAR_GET(lIndicateHit, =(float), mGameServerManager->GetVariableScope(), RTVAR_DEBUG_SERVERINDICATEHIT, 0.0);
	mGameServerManager->IndicatePosition(pBullet->GetPosition(), lIndicateHit);

	if (IsAvatarObject(pHitObject))
	{
		DrainHealth(pBullet, pHitObject, (float)Random::Normal(0.17, 0.01, 0.1, 0.3));
	}
}



Cure::ContextObject* PushServerDelegate::CreateAvatarForNpc(Npc* pNpc)
{
	double lSpawnPart;
	CURE_RTVAR_GET(lSpawnPart, =, mGameServerManager->GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	if (Random::Uniform(0, 0.999) >= lSpawnPart)
	{
		return 0;
	}

	Cure::ContextObject* lAvatar = mGameServerManager->GameManager::CreateContextObject(_T("hover_tank_01"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	TransformationF lTransform;
	lTransform.SetPosition(Vector3DF((float)Random::Uniform(3, 100), (float)Random::Uniform(-100, +100), 10));
	const float a = 1.0f/::sqrt(2.0f);
	lTransform.SetOrientation(QuaternionF(0, 0, -a, -a));
	lAvatar->SetInitialTransform(lTransform);
	lAvatar->SetExtraData((void*)-1);
	lAvatar->StartLoading();
	pNpc->SetAvatarId(lAvatar->GetInstanceId());
	return lAvatar;
}

void PushServerDelegate::AddAvatarToTeam(Cure::ContextObject* pAvatar, int pTeam)
{
	assert(pAvatar);
	assert(pTeam == 0 || pTeam == 1);
	assert(!IsAvatarObject(pAvatar));
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

void PushServerDelegate::RemoveAvatar(Cure::ContextObject* pAvatar)
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

const PushServerDelegate::AvatarIdSet& PushServerDelegate::GetAvatarsInTeam(int pTeam)
{
	assert(pTeam == 0 || pTeam == 1);
	return mAvatarTeamSets[pTeam];
}



void PushServerDelegate::CreateNpc()
{
	Npc* lNpc = new Npc(this);
	mGameServerManager->GetContext()->AddLocalObject(lNpc);
	mNpcSet.insert(lNpc->GetInstanceId());
	lNpc->StartCreateAvatar(0.1f);

	const str lPlayerName = strutil::Format(_T("NPC %u"), lNpc->GetInstanceId());
	CreateScore(lPlayerName, false);
}

void PushServerDelegate::DeleteNpc()
{
	assert(!mNpcSet.empty());
	AvatarIdSet::iterator x = mNpcSet.begin();
	mLog.Headlinef(_T("Deleting NPC %u."), *x);
	mGameServerManager->GetContext()->PostKillObject(*x);

	const str lPlayerName = strutil::Format(_T("NPC %u"), *x);
	DeleteScore(lPlayerName);

	mNpcSet.erase(x);
}

Npc* PushServerDelegate::GetNpcByAvatar(Cure::GameObjectId pAvatarId) const
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

void PushServerDelegate::CreateScore(const str& pPlayerName, bool pCreatePing)
{
	assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	assert(lScoreInfo);
	new Cure::IntAttribute(lScoreInfo, KILLS + pPlayerName, 0);
	new Cure::IntAttribute(lScoreInfo, DEATHS + pPlayerName, 0);
	if (pCreatePing)
	{
		new Cure::IntAttribute(lScoreInfo, PING + pPlayerName, 0);
	}
}

void PushServerDelegate::DeleteScore(const str& pPlayerName)
{
	assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	assert(lScoreInfo);
	lScoreInfo->DeleteAttribute(KILLS + pPlayerName);
	lScoreInfo->DeleteAttribute(DEATHS + pPlayerName);
	lScoreInfo->DeleteAttribute(PING + pPlayerName);
}

void PushServerDelegate::UpdatePing()
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

void PushServerDelegate::AddPoint(const str& pPrefix, const Cure::ContextObject* pAvatar, int pPoints)
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
	assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	assert(lScoreInfo);
	Cure::IntAttribute* lAttribute = (Cure::IntAttribute*)lScoreInfo->GetAttribute(pPrefix+lPlayerName);
	if (lAttribute)
	{
		lAttribute->SetValue(lAttribute->GetValue() + pPoints);
	}
}

void PushServerDelegate::SetPoints(const str& pPrefix, const Life::Client* pClient, int pPoints)
{
	const str lPlayerName = strutil::Encode(pClient->GetUserConnection()->GetLoginName());
	assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	assert(lScoreInfo);
	Cure::IntAttribute* lAttribute = (Cure::IntAttribute*)lScoreInfo->GetAttribute(pPrefix+lPlayerName);
	assert(lAttribute);
	if (lAttribute)
	{
		lAttribute->SetValue(pPoints);
	}
}

void PushServerDelegate::DrainHealth(Cure::ContextObject* pExplosive, Cure::ContextObject* pAvatar, float pDamage)
{
	Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)pAvatar->GetAttribute(_T("float_health"));
	assert(lHealth);
	const float lPriorHealth = lHealth->GetValue();
	float lRemainingHealth = lPriorHealth - pDamage;
	if (lPriorHealth > 0)
	{
		if (lRemainingHealth < 0)
		{
			lRemainingHealth = 0;
		}
		lHealth->SetValue(lRemainingHealth);
	}
	if (lPriorHealth > 0 && lRemainingHealth <= 0)
	{
		AddPoint(DEATHS, pAvatar, +1);
		if (pExplosive->GetOwnerInstanceId())
		{
			const int lPoints = (pExplosive->GetOwnerInstanceId() == pAvatar->GetInstanceId()) ? -1 : +1;	// Kills oneself?
			AddPoint(KILLS, mGameServerManager->GetContext()->GetObject(pExplosive->GetOwnerInstanceId()), lPoints);
		}
		Die(pAvatar);
	}
}

void PushServerDelegate::Die(Cure::ContextObject* pAvatar)
{
	Explosion::FallApart(mGameServerManager->GetPhysicsManager(), pAvatar);
	mGameServerManager->DeleteContextObjectDelay(pAvatar, 2);
	mGameServerManager->BroadcastNumberMessage(0, true, Cure::MessageNumber::INFO_FALL_APART, pAvatar->GetInstanceId(), 0);
}

bool PushServerDelegate::IsAvatarObject(const Cure::ContextObject* pObject) const
{
	return mAvatarTeamSets[0].find(pObject->GetInstanceId()) != mAvatarTeamSets[0].end() ||
		mAvatarTeamSets[1].find(pObject->GetInstanceId()) != mAvatarTeamSets[1].end();
}

void PushServerDelegate::TickNpcGhosts()
{
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



LOG_CLASS_DEFINE(GAME, PushServerDelegate);



}
