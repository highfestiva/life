
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/CppContextObject.h"
#include <assert.h>
#include "../../Lepra/Include/DiskFile.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsTrigger.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"



namespace Cure
{



CppContextObject::CppContextObject(ResourceManager* pResourceManager, const str& pClassId):
	ContextObject(pResourceManager, pClassId),
	mClassResource(0),
	mPhysicsResource(0),
	mAllowNetworkLogic(true)
{
}

CppContextObject::~CppContextObject()
{
	ClearPhysics();
	delete (mPhysicsResource);
	mPhysicsResource = 0;
	delete (mClassResource);
	mClassResource = 0;
}



void CppContextObject::SetAllowNetworkLogic(bool pAllow)
{
	mAllowNetworkLogic = pAllow;
}



const TBC::ChunkyClass::Tag* CppContextObject::FindTag(const str& pTagType, int pFloatValueCount, int pStringValueCount, const std::vector<int>& pTriggerIndexArray) const
{
	const TBC::ChunkyClass* lClass = GetClass();
	for (size_t x = 0; x < lClass->GetTagCount(); ++x)
	{
		const TBC::ChunkyClass::Tag& lTag = lClass->GetTag(x);
		if (lTag.mTagName == pTagType &&
			lTag.mFloatValueList.size() == (size_t)pFloatValueCount &&
			lTag.mStringValueList.size() == (size_t)pStringValueCount &&
			lTag.mBodyIndexList.size() == pTriggerIndexArray.size() &&
			std::equal(lTag.mBodyIndexList.begin(), lTag.mBodyIndexList.end(), pTriggerIndexArray.begin()))
		{
			return &lTag;
		}
	}
	return 0;
}



void CppContextObject::StartLoading()
{
	assert(mClassResource == 0);
	mClassResource = new UserClassResource();
	const str lAssetName = _T("Data/")+GetClassId()+_T(".class");	// TODO: move to central source file.
	mClassResource->Load(GetResourceManager(), lAssetName,
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));
}

void CppContextObject::StartLoadingPhysics(const str& pPhysicsName)
{
	assert(mPhysicsResource == 0);
	mPhysicsResource = new UserPhysicsResource();
	const str lAssetName = pPhysicsName+_T(".phys");	// TODO: move to central source file.
	mPhysicsResource->LoadUnique(GetResourceManager(), lAssetName,
		UserPhysicsResource::TypeLoadCallback(this, &CppContextObject::OnLoadPhysics));
}

bool CppContextObject::TryComplete()
{
	if (!mPhysicsResource)
	{
		return (false);
	}

	if (mPhysicsResource->GetLoadState() == RESOURCE_LOAD_COMPLETE)
	{
		if (GetPhysics() && GetPhysics()->GetEngineCount() > 0 && GetManager())
		{
			GetManager()->EnableMicroTickCallback(this);	// Used for engine force applications each micro frame.
		}
		SetLoadResult(true);
		return (true);
	}
	else if (mPhysicsResource->GetLoadState() != RESOURCE_LOAD_IN_PROGRESS)
	{
		SetLoadResult(false);
		return (true);
	}
	return (false);
}



TBC::ChunkyPhysics* CppContextObject::GetPhysics() const
{
	if (mPhysicsResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (mPhysicsResource->GetData());
	}
	return (0);
}

const TBC::ChunkyClass* CppContextObject::GetClass() const
{
	if (mClassResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (mClassResource->GetRamData());
	}
	return (0);
}



void CppContextObject::OnMicroTick(float pFrameTime)
{
	if (mPhysics && GetManager())
	{
		mPhysics->OnMicroTick(GetManager()->GetGameManager()->GetPhysicsManager(), pFrameTime);
	}
}

void CppContextObject::OnAlarm(int /*pAlarmId*/, void* /*pExtraData*/)
{
}

void CppContextObject::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody)
{
	if (!GetAllowNetworkLogic())
	{
		return;
	}

	ContextObject* lChild = (ContextObject*)GetTrigger(pTriggerId);
	if (lChild)
	{
		lChild->OnTrigger(pTriggerId, pBody);
	}
	else
	{
		mLog.Errorf(_T("Physical trigger not configured for logical trigging on %s."), GetClassId().c_str());
	}

	/*
	TODO: put this back when attaching objects to each other is working.
	ContextObject* lObject2 = (ContextObject*)mManager->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListener(pBody2);
	if (mManager->GetGameManager()->IsServer() && lObject2)
	{
		AttachToObject(pBody1, lObject2, pBody2);
	}*/
}



void CppContextObject::OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	(void)pPosition;
	(void)pRelativeVelocity;

	if (!IsAttachedTo((ContextObject*)pOtherObject))
	{
		// TODO: replace by sensible values. Like dividing by mass, for instance.
		//if (pForce.GetLengthSquared() > 100 || pTorque.GetLengthSquared() > 10)
		{
			mManager->GetGameManager()->OnCollision(pForce, pTorque, pPosition, this, (ContextObject*)pOtherObject,
				pOwnBodyId, pOtherBodyId);
		}
	}
}



void CppContextObject::OnLoadClass(UserClassResource* pClassResource)
{
	TBC::ChunkyClass* lClass = pClassResource->GetData();
	if (pClassResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		mLog.Errorf(_T("Could not load class '%s'."), pClassResource->GetName().c_str());
		assert(false);
		return;
	}
	else
	{
		StartLoadingPhysics(lClass->GetPhysicsBaseName());
	}
}

void CppContextObject::OnLoadPhysics(UserPhysicsResource* pPhysicsResource)
{
	if (pPhysicsResource->GetLoadState() != RESOURCE_LOAD_COMPLETE)
	{
		mLog.Errorf(_T("Could not load physics class '%s'."), pPhysicsResource->GetName().c_str());
		assert(false);
		return;
	}

	SetPhysics(pPhysicsResource->GetData());
	if (GetAllowNetworkLogic())
	{
		SetupChildHandlers();
	}
	TryComplete();
}



bool CppContextObject::GetAllowNetworkLogic() const
{
	return mAllowNetworkLogic;
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);



}
