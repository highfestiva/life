// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include "../../cure/include/cure.h"
#include "../../lepra/include/application.h"
#include "../../lepra/include/lepraos.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/loglistener.h"
#include "../../lepra/include/socket.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/diskfile.h"
#include "../../lepra/include/quaternion.h"
#include "../../tbc/include/tbc.h"
#ifndef CURE_TEST_WITHOUT_UI
#include "../../uilepra/include/uilepra.h"
#include "../../uitbc/include/uitbc.h"
#include "../../uicure/include/uicure.h"
#endif // !CURE_TEST_WITHOUT_UI

static bool TestRunDummy() { return (true); }
#define TEST_RUN_LEPRA_CON	TestLepra
#define TEST_RUN_TBC_CON	TestRunDummy
#define TEST_RUN_CURE_CON	TestCure
#define TEST_RUN_NETPHYS_CON	TestRunDummy
#ifndef CURE_TEST_WITHOUT_UI
#define TEST_RUN_LEPRA		TestUiLepra
#define TEST_RUN_TBC		TestUiTbc
#define TEST_RUN_CURE		TestUiCure
#define TEST_RUN_NETPHYS	TestPrototypeNetworkPhysics
#define LEPRA_NS		uilepra
#define TBC_NS			uitbc
#define CURE_NS			UiCure
#else // CURE_TEST_WITHOUT_UI
#define TEST_RUN_LEPRA		TEST_RUN_LEPRA_CON
#define TEST_RUN_TBC		TEST_RUN_TBC_CON
#define TEST_RUN_CURE		TEST_RUN_CURE_CON
#define TEST_RUN_NETPHYS	TEST_RUN_NETPHYS_CON
#define LEPRA_NS		lepra
#define TBC_NS			tbc
#define CURE_NS			cure
#endif // With / without kUi.

using namespace lepra;

bool TEST_RUN_LEPRA_CON();
bool TEST_RUN_TBC_CON();
bool TEST_RUN_CURE_CON();
bool TEST_RUN_NETPHYS_CON();
bool TEST_RUN_LEPRA();
bool TEST_RUN_TBC();
bool TEST_RUN_CURE();
bool TEST_RUN_NETPHYS();
void ShowTestResult(const LogDecorator& account, bool test_ok);



class CureTestApplication: public Application {
public:
	CureTestApplication(const strutil::strvec& argument_list);
	virtual void Init();
	int Run();

private:
	enum TestBits {
		kLepraBit = (1<<0),
		kTbcBit = (1<<1),
		kCureBit = (1<<2),
		kNetworkPhysicsBit = (1<<3),
		kConsoleBit = (1<<29),
		kTraceBit = (1<<30),
		kMasterBits = (kConsoleBit | kTraceBit)
	};
	int test_bits_;
	logclass();
};

LEPRA_RUN_APPLICATION(CureTestApplication, LEPRA_NS::Main);

CureTestApplication::CureTestApplication(const strutil::strvec& argument_list):
	Application(argument_list),
	test_bits_(~(unsigned)(kConsoleBit|kTraceBit)) {
	for (size_t x = 1; x < argument_list.size(); ++x) {
		str argument = argument_list[x];
		int mask = 0;
		if (argument == "lepra") {
			mask |= kLepraBit;
		} else if (argument == "tbc") {
			mask |= kTbcBit;
		} else if (argument == "cure") {
			mask |= kCureBit;
		} else if (argument == "netphys") {
			mask |= kNetworkPhysicsBit;
		} else if (argument == "console") {
			mask |= kConsoleBit;
		} else if (argument == "trace") {
			mask |= kTraceBit;
		} else {
			deb_assert(false);	// Unknown command line argument.
		}
		if ((mask&(~kMasterBits)) && ((test_bits_&(~kMasterBits)) == ~kMasterBits)) {
			test_bits_ &= kMasterBits;
		}
		test_bits_ &= ~mask;
		test_bits_ |= mask;
	}
}

void CureTestApplication::Init() {
	LEPRA_NS::Init();
	TBC_NS::Init();
	CURE_NS::Init();
};


int CureTestApplication::Run() {
	// We like to be on a single CPU on the time measuring thread, due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between different cores.
	Thread::GetCurrentThread()->SetCpuAffinityMask(0x0001);

	StdioConsoleLogListener* console_log_pointer = 0;
#ifdef LEPRA_CONSOLE
	StdioConsoleLogListener console_logger;
	console_logger.SetLevelThreashold((test_bits_&kTraceBit)? kLevelTrace : kLevelHeadline);
	console_log_pointer = &console_logger;
#endif // LEPRA_CONSOLE
	DebuggerLogListener debug_logger;
	FileLogListener file_logger("CureTestApp.log");
	FileLogListener performance_logger("CureTestPerformance.log");
	MemFileLogListener mem_logger(100*1024);
	LogType::GetLogger(LogType::kRoot)->SetupBasicListeners(console_log_pointer, &debug_logger,
		&file_logger, &performance_logger, &mem_logger);
	LogType::GetLogger(LogType::kRoot)->SetLevelThreashold(kLevelTrace);
	LogType::GetLogger(LogType::kNetwork)->SetLevelThreashold(kLevelError);

	log_.Headline("\n\n--- Build type: " kLepraStringTypeText " " kLepraBuildTypeText " ---\n");

	bool test_ok = true;
	if (test_ok && test_bits_&kLepraBit) {
		test_ok = (test_bits_&kConsoleBit)? TEST_RUN_LEPRA_CON() : TEST_RUN_LEPRA();
	}
	if (test_ok && test_bits_&kTbcBit) {
		test_ok = (test_bits_&kConsoleBit)? TEST_RUN_TBC_CON() : TEST_RUN_TBC();
	}
	if (test_ok && test_bits_&kCureBit) {
		test_ok = (test_bits_&kConsoleBit)? TEST_RUN_CURE_CON() : TEST_RUN_CURE();
	}
	if (test_ok && test_bits_&kNetworkPhysicsBit) {
		test_ok = (test_bits_&kConsoleBit)? TEST_RUN_NETPHYS_CON() : TEST_RUN_NETPHYS();
	}
#ifdef LEPRA_CONSOLE
	if (!test_ok) {
		mem_logger.Dump(console_logger, kLevelError);
	}
#endif // LEPRA_CONSOLE
	ShowTestResult(log_, test_ok);

	CURE_NS::Shutdown();
	TBC_NS::Shutdown();
	LEPRA_NS::Shutdown();
	return (0);
}



loginstance(kTest, CureTestApplication);
