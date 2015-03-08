
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
		mPythonWorker.GraceJoin(1.5);
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



StaticThread PythonRunner::mPythonWorker("PythonWorker");
wstr PythonRunner::mDirectory;
wstr PythonRunner::mFilename;
bool PythonRunner::mKillSimulator = true;
bool PythonRunner::mIsStopping = false;



}
