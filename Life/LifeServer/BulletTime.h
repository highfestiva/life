
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../StuntTrigger.h"



namespace Life
{



class BulletTime: public StuntTrigger
{
	typedef StuntTrigger Parent;
public:
	BulletTime(Cure::ContextManager* pManager);
	virtual ~BulletTime();

protected:
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	virtual void DidTrigger(Cure::ContextObject* pBody);

	logclass();
};



}
