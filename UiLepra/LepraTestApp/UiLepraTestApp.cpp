
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include "stdafx.h"
#include "../Include/Application.h"
#include "../Include/Log.h"
#include "../Include/LogListener.h"
#include "../Include/String.h"



bool TestUiLepra(const Lepra::String& pAccount);
void ShowTestResult(const Lepra::String& pAccount, bool pTestOk);



class UiLepraTestApplication: public Lepra::Application
{
public:
	UiLepraTestApplication(const Lepra::ArgumentList& pArgumentList);
	int Run();
};

LEPRA_RUN_APPLICATION(UiLepraTestApplication);

UiLepraTestApplication::UiLepraTestApplication(const Lepra::ArgumentList& pArgumentList):
	Lepra::Application(pArgumentList)
{
};

int LepraTestApplication::Run()
{
	Lepra::Init();

	// Run this to get the main thread running on one CPU only. Otherwise
	// time measurement gets screwed up when switching between cores; system CPUs
	// are not time synchronized and usually diffs about 2-3 seconds (AMD).
	Lepra::SystemManager::InitializeMainThreadAffinity();

	Lepra::ConsoleLogListener* lConsoleLogPointer = 0;
#ifdef LEPRA_CONSOLE
	Lepra::ConsoleLogListener lConsoleLogger;
	lConsoleLogPointer = &lConsoleLogger;
#endif // LEPRA_CONSOLE
	Lepra::FileLogListener lFileLogger(_TEXT_ALTERNATIVE("LepraTestApp.log", L"LepraUnicodeTestApp.log"));
	Lepra::FileLogListener lPerformanceLogger(_TEXT_ALTERNATIVE("LepraPerformance.log", L"LepraUnicodePerformance.log"));
	Lepra::MemFileLogListener lMemLogger;
	Lepra::Log::SetupMainStandardLogging(lConsoleLogPointer, &lFileLogger, &lPerformanceLogger, &lMemLogger);

	Lepra::Log::Out(_T("LepraTestApp/main"), _T("\n\n--- Build type: ") _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT) _T(" ---\n"), Lepra::Log::LOG_ERROR);
	lPerformanceLogger.LogListener::OnLogMessage(_T("LepraTestApp/main"), _T("\n\n---\n"), Lepra::Log::LOG_ERROR);
	lMemLogger.Clear();

	const Lepra::String lLogAccount(_T("LepraTestApp"));
	bool lTestOk = TestUiLepra(lLogAccount);
	ShowTestResult(lLogAccount, lTestOk);

	Lepra::Log::OutPerformanceNode(_T("LepraTestApp"));

	lMemLogger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	return (0);
}
