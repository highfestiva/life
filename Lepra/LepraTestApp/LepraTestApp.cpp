
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include "../Include/Application.h"
#include "../Include/Log.h"
#include "../Include/LogListener.h"
#include "../Include/PerformanceScope.h"
#include "../Include/String.h"
#include "../Include/Thread.h"



bool TestLepra();
void ShowTestResult(const Lepra::LogDecorator& pAccount, bool pTestOk);



class LepraTestApplication: public Lepra::Application
{
public:
	LepraTestApplication(const Lepra::ArgumentList& pArgumentList);
	int Run();

private:
	static Lepra::LogDecorator mLog;
};

LEPRA_RUN_APPLICATION(LepraTestApplication);

LepraTestApplication::LepraTestApplication(const Lepra::ArgumentList& pArgumentList):
	Lepra::Application(pArgumentList)
{
};

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

	Lepra::Log::GetLog()->Print(_T("LepraTestApp/main"), _T("\n\n--- Build type: ") _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT) _T(" ---\n"), Lepra::Log::LEVEL_ERROR);
	lPerformanceLogger.LogListener::OnLogMessage(_T("LepraTestApp/main"), _T("\n\n---\n"), Lepra::Log::LEVEL_ERROR);
	lMemLogger.Clear();

	bool lTestOk = TestLepra();
	ShowTestResult(mLog, lTestOk);

	Lepra::PerformanceScope::Log(mLog);

	lMemLogger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	return (0);
}

Lepra::LogDecorator LepraTestApplication::mLog(typeid(LepraTestApplication));
