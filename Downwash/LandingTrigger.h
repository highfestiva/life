
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/stunttrigger.h"



namespace Downwash {



class LandingTrigger: public life::StuntTrigger {
	typedef StuntTrigger Parent;
public:
	LandingTrigger(cure::ContextManager* manager);
	virtual ~LandingTrigger();

protected:
	virtual void DidTrigger(cure::ContextObject* body);
	virtual void OnAlarm(int alarm_id, void* extra_data);

	logclass();
};



}
