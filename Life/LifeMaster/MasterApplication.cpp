
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
	const str lLogName = Path::JoinPath(SystemManager::GetIoDirectory(_T("Life")), _T("Master"), _T("log"));
	mFileLogger = new FileLogListener(lLogName);
	mFileLogger->WriteLog(_T("\n\n"), Log::LEVEL_INFO);
	mMemLogger = new MemFileLogListener(100*1024);
	LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(mConsoleLogger, 0, mFileLogger, 0, mMemLogger);

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
