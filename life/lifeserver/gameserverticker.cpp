
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "gameserverticker.h"
#include "../../cure/include/contextobjectattribute.h"
#include "../../cure/include/resourcemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/systemmanager.h"
#include "../consolemanager.h"
#include "../lifeapplication.h"
#include "../lifestring.h"
#include "gameservermanager.h"
#include "masterserverconnection.h"
#include "rtvar.h"
#include "serverconsolemanager.h"



namespace life {



GameServerTicker::GameServerTicker(cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(physics_radius, physics_levels, physics_sensitivity),
	resource_manager_(resource_manager),
	game_manager_(0),
	master_connection_(0) {
	v_set(cure::GetSettings(), kRtvarApplicationAutoexitonemptyserver, false);
	v_set(cure::GetSettings(), kRtvarNetworkEnableopenserver, true);
	v_set(cure::GetSettings(), kRtvarNetworkServeraddress, _("0.0.0.0:16650"));

	cure::ContextObjectAttribute::SetCreator(&CreateObjectAttribute);

	resource_manager->InitDefault();

	cure::RuntimeVariableScope* variable_scope = new cure::RuntimeVariableScope(cure::GetSettings());
	game_manager_ = new GameServerManager(GetTimeManager(), variable_scope, resource_manager);
	game_manager_->SetTicker(this);
}

GameServerTicker::~GameServerTicker() {
	delete (game_manager_);
	game_manager_ = 0;

	{
		ConsoleManager console(resource_manager_, 0, cure::GetSettings(), 0, 0);
		console.InitCommands();
		console.ExecuteCommand("save-system-config-file 0 " + Application::GetIoFile("ServerBase", "lsh"));
	}

	resource_manager_ = 0;

	master_connection_->SetSocketInfo(0, -1);
	delete master_connection_;
	master_connection_ = 0;
}

void GameServerTicker::StartConsole(InteractiveConsoleLogListener* console_logger) {
	ConsoleManager console(resource_manager_, 0, cure::GetSettings(), 0, 0);
	console.InitCommands();
	console.ExecuteCommand("execute-file -i ServerDefault.lsh");
	console.ExecuteCommand("execute-file -i " + Application::GetIoFile("ServerBase", "lsh"));

	game_manager_->StartConsole(console_logger, new StdioConsolePrompt);
}

void GameServerTicker::SetMasterServerConnection(MasterServerConnection* connection) {
	delete master_connection_;
	master_connection_ = connection;
}



bool GameServerTicker::Initialize() {
	str server_address;
	v_get(server_address, =, cure::GetSettings(), kRtvarNetworkServeraddress, "localhost:16650");
	return game_manager_->Initialize(master_connection_, server_address);
}

bool GameServerTicker::Tick() {
	master_connection_->Tick();

	GetTimeManager()->Tick();

	bool ok = game_manager_->BeginTick();
	if (ok) {
		StartPhysicsTick();
		game_manager_->PreEndTick();
		WaitPhysicsTick();
		ok = game_manager_->EndTick();
	}

	resource_manager_->Tick();

	bool auto_shutdown;
	v_get(auto_shutdown, =, cure::GetSettings(), kRtvarApplicationAutoexitonemptyserver, false);
	if (auto_shutdown) {
		static size_t max_login_count = 0;
		size_t user_count = game_manager_->ListUsers().size();
		max_login_count = (user_count > max_login_count)? user_count : max_login_count;
		if (max_login_count > 0 && user_count == 0) {
			log_.Warning("Server automatically shuts down since rtvar active and all users now logged off.");
			SystemManager::AddQuitRequest(+1);
		}
	}

	return (ok);
}

void GameServerTicker::PollRoundTrip() {
	game_manager_->TickInput();
}

float GameServerTicker::GetTickTimeReduction() const {
	return GetTimeManager()->GetTickLoopTimeReduction();
}

float GameServerTicker::GetPowerSaveAmount() const {
	return (game_manager_->GetPowerSaveAmount());
}



void GameServerTicker::WillMicroTick(float time_delta) {
	game_manager_->MicroTick(time_delta);
}

void GameServerTicker::DidPhysicsTick() {
	game_manager_->PostPhysicsTick();
}



void GameServerTicker::OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	game_manager_->OnTrigger(trigger, trigger_listener_id, other_object_id, body_id, position, normal);
}

void GameServerTicker::OnForceApplied(int object_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque, const vec3& position, const vec3& relative_velocity) {
	game_manager_->OnForceApplied(object_id, other_object_id, body_id, other_body_id, force, torque, position, relative_velocity);
}



loginstance(kGame, GameServerTicker);



}
