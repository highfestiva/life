
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "npc.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/intattribute.h"
#include "../rtvar.h"
#include "gameserverlogic.h"

#define AMMO_VELOCITY_M_PER_S	500.0f	// TODO: remove hard-coding!



namespace HoverTank {



Npc::Npc(GameServerLogic* logic):
	Parent(0, "npc"),
	logic_(logic),
	intelligence_(0.5f),
	avatar_id_(0) {
	SetLoadResult(true);
}

Npc::~Npc() {
	if (avatar_id_) {
		GetManager()->PostKillObject(avatar_id_);
		avatar_id_ = 0;
	}
}



cure::GameObjectId Npc::GetAvatarId() const {
	return avatar_id_;
}

void Npc::SetAvatarId(cure::GameObjectId avatar_id) {
	avatar_id_ = avatar_id;
	if (avatar_id_) {
		GetManager()->EnableTickCallback(this);
	} else {
		StartCreateAvatar(10.0f);
	}
}

void Npc::StartCreateAvatar(float time) {
	GetManager()->DisableTickCallback(this);
	GetManager()->AddGameAlarmCallback(this, 5, time, 0);
}



void Npc::OnTick() {
	Parent::OnTick();

	if (!avatar_id_) {
		return;
	}
	cure::ContextObject* my_avatar = GetManager()->GetObject(avatar_id_);
	if (!my_avatar) {
		return;
	}
	cure::IntAttribute* team = (cure::IntAttribute*)my_avatar->GetAttribute("int_team");
	if (!team) {
		return;
	}

	const int other_team = team->GetValue()? 0 : 1;
	const GameServerLogic::AvatarIdSet avatar_id_set = logic_->GetAvatarsInTeam(other_team);
	cure::ContextObject* target = 0;
	if (!avatar_id_set.empty()) {
		target = GetManager()->GetObject(*avatar_id_set.begin());
	}
	float fwd = 0;
	float right = 0;
	float phi = 0;
	bool can_shoot = false;
	vec3 up(0, 0, 1);
	up = my_avatar->GetOrientation() * up;
	if (target && up.z > 0.2f) {	// Just steer+shoot if we're "standing up".
		vec3 diff = target->GetPosition() - my_avatar->GetPosition();
		float distance = diff.GetLength();
		const float time_until_hit = intelligence_ * distance / AMMO_VELOCITY_M_PER_S;
		diff = (target->GetPosition() + target->GetVelocity()*time_until_hit) - (my_avatar->GetPosition() + my_avatar->GetVelocity()*time_until_hit);

		float _;
		diff.GetSphericalAngles(_, phi);
		phi += PIF/2;
		float yaw;
		my_avatar->GetOrientation().GetEulerAngles(yaw, _, _);
		yaw -= PIF;
		Math::RangeAngles(phi, yaw);
		phi -= yaw;
		phi = -phi;
		const float rotation = phi;
		if (std::abs(phi) < 0.02f) {
			phi = 0;
		}
		phi *= 4.0f;
		phi *= std::abs(phi);

		distance = diff.GetLength();
		if (distance > 15 && std::abs(diff.z) < 2) {	// TODO: implement different shooting pattern for other weapons.
			can_shoot = true;	// Allow grenade shooting if out of range.
		}
		if (distance > 100 && std::abs(rotation) < 0.4f) {
			fwd = +1.0f;	// Head towards the target if too far away.
		} else if (distance < 30 && std::abs(rotation) < 0.4f) {
			fwd = -1.0f;	// Move away if too close.
		}
		if (rotation != 0) {
			right = 5.0f * rotation;	// Strafe if slightly off.
			right *= (std::abs(rotation) < 0.4f)? 1 : -1;
		}
		const float steering_power = std::abs(Math::Clamp(fwd, -1.0f, +1.0f));
		phi *= Math::Lerp(0.8f, 2.0f, steering_power);

		fwd *= intelligence_;
		right *= intelligence_;
		phi /= intelligence_;
	}
	deb_assert(phi >= -10000);
	deb_assert(phi <= +10000);
	my_avatar->SetEnginePower(0, Math::Clamp(fwd-phi, -1.0f, +1.0f));
	my_avatar->SetEnginePower(1, Math::Clamp(right, -1.0f, +1.0f));
	my_avatar->SetEnginePower(4, Math::Clamp(fwd+phi, -1.0f, +1.0f));
	my_avatar->SetEnginePower(5, Math::Clamp(right, -1.0f, +1.0f));
	my_avatar->SetEnginePower(8, Math::Clamp(+phi, -1.0f, +1.0f));

	if (can_shoot &&
		std::abs(phi) < Math::Lerp(0.1f, 0.015f, intelligence_) &&
		shoot_wait_.QueryTimeDiff() > Math::Lerp(10.0, 0.3, intelligence_)) {
		shoot_wait_.ClearTimeDiff();
		logic_->Shoot(my_avatar, 1);
	}
}

void Npc::OnAlarm(int alarm_id, void* extra_data) {
	(void)extra_data;

	if (alarm_id == 5) {
		v_get(intelligence_, =(float), GetManager()->GetGameManager()->GetVariableScope(), kRtvarGameNpcskill, 0.5f);
		intelligence_ = Math::Clamp(intelligence_, 0.1f, 1.0f);
		if (!logic_->CreateAvatarForNpc(this)) {
			StartCreateAvatar(10.0f);
		}
	}
}



loginstance(kGameContextCpp, Npc);



}
