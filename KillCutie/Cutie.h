
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"



namespace GrenadeRun
{



using namespace Lepra;



class Cutie: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Cutie(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Cutie();

	void DrainHealth(float pDrain);
	float GetHealth() const;
	bool QueryFlip();
	bool IsUpsideDown() const;

private:
	virtual void OnTick();

	float mHealth;
	int mKillJointsTickCount;
	int mWheelExpelTickCount;

	logclass();
};



}
