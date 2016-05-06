
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "client.h"
#include "../../cure/include/contextobject.h"
#include "../../cure/include/cure.h"
#include "../../cure/include/networkagent.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../cure/include/userconnection.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/resourcetracker.h"
#include "../life.h"
#include "rtvar.h"



namespace life {



Client::Client(const cure::TimeManager* time_manager, cure::NetworkAgent* network_agent, cure::UserConnection* user_connection):
	time_manager_(time_manager),
	network_agent_(network_agent),
	user_connection_(user_connection),
	avatar_id_(0),
	network_frame_latency_array_(NETWORK_LATENCY_CALCULATION_ARRAY_SIZE, 0),
	network_frame_latency_array_index_(0),
	measured_network_latency_frame_count_(PHYSICS_FPS*0.3f),
	measured_network_jitter_frame_count_(PHYSICS_FPS*0.06f),
	strive_send_error_time_counter_(0),
	strive_send_unpause_frame_(time_manager->GetCurrentPhysicsFrame()),
	ignore_strive_error_time_counter_(0) {
	LEPRA_ACQUIRE_RESOURCE(Client);
}

Client::~Client() {
	measured_network_jitter_frame_count_ = -1;
	measured_network_latency_frame_count_ = -1;
	network_frame_latency_array_index_ = (size_t)-1;
	avatar_id_ = 0;
	user_connection_ = 0;
	network_agent_ = 0;
	time_manager_ = 0;

	LEPRA_RELEASE_RESOURCE(Client);
}



cure::UserConnection* Client::GetUserConnection() const {
	return (user_connection_);
}

void Client::SetUserConnection(cure::UserConnection* user_connection) {
	user_connection_ = user_connection;
}



cure::GameObjectId Client::GetAvatarId() const {
	return (avatar_id_);
}

void Client::SetAvatarId(cure::GameObjectId avatar_id) {
	avatar_id_ = avatar_id;
}

void Client::SendAvatar(const cure::UserAccount::AvatarId& avatar_id, cure::Packet* packet) {
	network_agent_->SendStatusMessage(user_connection_->GetSocket(), 0, cure::kRemoteOk,
		cure::MessageStatus::kInfoAvatar, avatar_id, packet);
}



void Client::StoreFrameDiff(int frame_diff) {
	network_frame_latency_array_[network_frame_latency_array_index_] = frame_diff;
	++network_frame_latency_array_index_;
	if (network_frame_latency_array_index_ >= network_frame_latency_array_.size()) {
		network_frame_latency_array_index_ = 0;
	}

	const int median = Math::CalculateMedian<int, NetworkLatencyArray>(network_frame_latency_array_);
	const float deviation = Math::CalculateDeviation<float, int, NetworkLatencyArray>(network_frame_latency_array_, median);
	const float sliding_mean_lerp_factor = 0.5f;
	measured_network_latency_frame_count_ = Math::Lerp<float>((float)median, measured_network_latency_frame_count_, sliding_mean_lerp_factor);
	measured_network_jitter_frame_count_ = Math::Lerp<float>(deviation, measured_network_jitter_frame_count_, sliding_mean_lerp_factor);
}

void Client::QuerySendStriveTimes() {
	// Latency+jitter is where we're looking to hit.
	int _network_frame_diff_count = CalculateNetworkLatencyFrameDiffCount();
	// Check if we should send or not.
	int slow_frame_count;
	v_get(slow_frame_count, =, cure::GetSettings(), kRtvarNetphysSlowframecount, 2);
	if (_network_frame_diff_count < -slow_frame_count || _network_frame_diff_count > 0) {
		++strive_send_error_time_counter_;
		ignore_strive_error_time_counter_ = 0;
		if (time_manager_->GetCurrentPhysicsFrameDelta(strive_send_unpause_frame_) < 0) {
			log_debug("Want to send strive times, but skipping since last transmission is still in effekt.");
		} else if (strive_send_error_time_counter_ >= NETWORK_DEVIATION_ERROR_COUNT) {	// Only send if the error repeats itself a few times.
			strive_send_error_time_counter_ = 0;
			strive_send_unpause_frame_ = time_manager_->GetCurrentPhysicsFrameAddFrames(SendStriveTimes(_network_frame_diff_count));

			// Reset the whole latency array to be more like something ideal.
			size_t count = network_frame_latency_array_.size();
			network_frame_latency_array_.resize(0);
			network_frame_latency_array_.resize(count, 1);
		} else {
			strive_send_unpause_frame_ = time_manager_->GetCurrentPhysicsFrame();

			str s;
			if (_network_frame_diff_count < 0) {
				s = strutil::Format("Client movement arrives %i frames early on average.", -_network_frame_diff_count);
			} else {
				s = strutil::Format("Client movement arrives %i frames late on average.", _network_frame_diff_count);
			}
			log_debug(s);
		}
	} else {
		++ignore_strive_error_time_counter_;
		if (ignore_strive_error_time_counter_ >= (int)NETWORK_DEVIATION_ERROR_COUNT) {	// Reset send counter if we're mostly good.
			log_volatile(log_.Debugf("Resetting strive error counter (time diff is %i frames).", _network_frame_diff_count));
			ignore_strive_error_time_counter_ = 0;
			strive_send_error_time_counter_ = 0;
		}
	}
}

void Client::SendPhysicsFrame(int physics_frame_index, cure::Packet* packet) {
	network_agent_->SendNumberMessage(true, user_connection_->GetSocket(), cure::MessageNumber::kInfoSetTime, physics_frame_index, 0, packet);
}

float Client::GetPhysicsFrameAheadCount() const {
	return (measured_network_latency_frame_count_+measured_network_jitter_frame_count_);
}



void Client::SendLoginCommands(cure::Packet* packet, const str& server_greeting) {
	int physics_fps;
	v_get(physics_fps, =, cure::GetSettings(), kRtvarPhysicsFps, PHYSICS_FPS);
	double physics_rtr;
	v_get(physics_rtr, =, cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
	bool physics_halt;
	v_get(physics_halt, =, cure::GetSettings(), kRtvarPhysicsHalt, false);
	const str cmd = strutil::Format(
		"#" kRtvarPhysicsFps " %i;\n"
		"#" kRtvarPhysicsRtr " %f;\n"
		"#" kRtvarPhysicsHalt " %s;\n"
		"%s",
		physics_fps, physics_rtr, strutil::BoolToString(physics_halt).c_str(), server_greeting.c_str());
	network_agent_->SendStatusMessage(user_connection_->GetSocket(), 0, cure::kRemoteOk,
		cure::MessageStatus::kInfoCommand, cmd, packet);
}



int Client::SendStriveTimes(int network_frame_diff_count) {
	// TODO: insert half a ping time below!
	const int half_ping_frame_count = PHYSICS_FPS/3+(int)measured_network_jitter_frame_count_;

	if (::abs(network_frame_diff_count) >= PHYSICS_FPS) {
		SendPhysicsFrame(time_manager_->GetCurrentPhysicsFrameAddFrames(half_ping_frame_count));
		return (half_ping_frame_count*2 + 2);
	}

	int physics_tick_adjustment_frame_count = PHYSICS_FPS;	// Spread over some time (=frames).
	// Try to adjust us to be x frames early. Being too early is much better than being late,
	// since early network data can be buffered and used in upcomming frames, but late network
	// data is always thrown away.
	++network_frame_diff_count;
	float physics_tick_adjustment_time = 0;
	if (network_frame_diff_count > 0) {
		// Speed up client physics by taking longer time steps.
		physics_tick_adjustment_time = (float)network_frame_diff_count/physics_tick_adjustment_frame_count;
	} else {
		// Slow down client physics by taking shorter time steps.
		network_frame_diff_count = -network_frame_diff_count;	// Negated again below.
		if (network_frame_diff_count >= physics_tick_adjustment_frame_count*0.8f) {
			physics_tick_adjustment_frame_count = network_frame_diff_count+PHYSICS_FPS;
		}
		physics_tick_adjustment_time = (float)-network_frame_diff_count/physics_tick_adjustment_frame_count;
		network_frame_diff_count = -network_frame_diff_count;
	}
	log_volatile(log_.Debugf("Sending time adjustment %i frames, spread over %i frames, to client.", network_frame_diff_count, physics_tick_adjustment_frame_count));
	network_agent_->SendNumberMessage(true, user_connection_->GetSocket(), cure::MessageNumber::kInfoAdjustTime, physics_tick_adjustment_frame_count, physics_tick_adjustment_time);
	return (half_ping_frame_count*2 + physics_tick_adjustment_frame_count);
}



int Client::CalculateNetworkLatencyFrameDiffCount() const {
	// Note that latency might be negative, but jitter is always positive. We always add,
	// never subtract, the jitter, since we want client to be these frames ahead of the server.
	float diff = measured_network_latency_frame_count_ + measured_network_jitter_frame_count_;
	return ((int)::ceil(diff));
}



loginstance(kNetworkServer, Client);



}
