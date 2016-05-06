
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../stunttrigger.h"



namespace life {



class BulletTime: public StuntTrigger {
	typedef StuntTrigger Parent;
public:
	BulletTime(cure::ContextManager* manager);
	virtual ~BulletTime();

protected:
	virtual void OnAlarm(int alarm_id, void* extra_data);
	virtual void DidTrigger(cure::ContextObject* body);

	logclass();
};



}
