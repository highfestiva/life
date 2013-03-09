
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



namespace Life
{
namespace Options
{



struct CamControl
{
	enum Direction
	{
		CAMDIR_UP = 0,
		CAMDIR_DOWN,
		CAMDIR_LEFT,
		CAMDIR_RIGHT,
		CAMDIR_FORWARD,
		CAMDIR_BACKWARD,
		CAMDIR_COUNT
	};
	float mControl[CAMDIR_COUNT];

	inline CamControl()
	{
		::memset(mControl, 0, sizeof(mControl));
	}

	inline void operator=(const CamControl& pControl)
	{
		::memcpy(mControl, pControl.mControl, sizeof(mControl));
	}
};



struct Steering
{
	enum Control
	{
		CONTROL_FORWARD = 0,
		CONTROL_BACKWARD,
		CONTROL_FORWARD3D,
		CONTROL_BACKWARD3D,
		CONTROL_BRAKEANDBACK,
		CONTROL_LEFT,
		CONTROL_RIGHT,
		CONTROL_LEFT3D,
		CONTROL_RIGHT3D,
		CONTROL_UP,
		CONTROL_DOWN,
		CONTROL_UP3D,
		CONTROL_DOWN3D,
		CONTROL_HANDBRAKE,
		CONTROL_BREAK,
		CONTROL_YAW_ANGLE,
		CONTROL_PITCH_ANGLE,
		CONTROL_COUNT
	};
	float mControl[CONTROL_COUNT];

	inline Steering()
	{
		::memset(mControl, 0, sizeof(mControl));
	}

	inline void operator=(const Steering& pVehicle)
	{
		::memcpy(mControl, pVehicle.mControl, sizeof(mControl));
	}

	inline float operator-(const Steering& pVehicle) const
	{
		float lSum = 0;
		for (int x = 0; x < CONTROL_COUNT; ++x)
		{
			lSum += fabs(mControl[x]-pVehicle.mControl[x]);
		}
		return (lSum);
	}
};



struct FireControl
{
	enum Control
	{
		FIRE0 = 0,
		FIRE1,
		FIRE2,
		FIRE_COUNT
	};
	float mControl[FIRE_COUNT];

	inline FireControl()
	{
		::memset(mControl, 0, sizeof(mControl));
	}

	inline void operator=(const FireControl& pShooting)
	{
		::memcpy(mControl, pShooting.mControl, sizeof(mControl));
	}
};



}
}
