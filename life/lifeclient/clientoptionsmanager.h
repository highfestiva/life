
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "optionsmanager.h"
#include "clientoptions.h"



namespace life {
namespace options {



struct ClientOptionsManager: OptionsManager {
	ClientOptionsManager(cure::RuntimeVariableScope* variable_scope, int priority);

	const bool IsEscape() const;
	const bool GetShowScore() const;
	const Steering& GetSteeringControl() const;
	const CamControl& GetCamControl() const;
	const FireControl& GetFireControl() const;

	virtual void DoRefreshConfiguration();
	virtual bool UpdateInput(uilepra::InputElement* element);

private:
	virtual bool SetDefault(int priority);

	float mouse_sensitivity_;
	float escape_;
	float show_score_;
	Steering steering_control_;
	CamControl cam_control_;
	FireControl fire_control_;

	void operator=(const ClientOptionsManager&);

	logclass();
};



}
}
