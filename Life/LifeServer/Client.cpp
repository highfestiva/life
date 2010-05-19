
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games
 


#include "Client.h"
#include "../../Cure/Include/ContextObject.h"
#include "../../Cure/Include/Cure.h"
#include "../../Cure/Include/NetworkAgent.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Cure/Include/UserConnection.h"
#include "../../Lepra/Include/Math.h"
#include "../Life.h"



namespace Life
{



Client::Client(Cure::TimeManager* pTimeManager, Cure::NetworkAgent* pNetworkAgent, Cure::UserConnection* pUserConnection):
	mTimeManager(pTimeManager),
	mNetworkAgent(pNetworkAgent),
	mUserConnection(pUserConnection),
	mAvatarId(0),
	mNetworkFrameLatencyArray(NETWORK_LATENCY_CALCULATION_ARRAY_SIZE, 0),
	mNetworkFrameLatencyArrayIndex(0),
	mMeasuredNetworkLatencyFrameCount(PHYSICS_FPS*0.3f),
	mMeasuredNetworkJitterFrameCount(PHYSICS_FPS*0.06f),
	mStriveSendErrorTimeCounter(0),
	mStriveSendPauseFrameCount(-100000000),
	mIgnoreStriveErrorTimeCounter(0)
{
}

Client::~Client()
{
	mMeasuredNetworkJitterFrameCount = -1;
	mMeasuredNetworkLatencyFrameCount = -1;
	mNetworkFrameLatencyArrayIndex = (size_t)-1;
	mAvatarId = 0;
	mUserConnection = 0;
	mNetworkAgent = 0;
	mTimeManager = 0;
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

void Client::SendAvatar(const Cure::UserAccount::AvatarId& pAvatarId, Cure::Packet* pPacket)
{
	mNetworkAgent->SendStatusMessage(mUserConnection->GetSocket(), 0, Cure::REMOTE_OK,
		Cure::MessageStatus::INFO_AVATAR, wstrutil::Encode(pAvatarId), pPacket);
}



void Client::StoreFrameDiff(int pFrameDiff)
{
	mNetworkFrameLatencyArray[mNetworkFrameLatencyArrayIndex] = pFrameDiff;
	++mNetworkFrameLatencyArrayIndex;
	if (mNetworkFrameLatencyArrayIndex >= mNetworkFrameLatencyArray.size())
	{
		mNetworkFrameLatencyArrayIndex = 0;
	}

	const int lMedian = Math::CalculateMedian<int, NetworkLatencyArray>(mNetworkFrameLatencyArray);
	const float lDeviation = Math::CalculateDeviation<float, int, NetworkLatencyArray>(mNetworkFrameLatencyArray, lMedian);
	const float lSlidingMeanLerpFactor = 0.5f;
	mMeasuredNetworkLatencyFrameCount = Math::Lerp<float>((float)lMedian, mMeasuredNetworkLatencyFrameCount, lSlidingMeanLerpFactor);
	mMeasuredNetworkJitterFrameCount = Math::Lerp<float>(lDeviation, mMeasuredNetworkJitterFrameCount, lSlidingMeanLerpFactor);
}

void Client::QuerySendStriveTimes()
{
	// Latency+jitter is where we're looking to hit.
	int lNetworkFrameDiffCount = CalculateNetworkLatencyFrameDiffCount();
	// Check if we should send or not.
	if (lNetworkFrameDiffCount < -CURE_RTVAR_GET(Cure::GetSettings(), RTVAR_NETPHYS_SLOWFRAMECOUNT, 2) ||
		lNetworkFrameDiffCount > 0)
	{
		++mStriveSendErrorTimeCounter;
		mIgnoreStriveErrorTimeCounter = 0;
		if (mStriveSendPauseFrameCount-mTimeManager->GetCurrentPhysicsFrame() > 0)
		{
			mLog.AInfo("Want to send strive times, but skipping since last transmission is still in effekt.");
		}
		else if (mStriveSendErrorTimeCounter >= NETWORK_DEVIATION_ERROR_COUNT)	// Only send if the error repeats itself a few times.
		{
			mStriveSendErrorTimeCounter = 0;
			mStriveSendPauseFrameCount = SendStriveTimes(lNetworkFrameDiffCount);

			// Reset the whole latency array to be more like something ideal.
			size_t lCount = mNetworkFrameLatencyArray.size();
			mNetworkFrameLatencyArray.resize(0);
			mNetworkFrameLatencyArray.resize(lCount, 1);
		}
		else
		{
			str s;
			if (lNetworkFrameDiffCount < 0)
			{
				s = strutil::Format(_T("Client movement arrives %i frames early on average."), -lNetworkFrameDiffCount);
			}
			else
			{
				s = strutil::Format(_T("Client movement arrives %i frames late on average."), lNetworkFrameDiffCount);
			}
			log_debug(s);
		}
	}
	else
	{
		++mIgnoreStriveErrorTimeCounter;
		if (mIgnoreStriveErrorTimeCounter >= (int)NETWORK_DEVIATION_ERROR_COUNT)	// Reset send counter if we're mostly good.
		{
			log_volatile(mLog.Debugf(_T("Resetting strive error counter (time diff is %i frames)."), lNetworkFrameDiffCount));
			mIgnoreStriveErrorTimeCounter = 0;
			mStriveSendErrorTimeCounter = 0;
		}
	}
}

void Client::SendPhysicsFrame(int pPhysicsFrameIndex, Cure::Packet* pPacket)
{
	mNetworkAgent->SendNumberMessage(true, mUserConnection->GetSocket(), Cure::MessageNumber::INFO_SET_TIME, pPhysicsFrameIndex, 0, pPacket);
}

float Client::GetPhysicsFrameAheadCount() const
{
	return (mMeasuredNetworkLatencyFrameCount+mMeasuredNetworkJitterFrameCount);
}


int Client::SendStriveTimes(int pNetworkFrameDiffCount)
{
	// TODO: insert half a ping time below!
	const int lHalfPingFrameCount = PHYSICS_FPS/3+(int)mMeasuredNetworkJitterFrameCount;

	if (::abs(pNetworkFrameDiffCount) >= PHYSICS_FPS)
	{
		SendPhysicsFrame(mTimeManager->GetCurrentPhysicsFrame()+lHalfPingFrameCount);
		return (lHalfPingFrameCount*2);
	}

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
		pNetworkFrameDiffCount = -pNetworkFrameDiffCount;	// Negated again below.
		if (pNetworkFrameDiffCount >= lPhysicsTickAdjustmentFrameCount*0.8f)
		{
			lPhysicsTickAdjustmentFrameCount = pNetworkFrameDiffCount+PHYSICS_FPS;
		}
		lPhysicsTickAdjustmentTime = (float)-pNetworkFrameDiffCount/lPhysicsTickAdjustmentFrameCount;
		pNetworkFrameDiffCount = -pNetworkFrameDiffCount;
	}
	mLog.Infof(_T("Sending time adjustment %i frames, spread over %i frames, to client."), pNetworkFrameDiffCount, lPhysicsTickAdjustmentFrameCount);
	mNetworkAgent->SendNumberMessage(true, mUserConnection->GetSocket(), Cure::MessageNumber::INFO_ADJUST_TIME, lPhysicsTickAdjustmentFrameCount, lPhysicsTickAdjustmentTime);
	return (lHalfPingFrameCount*2+lPhysicsTickAdjustmentFrameCount);
}



int Client::CalculateNetworkLatencyFrameDiffCount() const
{
	// Note that latency might be negative, but jitter is always positive. We always add,
	// never subtract, the jitter, since we want client to be these frames ahead of the server.
	float lDiff = mMeasuredNetworkLatencyFrameCount + mMeasuredNetworkJitterFrameCount;
	return ((int)::ceil(lDiff));
}



LOG_CLASS_DEFINE(NETWORK_SERVER, Client);



}
