
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include "pch.h"
#include "../include/application.h"
#include "../include/log.h"
#include "../include/loglistener.h"
#include "../include/string.h"
#include "../include/thread.h"



bool TestLepra();
void ShowTestResult(const lepra::LogDecorator& account, bool test_ok);



class LepraTestApplication: public lepra::Application {
public:
	LepraTestApplication(const lepra::strutil::strvec& argument_vector);
	virtual void Init();
	virtual int Run();

private:
	static lepra::LogDecorator log_;
};

LEPRA_RUN_APPLICATION(LepraTestApplication, lepra::Main);

LepraTestApplication::LepraTestApplication(const lepra::strutil::strvec& argument_vector):
	lepra::Application(argument_vector) {
};

void LepraTestApplication::Init() {
}

int LepraTestApplication::Run() {
	lepra::Init();

	// We like to be on a single CPU on the time measuring thread, due to QueryPerformanceCounter()
	// and RDTSC core differences. Several seconds can differ between different CPUs.
	lepra::Thread::GetCurrentThread()->SetCpuAffinityMask(0x0001);

	lepra::ConsoleLogListener* console_log_pointer = 0;
#ifdef LEPRA_CONSOLE
	lepra::ConsoleLogListener console_logger;
	console_log_pointer = &console_logger;
#endif // LEPRA_CONSOLE
	lepra::FileLogListener file_logger(_TEXT_ALTERNATIVE("LepraTestApp.log", L"LepraUnicodeTestApp.log"));
	lepra::FileLogListener performance_logger(_TEXT_ALTERNATIVE("LepraPerformance.log", L"LepraUnicodePerformance.log"));
	lepra::MemFileLogListener mem_logger;
	lepra::Log::GetLog()->SetupStandardLogging(lepra::Log::kLevelLowestType,
		console_log_pointer, &file_logger, &performance_logger, &mem_logger);

	lepra::Log::GetLog()->Print("LepraTestApp/main", "\n\n--- Build type: " kLepraStringTypeText " " kLepraBuildTypeText " ---\n", lepra::Log::kLevelError);
	performance_logger.LogListener::OnLogMessage("LepraTestApp/main", "\n\n---\n", lepra::Log::kLevelError);
	mem_logger.Clear();

	bool _test_ok = TestLepra();
	ShowTestResult(log_, _test_ok);

	mem_logger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	return (0);
}

lepra::LogDecorator LepraTestApplication::log_(typeid(LepraTestApplication));
