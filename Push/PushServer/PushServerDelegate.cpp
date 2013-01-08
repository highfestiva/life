
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerDelegate.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Cure/Include/IntAttribute.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "../PushBarrel.h"
#include "../Explosion.h"
#include "../Version.h"
#include "Npc.h"
#include "ServerGrenade.h"

#define KILLS	_T("int_kills:")
#define DEATHS	_T("int_deaths:")



namespace Push
{



PushServerDelegate::PushServerDelegate(Life::GameServerManager* pGameServerManager):
	Parent(pGameServerManager),
	mScoreInfoId(0)
{
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
	CreateScore(lLoginName);

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



void PushServerDelegate::PreEndTick()
{
	TickNpcGhosts();
}



void PushServerDelegate::GetBarrel(Cure::GameObjectId pOwnerId, TransformationF& pTransform, Vector3DF& pVelocity) const
{
	PushBarrel::GetInfo(mGameServerManager, pOwnerId, pTransform, pVelocity);
}

void PushServerDelegate::Shoot(Cure::ContextObject* pAvatar)
{
	ServerGrenade* lGrenade = new ServerGrenade(mGameServerManager->GetResourceManager(), 200, this);
	mGameServerManager->AddContextObject(lGrenade, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	log_volatile(mLog.Debugf(_T("Shooting grenade with ID %i!"), (int)lGrenade->GetInstanceId()));
	lGrenade->SetOwnerId(pAvatar->GetInstanceId());
	TransformationF t(pAvatar->GetOrientation(), pAvatar->GetPosition()+Vector3DF(0, 0, +5.0f));
	lGrenade->SetInitialTransform(t);
	lGrenade->StartLoading();
}

void PushServerDelegate::Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pExplosive, Cure::ContextObject* pHitObject,
	TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pHitBodyId)
{
	(void)pForce;
	(void)pTorque;
	(void)pHitObject;
	(void)pExplosiveBodyId;
	(void)pHitBodyId;

	TBC::PhysicsManager* lPhysicsManager = mGameServerManager->GetPhysicsManager();
	Cure::ContextManager::ContextObjectTable lObjectTable = mGameServerManager->GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		const Cure::ContextObject* lObject = x->second;
		if (!lObject->IsLoaded())
		{
			continue;
		}
		const float lForce = Explosion::PushObject(lPhysicsManager, lObject, pPosition, 1.0f);
		if (lForce > 0 && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			if (IsAvatarObject(lObject))
			{
				Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)lObject->GetAttribute(_T("float_health"));
				assert(lHealth);
				const float lPriorHealth = lHealth->GetValue();
				const float lRemainingHealth = lPriorHealth - lForce*(float)Random::Normal(0.41, 0.05, 0.3, 0.5);
				if (lPriorHealth > 0 && lRemainingHealth > 0)
				{
					lHealth->SetValue(lRemainingHealth);
				}
				else if (lPriorHealth > 0 && lRemainingHealth <= 0)
				{
					AddPoint(DEATHS, lObject, +1);
					ServerGrenade* lGrenade = dynamic_cast<ServerGrenade*>(pExplosive);
					if (lGrenade)
					{
						const int lPoints = (lGrenade->GetOwnerId() == lObject->GetInstanceId()) ? -1 : +1;	// Kills oneself?
						AddPoint(KILLS, mGameServerManager->GetContext()->GetObject(lGrenade->GetOwnerId()), lPoints);
					}
					mGameServerManager->GetContext()->PostKillObject(lObject->GetInstanceId());
				}
			}
			x->second->ForceSend();
		}
	}
}



Cure::ContextObject* PushServerDelegate::CreateAvatarForNpc(Npc* pNpc)
{
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
	Npc* lNpc = new Npc(this, 0.5f);
	mGameServerManager->GetContext()->AddLocalObject(lNpc);
	mNpcSet.insert(lNpc->GetInstanceId());
	Cure::ContextObject* lAvatar = CreateAvatarForNpc(lNpc);
	mLog.Headlinef(_T("Creating NPC %u of type %s."), lNpc->GetInstanceId(), lAvatar->GetClassId().c_str());

	const str lPlayerName = strutil::Format(_T("NPC %u"), lNpc->GetInstanceId());
	CreateScore(lPlayerName);
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

void PushServerDelegate::CreateScore(const str& pPlayerName)
{
	assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	assert(lScoreInfo);
	new Cure::IntAttribute(lScoreInfo, KILLS + pPlayerName, 0);
	new Cure::IntAttribute(lScoreInfo, DEATHS + pPlayerName, 0);
}

void PushServerDelegate::DeleteScore(const str& pPlayerName)
{
	assert(mScoreInfoId);
	Cure::ContextObject* lScoreInfo = mGameServerManager->GetContext()->GetObject(mScoreInfoId);
	assert(lScoreInfo);
	lScoreInfo->DeleteAttribute(KILLS + pPlayerName);
	lScoreInfo->DeleteAttribute(DEATHS + pPlayerName);
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
				if (!lAvatar->QueryResendTime(0.2f, true))
				{
					continue;
				}
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
					lAvatar->GetNetworkOutputGhost()->CopyData(lPositionalData);
					mGameServerManager->BroadcastObjectPosition(lAvatar->GetInstanceId(), *lPositionalData, 0, false);
				}
			}
		}
	}
}



LOG_CLASS_DEFINE(GAME, PushServerDelegate);



}
