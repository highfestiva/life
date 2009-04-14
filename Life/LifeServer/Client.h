
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../../Cure/Include/Cure.h"
#include "../../Cure/Include/NetworkAgent.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/UserConnection.h"
#include "../LifeDefinitions.h"
#include "ServerContextResource.h"



namespace Life
{



class Client
{
public:
	Client(Cure::NetworkAgent* pNetworkAgent, Cure::UserConnection* pUserConnection);
	virtual ~Client();

	Cure::UserConnection* GetUserConnection() const;
	void SetUserConnection(Cure::UserConnection* pUserConnection);

	Cure::GameObjectId GetAvatarId() const;
	void SetAvatarId(Cure::GameObjectId pAvatarId);
	UserContextObjectAccountInfoResource* Client::GetAvatarResource();

	void StoreFrameDiff(int pFrameDiff);
	void QuerySendStriveTimes();
	void SendPhysicsFrame(int pPhysicsFrameIndex);
	float GetPhysicsFrameAheadCount() const;

private:
	void SendStriveTimes();

	// A positive return value n means that data from the client to the server comes n frames too late on average (client needs to speed up).
	// A negative return value n means that data from the client to the server comes n frames too early on average (client needs to slow down).
	int CalculateNetworkLatencyFrameDiffCount() const;

	static const int NETWORK_LATENCY_CALCULATION_ARRAY_SIZE = PHYSICS_FPS;
	typedef std::vector<int> NetworkLatencyArray;

	Cure::NetworkAgent* mNetworkAgent;
	Cure::UserConnection* mUserConnection;
	UserContextObjectAccountInfoResource mAvatarResource;
	Cure::GameObjectId mAvatarId;
	NetworkLatencyArray mNetworkFrameLatencyArray;
	size_t mNetworkFrameLatencyArrayIndex;
	float mMeasuredNetworkLatencyFrameCount;
	float mMeasuredNetworkJitterFrameCount;
	int mStriveSendErrorTimeCounter;
	int mIgnoreStriveErrorTimeCounter;

	LOG_CLASS_DECLARE();
};



}
