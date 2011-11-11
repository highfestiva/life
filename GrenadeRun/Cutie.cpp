
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
		mWheelExpelTickCount = 70;
	}
	else
	{
		mWheelExpelTickCount = 30 + (int)(40/lSpeed);
	}
	GetPhysics()->ClearEngines();
	TBC::ChunkyClass* lClass = (TBC::ChunkyClass*)GetClass();
	size_t lTagCount = lClass->GetTagCount();
	for (size_t x = 0; x < lTagCount;)
	{
		/*const str& lTagName = lClass->GetTag(x).mTagName;
		if (lTagName == _T("engine_sound") ||
			lTagName == _T("brake_light") ||
			lTagName == _T("exhaust"))*/
		{
			lClass->RemoveTag(x);
			--lTagCount;
		}
		/*else
		{
			++x;
		}*/
	}
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
		// Remove the joints, but don't allow collisions with body yet.
		mKillJointsTickCount = 10000;
		const int lBoneCount = GetPhysics()->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			TBC::ChunkyBoneGeometry* lGeometry = GetPhysics()->GetBoneGeometry(x);
			if (lGeometry->GetJointType() != TBC::ChunkyBoneGeometry::JOINT_EXCLUDE)
			{
				GetManager()->GetGameManager()->GetPhysicsManager()->DeleteJoint(lGeometry->GetJointId());
				lGeometry->ResetJointId();
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
		TBC::ChunkyBoneGeometry* lGeometry = GetPhysics()->GetBoneGeometry(x);
		if (lGeometry->GetJointType() != TBC::ChunkyBoneGeometry::JOINT_EXCLUDE)
		{
			GetManager()->RemovePhysicsBody(lGeometry->GetBodyId());
			GetManager()->GetGameManager()->GetPhysicsManager()->SetForceFeedbackListener(lGeometry->GetBodyId(), 0);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Cutie);



}
