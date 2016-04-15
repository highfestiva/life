
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS

#include "PythonRunner.h"
#import "AnimatedApp.h"
#include "TrabantSimTicker.h"



extern "C" int Py_OptimizeFlag;
extern "C" int Py_VerboseFlag;
extern "C" void Py_SetProgramName(wchar_t* pn);
extern "C" void Py_Initialize(void);
extern "C" void Py_Finalize(void);
extern "C" int Py_IsInitialized(void);
extern "C" void PySys_SetArgv(int argc, wchar_t** argv);
extern "C" int PyRun_SimpleFileEx(FILE* f, const char* p, int c);
extern "C" int Py_AddPendingCall(int (*func)(void *), void *arg);
extern "C" void PyEval_InitThreads(void);
extern "C" void PyEval_ReInitThreads(void);
extern "C" void PyEval_ReleaseLock(void);
typedef int PyGILState_STATE;
extern "C" PyGILState_STATE PyGILState_Ensure(void);
extern "C" void PyGILState_Release(PyGILState_STATE oldstate);
typedef void PyObject;
extern "C" PyObject* PyExc_KeyboardInterrupt;
extern "C" void PyErr_SetString(PyObject *exception, const char *string);
extern "C" void PyErr_SetInterrupt(void);
extern "C" void Py_Jb_ClearPendingCalls(void);

// This is to circumvent problems with the UINavigationViewController. The reason it goes here instead of into
// The rotating view controller is that it needs to be in a separate thread. I trust this way more than the
// Apple dispatch_whatever() macros.
#define MINIMUM_PYTHON_RUN_TIME	1.0



namespace TrabantSim
{



int quit(void*)
{
	PyErr_SetString(PyExc_KeyboardInterrupt, "...");
	PyErr_SetInterrupt();
	return -1;
}



void PythonRunner::Run(const wchar_t* pDirectory, const wchar_t* pFilename)
{
	Break();
	mDirectory = pDirectory;
	mFilename = pFilename;
	mPythonWorker.Start(&PythonRunner::WorkerEntry, 0);
	mStdOutReader.Start(&PythonRunner::StdOutReadEntry, 0);
}

bool PythonRunner::IsRunning()
{
	return mPythonWorker.IsRunning();
}

void PythonRunner::Break()
{
	if (!mIsStopping && mPythonWorker.IsRunning())
	{
		mIsStopping = true;
		mKillSimulator = false;
		for (int x = 0; x < 3 && !Py_IsInitialized(); ++x)
		{
			Thread::Sleep(0.1f);
		}
		if (Py_IsInitialized())
		{
			PyGILState_STATE state = PyGILState_Ensure();
			Py_AddPendingCall(&quit, NULL);
			PyGILState_Release(state);
			mPythonWorker.GraceJoin(0.7);
		}
		if (mPythonWorker.IsRunning())
		{
			printf("Warning: killing python thread!\n");
			pthread_cancel((pthread_t)mPythonWorker.GetThreadHandle());
			mPythonWorker.Kill();
		}
		Py_Jb_ClearPendingCalls();
		mKillSimulator = true;
		mIsStopping = false;
	}
}

str PythonRunner::GetStdOut()
{
	ScopeLock _(&mStdOutLock);
	return mStdOut;
}

void PythonRunner::ClearStdOut()
{
	if (mStdOutReader.IsRunning())
	{
		ScopeLock _(&mStdOutLock);
		mStdOut.clear();
	}
}



void PythonRunner::WorkerEntry(void*)
{
	int _;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &_);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &_);

	HiResTimer lTimer(false);
	str lFullPathname = mFilename;
	strutil::strvec lParts = strutil::Split(lFullPathname, "/");
	str lFilename = lParts[lParts.size()-1];
	FILE* fp = fopen(lFullPathname.c_str(), "r");
	if (!fp)
	{
		printf("Error: could not open file %s!\n", lFilename.c_str());
		Thread::Sleep(MINIMUM_PYTHON_RUN_TIME-lTimer.QueryTimeDiff());
		TrabantSim::mApp->FoldSimulator();
		TrabantSim::mApp->Suspend(false);
		return;
	}
	if (Py_IsInitialized())
	{
		// Here we try to repair if we had to force-kill the previous thread.
		PyGILState_Ensure();
		Py_Finalize();
	}
	Py_SetProgramName((wchar_t*)mDirectory.c_str());
	Py_OptimizeFlag = 1;
	//Py_VerboseFlag = 1;
	Py_Initialize();
	PyEval_InitThreads();
	wchar_t* wargv[3] = { (wchar_t*)mFilename.c_str(), (wchar_t*)L"addr=localhost", (wchar_t*)L"osname=ios" };
	PySys_SetArgv(3, wargv);
	PyRun_SimpleFileEx(fp, lFilename.c_str(), 1);
	Py_Finalize();
	PyEval_ReleaseLock();

	if (mKillSimulator)
	{
		// No use showing simulator still frame?
	 	mIsStopping = true;
		Thread::Sleep(MINIMUM_PYTHON_RUN_TIME-lTimer.QueryTimeDiff());
		TrabantSim::mApp->Suspend(false);
		TrabantSim::mApp->FoldSimulator();
		mIsStopping = false;
	}
}

void PythonRunner::StdOutReadEntry(void*)
{
	int _;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &_);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &_);

	static int lPipePair[2];
	if (pipe(lPipePair) != 0)
	{
		mStdOut = "Unable to read stdout.";
		return;
	}
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	dup2(lPipePair[1], STDOUT_FILENO);
	dup2(lPipePair[1], STDERR_FILENO);
	char* lBuffer = (char*)malloc(100);
	for (;;)
	{
		ssize_t lReadCount = read(lPipePair[0], lBuffer, 10);
		if (lReadCount > 0)
		{
			lBuffer[lReadCount] = 0;
			ScopeLock _(&mStdOutLock);
			mStdOut += lBuffer;
#ifdef LEPRA_DEBUG
			//NSLog(@"%s", lBuffer);
#endif // Debug
		}
	}
	free(lBuffer);
	mStdOut += "\nBroken pipe, unable to read more of stdout!";
}



StaticThread PythonRunner::mPythonWorker("PythonWorker");
StaticThread PythonRunner::mStdOutReader("StdOutReader");
Lock PythonRunner::mStdOutLock;
str PythonRunner::mStdOut;
wstr PythonRunner::mDirectory;
wstr PythonRunner::mFilename;
bool PythonRunner::mKillSimulator = true;
bool PythonRunner::mIsStopping = false;



}

#endif // iOS
