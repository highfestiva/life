
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/HiResTimer.h"
#include "../Include/Random.h"
#include "../Include/SHA1.h"
#include "../Include/SystemManager.h"
#include "../Include/Thread.h"



namespace Lepra
{



std::string SystemManager::GetRandomId()
{
	String s;
	s += GetLoginName();
	s += GetUniqueHardwareString();
	s += StringUtility::IntToString(Random::GetRandomNumber64(), 16);
	s += StringUtility::IntToString(GetCpuTick(), 16);
	s += StringUtility::IntToString(GetAvailRam(), 16);
	s += StringUtility::IntToString(GetAvailVirtualMemory(), 16);
	Lepra::uint8 lHashData[20];
	Lepra::SHA1::Hash((const Lepra::uint8*)s.c_str(), s.length()*sizeof(Lepra::tchar), lHashData);
	return (std::string((const char*)lHashData, sizeof(lHashData)));
}

String SystemManager::GetUniqueHardwareString()
{
	String s;
	s += StringUtility::IntToString(QueryCpuFrequency(), 16);
	s += StringUtility::IntToString(GetLogicalCpuCount(), 16);
	s += StringUtility::IntToString(GetPhysicalCpuCount(), 16);
	s += StringUtility::IntToString(GetCoreCount(), 16);
	s += GetOsName();
	s += StringUtility::IntToString(GetAmountRam(), 16);
	s += StringUtility::IntToString(GetAmountVirtualMemory(), 16);
	return (s);
}

uint64 SystemManager::SingleCpuTest()
{
	HiResTimer lTimer;
	int64 lDeltaTime = HiResTimer::GetFrequency() / 100;
	uint64 lStartTick;
	uint64 lEndTick;
	// Reset the timer and start counting.
	lTimer.UpdateTimer();
	lTimer.ClearTimeDiff();
	lStartTick = GetCpuTick();
	// Take 1000 samples.
	while (lTimer.GetCounterDiff() < lDeltaTime)
	{
		lTimer.UpdateTimer();
	}
	lEndTick = GetCpuTick();
	// Return the estimated frequency.
	return (lEndTick - lStartTick) * HiResTimer::GetFrequency() / lTimer.GetCounterDiff();
}

uint64 SystemManager::QueryCpuFrequency()
{
	uint64 lPreviousTest = 0xFFFFFFFF;
	uint64 lCurrentTest = 0xFFFFFFFF;
	for (int x = 0; x < 20; ++x)
	{
		Thread::YieldCpu();
		lPreviousTest = lCurrentTest;
		lCurrentTest = SingleCpuTest();
		// Allow +- x % deviation.
		const uint64 lDeviationPercent = 5;
		const uint64 lMinimum = lCurrentTest*(100-lDeviationPercent)/100;
		const uint64 lMaximum = lCurrentTest*(100+lDeviationPercent)/100;
		if (lPreviousTest >= lMinimum && lPreviousTest <= lMaximum)
		{
			break;
		}
	}
	// Based on average.
	return ((lPreviousTest+lCurrentTest)/2);
}

unsigned SystemManager::QueryCpuMips()
{
	unsigned lPreviousTest = 0xFFFFFFFF;
	unsigned lCurrentTest = 0xFFFFFFFF;
	for (int x = 0; x < 4; ++x)
	{
		Thread::YieldCpu();
		lPreviousTest = lCurrentTest;
		lCurrentTest = SingleMipsTest();
		// Allow +- x % deviation.
		const unsigned lDeviationPercent = 10;
		const unsigned lMinimum = lCurrentTest*(100-lDeviationPercent)/100;
		const unsigned lMaximum = lCurrentTest*(100+lDeviationPercent)/100;
		if (lPreviousTest >= lMinimum && lPreviousTest <= lMaximum)
		{
			break;
		}
	}
	// Based on average.
	return ((lPreviousTest+lCurrentTest)/2);
}



bool SystemManager::GetQuitRequest()
{
	return smQuitRequest;
}

void SystemManager::SetQuitRequest(bool pQuitRequest)
{
	smQuitRequest = pQuitRequest;
}



unsigned SystemManager::SingleMipsTest()
{
	unsigned lLoops = 0x10;
	HiResTimer lTimer;
	do
	{
		lLoops <<= 1;
		lTimer.UpdateTimer();
		lTimer.ClearTimeDiff();
		BOGOMIPSDelay(lLoops);
		lTimer.UpdateTimer();
	}
	while (lTimer.GetTimeDiff() < 1.0/8);
	return ((lLoops<<3)/(1000*1000));	// *8.
}

inline void SystemManager::BOGOMIPSDelay(unsigned pLoopCount)
{
#if defined(LEPRA_MSVC_X86)
	__asm
	{
		mov	ecx,[pLoopCount]
BogoLoop:	loop	BogoLoop
	}
#else // <Generic target>
	for (unsigned x = pLoopCount; x; --x)
		;
#endif // LEPRA_MSVC_X86/<Generic target>
}



bool SystemManager::smQuitRequest = false;

LOG_CLASS_DEFINE(GENERAL, SystemManager);



}
