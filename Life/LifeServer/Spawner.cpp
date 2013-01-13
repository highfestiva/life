
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
	float lSpawnPart;
	CURE_RTVAR_GET(lSpawnPart, =(float), GetManager()->GetGameManager()->GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	const int lSpawnCount = (int)(GetSpawner()->GetNumber() * lSpawnPart);

	if ((int)mChildList.size() < lSpawnCount)
	{
		const str lSpawnObject = GetSpawner()->GetSpawnObject((float)Random::Uniform(0, 1));
		if (!lSpawnObject.empty())
		{
			ContextObject* lObject = GetManager()->GetGameManager()->CreateContextObject(lSpawnObject, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
			AddChild(lObject);
			const Vector3DF lScalePoint((float)Random::Uniform(0, 1), (float)Random::Uniform(0, 1), (float)Random::Uniform(0, 1));
			lObject->SetInitialTransform(GetSpawner()->GetSpawnPoint(mParent->GetPhysics(), lScalePoint));
			lObject->StartLoading();
		}
	}

	GetManager()->AddAlarmCallback(this, 0, pCreateInterval, 0);
}

void Spawner::OnDestroy(float pDestroyInterval)
{
	if (pDestroyInterval)
	{
		if (!mChildList.empty())
		{
			const ContextObject* lObject = mChildList.front();
			mChildList.pop_front();
			GetManager()->DeleteObject(lObject->GetInstanceId());
		}
		GetManager()->AddAlarmCallback(this, 1, pDestroyInterval, 0);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Spawner);




}
