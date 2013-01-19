
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Npc.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/IntAttribute.h"
#include "../RtVar.h"
#include "GameServerLogic.h"

#define AMMO_VELOCITY_M_PER_S	500.0f	// TODO: remove hard-coding!



namespace Push
{



Npc::Npc(GameServerLogic* pLogic):
	Parent(0, _T("npc")),
	mLogic(pLogic),
	mIntelligence(0.5f),
	mAvatarId(0)
{
	SetLoadResult(true);
}

Npc::~Npc()
{
	if (mAvatarId)
	{
		GetManager()->PostKillObject(mAvatarId);
		mAvatarId = 0;
	}
}



Cure::GameObjectId Npc::GetAvatarId() const
{
	return mAvatarId;
}

void Npc::SetAvatarId(Cure::GameObjectId pAvatarId)
{
	mAvatarId = pAvatarId;
	if (mAvatarId)
	{
		GetManager()->EnableTickCallback(this);
	}
	else
	{
		StartCreateAvatar(10.0f);
	}
}

void Npc::StartCreateAvatar(float pTime)
{
	GetManager()->DisableTickCallback(this);
	GetManager()->AddAlarmCallback(this, 5, pTime, 0);
}



void Npc::OnTick()
{
	Parent::OnTick();

	if (!mAvatarId)
	{
		return;
	}
	Cure::ContextObject* lMyAvatar = GetManager()->GetObject(mAvatarId);
	if (!lMyAvatar)
	{
		return;
	}
	Cure::IntAttribute* lTeam = (Cure::IntAttribute*)lMyAvatar->GetAttribute(_T("int_team"));
	if (!lTeam)
	{
		return;
	}

	const int lOtherTeam = lTeam->GetValue()? 0 : 1;
	const GameServerLogic::AvatarIdSet lAvatarIdSet = mLogic->GetAvatarsInTeam(lOtherTeam);
	Cure::ContextObject* lTarget = 0;
	if (!lAvatarIdSet.empty())
	{
		lTarget = GetManager()->GetObject(*lAvatarIdSet.begin());
	}
	float lFwd = 0;
	float lRight = 0;
	float lPhi = 0;
	bool lCanShoot = false;
	Vector3DF lUp(0, 0, 1);
	lUp = lMyAvatar->GetOrientation() * lUp;
	if (lTarget && lUp.z > 0.2f)	// Just steer+shoot if we're "standing up".
	{
		Vector3DF lDiff = lTarget->GetPosition() - lMyAvatar->GetPosition();
		float lDistance = lDiff.GetLength();
		const float lTimeUntilHit = mIntelligence * lDistance / AMMO_VELOCITY_M_PER_S;
		lDiff = (lTarget->GetPosition() + lTarget->GetVelocity()*lTimeUntilHit) - (lMyAvatar->GetPosition() + lMyAvatar->GetVelocity()*lTimeUntilHit);

		float _;
		lDiff.GetSphericalAngles(_, lPhi);
		lPhi += PIF/2;
		float lYaw;
		lMyAvatar->GetOrientation().GetEulerAngles(lYaw, _, _);
		lYaw -= PIF;
		Math::RangeAngles(lPhi, lYaw);
		lPhi -= lYaw;
		lPhi = -lPhi;
		const float lRotation = lPhi;
		if (std::abs(lPhi) < 0.02f)
		{
			lPhi = 0;
		}
		lPhi *= 4.0f;
		lPhi *= std::abs(lPhi);

		lDistance = lDiff.GetLength();
		if (lDistance > 15 && std::abs(lDiff.z) < 2)	// TODO: implement different shooting pattern for other weapons.
		{
			lCanShoot = true;	// Allow grenade shooting if out of range.
		}
		if (lDistance > 100 && std::abs(lRotation) < 0.4f)
		{
			lFwd = +1.0f;	// Head towards the target if too far away.
		}
		else if (lDistance < 30 && std::abs(lRotation) < 0.4f)
		{
			lFwd = -1.0f;	// Move away if too close.
		}
		if (lRotation != 0)
		{
			lRight = 5.0f * lRotation;	// Strafe if slightly off.
			lRight *= (std::abs(lRotation) < 0.4f)? 1 : -1;
		}
		const float lSteeringPower = std::abs(Math::Clamp(lFwd, -1.0f, +1.0f));
		lPhi *= Math::Lerp(0.8f, 2.0f, lSteeringPower);

		lFwd *= mIntelligence;
		lRight *= mIntelligence;
		lPhi /= mIntelligence;
	}
	lMyAvatar->SetEnginePower(0, lFwd+lPhi);
	lMyAvatar->SetEnginePower(1, lRight);
	lMyAvatar->SetEnginePower(4, lFwd-lPhi);
	lMyAvatar->SetEnginePower(5, lRight);
	lMyAvatar->SetEnginePower(8, +lPhi);

	if (lCanShoot &&
		std::abs(lPhi) < Math::Lerp(0.1f, 0.015f, mIntelligence) &&
		mShootWait.QueryTimeDiff() > Math::Lerp(10.0, 0.3, mIntelligence))
	{
		mShootWait.ClearTimeDiff();
		mLogic->Shoot(lMyAvatar);
	}
}

void Npc::OnAlarm(int pAlarmId, void* pExtraData)
{
	(void)pExtraData;

	if (pAlarmId == 5)
	{
		CURE_RTVAR_GET(mIntelligence, =(float), GetManager()->GetGameManager()->GetVariableScope(), RTVAR_GAME_NPCSKILL, 0.5f);
		if (!mLogic->CreateAvatarForNpc(this))
		{
			StartCreateAvatar(10.0f);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Npc);



}
