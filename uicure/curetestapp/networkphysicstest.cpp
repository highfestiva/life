
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine




#include "pch.h"
#ifndef CURE_TEST_WITHOUT_UI
#include "../../lepra/include/lepraassert.h"
#include <list>
#include "../../cure/include/networkclient.h"
#include "../../cure/include/networkserver.h"
#include "../../cure/include/packet.h"
#include "../../cure/include/positionaldata.h"
#include "../../lepra/include/hirestimer.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/network.h"
#include "../../lepra/include/random.h"
#include "../../lepra/include/systemmanager.h"
#include "../../tbc/include/physicsmanager.h"
#include "../../tbc/include/physicsmanagerfactory.h"
#include "../../uilepra/include/uicore.h"
#include "../../uilepra/include/uidisplaymanager.h"
#include "../../uilepra/include/uiinput.h"
#include "../../uitbc/include/uibasicmeshcreator.h"
#include "../../uitbc/include/uirendererfactory.h"
#include "../../uitbc/include/uitrianglebasedgeometry.h"



using namespace lepra;

const int CLIENT_COUNT = 2;
const float PHYSICS_FPS = (float)kCureStandardFrameRate;
const float PHYSICS_SECONDS_PER_FRAME = 1/PHYSICS_FPS;
const int NETWORK_LATENCY_CALCULATION_ARRAY_SIZE = (int)PHYSICS_FPS;	// Just pick something small.
const int NETWORK_POSITIONAL_PACKET_BUFFER_SIZE = (int)PHYSICS_FPS/2;
class NetworkPhysicsTest {
};
lepra::LogDecorator gNptLog(lepra::LogType::GetLogger(lepra::LogType::kTest), typeid(NetworkPhysicsTest));



void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);
void StoreFrameDiff(int agent_index, int frame_diff);



struct AgentData: public uilepra::KeyCodeInputObserver {
	struct BodyInfo {
		str type_;
		tbc::PhysicsManager::BodyID physics_id_;
		tbc::GeometryBase* graphics_geometry_;
		uitbc::Renderer::GeometryID graphics_id_;
		int last_set_frame_index_;	// TODO: this is a hack, read up on what algo to use for clients that want to set other clients positions.
		lepra::vec3 graphics_position_;
		BodyInfo():
			type_("?"),
			physics_id_(0),
			graphics_geometry_(0),
			graphics_id_((uitbc::Renderer::GeometryID)-1),
			last_set_frame_index_(0) {
		}
		BodyInfo(str type, tbc::PhysicsManager::BodyID body_id, tbc::GeometryBase* geometry, uitbc::Renderer::GeometryID geometry_id):
			type_(type),
			physics_id_(body_id),
			graphics_geometry_(geometry),
			graphics_id_(geometry_id),
			last_set_frame_index_(0) {
		}
	};
	cure::NetworkAgent* network_agent_;
	tbc::PhysicsManager* physics_;
	std::vector<BodyInfo> body_array_;
	uilepra::DisplayManager* display_;
	lepra::Canvas* screen_;
	uitbc::Renderer* renderer_;
	uilepra::InputManager* input_;
	float tick_time_modulo_;
	bool active_;
	int current_physics_frame_index_;
	double emulated_network_send_latency_frame_count_;
	double emulated_network_send_jitter_frame_count_;
	double server_measured_network_latency_frame_count_;
	double server_measured_network_jitter_frame_count_;
	lepra::uint32 client_id_;
	int network_frame_latency_array_index_;
	std::vector<int> network_frame_latency_array_;
	int physics_tick_adjustment_frame_count_;
	float physics_tick_adjustment_time_;
	int strive_send_error_time_counter_;
	int ingore_strive_error_time_counter_;
	int packet_usage_late_drop_count_;
	int packet_usage_early_drop_count_;
	int packet_usage_use_count_;
	int move_keys_;
	std::vector<cure::ObjectPositionalData*> positional_buffer_;
	cure::ObjectPositionalData client_last_send_positional_data_;
	bool network_force_send_;
	double bandwidth_start_measure_time_;
	lepra::uint64 bandwidth_last_sent_;
	lepra::uint64 bandwidth_last_received_;

	AgentData():
		network_agent_(0),
		physics_(0),
		display_(0),
		screen_(0),
		renderer_(0),
		input_(0),
		tick_time_modulo_(0),
		active_(false),
		current_physics_frame_index_(0),
		emulated_network_send_latency_frame_count_(0),
		emulated_network_send_jitter_frame_count_(0),
		server_measured_network_latency_frame_count_(0),
		server_measured_network_jitter_frame_count_(0),
		client_id_(0),
		network_frame_latency_array_index_(0),
		network_frame_latency_array_(NETWORK_LATENCY_CALCULATION_ARRAY_SIZE),
		physics_tick_adjustment_frame_count_(0),
		physics_tick_adjustment_time_(0),
		strive_send_error_time_counter_(100),
		ingore_strive_error_time_counter_(0),
		packet_usage_late_drop_count_(0),
		packet_usage_early_drop_count_(0),
		packet_usage_use_count_(0),
		move_keys_(0),
		positional_buffer_(NETWORK_POSITIONAL_PACKET_BUFFER_SIZE),
		network_force_send_(false),
		bandwidth_start_measure_time_(0),
		bandwidth_last_sent_(0),
		bandwidth_last_received_(0) {
	}
	~AgentData() {
		for (int x = 0; x < NETWORK_POSITIONAL_PACKET_BUFFER_SIZE; ++x) {
			StorePositionalData(x, 0);
		}
		delete (input_);
		input_ = 0;
		delete (renderer_);
		renderer_ = 0;
		delete (screen_);
		screen_ = 0;
		delete (display_);
		display_ = 0;
		delete (physics_);
		physics_ = 0;
		delete (network_agent_);
		network_agent_ = 0;
	}

	void AddBody(str type, tbc::PhysicsManager::BodyID body_id, tbc::GeometryBase* geometry, uitbc::Renderer::GeometryID geometry_id) {
		body_array_.push_back(BodyInfo(type, body_id, geometry, geometry_id));
	}

