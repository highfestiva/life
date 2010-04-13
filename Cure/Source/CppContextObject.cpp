
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



static str RemoveMeTranslateToHardcodedShit(CppContextObject* pObject)
{
	if (pObject->GetClassId().find(_T("box_002")) != str::npos)
	{
		return (_T("box_01"));
	}
	else if (pObject->GetClassId().find(_T("car_001")) != str::npos)
	{
		return (_T("car_01"));
	}
	else if (pObject->GetClassId().find(_T("monster_001")) != str::npos)
	{
		return (_T("monster_01"));
	}
	else if (pObject->GetClassId().find(_T("excavator_703")) != str::npos)
	{
		return (_T("excavator_01"));
	}
	else if (pObject->GetClassId().find(_T("crane_whatever")) != str::npos)
	{
		return (_T("crane_01"));
	}
	else if (pObject->GetClassId().find(_T("ground_002")) != str::npos)
	{
		return (_T("world_01"));
	}
	return (EmptyString);
}



CppContextObject::CppContextObject(const str& pClassId):
	ContextObject(pClassId),
	mClassResource(0),
	mPhysicsResource(0)
{
}

CppContextObject::~CppContextObject()
{
	delete (mPhysicsResource);
	mPhysicsResource = 0;
	delete (mClassResource);
	mClassResource = 0;
}



void CppContextObject::__StartLoadingFuckedUpPhysicsRemoveMe(Cure::UserResource* pClassResource, const TBC::ChunkyClass* pClass)
{
	str lPhysics;
	if (pClassResource->GetLoadState() != RESOURCE_LOAD_COMPLETE)
	{
		lPhysics = RemoveMeTranslateToHardcodedShit(this);
		if (lPhysics.empty())
		{
			mLog.Errorf(_T("Could not load class '%s'."), pClassResource->GetName().c_str());
			assert(false);
			return;
		}
	}
	else
	{
		lPhysics = pClass->GetPhysicsBaseName();
	}

	StartLoadingPhysics(lPhysics);
}



void CppContextObject::StartLoading()
{
	assert(mClassResource == 0);
	mClassResource = new UserClassResource();
	const str lAssetName = _T("Data/")+GetClassId()+_T(".class");	// TODO: move to central source file.
	mClassResource->Load(GetManager()->GetGameManager()->GetResourceManager(), lAssetName,
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));
}

void CppContextObject::StartLoadingPhysics(const str& pPhysicsName)
{
	assert(mPhysicsResource == 0);
	mPhysicsResource = new UserPhysicsResource();
	const str lAssetName = pPhysicsName+_T(".phys");	// TODO: move to central source file.
	mPhysicsResource->LoadUnique(GetManager()->GetGameManager()->GetResourceManager(), lAssetName,
		UserPhysicsResource::TypeLoadCallback(this, &CppContextObject::OnLoadPhysics));
}

bool CppContextObject::TryComplete()
{
	if (mPhysicsResource->GetLoadState() == RESOURCE_LOAD_COMPLETE)
	{
		if (GetPhysics())
		{
			GetManager()->EnableTickCallback(this);	// TODO: clear out this mess. How to use these two callback types?
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



void CppContextObject::OnTick(float pFrameTime)
{
	if (mPhysics)
	{
		mPhysics->OnTick(GetManager()->GetGameManager()->GetPhysicsManager(), pFrameTime);
	}
}

void CppContextObject::OnAlarm(int /*pAlarmId*/, void* /*pExtraData*/)
{
}

void CppContextObject::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody)
{
	ContextObject* lChild = (ContextObject*)GetTrigger(pTriggerId);
	assert(lChild);
	lChild->OnTrigger(pTriggerId, pBody);

	/*
	TODO: put this back when attaching objects to each other is working.
	ContextObject* lObject2 = (ContextObject*)mManager->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListener(pBody2);
	if (mManager->GetGameManager()->IsConnectAuthorized() && lObject2)
	{
		AttachToObject(pBody1, lObject2, pBody2);
	}*/
}



void CppContextObject::OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
	const Vector3DF& pForce, const Vector3DF& pTorque)
{
	if (!IsAttachedTo((ContextObject*)pOtherObject))
	{
		// TODO: replace by sensible values. Like dividing by mass, for instance.
		//if (pForce.GetLengthSquared() > 100 || pTorque.GetLengthSquared() > 10)
		{
			mManager->GetGameManager()->OnCollision(pForce, pTorque, this, (ContextObject*)pOtherObject);
		}
	}
}



void CppContextObject::OnLoadClass(UserClassResource* pClassResource)
{
	__StartLoadingFuckedUpPhysicsRemoveMe(pClassResource, pClassResource->GetData());
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
	SetupChildTriggerHandlers();
	TryComplete();
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);



}
