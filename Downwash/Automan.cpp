
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "automan.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"



namespace Downwash {



Automan::Automan(cure::GameManager* game, cure::GameObjectId car_id, const vec3& direction):
	Parent(game->GetResourceManager(), "Automan"),
	car_id_(car_id),
	direction_(direction) {
	game->GetContext()->AddLocalObject(this);
	game->GetContext()->EnableTickCallback(this);
}

Automan::~Automan() {
}



void Automan::OnTick() {
	Parent::OnTick();

	cure::ContextObject* car = manager_->GetObject(car_id_, true);
	if (!car) {
		manager_->PostKillObject(GetInstanceId());
		return;
	}
	if (!car->IsLoaded()) {
		return;
	}
	car->SetEnginePower(0, 1);
	const vec2 wanted_direction(direction_.x, direction_.y);
	const vec3 car_direction3d = car->GetOrientation()*vec3(0,1,0);
	const vec2 car_direction(car_direction3d.x, car_direction3d.y);
	const float angle = wanted_direction.GetAngle(car_direction);
	car->SetEnginePower(1, angle);

	if (car->GetVelocity().GetLengthSquared() < 1.0f) {
		still_timer_.TryStart();
		if (still_timer_.QueryTimeDiff() > 4.0f) {
			cure::Health::Set(car, 0);
		}
	} else {
		still_timer_.Stop();
	}
}



}
