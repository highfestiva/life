
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Anti-cracking techniques. Warning: may contain unreadable code.



#include "pch.h"
#include "../../include/anticrack.h"



AntiCrack::AntiCrack() {
	BOOL present = FALSE;
	if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &present) && present) {
		// TODO: enable when shipping something really commercially viable.
		//SetBsodOnProcessExit();
	}
}



void AntiCrack::SetBsodOnProcessExit() {
	EnableCriticalPrivileges(SE_DEBUG_NAME);

	HANDLE dll_handle = LoadLibraryA("ntdll.dll");
	#define RtlSetProcessIsCriticalProc	p
	#define RtlSetProcessIsCritical		pq
	typedef VOID (_stdcall *RtlSetProcessIsCriticalProc) (IN BOOLEAN NewValue, OUT PBOOLEAN OldValue, IN BOOLEAN IsWinlogon);
	RtlSetProcessIsCriticalProc RtlSetProcessIsCritical =
		(RtlSetProcessIsCriticalProc)::GetProcAddress((HINSTANCE)dll_handle, "RtlSetProcessIsCritical");
	if (RtlSetProcessIsCritical) {
// Only include BSOD in the final product. Note that debugging will have to happen in the release candidate,
// which essentially is a final build with debug information.
#ifdef LEPRA_FINAL
		RtlSetProcessIsCritical(TRUE, 0, 0);
#else // Debug+Release Candidate
		RtlSetProcessIsCritical(FALSE, 0, 0);
#endif // Final / Debug+Release Candidate.
	}
}

bool AntiCrack::EnableCriticalPrivileges(LPCTSTR privilege_name) { // by Napalm
	HANDLE token_handle;
	if(!::OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY), &token_handle)) {
		return (false);
	}

	LUID luid;
	if(!::LookupPrivilegeValue(NULL, privilege_name, &luid)) {
		::CloseHandle(token_handle);
		return (false);
	}

	TOKEN_PRIVILEGES token_privileges;
	::ZeroMemory(&token_privileges, sizeof(token_privileges));
	token_privileges.PrivilegeCount = 1;
	token_privileges.Privileges[0].Luid = luid;
	token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	BOOL adjusted_privileges = ::AdjustTokenPrivileges(token_handle, FALSE, &token_privileges, sizeof(token_privileges), NULL, NULL);
	::CloseHandle(token_handle);
	return (!!adjusted_privileges);
}