	void StorePositionalData(int x, cure::ObjectPositionalData* data) {
		delete (positional_buffer_[x]);
		positional_buffer_[x] = data;
	}

	bool OnKeyDown(uilepra::InputManager::KeyCode key_code) {
		switch (key_code) {
			case uilepra::InputManager::kInKbdUp:	move_keys_ |= 1;	break;
			case uilepra::InputManager::kInKbdDown:	move_keys_ |= 2;	break;
			case uilepra::InputManager::kInKbdLeft:	move_keys_ |= 4;	break;
			case uilepra::InputManager::kInKbdRight:	move_keys_ |= 8;	break;
			default: break;
		}
		return (false);
	}
	bool OnKeyUp(uilepra::InputManager::KeyCode key_code) {
		switch (key_code) {
			case uilepra::InputManager::kInKbdUp:	move_keys_ &= ~1;	break;
			case uilepra::InputManager::kInKbdDown:	move_keys_ &= ~2;	break;
			case uilepra::InputManager::kInKbdLeft:	move_keys_ &= ~4;	break;
			case uilepra::InputManager::kInKbdRight:	move_keys_ &= ~8;	break;
			default: break;
		}
		network_force_send_ = true;
		return (false);
	}
};

struct DelayedNetworkSend {
	DelayedNetworkSend(double absolute_send_time, int client_index, int frame_index, const cure::ObjectPositionalData& movement,
		AgentData* from, AgentData* to):
		absolute_send_time_(absolute_send_time),
		from_(from),
		to_(to) {
		packet_ = to_->network_agent_->GetPacketFactory()->Allocate();
		cure::MessageObjectPosition* message = (cure::MessageObjectPosition*)to_->network_agent_->GetPacketFactory()->GetMessageFactory()->Allocate(cure::kMessageTypeObjectPosition);
		packet_->AddMessage(message);
		message->Store(packet_, client_index, frame_index, movement);
		packet_->StoreHeader();
	}
	double absolute_send_time_;
	AgentData* from_;
	AgentData* to_;
	cure::Packet* packet_;
};
class ServerDummyLoginListener: public cure::NetworkServer::LoginListener {
public:
	ServerDummyLoginListener(cure::UserAccountManager* user_account_manager):
		user_account_manager_(user_account_manager) {
	}
	cure::UserAccount::Availability QueryLogin(const cure::LoginId& login_id, cure::UserAccount::AccountId& account_id) {
		return (user_account_manager_->GetUserAccountStatus(login_id, account_id));
	};
	void OnLogin(cure::UserConnection*) {};
	void OnLogout(cure::UserConnection*) {};

	cure::UserAccountManager* user_account_manager_;
};



AgentData g_server;
ServerDummyLoginListener gLoginListener(new cure::MemoryUserAccountManager());
AgentData g_client[CLIENT_COUNT];
double g_absolute_time = 0;
double g_fps = 0;
std::list<DelayedNetworkSend> g_delayed_network_send_array;

void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);



bool NetworkLoginClients() {
	str _context;
	bool test_ok = true;

	if (test_ok) {
		_context = "network start";
		test_ok = lepra::Network::Start();
	}

	cure::NetworkServer* _server = 0;
	if (test_ok) {
		_context = "server start";
		_server = new cure::NetworkServer(cure::GetSettings(), &gLoginListener);
		g_server.network_agent_ = _server;
		//_server->GetPacketFactory()->SetMessageFactory(new PositionalMessageFactory());
		test_ok = _server->Start(":12345");
	}

	// Create user accounts.
	for (int x = 0; test_ok && x < CLIENT_COUNT; ++x) {
		_context = "client create";
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(lepra::strutil::Format("user%i", x), password);
		test_ok = gLoginListener.user_account_manager_->AddUserAccount(user);
	}

	class ServerPoller: public lepra::Thread {
	public:
		ServerPoller(cure::NetworkServer* server):
			Thread("Server poller"),
			server_(server) {
		}
		void Run() {
			cure::Packet* packet = server_->GetPacketFactory()->Allocate();
			packet->Release();
			while (!GetStopRequest()) {
				lepra::uint32 client_id;
				server_->ReceiveFirstPacket(packet, client_id);
				server_->SendAll();
				Thread::Sleep(0.05);
			}
			server_->GetPacketFactory()->Release(packet);
		};
		cure::NetworkServer* server_;
	};
	ServerPoller thread(_server);
	thread.Start();

	// Login clients.
	for (int x = 0; test_ok && x < CLIENT_COUNT; ++x) {
		cure::NetworkClient* _client = new cure::NetworkClient(cure::GetSettings());
		test_ok = _client->Open(":11233");
		deb_assert(test_ok);
		g_client[x].network_agent_ = _client;
		_context = "client connect+login";
		str bad_password("feddo");
		cure::MangledPassword password(bad_password);
		cure::LoginId user(lepra::strutil::Format("user%i", x), password);
		_client->StartConnectLogin(":12345", 2.0, user);
		cure::RemoteStatus status = _client->WaitLogin();
		g_client[x].client_id_ = _client->GetLoginAccountId();
		test_ok = (status == cure::kRemoteOk);
		if (test_ok) {
			g_server.emulated_network_send_latency_frame_count_ = 0.400f*PHYSICS_FPS;
			g_server.emulated_network_send_jitter_frame_count_ = 0.080f*PHYSICS_FPS;	// Interval around latency between -jitter to +jitter.
			g_client[x].emulated_network_send_latency_frame_count_ = (x+1)*0.400f*PHYSICS_FPS;
			g_client[x].emulated_network_send_jitter_frame_count_ = 0.080f*PHYSICS_FPS;	// Interval around latency between -jitter to +jitter.
			for (int y = 0; y < NETWORK_LATENCY_CALCULATION_ARRAY_SIZE; ++y) {
				StoreFrameDiff(x, (int)(g_client[x].emulated_network_send_latency_frame_count_+g_client[x].emulated_network_send_jitter_frame_count_*(y&1)*((y&2)-1)));
			}
		}
	}

	ReportTestResult(gNptLog, "NetworkLoginClients", _context, test_ok);
	return (test_ok);
}

