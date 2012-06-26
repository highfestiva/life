
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Vehicle.h"
#include "../Lepra/Include/Math.h"
#include "../TBC/Include/ChunkyBoneGeometry.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "Game.h"



namespace TireFire
{



Vehicle::Vehicle(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mHealth(1),
	mKillJointsTickCount(2),
	mWheelExpelTickCount(3)
{
}

Vehicle::~Vehicle()
{
}



void Vehicle::DrainHealth(float pDrain)
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
	CURE_RTVAR_GET(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	mWheelExpelTickCount = (int)(mWheelExpelTickCount/lRealTimeRatio);
	if (GetPhysics())
	{
		GetPhysics()->ClearEngines();
	}
	/*TBC::ChunkyClass* lClass = (TBC::ChunkyClass*)GetClass();
	size_t lTagCount = lClass->GetTagCount();
	for (size_t x = 0; x < lTagCount;)
	{
		lClass->RemoveTag(x);
		--lTagCount;
	}
	DeleteEngineSounds();*/
}

float Vehicle::GetHealth() const
{
	return mHealth;
}

bool Vehicle::QueryFlip()
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
		TransformationF& lTransform = lPositionData.mPosition.mTransformation;
		lTransform.SetPosition(GetPosition() + Vector3DF(0, 0, 1.5f));
		Vector3DF lEulerAngles;
		GetOrientation().GetEulerAngles(lEulerAngles);
		lTransform.GetOrientation().SetEulerAngles(lEulerAngles.x, 0, 0);
		lTransform.GetOrientation() *= GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		SetFullPosition(lPositionData);
		return true;
	}
	return false;
}

bool Vehicle::IsUpsideDown() const
{
	Vector3DF lUp(0, 0, 1);
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



void Vehicle::OnTick()
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
		const Vector3DF lPosition = GetPosition();
		const int lBoneCount = GetPhysics()->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			TBC::ChunkyBoneGeometry* lWheel = GetPhysics()->GetBoneGeometry(x);
			if (lWheel->GetJointType() != TBC::ChunkyBoneGeometry::JOINT_EXCLUDE)
			{
				GetManager()->GetGameManager()->GetPhysicsManager()->DeleteJoint(lWheel->GetJointId());
				lWheel->ResetJointId();
				// Push the wheel away somewhat, not too much.
				const int lPushFactor = 200;
				const Vector3DF lWheelPosition = GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lWheel->GetBodyId());
				const Vector3DF lForce = (lWheelPosition-lPosition).GetNormalized()*lPushFactor*lWheel->GetMass();
				GetManager()->GetGameManager()->GetPhysicsManager()->AddForce(lWheel->GetBodyId(), lForce);
			}
		}
	}
	if (mWheelExpelTickCount > 0)
	{
		return;
	}
	// Allow collisions with body.
	const Vector3DF lPosition = GetPosition();
	const int lBoneCount = GetPhysics()->GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		TBC::ChunkyBoneGeometry* lWheel = GetPhysics()->GetBoneGeometry(x);
		if (lWheel->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_EXCLUDE ||
			!GetManager()->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListener(lWheel->GetBodyId()))
		{
			continue;
		}
		Vector3DF lWheelPosition = GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lWheel->GetBodyId());
		bool lFarAway = (lPosition.GetDistanceSquared(lWheelPosition) >= 5*5);
		if (!lFarAway)
		{
			const QuaternionF lCarInverse = GetOrientation().GetInverse();
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



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Vehicle);



}
