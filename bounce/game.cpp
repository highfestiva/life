
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
#include "ball.h"
#include "racket.h"



namespace bounce {



Game::Game(UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager):
	cure::GameTicker(20, 3, 0),
	cure::GameManager(cure::GameTicker::GetTimeManager(), variable_scope, resource_manager),
	ui_manager_(ui_manager),
	collision_sound_manager_(0),
	light_id_(uitbc::Renderer::INVALID_LIGHT),
	racket_(0),
	ball_(0),
	racket_lift_factor_(0),
	score_(0) {
	SetTicker(this);

	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->SetScale(0.05f, 0.1f, 1, 0.1f);
	collision_sound_manager_->AddSound("hit", UiCure::CollisionSoundManager::SoundResourceInfo(0.9f, 0.1f, 0.5f));

	Initialize();
}

Game::~Game() {
	ui_manager_->GetRenderer()->RemoveLight(light_id_);

	delete collision_sound_manager_;
	collision_sound_manager_ = 0;
	ui_manager_ = 0;
	SetVariableScope(0);	// Not owned by us.
}

UiCure::GameUiManager* Game::GetUiManager() const {
	return ui_manager_;
}

bool Game::Tick() {
	if (!racket_ || !racket_->IsLoaded() ||
		!ball_ || !ball_->IsLoaded()) {
		return true;
	}

	GameTicker::GetTimeManager()->Tick();

	vec3 _position;
	vec3 velocity;
	collision_sound_manager_->Tick(_position);
	ui_manager_->SetMicrophonePosition(xform(kIdentityQuaternionF, _position), velocity);

	return true;
}



void Game::SetRacketForce(float lift_factor, const vec3& down) {
	racket_lift_factor_ = (down.z > 0)? -lift_factor : lift_factor;
	racket_down_direction_ = down;
}

bool Game::MoveRacket() {
	if (GetRacket() && GetRacket()->IsLoaded() &&
		GetBall() && GetBall()->IsLoaded()) {
		xform racket_transform;
		GameTicker::GetPhysicsManager(IsThreadSafe())->GetBodyTransform(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			racket_transform);
		vec3 racket_linear_velocity;
		GameTicker::GetPhysicsManager(IsThreadSafe())->GetBodyVelocity(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			racket_linear_velocity);
		vec3 racket_angular_velocity;
		GameTicker::GetPhysicsManager(IsThreadSafe())->GetBodyAngularVelocity(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			racket_angular_velocity);

		// Calculate where ball will be as it passes z = racket z.
		vec3 ball_position =
			GameTicker::GetPhysicsManager(IsThreadSafe())->GetBodyPosition(GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId());
		vec3 ball_velocity;
		GameTicker::GetPhysicsManager(IsThreadSafe())->GetBodyVelocity(
			GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			ball_velocity);
		if (ball_position.z < -2) {
			ball_position.Set(0, 0, 0.4f);
			GameTicker::GetPhysicsManager(IsThreadSafe())->SetBodyTransform(GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), xform(quat(), ball_position));
			ball_velocity.Set(0, 0, 2.0f);
			GameTicker::GetPhysicsManager(IsThreadSafe())->SetBodyVelocity(GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), ball_velocity);
			GameTicker::GetPhysicsManager(IsThreadSafe())->SetBodyAngularVelocity(GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), vec3());
			racket_transform.SetIdentity();
			GameTicker::GetPhysicsManager(IsThreadSafe())->SetBodyTransform(GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), racket_transform);
			racket_linear_velocity.Set(0, 0, 0);
			GameTicker::GetPhysicsManager(IsThreadSafe())->SetBodyVelocity(GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), racket_linear_velocity);
			racket_angular_velocity.Set(0, 0, 0);
			GameTicker::GetPhysicsManager(IsThreadSafe())->SetBodyAngularVelocity(GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), racket_angular_velocity);
			return false;
		}
		vec3 home;
		const float h = ball_position.z - racket_transform.GetPosition().z;
		if (h > -0.5f) {
			home.Set(ball_position.x*0.8f, ball_position.y*0.8f, 0);
		}
		const float vup = ball_velocity.z;
		const float a = +9.82f / 2;
		const float b = -vup;
		const float c = +h;
		const float b2 = b*b;
		const float _4ac = 4*a*c;
		if (b2 < _4ac || _4ac < 0) {
			// Does not compute.
		} else {
			const float t = (-b + sqrt(b2 - _4ac)) / (2*a);
			if (t > 0) {
				home.x += ball_velocity.x * t;
				home.y += ball_velocity.y * t;
			}
		}
		// Set linear force.
		const vec3 direction_home = home - racket_transform.GetPosition();
		float f = direction_home.GetLength();
		f *= 50;
		f *= f;
		vec3 _force = direction_home * f;
		_force -= racket_linear_velocity * 10;
		float user_force_factor = ::fabs(racket_lift_factor_) * 1.7f;
		user_force_factor = std::min(1.0f, user_force_factor);
		const float racket_acceleration = 100.0f * racket_lift_factor_;
		const float zForce = Math::Lerp(_force.z, racket_acceleration, user_force_factor);
		_force.z = zForce;
		f = _force.GetLength();
		if (f > 100) {
			_force *= 100 / f;
		}
		//mLog.Infof("force = (%f, %f, %f"), lForce.x, lForce.y, lForce.z);
		GameTicker::GetPhysicsManager(IsThreadSafe())->AddForce(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			_force);

		// Set torque. Note that racket is "flat" along the XY-plane.
		//const float lTiltAngleFactor = 1.2f;
		//const float dx = direction_home.x * lTiltAngleFactor;
		//const float dy = direction_home.y * lTiltAngleFactor;
		//const float dx = -racket_transform.GetPosition().x * lTiltAngleFactor;
		//const float dy = -racket_transform.GetPosition().y * lTiltAngleFactor;
		racket_down_direction_.Normalize();
		const vec3 home_torque = vec3(::acos(racket_down_direction_.y), ::acos(racket_down_direction_.x), 0);
		vec3 racket_torque = racket_transform.GetOrientation() * vec3(0,0,1);
		racket_torque = vec3(::acos(racket_torque.y), ::acos(racket_torque.x), 0);
		vec3 angle_home = home_torque - racket_torque;
		angle_home.y = -angle_home.y;
		angle_home.z = 0;
		f = Math::Clamp(-ball_velocity.z, 0.0f, 4.0f) / 4.0f;
		f = Math::Lerp(0.8f, 0.3f, f);
		f = angle_home.GetLength() * f;
		f *= f;
		f = 1;
		vec3 _torque = angle_home * f;
		_torque -= racket_angular_velocity * 0.2f;
		//mLog.Infof("torque = (%f, %f, %f"), lTorque.x, lTorque.y, lTorque.z);
		GameTicker::GetPhysicsManager(IsThreadSafe())->AddTorque(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			_torque);
	}
	return true;
}

