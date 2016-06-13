
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/lepraos.h"
#include <direct.h>
#include <LM.h>
#pragma warning(push)
#pragma warning(disable: 4201)	// Non-standard extension used: unnamed struct.
#include <MMSystem.h>
#pragma warning(pop)
#include <ShellAPI.h>
#include <ShlObj.h>
#include "../../include/diskfile.h"
#include "../../include/jsonstring.h"
#include "../../include/lepra.h"
#include "../../include/log.h"
#include "../../include/path.h"
#include "../../include/string.h"
#include "../../include/systemmanager.h"
#include "../../include/thread.h"



#pragma comment(lib, "Winmm.lib")	// For timeBeginPeriod() and timeEndPeriod().



namespace lepra {



LogDecorator gLog(LogType::GetLogger(LogType::kGeneral), typeid(SystemManager));



//
// Helper functions.
//

static bool IsHyperThreadingSupported() {
#ifdef LEPRA_MSVC_X86_32
	int supported = 0;
	__asm {
		mov	eax, 1
		cpuid
		shr	edx,28
		and	edx,1
		mov	[supported],edx
	}
	return (supported != 0);
#else // <Unimplemented target>
#error "Only LEPRA_MSVC_X86_32 supports hyper thread checking as of yet."
#endif // LEPRA_MSVC_X86/<Unimplemented target>
}

BOOL CtrlCallback(DWORD fdwCtrlType) {
	const bool new_thread = Thread::QueryInitializeThread();
	BOOL handled = FALSE;
	switch (fdwCtrlType) {
		case CTRL_CLOSE_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT: {
			gLog.Info("Setting quit state on console break event.");
			handled = TRUE;
			SystemManager::AddQuitRequest(+1);
		} break;
		case CTRL_LOGOFF_EVENT:
		default: {
			gLog.Info("Ignoring console break event (i.e. logoff or similar).");
			handled = TRUE;
		} break;
	}
	if (new_thread) {
		delete Thread::GetCurrentThread();
	}
	return (handled);
}



void SystemManager::Init() {
	::SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCallback, TRUE);

	Thread::InitializeMainThread();

	// Increase resolution on Win32's ::Sleep() to 1 ms.
	::timeBeginPeriod(1);
}

void SystemManager::Shutdown() {
	::timeEndPeriod(1);
}

void SystemManager::ResetTerminal() {
}

str SystemManager::GetRootDirectory() {
	return (strutil::Format("%c:/", (char)(::_getdrive() + 'A' - 1)));
}

str SystemManager::GetCurrentDirectory() {
	char buffer[2048];
	buffer[0] = 0;
	if (::_getcwd(buffer, sizeof(buffer)) == NULL) {
		log_.Error("Failed to GetCurrentDirectory()");
	}

	return strutil::ReplaceAll(buffer, '\\', '/');
}

str SystemManager::GetUserDirectory() {
	char home_dir[2048];
	if (FAILED(::SHGetFolderPath(0, CSIDL_PROFILE, NULL, 0, home_dir))) {
		log_.Warning("Failed to GetUserDirectory()");
	}
	str s(home_dir);
	s = strutil::ReplaceAll(s, '\\', '/');
	return s;
}

str SystemManager::GetDocumentsDirectory() {
	char docs_dir[2048];
	if (FAILED(::SHGetFolderPath(0, CSIDL_MYDOCUMENTS, NULL, 0, docs_dir))) {
		log_.Warning("Failed to GetDocumentsDirectory()");
	}
	str s(docs_dir);
	s = strutil::ReplaceAll(s, '\\', '/');
	return s;
}

str SystemManager::GetIoDirectory(const str& app_name) {
	char app_dir[2048];
	if (FAILED(::SHGetFolderPath(0, CSIDL_APPDATA, NULL, 0, app_dir))) {
		log_.Warning("Failed to GetIoDirectory()");
	}
	str io_dir(app_dir);
	io_dir = strutil::ReplaceAll(io_dir, '\\', '/');
	io_dir = Path::JoinPath(io_dir, app_name, "");
	if (!DiskFile::PathExists(io_dir)) {
		DiskFile::CreateDir(io_dir);
	}
	return (io_dir);
}

str SystemManager::GetDataDirectoryFromPath(const str& argv0) {
	argv0;
	return "data/";
}



str SystemManager::GetLoginName() {
	wchar_t login_name[128];
	DWORD length = sizeof(login_name);
	::GetUserNameW(login_name, &length);
	return strutil::Encode(login_name);
}

