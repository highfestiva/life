
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../UiCure/Include/UiMachine.h"
#include "../Life.h"



namespace Life
{



class Launcher;



class ExplodingMachine: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	ExplodingMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher);
	virtual ~ExplodingMachine();

	void SetExplosiveStrength(float pExplosiveStrength);
	void SetDeathFrameDelay(int pDeathFrameDelay);
	void SetDisappearAfterDeathDelay(float pDisappearDelay);

protected:
	virtual void OnTick();
	virtual void OnDie();

	Launcher* mLauncher;
	int mTriggerDeathFrame;
	int mDeathFrameDelay;
	float mDisappearAfterDeathDelay;
	bool mIsDetonated;
	float mExplosiveStrength;

	LOG_CLASS_DECLARE();
};



}
