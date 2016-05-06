
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "hovertankserverconsole.h"
#include "hovertankserverdelegate.h"



namespace HoverTank {



HoverTankServerConsole::HoverTankServerConsole(HoverTankServerDelegate* delegate, cure::ConsoleCommandManager* command_manager):
	delegate_(delegate) {
	command_manager->AddExecutor(this);
	command_manager->AddCommand("set-level");
}

HoverTankServerConsole::~HoverTankServerConsole() {
}



int HoverTankServerConsole::Execute(const str& command, const strutil::strvec& parameter_vector) {
	int _result = 0;
	if (command == "set-level") {
		if (parameter_vector.size() == 1) {
			delegate_->SetLevel(parameter_vector[0]);
		} else {
			log_.Warningf("usage: %s <level_name>", command.c_str());
			_result = 1;
		}
	} else {
		_result = -1;
	}
	return _result;
}

void HoverTankServerConsole::OnExecutionError(const str& command, const strutil::strvec& parameter_vector, int result) {
	(void)command;
	(void)parameter_vector;
	(void)result;
	deb_assert(false);
}



loginstance(kConsole, HoverTankServerConsole);



}
