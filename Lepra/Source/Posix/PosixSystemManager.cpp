
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include <direct.h>
#include <shlobj.h>
#include <Lm.h>
#include <Windows.h>	// Must reside here to avoid lean and mean (for timeXxxPeriod()).
#include "../../Include/Lepra.h"
#include "../../Include/String.h"
#include "../../Include/SystemManager.h"
#include "../../Include/Log.h"
#include "../../Include/HiResTimer.h"
#include "../../Include/Thread.h"



namespace Lepra
{



void SystemManager::Init()
{
	Thread::InitializeMainThread(_T("MainThread"));
}

void SystemManager::Shutdown()
{
}

String SystemManager::GetRootDirectory()
{
	return (_T("/"));
}

String SystemManager::GetCurrentDirectory()
{
	char lBuffer[2048];
	lBuffer[0] = 0;
	if (::getcwd(lBuffer, sizeof(lBuffer)) == NULL)
	{
		mLog.AError("Failed in GetCurrentDirectory()");
	}
	String lString(AnsiStringUtility::ToCurrentCode(AnsiString(lBuffer)));
	return (lString);
}

String SystemManager::GetUserDirectory()
{
	return (AnsiStringUtility::ToCurrentCode(AnsiString(::getenv("HOME"))));
}

String SystemManager::GetLoginName()
{
	return (AnsiStringUtility::ToCurrentCode(AnsiString(::getlogin())));
}

String SystemManager::QueryFullUserName()
{
	// TODO: use ::getlogin() to search /usr/passwd or something similarly gory.
	return (String(_T("?")));
}

unsigned SystemManager::GetLogicalCpuCount()
{
	return (1);	// TODO: ... something...
}

unsigned SystemManager::GetPhysicalCpuCount()
{
	// TODO: search /proc/cpuinfo by brute force.
	return (1);
}

unsigned SystemManager::GetCoreCount()
{
	return (1);	// TODO: ... something...
}

String SystemManager::GetCpuName()
{
	// TODO: search /proc/cpuinfo by brute force.
	return (1);
}

String SystemManager::GetOsName()
{
	// TODO: probably something like popen("uname -o").read()...
	return (_T("POSIX"));
}

uint64 SystemManager::GetAmountRam()
{
	// TODO: sigh... popen("free").read().parse()?
	return (500*1024*1024);
}

uint64 SystemManager::GetAvailRam()
{
	// TODO: sigh... popen("free").read().parse()?
	return (300*1024*1024);
}

uint64 SystemManager::GetAmountVirtualMemory()
{
	// TODO: sigh... popen("free").read().parse()?
	return (700*1024*1024);
}

uint64 SystemManager::GetAvailVirtualMemory()
{
	// TODO: sigh... popen("free").read().parse()?
	return (500*1024*1024);
}

void SystemManager::ExitProcess(int pExitCode)
{
	::exit(pExitCode);
}



}
