
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Automan.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"



namespace HeliForce
{



Automan::Automan(Cure::GameManager* pGame, Cure::GameObjectId pCarId, const Vector3DF& pDirection):
	Parent(pGame->GetResourceManager(), _T("Automan")),
	mCarId(pCarId),
	mDirection(pDirection)
{
	pGame->GetContext()->AddLocalObject(this);
	pGame->GetContext()->EnableTickCallback(this);
}

Automan::~Automan()
{
}



void Automan::OnTick()
{
	Parent::OnTick();

	Cure::ContextObject* lCar = mManager->GetObject(mCarId, true);
	if (!lCar)
	{
		mManager->PostKillObject(GetInstanceId());
		return;
	}
	if (!lCar->IsLoaded())
	{
		return;
	}
	lCar->SetEnginePower(0, 1);
	const Vector2DF lWantedDirection(mDirection.x, mDirection.y);
	const Vector3DF lCarDirection3d = lCar->GetOrientation()*Vector3DF(0,1,0);
	const Vector2DF lCarDirection(lCarDirection3d.x, lCarDirection3d.y);
	const float lAngle = lWantedDirection.GetAngle(lCarDirection);
	lCar->SetEnginePower(1, lAngle);

	if (lCar->GetVelocity().GetLengthSquared() < 1.0f)
	{
		mStillTimer.TryStart();
		if (mStillTimer.QueryTimeDiff() > 4.0f)
		{
			Cure::Health::Set(lCar, 0);
		}
	}
	else
	{
		mStillTimer.Stop();
	}
}



}
