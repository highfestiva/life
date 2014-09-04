
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiCure/Include/UiCppContextObject.h"
#include "Game.h"



namespace GrenadeRun
{



class SeeThrough: public UiCure::CppContextObject
{
	typedef UiCure::CppContextObject Parent;
public:
	SeeThrough(Cure::ContextManager* pManager, const Game* pGame);
	virtual ~SeeThrough();

private:
	virtual void SetTagIndex(int pIndex);
	virtual void OnTick();

	typedef Tbc::ChunkyClass::Tag Tag;
	const Game* mGame;
	const Tag* mTag;
	float mOpacity;

	logclass();
};



}
