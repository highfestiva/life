// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include <assert.h>
#include "../../Cure/Include/Cure.h"
#include "../../Lepra/Include/Application.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/LogListener.h"
#include "../../Lepra/Include/Socket.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/DiskFile.h"
#include "../../Lepra/Include/Quaternion.h"
#include "../../TBC/Include/TBC.h"
#ifndef CURE_TEST_WITHOUT_UI
#include "../../UiLepra/Include/UiLepra.h"
#include "../../UiTbc/Include/UiTBC.h"
#include "../../UiCure/Include/UiCure.h"
#endif // !CURE_TEST_WITHOUT_UI

#ifndef CURE_TEST_WITHOUT_UI
#define TEST_RUN_LEPRA		TestUiLepra
#define TEST_RUN_TBC		TestUiTbc
#define TEST_RUN_CURE		TestUiCure
#define TEST_RUN_NETPHYS	TestPrototypeNetworkPhysics
#define LEPRA_NS		UiLepra
#define TBC_NS			UiTbc
#define CURE_NS			UiCure
#else // CURE_TEST_WITHOUT_UI
#define TEST_RUN_LEPRA		TestLepra
#define TEST_RUN_TBC		TestRunDummy
#define TEST_RUN_CURE		TestCure
#define TEST_RUN_NETPHYS	TestRunDummy
static bool TestRunDummy() { return (true); }
#define LEPRA_NS		Lepra
#define TBC_NS			TBC
#define CURE_NS			Cure
#endif // With / without UI.

bool TEST_RUN_LEPRA();
bool TEST_RUN_TBC();
bool TEST_RUN_CURE();
bool TEST_RUN_NETPHYS();
void ShowTestResult(const Lepra::LogDecorator& pAccount, bool pbTestOk);



class CureTestApplication: public Lepra::Application
{
public:
	CureTestApplication(const Lepra::StringUtility::StringVector& pArgumentList);
	int Run();

private:
	enum TestBits
	{
		LEPRA_BIT = (1<<0),
		TBC_BIT = (1<<1),
		CURE_BIT = (1<<2),
		NETWORK_PHYSICS_BIT = (1<<3),
	};
	int mTestBits;
	LOG_CLASS_DECLARE();
};

LEPRA_RUN_APPLICATION(CureTestApplication);

CureTestApplication::CureTestApplication(const Lepra::StringUtility::StringVector& pArgumentList):
	Lepra::Application(pArgumentList),
	mTestBits(0xFFFFFFFF)
{
	for (size_t x = 1; x < pArgumentList.size(); ++x)
	{
		Lepra::String lArgument = pArgumentList[x];
		Lepra::StringUtility::ToUpper(lArgument);
		int lMask = 0;
		if (lArgument == _T("LEPRA"))
		{
			lMask |= LEPRA_BIT;
		}
		else if (lArgument == _T("TBC"))
		{
			lMask |= TBC_BIT;
		}
		else if (lArgument == _T("CURE"))
		{
			lMask |= CURE_BIT;
		}
		else if (lArgument == _T("NETPHYS"))
		{
			lMask |= NETWORK_PHYSICS_BIT;
		}
		else
		{
			assert(false);	// Unknown command line argument.
		}
		if (x == 1)
		{
			mTestBits = 0;
		}
		mTestBits &= ~lMask;
		mTestBits |= lMask;
	}
};


int CureTestApplication::Run()
{
	LEPRA_NS::Init();
	TBC_NS::Init();
	CURE_NS::Init();

	// We like to be on a single CPU on the time measuring thread, due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between different cores.
	Lepra::Thread::GetCurrentThread()->SetCpuAffinityMask(0x0001);

	Lepra::StdioConsoleLogListener* lConsoleLogPointer = 0;
#ifdef LEPRA_CONSOLE
	Lepra::StdioConsoleLogListener lConsoleLogger;
	lConsoleLogPointer = &lConsoleLogger;
#endif // LEPRA_CONSOLE
	Lepra::DebuggerLogListener lDebugLogger;
	Lepra::FileLogListener lFileLogger(_TEXT_ALTERNATIVE("CureTestApp.log", L"CureTestAppU.log"));
	Lepra::FileLogListener lPerformanceLogger(_TEXT_ALTERNATIVE("CureTestPerformance.log", L"CureTestPerformanceU.log"));
	Lepra::MemFileLogListener lMemLogger(100*1024);
	Lepra::LogType::GetLog(Lepra::LogType::SUB_ROOT)->SetupBasicListeners(lConsoleLogPointer, &lDebugLogger,
		&lFileLogger, &lPerformanceLogger, &lMemLogger);
	Lepra::LogType::GetLog(Lepra::LogType::SUB_ROOT)->SetLevelThreashold(Lepra::Log::LEVEL_DEBUG);

	mLog.Headline(_T("\n\n--- Build type: ") _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT) _T(" ---\n"));

	bool lTestOk = true;
	if (lTestOk && mTestBits&LEPRA_BIT)
	{
		lTestOk = TEST_RUN_LEPRA();
	}
	if (lTestOk && mTestBits&TBC_BIT)
	{
		lTestOk = TEST_RUN_TBC();
	}
	if (lTestOk && mTestBits&CURE_BIT)
	{
		lTestOk = TEST_RUN_CURE();
	}
	if (lTestOk && mTestBits&NETWORK_PHYSICS_BIT)
	{
		lTestOk = TEST_RUN_NETPHYS();
	}
	ShowTestResult(mLog, lTestOk);

	//lMemLogger.DumpToFile(_TEXT_ALTERNATIVE("Temp.log", L"TempUnicode.log"));

	CURE_NS::Shutdown();
	TBC_NS::Shutdown();
	LEPRA_NS::Shutdown();
	return (0);
}



LOG_CLASS_DEFINE(TEST, CureTestApplication);
