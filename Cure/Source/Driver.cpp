
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/Driver.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"
#include "../Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/PhysicsEngine.h"



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
			deb_assert(mTag.mFloatValueList.size() == 1);
			lEngine->SetValue(lEngine->GetControllerIndex(), (float)::cos(mTime.QueryTimeDiff()*f));
			const double lPeriod = 2*PI/f;
			if (mTime.GetTimeDiff() > lPeriod)
			{
				mTime.ReduceTimeDiff(lPeriod);
			}
		}
		else if (mType == _T("random_jerker"))
		{
			deb_assert(mTag.mFloatValueList.size() == 2);
			const float g = mTag.mFloatValueList[1];
			const float t = (float)mTime.QueryTimeDiff();
			if (t < f)
			{
				lEngine->SetValue(lEngine->GetControllerIndex()+0, 0);
				lEngine->SetValue(lEngine->GetControllerIndex()+1, 0);
				lEngine->SetValue(lEngine->GetControllerIndex()+3, 0);	// 3 for Z... Yup, I know!
			}
			else
			{
				Vector3DF lPush = RNDVEC(1.0f);
				lPush.x = ::pow(std::abs(lPush.x), 0.1f) * ((lPush.x <= 0)? -1 : +1);
				lPush.y = ::pow(std::abs(lPush.y), 0.1f) * ((lPush.y <= 0)? -1 : +1);
				lPush.z = ::pow(std::abs(lPush.z), 0.1f) * ((lPush.z <= 0)? -1 : +1);
				lEngine->SetValue(lEngine->GetControllerIndex()+0, lPush.x);
				lEngine->SetValue(lEngine->GetControllerIndex()+1, lPush.y);
				lEngine->SetValue(lEngine->GetControllerIndex()+3, lPush.z);	// 3 for Z... Yup, I know!
				if (t > f+g)
				{
					mTime.ReduceTimeDiff(f+g);
				}
			}
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Driver);




}
