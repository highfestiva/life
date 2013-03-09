
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Spawner.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/PhysicsSpawner.h"
#include "RtVar.h"



namespace Life
{



Spawner::Spawner(Cure::ContextManager* pManager):
	Cure::CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("Spawner"))
{
	pManager->AddLocalObject(this);
	pManager->AddAlarmCallback(this, 0, 0.5, 0);	// Create.
	pManager->AddAlarmCallback(this, 1, 0.5, 0);	// Destroy.
}

Spawner::~Spawner()
{
}



void Spawner::PlaceObject(Cure::ContextObject* pObject)
{
	const Vector3DF lScalePoint = RNDPOSVEC();
	pObject->SetInitialTransform(GetSpawner()->GetSpawnPoint(mParent->GetPhysics(), lScalePoint));
}



void Spawner::OnAlarm(int pAlarmId, void* pExtraData)
{
	Parent::OnAlarm(pAlarmId, pExtraData);

	const TBC::PhysicsSpawner::IntervalArray& lIntervals = GetSpawner()->GetIntervals();
	if (lIntervals.size() != 2)
	{
		mLog.AError("Error: spawner has badly configured intervals!");
		assert(false);
		return;
	}

	if (pAlarmId == 0)
	{
		OnCreate(lIntervals[0]);
	}
	else
	{
		OnDestroy(lIntervals[1]);
	}
}

void Spawner::OnCreate(float pCreateInterval)
{
	if (!pCreateInterval)
	{
		return;
	}

	float lSpawnPart;
	CURE_RTVAR_GET(lSpawnPart, =(float), GetManager()->GetGameManager()->GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	const int lSpawnCount = (int)(GetSpawner()->GetNumber() * lSpawnPart);

	if ((int)mChildArray.size() < lSpawnCount)
	{
		const str lSpawnObject = GetSpawner()->GetSpawnObject(Random::Uniform(0.0f, 1.0f));
		if (!lSpawnObject.empty())
		{
			ContextObject* lObject = GetManager()->GetGameManager()->CreateContextObject(lSpawnObject, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
			AddChild(lObject);
			PlaceObject(lObject);
			lObject->StartLoading();
		}
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



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Spawner);




}
