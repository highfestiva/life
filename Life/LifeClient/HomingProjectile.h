
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "FastProjectile.h"



namespace Life
{



class HomingProjectile: public FastProjectile
{
public:
	typedef FastProjectile Parent;

	HomingProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher);
	virtual ~HomingProjectile();
	void SetTarget(Cure::GameObjectId pTarget);

protected:
	virtual void OnTick();

	Cure::GameObjectId mTarget;

	logclass();
};



}
