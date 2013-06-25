
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Spawner.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/PhysicsSpawner.h"



namespace Cure
{



Spawner::Spawner(ContextManager* pManager):
	CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("Spawner"))
{
	pManager->AddLocalObject(this);
	pManager->AddAlarmCallback(this, 0, 0.5, 0);	// Create.
	pManager->AddAlarmCallback(this, 1, 0.5, 0);	// Destroy.
	pManager->AddAlarmCallback(this, 2, 0.5, 0);	// Recreate.
}

Spawner::~Spawner()
{
	while (!mChildArray.empty())
	{
		const ContextObject* lObject = mChildArray.front();
		mChildArray.erase(mChildArray.begin());
		GetManager()->DeleteObject(lObject->GetInstanceId());
	}
}



void Spawner::PlaceObject(ContextObject* pObject)
{
	const Vector3DF lScalePoint = RNDPOSVEC();
	pObject->SetInitialTransform(GetSpawner()->GetSpawnPoint(mParent->GetPhysics(), lScalePoint));
}

TransformationF Spawner::GetSpawnPoint() const
{
	return GetSpawner()->GetSpawnPoint(mParent->GetPhysics(), Vector3DF());
}



void Spawner::OnAlarm(int pAlarmId, void* pExtraData)
{
	Parent::OnAlarm(pAlarmId, pExtraData);

	const TBC::PhysicsSpawner::IntervalArray& lIntervals = GetSpawner()->GetIntervals();
	const size_t lIntervalCount = lIntervals.size();
	if (lIntervalCount < 2 || lIntervalCount > 3)
	{
		mLog.AError("Error: spawner has badly configured intervals!");
		assert(false);
		return;
	}

	if (pAlarmId == 0)
	{
		OnCreate(lIntervals[0]);
	}
	else if (pAlarmId == 1)
	{
		OnDestroy(lIntervals[1]);
	}
	else if (lIntervalCount >= 3)
	{
		OnRecreate(lIntervals[2]);
	}
}

void Spawner::OnCreate(float pCreateInterval)
{
	if (!pCreateInterval)
	{
		return;
	}

	if ((int)mChildArray.size() < GetSpawnCount())
	{
		Create();
	}

	GetManager()->AddAlarmCallback(this, 0, pCreateInterval, 0);
}

void Spawner::OnDestroy(float pDestroyInterval)
{
	if (!pDestroyInterval)
	{
		return;
	}

	if (!mChildArray.empty())
	{
		const ContextObject* lObject = mChildArray.front();
		mChildArray.erase(mChildArray.begin());
		GetManager()->DeleteObject(lObject->GetInstanceId());
	}
	GetManager()->AddAlarmCallback(this, 1, pDestroyInterval, 0);
}

void Spawner::OnRecreate(float pRecreateInterval)
{
	if ((int)mChildArray.size() < GetSpawnCount())
	{
		mRecreateTimer.TryStart();
		if (mRecreateTimer.QueryTimeDiff() >= pRecreateInterval)
		{
			mRecreateTimer.Stop();
			Create();
		}
	}
	GetManager()->AddAlarmCallback(this, 2, 0.5f, 0);
}

void Spawner::Create()
{
	const str lSpawnObject = GetSpawner()->GetSpawnObject(Random::Uniform(0.0f, 1.0f));
	if (!lSpawnObject.empty())
	{
		ContextObject* lObject = GetManager()->GetGameManager()->CreateContextObject(lSpawnObject, NETWORK_OBJECT_LOCALLY_CONTROLLED);
		AddChild(lObject);
		PlaceObject(lObject);
		lObject->StartLoading();
	}
}

int Spawner::GetSpawnCount() const
{
	return (int)GetSpawner()->GetNumber();
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Spawner);




}
