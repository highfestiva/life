
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/LepraTarget.h"
#include <direct.h>
#include <LM.h>
#pragma warning(push)
#pragma warning(disable: 4201)	// Non-standard extension used: unnamed struct.
#include <MMSystem.h>
#pragma warning(pop)
#include <ShellAPI.h>
#include <ShlObj.h>
#include "../../Include/DiskFile.h"
#include "../../Include/JsonString.h"
#include "../../Include/Lepra.h"
#include "../../Include/Log.h"
#include "../../Include/Path.h"
#include "../../Include/String.h"
#include "../../Include/SystemManager.h"
#include "../../Include/Thread.h"



#pragma comment(lib, "Winmm.lib")	// For timeBeginPeriod() and timeEndPeriod().



namespace Lepra
{



LogDecorator gLog(LogType::GetLog(LogType::SUB_GENERAL), typeid(SystemManager));



//
// Helper functions.
//

static bool IsHyperThreadingSupported()
{
#ifdef LEPRA_MSVC_X86_32
	int lSupported = 0;
	__asm
	{
		mov	eax, 1
		cpuid
		shr	edx,28
		and	edx,1
		mov	[lSupported],edx
	}
	return (lSupported != 0);
#else // <Unimplemented target>
#error "Only LEPRA_MSVC_X86_32 supports hyper thread checking as of yet."
#endif // LEPRA_MSVC_X86/<Unimplemented target>
}

BOOL CtrlCallback(DWORD fdwCtrlType)
{
	const bool lNewThread = Thread::QueryInitializeThread();
	BOOL lHandled = FALSE;
	switch (fdwCtrlType)
	{
		case CTRL_CLOSE_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		{
			gLog.AInfo("Setting quit state on console break event.");
			lHandled = TRUE;
			SystemManager::AddQuitRequest(+1);
		}
		break;
		case CTRL_LOGOFF_EVENT:
		default:
		{
			gLog.AInfo("Ignoring console break event (i.e. logoff or similar).");
			lHandled = TRUE;
		}
		break;
	}
	if (lNewThread)
	{
		delete Thread::GetCurrentThread();
	}
	return (lHandled);
}



void SystemManager::Init()
{
	::SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCallback, TRUE);

	Thread::InitializeMainThread();

	// Increase resolution on Win32's ::Sleep() to 1 ms.
	::timeBeginPeriod(1);
}

void SystemManager::Shutdown()
{
	::timeEndPeriod(1);
}

str SystemManager::GetRootDirectory()
{
	return (strutil::Format(_T("%c:/"), (tchar)(::_getdrive() + 'A' - 1)));
}

str SystemManager::GetCurrentDirectory()
{
	char lBuffer[2048];
	lBuffer[0] = 0;
	if (::_getcwd(lBuffer, sizeof(lBuffer)) == NULL)
	{
		mLog.AError("Failed in GetCurrentDirectory()");
	}

	str lString(strutil::Encode(astr(lBuffer)));
	lString = strutil::ReplaceAll(lString, _T('\\'), _T('/'));

	return (lString);
}

str SystemManager::GetUserDirectory()
{
	tchar lHomeDir[2048];
	if (FAILED(::SHGetFolderPath(0, CSIDL_PROFILE, NULL, 0, lHomeDir)))
	{
		mLog.AWarning("Failed in GetUserDirectory()");
	}
	str lString(lHomeDir);
	lString = strutil::ReplaceAll(lString, _T('\\'), _T('/'));
	return (lString);
}

str SystemManager::GetIoDirectory(const str& pAppName)
{
	tchar lAppDir[2048];
	if (FAILED(::SHGetFolderPath(0, CSIDL_APPDATA, NULL, 0, lAppDir)))
	{
		mLog.AWarning("Failed in GetIoDirectory()");
	}
	str lIoDir(lAppDir);
	lIoDir = strutil::ReplaceAll(lIoDir, _T('\\'), _T('/'));
	lIoDir = Path::JoinPath(lIoDir, pAppName, _T(""));
	if (!DiskFile::PathExists(lIoDir))
	{
		DiskFile::CreateDir(lIoDir);
	}
	return (lIoDir);
}

str SystemManager::GetDataDirectory(const str& pArgv0)
{
	pArgv0;
	return _T("Data/");
}



str SystemManager::GetLoginName()
{
	wchar_t lLoginName[128];
	DWORD lLength = sizeof(lLoginName);
	::GetUserNameW(lLoginName, &lLength);
	return (strutil::Encode(wstr(lLoginName)));
}

