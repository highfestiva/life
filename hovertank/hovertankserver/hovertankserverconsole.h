
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../cure/include/consolemanager.h"
#include "../hovertank.h"



namespace HoverTank {



class HoverTankServerDelegate;



class HoverTankServerConsole: public cure::ConsoleCommandExecutor {
public:
	HoverTankServerConsole(HoverTankServerDelegate* delegate, cure::ConsoleCommandManager* command_manager);
	virtual ~HoverTankServerConsole();

private:
	virtual int Execute(const str& command, const strutil::strvec& parameter_list);
	virtual void OnExecutionError(const str& command, const strutil::strvec& parameter_list, int result);

	HoverTankServerDelegate* delegate_;

	logclass();
};



}
