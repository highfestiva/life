
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Lepra/Include/Math.h"
#include "../Include/Cure.h"
#include "../Include/RuntimeVariable.h"
#include "../Include/TimeManager.h"



namespace Cure
{



TimeManager::TimeManager():
	mTargetFrameRate(CURE_RTVAR_SLOW_GET(Cure::GetSettings(), RTVAR_PHYSICS_FPS, 60))
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
	mReportFrame = pPhysicsFrameCounter;
	mPhysicsFrameTime = mAverageFrameTime;
	mPhysicsStepCount = 1;
}

void TimeManager::TickTime()
{
	CURE_RTVAR_GET(mTargetFrameRate, =, Cure::GetSettings(), RTVAR_PHYSICS_FPS, 2);
	CURE_RTVAR_GET(mRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	mRealTimeRatio = Math::Clamp(mRealTimeRatio, 0.1f, 350.0f);

	mCurrentFrameTime = (float)mTime.PopTimeDiff();
	if (mCurrentFrameTime > 1.0)	// Never take longer steps than one second.
	{
		mCurrentFrameTime = 1.0;
	}
	mAbsoluteTime += mCurrentFrameTime;

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

	const int lReportInterval = 5;	// Printout ever x seconds.
	if (mPhysicsFrameCounter > mReportFrame || mPhysicsFrameCounter < mReportFrame-mTargetFrameRate*lReportInterval)
	{
		mReportFrame = mPhysicsFrameCounter + mTargetFrameRate*lReportInterval;
		log_volatile(mLog.Debugf(_T("Time step. Target fps: %i, avg fps: %.1f,\n")
			_T("afforded phys steps: %i, afforded phys step time %.1f %%."), 
			mTargetFrameRate, 1/mAverageFrameTime, GetAffordedPhysicsStepCount(), GetAffordedPhysicsStepTime()*mTargetFrameRate*100));
	}
}

void TimeManager::TickPhysics()
{
	if (GetAffordedPhysicsStepCount() >= 1)
	{
		const float lThisStepTime = GetAffordedPhysicsTotalTime() / mRealTimeRatio;
		int lTargetStepCount = (int)::floorf(lThisStepTime * mTargetFrameRate);

		mPhysicsFrameCounter += lTargetStepCount;
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
	return mPhysicsFrameCounter;
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