void NetworkShutdown() {
	deb_assert(g_server.network_agent_->GetConnectionCount() == (size_t)CLIENT_COUNT);
	for (int x = 0; x < CLIENT_COUNT; ++x) {
		deb_assert(g_client[x].network_agent_->GetConnectionCount() == 1);
		delete (g_client[x].network_agent_);
		g_client[x].network_agent_ = 0;
	}
	delete (g_server.network_agent_);
	g_server.network_agent_ = 0;
	lepra::Network::Stop();
}

bool CreateWorld(AgentData& agent_data) {
	bool test_ok = true;
	agent_data.tick_time_modulo_ = 0;
	agent_data.physics_ = tbc::PhysicsManagerFactory::Create(tbc::PhysicsManagerFactory::kEngineOde, 1000, 6, 6);
	agent_data.physics_->SetGravity(lepra::vec3(0, 0, -10));
	// Create floor on server.
	const float floor_size = 100;
	const float client_size = 5;
	lepra::xform floor_placement;
	lepra::vec3 floor_position(0, 0, -floor_size/2);
	floor_placement.SetPosition(floor_position);
	tbc::PhysicsManager::BodyID physics_id;
	uitbc::TriangleBasedGeometry* _geometry;
	uitbc::Renderer::GeometryID graphics_id;
	physics_id = agent_data.physics_->CreateBox(true, floor_placement, 3, lepra::vec3(floor_size, floor_size, floor_size), tbc::PhysicsManager::kStatic, 1, 1.0f);
	_geometry = uitbc::BasicMeshCreator::CreateFlatBox(floor_size, floor_size, floor_size);
	_geometry->SetAlwaysVisible(true);
	graphics_id = agent_data.renderer_->AddGeometry(_geometry, uitbc::Renderer::kMatSingleColorSolid, uitbc::Renderer::kNoShadows);
	agent_data.AddBody("StaticBox", physics_id, _geometry, graphics_id);
	// Create client spheres on server.
	for (int y = 0; y < CLIENT_COUNT; ++y) {
		lepra::xform client_placement;
		lepra::vec3 client_position(-floor_size/2+y*client_size*4, 0, client_size*10);
		client_placement.SetPosition(client_position);
		physics_id = agent_data.physics_->CreateSphere(true, client_placement, client_size, client_size, tbc::PhysicsManager::kDynamic, 1, 1.0f);
		agent_data.physics_->EnableGravity(physics_id, true);
		_geometry = uitbc::BasicMeshCreator::CreateEllipsoid(client_size, client_size, client_size, 10, 10);
		_geometry->SetAlwaysVisible(true);
		tbc::GeometryBase::BasicMaterialSettings material(
			lepra::vec3(0, 0, 0),
			lepra::vec3((float)(y%2), (float)(y/2%2), (float)(y/3%2)),
			lepra::vec3(0.1f, 0.1f, 0.1f),
			0.8f, 1.0f, true);
		_geometry->SetBasicMaterialSettings(material);
		graphics_id = agent_data.renderer_->AddGeometry(_geometry, uitbc::Renderer::kMatSingleColorSolid, uitbc::Renderer::kNoShadows);
		agent_data.AddBody("DynamicSphere", physics_id, _geometry, graphics_id);
	}
	lepra::xform camera_placement;
	lepra::vec3 camera_position(0, -floor_size, client_size*5);
	camera_placement.SetPosition(camera_position);
	agent_data.renderer_->SetCameraTransformation(camera_placement);
	return (test_ok);
}

bool CreateWorld() {
	bool test_ok = true;
	if (test_ok) {
		test_ok = CreateWorld(g_server);
	}
	for (int x = 0; test_ok && x < CLIENT_COUNT; ++x) {
		test_ok = CreateWorld(g_client[x]);
	}
	return (test_ok);
}

void DeleteWorld(AgentData& agent_data) {
	for (unsigned x = 0; x < agent_data.body_array_.size(); ++x) {
		AgentData::BodyInfo& body = agent_data.body_array_[x];
		agent_data.renderer_->RemoveGeometry(body.graphics_id_);
		delete (body.graphics_geometry_);
		body.graphics_geometry_ = 0;
		agent_data.physics_->DeleteBody(body.physics_id_);
	}
	delete (agent_data.physics_);
	agent_data.physics_ = 0;
}

void DeleteWorld() {
	for (int x = 0; x < CLIENT_COUNT; ++x) {
		DeleteWorld(g_client[x]);
	}
	DeleteWorld(g_server);
}

void PostDelayedMovement(const cure::ObjectPositionalData& data, int client_index, int frame_index, AgentData* from, AgentData* to) {
	const double current_time = g_absolute_time;
	const double _absolute_send_time = current_time + from->emulated_network_send_latency_frame_count_*PHYSICS_SECONDS_PER_FRAME + lepra::Random::Uniform(-from->emulated_network_send_jitter_frame_count_, from->emulated_network_send_jitter_frame_count_)*PHYSICS_SECONDS_PER_FRAME;
	/*if (from == &g_client[CLIENT_COUNT-1]) {
		logdebug("Client post", lepra::strutil::Format("Frame %i when server on %i, waiting for time %f, current time %f.", g_client[CLIENT_COUNT-1].current_physics_frame_index_, g_server.current_physics_frame_index_, _absolute_send_time, g_absolute_time));
	}*/
	for (std::list<DelayedNetworkSend>::iterator x = g_delayed_network_send_array.begin(); x != g_delayed_network_send_array.end(); ++x) {
		const DelayedNetworkSend& delayed_send = *x;
		if (delayed_send.absolute_send_time_ > _absolute_send_time) {
			g_delayed_network_send_array.insert(x, DelayedNetworkSend(_absolute_send_time, client_index, frame_index, data, from, to));
			return;	// TRICKY.
		}
	}
	g_delayed_network_send_array.push_back(DelayedNetworkSend(_absolute_send_time, client_index, frame_index, data, from, to));
	/*if (from == &g_client[CLIENT_COUNT-1]) {
		cure::MessageObjectPosition* message = (cure::MessageObjectPosition*)g_delayed_network_send_array.back().packet_->GetMessageAt(0);
		int _frame_index = 0;
		cure::ObjectPositionalData position;
		message->GetData(_frame_index, position);
		logdebug("Client post", lepra::strutil::Format("Frame %i when server on %i, waiting for time %f, current time %f.", _frame_index, g_server.current_physics_frame_index_, g_delayed_network_send_array.back().absolute_send_time_, g_absolute_time));
	}*/
}

