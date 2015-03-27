
// Author: Jonas Bystrˆm
// Copyright (c) 2002-2012, Pixel Doctrine



#pragma once

#include "TrabantSim.h"
#include "../Lepra/Include/Thread.h"



namespace TrabantSim
{



class PythonRunner
{
public:
	static void Run(const wchar_t* pDirectory, const wchar_t* pFilename);
	static void Break();
	static astr GetStdOut();
	static void ClearStdOut();

private:
	static void WorkerEntry(void*);
	static void StdOutReadEntry(void*);

	static StaticThread mPythonWorker;
	static StaticThread mStdOutReader;
	static Lock mStdOutLock;
	static astr mStdOut;
	static wstr mDirectory;
	static wstr mFilename;
	static bool mKillSimulator;
	static bool mIsStopping;
};



}