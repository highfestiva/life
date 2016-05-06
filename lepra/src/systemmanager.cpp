
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/systemmanager.h"
#include <algorithm>
#include "../include/hirestimer.h"
#include "../include/random.h"
#include "../include/sha1.h"
#include "../include/thread.h"



namespace lepra {



void SystemManager::SetArgumentVector(const strutil::strvec& argument_vector) {
	argument_vector_ = argument_vector;
}

const strutil::strvec& SystemManager::GetArgumentVector() {
	return argument_vector_;
}

str SystemManager::GetDataDirectory() {
	return GetDataDirectoryFromPath(argument_vector_[0]);
}

str SystemManager::GetRandomId() {
	str s;
	s += GetLoginName();
	s += GetUniqueHardwareString();
	s += strutil::IntToString(Random::GetRandomNumber64(), 16);
	s += strutil::IntToString(GetCpuTick(), 16);
	s += strutil::IntToString(GetAvailRam(), 16);
	s += strutil::IntToString(GetAvailVirtualMemory(), 16);
	uint8 hash_data[20];
	SHA1::Hash((const uint8*)s.c_str(), s.length()*sizeof(char), hash_data);
	return str((const char*)hash_data, sizeof(hash_data));
}

str SystemManager::GetSystemPseudoId() {
	str s;
	s += GetLoginName();
	s += GetUniqueHardwareString();
	uint8 hash_data[20];
	SHA1::Hash((const uint8*)s.c_str(), s.length()*sizeof(char), hash_data);
	str result;
	for (unsigned x = 0; x < sizeof(hash_data); ++x) {
		if (hash_data[x] == '\\') {
			result += "__";
		} else if (hash_data[x] <= '"') {
			result.push_back('_');
			result.push_back('#'+(hash_data[x]));
		} else if (hash_data[x] < 127) {
			result.push_back(hash_data[x]);
		} else {
			result.push_back('?'+(hash_data[x]>>5));
			result.push_back('?'+(hash_data[x]&0x1F));
		}
	}
	return result;
}

str SystemManager::GetUniqueHardwareString() {
	str s;
	s += GetHwName();
	s += strutil::IntToString(GetLogicalCpuCount(), 16);
	s += strutil::IntToString(GetPhysicalCpuCount(), 16);
	s += strutil::IntToString(GetCoreCount(), 16);
	s += GetOsName();
	s += strutil::IntToString(GetAmountRam(), 16);
	s += strutil::IntToString(GetAmountVirtualMemory(), 16);
	return (s);
}

uint64 SystemManager::SingleCpuTest() {
	HiResTimer timer(false);
	int64 delta_time = HiResTimer::GetFrequency() / 100;
	uint64 start_tick;
	uint64 end_tick;
	// Reset the timer and start counting.
	timer.UpdateTimer();
	timer.ClearTimeDiff();
	start_tick = GetCpuTick();
	// Take 1000 samples.
	while (timer.GetCounterDiff() < delta_time) {
		timer.UpdateTimer();
	}
	end_tick = GetCpuTick();
	// Return the estimated frequency.
	return (end_tick - start_tick) * HiResTimer::GetFrequency() / timer.GetCounterDiff();
}

uint64 SystemManager::QueryCpuFrequency() {
	uint64 previous_test = 0xFFFFFFFF;
	uint64 current_test = 0xFFFFFFFF;
	for (int x = 0; x < 20; ++x) {
		Thread::YieldCpu();
		previous_test = current_test;
		current_test = SingleCpuTest();
		// Allow +- x % deviation.
		const uint64 deviation_percent = 5;
		const uint64 minimum = current_test*(100-deviation_percent)/100;
		const uint64 maximum = current_test*(100+deviation_percent)/100;
		if (previous_test >= minimum && previous_test <= maximum) {
			break;
		}
	}
	// Based on average.
	return ((previous_test+current_test)/2);
}

unsigned SystemManager::QueryCpuMips() {
	unsigned previous_test = 0xFFFFFFFF;
	unsigned current_test = 0xFFFFFFFF;
	for (int x = 0; x < 4; ++x) {
		Thread::YieldCpu();
		previous_test = current_test;
		current_test = SingleMipsTest();
		// Allow +- x % deviation.
		const unsigned deviation_percent = 10;
		const unsigned minimum = current_test*(100-deviation_percent)/100;
		const unsigned maximum = current_test*(100+deviation_percent)/100;
		if (previous_test >= minimum && previous_test <= maximum) {
			break;
		}
	}
	// Based on average.
	return ((previous_test+current_test)/2);
}



double SystemManager::GetSleepResolution() {
	if (sleep_resolution_) {
		return sleep_resolution_;
	}

	sleep_resolution_ = 1.0;
	std::vector<double> measured_times;
	measured_times.reserve(5);
	Thread::YieldCpu();
	const int measurement_count = 3;
	for (int x = 0; x < measurement_count; ++x) {
		HiResTimer sleep_time(false);
		Thread::Sleep(0.001);
		measured_times.push_back(sleep_time.QueryTimeDiff());
	}
	std::sort(measured_times.begin(), measured_times.end());
	sleep_resolution_ = measured_times[measurement_count/2];
	return sleep_resolution_;
}



int SystemManager::GetQuitRequest() {
	return (quit_request_);
}

void SystemManager::AddQuitRequest(int value) {
	quit_request_ += value;
	if (quit_request_ >= 6) {
		ExitProcess(2);	// Seems our attempt to log+exit failed, so just finish us off.
	} else if (quit_request_ >= 4) {
		log_.Warning("Hard termination due to several termination requests.");
		Thread::Sleep(0.5);	// Try to flush logs to disk.
		ExitProcess(1);
	} else if (quit_request_ < 0) {
		quit_request_ = 0;
	}
	if (quit_request_ > 0 && !quit_request_callback_.empty()) {
		quit_request_callback_(quit_request_);
	}
}

void SystemManager::SetQuitRequestCallback(const QuitRequestCallback& callback) {
	quit_request_callback_ = callback;
}


unsigned SystemManager::SingleMipsTest() {
	unsigned loops = 0x10;
	HiResTimer timer(false);
	do {
		loops <<= 1;
		timer.UpdateTimer();
		timer.ClearTimeDiff();
		BOGOMIPSDelay(loops);
		timer.UpdateTimer();
	} while (timer.GetTimeDiff() < 1.0/8);
	return ((loops<<3)/(1000*1000));	// *8.
}

inline void SystemManager::BOGOMIPSDelay(unsigned loop_count) {
#if defined(LEPRA_MSVC_X86_32)
	__asm {
		mov	ecx,[loop_count]
BogoLoop:	loop	BogoLoop
	}
#else // <Generic target>
	for (unsigned x = loop_count; x; --x)
		;
#endif // LEPRA_MSVC_X86_32/<Generic target>
}



strutil::strvec SystemManager::argument_vector_;
int SystemManager::quit_request_ = 0;
SystemManager::QuitRequestCallback SystemManager::quit_request_callback_;
double SystemManager::sleep_resolution_ = 0;

loginstance(kGeneral, SystemManager);



}