str SystemManager::QueryFullUserName()
{
	str lFullName(GetLoginName());
	LPBYTE lDomainControllerName = 0;
	bool lOk = (::NetGetDCName(0, 0, &lDomainControllerName) == NERR_Success);
	struct _USER_INFO_2* lUserInfo;
	//if (lOk)
	{
		wstr lUnicodeLoginName = wstrutil::Encode(lFullName).c_str();
		lOk = (::NetUserGetInfo((LPWSTR)lDomainControllerName, lUnicodeLoginName.c_str(), 2, (LPBYTE*)&lUserInfo) == NERR_Success);
		if (lOk)
		{
			if (lUserInfo->usri2_full_name[0])
			{
				lFullName = strutil::Encode(lUserInfo->usri2_full_name);
			}
			::NetApiBufferFree(lUserInfo);
		}
	}
	if (lDomainControllerName)
	{
		::NetApiBufferFree(lDomainControllerName);
	}
	return (lFullName);
}

void SystemManager::WebBrowseTo(const str& pUrl)
{
	::ShellExecute(0, _T("open"), pUrl.c_str(), 0, 0, SW_SHOWDEFAULT);
}

void SystemManager::EmailTo(const str& pTo, const str& pSubject, const str& pBody)
{
	const str lUrlSubject = JsonString::UrlEncode(pSubject);
	const str lUrlBody = JsonString::UrlEncode(pBody);
	str lUrl = _T("mailto:") + pTo + _T("?subject=") + lUrlSubject + _T("&body=") + lUrlBody;
	::ShellExecute(0, _T("open"), lUrl.c_str(), 0, 0, SW_SHOWDEFAULT);
}

str SystemManager::GetHwName()
{
	return _T("PC");
}

unsigned SystemManager::GetLogicalCpuCount()
{
	unsigned lLogicalCpuPerPhysicalCpu = 1;
	if (IsHyperThreadingSupported())
	{
		__asm
		{
			xor	ebx,ebx
			mov	eax, 1
			cpuid
			shr	ebx,16
			and	ebx,0xFF
			mov	[lLogicalCpuPerPhysicalCpu],ebx
		}
	}
	return (lLogicalCpuPerPhysicalCpu);
}

unsigned SystemManager::GetPhysicalCpuCount()
{
	SYSTEM_INFO lInfo;
	::GetSystemInfo(&lInfo);
	return (lInfo.dwNumberOfProcessors);
}

unsigned SystemManager::GetCoreCount()
{
	unsigned lCpuCoreCountPerPhysicalProcessor = 1;
	if (IsHyperThreadingSupported())
	{
		__asm
		{
			xor	ecx,ecx
			mov	eax,0x80000008
			cpuid
			and	ecx,0xFF
			inc	ecx
			mov	[lCpuCoreCountPerPhysicalProcessor],ecx
		}
	}
	return (lCpuCoreCountPerPhysicalProcessor);
}

str SystemManager::GetCpuName()
{
	char lCpuName[13];
	__asm
	{
		pusha
		lea	esi,lCpuName	// esi = lCpuName
		xor	eax,eax		// eax = 0
		cpuid			// Stores the cpu name into ebx, edx and ecx.
		mov	[esi],ebx	// Copy bytes to lCpuName.
		mov	[esi+4],edx
		mov	[esi+8],ecx
		popa
	}
	lCpuName[12] = 0;
	return (strutil::Encode(astr(lCpuName)));
}

str SystemManager::GetOsName()
{
	OSVERSIONINFO lOsVer;
	lOsVer.dwOSVersionInfoSize = sizeof(lOsVer);

	if (::GetVersionEx(&lOsVer))
	{
		switch(lOsVer.dwPlatformId)
		{
			case VER_PLATFORM_WIN32_NT:
				return _T("Windows NT");
			case VER_PLATFORM_WIN32_WINDOWS:
				return _T("Windows 9x/Me");
		}
	}
	return _T("Windows");
}

uint64 SystemManager::GetAmountRam()
{
	MEMORYSTATUS lMemStatus;
	lMemStatus.dwLength = sizeof(lMemStatus);

	::GlobalMemoryStatus(&lMemStatus);
	return (uint64)lMemStatus.dwTotalPhys;
}

uint64 SystemManager::GetAvailRam()
{
	MEMORYSTATUS lMemStatus;
	lMemStatus.dwLength = sizeof(lMemStatus);

	::GlobalMemoryStatus(&lMemStatus);
	return (uint64)lMemStatus.dwAvailPhys;
}

uint64 SystemManager::GetAmountVirtualMemory()
{
	MEMORYSTATUS lMemStatus;
	lMemStatus.dwLength = sizeof(lMemStatus);

	::GlobalMemoryStatus(&lMemStatus);
	return (uint64)lMemStatus.dwTotalVirtual;
}

uint64 SystemManager::GetAvailVirtualMemory()
{
	MEMORYSTATUS lMemStatus;
	lMemStatus.dwLength = sizeof(lMemStatus);

	::GlobalMemoryStatus(&lMemStatus);
	return (uint64)lMemStatus.dwAvailVirtual;
}

void SystemManager::ExitProcess(int pExitCode)
{
	::ExitProcess(pExitCode);
}



}
