
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../../Lepra/Include/Math.h"
#include "../Include/TimeManager.h"



namespace Cure
{



TimeManager::TimeManager(int pFixedFrameRate):
	mFixedFrameRate(pFixedFrameRate)
{
	Clear(0);
}

TimeManager::~TimeManager()
{
}



void TimeManager::Clear(int pPhysicsFrameCounter)
{
	mTime.PopTimeDiff();
	mTickTimeModulo = 0;
	mPhysicsSpeedAdjustmentTime = 0;
	mPhysicsSpeedAdjustmentFrameCount = 0;
	mAbsoluteTime = 0;
	mCurrentFrameTime = 0;
	mPhysicsFrameCounter = pPhysicsFrameCounter;
	mAverageFrameTime = 1/60.0f;
}

void TimeManager::TickTime()
{
	mCurrentFrameTime = (float)mTime.PopTimeDiff();
	if (mCurrentFrameTime > 1.0)	// Never take longer steps than one second.
	{
		mCurrentFrameTime = 1.0;
	}
	mAbsoluteTime += mCurrentFrameTime;

	mTickTimeModulo += mCurrentFrameTime;

	mAverageFrameTime = Lepra::Math::Lerp(mAverageFrameTime, mCurrentFrameTime, 0.1f);

	mCurrentPhysicsStepCount = (int)(mTickTimeModulo*mFixedFrameRate);
}

void TimeManager::TickPhysics()
{
	if (mCurrentPhysicsStepCount >= 1)
	{
		mPhysicsFrameCounter += mCurrentPhysicsStepCount;

		const float lThisStepTime = mCurrentPhysicsStepCount/(float)mFixedFrameRate;
		mTickTimeModulo -= lThisStepTime;

		mTickTimeModulo += mPhysicsSpeedAdjustmentTime/mFixedFrameRate;
		if (mPhysicsSpeedAdjustmentFrameCount > 0)
		{
			mPhysicsSpeedAdjustmentFrameCount -= mCurrentPhysicsStepCount;
		}
		else
		{
			mPhysicsSpeedAdjustmentTime = 0;
		}
	}
}

float TimeManager::GetAbsoluteTime() const
{
	return (mAbsoluteTime);
}

float TimeManager::GetCurrentFrameTime() const
{
	return (mCurrentFrameTime);
}

int TimeManager::GetCurrentPhysicsFrame() const
{
	return (mPhysicsFrameCounter);
}

float TimeManager::GetNormalFrameTime() const
{
	return (mAverageFrameTime);
}

void TimeManager::SetCurrentPhysicsFrame(int pPhysicsFrame)
{
	mPhysicsFrameCounter = pPhysicsFrame;
}

int TimeManager::GetCurrentPhysicsStepCount() const
{
	return (mCurrentPhysicsStepCount);
}

int TimeManager::GetAffordedPhysicsStepCount() const
{
	int lMinStepCount = (int)::floorf(1/mAverageFrameTime*mFixedFrameRate);
	if (lMinStepCount > mCurrentPhysicsStepCount)
	{
		lMinStepCount = mCurrentPhysicsStepCount;
	}
	if (lMinStepCount < 1)
	{
		lMinStepCount = 1;
	}
	return (lMinStepCount);
}

float TimeManager::GetAffordedStepPeriod() const
{
	return ((GetCurrentPhysicsStepCount()/(float)GetAffordedPhysicsStepCount())/(float)mFixedFrameRate);
	//return (1/(float)mFixedFrameRate);
}

int TimeManager::GetDesiredPhysicsFps() const
{
	return (mFixedFrameRate);
}



void TimeManager::SetPhysicsSpeedAdjustment(float pTime, int pFrameCount)
{
	mPhysicsSpeedAdjustmentTime = pTime;
	mPhysicsSpeedAdjustmentFrameCount = pFrameCount;
}



int TimeManager::ConvertSecondsToPhysicsFrames(float pSeconds) const
{
	return ((int)(pSeconds*mFixedFrameRate));
}

float TimeManager::ConvertPhysicsFramesToSeconds(int pSteps) const
{
	return (pSteps/(float)mFixedFrameRate);
}



}
