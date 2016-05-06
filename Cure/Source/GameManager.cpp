
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/gamemanager.h"
#include "../../lepra/include/number.h"
#include "../../lepra/include/systemmanager.h"
#include "../../tbc/include/physicsmanager.h"
#include "../../tbc/include/physicsmanagerfactory.h"
#include "../../tbc/include/physicsspawner.h"
#include "../include/consolemanager.h"
#include "../include/contextmanager.h"
#include "../include/contextobject.h"
#include "../include/gameticker.h"
#include "../include/networkagent.h"
#include "../include/resourcemanager.h"
#include "../include/runtimevariable.h"
#include "../include/runtimevariablename.h"
#include "../include/spawner.h"
#include "../include/terrainmanager.h"
#include "../include/timemanager.h"



namespace cure {



GameManager::GameManager(const TimeManager* time, RuntimeVariableScope* variable_scope, ResourceManager* resource_manager):
	is_thread_safe_(true),
	variable_scope_(variable_scope),
	resource_(resource_manager),
	network_(0),
	ticker_(0),
	time_(time),
	context_(0),
	terrain_(0),//new TerrainManager(pResourceManager)),
	console_(0) {
	context_ = new ContextManager(this);
}

GameManager::~GameManager() {
	delete (console_);
	console_ = 0;
	delete (context_);
	context_ = 0;

	// Free after killing all game objects.
	resource_->ForceFreeCache();
	resource_->ForceFreeCache();

	time_ = 0;	// Not owned resource.
	ticker_ = 0;	// Not owned resource.
	delete (terrain_);
	terrain_ = 0;
	delete (network_);
	network_ = 0;
	resource_ = 0;
	delete (variable_scope_);
	variable_scope_ = 0;

	while (lock_.IsOwner()) {
		lock_.Release();
	}
}

const GameTicker* GameManager::GetTicker() const {
	return ticker_;
}

void GameManager::SetTicker(const GameTicker* ticker) {
	ticker_ = ticker;
}



bool GameManager::IsPrimaryManager() const {
	return true;
}

bool GameManager::BeginTick() {
	LEPRA_MEASURE_SCOPE(BeginTick);

	bool performance_text;
	v_get(performance_text, =, GetVariableScope(), kRtvarPerformanceTextEnable, false);
	double _report_interval;
	v_get(_report_interval, =, GetVariableScope(), kRtvarPerformanceTextInterval, 1.0);
	UpdateReportPerformance(performance_text, _report_interval);

	{
		//LEPRA_MEASURE_SCOPE(AcquireTickLock);
		GetTickLock()->Acquire();	// Lock for physics propagation, user input, etc.
	}

	{
		//LEPRA_MEASURE_SCOPE(NetworkAndInput);

		//time_->Tick();

		// Sorts up incoming network data; adds/removes objects (for instance via remote create/delete).
		// On kUi-based managers we handle user input here as well.
		TickInput();
	}

	{
		LEPRA_MEASURE_SCOPE(PhysicsPropagation);
		ScriptPhysicsTick();
	}

	is_thread_safe_ = false;

	return true;
}

void GameManager::PreEndTick() {
	is_thread_safe_ = true;
	GetTickLock()->Release();
}

bool GameManager::EndTick() {
	{
		//LEPRA_MEASURE_SCOPE(NetworkSend);

		// Sends network packets. Among other things, movement of locally-controlled objects are sent.
		// This must be run after input processing, otherwise input-physics-output loop won't have
		// the desired effect.
		TickNetworkOutput();
	}

	if (GetConsoleManager()) {
		GetConsoleManager()->ExecuteYieldCommand();
	}

	return true;
}

bool GameManager::TickNetworkOutput() {
	context_->HandleAttributeSend();
	if (network_) {
		return (network_->SendAll());
	}
	return true;
}

Lock* GameManager::GetTickLock() const {
	return &lock_;
}



RuntimeVariableScope* GameManager::GetVariableScope() const {
	return (variable_scope_);
}

void GameManager::SetVariableScope(RuntimeVariableScope* scope) {
	variable_scope_ = scope;
}

ResourceManager* GameManager::GetResourceManager() const {
	return (resource_);
}

ContextManager* GameManager::GetContext() const {
	return (context_);
}

const TimeManager* GameManager::GetTimeManager() const {
	return (time_);
}

tbc::PhysicsManager* GameManager::GetPhysicsManager() const {
	return ticker_->GetPhysicsManager(is_thread_safe_);
}

ConsoleManager* GameManager::GetConsoleManager() const {
	return (console_);
}

void GameManager::SetConsoleManager(ConsoleManager* console) {
	console_ = console;
}



void GameManager::MicroTick(float time_delta) {
	ScopeLock lock(GetTickLock());
	context_->MicroTick(time_delta);
}

void GameManager::PostPhysicsTick() {
	context_->HandleIdledBodies();
	context_->HandlePhysicsSend();
	HandleWorldBoundaries();
}



bool GameManager::IsObjectRelevant(const vec3& position, float distance) const {
	(void)position;
	(void)distance;
	return true;
}

ContextObject* GameManager::CreateContextObject(const str& class_id, NetworkObjectType network_type, GameObjectId instance_id) {
	ContextObject* _object = CreateContextObject(class_id);
	AddContextObject(_object, network_type, instance_id);
	return (_object);
}

void GameManager::DeleteContextObject(GameObjectId instance_id) {
	context_->DeleteObject(instance_id);
}

void GameManager::AddContextObject(ContextObject* object, NetworkObjectType network_type, GameObjectId instance_id) {
	object->SetNetworkObjectType(network_type);
	if (instance_id) {
		object->SetInstanceId(instance_id);
	} else {
		object->SetInstanceId(GetContext()->AllocateGameObjectId(network_type));
	}
	object->SetManager(GetContext());
	GetContext()->AddObject(object);
}

ContextObject* GameManager::CreateLogicHandler(const str&) {
	return 0;
}

Spawner* GameManager::GetAvatarSpawner(GameObjectId level_id) const {
	ContextObject* level = GetContext()->GetObject(level_id);
	if (!level) {
		return 0;
	}
	const ContextObject::Array& child_array = level->GetChildArray();
	ContextObject::Array::const_iterator x = child_array.begin();
	for (; x != child_array.end(); ++x) {
		if ((*x)->GetClassId() != "Spawner") {
			continue;
		}
		Spawner* spawner = (Spawner*)*x;
		const tbc::PhysicsSpawner* spawn_shape = spawner->GetSpawner();
		if (spawn_shape->GetNumber() == 0) {
			return spawner;
		}
	}
	return 0;
}

bool GameManager::IsUiMoveForbidden(GameObjectId) const {
	return false;	// Non-kUi implementors need not bother.
}

void GameManager::OnStopped(ContextObject* object, tbc::PhysicsManager::BodyID body_id) {
#ifdef LEPRA_DEBUG
	const unsigned root_index = 0;
	deb_assert(object->GetStructureGeometry(root_index));
	deb_assert(object->GetStructureGeometry(root_index)->GetBodyId() == body_id);
#endif // Debug / !Debug
	(void)body_id;

	if (object->GetNetworkObjectType() == kNetworkObjectLocallyControlled) {
		log_volatile(log_.Debugf("Object %u/%s stopped, sending position.", object->GetInstanceId(), object->GetClassId().c_str()));
		GetContext()->AddPhysicsSenderObject(object);
	}
}



bool GameManager::ValidateVariable(int security_level, const str& variable, str& value) const {
	if (security_level < 1 && (variable == kRtvarPhysicsFps ||
		variable == kRtvarPhysicsRtr ||
		variable == kRtvarPhysicsHalt)) {
		log_.Warning("You're not authorized to change this variable.");
		return false;
	}
	if (variable == kRtvarPhysicsFps || variable == kRtvarPhysicsMicrosteps) {
		int _value = 0;
		if (!strutil::StringToInt(value, _value)) return false;
		_value = (variable == kRtvarPhysicsFps)? Math::Clamp(_value, 5, 10000) : Math::Clamp(_value, 1, 10);
		value = strutil::IntToString(_value, 10);
	} else if (variable == kRtvarPhysicsRtr) {
		double _value = 0;
		if (!strutil::StringToDouble(value, _value)) return false;
		_value = Math::Clamp(_value, 0.01, 4.0);
		strutil::DoubleToString(_value, 4, value);
	} else if (variable == kRtvarPhysicsHalt) {
		bool _value = false;
		if (!strutil::StringToBool(value, _value)) return false;
		value = strutil::BoolToString(_value);
	}
	return true;
}



void GameManager::UpdateReportPerformance(bool report, double report_interval) {
	performance_report_timer_.UpdateTimer();
	const double time_diff = performance_report_timer_.GetTimeDiff();
	if (time_diff >= report_interval) {
		performance_report_timer_.ClearTimeDiff();

		if (network_ && network_->IsOpen()) {
			send_bandwidth_.Append(time_diff, 0, network_->GetSentByteCount());
			receive_bandwidth_.Append(time_diff, 0, network_->GetReceivedByteCount());
			if (report) {
				log_.Performancef("Network bandwith. Up: %sB/s (peak %sB/s). Down: %sB/s (peak %sB/s).",
					Number::ConvertToPostfixNumber(send_bandwidth_.GetLast(), 2).c_str(),
					Number::ConvertToPostfixNumber(send_bandwidth_.GetMaximum(), 2).c_str(),
					Number::ConvertToPostfixNumber(receive_bandwidth_.GetLast(), 2).c_str(),
					Number::ConvertToPostfixNumber(receive_bandwidth_.GetMaximum(), 2).c_str());
			}
		} else {
			send_bandwidth_.Clear();
			receive_bandwidth_.Clear();
		}

		if (report) {
			const ScopePerformanceData::NodeArray roots = ScopePerformanceData::GetRoots();
			ReportPerformance(ScopePerformanceData::GetRoots(), 0);
		}
	}
}

void GameManager::ClearPerformanceData() {
	ScopeLock lock(&lock_);

	send_bandwidth_.Clear();
	receive_bandwidth_.Clear();

	ScopePerformanceData::ResetAll();
}

void GameManager::GetBandwidthData(BandwidthData& sent, BandwidthData& received) {
	sent = send_bandwidth_;
	received = receive_bandwidth_;
}



void GameManager::OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	ContextObject* object1 = GetContext()->GetObject(trigger_listener_id);
	if (object1) {
		ContextObject* object2 = GetContext()->GetObject(other_object_id);
		if (object2) {
			object1->OnTrigger(trigger, object2, body_id, position, normal);
		}
	}
}

