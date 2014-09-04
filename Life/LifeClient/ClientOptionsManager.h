
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	virtual bool UpdateInput(UiLepra::InputElement* pElement);

private:
	virtual bool SetDefault(int pPriority);

	float mMouseSensitivity;
	float mShowScore;
	Steering mSteeringControl;
	CamControl mCamControl;
	FireControl mFireControl;

	void operator=(const ClientOptionsManager&);

	logclass();
};



}
}
