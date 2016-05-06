
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "roboball.h"
#include "../tbc/include/physicsengine.h"
#include "cutie.h"



namespace grenaderun {



RoboBall::RoboBall(const Game* game, const str& class_id):
	Parent(game->GetResourceManager(), class_id, game->GetUiManager()),
	game_(game),
	sound_(0) {
	head_away_timer_.EnableShadowCounter(true);
}

RoboBall::~RoboBall() {
	delete sound_;
	sound_ = 0;
	game_ = 0;
}



void RoboBall::OnTick() {
	Parent::OnTick();

	if (game_->GetCutie() && game_->GetCutie()->IsLoaded()) {
		float strength = Math::Lerp(0.25f, 1.0f, game_->GetComputerDifficulty());
		const vec3 position = GetPosition();
		const vec3 cutie_position = game_->GetCutie()->GetPosition();
		const vec3 direction = cutie_position - position;
		vec3 steer_direction;
		if (head_away_timer_.QueryTimeDiff() > 2.0) {
			const vec3 velocity = GetVelocity();
			const vec3 cutie_velocity = game_->GetCutie()->GetVelocity();
			const float distance = position.GetDistance(cutie_position);
			const float speed = velocity.GetLength();
			if (speed < 0.1f) {
				return;
			}
			const float time_til_impact = Math::Clamp(distance / speed, 0.1f, 3.0f);
			const float speed_diff2 = velocity.GetDistanceSquared(cutie_velocity);
			if ((time_til_impact < 1 || speed < 1) &&
				distance < 10 &&
				speed_diff2 < 1) {
				log_debug("Too close and too slow, backing up!");
				head_away_timer_.PopTimeDiff();
			}
			const vec3 future_position = position + velocity*time_til_impact;
			const vec3 future_cutie_position = cutie_position + cutie_velocity*time_til_impact;
			const vec3 future_direction = future_cutie_position - future_position;
			if (future_direction.Dot(direction) > 1) {
				steer_direction = future_direction;
			} else {
				// Just use the current position, to not get a vector of opposite direction when
				// the future position indicates we will have passed the target. Which hopefully
				// ends up in a big crash! :)
				steer_direction = direction;
			}
			if (strength < 0.6f && distance > 30.0f && speed < 2) {
				++bad_speed_counter_;
			} else {
				bad_speed_counter_ = 0;
			}
		} else {
			steer_direction = vec3(100, 100, 13) - position;
		}
		if (bad_speed_counter_ > 2) {
			strength = 0.6f;
		}
		steer_direction.z = 0;
		steer_direction.Normalize();
		GetPhysics()->GetEngine(0)->SetValue(0, steer_direction.y * strength);
		GetPhysics()->GetEngine(0)->SetValue(1, steer_direction.x * strength);
		static int c = 0;
		if (++c > 20) {
			c = 0;
			log_volatile(log_.Debugf("RoboBall at (%.1f, %.1f, %.1f), heading towards (%.1f, %.1f), diff (%.1f, %.1f, %.1f).",
				position.x, position.y, position.z,
				steer_direction.x, steer_direction.y,
				direction.x, direction.y, direction.z));
		}

		static int d = 0;
		if (++d > 20) {
			d = 0;
			if (position.z < -50 || position.z > 200) {
				log_.Warning("Fell off of playing field, resetting position!");
				const cure::ObjectPositionalData* placement;
				UpdateFullPosition(placement);
				cure::ObjectPositionalData* new_placement = (cure::ObjectPositionalData*)placement->Clone();
				new_placement->position_.transformation_.GetPosition().Set(0, 0, 30);
				new_placement->position_.velocity_.Set(0, 0, 0);
				SetFullPosition(*new_placement, 0);
			}
		}
	}
}


loginstance(kGameContextCpp, RoboBall);



}
