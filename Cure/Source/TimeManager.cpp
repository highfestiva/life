
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/TimeManager.h"
#include <assert.h>
#include "../../Lepra/Include/Math.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



static const int gTimeWrapLimit = 10*60;	// Unit is seconds. Anything small is good enough.



TimeManager::TimeManager():
	mTargetFrameRate(CURE_STANDARD_FRAME_RATE)
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
	mPhysicsFrameCounter = pPhysicsFrameCounter;
	mCurrentFrameTime = 1/(float)mTargetFrameRate;
	mAverageFrameTime = mCurrentFrameTime;
	mPhysicsFrameTime = mAverageFrameTime;
	mPhysicsStepCount = 1;
	mPhysicsFrameWrapLimit = gTimeWrapLimit*mTargetFrameRate;
}

void TimeManager::TickTime()
{
	CURE_RTVAR_GET(mTargetFrameRate, =, Cure::GetSettings(), RTVAR_PHYSICS_FPS, 2);
	CURE_RTVAR_GET(mRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	mPhysicsFrameWrapLimit = gTimeWrapLimit*mTargetFrameRate;

	mCurrentFrameTime = (float)mTime.PopTimeDiff();
	if (mCurrentFrameTime > 1.0)	// Never take longer steps than one second.
	{
		mCurrentFrameTime = 1.0;
	}
	mAbsoluteTime += mCurrentFrameTime;
	mAbsoluteTime = ::fmod(mAbsoluteTime, (float)gTimeWrapLimit);

	mTickTimeModulo += mCurrentFrameTime;

	mPhysicsFrameTime = 1/(float)mTargetFrameRate;
	while (mPhysicsFrameTime*2 < mAverageFrameTime)
	{
		mPhysicsFrameTime *= 2;
	}
	mPhysicsStepCount = (int)::floor(mTickTimeModulo / mPhysicsFrameTime);
	mPhysicsFrameTime *= mRealTimeRatio;

	if (mPhysicsStepCount > 0)
	{
		mAverageFrameTime = Math::Lerp(mAverageFrameTime, mCurrentFrameTime, 0.01f);
	}
}

void TimeManager::TickPhysics()
{
	if (GetAffordedPhysicsStepCount() >= 1)
	{
		const float lThisStepTime = GetAffordedPhysicsTotalTime() / mRealTimeRatio;
		int lTargetStepCount = (int)::floorf(lThisStepTime * mTargetFrameRate);

		mPhysicsFrameCounter = GetCurrentPhysicsFrameAddFrames(lTargetStepCount);
		//mPhysicsFrameCounter += lTargetStepCount;
		mTickTimeModulo -= lThisStepTime;

		if (mPhysicsSpeedAdjustmentFrameCount > 0)
		{
			if (lTargetStepCount > mPhysicsSpeedAdjustmentFrameCount)
			{
				lTargetStepCount = mPhysicsSpeedAdjustmentFrameCount;
			}
			mTickTimeModulo += mPhysicsSpeedAdjustmentTime*lTargetStepCount / mTargetFrameRate;
			mPhysicsSpeedAdjustmentFrameCount -= lTargetStepCount;
		}
		else
		{
			mPhysicsSpeedAdjustmentTime = 0;
		}
	}
}

float TimeManager::GetAbsoluteTime(float pOffset) const
{
	if (pOffset) return ::fmod(mAbsoluteTime+pOffset, (float)gTimeWrapLimit);
	return (mAbsoluteTime);
}

float TimeManager::GetAbsoluteTimeDiff(float pEnd, float pStart)
{
	float lDiff = pEnd - pStart;
	if (lDiff < 0)
	{
		lDiff += gTimeWrapLimit;
	}
	return lDiff;
}

float TimeManager::GetCurrentFrameTime() const
{
	return (mCurrentFrameTime);
}

int TimeManager::GetCurrentPhysicsFrame() const
{
	return mPhysicsFrameCounter;
}

int TimeManager::GetCurrentPhysicsFrameAddFrames(int pFrames) const
{
	return GetPhysicsFrameAddFrames(mPhysicsFrameCounter, pFrames);
}

int TimeManager::GetCurrentPhysicsFrameAddSeconds(float pSeconds) const
{
	assert(pSeconds <= 60);
	return GetCurrentPhysicsFrameAddFrames(ConvertSecondsToPhysicsFrames(pSeconds));
}

int TimeManager::GetPhysicsFrameAddFrames(int pFrameCounter, int pFrames) const
{
	assert(pFrames > -mPhysicsFrameWrapLimit/2 &&
		pFrames < mPhysicsFrameWrapLimit/2);

	int lNewFrame = pFrameCounter + pFrames;
	if (lNewFrame < 0)
	{
		lNewFrame += mPhysicsFrameWrapLimit;
	}
	else if (lNewFrame >= mPhysicsFrameWrapLimit)
	{
		lNewFrame -= mPhysicsFrameWrapLimit;
	}

	assert(lNewFrame >= 0 && lNewFrame < mPhysicsFrameWrapLimit);
	return lNewFrame;
}

int TimeManager::GetCurrentPhysicsFrameDelta(int pStart) const
{
	return GetPhysicsFrameDelta(mPhysicsFrameCounter, pStart);
}

int TimeManager::GetPhysicsFrameDelta(int pEnd, int pStart) const
{
	if (pStart >= mPhysicsFrameWrapLimit || pEnd >= mPhysicsFrameWrapLimit)
	{
		return -1;
	}
	assert(pEnd >= 0);
	assert(pStart >= 0);

	int lDiff = pEnd - pStart;
	if (lDiff > mPhysicsFrameWrapLimit/2)
	{
		lDiff -= mPhysicsFrameWrapLimit;
	}
	else if (lDiff < -mPhysicsFrameWrapLimit/2)
	{
		lDiff += mPhysicsFrameWrapLimit;
	}

	assert(lDiff > -mPhysicsFrameWrapLimit/2 && lDiff < mPhysicsFrameWrapLimit/2);
	return lDiff;
}

float TimeManager::GetNormalFrameTime() const
{
	return mAverageFrameTime * mRealTimeRatio;
}

float TimeManager::GetRealNormalFrameTime() const
{
	return mAverageFrameTime;
}

float TimeManager::GetRealTimeRatio() const
{
	return mRealTimeRatio;
}

void TimeManager::SetCurrentPhysicsFrame(int pPhysicsFrame)
{
	mPhysicsFrameCounter = pPhysicsFrame;
}

int TimeManager::GetAffordedPhysicsStepCount() const
{
	return (mPhysicsStepCount);
}

float TimeManager::GetAffordedPhysicsStepTime() const
{
	return (mPhysicsFrameTime);
}

float TimeManager::GetAffordedPhysicsTotalTime() const
{
	return (GetAffordedPhysicsStepCount() * GetAffordedPhysicsStepTime());
}

int TimeManager::GetDesiredMicroSteps() const
{
	int lMicroSteps;
	CURE_RTVAR_GET(lMicroSteps, =, Cure::GetSettings(), RTVAR_PHYSICS_MICROSTEPS, 1);
	return (mTargetFrameRate * lMicroSteps);
}



void TimeManager::SetPhysicsSpeedAdjustment(float pTime, int pFrameCount)
{
	mPhysicsSpeedAdjustmentTime = pTime;
	mPhysicsSpeedAdjustmentFrameCount = pFrameCount;
}



int TimeManager::ConvertSecondsToPhysicsFrames(float pSeconds) const
{
	return ((int)(pSeconds*mTargetFrameRate));
}

float TimeManager::ConvertPhysicsFramesToSeconds(int pSteps) const
{
	return (pSteps/(float)mTargetFrameRate);
}



LOG_CLASS_DEFINE(PHYSICS, TimeManager);



}
