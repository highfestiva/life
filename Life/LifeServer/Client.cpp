
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games
 


#include "../../../Cure/Include/RuntimeVariable.h"
#include "../../../Lepra/Include/Math.h"
#include "Client.h"



namespace Life
{



Client::Client(Cure::NetworkAgent* pNetworkAgent, Cure::UserConnection* pUserConnection):
	mNetworkAgent(pNetworkAgent),
	mUserConnection(pUserConnection),
	mAvatarResource(ContextObjectAccountInfo(0, 0, 0, Cure::NETWORK_OBJECT_REMOTE_CONTROLLED,
	pUserConnection->GetAccountId())),
	mAvatarId(0),
	mNetworkFrameLatencyArray(NETWORK_LATENCY_CALCULATION_ARRAY_SIZE, 0),
	mNetworkFrameLatencyArrayIndex(0),
	mMeasuredNetworkLatencyFrameCount(PHYSICS_FPS*0.3f),
	mMeasuredNetworkJitterFrameCount(PHYSICS_FPS*0.06f),
	mStriveSendErrorTimeCounter(0),
	mIgnoreStriveErrorTimeCounter(0)
{
	mAvatarResource.GetExtraData().mLoadingResource = &mAvatarResource;
}

Client::~Client()
{
	mMeasuredNetworkJitterFrameCount = -1;
	mMeasuredNetworkLatencyFrameCount = -1;
	mNetworkFrameLatencyArrayIndex = (size_t)-1;
	mAvatarId = 0;
	mUserConnection = 0;
	mNetworkAgent = 0;
}



Cure::UserConnection* Client::GetUserConnection() const
{
	return (mUserConnection);
}

void Client::SetUserConnection(Cure::UserConnection* pUserConnection)
{
	mUserConnection = pUserConnection;
}



Cure::GameObjectId Client::GetAvatarId() const
{
	return (mAvatarId);
}

void Client::SetAvatarId(Cure::GameObjectId pAvatarId)
{
	mAvatarId = pAvatarId;
}

UserContextObjectAccountInfoResource* Client::GetAvatarResource()
{
	return (&mAvatarResource);
}



void Client::StoreFrameDiff(int pFrameDiff)
{
	mNetworkFrameLatencyArray[mNetworkFrameLatencyArrayIndex] = pFrameDiff;
	++mNetworkFrameLatencyArrayIndex;
	if (mNetworkFrameLatencyArrayIndex >= mNetworkFrameLatencyArray.size())
	{
		mNetworkFrameLatencyArrayIndex = 0;
	}

	const int lMedian = Lepra::Math::CalculateMedian<int, NetworkLatencyArray>(mNetworkFrameLatencyArray);
	const float lDeviation = Lepra::Math::CalculateDeviation<float, int, NetworkLatencyArray>(mNetworkFrameLatencyArray, lMedian);
	const float lSlidingMeanLerpFactor = 0.5f;
	mMeasuredNetworkLatencyFrameCount = Lepra::Math::Lerp<float>((float)lMedian, mMeasuredNetworkLatencyFrameCount, lSlidingMeanLerpFactor);
	mMeasuredNetworkJitterFrameCount = Lepra::Math::Lerp<float>(lDeviation, mMeasuredNetworkJitterFrameCount, lSlidingMeanLerpFactor);
}

void Client::QuerySendStriveTimes()
{
	// Latency+jitter is where we're looking to hit.
	int lNetworkFrameDiffCount = CalculateNetworkLatencyFrameDiffCount();
	// Check if we should send or not.
	if (lNetworkFrameDiffCount < -CURE_RTVAR_GET(Cure::GetSettings(), RTVAR_NETPHYS_SLOWFRAMECOUNT, 2) || lNetworkFrameDiffCount > 0)
	{
		++mStriveSendErrorTimeCounter;
		mIgnoreStriveErrorTimeCounter = 0;
		// Only send once every now and then.
		if (mStriveSendErrorTimeCounter > (int)NETWORK_DEVIATION_ERROR_COUNT)
		{
			mStriveSendErrorTimeCounter = 0;
			SendStriveTimes(lNetworkFrameDiffCount);
		}
		else
		{
			Lepra::String s;
			if (lNetworkFrameDiffCount < 0)
			{
				s = Lepra::StringUtility::Format(_T("Client movement arrives %i frames early on average."), -lNetworkFrameDiffCount);
			}
			else
			{
				s = Lepra::StringUtility::Format(_T("Client movement arrives %i frames late on average."), lNetworkFrameDiffCount);
			}
			log_debug(s);
		}
	}
	else
	{
		++mIgnoreStriveErrorTimeCounter;
		if (mIgnoreStriveErrorTimeCounter > (int)NETWORK_DEVIATION_ERROR_COUNT)	// Reset send counter if we're mostly good.
		{
			mIgnoreStriveErrorTimeCounter = 0;
			mStriveSendErrorTimeCounter = 0;
		}
	}
}

void Client::SendPhysicsFrame(int pPhysicsFrameIndex)
{
	mNetworkAgent->SendNumberMessage(true, mUserConnection->GetSocket(), Cure::MessageNumber::INFO_SET_TIME, pPhysicsFrameIndex, 0);
}

float Client::GetPhysicsFrameAheadCount() const
{
	return (mMeasuredNetworkLatencyFrameCount+mMeasuredNetworkJitterFrameCount);
}


void Client::SendStriveTimes(int pNetworkFrameDiffCount)
{
	int lPhysicsTickAdjustmentFrameCount = PHYSICS_FPS;	// Spread over some time (=frames).
	// Try to adjust us to be x frames early. Being too early is much better than being late,
	// since early network data can be buffered and used in upcomming frames, but late network
	// data is always thrown away.
	++pNetworkFrameDiffCount;
	float lPhysicsTickAdjustmentTime = 0;
	if (pNetworkFrameDiffCount > 0)
	{
		// Speed up client physics by taking longer time steps.
		lPhysicsTickAdjustmentTime = (float)pNetworkFrameDiffCount/lPhysicsTickAdjustmentFrameCount;
	}
	else
	{
		// Slow down client physics by taking shorter time steps.
		pNetworkFrameDiffCount = -pNetworkFrameDiffCount;	// We always send a positive frame count.
		while (pNetworkFrameDiffCount > lPhysicsTickAdjustmentFrameCount)
		{
			lPhysicsTickAdjustmentFrameCount *= (pNetworkFrameDiffCount/lPhysicsTickAdjustmentFrameCount + 1);
		}
		lPhysicsTickAdjustmentTime = (float)-pNetworkFrameDiffCount/lPhysicsTickAdjustmentFrameCount;
	}
	mNetworkAgent->SendNumberMessage(true, mUserConnection->GetSocket(), Cure::MessageNumber::INFO_ADJUST_TIME, pNetworkFrameDiffCount, lPhysicsTickAdjustmentTime);
}



int Client::CalculateNetworkLatencyFrameDiffCount() const
{
	float lDiff = mMeasuredNetworkLatencyFrameCount;
	lDiff += (mMeasuredNetworkLatencyFrameCount < 0)? -mMeasuredNetworkJitterFrameCount : mMeasuredNetworkJitterFrameCount;
	return ((int)::ceil(lDiff));
}



LOG_CLASS_DEFINE(NETWORK_SERVER, Client);



}
