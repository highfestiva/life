
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Bound.h"



namespace Bound
{



class Ball: public UiCure::Machine
{
	typedef UiCure::Machine Parent;
public:
	Ball(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Ball();

	virtual void OnTick();

private:
};



}
