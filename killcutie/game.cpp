
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
#include "../uicure/include/uiexhaustemitter.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uigravelemitter.h"
#include "../uicure/include/uiprops.h"
#include "../uicure/include/uiruntimevariablename.h"
#include "../uicure/include/uisound.h"
#include "ctf.h"
#include "cutie.h"
#include "cutieelevator.h"
#include "grenade.h"
#include "launcher.h"
#include "launcherai.h"
#include "level.h"
#include "roboball.h"
#include "seethrough.h"
#include "spawner.h"
#include "vehicleai.h"



#define GRENADE_RELAUNCH_DELAY	2.7f
#define CUTIE_START		vec3(-57.67f, -28.33f, 2.33f)



namespace grenaderun {



Game::Game(UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager):
	GameTicker(400, 4, 3),
	GameManager(GameTicker::GetTimeManager(), variable_scope, resource_manager),
	ui_manager_(ui_manager),
	collision_sound_manager_(0),
	light_id_(uitbc::Renderer::INVALID_LIGHT),
	level_(0),
	flyby_mode_(kFlybyInactive),
	fly_by_time_(0),
	vehicle_(0),
	vehicle_cam_pos_(0, 0, 200),
	vehicle_cam_height_(15),
	is_launcher_barrel_free_(true),
	launcher_yaw_(0),
	launcher_pitch_(-PIF/4),
	winner_index_(-1),
	previous_frame_winner_index_(-1),
	ctf_(0),
	vehicle_ai_(0),
	launcher_(0),
	launcher_ai_(0),
	computer_index_(-1),
	computer_difficulty_(0.5f),
	heart_balance_(0),
	allow_win_(false),
	flip_render_side_(0),
	flip_render_side_factor_(0),
	score_(0),
	score_counting_enabled_(false),
	round_index_(0) {
	SetTicker(this);

	previous_canvas_angle_ = ui_manager_->GetCanvas()->GetOutputRotation();

	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->AddSound("explosion",	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	collision_sound_manager_->AddSound("small_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	collision_sound_manager_->AddSound("big_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	collision_sound_manager_->AddSound("rubber",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	collision_sound_manager_->AddSound("wood",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
}

Game::~Game() {
	ui_manager_->GetRenderer()->RemoveLight(light_id_);

	delete collision_sound_manager_;
	collision_sound_manager_ = 0;
	ui_manager_ = 0;
	SetVariableScope(0);	// Not owned by us.

	ctf_ = 0;
	vehicle_ai_ = 0;
	launcher_ = 0;
	launcher_ai_ = 0;
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
	ctf_ = 0;
	return InitializeUniverse();
}

bool Game::RestartLevel() {
	return Initialize();
}

xform Game::GetCutieStart() const {
	xform t(kIdentityQuaternionF, CUTIE_START);
	t.GetOrientation().RotateAroundOwnZ(-PIF*0.45f);
	return t;
}

bool Game::Tick() {
	if (!level_ || !level_->IsLoaded()) {
		return true;
	}

	if (laucher_lock_watch_.IsStarted()) {
		laucher_lock_watch_.UpdateTimer();
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
		_position = vehicle_->GetPosition()+vec3(0, 0, -1);
		if (_position.z < -50 || _position.z > 100 /*|| isnan(_position.z) || isinf(_position.z)*/) {
			const float health = vehicle_->GetHealth();
			const str vehicle_type = vehicle_->GetClassId();	// TRICKY: don't fetch reference!!!
			SetVehicle(vehicle_type);
			vehicle_->DrainHealth(vehicle_->GetHealth() - health);
		} else {
			_velocity = vehicle_->GetVelocity();
			vehicle_->QueryFlip();
		}
	} else if (launcher_ && launcher_->IsLoaded()) {
		_position = launcher_->GetPosition();
	}
	collision_sound_manager_->Tick(_position);
	ui_manager_->SetMicrophonePosition(xform(kIdentityQuaternionF, _position), _velocity);

	if (launcher_ && launcher_->IsLoaded()) {
		float real_time_ratio;
		v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
		launcher_yaw_ -= launcher_->ContextObject::GetPhysics()->GetEngine(1)->GetLerpThrottle(0.2f, 0.2f, false) * 0.01f * real_time_ratio;
		launcher_pitch_ -= launcher_->ContextObject::GetPhysics()->GetEngine(0)->GetLerpThrottle(0.2f, 0.2f, false) * 0.01f * real_time_ratio;
		if (launcher_yaw_ < -PIF) {
			launcher_yaw_ += 2*PIF;
		} else if (launcher_yaw_ > +PIF) {
			launcher_yaw_ -= 2*PIF;
		}
		launcher_pitch_ = Math::Clamp(launcher_pitch_, -PIF/2*0.6f, -PIF/90);
		launcher_->SetBarrelAngle(launcher_yaw_, launcher_pitch_);
	}

	if (previous_frame_winner_index_ == -1 && winner_index_ != -1) {
		if (winner_index_ == 0 && computer_index_ != 0) {
			AddContextObject(new UiCure::Sound(GetResourceManager(), "win.wav", ui_manager_), cure::kNetworkObjectLocalOnly, 0);
		} else if (winner_index_ == 1 && computer_index_ != 1) {
			AddContextObject(new UiCure::Sound(GetResourceManager(), "win.wav", ui_manager_), cure::kNetworkObjectLocalOnly, 0);
		} else {
			AddContextObject(new UiCure::Sound(GetResourceManager(), "kia.wav", ui_manager_), cure::kNetworkObjectLocalOnly, 0);
		}
		heart_balance_ += (!!winner_index_ == flip_render_side_)? -1 : +1;
	}
	previous_frame_winner_index_ = winner_index_;

	return true;
}

void Game::TickFlyby() {
	const double frame_time = 1.0/kFps;
	fly_by_time_ += frame_time;
}



str Game::GetVehicle() const {
	if (vehicle_) {
		return vehicle_->GetClassId();
	}
	return str();
}

void Game::SetVehicle(const str& vehicle) {
	allow_win_ = true;
	if (vehicle_ && vehicle_->IsLoaded() &&
		vehicle_->GetPosition().GetDistance(GetCutieStart().GetPosition()) < 2.0f*kScaleFactor &&
		vehicle_->GetClassId() == vehicle &&
		vehicle_->GetHealth() > 0) {
		vehicle_->DrainHealth(-1);
		// Degrade health every other round, so it becomes increasingly difficult to extend the game.
		if (GetComputerIndex() == 1) {
			vehicle_->DrainHealth(1 - GetVehicleStartHealth(GetRoundIndex()));
		}
		return;
	}
	delete vehicle_;
	vehicle_ = (Cutie*)GameManager::CreateContextObject(vehicle, cure::kNetworkObjectLocalOnly, 0);
	bool _ok = (vehicle_ != 0);
	deb_assert(_ok);
	if (_ok) {
		vehicle_->SetInitialTransform(GetCutieStart());
		vehicle_->DrainHealth(1 - GetVehicleStartHealth(GetRoundIndex()));
		vehicle_->StartLoading();
	}
}

void Game::ResetLauncher() {
	allow_win_ = true;
	delete launcher_;
	launcher_ = new Launcher(this);
	AddContextObject(launcher_, cure::kNetworkObjectLocalOnly, 0);
	launcher_->EnableRootShadow(false);
	launcher_->StartLoading();
	launcher_yaw_ = PIF*0.24f;
	launcher_pitch_ = -PIF/4;

	// Drop all grenades.
	cure::ContextManager::ContextObjectTable object_table = GetContext()->GetObjectTable();
	cure::ContextManager::ContextObjectTable::iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		if (_object->GetClassId().find("grenade") != str::npos) {
			GetContext()->PostKillObject(_object->GetInstanceId());
		}
	}
}

UiCure::CppContextObject* Game::GetP1() const {
	return vehicle_;
}

UiCure::CppContextObject* Game::GetP2() const {
	return launcher_;
}

Level* Game::GetLevel() const {
	return level_;
}

Cutie* Game::GetCutie() const {
	return vehicle_;
}

Launcher* Game::GetLauncher() const {
	return launcher_;
}

Ctf* Game::GetCtf() const {
	return ctf_;
}

void Game::GetVehicleMotion(vec3& position, vec3 velocity) const {
	if (vehicle_ && vehicle_->IsLoaded()) {
		position = vehicle_->GetPosition();
		velocity = vehicle_->GetVelocity();
	}
}

void Game::GetLauncherTransform(xform& transform) const {
	if (launcher_ && launcher_->IsLoaded()) {
		transform.SetPosition(launcher_->GetPosition());
		transform.SetOrientation(launcher_->GetOrientation());
	}
}

void Game::SetThrottle(UiCure::CppContextObject* player, float throttle) {
	if (player == launcher_) {
		launcher_->SetEnginePower(0, throttle);
		return;
	}
	if (throttle < 0 && vehicle_->GetForwardSpeed() > 0.5f) {
		vehicle_->SetEnginePower(2, -throttle);
		return;
	}
	vehicle_->SetEnginePower(2, 0);	// Disengage brakes.
	vehicle_->SetEnginePower(0, throttle);
}

bool Game::Shoot() {
	if (GetLauncherLockPercent() < 1.0f) {
		return false;
	}

	Grenade* grenade = (Grenade*)GameManager::CreateContextObject("grenade", cure::kNetworkObjectLocalOnly, 0);
	bool _ok = (grenade != 0);
	deb_assert(_ok);
	if (_ok) {
		xform t(launcher_->GetOrientation(), launcher_->GetPosition()+vec3(0, 0, +2.5f));
		grenade->SetInitialTransform(t);
		grenade->StartLoading();
		laucher_lock_watch_.Start();
		is_launcher_barrel_free_ = false;
	}
	return _ok;
}

cure::ContextObject* Game::CreateRoboBall() {
	RoboBall* robo_ball = (RoboBall*)GameManager::CreateContextObject("robo_ball", cure::kNetworkObjectLocalOnly, 0);
	deb_assert(robo_ball);
	if (robo_ball) {
		xform t(quat(), vec3(0, 0, 30));
		robo_ball->SetInitialTransform(t);
		robo_ball->StartLoading();
	}
	return robo_ball;
}

float Game::GetMuzzleVelocity() const {
	float muzzle_velocity = 60.0;
	if (launcher_ && launcher_->IsLoaded() && vehicle_ && vehicle_->IsLoaded()) {
		muzzle_velocity = ::pow(launcher_->GetPosition().GetDistance(vehicle_->GetPosition()), 0.5f) * 3.6f;
	}
	return muzzle_velocity;
}

float Game::GetLauncherLockPercent() const {
	float value = 1;
	if (laucher_lock_watch_.IsStarted()) {
		float real_time_ratio;
		v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
		value = std::min(1.0f, (float)laucher_lock_watch_.GetTimeDiff() * real_time_ratio / GRENADE_RELAUNCH_DELAY);
	}
	return value;
}

bool Game::IsLauncherBarrelFree() const {
	return is_launcher_barrel_free_;
}

void Game::FreeLauncherBarrel() {
	is_launcher_barrel_free_ = true;
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
	round_index_ = 0;
}

void Game::AddScore(double cutie_score, double launcher_score) {
	if (computer_index_ == -1 || winner_index_ != -1 || !IsScoreCountingEnabled()) {
		return;
	}
	// Difficulty: multiply by lerp(1, 4) ^ 2, with some factor...
	double difficulty_factor = (computer_difficulty_ * 3.0 + 1.0);
	difficulty_factor *= difficulty_factor;
	if (computer_index_ == 1) {
		score_ += cutie_score * difficulty_factor;
	} else {
		score_ += launcher_score * difficulty_factor;
	}
}

double Game::GetScore() const {
	return score_;
}

void Game::EnableScoreCounting(bool enable) {
	score_counting_enabled_ = enable;
}

bool Game::IsScoreCountingEnabled() const {
	return (score_counting_enabled_ && computer_index_ != -1);
}

void Game::Detonate(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* explosive, cure::ContextObject* target, tbc::PhysicsManager::BodyID explosive_body_id, tbc::PhysicsManager::BodyID target_body_id) {
	collision_sound_manager_->OnCollision(force, torque, position, explosive, level_, explosive_body_id, 10000, true);

	if (target == level_) {
		// Stones and mud. More if hit ground, less otherwise.
		const float scale = kScaleFactor * 320 / ui_manager_->GetCanvas()->GetWidth();
		const int particle_count = (level_->GetStructureGeometry((unsigned)0)->GetBodyId() == target_body_id)? Random::GetRandomNumber()%50+50 : 10;
		for (int i = 0; i < particle_count; ++i) {
			UiCure::Props* puff = new UiCure::Props(GetResourceManager(), "mud_particle_01", ui_manager_);
			AddContextObject(puff, cure::kNetworkObjectLocalOnly, 0);
			puff->EnableRootShadow(false);
			float x = Random::Uniform(-1.0f, 1.0f);
			float y = Random::Uniform(-1.0f, 1.0f);
			float z = -1;
			xform _transform(kIdentityQuaternionF, position + vec3(x, y, z));
			puff->SetInitialTransform(_transform);
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
			xform _transform(kIdentityQuaternionF, position + vec3(x, y, z));
			puff->SetInitialTransform(_transform);
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
	if (level_name_ == "level_elevate") {	// Bigger and open level = easier to hit Cutie.
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
		for (int y = 0; y < bone_count; ++y) {
			const tbc::ChunkyBoneGeometry* geometry = physics->GetBoneGeometry(y);
			if (geometry->GetBodyId() == tbc::INVALID_BODY) {
				continue;
			}
			if ((physics->GetPhysicsType() == tbc::ChunkyPhysics::kWorld || physics->GetPhysicsType() == tbc::ChunkyPhysics::kStatic)
				&& geometry->GetJointType() == tbc::ChunkyBoneGeometry::kJointExclude) {
				continue;	// No use pushing around trees, etc.
			}
			const vec3 body_center = GameTicker::GetPhysicsManager(true)->GetBodyPosition(geometry->GetBodyId());
			vec3 f = body_center - epicenter;
			float d = f.GetLength();
			if (d > 80*kScaleFactor ||
				(d > 50*kScaleFactor && _object != vehicle_)) {
				continue;
			}
			float distance = d;
			d = 1/d;
			f *= d;
			d *= level_shoot_easyness;
			double score = d;
			d = d*d*d;
			d = std::min(1.0f, d);
			const float max_force_factor = 800.0f;
			const float ff = max_force_factor * _object->GetMass() * d;
			if (f.z <= 0.1f) {
				f.z += 0.3f;
			}
			f *= ff;
			GameTicker::GetPhysicsManager(true)->AddForce(geometry->GetBodyId(), f);
			if (_object == vehicle_) {
				if (d > 0.6f) {
					v_set(GetVariableScope(), kRtvarPhysicsRtr, 0.2);
					slowmo_timer_.Start();
				}
				d = std::max(0.005f, d);
				vehicle_->DrainHealth(d);
				if (computer_index_ != -1) {
					// Weigh up minimum score for computer, if very bad.
					// Otherwise the car can go round, round and only
					// gain more points for every lap.
					score = std::max(0.18-computer_difficulty_, score);
				}
				score *= 63;
				score = std::min(20000.0, score*score);
				if (distance < 25*kScaleFactor) {
					did_hit_vehicle = true;
				} else {
					score = 0;
				}
				if (vehicle_->GetHealth() <= 0) {
					if (allow_win_) {
						AddScore(-19000, score+4000);
						winner_index_ = (winner_index_ != 0)? 1 : winner_index_;
					}
				} else {
					AddScore(-score, score);
				}
			}
		}
	}
	if (!did_hit_vehicle) {
		AddScore(+2000, -2000);
	}
}

void Game::OnCapture() {
	if (allow_win_) {
		AddScore(+10000, -10000);
		winner_index_ = (winner_index_ != 1)? 0 : winner_index_;
	}
}

int Game::GetWinnerIndex() const {
	return winner_index_;
}

void Game::ResetWinnerIndex() {
	allow_win_ = false;
	if (ctf_) {
		ctf_->StartSlideDown();
	}
	winner_index_ = -1;
	previous_frame_winner_index_ = -1;
}

void Game::SetComputerIndex(int index) {
	deb_assert(index >= -1 && index <= 1);
	flip_render_side_ = 0;
	flip_render_side_factor_ = 0;
	computer_index_ = index;
	delete vehicle_ai_;
	vehicle_ai_ = 0;
	delete launcher_ai_;
	launcher_ai_ = 0;
	if (computer_index_ == 0) {
		vehicle_ai_ = new VehicleAi(this);
		AddContextObject(vehicle_ai_, cure::kNetworkObjectLocalOnly, 0);
		vehicle_ai_->Init();
	} else if (computer_index_ == 1) {
		launcher_ai_ = new LauncherAi(this);
		AddContextObject(launcher_ai_, cure::kNetworkObjectLocalOnly, 0);
		launcher_ai_->Init();
	}
}

int Game::GetComputerIndex() const {
	return computer_index_;
}

void Game::NextComputerIndex() {
	if (++computer_index_ > 1) {
		computer_index_ = -1;
	}
}

float Game::GetComputerDifficulty() const {
	return computer_difficulty_;
}

void Game::SetComputerDifficulty(float difficulty) {
	deb_assert(difficulty >= -1 && difficulty <= 1);
	computer_difficulty_ = difficulty;
}

int Game::GetHeartBalance() const {
	return heart_balance_;
}

void Game::SetHeartBalance(int balance) {
	heart_balance_ = balance;
}

void Game::FlipRenderSides() {
	flip_render_side_ = !flip_render_side_;
}

bool Game::IsFlipRenderSide() const {
	return !!flip_render_side_;
}

void Game::NextRound() {
	++round_index_;
}

int Game::GetRoundIndex() const {
	return round_index_;
}

float Game::GetVehicleStartHealth(int round_index) const {
	if (GetComputerIndex() == -1 || round_index < 2) {
		return 1;
	}
	return ::pow(kHealthRoundFactor, round_index/2);
}

void Game::EndSlowmo() {
	slowmo_timer_.ReduceTimeDiff(-20);
}



void Game::SyncCameraPositions() {
	switch (GetComputerIndex()) {
		//case -1:	std::swap(left_camera_, right_camera_);	break;
		case 0:		left_camera_ = right_camera_;		break;	// Computer is "left", copy player.
		case 1:		right_camera_ = left_camera_;		break;	// Computer is "right", copy player.
	}
}

bool Game::Render() {
	if (!vehicle_ || !vehicle_->IsLoaded() ||
		!level_ || !level_->IsLoaded() ||
		!ctf_) {
		return true;
	}

	const PixelRect full_rect(0, 0, ui_manager_->GetCanvas()->GetActualWidth(), ui_manager_->GetCanvas()->GetActualHeight());
	left_rect_ = full_rect;
	if (full_rect.right_ < full_rect.bottom_) {	// Portrait?
		left_rect_.bottom_ = left_rect_.bottom_/2 - 5;
	} else {
		left_rect_.right_ = left_rect_.right_/2 - 5;
	}
	right_rect_ = full_rect;
	if (full_rect.right_ < full_rect.bottom_) {	// Portrait?
		right_rect_.top_ = left_rect_.bottom_ + 10;
	} else {
		right_rect_.left_ = left_rect_.right_ + 10;
	}
	if (flip_render_side_factor_) {
		if (flip_render_side_factor_ == 1) {
			std::swap(left_rect_, right_rect_);
		} else {
			const PixelRect r = right_rect_;
			right_rect_.left_ = Math::Lerp(right_rect_.left_, left_rect_.left_, flip_render_side_factor_);
			right_rect_.right_ = Math::Lerp(right_rect_.right_, left_rect_.right_, flip_render_side_factor_);
			right_rect_.top_ = Math::Lerp(right_rect_.top_, left_rect_.top_, flip_render_side_factor_);
			right_rect_.bottom_ = Math::Lerp(right_rect_.bottom_, left_rect_.bottom_, flip_render_side_factor_);
			left_rect_.left_ = Math::Lerp(left_rect_.left_, r.left_, flip_render_side_factor_);
			left_rect_.right_ = Math::Lerp(left_rect_.right_, r.right_, flip_render_side_factor_);
			left_rect_.top_ = Math::Lerp(left_rect_.top_, r.top_, flip_render_side_factor_);
			left_rect_.bottom_ = Math::Lerp(left_rect_.bottom_, r.bottom_, flip_render_side_factor_);
		}
	}
	switch (GetComputerIndex()) {
		case -1:			break;	// Two player game.
		case 0:	right_rect_ = full_rect;	break;	// Single player, to right.
		case 1:	left_rect_ = full_rect;	break;	// Single player, to left.
	}

	launcher_->SetRootPosition(launcher_position_);

	// Yield smooth rotation when canvas orientation changed.
	if (previous_canvas_angle_ != ui_manager_->GetCanvas()->GetOutputRotation()) {
		previous_canvas_angle_ = ui_manager_->GetCanvas()->GetOutputRotation();
		left_camera_.GetOrientation().RotateAroundOwnY(PIF);
		right_camera_.GetOrientation().RotateAroundOwnY(PIF);
	}

	if (GetComputerIndex() == 0) {
		left_camera_ = right_camera_;	// For smooth transitions.
	} else if (GetComputerIndex() == 1) {
		right_camera_ = left_camera_;	// For smooth transitions.
	}

	if (flyby_mode_ != kFlybyInactive) {
		return FlybyRender();
	}

	if (GetComputerIndex() != 0) {
		xform t(kIdentityQuaternionF, vec3(-50, -70, -5));
		const vec3 vehicle_pos = vehicle_->GetPosition();
		vec3 offset = vehicle_cam_pos_ - vehicle_pos;
		offset.z = 0;
		const float cam_xy_distance = 13 * kScaleFactor;
		float cam_height = 5 * kScaleFactor;
		offset.Normalize(cam_xy_distance);
		float angle = (-offset).GetAngle(vec3(0, cam_xy_distance, 0));
		if (offset.x < 0) {
			angle = -angle;
		}
		t.GetOrientation().RotateAroundOwnZ(angle);
		offset.z = cam_height;

		const tbc::PhysicsManager::BodyID terrain_body_id = level_->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		vec3 collision_point;
		int x = 0;
		for (x = 0; x < 3; ++x) {
			const bool is_collision = (GameTicker::GetPhysicsManager(true)->QueryRayCollisionAgainst(
				vehicle_pos, offset, offset.GetLength(), terrain_body_id, &collision_point, 1) > 0);
			if (!is_collision) {
				break;
			}
			cam_height += 10*kScaleFactor;
			offset.z = cam_height;
		}
		vehicle_cam_height_ = Math::Lerp(vehicle_cam_height_, cam_height, 0.2f);
		offset.z = vehicle_cam_height_;

		t.GetOrientation().RotateAroundOwnX(-::atan(vehicle_cam_height_/cam_xy_distance) + PIF/18);
		vehicle_cam_pos_ = vehicle_pos + offset;
		t.GetPosition() = vehicle_cam_pos_;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		// If computer runs the launcher, the vehicle should be displayed in landscape mode.
		if (GetComputerIndex() != 1) {
			t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
		}
		if (flip_render_side_factor_) {
			t.GetOrientation().RotateAroundOwnY(PIF*flip_render_side_factor_);
		}
#endif // touch
		left_camera_.Interpolate(left_camera_, t, 0.1f);
		ui_manager_->SetCameraPosition(left_camera_);
		ui_manager_->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
		ui_manager_->Render(left_rect_);
		ui_manager_->GetRenderer()->ResetAmbientLight(true);
	}

	if (GetComputerIndex() != 1) {
		const float launcher_height = 3;
		const vec3 muzzle_position(launcher_position_ + launcher_->GetOrientation()*vec3(0, 0, launcher_height));

		float range = 100 * kScaleFactor;
		float look_down_angle = -PIF/2;
		if (vehicle_ && vehicle_->IsLoaded()) {
			range = muzzle_position.GetDistance(vehicle_->GetPosition());
			look_down_angle = ::sin((vehicle_->GetPosition().z-muzzle_position.z)/range);
		}

		vec3 straight_vector(vehicle_->GetPosition() - muzzle_position);
		const float cam_distance = 10 * kScaleFactor;
		straight_vector.Normalize(cam_distance);
		straight_vector.x = cam_distance*sin(launcher_yaw_);
		straight_vector.y = -cam_distance*cos(launcher_yaw_);
		straight_vector.z = -straight_vector.z + launcher_height*0.7f;
		xform t(kIdentityQuaternionF, launcher_position_+straight_vector);
		t.GetOrientation().RotateAroundOwnZ(launcher_yaw_*0.9f);
		t.GetOrientation().RotateAroundOwnX(look_down_angle);
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (GetComputerIndex() == -1) {
			// Launcher always displayed in portrait in dual play.
			const float orientation_factor = flip_render_side_factor_ + 0.5f;
			t.GetOrientation().RotateAroundOwnY(PIF*orientation_factor);
		} else {
			// Single play in landscape mode.
			if (flip_render_side_factor_) {
				t.GetOrientation().RotateAroundOwnY(PIF*flip_render_side_factor_);
			}
		}
#endif // touch
		right_camera_.Interpolate(right_camera_, t, 0.1f);
		ui_manager_->SetCameraPosition(right_camera_);
		const float distance_fo_v = 5667 / ::pow(range, 1.2f);
		ui_manager_->GetRenderer()->SetViewFrustum(std::min(60.0f, distance_fo_v), 1.5f, 500);
		ui_manager_->Render(right_rect_);
	}
	return true;
}

bool Game::Paint() {
	return true;
}



bool Game::FlybyRender() {
	const vec3 cutie = vehicle_->GetPosition();
	const vec3 goal = ctf_->GetPosition();
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
	const float distance = 100 * kScaleFactor;
	if (fly_by_time_ < sweep_time || flyby_mode_ == kFlybyPause) {
		// Sweep around the area in a circle.
		const float a = 0.8f * 2*PIF * (float)(fly_by_time_/sweep_time);
		t.GetOrientation().RotateAroundOwnZ(a + PIF/2);
		t.GetOrientation().RotateAroundOwnX(-PIF/8);
		t.SetPosition(vec3(::cos(a)*distance, ::sin(a)*distance, ::sin(a+PIF/8)*distance*0.1f + distance/3.5f));
	} else {
		// Look at cutie, goal and launcher in more detail.
		const double detail_time = total_flyby_time - sweep_time;
		// Orientation. Treat orientation and position in different time slices, because if
		// both happen at the same time, perception of space is without a doubt lost.
		if (fly_by_time_-sweep_time < detail_time * 1/12) {
			// Stare right at Cutie.
			t.GetOrientation().RotateAroundOwnZ(+PIF/2);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
		} else if (fly_by_time_-sweep_time < detail_time * 3/12) {
			// Stand beside Cutie.
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
		} else if (fly_by_time_-sweep_time < detail_time * 10/12) {
			// Look right at the launcher.
			t.GetOrientation().RotateAroundOwnZ(+PIF*7/8);
			t.GetOrientation().RotateAroundOwnX(-PIF/10);
		} else {
			// Stand beside the launcher.
			t.GetOrientation().RotateAroundOwnZ(+PIF/2);
			t.GetOrientation().RotateAroundOwnX(-PIF/4);
		}
		// Position.
		if (fly_by_time_-sweep_time < detail_time * 1/3) {
			t.SetPosition(cutie + vec3(+1.33f, +6.67f, +3.33f));
		} else if (fly_by_time_-sweep_time < detail_time * 2/3) {
			t.SetPosition(goal + vec3(-13.33f, -10, +10));
		} else if (fly_by_time_-sweep_time < detail_time * 10/12) {
			t.SetPosition(launcher_position_ + vec3(+1.67f, +5, +3.33f));	// In front of launcher.
		} else {
			t.SetPosition(launcher_position_ + vec3(+4.67f, 0, +4.67f));	// Beside launcher.
		}
	}
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	// If computer runs the launcher, the vehicle should be displayed in landscape mode.
	if (GetComputerIndex() != 1) {
		t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
	}
#endif // touch
	if (GetComputerIndex() != 0) {
		left_camera_.Interpolate(left_camera_, t, 0.05f);
		ui_manager_->SetCameraPosition(left_camera_);
		ui_manager_->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
		ui_manager_->Render(left_rect_);
	}
	if (GetComputerIndex() != 1) {
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (GetComputerIndex() == 0) {
			// Fly-by always in landscape when single playing.
			t.GetOrientation().RotateAroundOwnY(PIF*0.5f);
		} else {
			// The launcher in portrait for dual play.
			t.GetOrientation().RotateAroundOwnY(PIF);
		}
#endif // touch
		right_camera_.Interpolate(right_camera_, t, 0.05f);
		ui_manager_->SetCameraPosition(right_camera_);
		ui_manager_->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
		ui_manager_->Render(right_rect_);
	}
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
	if (ok && object == launcher_) {
		// Create a mock engine on the launcher that we use to navigate.
		launcher_->CreateEngines();
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
	UiCure::Machine* machine = 0;
	if (strutil::StartsWith(class_id, "grenade")) {
		machine = new Grenade(GetResourceManager(), class_id, ui_manager_, GetMuzzleVelocity());
	} else if (strutil::StartsWith(class_id, "robo_ball")) {
		machine = new RoboBall(this, class_id);
	} else if (strutil::StartsWith(class_id, "cutie") ||
		strutil::StartsWith(class_id, "monster") ||
		strutil::StartsWith(class_id, "corvette") ||
		strutil::StartsWith(class_id, "road_roller")) {
		machine = new Cutie(GetResourceManager(), class_id, ui_manager_);
	} else {
		machine = new UiCure::Machine(GetResourceManager(), class_id, ui_manager_);
	}
	if (machine) {
		machine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), ui_manager_));
	}
	return machine;
}

bool Game::Initialize() {
	bool _ok = true;
	if (_ok) {
		winner_index_ = -1;
		previous_frame_winner_index_ = -1;

		quat rotation;
		rotation.RotateAroundOwnX(-PIF/4);
		rotation.RotateAroundOwnZ(-PIF/8);
		left_camera_ = xform(rotation, vec3(-25, -50, 35));
		right_camera_ = left_camera_;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		left_camera_.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
		right_camera_.GetOrientation().RotateAroundOwnY(+PIF*0.5f);
#endif // touch

		launcher_position_ = vec3(0, -75.2f, 10.5f);

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
		SetVehicle("cutie");
		ResetLauncher();
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
		UiCure::GravelEmitter* gravel_particle_emitter = new UiCure::GravelEmitter(GetResourceManager(), ui_manager_, 1, 1, 10, 2);
		level_ = new Level(GetResourceManager(), level_name_, ui_manager_, gravel_particle_emitter);
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
	} else if (type == "trig_ctf") {
		ctf_ = new Ctf(GetContext());
		return ctf_;
	} else if (type == "trig_elevator") {
		return new CutieElevator(this);
	} else if (type == "context_path") {
		return level_->QueryPath();
	} else if (type == "see_through") {
		return new SeeThrough(GetContext(), this);
	} else if (type == "anything") {
		return CreateRoboBall();
	}
	return (0);
}



}
