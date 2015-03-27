
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#include "PythonRunner.h"
#import "AnimatedApp.h"
#include "TrabantSimTicker.h"



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

void PythonRunner::Break()
{
	if (!mIsStopping && mPythonWorker.IsRunning())
	{
		mIsStopping = true;
		mKillSimulator = false;
		PyGILState_STATE state = PyGILState_Ensure();
		Py_AddPendingCall(&quit, NULL);
		PyGILState_Release(state);
		mPythonWorker.GraceJoin(0.7);
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

astr PythonRunner::GetStdOut()
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

	astr lFullPathname = astrutil::Encode(mFilename);
	astrutil::strvec lParts = astrutil::Split(lFullPathname, "/");
	astr lFilename = lParts[lParts.size()-1];
	FILE* fp = fopen(lFullPathname.c_str(), "r");
	if (!fp)
	{
		printf("Error: could not open file %s!\n", lFilename.c_str());
		TrabantSim::mApp->FoldSimulator();
		return;
	}
	if (Py_IsInitialized())
	{
		// Here we try to repair if we had to force-kill the previous thread.
		PyGILState_Ensure();
		Py_Finalize();
	}
	Py_SetProgramName((wchar_t*)mDirectory.c_str());
	Py_Initialize();
	PyEval_InitThreads();
	wchar_t* wargv[3] = { (wchar_t*)mFilename.c_str(), (wchar_t*)L"addr=localhost", (wchar_t*)L"osname=ios" };
	PySys_SetArgv(3, wargv);
	PyRun_SimpleFileEx(fp, lFilename.c_str(), 1);
	Py_Finalize();
	PyEval_ReleaseLock();
	printf("Python thread exits.\n");

	if (mKillSimulator)
	{
		// No use showing simulator still frame?
		mIsStopping = true;
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
	char* lBuffer = (char*)malloc(sizeof(char)*1024);
	for (;;)
	{
		ssize_t lReadCount = read(lPipePair[0], lBuffer, 1023);
		if (lReadCount > 0)
		{
			lBuffer[lReadCount] = 0;
			ScopeLock _(&mStdOutLock);
			mStdOut += lBuffer;
		}
		else if (lReadCount == -1)
		{
			break;
		}
	}
	free(lBuffer);
	mStdOut += "\nBroken pipe, unable to read more of stdout!";
}



StaticThread PythonRunner::mPythonWorker("PythonWorker");
StaticThread PythonRunner::mStdOutReader("StdOutReader");
Lock PythonRunner::mStdOutLock;
astr PythonRunner::mStdOut;
wstr PythonRunner::mDirectory;
wstr PythonRunner::mFilename;
bool PythonRunner::mKillSimulator = true;
bool PythonRunner::mIsStopping = false;



}