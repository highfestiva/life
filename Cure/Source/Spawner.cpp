
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/Spawner.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/PhysicsSpawner.h"



namespace Cure
{



Spawner::Spawner(ContextManager* pManager):
	CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("Spawner")),
	mSpawnPointIndex(0)
{
	pManager->AddLocalObject(this);
	pManager->EnableTickCallback(this);
	mManager->AddAlarmCallback(this, 0, 0.5f, 0);	// Create.
	mManager->AddAlarmCallback(this, 1, 0.6f, 0);	// Destroy.
	mManager->AddAlarmCallback(this, 2, 0.7f, 0);	// Recreate.
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



void Spawner::PlaceObject(ContextObject* pObject, int pSpawnPointIndex)
{
	const Vector3DF lScalePoint = RNDPOSVEC();
	if (pSpawnPointIndex < 0)
	{
		pSpawnPointIndex = Random::GetRandomNumber() % GetSpawner()->GetSpawnPointCount();
	}
	Vector3DF lInitialVelocity;
	pObject->SetInitialTransform(GetSpawner()->GetSpawnPoint(mParent->GetPhysics(), lScalePoint, pSpawnPointIndex, lInitialVelocity));
	pObject->SetRootVelocity(lInitialVelocity);
}

TransformationF Spawner::GetSpawnPoint() const
{
	Vector3DF lInitialVelocity;
	return GetSpawner()->GetSpawnPoint(mParent->GetPhysics(), Vector3DF(), 0, lInitialVelocity);
}

void Spawner::EaseDown(TBC::PhysicsManager* pPhysicsManager, ContextObject* pObject, const Vector3DF* pStartPosition)
{
	const Cure::ObjectPositionalData* lPositionalData = 0;
	pObject->UpdateFullPosition(lPositionalData);
	Cure::ObjectPositionalData* lNewPositionalData = (Cure::ObjectPositionalData*)lPositionalData->Clone();
	if (pStartPosition)
	{
		lNewPositionalData->mPosition.mTransformation.SetPosition(*pStartPosition);
	}
	/*lNewPositionalData->mPosition.mAcceleration = Vector3DF();
	lNewPositionalData->mPosition.mVelocity = Vector3DF();
	lNewPositionalData->mPosition.mAngularAcceleration = Vector3DF();
	lNewPositionalData->mPosition.mAngularVelocity = Vector3DF();*/
	bool lHasTouchedGround = false;
	float lStep = 1.0f;
	for (int x = 0; x < 20; ++x)
	{
		pObject->SetFullPosition(*lNewPositionalData, 0);
		const bool lIsColliding = pPhysicsManager->IsColliding(pObject->GetInstanceId());
		//mLog.Infof(_T("%s at step %f"), lIsColliding? _T("Is colliding") : _T("Not colliding"), lStep);
		if (lStep < 0.0001f && lIsColliding)
		{
			break;
		}
		lNewPositionalData->mPosition.mTransformation.GetPosition().z += lIsColliding? +lStep : -lStep;
		lHasTouchedGround |= lIsColliding;
		if (lHasTouchedGround)
		{
			lStep /= 2;
		}
	}
	pObject->SetFullPosition(*lNewPositionalData, 0);
	pObject->SetPositionFinalized();
	delete lNewPositionalData;
}



void Spawner::OnTick()
{
	Parent::OnTick();

	for (GameObjectIdArray::iterator x = mEaseDownObjects.begin(); x != mEaseDownObjects.end(); ++x)
	{
		ContextObject* lObject = GetManager()->GetObject(*x, true);
		if (!lObject)
		{
			mEaseDownObjects.erase(x);
			break;
		}
		if (lObject->IsLoaded())
		{
			EaseDown(GetManager()->GetGameManager()->GetPhysicsManager(), lObject, 0);
			mEaseDownObjects.erase(x);
			break;
		}
	}
}

void Spawner::OnAlarm(int pAlarmId, void* pExtraData)
{
	Parent::OnAlarm(pAlarmId, pExtraData);

	const TBC::PhysicsSpawner::IntervalArray& lIntervals = GetSpawner()->GetIntervals();
	const size_t lIntervalCount = lIntervals.size();
	if (lIntervalCount < 2 || lIntervalCount > 3)
	{
		mLog.AError("Error: spawner has badly configured intervals!");
		deb_assert(false);
		return;
	}

	if (pAlarmId == 0)
	{
		const bool lHasRecreate = (lIntervalCount >= 3 && lIntervals[2]);
		OnCreate(lIntervals[0], lHasRecreate);
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

void Spawner::OnCreate(float pCreateInterval, bool pHasRecreate)
{
	if (!pCreateInterval)
	{
		return;
	}

	if ((int)mChildArray.size() < GetSpawnCount())
	{
		Create();
	}

	if (!pHasRecreate)
	{
		mRecreateTimer.Stop();	// Make sure re-create starts over when we're done.
		GetManager()->AddAlarmCallback(this, 0, pCreateInterval, 0);
	}
	else if (pCreateInterval < 0 && (int)mChildArray.size() < GetSpawnCount())
	{
		mRecreateTimer.Stop();	// Make sure re-create starts over when we're done.
		GetManager()->AddAlarmCallback(this, 0, -pCreateInterval, 0);
	}
}

void Spawner::OnDestroy(float pDestroyInterval)
{
	if (!pDestroyInterval)
	{
		return;
	}

	if (!mChildArray.empty())
	{
		if (pDestroyInterval > 0 || (int)mChildArray.size() == GetSpawnCount())
		{
			const ContextObject* lObject = mChildArray.front();
			mChildArray.erase(mChildArray.begin());
			GetManager()->DeleteObject(lObject->GetInstanceId());
		}
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
		if (mSpawner->IsEaseDown())
		{
			mEaseDownObjects.push_back(lObject->GetInstanceId());
		}
		PlaceObject(lObject, mSpawnPointIndex);
		if (++mSpawnPointIndex >= GetSpawner()->GetSpawnPointCount())
		{
			if (GetSpawner()->GetFunction() == _T("spawner_init"))
			{
				deb_assert(GetSpawner()->GetSpawnPointCount() >= 2);
				mSpawnPointIndex = 1;
			}
			else
			{
				mSpawnPointIndex = 0;
			}
		}
		lObject->StartLoading();
	}
}

int Spawner::GetSpawnCount() const
{
	return (int)GetSpawner()->GetNumber();
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Spawner);




}