void StoreFrameDiff(int agent_index, int frame_diff) {
	int& reference_frame_diff_index = g_client[agent_index].network_frame_latency_array_index_;
	g_client[agent_index].network_frame_latency_array_[reference_frame_diff_index++] = frame_diff;
	if (reference_frame_diff_index >= NETWORK_LATENCY_CALCULATION_ARRAY_SIZE) {
		reference_frame_diff_index = 0;
	}
}

void CalculateClientNetworkParameters(int agent_index, const int frame_diff) {
	AgentData& _client = g_client[agent_index];
	StoreFrameDiff(agent_index, frame_diff);
	const int median = lepra::Math::CalculateMedian<int>(_client.network_frame_latency_array_);
	const double deviation = lepra::Math::CalculateDeviation<double, int>(_client.network_frame_latency_array_, median);
	const double sliding_mean_lerp_factor = 0.5;
	_client.server_measured_network_latency_frame_count_ = lepra::Math::Lerp<double>(median, _client.server_measured_network_latency_frame_count_, sliding_mean_lerp_factor);
	_client.server_measured_network_jitter_frame_count_ = lepra::Math::Lerp<double>(deviation, _client.server_measured_network_jitter_frame_count_, sliding_mean_lerp_factor);
	//logdebug("Server measures latencies", lepra::strutil::Format("Client %i: latency frame count=%f, jitter frame count=%f.", agent_index, _client.server_measured_network_latency_frame_count_, _client.server_measured_network_jitter_frame_count_));
}

// A positive return value n means that data from the client to the server comes n frames too late on average (client needs to speed up).
// A negative return value n means that data from the client to the server comes n frames too early on average (client needs to slow down).
int CalculateNetworkLatencyFrameDiffCount(AgentData& client) {
	return ((int)(client.server_measured_network_latency_frame_count_ + client.server_measured_network_jitter_frame_count_));
}

void UpdateAssumedNetworkLatency(AgentData& agent_data, int adjust_frame_count) {
	// Adjust some of the oldest measured frame diffs.
	for (int x = 0; x < NETWORK_LATENCY_CALCULATION_ARRAY_SIZE; ++x) {
		agent_data.network_frame_latency_array_[x] -= adjust_frame_count;
	}
	agent_data.server_measured_network_latency_frame_count_ /= 2;
}

int SendClientStriveTimes(int agent_index) {
	// Don't really need to send, we just go through RAM.
	AgentData& _client = g_client[agent_index];
	_client.physics_tick_adjustment_frame_count_ = (int)PHYSICS_FPS;	// Spread over some time (=frames).
	int network_frame_diff_count = CalculateNetworkLatencyFrameDiffCount(_client);
	// Try to adjust us to be exactly one frame too early. Being too early is much better than being
	// late, since early network data can be buffered and used in upcomming frames, but late network
	// data is always thrown away.
	++network_frame_diff_count;
	if (network_frame_diff_count > 0) {
		// Speed up client physics by taking longer time steps.
		_client.physics_tick_adjustment_time_ = (float)network_frame_diff_count/_client.physics_tick_adjustment_frame_count_;
	} else {
		// Slow down client physics by taking shorter time steps.
		while (-network_frame_diff_count > _client.physics_tick_adjustment_frame_count_) {
			_client.physics_tick_adjustment_frame_count_ *= (-network_frame_diff_count/_client.physics_tick_adjustment_frame_count_ + 1);
		}
		_client.physics_tick_adjustment_time_ = (float)network_frame_diff_count/_client.physics_tick_adjustment_frame_count_;
	}
	UpdateAssumedNetworkLatency(_client, network_frame_diff_count);
	return (network_frame_diff_count);
}

