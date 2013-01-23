
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Cure/Include/ConsoleManager.h"
#include "../Push.h"



namespace Push
{



class PushServerDelegate;



class PushServerConsole: public Cure::ConsoleCommandExecutor
{
public:
	PushServerConsole(PushServerDelegate* pDelegate, Cure::ConsoleCommandManager* pCommandManager);
	virtual ~PushServerConsole();

private:
	virtual int Execute(const str& pCommand, const strutil::strvec& pParameterList);
	virtual void OnExecutionError(const str& pCommand, const strutil::strvec& pParameterList, int pResult);

	PushServerDelegate* mDelegate;

	LOG_CLASS_DECLARE();
};



}
