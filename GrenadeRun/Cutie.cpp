
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Cutie.h"
#include "../Lepra/Include/Math.h"
#include "../TBC/Include/ChunkyBoneGeometry.h"
#include "../Cure/Include/ContextManager.h"
#include "Game.h"



namespace GrenadeRun
{



Cutie::Cutie(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mHealth(1),
	mKillJointsTickCount(2),
	mWheelExpelTickCount(3)
{
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
	const float lSpeed = GetVelocity().GetLength();
	if (lSpeed < 1)
	{
		mWheelExpelTickCount = 50;
	}
	else
	{
		mWheelExpelTickCount = 20 + (int)(30/lSpeed);
	}
	GetPhysics()->ClearEngines();
	/*TBC::ChunkyClass* lClass = (TBC::ChunkyClass*)GetClass();
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
		TransformationF& lTransform = lPositionData.mPosition.mTransformation;
		lTransform.SetPosition(GetPosition() + Vector3DF(0, 0, 3));
		Vector3DF lEulerAngles;
		GetOrientation().GetEulerAngles(lEulerAngles);
		lTransform.GetOrientation().SetEulerAngles(lEulerAngles.x, 0, 0);
		lTransform.GetOrientation() *= GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
		SetFullPosition(lPositionData);
		return true;
	}
	return false;
}

bool Cutie::IsUpsideDown() const
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
				const int lPushFactor = 40;
				const Vector3DF lWheelPosition = GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lWheel->GetBodyId());
				GetManager()->GetGameManager()->GetPhysicsManager()->AddForce(lWheel->GetBodyId(), (lWheelPosition-lPosition)*lPushFactor*lWheel->GetMass());
			}
		}
	}
	if (mWheelExpelTickCount > 0)
	{
		return;
	}
	// Allow collisions with body.
	const int lBoneCount = GetPhysics()->GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		TBC::ChunkyBoneGeometry* lWheel = GetPhysics()->GetBoneGeometry(x);
		if (lWheel->GetJointType() != TBC::ChunkyBoneGeometry::JOINT_EXCLUDE)
		{
			GetManager()->RemovePhysicsBody(lWheel->GetBodyId());
			GetManager()->GetGameManager()->GetPhysicsManager()->SetForceFeedbackListener(lWheel->GetBodyId(), 0);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Cutie);



}
