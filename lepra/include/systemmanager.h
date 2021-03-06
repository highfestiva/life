
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../thirdparty/FastDelegate/FastDelegate.h"
#include "lepraos.h"
#include "lepratypes.h"
#include "log.h"
#include "string.h"



namespace lepra {



class SystemManager {
public:
	typedef fastdelegate::FastDelegate1<int, void> QuitRequestCallback;

	static void Init();
	static void Shutdown();
	static void ResetTerminal();

	static void SetArgumentVector(const strutil::strvec& argument_vector);
	static const strutil::strvec& GetArgumentVector();
	static str GetDataDirectory();
	// Get root directory ( '/', 'C:\', 'Macintosh HD', etc..).
	static str GetRootDirectory();
	static str GetCurrentDirectory();
	static str GetUserDirectory();
	static str GetDocumentsDirectory();
	static str GetIoDirectory(const str& app_name);
	static str GetDataDirectoryFromPath(const str& argv0);
	static str GetLoginName();
	static str QueryFullUserName();
	static str GetRandomId();
	static str GetSystemPseudoId();

	static void WebBrowseTo(const str& url);
	static void EmailTo(const str& to, const str& subject, const str& body);

	static str GetUniqueHardwareString();

	// The Cpu frequency may be an approximation, which means that it may vary
	// from call to call.
	static uint64 QueryCpuFrequency();
	static inline uint64 GetCpuTick();
	static str GetHwName();
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
	static double GetSleepResolution();

	static int GetQuitRequest();
	static void AddQuitRequest(int value);
	static void SetQuitRequestCallback(const QuitRequestCallback& callback);
	static void ExitProcess(int exit_code);	// Exit this application NOW.

private:
	static uint64 SingleCpuTest();
	static inline unsigned SingleMipsTest();
	static inline void BOGOMIPSDelay(unsigned loop_count);

	static strutil::strvec argument_vector_;

	static int quit_request_;
	static QuitRequestCallback quit_request_callback_;

	static double sleep_resolution_;

	logclass();
};



inline uint64 SystemManager::GetCpuTick() {
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
#elif defined(LEPRA_GCC_POWERPC)
	unsigned long upper, lower, tmp;
	__asm__ volatile(
		"0:                     \n"
		"\tmftbu   %0           \n"
		"\tmftb    %1           \n"
		"\tmftbu   %2           \n"
		"\tcmpw    %2, %0       \n"
		"\tbne     0b           \n"
		: "=r"(upper),"=r"(lower),"=r"(tmp)
	);
	return (((uint64)upper)<<32 | lower)*75;
#elif defined(LEPRA_GCC_ARM)
        timeval time_value;
        ::gettimeofday(&time_value, 0);
	return ((uint64)time_value.tv_sec*1000000 + time_value.tv_usec);
#else
#error "GetCpuTick() not yet implemented on this platform."
#endif // Platform.
}



}
