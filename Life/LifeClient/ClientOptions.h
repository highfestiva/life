
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



namespace Life
{
namespace Options
{



struct Vehicle
{
	enum Control
	{
		CONTROL_FORWARD = 0,
		CONTROL_BACKWARD,
		CONTROL_FORWARD3D,
		CONTROL_BACKWARD3D,
		CONTROL_BREAKANDBACK,
		CONTROL_LEFT,
		CONTROL_RIGHT,
		CONTROL_LEFT3D,
		CONTROL_RIGHT3D,
		CONTROL_UP,
		CONTROL_DOWN,
		CONTROL_UP3D,
		CONTROL_DOWN3D,
		CONTROL_HANDBREAK,
		CONTROL_BREAK,
		CONTROL_COUNT
	};
	float mControl[CONTROL_COUNT];

	inline Vehicle()
	{
		::memset(mControl, 0, sizeof(mControl));
	}

	inline void operator=(const Vehicle& pVehicle)
	{
		::memcpy(mControl, pVehicle.mControl, sizeof(mControl));
	}

	inline float operator-(const Vehicle& pVehicle) const
	{
		float lSum = 0;
		for (int x = 0; x < CONTROL_COUNT; ++x)
		{
			lSum += fabs(mControl[x]-pVehicle.mControl[x]);
		}
		return (lSum);
	}
};



}
}
