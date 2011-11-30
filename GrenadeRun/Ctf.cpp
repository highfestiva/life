
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Ctf.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/ContextManager.h"
#include "../TBC/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiCppContextObject.h"



namespace GrenadeRun
{



Ctf::Ctf(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), _T("Ctf")),
	mTrigger(0),
	mLastFrameTriggered(false),
	mIsTriggerTimerStarted(false),
	mFlagMesh(0)
{
	pManager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Ctf::~Ctf()
{
}



Vector3DF Ctf::GetPosition() const
{
	const TBC::ChunkyBoneGeometry* lCutieGoal = mTrigger->GetTriggerGeometry(0);
	return GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lCutieGoal->GetTriggerId());
}

float Ctf::GetCaptureLevel() const
{
	return 1 - (mFlagTop - mFlagMesh->GetOffsetTransformation().GetPosition()).GetLength() / mFlagOffset.GetLength();
}



void Ctf::FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger)
{
	mTrigger = pTrigger;
	UiCure::CppContextObject* lParent = (UiCure::CppContextObject*)mParent;
	const TBC::ChunkyClass::Tag* lTag = lParent->FindTag(_T("stunt_trigger_data"), 4, 0, std::vector<int>());
	assert(lTag && lTag->mMeshIndexList.size() == 1);
	if (lTag && lTag->mMeshIndexList.size() == 1)
	{
		mFlagOffset.x		= lTag->mFloatValueList[0];
		mFlagOffset.y		= lTag->mFloatValueList[1];
		mFlagOffset.z		= lTag->mFloatValueList[2];
		mCatchingFlagVelocity	= -mFlagOffset / lTag->mFloatValueList[3];
	}
}

void Ctf::OnTick()
{
	if (!mLastFrameTriggered)
	{
		mIsTriggerTimerStarted = false;
	}

	UiCure::CppContextObject* lParent = (UiCure::CppContextObject*)mParent;
	if (!mFlagMesh)
	{
		const TBC::ChunkyClass::Tag* lTag = lParent->FindTag(_T("stunt_trigger_data"), 4, 0, std::vector<int>());
		mFlagMesh = (TBC::GeometryReference*)lParent->GetMesh(lTag->mMeshIndexList[0]);
		if (!mFlagMesh)
		{
			return;
		}
		mFlagTop = mFlagMesh->GetOffsetTransformation().GetPosition();
		mFlagMesh->AddOffset(mFlagOffset);
	}
	else
	{
		lParent->EnableMeshSlide(false);
	}

	// Move flag up or down...
	if (mIsTriggerTimerStarted)
	{
		// Move up or stop if reached top.
		mFlagMesh->AddOffset(mCatchingFlagVelocity / 20);
		if ((mFlagTop - mFlagMesh->GetOffsetTransformation().GetPosition()).Dot(mCatchingFlagVelocity) <= 0)
		{
			mCatchingFlagVelocity.Set(0, 0, 0);
			((Game*)GetManager()->GetGameManager())->OnCapture();
		}
	}
	/*else
	{
		// Move down if not at bottom.
		if ((mFlagOffset - (mFlagMesh->GetOffsetTransformation().GetPosition() - mFlagTop)).Dot(mFlagOffset) > 0)
		{
			mFlagMesh->AddOffset(mCatchingFlagVelocity / -60);
		}
	}*/

	mLastFrameTriggered = false;
	mTriggerTimer.UpdateTimer();
}

void Ctf::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener*)
{
	(void)pTriggerId;

	mLastFrameTriggered = true;
	if (!mIsTriggerTimerStarted)
	{
		mTriggerTimer.PopTimeDiff();
		mIsTriggerTimerStarted = true;
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Ctf);




}