void MaybeSendClientStriveTimes(int agent_index) {
	AgentData& _client = g_client[agent_index];
	const int network_frame_diff_count = CalculateNetworkLatencyFrameDiffCount(_client);
	if (network_frame_diff_count < -(int)PHYSICS_FPS/10 || network_frame_diff_count > 0) {	// TODO: don't hard-code lower (early) limit? Come up with something smarter?
		if (network_frame_diff_count < -(int)PHYSICS_FPS/2) {	// Dropped because too early.
			++_client.packet_usage_early_drop_count_;
		} else if (network_frame_diff_count > 0) {	// Dropped because too late (we've already passed that frame, we don't have time to re-simulate it).
			++_client.packet_usage_late_drop_count_;
		}
		++_client.strive_send_error_time_counter_;
		_client.ingore_strive_error_time_counter_ = 0;
		// Only send once every now and then.
		if ((network_frame_diff_count > 0 && _client.strive_send_error_time_counter_ > (int)PHYSICS_FPS*2)	||
			(network_frame_diff_count < 0 && _client.strive_send_error_time_counter_ > (int)PHYSICS_FPS*2)) {
			_client.strive_send_error_time_counter_ = 0;
			const int target_network_frame_diff_count = SendClientStriveTimes(agent_index);
			gNptLog.Warning("Server \"sending\" physics tick time adjustments. " + lepra::strutil::Format("Client %i: %+i frames (adjusted over %i frames).", agent_index, target_network_frame_diff_count, _client.physics_tick_adjustment_frame_count_));
		}
		/*else {
			str s;
			if (network_frame_diff_count < 0) {
				s = lepra::strutil::Format("Client %i: data arrives %i frames early on average.", agent_index, -network_frame_diff_count);
			} else {
				s = lepra::strutil::Format("Client %i: data arrives %i frames late on average.", agent_index, network_frame_diff_count);
			}
			logtrace("Server find client out of range", s);
		}*/
	} else {
		++_client.packet_usage_use_count_;
		static int x = 0;
		if (++x > 25) {
			x = 0;
			double late_drop_percent = 100.0*_client.packet_usage_late_drop_count_/(_client.packet_usage_late_drop_count_+_client.packet_usage_early_drop_count_+_client.packet_usage_use_count_);
			double early_drop_percent = 100.0*_client.packet_usage_early_drop_count_/(_client.packet_usage_late_drop_count_+_client.packet_usage_early_drop_count_+_client.packet_usage_use_count_);
			str s = lepra::strutil::Format("Client %i: data arrives %i frames early on average with a drop of %.1f %% (late) and %.1f %% (early).", agent_index, -network_frame_diff_count, late_drop_percent, early_drop_percent);
			log_volatile(gNptLog.Debug("Server find client in range "+s));
			s = lepra::strutil::Format("Client %i: up %.1f kB/s, down %.1f kB/s (%.2f FPS).", agent_index,
				(_client.network_agent_->GetSentByteCount()-_client.bandwidth_last_sent_)/1000.0/(g_absolute_time-_client.bandwidth_start_measure_time_),
				(_client.network_agent_->GetReceivedByteCount()-_client.bandwidth_last_received_)/1000.0/(g_absolute_time-_client.bandwidth_start_measure_time_), g_fps);
			log_volatile(gNptLog.Debug("Client bandwidth "+s));
			_client.bandwidth_last_sent_ = _client.network_agent_->GetSentByteCount();
			_client.bandwidth_last_received_ = _client.network_agent_->GetReceivedByteCount();
			_client.bandwidth_start_measure_time_ = g_absolute_time;
			/*for (int x = 0; x < NETWORK_LATENCY_CALCULATION_ARRAY_SIZE; ++x) {
				logdebug("LatencyArray", lepra::strutil::Format("%i", _client.network_frame_latency_array_[x]));
			}*/
		}
		++_client.ingore_strive_error_time_counter_;
		if (_client.ingore_strive_error_time_counter_ > (int)PHYSICS_FPS*2) {	// Reset send counter if we're mostly good.
			_client.packet_usage_late_drop_count_ = 0;
			_client.packet_usage_early_drop_count_ = 0;
			_client.packet_usage_use_count_ = 0;
			_client.ingore_strive_error_time_counter_ = 0;
			_client.strive_send_error_time_counter_ = 0;
		}
	}
}

void ClientSetMovement(AgentData& client_data, int client_index, int client_frame_index, const cure::ObjectPositionalData& data) {
	int& last_frame = client_data.body_array_[client_index+1].last_set_frame_index_;
	if (last_frame < client_frame_index) {
		last_frame = client_frame_index;
		//str s = lepra::strutil::Format("client %i at frame %i", client_index, client_frame_index);
		//logdebug("Client set pos of other client", s);
		tbc::PhysicsManager::BodyID physics_id = client_data.body_array_[client_index+1].physics_id_;
		client_data.physics_->SetBodyTransform(physics_id, data.position_.transformation_);
		client_data.physics_->SetBodyVelocity(physics_id, data.position_.velocity_);
		client_data.physics_->SetBodyForce(physics_id, data.position_.acceleration_);
		client_data.physics_->SetBodyAngularVelocity(physics_id, data.position_.angular_velocity_);
		client_data.physics_->SetBodyTorque(physics_id, data.position_.angular_acceleration_);
	}
}

void ClientReceive(int agent_index) {
	AgentData& _client_data = g_client[agent_index];
	cure::Packet* packet = _client_data.network_agent_->GetPacketFactory()->Allocate();
	cure::NetworkAgent::ReceiveStatus received = dynamic_cast<cure::NetworkClient*>(_client_data.network_agent_)->ReceiveNonBlocking(packet);
	if (received == cure::NetworkAgent::kReceiveOk) {
		cure::Packet::ParseResult parse_result;
		do {
			for (int i = 0; i < packet->GetMessageCount(); ++i) {
				cure::MessageObjectPosition* message = (cure::MessageObjectPosition*)packet->GetMessageAt(0);
				int _client_frame_index = -1;
				int _client_index = -1;
				_client_index = message->GetObjectId();
				_client_frame_index = message->GetFrameIndex();
				const cure::ObjectPositionalData& _data = message->GetPositionalData();
				ClientSetMovement(_client_data, _client_index, _client_frame_index, _data);
			}
			parse_result = packet->ParseMore();
		} while (parse_result == cure::Packet::kParseOk);
		deb_assert(parse_result == cure::Packet::kParseNoData);
	} else if (received != cure::NetworkAgent::kReceiveNoData) {
		log_volatile(gNptLog.Debug("Client networking error: problem receiving data!"));
	}
	g_server.network_agent_->GetPacketFactory()->Release(packet);
}

void ServerAdjustClientSimulationSpeed(int client_index, int frame_index) {
	str s;
	int _frame_diff = g_server.current_physics_frame_index_-frame_index;
	/*if (_frame_diff == 0) {
		s = lepra::strutil::Format("client %i right on time", client_index);
		//logdebug("Server receive", s);
	} else if (_frame_diff < 0) {
		s = lepra::strutil::Format("client %i is %i frames ahead of server", client_index, -_frame_diff);
		//logdebug("Server receive", s);
	} else {
		s = lepra::strutil::Format("client %i is %i frames after server (s=%i, c=%i - this is no good"), client_index, _frame_diff, g_server.current_physics_frame_index_, g_client[client_index].current_physics_frame_index_);
		//logdebug("Server receive", s);
	}*/
	CalculateClientNetworkParameters(client_index, _frame_diff);
	MaybeSendClientStriveTimes(client_index);
}

