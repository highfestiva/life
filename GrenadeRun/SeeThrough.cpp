
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "SeeThrough.h"
#include "../Cure/Include/CppContextObject.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/CollisionDetector3D.h"
#include "../TBC/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiCppContextObject.h"



namespace GrenadeRun
{



SeeThrough::SeeThrough(Cure::ContextManager* pManager, const Game* pGame):
	Parent(pManager->GetGameManager()->GetResourceManager(), _T("SeeThrough"), pGame->GetUiManager()),
	mGame(pGame),
	mTag(0),
	mOpacity(0)
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
		lTag->mFloatValueList.size() == 8 && lTag->mMeshIndexList.size() == 1 &&
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

	enum FloatValues
	{
		MAX_TRANSPARENCY = 0,
		MAX_OPACITY,
		XO,
		YO,
		ZO,
		XS,
		YS,
		ZS,
	};
	UiCure::CppContextObject* lParent = (UiCure::CppContextObject*)mParent;
	TBC::ChunkyPhysics* lPhysics = mParent->GetPhysics();
	const int lBoneIndex = mTag->mBodyIndexList[0];
	const Vector3DF lBonePosition = lPhysics->GetBoneTransformation(lBoneIndex).GetPosition();
	const Vector3DF lTargetPosition = lTarget->GetPosition();
	const Vector3DF lHalfSize(mTag->mFloatValueList[XS]/2, mTag->mFloatValueList[YS]/2, mTag->mFloatValueList[ZS]/2);
	AABB<float> lAABB;
	lAABB.SetPosition(lBonePosition + Vector3DF(mTag->mFloatValueList[XO], mTag->mFloatValueList[YO], mTag->mFloatValueList[ZO]));
	lAABB.SetSize(lHalfSize);
	if (CollisionDetector3D<float>::IsAABBEnclosingPoint(lAABB, lTargetPosition))
	{
		mOpacity = Math::Lerp(mOpacity, mTag->mFloatValueList[MAX_TRANSPARENCY], 0.05f);
	}
	else
	{
		mOpacity = Math::Lerp(mOpacity, mTag->mFloatValueList[MAX_OPACITY], 0.05f);
		if (mOpacity >= mTag->mFloatValueList[MAX_OPACITY]*0.95f)
		{
			mOpacity = mTag->mFloatValueList[MAX_OPACITY];
		}
	}

	UiCure::UserGeometryReferenceResource* lMesh = lParent->GetMeshResource(mTag->mMeshIndexList[0]);
	if (lMesh)
	{
		const float lAlpha = lMesh->GetRamData()->GetBasicMaterialSettings().mAlpha;
		lMesh->GetRamData()->GetBasicMaterialSettings().mAlpha = mOpacity;
		if ((lAlpha == 1 || mOpacity == 1) && lAlpha != mOpacity)
		{
			lParent->EnablePixelShader(false);
			lParent->UpdateMaterial(mTag->mMeshIndexList[0]);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, SeeThrough);




}
