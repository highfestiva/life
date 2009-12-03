
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../../Lepra/Include/Math.h"
#include "../Include/Cure.h"
#include "../Include/RuntimeVariable.h"
#include "../Include/TimeManager.h"



namespace Cure
{



TimeManager::TimeManager():
	mTargetFrameRate(60)
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
	mAverageFrameTime = 1/(float)mTargetFrameRate;
	mReportFrame = pPhysicsFrameCounter;
	mPhysicsFrameTime = mAverageFrameTime;
	mPhysicsStepCount = 1;
}

void TimeManager::TickTime()
{
	mTargetFrameRate = CURE_RTVAR_GET(Cure::GetSettings(), RTVAR_PHYSICS_FPS, 2);

	float lCurrentFrameTime = (float)mTime.PopTimeDiff();
	if (lCurrentFrameTime > 1.0)	// Never take longer steps than one second.
	{
		lCurrentFrameTime = 1.0;
	}
	mAbsoluteTime += lCurrentFrameTime;

	mTickTimeModulo += lCurrentFrameTime;

	mAverageFrameTime = Lepra::Math::Lerp(mAverageFrameTime, lCurrentFrameTime, 0.01f);

	mPhysicsFrameTime = 1/(float)mTargetFrameRate;
	while (mPhysicsFrameTime*2 < mAverageFrameTime)
	{
		mPhysicsFrameTime *= 2;
	}
	mPhysicsStepCount = (int)::floor(mTickTimeModulo/mPhysicsFrameTime);

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
		const float lThisStepTime = GetAffordedPhysicsTotalTime();
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

int TimeManager::GetDesiredPhysicsFps() const
{
	return (mTargetFrameRate);
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
