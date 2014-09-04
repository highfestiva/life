
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../Cure/Include/ConsoleManager.h"
#include "../HoverTank.h"



namespace HoverTank
{



class HoverTankServerDelegate;



class HoverTankServerConsole: public Cure::ConsoleCommandExecutor
{
public:
	HoverTankServerConsole(HoverTankServerDelegate* pDelegate, Cure::ConsoleCommandManager* pCommandManager);
	virtual ~HoverTankServerConsole();

private:
	virtual int Execute(const str& pCommand, const strutil::strvec& pParameterList);
	virtual void OnExecutionError(const str& pCommand, const strutil::strvec& pParameterList, int pResult);

	HoverTankServerDelegate* mDelegate;

	logclass();
};



}
