
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/StuntTrigger.h"



namespace Downwash
{



class LandingTrigger: public Life::StuntTrigger
{
	typedef StuntTrigger Parent;
public:
	LandingTrigger(Cure::ContextManager* pManager);
	virtual ~LandingTrigger();

protected:
	virtual void DidTrigger(Cure::ContextObject* pBody);
	virtual void OnAlarm(int pAlarmId, void* pExtraData);

	LOG_CLASS_DECLARE();
};



}
