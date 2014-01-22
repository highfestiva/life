
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ExplodingMachine.h"
#include "Fire.h"



namespace Fire
{



class BaseMachine: public Life::ExplodingMachine
{
	typedef Life::ExplodingMachine Parent;
public:
	BaseMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher);
	virtual ~BaseMachine();

	float mLevelSpeed;
	float mPanicLevel;

protected:
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	virtual void OnDie();
};



}