Racket* Game::GetRacket() const {
	return racket_;
}

Ball* Game::GetBall() const {
	return ball_;
}

void Game::ResetScore() {
	score_ = 0;
}

double Game::GetScore() const {
	return score_;
}



bool Game::Render() {
	quat orientation;
	orientation.RotateAroundOwnX(-PIF/5);
	ui_manager_->SetCameraPosition(xform(orientation, vec3(0, -0.4f, 0.6f)));
	const PixelRect full_rect(0, 0, ui_manager_->GetCanvas()->GetWidth(), ui_manager_->GetCanvas()->GetHeight());
	ui_manager_->Render(full_rect);
	return true;
}

bool Game::Paint() {
	return true;
}



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
	(void)object;
	(void)ok;
}

void Game::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) {
	(void)body2_id;
	collision_sound_manager_->OnCollision(force, torque, position, object1, object2, body1_id, 2000, false);

	const float _force = force.GetLength();
	if (object1 == ball_ && _force > 1.0f) {
		score_ += ::sqrt(_force) * 34;
	}
}

bool Game::OnPhysicsSend(cure::ContextObject* object) {
	(void)object;
	return true;
}

bool Game::OnAttributeSend(cure::ContextObject* object) {
	(void)object;
	return true;
}

bool Game::IsServer() {
	return true;
}

void Game::SendAttach(cure::ContextObject* object1, unsigned id1, cure::ContextObject* object2, unsigned id2) {
	(void)object1;
	(void)id1;
	(void)object2;
	(void)id2;
}

void Game::SendDetach(cure::ContextObject* object1, cure::ContextObject* object2) {
	(void)object1;
	(void)object2;
}

void Game::TickInput() {
}

cure::ContextObject* Game::CreateContextObject(const str& class_id) const {
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
	return _ok;
}

bool Game::InitializeUniverse() {
	bool _ok = true;
	if (_ok) {
		delete racket_;
		racket_ = new Racket(GetResourceManager(), "racket", ui_manager_);
		AddContextObject(racket_, cure::kNetworkObjectLocalOnly, 0);
		_ok = (racket_ != 0);
		deb_assert(_ok);
		if (_ok) {
			racket_->SetInitialTransform(xform(quat(), vec3(0, 0, 0)));
			racket_->EnableRootShadow(false);
			racket_->StartLoading();
		}
	}
	if (_ok) {
		delete ball_;
		ball_ = new Ball(GetResourceManager(), "ball", ui_manager_);
		AddContextObject(ball_, cure::kNetworkObjectLocalOnly, 0);
		_ok = (ball_ != 0);
		deb_assert(_ok);
		if (_ok) {
			ball_->SetInitialTransform(xform(quat(), vec3(0, 0, 0.4f)));
			ball_->EnableRootShadow(false);
			ball_->StartLoading();
		}
	}
	return _ok;
}



cure::ContextObject* Game::CreateLogicHandler(const str& type) {
	(void)type;
	return 0;
}



loginstance(kGame, Game);



}
