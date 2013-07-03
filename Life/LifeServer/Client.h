
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/Cure.h"
#include "../../Cure/Include/UserAccount.h"
#include "../Life.h"



namespace Cure
{
class ContextObject;
class NetworkAgent;
class Packet;
class TimeManager;
class UserConnection;
}



namespace Life
{



class Client
{
public:
	Client(const Cure::TimeManager* pTimeManager, Cure::NetworkAgent* pNetworkAgent, Cure::UserConnection* pUserConnection);
	virtual ~Client();

	Cure::UserConnection* GetUserConnection() const;
	void SetUserConnection(Cure::UserConnection* pUserConnection);

	Cure::GameObjectId GetAvatarId() const;
	void SetAvatarId(Cure::GameObjectId pAvatarId);
	void SendAvatar(const Cure::UserAccount::AvatarId& pAvatarId, Cure::Packet* pPacket);

	void StoreFrameDiff(int pFrameDiff);
	void QuerySendStriveTimes();
	void SendPhysicsFrame(int pPhysicsFrameIndex, Cure::Packet* pPacket = 0);
	float GetPhysicsFrameAheadCount() const;

	void SendLoginCommands(Cure::Packet* pPacket, const str& pServerGreeting);

private:
	int SendStriveTimes(int pNetworkFrameDiffCount);

	// A positive return value n means that data from the client to the server comes n frames too late on average (client needs to speed up).
	// A negative return value n means that data from the client to the server comes n frames too early on average (client needs to slow down).
	int CalculateNetworkLatencyFrameDiffCount() const;

	static const int NETWORK_DEVIATION_ERROR_COUNT = 4;
	static const int NETWORK_LATENCY_CALCULATION_ARRAY_SIZE = NETWORK_DEVIATION_ERROR_COUNT;
	typedef std::vector<int> NetworkLatencyArray;

	const Cure::TimeManager* mTimeManager;
	Cure::NetworkAgent* mNetworkAgent;
	Cure::UserConnection* mUserConnection;
	Cure::GameObjectId mAvatarId;
	NetworkLatencyArray mNetworkFrameLatencyArray;
	size_t mNetworkFrameLatencyArrayIndex;
	float mMeasuredNetworkLatencyFrameCount;
	float mMeasuredNetworkJitterFrameCount;
	int mStriveSendErrorTimeCounter;
	int mStriveSendUnpauseFrame;
	int mIgnoreStriveErrorTimeCounter;

	LOG_CLASS_DECLARE();
};



}
