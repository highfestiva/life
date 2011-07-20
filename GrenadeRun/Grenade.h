
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"



namespace GrenadeRun
{



using namespace Lepra;



class Grenade: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Grenade(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Grenade();

	void Start();

private:
	void OnAlarm(int, void*);

	LOG_CLASS_DECLARE();
};



}
