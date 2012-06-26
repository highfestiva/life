
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



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
	bool IsCloseToTarget(const Vector3DF& pPosition, float pDistance) const;
	float GetClosestPathDistance(const Vector3DF& pPosition, const int pPath = -1, float* pLikeliness = 0, float pSteepFactor = 5) const;
	Vector3DF GetClosestElevatorPosition(const Vector3DF& pPosition, const Cure::Elevator*& pNearestElevator) const;
	bool HasElevatorArrived(const Cure::Elevator*& pNearestElevator, const float pPositionZ, Vector3DF& pNearestLiftPosition2d, float& pElevatorXyDistance2ToElevatorStop);
	float GetClosestElevatorRadius() const;
	static bool IsVertical(const Vector3DF& pVector);
	float GetRelativeDriveOnAngle(const Vector3DF& pDirection) const;
	bool QueryVehicleHindered(const Cure::TimeManager* pTime, const Vector3DF& pVelocity);

	Game* mGame;
	Mode mPreviousMode;
	Mode mMode;
	int mModeStartFrame;
	int mStoppedFrame;
	int mActivePath;
	int mStuckCount;
	float mLastAverageAngle;
	float mRotateAngle;
	Vector3DF mElevatorGetOnPosition;
	Vector3DF mElevatorGetOffPosition;

	LOG_CLASS_DECLARE();
};



}
