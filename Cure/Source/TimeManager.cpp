
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/TimeManager.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/GameTimer.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Performance.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



static const int gTimeWrapLimit = 10*60*60;	// Unit is seconds. Anything small is good enough.
static const int gPhysicsFrameWrapLimit = gTimeWrapLimit*CURE_STANDARD_FRAME_RATE;



TimeManager::TimeManager():
	mTargetFrameRate(CURE_STANDARD_FRAME_RATE),
	mTime(false)
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
	mTickTimeOverhead = 0;
	mRealTimeRatio = 1.0;
	mGameRealTimeRatio = 1.0f;
	mPhysicsFrameCounter = pPhysicsFrameCounter;
	mCurrentFrameTime = 1/(float)mTargetFrameRate;
	mAverageFrameTime = mCurrentFrameTime;
	mPhysicsFrameTime = mAverageFrameTime;
	mPhysicsStepCount = 1;
}

void TimeManager::Tick()
{
	CURE_RTVAR_GET(mTargetFrameRate, =, Cure::GetSettings(), RTVAR_PHYSICS_FPS, 2);
	CURE_RTVAR_GET(mRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	bool lPhysicsHalt;
	CURE_RTVAR_GET(lPhysicsHalt, =, Cure::GetSettings(), RTVAR_PHYSICS_HALT, false);
	mGameRealTimeRatio = lPhysicsHalt? 0 : mRealTimeRatio;
	GameTimer::SetRealTimeRatio(mGameRealTimeRatio);
	bool lIsFixedFps;
	CURE_RTVAR_GET(lIsFixedFps, =, Cure::GetSettings(), RTVAR_PHYSICS_ISFIXEDFPS, false);

	mCurrentFrameTime = (float)mTime.PopTimeDiff();
	if (mCurrentFrameTime > 1.0)	// Never take longer steps than one second.
	{
		mCurrentFrameTime = 1.0;
	}
	mAbsoluteTime += mCurrentFrameTime;
	mAbsoluteTime = ::fmod(mAbsoluteTime, (float)gTimeWrapLimit);

	if (lIsFixedFps)
	{
		mPhysicsFrameTime = mRealTimeRatio/mTargetFrameRate;
		mPhysicsFrameCounter = GetCurrentPhysicsFrameAddFrames(1);
		return;
	}

	mTickTimeModulo += mCurrentFrameTime;

	const float lTargetPeriod = 1/(float)mTargetFrameRate;
	mPhysicsFrameTime = lTargetPeriod;
	bool lAllowFpsDegradation;
	CURE_RTVAR_GET(lAllowFpsDegradation, =, Cure::GetSettings(), RTVAR_NETPHYS_ALLOWFPSDEGRADATION, true);
	if (lAllowFpsDegradation)
	{
		while (mPhysicsFrameTime*2 < mAverageFrameTime)	// Half framerate if we're on a slow platform (multiplayer over network), reiterate.
		{
			mPhysicsFrameTime *= 2;
			log_adebug("Halfing physics frame rate!");
		}
	}
	mPhysicsStepCount = (int)::floor(mTickTimeModulo / mPhysicsFrameTime);
	mPhysicsFrameTime *= mRealTimeRatio;

	/*static int c = 0;
	if (++c >= 11)
	{
		mLog.Infof(_T("Instance %p: tick time modulo is %.0f %%."), this, mTickTimeModulo*100/lTargetPeriod);
		c = 0;
	}*/

	if (mPhysicsStepCount > 0)
	{
		mAverageFrameTime = Math::Lerp(mAverageFrameTime, mCurrentFrameTime, 0.01f);

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
	/*if (c == 0)
	{
		mLog.Infof(_T("Instance %p: modulo offset is %.0f %%."), this, lModuloOffset*100/lTargetPeriod);
	}*/
	const float lDesiredModuloOffset = 0.4f;
	/*const float lCurrentTargetOverhead = mTickTimeModulo - lDesiredModuloOffset*lTargetPeriod;
	mTickTimeOverhead = Math::Lerp(mTickTimeOverhead, 2 * lCurrentTargetOverhead, 0.5f);*/
	const float lCurrentTargetOverhead = mAverageFrameTime + 1.6f*mTickTimeModulo - (1+lDesiredModuloOffset)*lTargetPeriod;
	mTickTimeOverhead = Math::Lerp(mTickTimeOverhead, lCurrentTargetOverhead, 0.6f);
	//log_volatile(mLog.Debugf(_T("AverageFrameTime %f s, modulo placement %f %%, reduction %f %%, lTargetPeriod %f s."), mAverageFrameTime, mTickTimeModulo*100/lTargetPeriod, mTickTimeOverhead*100/lTargetPeriod, lTargetPeriod));
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
	deb_assert(pSeconds <= 10*60);
	return GetCurrentPhysicsFrameAddFrames(ConvertSecondsToPhysicsFrames(pSeconds));
}

int TimeManager::GetPhysicsFrameAddFrames(int pFrameCounter, int pFrames) const
{
	deb_assert(pFrames > -gPhysicsFrameWrapLimit/2 &&
		pFrames < gPhysicsFrameWrapLimit/2);

	int lNewFrame = pFrameCounter + pFrames;
	if (lNewFrame < 0)
	{
		lNewFrame += gPhysicsFrameWrapLimit;
	}
	else if (lNewFrame >= gPhysicsFrameWrapLimit)
	{
		lNewFrame -= gPhysicsFrameWrapLimit;
	}

	deb_assert(lNewFrame >= 0 && lNewFrame < gPhysicsFrameWrapLimit);
	return lNewFrame;
}

int TimeManager::GetCurrentPhysicsFrameDelta(int pStart) const
{
	return GetPhysicsFrameDelta(mPhysicsFrameCounter, pStart);
}

int TimeManager::GetPhysicsFrameDelta(int pEnd, int pStart) const
{
	if (pStart >= gPhysicsFrameWrapLimit || pEnd >= gPhysicsFrameWrapLimit)
	{
		deb_assert(false);
		return -1;
	}
	deb_assert(pEnd >= 0);
	deb_assert(pStart >= 0);

	int lDiff = pEnd - pStart;
	if (lDiff > gPhysicsFrameWrapLimit/2)
	{
		lDiff -= gPhysicsFrameWrapLimit;
	}
	else if (lDiff < -gPhysicsFrameWrapLimit/2)
	{
		lDiff += gPhysicsFrameWrapLimit;
	}

	deb_assert(lDiff >= -gPhysicsFrameWrapLimit/2 && lDiff <= gPhysicsFrameWrapLimit/2);
	return lDiff;
}

float TimeManager::GetNormalFrameTime() const
{
	return mAverageFrameTime * mRealTimeRatio;
}

float TimeManager::GetNormalGameFrameTime() const
{
	return mAverageFrameTime * mGameRealTimeRatio;
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

float TimeManager::GetTickLoopTimeReduction() const
{
	return mTickTimeOverhead;
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
