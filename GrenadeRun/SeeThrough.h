
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "../Lepra/Include/HiResTimer.h"
#include "Game.h"



namespace GrenadeRun
{



class SeeThrough: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	SeeThrough(Cure::ContextManager* pManager, const Game* pGame);
	virtual ~SeeThrough();

private:
	void SetTagIndex(int pIndex);
	virtual void OnTick();

	typedef TBC::ChunkyClass::Tag Tag;
	const Game* mGame;
	const Tag* mTag;

	LOG_CLASS_DECLARE();
};



}
