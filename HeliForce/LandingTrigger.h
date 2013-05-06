
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Life/StuntTrigger.h"



namespace HeliForce
{



class LandingTrigger: public Life::StuntTrigger
{
	typedef StuntTrigger Parent;
public:
	LandingTrigger(Cure::ContextManager* pManager);
	virtual ~LandingTrigger();

protected:
	virtual void DidTrigger();
	virtual void OnAlarm(int pAlarmId, void* pExtraData);

	LOG_CLASS_DECLARE();
};



}
