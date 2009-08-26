
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/CppContextObject.h"
#include <assert.h>
#include "../../Lepra/Include/DiskFile.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"



namespace Cure
{



static Lepra::String RemoveMeTranslateToHardcodedShit(CppContextObject* pObject)
{
	if (pObject->GetClassId().find(_T("box_002")) != Lepra::String::npos)
	{
		return (_T("box_01"));
	}
	else if (pObject->GetClassId().find(_T("sphere_002")) != Lepra::String::npos)
	{
		return (_T("sphere_01"));
	}
	else if (pObject->GetClassId().find(_T("car_001")) != Lepra::String::npos)
	{
		return (_T("car_01"));
	}
	else if (pObject->GetClassId().find(_T("monster_001")) != Lepra::String::npos)
	{
		return (_T("monster_01"));
	}
	else if (pObject->GetClassId().find(_T("excavator_703")) != Lepra::String::npos)
	{
		return (_T("excavator_01"));
	}
	else if (pObject->GetClassId().find(_T("crane_whatever")) != Lepra::String::npos)
	{
		return (_T("crane_01"));
	}
	else if (pObject->GetClassId().find(_T("ground_002")) != Lepra::String::npos)
	{
		return (_T("world_01"));
	}
	return (Lepra::EmptyString);
}



CppContextObject::CppContextObject(const Lepra::String& pClassId):
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
	Lepra::String lPhysics;
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
	mClassResource->LoadUnique(GetManager()->GetGameManager()->GetResourceManager(), GetClassId(),
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));
}

void CppContextObject::StartLoadingPhysics(const Lepra::String& pPhysicsName)
{
	assert(mPhysicsResource == 0);
	mPhysicsResource = new UserPhysicsResource();
	const Lepra::String lAssetName = _T("../../Data/")+pPhysicsName+_T(".phys");	// TODO: move to central source file.
	mPhysicsResource->LoadUnique(GetManager()->GetGameManager()->GetResourceManager(), lAssetName,
		UserPhysicsResource::TypeLoadCallback(this, &CppContextObject::OnLoadPhysics));
}

void CppContextObject::TryComplete()
{
	//if (mClassResource->GetLoadState() == RESOURCE_LOAD_COMPLETE &&	TODO: check this!!!
	if (	mPhysicsResource->GetLoadState() == RESOURCE_LOAD_COMPLETE)
	{
		if (GetPhysics() && GetPhysics()->GetPhysicsType() != TBC::ChunkyPhysics::STATIC)
		{
			GetManager()->EnableTickCallback(this);	// TODO: clear out this mess. How to use these two callback types?
		}
		SetLoadResult(true);
	}
	else if (mPhysicsResource->GetLoadState() != RESOURCE_LOAD_IN_PROGRESS)
	{
		SetLoadResult(false);
	}
}



TBC::ChunkyPhysics* CppContextObject::GetPhysics() const
{
	return (mPhysicsResource->GetData());
}



void CppContextObject::OnTick(float pFrameTime)
{
	if (mPhysics)
	{
		mPhysics->OnTick(GetManager()->GetGameManager()->GetPhysicsManager(), pFrameTime);
	}
}

void CppContextObject::OnAlarm(int)// pAlarmId)
{
}

void CppContextObject::OnTrigger(TBC::PhysicsManager::BodyID pBody1, TBC::PhysicsManager::BodyID pBody2)
{
	ContextObject* lObject2 = (ContextObject*)mManager->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListener(pBody2);
	if (mManager->GetGameManager()->IsConnectAuthorized() && lObject2)
	{
		AttachToObject(pBody1, lObject2, pBody2);
	}
}



void CppContextObject::OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
	const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque)
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
		mLog.Errorf(_T("Could not physics class '%s'."), pPhysicsResource->GetName().c_str());
		assert(false);
		return;
	}

	SetPhysics(pPhysicsResource->GetData());
	TryComplete();
}



/*bool CppContextObjectFactory::CreatePhysics(ContextObject* pObject) const
{
	// TODO: go away! Use world loader/spawn engine instead.

	Lepra::String lAssetName;
	if (pObject->GetClassId().find(_T("box_002")) != Lepra::String::npos)
	{
		return (_T("box_01");
	}
	else if (pObject->GetClassId().find(_T("sphere_002")) != Lepra::String::npos)
	{
		return (_T("sphere_01");
	}
	else if (pObject->GetClassId().find(_T("car_001")) != Lepra::String::npos)
	{
		return (_T("car_01");
	}
	else if (pObject->GetClassId().find(_T("monster_001")) != Lepra::String::npos)
	{
		return (_T("monster_01");
	}
	else if (pObject->GetClassId().find(_T("excavator_703")) != Lepra::String::npos)
	{
		return (_T("excavator_01");
	}
	else if (pObject->GetClassId().find(_T("crane_whatever")) != Lepra::String::npos)
	{
		return (_T("crane_01");
	}
	else if (pObject->GetClassId().find(_T("ground_002")) != Lepra::String::npos)
	{
		return (_T("world_01");
	}
	else
	{
		mLog.Error(_T("Unknown context object type."));
		assert(false);
	}

	bool lOk = !lAssetName.empty();
	Lepra::DiskFile lFile;
	if (lOk)
	{
		// TODO: remove path hard-coding.
		lOk = lFile.Open(_T("../../Data/")+lAssetName, Lepra::DiskFile::MODE_READ);
		assert(lOk);
	}
	TBC::ChunkyPhysics* lStructure = 0;
	if (lOk)
	{
		lStructure = new TBC::ChunkyPhysics(TBC::ChunkyPhysics::TRANSFORM_LOCAL2WORLD);
		TBC::ChunkyPhysicsLoader lLoader(&lFile, false);
		lOk = lLoader.Load(lStructure);
		assert(lOk);
	}
	if (lOk)
	{
		lOk = pObject->SetPhysics(lStructure);
		assert(lOk);
	}
	if (lOk && lStructure->GetPhysicsType() != TBC::ChunkyPhysics::STATIC)
	{
		pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);
	}
	if (!lOk && lStructure)
	{
		delete (lStructure);
	}

	return (lOk);
}*/



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);



}
