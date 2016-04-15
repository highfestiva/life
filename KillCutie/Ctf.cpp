
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Ctf.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Tbc/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiCppContextObject.h"



namespace GrenadeRun
{



Ctf::Ctf(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), "Ctf"),
	mTrigger(0),
	mLastFrameTriggered(false),
	mIsTriggerTimerStarted(false),
	mFlagMesh(0),
	mSlideDown(false),
	mBlinkTime(0)
{
	pManager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Ctf::~Ctf()
{
}



vec3 Ctf::GetPosition() const
{
	const Tbc::ChunkyBoneGeometry* lCutieGoal = mTrigger->GetTriggerGeometry(0);
	return GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lCutieGoal->GetBodyId());
}

float Ctf::GetCaptureLevel() const
{
	return 1 - (mFlagTop - mFlagMesh->GetOffsetTransformation().GetPosition()).GetLength() / mFlagOffset.GetLength();
}

void Ctf::StartSlideDown()
{
	mSlideDown = true;
	mCatchingFlagVelocity = mStartFlagVelocity;
}


void Ctf::FinalizeTrigger(const Tbc::PhysicsTrigger* pTrigger)
{
	mTrigger = pTrigger;
	UiCure::CppContextObject* lParent = (UiCure::CppContextObject*)mParent;
	const Tbc::ChunkyClass::Tag* lTag = lParent->FindTag("stunt_trigger_data", 4, 0);
	deb_assert(lTag && lTag->mMeshIndexList.size() == 2);
	if (lTag && lTag->mMeshIndexList.size() == 2)
	{
		mFlagOffset.x		= lTag->mFloatValueList[0];
		mFlagOffset.y		= lTag->mFloatValueList[1];
		mFlagOffset.z		= lTag->mFloatValueList[2];
		mCatchingFlagVelocity	= -mFlagOffset / lTag->mFloatValueList[3];
		mStartFlagVelocity	= mCatchingFlagVelocity;
	}
}

void Ctf::OnTick()
{
	if (!mLastFrameTriggered)
	{
		mIsTriggerTimerStarted = false;
	}

	UiCure::CppContextObject* lParent = (UiCure::CppContextObject*)mParent;
	if (!mFlagMesh || !mBlinkMesh)
	{
		const Tbc::ChunkyClass::Tag* lTag = lParent->FindTag("stunt_trigger_data", 4, 0);
		mFlagMesh = (Tbc::GeometryReference*)lParent->GetMesh(lTag->mMeshIndexList[0]);
		mBlinkMesh = (Tbc::GeometryReference*)lParent->GetMesh(lTag->mMeshIndexList[1]);
		if (!mFlagMesh || !mBlinkMesh)
		{
			return;
		}
		mFlagTop = mFlagMesh->GetOffsetTransformation().GetPosition();
		mFlagMesh->AddOffset(mFlagOffset);
		mBlinkStartColor = mBlinkMesh->GetBasicMaterialSettings().mDiffuse;
	}
	//else
	//{
	//	lParent->EnableMeshSlide(false);
	//}

	// Move flag up or down...
	Game* lGame = (Game*)GetManager()->GetGameManager();
	float lFactor = 1.0f / FPS;
	if (mIsTriggerTimerStarted && !mSlideDown)
	{
		// Move up or stop if reached top.
		if (lGame->GetComputerIndex() == 0)
		{
			const float t = lGame->GetComputerDifficulty();
			if (t >= 0 && t < 0.5f)
			{
				lFactor *= Math::Lerp(0.4f, 1.0f, t*2);
			}
		}
		float lRealTimeRatio;
		v_get(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
		lFactor *= lRealTimeRatio;
		mFlagMesh->AddOffset(mCatchingFlagVelocity * lFactor);
		if ((mFlagTop - mFlagMesh->GetOffsetTransformation().GetPosition()).Dot(mCatchingFlagVelocity) <= 0)
		{
			mCatchingFlagVelocity.Set(0, 0, 0);
			lGame->OnCapture();
		}
		mBlinkTime += lRealTimeRatio * 0.05f;
		const float r = -::cos(mBlinkTime*3)*0.5f + 0.5f;
		const float g = -::cos(mBlinkTime*4)*0.5f + 0.5f;
		const float b = -::cos(mBlinkTime*5)*0.5f + 0.5f;
		mBlinkMesh->GetBasicMaterialSettings().mDiffuse.Set(r, g, b);
	}
	else
	{
		mBlinkMesh->GetBasicMaterialSettings().mDiffuse = mBlinkStartColor;
		mBlinkTime = 0;
		// Move down if not at bottom.
		if (mSlideDown && (mFlagOffset - (mFlagMesh->GetOffsetTransformation().GetPosition() - mFlagTop)).Dot(mFlagOffset) > 0)
		{
			mFlagMesh->AddOffset(mCatchingFlagVelocity * -lFactor);
		}
		else
		{
			mSlideDown = false;
		}
	}

	mLastFrameTriggered = false;
	mTriggerTimer.UpdateTimer();
}

void Ctf::OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal)
{
	(void)pTriggerId;
	(void)pOtherObject;
	(void)pBodyId;
	(void)pNormal;

	mLastFrameTriggered = true;
	if (!mIsTriggerTimerStarted)
	{
		mTriggerTimer.PopTimeDiff();
		mIsTriggerTimerStarted = true;
	}
}



loginstance(GAME_CONTEXT_CPP, Ctf);




}
