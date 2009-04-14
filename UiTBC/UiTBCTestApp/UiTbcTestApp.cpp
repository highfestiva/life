// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games

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
	UiTBC::Init();

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

	Lepra::Log::Out(_T("TbcTestApp/main"), _T("\n\n--- Build type: ") _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT) _T(" ---\n"), Lepra::Log::LOG_ERROR);
	lMemLogger.Clear();

	bool lTestOk = true;
	if (lTestOk)
	{
		lPerformanceLogger.LogListener::OnLogMessage(_T("Lepra"), _T("\n\n---\n"), Lepra::Log::LOG_PERFORMANCE);
		lTestOk = TestUiLepra(_T("Lepra"));
		Lepra::Log::OutPerformanceNode(_T("Lepra"));
		Lepra::PerformanceScope::Clear();
	}
	if (lTestOk)
	{
		lPerformanceLogger.LogListener::OnLogMessage(_T("TBC"), _T("\n\n---\n"), Lepra::Log::LOG_PERFORMANCE);
		Lepra::Thread::Sleep(1.0);
		lTestOk = TestUiTbc(_T("  TBC"));
		Lepra::Log::OutPerformanceNode(_T("TBC"));
		Lepra::PerformanceScope::Clear();
	}
	ShowTestResult(_T("  TBC"), lTestOk);

	Lepra::Log::OutPerformanceNode(_T("TBCTestApp"));
	//lMemLogger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	return (0);
}
