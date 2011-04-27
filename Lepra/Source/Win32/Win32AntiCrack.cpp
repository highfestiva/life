
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// Anti-cracking techniques. Warning: may contain unreadable code.



#include "../../Include/AntiCrack.h"



AntiCrack::AntiCrack()
{
	BOOL lPresent = FALSE;
	if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &lPresent) && lPresent)
	{
		// TODO: enable when shipping something really commercially viable.
		//SetBsodOnProcessExit();
	}
}



void AntiCrack::SetBsodOnProcessExit()
{
	EnableCriticalPrivileges(SE_DEBUG_NAME);

	HANDLE lDllHandle = LoadLibraryA("ntdll.dll");
	#define RtlSetProcessIsCriticalProc	p
	#define RtlSetProcessIsCritical		pq
	typedef VOID (_stdcall *RtlSetProcessIsCriticalProc) (IN BOOLEAN NewValue, OUT PBOOLEAN OldValue, IN BOOLEAN IsWinlogon); 
	RtlSetProcessIsCriticalProc RtlSetProcessIsCritical =
		(RtlSetProcessIsCriticalProc)::GetProcAddress((HINSTANCE)lDllHandle, "RtlSetProcessIsCritical");
	if (RtlSetProcessIsCritical)
	{
// Only include BSOD in the final product. Note that debugging will have to happen in the release candidate,
// which essentially is a final build with debug information.
#ifdef LEPRA_FINAL
		RtlSetProcessIsCritical(TRUE, 0, 0); 
#else // Debug+Release Candidate
		RtlSetProcessIsCritical(FALSE, 0, 0); 
#endif // Final / Debug+Release Candidate.
	}
}

bool AntiCrack::EnableCriticalPrivileges(LPCTSTR pPrivilegeName) // by Napalm
{
	HANDLE lTokenHandle;
	if(!::OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY), &lTokenHandle))
	{
		return (false);
	}

	LUID lLuid;
	if(!::LookupPrivilegeValue(NULL, pPrivilegeName, &lLuid))
	{
		::CloseHandle(lTokenHandle);
		return (false);
	}

	TOKEN_PRIVILEGES lTokenPrivileges;
	::ZeroMemory(&lTokenPrivileges, sizeof(lTokenPrivileges));
	lTokenPrivileges.PrivilegeCount = 1;
	lTokenPrivileges.Privileges[0].Luid = lLuid;
	lTokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	BOOL lAdjustedPrivileges = ::AdjustTokenPrivileges(lTokenHandle, FALSE, &lTokenPrivileges, sizeof(lTokenPrivileges), NULL, NULL);
	::CloseHandle(lTokenHandle);
	return (!!lAdjustedPrivileges);
}