void ServerStoreClientMovement(int client_index, int frame_index, cure::ObjectPositionalData* data) {
	const int frame_offset = frame_index-g_server.current_physics_frame_index_;
	if (frame_offset >= 0 && frame_offset < NETWORK_POSITIONAL_PACKET_BUFFER_SIZE) {
		const int frame_cycle_index = (g_server.current_physics_frame_index_+frame_offset)%NETWORK_POSITIONAL_PACKET_BUFFER_SIZE;
		g_client[client_index].StorePositionalData(frame_cycle_index, data);
	} else {
		// This input data is already old or too much ahead!
		delete (data);
	}
}

cure::ObjectPositionalData* ServerPopClientMovement(int client_index, int server_frame_index) {
	const int frame_cycle_index = server_frame_index%NETWORK_POSITIONAL_PACKET_BUFFER_SIZE;
	cure::ObjectPositionalData* _data = g_client[client_index].positional_buffer_[frame_cycle_index];
	g_client[client_index].positional_buffer_[frame_cycle_index] = 0;
	return (_data);
}

void ServerAdjustClientMovement(int client_index, const cure::ObjectPositionalData& data) {
	tbc::PhysicsManager::BodyID physics_id = g_server.body_array_[client_index+1].physics_id_;
	g_server.physics_->SetBodyTransform(physics_id, data.position_.transformation_);
	g_server.physics_->SetBodyVelocity(physics_id, data.position_.velocity_);
	g_server.physics_->SetBodyForce(physics_id, data.position_.acceleration_);
	g_server.physics_->SetBodyAngularVelocity(physics_id, data.position_.angular_velocity_);
	g_server.physics_->SetBodyTorque(physics_id, data.position_.angular_acceleration_);
}

void ServerHandleNetworkInput(int frame_index) {
	for (int x = 0; x < CLIENT_COUNT; ++x) {
		cure::ObjectPositionalData* _data = ServerPopClientMovement(x, frame_index);
		if (_data) {
			ServerAdjustClientMovement(x, *_data);
		}
		delete (_data);
	}
}

void ServerBroadcast(int client_index, int client_frame_index, const cure::ObjectPositionalData& data) {
	for (int x = 0; x < CLIENT_COUNT; ++x) {
		if (x != client_index) {
			PostDelayedMovement(data, client_index, client_frame_index, &g_server, &g_client[x]);
		}
	}
}

int GetClientIndexFromNetworkId(lepra::uint32 network_client_id) {
	for (int x = 0; x < CLIENT_COUNT; ++x) {
		if (g_client[x].client_id_ == network_client_id) {
			return (x);
		}
	}
	return (-100000000);
}

void ServerReceive() {
	cure::Packet* packet = g_server.network_agent_->GetPacketFactory()->Allocate();
	cure::NetworkAgent::ReceiveStatus received = cure::NetworkAgent::kReceiveOk;
	while (received != cure::NetworkAgent::kReceiveNoData) {
		lepra::uint32 id;
		received = dynamic_cast<cure::NetworkServer*>(g_server.network_agent_)->ReceiveFirstPacket(packet, id);
		int x = GetClientIndexFromNetworkId(id);
		if (received == cure::NetworkAgent::kReceiveOk) {
			//gNptLog.Debugf("Server received data from client %i!", x);
			cure::Packet::ParseResult parse_result;
			do {
				for (int i = 0; i < packet->GetMessageCount(); ++i) {
					cure::MessageObjectPosition* message = (cure::MessageObjectPosition*)packet->GetMessageAt(i);
					int _client_index = -1;
					int _client_frame_index = -1;
					cure::ObjectPositionalData* _data = new cure::ObjectPositionalData();
					_client_index = message->GetObjectId();
					_client_frame_index = message->GetFrameIndex();
					_data->CopyData(&message->GetPositionalData());
					ServerAdjustClientSimulationSpeed(x, _client_frame_index);

					// Pass on to other clients.
					ServerBroadcast(x, _client_frame_index, *_data);

					ServerStoreClientMovement(x, _client_frame_index, _data);
				}
				parse_result = packet->ParseMore();
			} while (parse_result == cure::Packet::kParseOk);
			deb_assert(parse_result == cure::Packet::kParseNoData);
		} else if (received != cure::NetworkAgent::kReceiveNoData) {
			gNptLog.Error("Server networking error: problem receiving data!");
		}
	}
	g_server.network_agent_->GetPacketFactory()->Release(packet);
}

void ClientSendSelf(int client_index) {
	AgentData& _from = g_client[client_index];
	cure::ObjectPositionalData _data;
	_from.physics_->GetBodyTransform(_from.body_array_[client_index+1].physics_id_, _data.position_.transformation_);
	_from.physics_->GetBodyVelocity(_from.body_array_[client_index+1].physics_id_, _data.position_.velocity_);
	_from.physics_->GetBodyForce(_from.body_array_[client_index+1].physics_id_, _data.position_.acceleration_);
	_from.physics_->GetBodyAngularVelocity(_from.body_array_[client_index+1].physics_id_, _data.position_.angular_velocity_);
	_from.physics_->GetBodyTorque(_from.body_array_[client_index+1].physics_id_, _data.position_.angular_acceleration_);
	if (_from.network_force_send_ || _data.GetScaledDifference(&_from.client_last_send_positional_data_) > 10.0f) {
		_from.network_force_send_ = false;
		_from.client_last_send_positional_data_.CopyData(&_data);
		PostDelayedMovement(_data, client_index, _from.current_physics_frame_index_, &_from, &g_server);
	}
}

