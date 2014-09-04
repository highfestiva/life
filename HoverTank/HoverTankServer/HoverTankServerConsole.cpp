
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "HoverTankServerConsole.h"
#include "HoverTankServerDelegate.h"



namespace HoverTank
{



HoverTankServerConsole::HoverTankServerConsole(HoverTankServerDelegate* pDelegate, Cure::ConsoleCommandManager* pCommandManager):
	mDelegate(pDelegate)
{
	pCommandManager->AddExecutor(this);
	pCommandManager->AddCommand(_T("set-level"));
}

HoverTankServerConsole::~HoverTankServerConsole()
{
}



int HoverTankServerConsole::Execute(const str& pCommand, const strutil::strvec& pParameterVector)
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

void HoverTankServerConsole::OnExecutionError(const str& pCommand, const strutil::strvec& pParameterVector, int pResult)
{
	(void)pCommand;
	(void)pParameterVector;
	(void)pResult;
	deb_assert(false);
}



loginstance(CONSOLE, HoverTankServerConsole);



}
