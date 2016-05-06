
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/cure.h"
#include "../../cure/include/useraccount.h"
#include "../life.h"



namespace cure {
class ContextObject;
class NetworkAgent;
class Packet;
class TimeManager;
class UserConnection;
}



namespace life {



class Client {
public:
	Client(const cure::TimeManager* time_manager, cure::NetworkAgent* network_agent, cure::UserConnection* user_connection);
	virtual ~Client();

	cure::UserConnection* GetUserConnection() const;
	void SetUserConnection(cure::UserConnection* user_connection);

	cure::GameObjectId GetAvatarId() const;
	void SetAvatarId(cure::GameObjectId avatar_id);
	void SendAvatar(const cure::UserAccount::AvatarId& avatar_id, cure::Packet* packet);

	void StoreFrameDiff(int frame_diff);
	void QuerySendStriveTimes();
	void SendPhysicsFrame(int physics_frame_index, cure::Packet* packet = 0);
	float GetPhysicsFrameAheadCount() const;

	void SendLoginCommands(cure::Packet* packet, const str& server_greeting);

private:
	int SendStriveTimes(int network_frame_diff_count);

	// A positive return value n means that data from the client to the server comes n frames too late on average (client needs to speed up).
	// A negative return value n means that data from the client to the server comes n frames too early on average (client needs to slow down).
	int CalculateNetworkLatencyFrameDiffCount() const;

	static const int NETWORK_DEVIATION_ERROR_COUNT = 4;
	static const int NETWORK_LATENCY_CALCULATION_ARRAY_SIZE = NETWORK_DEVIATION_ERROR_COUNT;
	typedef std::vector<int> NetworkLatencyArray;

	const cure::TimeManager* time_manager_;
	cure::NetworkAgent* network_agent_;
	cure::UserConnection* user_connection_;
	cure::GameObjectId avatar_id_;
	NetworkLatencyArray network_frame_latency_array_;
	size_t network_frame_latency_array_index_;
	float measured_network_latency_frame_count_;
	float measured_network_jitter_frame_count_;
	int strive_send_error_time_counter_;
	int strive_send_unpause_frame_;
	int ignore_strive_error_time_counter_;

	logclass();
};



}
