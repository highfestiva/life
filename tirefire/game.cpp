
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "game.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"
#include "../cure/include/floatattribute.h"
#include "../cure/include/runtimevariable.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/random.h"
#include "../tbc/include/physicsengine.h"
#include "../uicure/include/uicollisionsoundmanager.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiprops.h"
#include "../uicure/include/uiruntimevariablename.h"
#include "../uicure/include/uisound.h"
#include "goal.h"
#include "vehicle.h"
#include "vehicleelevator.h"
#include "level.h"
#include "spawner.h"
#include "vehicleai.h"



#define VEHICLE_START	vec3(-90, 0, 13)
#define CAM_OFFSET	vec3(+2, -10, +3)



namespace tirefire {



Game::Game(UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager):
	cure::GameTicker(400, 4, 3),
	cure::GameManager(cure::GameTicker::GetTimeManager(), variable_scope, resource_manager),
	ui_manager_(ui_manager),
	collision_sound_manager_(0),
	light_id_(uitbc::Renderer::INVALID_LIGHT),
	level_(0),
	flyby_mode_(kFlybyInactive),
	fly_by_time_(0),
	vehicle_(0),
	vehicle_cam_pos_(-80, -40, 3),
	goal_(0),
	vehicle_ai_(0),
	flip_render_side_(0),
	flip_render_side_factor_(0),
	score_(0),
	score_counting_enabled_(false) {
	SetTicker(this);

	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->AddSound("explosion",	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	collision_sound_manager_->AddSound("small_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(0.1f, 0.2f, 0));
	collision_sound_manager_->AddSound("big_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	collision_sound_manager_->AddSound("rubber",		UiCure::CollisionSoundManager::SoundResourceInfo(0.4f, 0.1f, 0));
	collision_sound_manager_->AddSound("wood",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
}

Game::~Game() {
	ui_manager_->GetRenderer()->RemoveLight(light_id_);

	delete collision_sound_manager_;
	collision_sound_manager_ = 0;
	ui_manager_ = 0;
	SetVariableScope(0);	// Not owned by us.

	goal_ = 0;
	vehicle_ai_ = 0;
}

UiCure::GameUiManager* Game::GetUiManager() const {
	return ui_manager_;
}

const str& Game::GetLevelName() const {
	return level_name_;
}

bool Game::SetLevelName(const str& level) {
	level_name_ = level;
	if (!vehicle_) {
		return RestartLevel();
	}
	goal_ = 0;
	return InitializeUniverse();
}

bool Game::RestartLevel() {
	return Initialize();
}

xform Game::GetVehicleStart() const {
	xform t(kIdentityQuaternionF, VEHICLE_START);
	t.GetOrientation().RotateAroundOwnZ(-PIF/2);
	return t;
}

bool Game::Tick() {
	if (!level_ || !level_->IsLoaded()) {
		return true;
	}

	if (slowmo_timer_.IsStarted() && slowmo_timer_.QueryTimeDiff() > 4.0f) {
		slowmo_timer_.Stop();
		v_set(GetVariableScope(), kRtvarPhysicsRtr, 1.0);
	}

	GameTicker::GetTimeManager()->Tick();

	flip_render_side_factor_ = Math::Lerp(flip_render_side_factor_, (float)flip_render_side_, 0.15f);
	if (::fabs(flip_render_side_factor_-(float)flip_render_side_) < 0.001f) {
		flip_render_side_factor_ = (float)flip_render_side_;
	}

	vec3 _position;
	vec3 _velocity;
	if (vehicle_ && vehicle_->IsLoaded()) {
		_position = vehicle_->GetPosition()+vec3(0, 0, +10);
		if (_position.z < -50 || _position.z > 100 /*|| isnan(_position.z) || isinf(_position.z)*/) {
			const float health = vehicle_->GetHealth();
			const str vehicle_type = vehicle_->GetClassId();	// TRICKY: don't fetch reference!!!
			SetVehicleName(vehicle_type);
			vehicle_->DrainHealth(vehicle_->GetHealth() - health);
		} else {
			_velocity = vehicle_->GetVelocity();
			vehicle_->QueryFlip();
		}
	}
	collision_sound_manager_->Tick(_position);
	ui_manager_->SetMicrophonePosition(xform(kIdentityQuaternionF, _position), _velocity);

	/*if (win!) {
		AddContextObject(new UiCure::Sound(GetResourceManager(), "win.wav", ui_manager_), cure::kNetworkObjectLocalOnly, 0);
	}
	previous_frame_winner_index_ = winner_index_;*/

	return true;
}

void Game::TickFlyby() {
	const double frame_time = 1.0/kFps;
	fly_by_time_ += frame_time;
}



str Game::GetVehicleName() const {
	if (vehicle_) {
		return vehicle_->GetClassId();
	}
	return str();
}

void Game::SetVehicleName(const str& vehicle) {
	if (vehicle_ && vehicle_->IsLoaded() &&
		vehicle_->GetPosition().GetDistance(GetVehicleStart().GetPosition()) < 2.0f*SCALE_FACTOR &&
		vehicle_->GetClassId() == vehicle &&
		vehicle_->GetHealth() > 0) {
		vehicle_->DrainHealth(-1);
		return;
	}
	delete vehicle_;
	vehicle_ = (Vehicle*)GameManager::CreateContextObject(vehicle, cure::kNetworkObjectLocalOnly, 0);
	bool _ok = (vehicle_ != 0);
	deb_assert(_ok);
	if (_ok) {
		vehicle_->SetInitialTransform(GetVehicleStart());
		vehicle_->DrainHealth(-1);
		vehicle_->StartLoading();
	}
}

Level* Game::GetLevel() const {
	return level_;
}

Vehicle* Game::GetVehicle() const {
	return vehicle_;
}

Goal* Game::GetGoal() const {
	return goal_;
}

void Game::GetVehicleMotion(vec3& position, vec3 velocity) const {
	if (vehicle_ && vehicle_->IsLoaded()) {
		position = vehicle_->GetPosition();
		velocity = vehicle_->GetVelocity();
	}
}

void Game::SetThrottle(float throttle) {
	if (throttle < 0 && vehicle_->GetForwardSpeed() > 0.5f) {
		vehicle_->SetEnginePower(0, 0);	// Disengage throttle while braking.
		vehicle_->SetEnginePower(2, -throttle);
		return;
	}
	vehicle_->SetEnginePower(2, 0);	// Disengage brakes.
	vehicle_->SetEnginePower(0, throttle);
}

Game::FlybyMode Game::GetFlybyMode() const {
	if (!vehicle_ || !vehicle_->IsLoaded()) {
		return kFlybyPause;
	}
	return flyby_mode_;
}

void Game::SetFlybyMode(FlybyMode flyby_mode) {
	flyby_mode_ = flyby_mode;
	if (flyby_mode == kFlybyIntroduction) {
		fly_by_time_ = 0;
	}
}

void Game::ResetScore() {
	score_ = 0;
}

void Game::AddScore(double score) {
	if (!IsScoreCountingEnabled()) {
		return;
	}
	score_ += score;
}

double Game::GetScore() const {
	return score_;
}

void Game::EnableScoreCounting(bool enable) {
	score_counting_enabled_ = enable;
}

bool Game::IsScoreCountingEnabled() const {
	return score_counting_enabled_;
}

void Game::Detonate(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* explosive, cure::ContextObject* target, tbc::PhysicsManager::BodyID explosive_body_id, tbc::PhysicsManager::BodyID target_body_id) {
	collision_sound_manager_->OnCollision(force, torque, position, explosive, level_, explosive_body_id, 10000, true);

	if (target == level_) {
		// Stones and mud. More if hit ground, less otherwise.
		const float scale = SCALE_FACTOR * 320 / ui_manager_->GetCanvas()->GetWidth();
		const int particle_count = (level_->GetStructureGeometry((unsigned)0)->GetBodyId() == target_body_id)? Random::GetRandomNumber()%50+50 : 10;
		for (int i = 0; i < particle_count; ++i) {
			UiCure::Props* puff = new UiCure::Props(GetResourceManager(), "mud_particle_01", ui_manager_);
			AddContextObject(puff, cure::kNetworkObjectLocalOnly, 0);
			puff->EnableRootShadow(false);
			float x = Random::Uniform(-1.0f, 1.0f);
			float y = Random::Uniform(-1.0f, 1.0f);
			float z = -1;
			xform transform(kIdentityQuaternionF, position + vec3(x, y, z));
			puff->SetInitialTransform(transform);
			const float angle = Random::Uniform(0.0f, 2*PIF);
			x = (14.0f * i/particle_count - 10) * cos(angle);
			y = (6 * Random::Uniform(-1.0f, 1.0f)) * sin(angle);
			z = (17 + 8 * sin(5*PIF*i/particle_count) * Random::Uniform(0.0f, 1.0f)) * Random::Uniform(0.2f, 1.0f);
			puff->StartParticle(UiCure::Props::kParticleSolid, vec3(x, y, z), Random::Uniform(3.0f, 7.0f) * scale, 0.5f, Random::Uniform(3.0f, 7.0f));
#ifdef LEPRA_TOUCH_LOOKANDFEEL
			puff->SetFadeOutTime(0.3f);
#endif // touch L&F
			puff->StartLoading();
		}
	}

	{
		// Release gas puffs.
		const int particle_count = (Random::GetRandomNumber() % 4) + 2;
		for (int i = 0; i < particle_count; ++i) {
			UiCure::Props* puff = new UiCure::Props(GetResourceManager(), "cloud_01", ui_manager_);
			AddContextObject(puff, cure::kNetworkObjectLocalOnly, 0);
			puff->EnableRootShadow(false);
			float x = Random::Uniform(-1.0f, 1.0f);
			float y = Random::Uniform(-1.0f, 1.0f);
			float z = Random::Uniform(-1.0f, 1.0f);
			xform transform(kIdentityQuaternionF, position + vec3(x, y, z));
			puff->SetInitialTransform(transform);
			const float opacity = Random::Uniform(0.025f, 0.1f);
			puff->SetOpacity(opacity);
			x = x*12;
			y = y*12;
			z = Random::Uniform(0.0f, 7.0f);
			puff->StartParticle(UiCure::Props::kParticleGas, vec3(x, y, z), 0.003f / opacity, 0.1f, Random::Uniform(1.5f, 4.0f));
			puff->StartLoading();
		}
	}

	float level_shoot_easyness = 4.5f;
	if (level_name_ == "level_elevate") {	// Bigger and open level = easier to hit Vehicle.
		level_shoot_easyness = 3.2f;
	}
	bool did_hit_vehicle = false;
	cure::ContextManager::ContextObjectTable object_table = GetContext()->GetObjectTable();
	cure::ContextManager::ContextObjectTable::iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		const cure::ContextObject* _object = x->second;
		tbc::ChunkyPhysics* physics = _object->ContextObject::GetPhysics();
		if (!_object->IsLoaded() || !physics) {
			continue;
		}
		// Dynamics only get hit in the main body, while statics gets all their dynamic sub-bodies hit.
		const vec3 epicenter = position + vec3(0, 0, -0.75f);
		const int bone_count = (physics->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic)? 1 : physics->GetBoneCount();
		for (int x = 0; x < bone_count; ++x) {
			const tbc::ChunkyBoneGeometry* geometry = physics->GetBoneGeometry(x);
			if (geometry->GetBodyId() == tbc::INVALID_BODY) {
				continue;
			}
			const vec3 body_center = GameTicker::GetPhysicsManager(IsThreadSafe())->GetBodyPosition(geometry->GetBodyId());
			vec3 f = body_center - epicenter;
			float d = f.GetLength();
			if (d > 80*SCALE_FACTOR ||
				(d > 50*SCALE_FACTOR && _object != vehicle_)) {
				continue;
			}
			float distance = d;
			d = 1/d;
			f *= d;
			d *= level_shoot_easyness;
			double _score = d;
			d = d*d*d;
			d = std::min(1.0f, d);
			const float max_force_factor = 800.0f;
			const float ff = max_force_factor * _object->GetMass() * d;
			if (f.z <= 0.1f) {
				f.z += 0.3f;
			}
			f *= ff;
			GameTicker::GetPhysicsManager(IsThreadSafe())->AddForce(geometry->GetBodyId(), f);
			if (_object == vehicle_) {
				if (d > 0.6f) {
					v_set(GetVariableScope(), kRtvarPhysicsRtr, 0.2);
					slowmo_timer_.Start();
				}
				d = std::max(0.005f, d);
				vehicle_->DrainHealth(d);
				_score *= 63;
				_score = std::min(20000.0, _score*_score);
				if (distance < 25*SCALE_FACTOR) {
					did_hit_vehicle = true;
				} else {
					_score = 0;
				}
				if (vehicle_->GetHealth() <= 0) {
					AddScore(-19000);
				} else {
					AddScore(-_score);
				}
			}
		}
	}
	if (!did_hit_vehicle) {
		AddScore(+2000);
	}
}

void Game::OnCapture() {
	AddScore(+10000);
}

void Game::FlipRenderSides() {
	flip_render_side_ = !flip_render_side_;
}

bool Game::IsFlipRenderSide() const {
	return !!flip_render_side_;
}

void Game::EndSlowmo() {
	slowmo_timer_.ReduceTimeDiff(-20);
}



bool Game::Render() {
	if (!vehicle_ || !vehicle_->IsLoaded() ||
		!level_ || !level_->IsLoaded() ||
		!goal_) {
		return true;
	}

	const PixelRect full_rect(0, 0, ui_manager_->GetCanvas()->GetActualWidth(), ui_manager_->GetCanvas()->GetActualHeight());

	if (flyby_mode_ != kFlybyInactive) {
		return FlybyRender();
	}

	const vec3 vehicle_pos = vehicle_->GetPosition();
	const vec3 target_pos = vec3(vehicle_pos.x, vehicle_pos.y, vehicle_pos.z) + CAM_OFFSET;
	vehicle_cam_pos_ = Math::Lerp(vehicle_cam_pos_, target_pos, 0.5f);
	xform t(kIdentityQuaternionF, vehicle_cam_pos_);
	const float vehicle_cam_height = vehicle_cam_pos_.z - vehicle_pos.z;
	const float cam_distance = vehicle_cam_pos_.GetDistance(vehicle_pos);
	t.GetOrientation().RotateAroundOwnX(-::atan(vehicle_cam_height/cam_distance));
/*#ifdef LEPRA_TOUCH_LOOKANDFEEL
	t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
	if (flip_render_side_factor_) {
		t.GetOrientation().RotateAroundOwnY(PIF*flip_render_side_factor_);
	}
#endif // touch*/
	ui_manager_->SetCameraPosition(t);
	ui_manager_->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
	ui_manager_->Render(full_rect);
	ui_manager_->GetRenderer()->ResetAmbientLight(true);
	return true;
}

bool Game::Paint() {
	return true;
}



bool Game::FlybyRender() {
	const vec3 _vehicle = vehicle_->GetPosition();
	const vec3 goal = goal_->GetPosition();
	const double total_flyby_time = 37.0;
	const double total_introduction_time = 75.0;
	if (flyby_mode_ == kFlybyIntroduction) {
		if (fly_by_time_ > total_flyby_time) {
			flyby_mode_ = kFlybyIntroductionFinishingUp;
			return true;
		}
	} else if (flyby_mode_ == kFlybyIntroductionFinishingUp) {
		if (fly_by_time_ > total_introduction_time) {
			flyby_mode_ = kFlybyInactive;
			return true;
		}
	}

	xform t;
	const double sweep_time = total_flyby_time * 0.25;
	const float distance = 100 * SCALE_FACTOR;
	if (fly_by_time_ < sweep_time || flyby_mode_ == kFlybyPause) {
		// Sweep around the area in a circle.
		const float a = 0.8f * 2*PIF * (float)(fly_by_time_/sweep_time);
		t.GetOrientation().RotateAroundOwnZ(a + PIF/2);
		t.GetOrientation().RotateAroundOwnX(-PIF/8);
		t.SetPosition(vec3(::cos(a)*distance, ::sin(a)*distance, ::sin(a+PIF/8)*distance*0.1f + distance/3.5f));
	} else {
		// Look at level in more detail.
		const double detail_time = total_flyby_time - sweep_time;
		// Orientation. Treat orientation and position in different time slices, because if
		// both happen at the same time, perception of space is without a doubt lost.
		if (fly_by_time_-sweep_time < detail_time * 1/12) {
			// Stare right at Vehicle.
			t.GetOrientation().RotateAroundOwnZ(+PIF/2);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
		} else if (fly_by_time_-sweep_time < detail_time * 3/12) {
			// Stand beside Vehicle.
			t.GetOrientation().RotateAroundOwnZ(+PIF*11/12);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
		} else if (fly_by_time_-sweep_time < detail_time * 4/12) {
			// Look up at the goal.
			t.GetOrientation().RotateAroundOwnZ(-PIF*2/5);
			t.GetOrientation().RotateAroundOwnX(+PIF/12);
		} else if (fly_by_time_-sweep_time < detail_time * 7/12) {
			// Look down at the goal.
			t.GetOrientation().RotateAroundOwnZ(-PIF*2/5);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
		}
		// Position.
		if (fly_by_time_-sweep_time < detail_time * 1/3) {
			t.SetPosition(_vehicle + vec3(+1.33f, +6.67f, +3.33f));
		} else if (fly_by_time_-sweep_time < detail_time * 2/3) {
			t.SetPosition(goal + vec3(-13.33f, -10, +10));
		}
	}
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
#endif // touch
	//left_camera_.Interpolate(left_camera_, t, 0.05f);
	//ui_manager_->SetCameraPosition(left_camera_);
	ui_manager_->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
	//ui_manager_->Render(left_rect_);
	return true;
}



/*void Game::MoveTo(const FingerMovement& move) {
}*/


void Game::PollRoundTrip() {
}

float Game::GetTickTimeReduction() const {
	return 0;
}

float Game::GetPowerSaveAmount() const {
	bool is_minimized = !ui_manager_->GetDisplayManager()->IsVisible();
	return (is_minimized? 1.0f : 0);
}



void Game::WillMicroTick(float time_delta) {
	MicroTick(time_delta);
}

void Game::DidPhysicsTick() {
	PostPhysicsTick();
}

void Game::OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	GameManager::OnTrigger(trigger, trigger_listener_id, other_object_id, body_id, position, normal);
}

void Game::OnForceApplied(int object_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, tbc::PhysicsManager::BodyID other_body_id,
	const vec3& force, const vec3& torque, const vec3& position, const vec3& relative_velocity) {
	GameManager::OnForceApplied(object_id, other_object_id, body_id, other_body_id, force, torque, position, relative_velocity);
}



void Game::OnLoadCompleted(cure::ContextObject* object, bool ok) {
	if (ok && object == vehicle_) {
		deb_assert(object->GetPhysics()->GetEngineCount() == 3);
		const str name = "float_childishness";
		new cure::FloatAttribute(vehicle_, name, 0.67f);
	}
}

void Game::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) {
	(void)body2_id;
	collision_sound_manager_->OnCollision(force, torque, position, object1, object2, body1_id, 2000, false);
}

bool Game::OnPhysicsSend(cure::ContextObject* object) {
	object;
	return true;
}

bool Game::OnAttributeSend(cure::ContextObject* object) {
	object;
	return true;
}

bool Game::IsServer() {
	return true;
}

void Game::SendAttach(cure::ContextObject* object1, unsigned id1, cure::ContextObject* object2, unsigned id2) {
	object1;
	id1;
	object2;
	id2;
}

void Game::SendDetach(cure::ContextObject* object1, cure::ContextObject* object2) {
	object1;
	object2;
}

void Game::TickInput() {
}

cure::ContextObject* Game::CreateContextObject(const str& class_id) const {
	if (strutil::StartsWith(class_id, "monster_01")) {
		return new Vehicle(GetResourceManager(), class_id, ui_manager_);
	}
	return new UiCure::Machine(GetResourceManager(), class_id, ui_manager_);
}

bool Game::Initialize() {
	bool _ok = true;
	if (_ok) {
		_ok = InitializeUniverse();
	}
	if (_ok) {
		const bool pixel_shaders_enabled = ui_manager_->GetRenderer()->IsPixelShadersEnabled();
		light_id_ = ui_manager_->GetRenderer()->AddDirectionalLight(
			uitbc::Renderer::kLightMovable, vec3(-1, 0.5f, -1.5),
			vec3(1,1,1) * (pixel_shaders_enabled? 1.0f : 1.5f), 300);
		ui_manager_->GetRenderer()->EnableAllLights(true);
	}
	if (_ok) {
		SetVehicleName("monster_01");
	}
	if (_ok) {
		SetFlybyMode(kFlybyInactive);
	}
	return _ok;
}

bool Game::InitializeUniverse() {
	bool _ok = true;
	if (_ok) {
		delete level_;
		level_ = new Level(GetResourceManager(), level_name_, ui_manager_);
		AddContextObject(level_, cure::kNetworkObjectLocalOnly, 0);
		_ok = (level_ != 0);
		deb_assert(_ok);
		if (_ok) {
			level_->EnableRootShadow(false);
			level_->StartLoading();
		}
	}
	return _ok;
}



cure::ContextObject* Game::CreateLogicHandler(const str& type) {
	if (type == "spawner") {
		return new Spawner(GetContext());
	} else if (type == "trig_goal") {
		goal_ = new Goal(GetContext());
		return goal_;
	} else if (type == "trig_elevator") {
		return new VehicleElevator(this);
	} else if (type == "context_path") {
		return level_->QueryPath();
	}
	return (0);
}



}
