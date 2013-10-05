
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/Level.h"
#include "Downwash.h"



namespace Cure
{
class ContextPath;
}



namespace Downwash
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
