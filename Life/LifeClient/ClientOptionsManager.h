
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "OptionsManager.h"
#include "ClientOptions.h"



namespace Life
{
namespace Options
{



struct ClientOptionsManager: OptionsManager
{
	ClientOptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority);

	const Steering& GetSteeringControl() const;
	const CamControl& GetCamControl() const;

private:
	virtual bool SetDefault(int pPriority);
	virtual void DoRefreshConfiguration();

	Steering mSteeringControl;
	CamControl mCamControl;

	void operator=(const ClientOptionsManager&);

	LOG_CLASS_DECLARE();
};



}
}
