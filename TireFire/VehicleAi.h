
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "Game.h"



namespace Cure
{
class Elevator;
}



namespace TireFire
{



class Game;



class VehicleAi: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	VehicleAi(Game* pGame);
	virtual ~VehicleAi();
	void Init();

private:
	enum Mode
	{
		MODE_FIND_BEST_PATH = 1,
		MODE_FIND_PATH_OFF_ELEVATOR,
		MODE_NORMAL,
		MODE_HEADING_BACK_ON_TRACK,
		MODE_BACKING_UP,
		MODE_BACKING_UP_TO_GOAL,
		MODE_FLEE,
		MODE_STOPPING_AT_GOAL,
		MODE_AT_GOAL,
		MODE_WAITING_FOR_ELEVATOR,
		MODE_GET_ON_ELEVATOR,
		MODE_GET_OFF_ELEVATOR,
		MODE_ON_ELEVATOR,
		MODE_ROTATE_ON_THE_SPOT,
		MODE_ROTATE_ON_THE_SPOT_DURING,
		MODE_ROTATE_ON_THE_SPOT_WAITING,
	};

	virtual void OnTick();
	void SetMode(Mode pMode);
	bool IsCloseToTarget(const vec3& pPosition, float pDistance) const;
	float GetClosestPathDistance(const vec3& pPosition, const int pPath = -1, float* pLikeliness = 0, float pSteepFactor = 5) const;
	vec3 GetClosestElevatorPosition(const vec3& pPosition, const Cure::Elevator*& pNearestElevator) const;
	bool HasElevatorArrived(const Cure::Elevator*& pNearestElevator, const float pPositionZ, vec3& pNearestLiftPosition2d, float& pElevatorXyDistance2ToElevatorStop);
	float GetClosestElevatorRadius() const;
	static bool IsVertical(const vec3& pVector);
	float GetRelativeDriveOnAngle(const vec3& pDirection) const;
	bool QueryVehicleHindered(const Cure::TimeManager* pTime, const vec3& pVelocity);

	Game* mGame;
	Mode mPreviousMode;
	Mode mMode;
	int mModeStartFrame;
	int mStoppedFrame;
	int mActivePath;
	int mStuckCount;
	float mLastAverageAngle;
	float mRotateAngle;
	vec3 mElevatorGetOnPosition;
	vec3 mElevatorGetOffPosition;

	logclass();
};



}
