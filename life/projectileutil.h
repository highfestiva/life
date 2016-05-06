
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/transformation.h"
#include "life.h"



namespace cure {
class ContextManager;
class ContextObject;
class CppContextObject;
class GameManager;
}



namespace life {



class Launcher;



class ProjectileUtil {
public:
	static bool GetBarrel(cure::ContextObject* projectile, xform& transform, vec3& velocity);
	static bool GetBarrelByShooter(cure::CppContextObject* shooter, xform& transform, vec3& velocity);
	static void StartBullet(cure::ContextObject* bullet, float muzzle_velocity, bool use_barrel);
	static void BulletMicroTick(cure::ContextObject* bullet, float frame_time, float max_velocity, float acceleration);
	static void Detonate(cure::ContextObject* grenade, bool* is_detonated, Launcher* launcher, const vec3& position, const vec3& velocity, const vec3& normal, float strength, float delete_delay);
	static void OnBulletHit(cure::ContextObject* bullet, bool* is_detonated, Launcher* launcher, cure::ContextObject* target);
	static float GetShotSounds(cure::ContextManager* manager, const strutil::strvec& sound_names, str& launch_sound_name, str& shreek_sound_name);

	static vec3 CalculateInitialProjectileDirection(const vec3& distance, float acceleration, float terminal_speed, const vec3& gravity, float acceleration_gravity_recip);
private:
	logclass();
};



}
