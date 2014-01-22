
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/Level.h"
#include "Fire.h"



namespace Cure
{
class ContextPath;
}



namespace Fire
{



class Level: public Life::Level
{
	typedef Life::Level Parent;
public:
	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter);
	virtual ~Level();

	virtual void OnLoaded();
	Cure::ContextPath* QueryPath();
	float GetLevelSpeed() const;

private:
	Cure::ContextPath* mPath;
	float mLevelSpeed;
};



}
