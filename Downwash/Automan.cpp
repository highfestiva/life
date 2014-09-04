
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Automan.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"



namespace Downwash
{



Automan::Automan(Cure::GameManager* pGame, Cure::GameObjectId pCarId, const vec3& pDirection):
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
	const vec2 lWantedDirection(mDirection.x, mDirection.y);
	const vec3 lCarDirection3d = lCar->GetOrientation()*vec3(0,1,0);
	const vec2 lCarDirection(lCarDirection3d.x, lCarDirection3d.y);
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
