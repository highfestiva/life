
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "basemachine.h"
#include "downwash.h"



namespace Downwash {



class CenteredMachine: public BaseMachine {
	typedef BaseMachine Parent;
public:
	CenteredMachine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, life::Launcher* launcher);
	virtual ~CenteredMachine();

protected:
	virtual bool TryComplete();
	virtual void OnMicroTick(float frame_time);

	logclass();
};



}