void GameManager::OnForceApplied(int object_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque, const vec3& position, const vec3& relative_velocity) {
	ContextObject* object1 = GetContext()->GetObject(object_id);
	if (object1) {
		ContextObject* object2 = GetContext()->GetObject(other_object_id);
		if (object2) {
			object1->OnForceApplied(object2, body_id, other_body_id, force, torque, position, relative_velocity);
		}
	}
}



NetworkAgent* GameManager::GetNetworkAgent() const {
	return (network_);
}

void GameManager::SetNetworkAgent(NetworkAgent* network) {
	delete (network_);
	network_ = network;
}




void GameManager::ScriptPhysicsTick() {
	GetTickLock()->Release();
	ScopeLock phys_lock(((GameTicker*)GetTicker())->GetPhysicsLock());
	GetTickLock()->Acquire();

	//if (time_->GetAffordedPhysicsStepCount() > 0)
	{
		context_->HandlePostKill();
		context_->TickPhysics();
	}
}



void GameManager::ReportPerformance(const ScopePerformanceData::NodeArray& nodes, int recursion) {
	const str indent = str(recursion*3, ' ');
	ScopePerformanceData::NodeArray::const_iterator x = nodes.begin();
	for (; x != nodes.end(); ++x) {
		const ScopePerformanceData* node = *x;
		str name = strutil::Split(node->GetName(), ";")[0];
		log_.Performancef((indent+name+" Min: %ss, last: %ss, savg: %ss, max: %ss.").c_str(),
			Number::ConvertToPostfixNumber(node->GetMinimum(), 2).c_str(),
			Number::ConvertToPostfixNumber(node->GetLast(), 2).c_str(),
			Number::ConvertToPostfixNumber(node->GetSlidingAverage(), 2).c_str(),
			Number::ConvertToPostfixNumber(node->GetMaximum(), 2).c_str());
		ReportPerformance(node->GetChildren(), recursion+1);
	}
}



bool GameManager::IsThreadSafe() const {
	return (is_thread_safe_);
}

void GameManager::HandleWorldBoundaries() {
}



loginstance(kGame, GameManager);



}
