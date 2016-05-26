
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "launcherai.h"
#include "../cure/include/contextmanager.h"
#include "../lepra/include/random.h"
#include "../tbc/include/physicsengine.h"
#include "ctf.h"
#include "cutie.h"
#include "game.h"
#include "launcher.h"



namespace grenaderun {



LauncherAi::LauncherAi(Game* game):
	Parent(game->GetResourceManager(), "LauncherAi"),
	game_(game),
	did_shoot_(false),
	shot_count_(0) {
}

LauncherAi::~LauncherAi() {
}

void LauncherAi::Init() {
	GetManager()->EnableTickCallback(this);
}



void LauncherAi::OnTick() {
	if (game_->GetFlybyMode() != Game::kFlybyInactive ||
		!game_->GetLauncher() || !game_->GetLauncher()->IsLoaded() ||
		!game_->GetCutie() || !game_->GetCutie()->IsLoaded()) {
		return;
	}

	vec3 target_position = game_->GetCutie()->GetPosition();
	vec3 target_velocity = (game_->GetCutie()->GetVelocity() +
		game_->GetCutie()->GetAcceleration() * 7) * 1.1f;
	const float target_speed = target_velocity.GetLength();
	const vec3 ctf_position = game_->GetCtf()->GetPosition();
	vec3 direction = ctf_position-target_position;
	const float ctf_distance = direction.GetLength();
	direction /= ctf_distance;
	const bool heading_towards_ctf = (direction.Dot(target_velocity/target_speed) > 0.8f);
	bool adjusted_for_slowing_down = false;
	const float difficulty = game_->GetComputerDifficulty();
	bool handled = false;
	if (did_shoot_ && game_->IsLauncherBarrelFree()) {
		log_.Headline("Fire in the hole!");
		++shot_count_;
		did_shoot_ = false;
	}
	if (difficulty > 0.9f) {
		if (ctf_distance < 60*kScaleFactor && ctf_position.z-target_position.z < 7) {
			handled = true;
			if (shot_count_ > 2) {
				shot_count_ = 0;
			}
			// Alternate between firing twice at CTF platform and once at vehicle.
			if (shot_count_ <= 1) {
				target_position = ctf_position;
				target_position.x += -direction.x * (2 * kScaleFactor) + Random::Uniform(-0.3f*kScaleFactor, 0.3f*kScaleFactor);
				target_position.y += -direction.y * (2 * kScaleFactor) + Random::Uniform(-0.3f*kScaleFactor, 0.3f*kScaleFactor);
				target_velocity.Set(0, 0, 0);
				target_offset_.Set(0, 0, 0);
				adjusted_for_slowing_down = true;
				log_.Headline("Shooting at CTF platform!");
			} else if (heading_towards_ctf) {	// Only assume slowdown if going towards our goal.
				target_velocity *= 0.3f;
				target_offset_.Set(0, 0, 0);
				adjusted_for_slowing_down = true;
				log_.Headline("Shooting at slowing vehicle!");
			} else {
				target_offset_.Set(0, 0, 0);
				log_.Headline("Shooting at vehicle plain and simple!");
			}
		}
	} else {
		if (ctf_distance < 20*kScaleFactor) {
			handled = true;
			// She's close, assume she's going to be close and brake hard soon.
			//target_position = (target_position+ctf_position) * 0.5f;
			target_velocity *= 0.3f;
			adjusted_for_slowing_down = true;
		}
	}
	if (!handled && heading_towards_ctf) {
		if (ctf_distance < 170*kScaleFactor) {
			if (ctf_position.z-target_position.z > 7) {
				// Nevermind - this gal is far below the goal. Keep aiming at her instead of camping at the goal.
			} else if (target_speed * 10 > ctf_distance * 1.2f) {
				// She's probably going to reach the target and brake hard,
				// so don't aim too far ahead.
				target_velocity *= 0.4f;
				adjusted_for_slowing_down = true;
			} else if (target_speed * 10 > ctf_distance) {
				// She's going fast, but not superfast and thus she'll brake
				// hard, so don't aim too far ahead.
				target_velocity *= 0.7f;
				adjusted_for_slowing_down = true;
			}
		}
	}

	float pitch;
	float guide_pitch;
	float yaw;
	float guide_yaw;
	game_->GetLauncher()->GetAngles(target_position+target_offset_, target_velocity, pitch, guide_pitch, yaw, guide_yaw);
	const float velocity = target_velocity.GetLength();
	const float pitch_factor = Math::Clamp((float)::fabs((pitch - guide_pitch) * (velocity+3) + 0.03f), -2.0f, +2.0f);
	if (pitch < guide_pitch) {
		game_->GetLauncher()->SetEnginePower(0, -1*pitch_factor);
	} else {
		game_->GetLauncher()->SetEnginePower(0, +1*pitch_factor);
	}
	const float yaw_factor = Math::Clamp((float)::fabs((yaw - guide_yaw) * (velocity+4) + 0.03f), -2.0f, +2.0f);
	if (yaw < guide_yaw) {
		game_->GetLauncher()->SetEnginePower(1, -1*yaw_factor);
	} else {
		game_->GetLauncher()->SetEnginePower(1, +1*yaw_factor);
	}
	const float longest_time_base = 8.0f * (1-difficulty*0.8f);
	const double last_shot_diff = last_shot_.QueryTimeDiff();
	if (last_shot_diff > longest_time_base &&	// Wait at least this long.
		((yaw_factor < 0.1f && pitch_factor < 0.1f) ||	// In range.
		last_shot_diff > longest_time_base*2)) {
		if (game_->Shoot()) {
			did_shoot_ = true;
			last_shot_.ClearTimeDiff();
			if (difficulty >= 0.7f) {
				// Good player has some tactics.
				if (target_speed > 2*kScaleFactor && !adjusted_for_slowing_down &&
					game_->GetCutie()->GetPhysics()->GetEngineCount() >= 2) {
					// Guess direction depending on steering.
					const float angle = game_->GetCutie()->GetPhysics()->GetEngine(1)->GetValue();
					vec3 direction = game_->GetCutie()->GetForwardDirection();
					quat rotation(angle*-0.5f, vec3(0, 0, 1));
					direction = rotation * direction;
					direction.z = 0;
					const float adjusted_target_speed = (target_speed > 10)? 10 : target_speed;
					target_offset_ = direction * adjusted_target_speed * 1.0f;
					if (target_speed < 30) {
						target_offset_ += target_velocity * 2.0f;
					}
				} else {
					target_offset_.Set(0, 0, 0);
				}
			} else if (difficulty <= 0.3f) {
				// Poor player spreads 'em a lot.
				const float o = 16*kScaleFactor / (0.8f+difficulty*8);
				target_offset_.Set(Random::Uniform(-o, o), Random::Uniform(-o, o), 0);
			} else {
				// Average computer player always spreads 'em a little.
				const float o = 5*kScaleFactor;
				target_offset_.Set(Random::Uniform(-o, o), Random::Uniform(-o, o), 0);
			}
		}
	}
}



loginstance(kGameContextCpp, LauncherAi);



}
