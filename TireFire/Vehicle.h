
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"



namespace TireFire
{



using namespace Lepra;



class Vehicle: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Vehicle(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Vehicle();

	void DrainHealth(float pDrain);
	float GetHealth() const;
	bool QueryFlip();
	bool IsUpsideDown() const;

private:
	virtual void OnTick();

	float mHealth;
	int mKillJointsTickCount;
	int mWheelExpelTickCount;

	LOG_CLASS_DECLARE();
};



}
