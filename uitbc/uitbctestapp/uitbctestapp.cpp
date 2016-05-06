// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games

#include "pch.h"
#include "stdafx.h"
#include "../../lepra/include/application.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/loglistener.h"
#include "../../lepra/include/socket.h"
#include "../../lepra/include/string.h"



bool TestUiLepra(const lepra::String& account);
bool TestUiTbc(const lepra::String& account);
void ShowTestResult(const lepra::String& account, bool test_ok);



class UiTbcTestApplication: public lepra::Application {
public:
	UiTbcTestApplication(const lepra::ArgumentList& argument_list);
	int Run();
};

LEPRA_RUN_APPLICATION(UiTbcTestApplication);

UiTbcTestApplication::UiTbcTestApplication(const lepra::ArgumentList& argument_list):
	lepra::Application(argument_list) {
};

int UiTbcTestApplication::Run() {
	uitbc::Init();

	// Run this to get the main thread running on one CPU only. Otherwise
	// time measurement gets screwed up when switching between cores; system CPUs
	// are not time synchronized and usually diffs about 2-3 seconds (AMD).
	lepra::SystemManager::InitializeMainThreadAffinity();

	lepra::ConsoleLogListener* console_log_pointer = 0;
#ifdef LEPRA_CONSOLE
	lepra::ConsoleLogListener console_logger;
	console_log_pointer = &console_logger;
#endif // LEPRA_CONSOLE
	lepra::FileLogListener file_logger(_TEXT_ALTERNATIVE("TbcTestApp.log", L"TbcUnicodeTestApp.log"));
	lepra::FileLogListener performance_logger(_TEXT_ALTERNATIVE("TbcPerformance.log", L"TbcUnicodePerformance.log"));
	lepra::MemFileLogListener mem_logger;
	lepra::Log::SetupMainStandardLogging(console_log_pointer, &file_logger, &performance_logger, &mem_logger);

	lepra::Log::Out("TbcTestApp/main", "\n\n--- Build type: " kLepraStringTypeText " " kLepraBuildTypeText " ---\n", lepra::Log::LOG_ERROR);
	mem_logger.Clear();

	bool _test_ok = true;
	if (_test_ok) {
		performance_logger.LogListener::OnLogMessage("lepra", "\n\n---\n", lepra::Log::LOG_PERFORMANCE);
		_test_ok = TestUiLepra("Lepra");
		lepra::Log::OutPerformanceNode("lepra");
		lepra::PerformanceScope::Clear();
	}
	if (_test_ok) {
		performance_logger.LogListener::OnLogMessage("Tbc", "\n\n---\n", lepra::Log::LOG_PERFORMANCE);
		lepra::Thread::Sleep(1.0);
		_test_ok = TestUiTbc("  Tbc");
		lepra::Log::OutPerformanceNode("Tbc");
		lepra::PerformanceScope::Clear();
	}
	ShowTestResult("  Tbc", _test_ok);

	lepra::Log::OutPerformanceNode("TbcTestApp");
	//mem_logger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	return (0);
}
