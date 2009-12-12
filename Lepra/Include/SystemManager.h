
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTypes.h"
#include "Log.h"
#include "String.h"
#include "String.h"



namespace Lepra
{

class SystemManager
{
public:
	// Get root directory ( '/', 'C:\', 'Macintosh HD', etc..).
	static String GetRootDirectory();
	static String GetCurrentDirectory();
	static String GetUserDirectory();
	static String GetIoDirectory(const String& pAppName);
	static String GetLoginName();
	static String QueryFullUserName();
	static std::string GetRandomId();

	//
	// Hardware info.
	//
	static String GetUniqueHardwareString();

	// The Cpu frequency may be an approximation, which means that it may vary 
	// from call to call.
	static uint64 QueryCpuFrequency();
	static inline uint64 GetCpuTick();
	static unsigned GetLogicalCpuCount();
	static unsigned GetPhysicalCpuCount();
	static unsigned GetCoreCount();
	static String GetCpuName();
	static unsigned QueryCpuMips();
	static String GetOsName();
	static uint64 GetAmountRam();
	static uint64 GetAvailRam();
	static uint64 GetAmountVirtualMemory();
	static uint64 GetAvailVirtualMemory();

	static bool GetQuitRequest();
	static void SetQuitRequest(bool pQuitRequest);

	// Platform specific functions.

	static void Init();
	static void Shutdown();

	// Exit this application NOW.
	static void ExitProcess(int pExitCode);

private:
	static uint64 SingleCpuTest();
	static inline unsigned SingleMipsTest();
	static inline void BOGOMIPSDelay(unsigned pLoopCount);

#if defined(LEPRA_WINDOWS)
	static BOOL CtrlCallback(DWORD fdwCtrlType);
#endif // LEPRA_WINDOWS

	static bool smQuitRequest;

	LOG_CLASS_DECLARE();
};

inline uint64 SystemManager::GetCpuTick()
{
#ifdef LEPRA_MSVC_X86
	__asm rdtsc;
#elif defined (LEPRA_GCC) && defined(__i386__)
	uint64 x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return x;
#elif defined (LEPRA_GCC) && defined(__x86_64__)
	unsigned hi, lo;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64)lo) | (((uint64)hi)<<32);
#else // <Unimplemented target>
#error "GetCpuTick() not yet implemented on this platform."
#endif // LEPRA_MSVC_X86/<Unimplemented target>
}

}
