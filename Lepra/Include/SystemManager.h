
// Author: Jonas Bystr�m
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
	static void Init();
	static void Shutdown();

	// Get root directory ( '/', 'C:\', 'Macintosh HD', etc..).
	static str GetRootDirectory();
	static str GetCurrentDirectory();
	static str GetUserDirectory();
	static str GetIoDirectory(const str& pAppName);
	static str GetLoginName();
	static str QueryFullUserName();
	static std::string GetRandomId();

	static void WebBrowseTo(const str& pUrl);

	static str GetUniqueHardwareString();

	// The Cpu frequency may be an approximation, which means that it may vary 
	// from call to call.
	static uint64 QueryCpuFrequency();
	static inline uint64 GetCpuTick();
	static unsigned GetLogicalCpuCount();
	static unsigned GetPhysicalCpuCount();
	static unsigned GetCoreCount();
	static str GetCpuName();
	static unsigned QueryCpuMips();
	static str GetOsName();
	static uint64 GetAmountRam();
	static uint64 GetAvailRam();
	static uint64 GetAmountVirtualMemory();
	static uint64 GetAvailVirtualMemory();

	static int GetQuitRequest();
	static void AddQuitRequest(int pValue);
	static void ExitProcess(int pExitCode);	// Exit this application NOW.

private:
	static uint64 SingleCpuTest();
	static inline unsigned SingleMipsTest();
	static inline void BOGOMIPSDelay(unsigned pLoopCount);

	static int mQuitRequest;

	LOG_CLASS_DECLARE();
};



inline uint64 SystemManager::GetCpuTick()
{
#ifdef LEPRA_MSVC_X86
	__asm rdtsc;
#elif defined(LEPRA_GCC_X86_32)
	uint64 x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return x;
#elif defined(LEPRA_GCC_X86_64)
	unsigned hi, lo;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64)lo) | (((uint64)hi)<<32);
#else
#error "GetCpuTick() not yet implemented on this platform."
#endif // Platform.
}



}
