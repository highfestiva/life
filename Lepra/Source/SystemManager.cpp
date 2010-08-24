
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
	str s;
	s += GetLoginName();
	s += GetUniqueHardwareString(true);
	s += strutil::IntToString(Random::GetRandomNumber64(), 16);
	s += strutil::IntToString(GetCpuTick(), 16);
	s += strutil::IntToString(GetAvailRam(), 16);
	s += strutil::IntToString(GetAvailVirtualMemory(), 16);
	uint8 lHashData[20];
	SHA1::Hash((const uint8*)s.c_str(), s.length()*sizeof(tchar), lHashData);
	return (std::string((const char*)lHashData, sizeof(lHashData)));
}

std::string SystemManager::GetSystemPseudoId()
{
	str s;
	s += GetLoginName();
	s += GetUniqueHardwareString(false);
	uint8 lHashData[20];
	SHA1::Hash((const uint8*)s.c_str(), s.length()*sizeof(tchar), lHashData);
	std::string lResult;
	for (int x = 0; x < sizeof(lHashData); ++x)
	{
		if (lHashData[x] < ' ')
		{
			lResult.push_back(' ');
			lResult.push_back(' '+(lHashData[x]));
		}
		else if (lHashData[x] < 127)
		{
			lResult.push_back(lHashData[x]);
		}
		else
		{
			lResult.push_back('?'+(lHashData[x]>>6));
			lResult.push_back('?'+(lHashData[x]&0x3F));
		}
	}
	return lResult;
}

str SystemManager::GetUniqueHardwareString(bool pRandomize)
{
	str s;
	if (pRandomize)
	{
		s += strutil::IntToString(QueryCpuFrequency(), 16);
	}
	s += strutil::IntToString(GetLogicalCpuCount(), 16);
	s += strutil::IntToString(GetPhysicalCpuCount(), 16);
	s += strutil::IntToString(GetCoreCount(), 16);
	s += GetOsName();
	s += strutil::IntToString(GetAmountRam(), 16);
	s += strutil::IntToString(GetAmountVirtualMemory(), 16);
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



int SystemManager::GetQuitRequest()
{
	return (mQuitRequest);
}

void SystemManager::AddQuitRequest(int pValue)
{
	mQuitRequest += pValue;
	if (mQuitRequest >= 6)
	{
		ExitProcess(2);	// Seems our attempt to log+exit failed, so just finish us off.
	}
	else if (mQuitRequest >= 4)
	{
		mLog.AWarning("Hard termination due to several termination requests.");
		Thread::Sleep(0.5);	// Try to flush logs to disk.
		ExitProcess(1);
	}
	else if (mQuitRequest < 0)
	{
		mQuitRequest = 0;
	}
	if (mQuitRequest > 0 && !mQuitRequestCallback.empty())
	{
		mQuitRequestCallback(mQuitRequest);
	}
}

void SystemManager::SetQuitRequestCallback(const QuitRequestCallback& pCallback)
{
	mQuitRequestCallback = pCallback;
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
#if defined(LEPRA_MSVC_X86_32)
	__asm
	{
		mov	ecx,[pLoopCount]
BogoLoop:	loop	BogoLoop
	}
#else // <Generic target>
	for (unsigned x = pLoopCount; x; --x)
		;
#endif // LEPRA_MSVC_X86_32/<Generic target>
}



int SystemManager::mQuitRequest = 0;
SystemManager::QuitRequestCallback SystemManager::mQuitRequestCallback;


LOG_CLASS_DEFINE(GENERAL, SystemManager);



}