str SystemManager::QueryFullUserName() {
	str full_name(GetLoginName());
	LPBYTE domain_controller_name = 0;
	bool ok = (::NetGetDCName(0, 0, &domain_controller_name) == NERR_Success);
	struct _USER_INFO_2* user_info;
	//if (ok)
	{
		wstr unicode_login_name = wstrutil::Encode(full_name).c_str();
		ok = (::NetUserGetInfo((LPWSTR)domain_controller_name, unicode_login_name.c_str(), 2, (LPBYTE*)&user_info) == NERR_Success);
		if (ok) {
			if (user_info->usri2_full_name[0]) {
				full_name = strutil::Encode(user_info->usri2_full_name);
			}
			::NetApiBufferFree(user_info);
		}
	}
	if (domain_controller_name) {
		::NetApiBufferFree(domain_controller_name);
	}
	return (full_name);
}

void SystemManager::WebBrowseTo(const str& url) {
	::ShellExecute(0, "open", url.c_str(), 0, 0, SW_SHOWDEFAULT);
}

void SystemManager::EmailTo(const str& to, const str& subject, const str& body) {
	const str url_subject = JsonString::UrlEncode(subject);
	const str url_body = JsonString::UrlEncode(body);
	str _url = "mailto:" + to + "?subject=" + url_subject + "&body=" + url_body;
	::ShellExecute(0, "open", _url.c_str(), 0, 0, SW_SHOWDEFAULT);
}

str SystemManager::GetHwName() {
	return "PC";
}

unsigned SystemManager::GetLogicalCpuCount() {
	unsigned logical_cpu_per_physical_cpu = 1;
	if (IsHyperThreadingSupported()) {
		__asm {
			xor	ebx,ebx
			mov	eax, 1
			cpuid
			shr	ebx,16
			and	ebx,0xFF
			mov	[logical_cpu_per_physical_cpu],ebx
		}
	}
	return (logical_cpu_per_physical_cpu);
}

unsigned SystemManager::GetPhysicalCpuCount() {
	SYSTEM_INFO info;
	::GetSystemInfo(&info);
	return (info.dwNumberOfProcessors);
}

unsigned SystemManager::GetCoreCount() {
	unsigned cpu_core_count_per_physical_processor = 1;
	if (IsHyperThreadingSupported()) {
		__asm {
			xor	ecx,ecx
			mov	eax,0x80000008
			cpuid
			and	ecx,0xFF
			inc	ecx
			mov	[cpu_core_count_per_physical_processor],ecx
		}
	}
	return (cpu_core_count_per_physical_processor);
}

str SystemManager::GetCpuName() {
	char cpu_name[13];
	__asm {
		pusha
		lea	esi,cpu_name	// esi = cpu_name
		xor	eax,eax		// eax = 0
		cpuid			// Stores the cpu name into ebx, edx and ecx.
		mov	[esi],ebx	// Copy bytes to cpu_name.
		mov	[esi+4],edx
		mov	[esi+8],ecx
		popa
	}
	cpu_name[12] = 0;
	return (str(cpu_name));
}

str SystemManager::GetOsName() {
	OSVERSIONINFO os_ver;
	os_ver.dwOSVersionInfoSize = sizeof(os_ver);

#pragma warning(disable: 4996)	// XXX was declared deprecated.
	if (::GetVersionEx(&os_ver)) {
		switch(os_ver.dwPlatformId) {
			case VER_PLATFORM_WIN32_NT:
				return "Windows NT";
			case VER_PLATFORM_WIN32_WINDOWS:
				return "Windows 9x/Me";
		}
	}
	return "Windows";
}

uint64 SystemManager::GetAmountRam() {
	MEMORYSTATUS mem_status;
	mem_status.dwLength = sizeof(mem_status);

	::GlobalMemoryStatus(&mem_status);
	return (uint64)mem_status.dwTotalPhys;
}

uint64 SystemManager::GetAvailRam() {
	MEMORYSTATUS mem_status;
	mem_status.dwLength = sizeof(mem_status);

	::GlobalMemoryStatus(&mem_status);
	return (uint64)mem_status.dwAvailPhys;
}

uint64 SystemManager::GetAmountVirtualMemory() {
	MEMORYSTATUS mem_status;
	mem_status.dwLength = sizeof(mem_status);

	::GlobalMemoryStatus(&mem_status);
	return (uint64)mem_status.dwTotalVirtual;
}

uint64 SystemManager::GetAvailVirtualMemory() {
	MEMORYSTATUS mem_status;
	mem_status.dwLength = sizeof(mem_status);

	::GlobalMemoryStatus(&mem_status);
	return (uint64)mem_status.dwAvailVirtual;
}

void SystemManager::ExitProcess(int exit_code) {
	::ExitProcess(exit_code);
}



}