bool TickEmulatedLatencyNetwork() {
	bool server_flush = false;
	bool ok = true;
	while (ok && g_delayed_network_send_array.size() > 0) {
		double time_until_send = g_delayed_network_send_array.front().absolute_send_time_-g_absolute_time;
		if (time_until_send <= 0) {
			const DelayedNetworkSend transmit = g_delayed_network_send_array.front();
			g_delayed_network_send_array.pop_front();
			if (transmit.from_->client_id_ == 0) {
				server_flush = true;
				cure::NetworkServer* network_server = dynamic_cast<cure::NetworkServer*>(transmit.to_->network_agent_);
				if (network_server) {
					ok = network_server->PlaceInSendBuffer(false, transmit.packet_, transmit.to_->client_id_);
					if (!ok) {
						gNptLog.Error("Could not send to remote client!");
					}
				}
			} else {
				/*if (transmit.from_ == &g_client[CLIENT_COUNT-1]) {
					logdebug("Client send", lepra::strutil::Format("Frame %i when server on %i, waited for time %f, current time %f.", g_client[CLIENT_COUNT-1].current_physics_frame_index_, g_server.current_physics_frame_index_, transmit.absolute_send_time_, g_absolute_time));
				}*/
				cure::NetworkClient* network_client = dynamic_cast<cure::NetworkClient*>(transmit.from_->network_agent_);
				ok = network_client->PlaceInSendBuffer(false, network_client->GetSocket(), transmit.packet_);
				if (!ok) {
					gNptLog.Error("Could not send to remote server!");
				}
			}
		} else {
			break;
		}
	}
	if (ok && server_flush) {
		ok = g_server.network_agent_->SendAll();
	}
	for (int x = 0; ok && x < CLIENT_COUNT; ++x) {
		ok = g_client[x].network_agent_->SendAll();
	}
	return (ok);
}

bool OpenWindow(AgentData& agent_data, const str& caption) {
	str _context;
	bool test_ok = true;
	if (test_ok) {
		_context = "create display manager";
		agent_data.display_ = uilepra::DisplayManager::CreateDisplayManager(uilepra::DisplayManager::kOpenglContext);
		test_ok = (agent_data.display_ != 0);
	}
	uilepra::DisplayMode display_mode;
	/*if (test_ok) {
		_context = "find display mode";
		test_ok = agent_data.display_->FindDisplayMode(display_mode, 640, 480, 32);
	}*/
	if (test_ok) {
		_context = "open screen";
		test_ok = agent_data.display_->OpenScreen(display_mode, uilepra::DisplayManager::kWindowed, uilepra::DisplayManager::kOrientationAllowAny);
	}
	if (test_ok) {
		agent_data.screen_ = new lepra::Canvas(agent_data.display_->GetWidth(), agent_data.display_->GetHeight(), lepra::Canvas::IntToBitDepth(agent_data.display_->GetBitDepth()));
		agent_data.screen_->SetBuffer(0);
		agent_data.renderer_ = uitbc::RendererFactory::Create(uilepra::DisplayManager::kOpenglContext, agent_data.screen_);
		agent_data.renderer_->SetClearColor(lepra::Color(128, 128, 128));
		agent_data.display_->SetCaption(caption);
		/*test_ok =*/ agent_data.display_->SetVSyncEnabled(false);
	}
	if (test_ok) {
		agent_data.input_ = uilepra::InputManager::CreateInputManager(agent_data.display_);
		agent_data.input_->AddKeyCodeInputObserver(&agent_data);
	}
	if (test_ok) {
		agent_data.active_ = true;
	}
	ReportTestResult(gNptLog, "OpenWindow", _context, test_ok);
	return (test_ok);
}

bool OpenWindows() {
	str _context;
	bool test_ok = true;
	if (test_ok) {
		_context = "create server window";
		test_ok = OpenWindow(g_server, "Server");
	}
	for (int x = 0; test_ok && x < CLIENT_COUNT; ++x) {
		_context = lepra::strutil::Format("create client window %i", x);
		test_ok = OpenWindow(g_client[x], lepra::strutil::Format("Client %i", x));
	}
	ReportTestResult(gNptLog, "OpenWindows", _context, test_ok);
	return (test_ok);
}

void CloseWindow(AgentData& agent_data) {
	delete (agent_data.input_);
	agent_data.input_ = 0;
	delete (agent_data.renderer_);
	agent_data.renderer_ = 0;
	delete (agent_data.screen_);
	agent_data.screen_ = 0;
	delete (agent_data.display_);
	agent_data.display_ = 0;
}

void CloseWindows() {
	CloseWindow(g_server);
	for (int x = 0; x < CLIENT_COUNT; ++x) {
		CloseWindow(g_client[x]);
	}
}

bool UpdateGraphicsObjects(int agent_index) {
	AgentData* _agent_data;
	if (agent_index < 0) {
		_agent_data = &g_server;
	} else {
		_agent_data = &g_client[agent_index];
	}

	// Below is some data used when lerping (optimized by pulling out of the inner loop).
	const float close_after_time = 0.5f;	// Unit is seconds. In this many lerps, we want to be close to the
	const float closeness = 5e-9f;	// Some sort of lerp thingie.
	const float lerp_time = ::pow(closeness, 1/(close_after_time*PHYSICS_FPS));

	bool test_ok = true;
	lepra::xform transformation;
	for (int x = 0; x < (int)_agent_data->body_array_.size(); ++x) {
		AgentData::BodyInfo& body = _agent_data->body_array_[x];
		_agent_data->physics_->GetBodyTransform(body.physics_id_, transformation);
		// Check if we "slide" to the correct position.
		if (agent_index == -1 || x == 0 || x-1 == agent_index) {
			// Server: set all without sliding. Client: set box and self without sliding.
			body.graphics_geometry_->SetTransformation(transformation);
		} else {
			// Client: set other with sliding (moving average).
			body.graphics_position_ = lepra::Math::Lerp<lepra::vec3, float>(transformation.GetPosition(), body.graphics_position_, lerp_time);
			transformation.SetPosition(body.graphics_position_);
			body.graphics_geometry_->SetTransformation(transformation);
		}
	}
	return (test_ok);
}

