
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"
#include "game.h"



namespace cure {
class Elevator;
}



namespace tirefire {



class Game;



class VehicleAi: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	VehicleAi(Game* game);
	virtual ~VehicleAi();
	void Init();

private:
	enum Mode {
		kModeFindBestPath = 1,
		kModeFindPathOffElevator,
		kModeNormal,
		kModeHeadingBackOnTrack,
		kModeBackingUp,
		kModeBackingUpToGoal,
		kModeFlee,
		kModeStoppingAtGoal,
		kModeAtGoal,
		kModeWaitingForElevator,
		kModeGetOnElevator,
		kModeGetOffElevator,
		kModeOnElevator,
		kModeRotateOnTheSpot,
		kModeRotateOnTheSpotDuring,
		kModeRotateOnTheSpotWaiting,
	};

	virtual void OnTick();
	void SetMode(Mode mode);
	bool IsCloseToTarget(const vec3& position, float distance) const;
	float GetClosestPathDistance(const vec3& position, const int path = -1, float* likeliness = 0, float steep_factor = 5) const;
	vec3 GetClosestElevatorPosition(const vec3& position, const cure::Elevator*& nearest_elevator) const;
	bool HasElevatorArrived(const cure::Elevator*& nearest_elevator, const float position_z, vec3& nearest_lift_position2d, float& elevator_xy_distance2_to_elevator_stop);
	float GetClosestElevatorRadius() const;
	static bool IsVertical(const vec3& vector);
	float GetRelativeDriveOnAngle(const vec3& direction) const;
	bool QueryVehicleHindered(const cure::TimeManager* time, const vec3& velocity);

	Game* game_;
	Mode previous_mode_;
	Mode mode_;
	int mode_start_frame_;
	int stopped_frame_;
	int active_path_;
	int stuck_count_;
	float last_average_angle_;
	float rotate_angle_;
	vec3 elevator_get_on_position_;
	vec3 elevator_get_off_position_;

	logclass();
};



}
