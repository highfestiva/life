
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "Vehicle.h"



namespace Life
{



class Props: public Vehicle
{
public:
	typedef Vehicle Parent;

	Props(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Props();

protected:
	void OnPhysicsTick();
};



}
