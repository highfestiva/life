// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
#include "../../UiTBC/Include/UiTBC.h"
#include "../../UiCure/Include/UiCure.h"
#endif // !CURE_TEST_WITHOUT_UI

#ifndef CURE_TEST_WITHOUT_UI
#define TEST_RUN_LEPRA		TestUiLepra
#define TEST_RUN_TBC		TestUiTbc
#define TEST_RUN_CURE		TestUiCure
#define TEST_RUN_NETPHYS	TestPrototypeNetworkPhysics
#define EXPORT_MESH		ExportMesh
#define LEPRA_NS		UiLepra
#define TBC_NS			UiTbc
#define CURE_NS			UiCure
#else // CURE_TEST_WITHOUT_UI
#define TEST_RUN_LEPRA		TestLepra
#define TEST_RUN_TBC		TestRunDummy
#define TEST_RUN_CURE		TestCure
#define TEST_RUN_NETPHYS	TestRunDummy
#define EXPORT_MESH		TestRunDummy
static bool TestRunDummy() { return (true); }
#define LEPRA_NS		Lepra
#define TBC_NS			TBC
#define CURE_NS			Cure
#endif // With / without UI.

using namespace Lepra;

bool TEST_RUN_LEPRA();
bool TEST_RUN_TBC();
bool TEST_RUN_CURE();
bool TEST_RUN_NETPHYS();
//bool ExportStructure();
bool EXPORT_MESH();
void ShowTestResult(const LogDecorator& pAccount, bool pbTestOk);



class CureTestApplication: public Application
{
public:
	CureTestApplication(const strutil::strvec& pArgumentList);
	int Run();

private:
	enum TestBits
	{
		LEPRA_BIT = (1<<0),
		TBC_BIT = (1<<1),
		CURE_BIT = (1<<2),
		NETWORK_PHYSICS_BIT = (1<<3),
	};
	unsigned mTestBits;
	LOG_CLASS_DECLARE();
};

LEPRA_RUN_APPLICATION(CureTestApplication);

CureTestApplication::CureTestApplication(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mTestBits((unsigned)~0)
{
	for (size_t x = 1; x < pArgumentList.size(); ++x)
	{
		str lArgument = pArgumentList[x];
		strutil::ToUpper(lArgument);
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
	Thread::GetCurrentThread()->SetCpuAffinityMask(0x0001);

	StdioConsoleLogListener* lConsoleLogPointer = 0;
#ifdef LEPRA_CONSOLE
	StdioConsoleLogListener lConsoleLogger;
	lConsoleLogger.SetLevelThreashold(Log::LEVEL_HEADLINE);
	lConsoleLogPointer = &lConsoleLogger;
#endif // LEPRA_CONSOLE
	DebuggerLogListener lDebugLogger;
	FileLogListener lFileLogger(_T("CureTestApp.log"));
	FileLogListener lPerformanceLogger(_T("CureTestPerformance.log"));
	MemFileLogListener lMemLogger(100*1024);
	LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(lConsoleLogPointer, &lDebugLogger,
		&lFileLogger, &lPerformanceLogger, &lMemLogger);
	LogType::GetLog(LogType::SUB_ROOT)->SetLevelThreashold(Log::LEVEL_TRACE);

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
#ifdef LEPRA_CONSOLE
	if (!lTestOk)
	{
		lMemLogger.Dump(lConsoleLogger, Log::LEVEL_ERROR);
	}
#endif // LEPRA_CONSOLE
	ShowTestResult(mLog, lTestOk);

	CURE_NS::Shutdown();
	TBC_NS::Shutdown();
	LEPRA_NS::Shutdown();
	return (0);
}



LOG_CLASS_DEFINE(TEST, CureTestApplication);
