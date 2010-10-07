
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../../Lepra/Include/Application.h"
#include "../../Lepra/Include/LogListener.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Path.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../Life.h"
#include "MasterServer.h"



namespace Life
{



class MasterApplication: public Application
{
public:
	typedef Application Parent;

	MasterApplication(const strutil::strvec& pArgumentList);
	virtual ~MasterApplication();
	virtual int Run();

private:
	LogListener* mConsoleLogger;
	DebuggerLogListener* mDebugLogger;
	FileLogListener* mFileLogger;
	MemFileLogListener* mMemLogger;
};



}



LEPRA_RUN_APPLICATION(Life::MasterApplication);



namespace Life
{



MasterApplication::MasterApplication(const strutil::strvec& pArgumentList):
	Parent(pArgumentList)
{
	Lepra::Init();

	mConsoleLogger = new StdioConsoleLogListener;
#ifndef NO_LOG_DEBUG_INFO
	mDebugLogger = new DebuggerLogListener();
#endif // Showing debug information.
	const str lLogName = Path::JoinPath(SystemManager::GetIoDirectory(_T("Life")), _T("Master"), _T("log"));
	mFileLogger = new FileLogListener(lLogName);
	mFileLogger->WriteLog(_T("\n\n"), Log::LEVEL_INFO);
	mMemLogger = new MemFileLogListener(3*1024);
	LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(mConsoleLogger, mDebugLogger, mFileLogger, 0, mMemLogger);
#ifndef NO_LOG_DEBUG_INFO
	const std::vector<Log*> lLogArray = LogType::GetLogs();
	std::vector<Log*>::const_iterator x = lLogArray.begin();
	const Log::LogLevel lLogLevel = Log::LEVEL_DEBUG;
	for (; x != lLogArray.end(); ++x)
	{
		(*x)->SetLevelThreashold(lLogLevel);
	}
#endif // Showing debug information.

	Network::Start();
}

MasterApplication::~MasterApplication()
{
	Network::Stop();
	Lepra::Shutdown();

	// Drop performance measurement resources.
	ScopePerformanceData::ClearAll();	// Make all useless.
	ScopePerformanceData::ClearAll();	// Delete all useless.

	delete (mConsoleLogger);
	mConsoleLogger = 0;
	delete (mDebugLogger);
	mDebugLogger = 0;
	delete (mMemLogger);
	mMemLogger = 0;
	delete (mFileLogger);
	mFileLogger = 0;

	LogType::Close();
};

int MasterApplication::Run()
{
	MasterServer lServer;
	return lServer.Run()? 1 : 0;
}



}
