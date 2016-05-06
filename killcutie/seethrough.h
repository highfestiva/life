
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/hirestimer.h"
#include "../uicure/include/uicppcontextobject.h"
#include "game.h"



namespace grenaderun {



class SeeThrough: public UiCure::CppContextObject {
	typedef UiCure::CppContextObject Parent;
public:
	SeeThrough(cure::ContextManager* manager, const Game* game);
	virtual ~SeeThrough();

private:
	virtual void SetTagIndex(int index);
	virtual void OnTick();

	typedef tbc::ChunkyClass::Tag Tag;
	const Game* game_;
	const Tag* tag_;
	float opacity_;

	logclass();
};



}
