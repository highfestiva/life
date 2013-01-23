
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushServerConsole.h"
#include "PushServerDelegate.h"



namespace Push
{



PushServerConsole::PushServerConsole(PushServerDelegate* pDelegate, Cure::ConsoleCommandManager* pCommandManager):
	mDelegate(pDelegate)
{
	pCommandManager->AddExecutor(this);
	pCommandManager->AddCommand(_T("set-level"));
}

PushServerConsole::~PushServerConsole()
{
}



int PushServerConsole::Execute(const str& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = 0;
	if (pCommand == _T("set-level"))
	{
		if (pParameterVector.size() == 1)
		{
			mDelegate->SetLevel(pParameterVector[0]);
		}
		else
		{
			mLog.Warningf(_T("usage: %s <level_name>"), pCommand.c_str());
			lResult = 1;
		}
	}
	else
	{
		lResult = -1;
	}
	return lResult;
}

void PushServerConsole::OnExecutionError(const str& pCommand, const strutil::strvec& pParameterVector, int pResult)
{
	(void)pCommand;
	(void)pParameterVector;
	(void)pResult;
	assert(false);
}



LOG_CLASS_DEFINE(CONSOLE, PushServerConsole);



}
