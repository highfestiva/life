
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <stdlib.h>
#include <termios.h>
#include "../../Include/DiskFile.h"
#include "../../Include/HiResTimer.h"
#include "../../Include/Lepra.h"
#include "../../Include/Log.h"
#include "../../Include/Path.h"
#include "../../Include/String.h"
#include "../../Include/SystemManager.h"
#include "../../Include/Thread.h"



namespace Lepra
{



static termios gInitialTermios;



static str ExecGetStdout(const char* pExecutable)
{
	FILE* lFile = ::popen(pExecutable, "r");
	if (!lFile)
	{
		return str();
	}
	char lRaw[1024];
	::memset(lRaw, 0, sizeof(lRaw));
	::fread(lRaw, sizeof(lRaw), 1, lFile);
	::fclose(lFile);
	str lStdout(astrutil::ToCurrentCode(lRaw));
	if (lStdout.length() > 1)
	{
		lStdout.resize(lStdout.length()-1);
	}
	return (lStdout);
}



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

str SystemManager::GetRootDirectory()
{
	return (_T("/"));
}

str SystemManager::GetCurrentDirectory()
{
	char lBuffer[2048];
	lBuffer[0] = 0;
	if (::getcwd(lBuffer, sizeof(lBuffer)) == NULL)
	{
		mLog.AError("Failed in GetCurrentDirectory()");
	}
	str lString(astrutil::ToCurrentCode(astr(lBuffer)));
	return (lString);
}

str SystemManager::GetUserDirectory()
{
	return (astrutil::ToCurrentCode(astr(::getenv("HOME"))));
}

str SystemManager::GetIoDirectory(const str& pAppName)
{
	const str lIoDir = Path::JoinPath(GetUserDirectory(), _T(".")+pAppName, _T(""));
	if (!DiskFile::PathExists(lIoDir))
	{
		DiskFile::CreateDir(lIoDir);
	}
	return (lIoDir);
}

str SystemManager::GetLoginName()
{
	const char* lLoginName = ::getlogin();
	if (!lLoginName)
	{
		lLoginName = "<Unknown>";
	}
	return (astrutil::ToCurrentCode(astr(lLoginName)));
}

str SystemManager::QueryFullUserName()
{
	// TODO: use ::getlogin() to search /usr/passwd or something similarly gory.
	return (GetLoginName());
}

void SystemManager::WebBrowseTo(const str& pUrl)
{
	if (::fork() == 0)
	{
		astr lUrl = astrutil::ToOwnCode(pUrl);
		bool lFound = false;
#ifdef LEPRA_MAC
		lFound = lFound || (::system(("open "+lUrl).c_str()) == 0);
		lFound = lFound || (::system(("/Applications/Firefox.app/Contents/MacOS/firefox "+lUrl).c_str()) == 0);
#else // Other Posix
		lFound = lFound || (::system(("sensible-browser "+lUrl).c_str()) == 0);
		lFound = lFound || (::system(("firefox "+lUrl).c_str()) == 0);
#endif // OS X / Other Posix
		::exit(0);
	}
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

str SystemManager::GetCpuName()
{
#if defined(LEPRA_GCC_X86_32)
	return (_T("x86"));
#elif defined(LEPRA_GCC_X86_64)
	return (_T("x64"));
#elif defined(LEPRA_GCC_POWERPC)
	return (_T("PowerPC"));
#else // Unkonwn CPU type.
	return (_T("Unknown"));
#endif // CPU check.
}

str SystemManager::GetOsName()
{
	str lOsName = ExecGetStdout("uname");
	if (lOsName.empty())
	{
		return (_T("Posix"));
	}
	return (lOsName);
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
