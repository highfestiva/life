
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include "pch.h"
#include "../Include/Application.h"
#include "../Include/Log.h"
#include "../Include/LogListener.h"
#include "../Include/String.h"
#include "../Include/Thread.h"



bool TestLepra();
void ShowTestResult(const Lepra::LogDecorator& pAccount, bool pTestOk);



class LepraTestApplication: public Lepra::Application
{
public:
	LepraTestApplication(const Lepra::strutil::strvec& pArgumentVector);
	virtual void Init();
	virtual int Run();

private:
	static Lepra::LogDecorator mLog;
};

LEPRA_RUN_APPLICATION(LepraTestApplication, Lepra::Main);

LepraTestApplication::LepraTestApplication(const Lepra::strutil::strvec& pArgumentVector):
	Lepra::Application(pArgumentVector)
{
};

void LepraTestApplication::Init()
{
}

int LepraTestApplication::Run()
{
	Lepra::Init();

	// We like to be on a single CPU on the time measuring thread, due to QueryPerformanceCounter()
	// and RDTSC core differences. Several seconds can differ between different CPUs.
	Lepra::Thread::GetCurrentThread()->SetCpuAffinityMask(0x0001);

	Lepra::ConsoleLogListener* lConsoleLogPointer = 0;
#ifdef LEPRA_CONSOLE
	Lepra::ConsoleLogListener lConsoleLogger;
	lConsoleLogPointer = &lConsoleLogger;
#endif // LEPRA_CONSOLE
	Lepra::FileLogListener lFileLogger(_TEXT_ALTERNATIVE("LepraTestApp.log", L"LepraUnicodeTestApp.log"));
	Lepra::FileLogListener lPerformanceLogger(_TEXT_ALTERNATIVE("LepraPerformance.log", L"LepraUnicodePerformance.log"));
	Lepra::MemFileLogListener lMemLogger;
	Lepra::Log::GetLog()->SetupStandardLogging(Lepra::Log::LEVEL_LOWEST_TYPE,
		lConsoleLogPointer, &lFileLogger, &lPerformanceLogger, &lMemLogger);

	Lepra::Log::GetLog()->Print("LepraTestApp/main", "\n\n--- Build type: " LEPRA_STRING_TYPE_TEXT " " LEPRA_BUILD_TYPE_TEXT " ---\n", Lepra::Log::LEVEL_ERROR);
	lPerformanceLogger.LogListener::OnLogMessage("LepraTestApp/main", "\n\n---\n", Lepra::Log::LEVEL_ERROR);
	lMemLogger.Clear();

	bool lTestOk = TestLepra();
	ShowTestResult(mLog, lTestOk);

	lMemLogger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	return (0);
}

Lepra::LogDecorator LepraTestApplication::mLog(typeid(LepraTestApplication));