void ClientHandleUserInput(int client_index) {
	//uilepra::InputManager* input_manager = uilepra::InputManager::GetInputManager();
	//input_manager->PollEvents();
	AgentData& _agent_data = g_client[client_index];
	const float force = 800;
	lepra::vec3 force_vector[] = { lepra::vec3(0, force, 0), lepra::vec3(0, -force, 0), lepra::vec3(-force, 0, 0), lepra::vec3(force, 0, 0), };
	tbc::PhysicsManager::BodyID physics_id = _agent_data.body_array_[client_index+1].physics_id_;
	for (int key = 0; key < 4; ++key) {
		if (_agent_data.move_keys_&(1<<key)) {
			_agent_data.physics_->AddForce(physics_id, force_vector[key]);
			_agent_data.network_force_send_ = true;
		}
	}
}

void StepPhysics(int agent_index, int step_count) {
	AgentData* _agent_data;
	if (agent_index < 0) {
		_agent_data = &g_server;
	} else {
		_agent_data = &g_client[agent_index];
	}
	const float this_step_time = PHYSICS_SECONDS_PER_FRAME*step_count;
	_agent_data->physics_->PreSteps();
	_agent_data->physics_->StepFast(this_step_time, true);
	_agent_data->physics_->PostSteps();
	_agent_data->current_physics_frame_index_ += step_count;
	_agent_data->tick_time_modulo_ -= this_step_time;

	// Adjust some if needed (server tells client to adjust time so that it will receive network data just in time).
	_agent_data->tick_time_modulo_ += _agent_data->physics_tick_adjustment_time_*PHYSICS_SECONDS_PER_FRAME;
	if (_agent_data->physics_tick_adjustment_frame_count_ > 0) {
		_agent_data->physics_tick_adjustment_frame_count_ -= step_count;
	} else {
		_agent_data->physics_tick_adjustment_time_ = 0;
	}

	// Simulate basic physics on the network output ghost(s). The N.O.G. contains the last sent information.
	if (agent_index < 0) {
		// TODO
	} else {
		cure::ObjectPositionalData& _data = _agent_data->client_last_send_positional_data_;
		// TODO: improve. For now we just apply velocity to position.
		_data.position_.transformation_.GetPosition().Add(_data.position_.velocity_*this_step_time);
	}
}

bool Tick(float tick_time, int agent_index) {
	AgentData* _agent_data;
	if (agent_index < 0) {
		_agent_data = &g_server;
	} else {
		_agent_data = &g_client[agent_index];
	}

	bool test_ok = true;
	_agent_data->tick_time_modulo_ += tick_time;
	const int _step_count = (int)(_agent_data->tick_time_modulo_*PHYSICS_FPS);
	if (_step_count >= 1 && _agent_data->active_) {
		if (test_ok) {
			bool window_open = _agent_data->display_->Activate();
			if (!window_open) {
				_agent_data->active_ = false;
			}
		}
		if (test_ok) {
			_agent_data->renderer_->Clear();
			if (agent_index < 0) {
				ServerReceive();
				for (int x = 0; x < _step_count; ++x) {
					ServerHandleNetworkInput(g_server.current_physics_frame_index_+x);
				}
			} else {
				ClientReceive(agent_index);
				for (int x = 0; x < _step_count; ++x) {
					ClientHandleUserInput(agent_index);
				}
				ClientSendSelf(agent_index);
			}
			StepPhysics(agent_index, _step_count);
			test_ok = UpdateGraphicsObjects(agent_index);
		}
		if (test_ok) {
			_agent_data->renderer_->RenderScene();
			bool window_open = _agent_data->display_->UpdateScreen();
			if (!window_open) {
				_agent_data->active_ = false;
			}
		}
		if (test_ok) {
			_agent_data->display_->Deactivate();
		}
		if (!_agent_data->active_) {
			_agent_data->display_->CloseScreen();
		}
	}
	return (test_ok);
}

bool Tick(float tick_time, bool& quit) {
	uilepra::Core::ProcessMessages();
	quit = (lepra::SystemManager::GetQuitRequest() != 0);
	if (!quit) {
		quit = !g_server.active_;
	}
	bool test_ok = true;
	for (int x = 0; !quit && test_ok && x < CLIENT_COUNT; ++x) {
		test_ok = Tick(tick_time, x);
	}
	if (!quit && test_ok) {
		test_ok = TickEmulatedLatencyNetwork();
	}
	if (!quit && test_ok) {
		test_ok = Tick(tick_time, -1);
	}
	g_absolute_time += tick_time;
	return (test_ok);
}

bool TestPrototypeNetworkPhysics() {
	bool test_ok = true;

	lepra::SystemManager::AddQuitRequest(-1);

	if (test_ok) {
		test_ok = NetworkLoginClients();
	}
	if (test_ok) {
		test_ok = OpenWindows();
	}
	if (test_ok) {
		test_ok = CreateWorld();
	}

	bool _quit = false;
	lepra::HiResTimer irl_timer(false);
	lepra::HiResTimer timer(false);
	while (!_quit && test_ok && irl_timer.GetTimeDiff() < 20.0) {
		float step_time = (float)timer.GetTimeDiff();
		if (step_time > 0) {
			g_fps = 1/step_time;
		}
		if (step_time > 0.5f) {
			step_time = 0.5f;
		}
		test_ok = Tick(step_time, _quit);
		//test_ok = Tick(pLogAccount, (float)PHYSICS_SECONDS_PER_FRAME, _quit);
		//test_ok = Tick(pLogAccount, (float)1.0, _quit);
		lepra::Thread::Sleep(0.001);
		//lepra::Thread::YieldCpu();
		irl_timer.UpdateTimer();
		timer.ClearTimeDiff();
		timer.UpdateTimer();
	}

	DeleteWorld();
	CloseWindows();
	NetworkShutdown();
	uilepra::Core::ProcessMessages();
	lepra::Thread::Sleep(0.05);
	uilepra::Core::ProcessMessages();

	if (_quit) {
		gNptLog.Warning("user requested termination");
	} else if (!test_ok) {
		ReportTestResult(gNptLog, "ExitStatus", "terminating due to error", test_ok);
	}

	return (test_ok);
}

#endif // !CURE_TEST_WITHOUT_UI
