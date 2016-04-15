
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <sys/sysctl.h>
#include <stdlib.h>
#include <termios.h>
#include "../../Include/Posix/MacLog.h"
#include "../../Include/DiskFile.h"
#include "../../Include/JsonString.h"
#include "../../Include/LepraOS.h"
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
#ifdef LEPRA_MAC
	::pclose(lFile);
#else	// Posix.
	::fclose(lFile);
#endif	// Mac / Posix.
	str lStdout(lRaw);
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

	Thread::InitializeMainThread();
}

void SystemManager::Shutdown()
{
	ResetTerminal();
}

void SystemManager::ResetTerminal()
{
	::tcsetattr(STDIN_FILENO, TCSANOW, &gInitialTermios);
}

str SystemManager::GetRootDirectory()
{
	return ("/");
}

str SystemManager::GetCurrentDirectory()
{
	char lBuffer[2048];
	lBuffer[0] = 0;
	if (::getcwd(lBuffer, sizeof(lBuffer)) == NULL)
	{
		mLog.Error("Failed to GetCurrentDirectory()");
	}
	return str(lBuffer);
}

str SystemManager::GetUserDirectory()
{
	return str(::getenv("HOME"));
}

str SystemManager::GetDocumentsDirectory()
{
#ifdef LEPRA_IOS
	NSArray* lPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* lPath = [lPaths objectAtIndex:0];
	return MacLog::Decode(lPath);
#else // Mac
	return Path::JoinPath(GetUserDirectory(), "Documents");
#endif
}

str SystemManager::GetIoDirectory(const str& pAppName)
{
	const str lIoDir = Path::JoinPath(GetUserDirectory(), "."+pAppName, "");
	if (!DiskFile::PathExists(lIoDir))
	{
		DiskFile::CreateDir(lIoDir);
	}
	return (lIoDir);
}

str SystemManager::GetDataDirectoryFromPath(const str& pArgv0)
{
#if defined(LEPRA_IOS)
	return Path::GetDirectory(pArgv0);
#elif defined(LEPRA_MAC)
	str lDataDir = Path::GetDirectory(pArgv0);
	lDataDir = Path::GetParentDirectory(lDataDir);
	return Path::JoinPath(lDataDir, "Resources/Data/");
#else // Posix
	(void)pArgv0;
	return "Data/";
#endif // iOS/Mac/Posix
}



str SystemManager::GetLoginName()
{
	const char* lLoginName = ::getlogin();
	if (!lLoginName)
	{
		lLoginName = "<Unknown>";
	}
	return (str(lLoginName));
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
		str lUrl = pUrl;
		bool lFound = false;
#ifdef LEPRA_MAC
		lFound = lFound || (::system(("open "+lUrl).c_str()) == 0);
		lFound = lFound || (::system(("/Applications/Firefox.app/Contents/MacOS/firefox "+lUrl).c_str()) == 0);
#else // Other Posix
		lFound = lFound || (::system(("sensible-browser "+lUrl).c_str()) == 0);
		lFound = lFound || (::system(("firefox "+lUrl).c_str()) == 0);
#endif // OS X / Other Posix
		::_exit(0);
		::exit(0);
	}
}

void SystemManager::EmailTo(const str& pTo, const str& pSubject, const str& pBody)
{
	if (::fork() == 0)
	{
		const str lUrlSubject = JsonString::UrlEncode(pSubject);
		const str lUrlBody = JsonString::UrlEncode(pBody);
		str lWUrl = "mailto:" + pTo + "?subject=" + lUrlSubject + "&body=" + lUrlBody;
		str lUrl = lWUrl;
		bool lFound = false;
#ifdef LEPRA_MAC
		lFound = lFound || (::system(("open "+lUrl).c_str()) == 0);
		lFound = lFound || (::system(("/Applications/Mail.app/Contents/MacOS/mail "+lUrl).c_str()) == 0);
#else // Other Posix
		lFound = lFound || (::system(("sensible-browser "+lUrl).c_str()) == 0);
		lFound = lFound || (::system(("firefox "+lUrl).c_str()) == 0);
#endif // OS X / Other Posix
		::_exit(0);
		::exit(0);
	}
}

str SystemManager::GetHwName()
{
#ifdef LEPRA_MAC
	char lMachine[4096];
	size_t lSize = sizeof(lMachine);
	::memset(lMachine, 0, lSize);
	::sysctlbyname("hw.machine", lMachine, &lSize, 0, 0);
	return str(lMachine);
#else // Other Posix
	return "PC";
#endif // OS X / Other Posix
}

unsigned SystemManager::GetLogicalCpuCount()
{
	return GetPhysicalCpuCount();
}

unsigned SystemManager::GetPhysicalCpuCount()
{
	return GetCoreCount();
}

unsigned SystemManager::GetCoreCount()
{
#ifdef LEPRA_MAC
	int mib[4];
	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;
	unsigned int ncpu = 1;
	size_t len = sizeof(ncpu); 
	::sysctl(mib, 2, &ncpu, &len, NULL, 0);
	if (ncpu < 1)
	{
		ncpu = 1;
	}
	return ncpu;
#else // Linux
	return ::sysconf(_SC_NPROCESSORS_ONLN);
#endif // Apple / Linux.
}

str SystemManager::GetCpuName()
{
#if defined(LEPRA_GCC_X86_32)
	return ("x86");
#elif defined(LEPRA_GCC_X86_64)
	return ("x64");
#elif defined(LEPRA_GCC_POWERPC)
	return ("PowerPC");
#elif defined(LEPRA_GCC_ARM_32)
	return ("ARM");
#elif defined(LEPRA_GCC_ARM_64)
	return ("ARM64");
#else // Unkonwn CPU type.
	return ("Unknown");
#endif // CPU check.
}

str SystemManager::GetOsName()
{
	str lOsName = ExecGetStdout("uname");
	if (lOsName.empty())
	{
		return ("Posix");
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
