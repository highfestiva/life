
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include "pch.h"
#include "stdafx.h"
#include "../include/application.h"
#include "../include/log.h"
#include "../include/loglistener.h"
#include "../include/string.h"



bool TestUiLepra(const lepra::String& account);
void ShowTestResult(const lepra::String& account, bool test_ok);



class UiLepraTestApplication: public lepra::Application {
public:
	UiLepraTestApplication(const lepra::ArgumentList& argument_list);
	int Run();
};

LEPRA_RUN_APPLICATION(UiLepraTestApplication);

UiLepraTestApplication::UiLepraTestApplication(const lepra::ArgumentList& argument_list):
	lepra::Application(argument_list) {
};

int LepraTestApplication::Run() {
	lepra::Init();

	// Run this to get the main thread running on one CPU only. Otherwise
	// time measurement gets screwed up when switching between cores; system CPUs
	// are not time synchronized and usually diffs about 2-3 seconds (AMD).
	lepra::SystemManager::InitializeMainThreadAffinity();

	lepra::ConsoleLogListener* console_log_pointer = 0;
#ifdef LEPRA_CONSOLE
	lepra::ConsoleLogListener console_logger;
	console_log_pointer = &console_logger;
#endif // LEPRA_CONSOLE
	lepra::FileLogListener file_logger(_TEXT_ALTERNATIVE("LepraTestApp.log", L"LepraUnicodeTestApp.log"));
	lepra::FileLogListener performance_logger(_TEXT_ALTERNATIVE("LepraPerformance.log", L"LepraUnicodePerformance.log"));
	lepra::MemFileLogListener mem_logger;
	lepra::Log::SetupMainStandardLogging(console_log_pointer, &file_logger, &performance_logger, &mem_logger);

	lepra::Log::Out("LepraTestApp/main", "\n\n--- Build type: " kLepraStringTypeText " " kLepraBuildTypeText " ---\n", lepra::Log::LOG_ERROR);
	performance_logger.LogListener::OnLogMessage("LepraTestApp/main", "\n\n---\n", lepra::Log::LOG_ERROR);
	mem_logger.Clear();

	const lepra::String log_account("LepraTestApp");
	bool _test_ok = TestUiLepra(log_account);
	ShowTestResult(log_account, _test_ok);

	lepra::Log::OutPerformanceNode("LepraTestApp");

	mem_logger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	return (0);
}
