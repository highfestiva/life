
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "launcher.h"
#include "../tbc/include/physicsengine.h"
#include "../uicure/include/uigameuimanager.h"



namespace grenaderun {



Launcher::Launcher(Game* game):
	Parent(game->GetResourceManager(), "launcher", game->GetUiManager()),
	game_(game) {
	SetForceLoadUnique(true);	// Needs to be unique as we set unique engines per view.
}

Launcher::~Launcher() {
}



void Launcher::SetBarrelAngle(float yaw, float pitch) {
	quat quaternion;
	quaternion.RotateAroundWorldZ(yaw);
	quaternion.RotateAroundOwnX(pitch);
	SetRootOrientation(quaternion);

	// Keep supportive thingie on ground.
	tbc::GeometryReference* mesh = (tbc::GeometryReference*)GetMesh(1);
	static bool has_stored_transformation = false;
	static quat original_orientation;
	if (!has_stored_transformation) {
		original_orientation = mesh->GetOffsetTransformation().GetOrientation();
		has_stored_transformation = true;
	}
	quaternion = original_orientation;
	quaternion.RotateAroundOwnX(-pitch);
	xform transform = mesh->GetOffsetTransformation();
	transform.SetOrientation(quaternion);
	mesh->SetOffsetTransformation(transform);
}



void Launcher::GetAngles(const cure::ContextObject* target, float& pitch, float& guide_pitch,
	float& yaw, float& guide_yaw) const {
	GetAngles(target->GetPosition(), target->GetVelocity(), pitch, guide_pitch, yaw, guide_yaw);
}

void Launcher::GetAngles(const vec3& target_position, const vec3& target_velocity,
	float& pitch, float& guide_pitch, float& yaw, float& guide_yaw) const {
	// GetBallisticData calculates the trajectory by polynome approximation (don't remember
	// the math any more), but calling it twice gets us pretty close to the sweet spot.
	vec3 _position1 = target_position;
	const vec3 _position2 = this->GetPosition();
	float roll;
	this->GetOrientation().GetEulerAngles(yaw, pitch, roll);
	float _time = 10.0f;
	GetBallisticData(_position1, _position2, pitch, guide_pitch, guide_yaw, _time);
	float better_pitch = guide_pitch;
	{
		vec3 p = _position1 + target_velocity * _time;
		GetBallisticData(p, _position2, better_pitch, guide_pitch, guide_yaw, _time);
	}
	better_pitch = guide_pitch;
	_position1 = _position1 + target_velocity * _time;
	GetBallisticData(_position1, _position2, better_pitch, guide_pitch, guide_yaw, _time);
	guide_pitch = Math::Clamp(guide_pitch, -PIF/2, 0.0f);
	//guide_yaw = Math::Clamp(guide_yaw, -PIF/2, PIF/2);
	const float yaw_delta = guide_yaw - yaw;
	if (yaw_delta > +PIF) {
		guide_yaw -= 2*PIF;
	} else if (yaw_delta < -PIF) {
		guide_yaw += 2*PIF;
	}
}

void Launcher::CreateEngines() {
	deb_assert(GetPhysics()->GetEngineCount() == 0);
	tbc::PhysicsEngine* pitch_engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineRotorTilt, 1, 1, 1, 1, 0);
	GetPhysics()->AddEngine(pitch_engine);
	tbc::PhysicsEngine* yaw_engine = new tbc::PhysicsEngine(tbc::PhysicsEngine::kEngineHingeRoll, 1, 1, 1, 1, 1);
	GetPhysics()->AddEngine(yaw_engine);
}

void Launcher::GetBallisticData(const vec3& position1, const vec3& position2,
	float pitch, float& guide_pitch, float& guide_yaw, float &time) const {
	const vec3 delta = position1 - position2;
	const vec2 yaw_vector(delta.x, delta.y);
	guide_yaw = vec2(0, 1).GetAngle(yaw_vector);

	const float h = delta.z;
	const float v = game_->GetMuzzleVelocity();
	const float vup = v * ::cos(pitch);
	// g*t^2/2 - vup*t + h = 0
	//
	// Quaderatic formula:
	// ax^2 + bx + c = 0
	// =>
	//     -b +- sqrt(b^2 - 4ac)
	// x = ---------------------
	//             2a
	const float a = +9.82f/2;
	const float b = -vup;
	const float c = +h;
	const float b2 = b*b;
	const float _4ac = 4*a*c;
	if (b2 < _4ac) {	// Does not compute.
		guide_pitch = -PIF/4;
	} else {
		const float t = (-b + sqrt(b2 - _4ac)) / (2*a);
		//deb_assert(t > 0);
		time = t;
		const float vfwd = yaw_vector.GetLength() / t;
		guide_pitch = -::atan(vfwd/vup);
		if (guide_pitch < pitch) {	// Aiming downwards?
			guide_pitch += (guide_pitch-pitch);	// Tss! Homebrew... seems to be working somewhat! :)
		}
	}
}



void Launcher::DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* mesh_resource) {
	ui_manager_->GetRenderer()->SetShadows(mesh_resource->GetData(), uitbc::Renderer::kForceNoShadows);
	Parent::DispatchOnLoadMesh(mesh_resource);
}



}
