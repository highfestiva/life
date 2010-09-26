
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Props.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/TimeManager.h"



namespace Life
{



Props::Props(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mIsParticle(false)
{
	SetPhysicsTypeOverride(PHYSICS_OVERRIDE_BONES);
}

Props::~Props()
{
}



void Props::StartParticle(const Vector3DF& pStartVelocity)
{
	mIsParticle = true;
	mVelocity = pStartVelocity;
	GetManager()->AddAlarmCallback(this, 5, 2, 0);
}



void Props::DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource)
{
	Parent::DispatchOnLoadMesh(pMeshResource);
	if (mIsParticle && pMeshResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		pMeshResource->GetRamData()->SetIsSimpleObject();
	}
}

void Props::OnPhysicsTick()
{
	if (mIsParticle)
	{
		const float lFrameTime = GetManager()->GetGameManager()->GetTimeManager()->GetNormalFrameTime();
		SetRootPosition(GetPosition() + mVelocity*lFrameTime);
		mVelocity.z -= 9.82f*lFrameTime;
	}

	Parent::Parent::OnPhysicsTick();	// TRICKY: not a vehicle in this sense.
}

void Props::OnAlarm(int pAlarmId, void* /*pExtraData*/)
{
	if (pAlarmId == 5)
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
}


}
