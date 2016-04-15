// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games

#include "pch.h"
#include "stdafx.h"
#include "../../Lepra/Include/Application.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/LogListener.h"
#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/String.h"



bool TestUiLepra(const Lepra::String& pAccount);
bool TestUiTbc(const Lepra::String& pAccount);
void ShowTestResult(const Lepra::String& pAccount, bool pTestOk);



class UiTbcTestApplication: public Lepra::Application
{
public:
	UiTbcTestApplication(const Lepra::ArgumentList& pArgumentList);
	int Run();
};

LEPRA_RUN_APPLICATION(UiTbcTestApplication);

UiTbcTestApplication::UiTbcTestApplication(const Lepra::ArgumentList& pArgumentList):
	Lepra::Application(pArgumentList)
{
};

int UiTbcTestApplication::Run()
{
	UiTbc::Init();

	// Run this to get the main thread running on one CPU only. Otherwise
	// time measurement gets screwed up when switching between cores; system CPUs
	// are not time synchronized and usually diffs about 2-3 seconds (AMD).
	Lepra::SystemManager::InitializeMainThreadAffinity();

	Lepra::ConsoleLogListener* lConsoleLogPointer = 0;
#ifdef LEPRA_CONSOLE
	Lepra::ConsoleLogListener lConsoleLogger;
	lConsoleLogPointer = &lConsoleLogger;
#endif // LEPRA_CONSOLE
	Lepra::FileLogListener lFileLogger(_TEXT_ALTERNATIVE("TbcTestApp.log", L"TbcUnicodeTestApp.log"));
	Lepra::FileLogListener lPerformanceLogger(_TEXT_ALTERNATIVE("TbcPerformance.log", L"TbcUnicodePerformance.log"));
	Lepra::MemFileLogListener lMemLogger;
	Lepra::Log::SetupMainStandardLogging(lConsoleLogPointer, &lFileLogger, &lPerformanceLogger, &lMemLogger);

	Lepra::Log::Out("TbcTestApp/main", "\n\n--- Build type: " LEPRA_STRING_TYPE_TEXT " " LEPRA_BUILD_TYPE_TEXT " ---\n", Lepra::Log::LOG_ERROR);
	lMemLogger.Clear();

	bool lTestOk = true;
	if (lTestOk)
	{
		lPerformanceLogger.LogListener::OnLogMessage("Lepra", "\n\n---\n", Lepra::Log::LOG_PERFORMANCE);
		lTestOk = TestUiLepra("Lepra");
		Lepra::Log::OutPerformanceNode("Lepra");
		Lepra::PerformanceScope::Clear();
	}
	if (lTestOk)
	{
		lPerformanceLogger.LogListener::OnLogMessage("Tbc", "\n\n---\n", Lepra::Log::LOG_PERFORMANCE);
		Lepra::Thread::Sleep(1.0);
		lTestOk = TestUiTbc("  Tbc");
		Lepra::Log::OutPerformanceNode("Tbc");
		Lepra::PerformanceScope::Clear();
	}
	ShowTestResult("  Tbc", lTestOk);

	Lepra::Log::OutPerformanceNode("TbcTestApp");
	//lMemLogger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	return (0);
}
