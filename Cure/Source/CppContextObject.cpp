
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/CppContextObject.h"
#include <assert.h>
#include "../../Lepra/Include/DiskFile.h"
#include "../../TBC/Include/ChunkyStructure.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"



namespace Cure
{



CppContextObject::CppContextObject(const Lepra::String& pClassId):
	ContextObject(pClassId)
{
}

CppContextObject::~CppContextObject()
{
}



void CppContextObject::OnTick(float pFrameTime)
{
	if (mStructure)
	{
		mStructure->OnTick(GetManager()->GetGameManager()->GetPhysicsManager(), pFrameTime);
	}
}

void CppContextObject::OnAlarm(int)// pAlarmId)
{
}

void CppContextObject::OnTrigger(TBC::PhysicsEngine::BodyID pBody1, TBC::PhysicsEngine::BodyID pBody2)
{
	ContextObject* lObject2 = (ContextObject*)mManager->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListener(pBody2);
	if (mManager->GetGameManager()->IsConnectAuthorized() && lObject2)
	{
		AttachToObject(pBody1, lObject2, pBody2);
	}
}



void CppContextObject::OnForceApplied(TBC::PhysicsEngine::ForceFeedbackListener* pOtherObject,
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



CppContextObjectFactory::CppContextObjectFactory()
{
}

CppContextObjectFactory::~CppContextObjectFactory()
{
}

ContextObject* CppContextObjectFactory::Create(const Lepra::String& pClassId) const
{
	return (new CppContextObject(pClassId));
}

bool CppContextObjectFactory::CreatePhysics(ContextObject* pObject) const
{
	// TODO: go away! Use world loader/spawn engine instead.

	Lepra::String lAssetName;
	if (pObject->GetClassId().find(_T("box_002")) != Lepra::String::npos)
	{
		lAssetName = _T("box_01.str");
	}
	else if (pObject->GetClassId().find(_T("sphere_002")) != Lepra::String::npos)
	{
		lAssetName = _T("sphere_01.str");
	}
	else if (pObject->GetClassId().find(_T("car_001")) != Lepra::String::npos)
	{
		lAssetName = _T("car_01.str");
	}
	else if (pObject->GetClassId().find(_T("monster_001")) != Lepra::String::npos)
	{
		lAssetName = _T("monster_01.str");
	}
	else if (pObject->GetClassId().find(_T("excavator_703")) != Lepra::String::npos)
	{
		lAssetName = _T("excavator_01.str");
	}
	else if (pObject->GetClassId().find(_T("crane_whatever")) != Lepra::String::npos)
	{
		lAssetName = _T("crane_01.str");
	}
	else if (pObject->GetClassId().find(_T("ground_002")) != Lepra::String::npos)
	{
		lAssetName = _T("world_01.str");
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
	TBC::ChunkyStructure* lStructure = 0;
	if (lOk)
	{
		lStructure = new TBC::ChunkyStructure(TBC::BoneHierarchy::TRANSFORM_LOCAL2WORLD);
		TBC::ChunkyStructureLoader lLoader(&lFile, false);
		lOk = lLoader.Load(lStructure);
		assert(lOk);
	}
	if (lOk)
	{
		lOk = pObject->SetStructure(lStructure);
		assert(lOk);
	}
	if (lOk && lStructure->GetPhysicsType() != TBC::ChunkyStructure::STATIC)
	{
		pObject->GetManager()->EnablePhysicsUpdateCallback(pObject);
	}
	if (!lOk && lStructure)
	{
		delete (lStructure);
	}

	return (lOk);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);
LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObjectFactory);



}
