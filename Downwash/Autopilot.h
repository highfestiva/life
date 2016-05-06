
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/contextpath.h"
#include "../cure/include/cppcontextobject.h"
#include "downwashmanager.h"



namespace Downwash {



class DownwashManager;



class Autopilot: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	Autopilot(DownwashManager* game);
	virtual ~Autopilot();
	void Reset();
	vec3 GetSteering();
	void AttemptCloserPathDistance();
	float GetClosestPathDistance() const;
	vec3 GetClosestPathVector() const;
	vec3 GetLastAvatarPosition() const;
	float GetRotorSpeed(const cure::ContextObject* chopper) const;

private:
	void CheckStalledRotor(cure::ContextObject* chopper);
	float GetClosestPathDistance(const vec3& position, vec3& closest_point) const;

	typedef cure::ContextPath::SplinePath Spline;

	DownwashManager* game_;
	GameTimer stalled_rotor_timer_;
	float closest_path_distance_;
	vec3 closest_path_position_;
	vec3 last_avatar_position_;
	Spline* path_;
	logclass();
};



}
