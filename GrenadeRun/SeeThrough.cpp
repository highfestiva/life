
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "SeeThrough.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/ContextManager.h"
#include "../TBC/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiCppContextObject.h"



namespace GrenadeRun
{



SeeThrough::SeeThrough(Cure::ContextManager* pManager, const Game* pGame):
	Parent(pManager->GetGameManager()->GetResourceManager(), _T("SeeThrough")),
	mGame(pGame),
	mTag(0)
{
	pManager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

SeeThrough::~SeeThrough()
{
}



void SeeThrough::SetTagIndex(int pIndex)
{
	Cure::CppContextObject* lParent = (Cure::CppContextObject*)mParent;
	const Tag* lTag = &lParent->GetClass()->GetTag(pIndex);
	if (lTag->mBodyIndexList.size() == 1 && lTag->mEngineIndexList.size() == 0 &&
		lTag->mFloatValueList.size() == 4 && lTag->mMeshIndexList.size() == 1 &&
		lTag->mStringValueList.size() == 0)
	{
		mTag = lTag;
	}
	else
	{
		mLog.AError("Badly configured see-through tag!");
		assert(false);
	}
}

void SeeThrough::OnTick()
{
	assert(mTag);
	if (!mTag)
	{
		return;
	}
	const Cure::ContextObject* lTarget = mGame->GetP1();
	if (!lTarget || !lTarget->IsLoaded())
	{
		return;
	}

	UiCure::CppContextObject* lParent = (UiCure::CppContextObject*)mParent;
	TBC::ChunkyPhysics* lPhysics = mParent->GetPhysics();
	const int lBoneIndex = mTag->mBodyIndexList[0];
	const Vector3DF lPosition = lPhysics->GetBoneTransformation(lBoneIndex).GetPosition();
	if (lPosition.GetDistanceSquared(lTarget->GetPosition()) < mTag->mFloatValueList[2]*mTag->mFloatValueList[2])
	{
		TBC::GeometryBase* lMesh = lParent->GetMesh(mTag->mMeshIndexList[0]);
		if (lMesh)
		{
			lMesh->GetBasicMaterialSettings().mAlpha = mTag->mFloatValueList[0];
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, SeeThrough);




}
