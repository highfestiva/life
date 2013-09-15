
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/Driver.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"
#include "../Include/TimeManager.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsTrigger.h"



namespace Cure
{



Driver::Driver(ContextManager* pManager):
	CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("Driver"))
{
	pManager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Driver::~Driver()
{
}



void Driver::SetTagIndex(int pIndex)
{
	mTag = ((CppContextObject*)mParent)->GetClass()->GetTag(pIndex);
	mType = strutil::Split(mTag.mTagName, _T(":"))[1];
}

void Driver::OnTick()
{
	Parent::OnTick();
	if (!mParent->GetPhysics())
	{
		return;
	}

	for (size_t x = 0; x < mTag.mEngineIndexList.size(); ++x)
	{
		const int lEngineIndex = mTag.mEngineIndexList[x];
		if (lEngineIndex >= mParent->GetPhysics()->GetEngineCount())
		{
			break;
		}
		TBC::PhysicsEngine* lEngine = mParent->GetPhysics()->GetEngine(lEngineIndex);

		float f = 1;
		if (mTag.mFloatValueList.size() > 0)
		{
			f = mTag.mFloatValueList[0];
		}
		if (mType == _T("cos"))
		{
			assert(mTag.mFloatValueList.size() == 1);
			lEngine->SetValue(lEngine->GetControllerIndex(), (float)::cos(mTime.QueryTimeDiff()*f));
			const double lPeriod = 2*PI/f;
			if (mTime.GetTimeDiff() > lPeriod)
			{
				mTime.ReduceTimeDiff(lPeriod);
			}
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Driver);




}
