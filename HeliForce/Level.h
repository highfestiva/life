
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Life/LifeClient/Level.h"
#include "HeliForce.h"



namespace Cure
{
class ContextPath;
}



namespace HeliForce
{



class Level: public Life::Level
{
	typedef Life::Level Parent;
public:
	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter);
	virtual ~Level();

	virtual void OnLoaded();
	const str& GetBackgroundName() const;
	Cure::ContextPath* QueryPath();

private:
	str mBackgroundName;
	Cure::ContextPath* mPath;
};



}
