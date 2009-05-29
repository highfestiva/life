
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include <termios.h>
#include "../../Include/Lepra.h"
#include "../../Include/String.h"
#include "../../Include/SystemManager.h"
#include "../../Include/Log.h"
#include "../../Include/HiResTimer.h"
#include "../../Include/Thread.h"



namespace Lepra
{



static termios gInitialTermios;



void SystemManager::Init()
{
	::tcgetattr(STDIN_FILENO, &gInitialTermios);
	termios lTermios = gInitialTermios;
	lTermios.c_lflag &= ~(ICANON|ECHO);
	lTermios.c_cc[VMIN] = 1;
	lTermios.c_cc[VTIME] = 5;
	::tcsetattr(STDIN_FILENO, TCSANOW, &lTermios);

	Thread::InitializeMainThread(_T("MainThread"));
}

void SystemManager::Shutdown()
{
	::tcsetattr(STDIN_FILENO, TCSANOW, &gInitialTermios);
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
	return (GetLoginName());
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
	return (_T("GenuineIntel"));
}

String SystemManager::GetOsName()
{
	// TODO: probably something like popen("uname -o").read()...
	return (_T("Posix"));
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
