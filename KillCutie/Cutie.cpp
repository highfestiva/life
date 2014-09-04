
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Cutie.h"
#include "../Lepra/Include/Math.h"
#include "../Tbc/Include/ChunkyBoneGeometry.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "Game.h"



namespace GrenadeRun
{



Cutie::Cutie(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mHealth(1),
	mKillJointsTickCount(2),
	mWheelExpelTickCount(3)
{
	SetForceLoadUnique(true);	// Needs to be unique as physics are modified and reloaded on every death.
}

Cutie::~Cutie()
{
}



void Cutie::DrainHealth(float pDrain)
{
	if (!mHealth)	// Already killed?
	{
		return;
	}
	mHealth = Math::Clamp(mHealth-pDrain, 0.0f, 1.0f);
	if (mHealth)
	{
		return;
	}

	// Was killed. Drop all wheels! :)
	mKillJointsTickCount = 2;
	mWheelExpelTickCount = 3;
	float lRealTimeRatio;
	v_get(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	mWheelExpelTickCount = (int)(mWheelExpelTickCount/lRealTimeRatio);
	if (GetPhysics())
	{
		GetPhysics()->ClearEngines();
	}
	/*Tbc::ChunkyClass* lClass = (Tbc::ChunkyClass*)GetClass();
	size_t lTagCount = lClass->GetTagCount();
	for (size_t x = 0; x < lTagCount;)
	{
		lClass->RemoveTag(x);
		--lTagCount;
	}
	DeleteEngineSounds();*/
}

float Cutie::GetHealth() const
{
	return mHealth;
}

bool Cutie::QueryFlip()
{
	if (!mHealth || !IsUpsideDown())
	{
		return false;
	}

	// Reset vehicle in the direction it was heading.
	const Cure::ObjectPositionalData* lOriginalPositionData;
	if (UpdateFullPosition(lOriginalPositionData))
	{
		Cure::ObjectPositionalData lPositionData;
		lPositionData.CopyData(lOriginalPositionData);
		lPositionData.Stop();
		xform& lTransform = lPositionData.mPosition.mTransformation;
		lTransform.SetPosition(GetPosition() + vec3(0, 0, 1.5f));
		vec3 lEulerAngles;
		GetOrientation().GetEulerAngles(lEulerAngles);
		lTransform.GetOrientation().SetEulerAngles(lEulerAngles.x, 0, 0);
		lTransform.GetOrientation() *= GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		SetFullPosition(lPositionData, 0);
		return true;
	}
	return false;
}

bool Cutie::IsUpsideDown() const
{
	vec3 lUp(0, 0, 1);
	lUp = GetOrientation() * lUp;
	if (lUp.z > 0.4f ||
		GetVelocity().GetLengthSquared() > 0.1f ||
		GetAngularVelocity().GetLengthSquared() > 0.1f)
	{
		// Nope, still standing, or at least moving. Might be drunken style,
		// but at least not on it's head yet.
		return false;
	}
	return true;
}



void Cutie::OnTick()
{
	Parent::OnTick();

	// If killed: check if we should expel wheels! :)
	if (mHealth)
	{
		return;
	}
	if (mWheelExpelTickCount <= 0)
	{
		return;
	}
	--mKillJointsTickCount;
	--mWheelExpelTickCount;
	if (mKillJointsTickCount <= 0)
	{
		mKillJointsTickCount = 0x7FFFFFFF;
		// Remove the joints, but don't allow collisions with body yet.
		const vec3 lPosition = GetPosition();
		const int lBoneCount = GetPhysics()->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			Tbc::ChunkyBoneGeometry* lWheel = GetPhysics()->GetBoneGeometry(x);
			if (lWheel->GetJointType() != Tbc::ChunkyBoneGeometry::JOINT_EXCLUDE)
			{
				GetManager()->GetGameManager()->GetPhysicsManager()->DeleteJoint(lWheel->GetJointId());
				lWheel->ResetJointId();
				// Push the wheel away somewhat, not too much.
				const int lPushFactor = 200;
				const vec3 lWheelPosition = GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lWheel->GetBodyId());
				const vec3 lForce = (lWheelPosition-lPosition).GetNormalized()*lPushFactor*lWheel->GetMass();
				GetManager()->GetGameManager()->GetPhysicsManager()->AddForce(lWheel->GetBodyId(), lForce);
			}
		}
	}
	if (mWheelExpelTickCount > 0)
	{
		return;
	}
	// Allow collisions with body.
	const vec3 lPosition = GetPosition();
	const int lBoneCount = GetPhysics()->GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		Tbc::ChunkyBoneGeometry* lWheel = GetPhysics()->GetBoneGeometry(x);
		if (lWheel->GetJointType() == Tbc::ChunkyBoneGeometry::JOINT_EXCLUDE ||
			!GetManager()->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListenerId(lWheel->GetBodyId()))
		{
			continue;
		}
		vec3 lWheelPosition = GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lWheel->GetBodyId());
		bool lFarAway = (lPosition.GetDistanceSquared(lWheelPosition) >= 5*5);
		if (!lFarAway)
		{
			const quat lCarInverse = GetOrientation().GetInverse();
			lWheelPosition = lCarInverse * (lWheelPosition - lPosition);
			const float lMinDistance = lWheel->GetShapeSize().x * 0.5f;
			if (::fabs(lWheelPosition.x) > ::fabs(GetPhysics()->GetOriginalBoneTransformation(x).GetPosition().x) + lMinDistance)
			{
				lFarAway = true;
			}
		}
		if (lFarAway)
		{
			GetManager()->RemovePhysicsBody(lWheel->GetBodyId());
			GetManager()->GetGameManager()->GetPhysicsManager()->SetForceFeedbackListener(lWheel->GetBodyId(), 0);
		}
		else
		{
			// Come back later for this one.
			mWheelExpelTickCount = 1;
		}
	}
}



loginstance(GAME_CONTEXT_CPP, Cutie);



}
