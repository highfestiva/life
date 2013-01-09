
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

	const bool GetShowScore() const;
	const Steering& GetSteeringControl() const;
	const CamControl& GetCamControl() const;
	const FireControl& GetFireControl() const;

	virtual void DoRefreshConfiguration();

private:
	virtual bool SetDefault(int pPriority);

	float mShowScore;
	Steering mSteeringControl;
	CamControl mCamControl;
	FireControl mFireControl;

	void operator=(const ClientOptionsManager&);

	LOG_CLASS_DECLARE();
};



}
}
