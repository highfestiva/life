
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiCppContextObject.h"
#include "../Life.h"



namespace Life
{



class Vehicle: public UiCure::CppContextObject
{
	typedef UiCure::CppContextObject Parent;
public:
	Vehicle(const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Vehicle();

protected:
	void OnPhysicsTick();

private:
	LOG_CLASS_DECLARE();
};



}
