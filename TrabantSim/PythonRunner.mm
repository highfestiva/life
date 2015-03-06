
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#include "PythonRunner.h"
#import "AnimatedApp.h"



extern "C" void Py_SetProgramName(wchar_t* pn);
extern "C" void Py_Initialize(void);
extern "C" void Py_Finalize(void);
extern "C" int Py_IsInitialized(void);
extern "C" void PySys_SetArgv(int argc, wchar_t** argv);
extern "C" int PyRun_SimpleFileEx(FILE* f, const char* p, int c);



namespace TrabantSim
{



void PythonRunner::Run(const wchar_t* pDirectory, const wchar_t* pFilename)
{
	if (mPythonWorker.IsRunning())
	{
		mPythonWorker.Kill();
	}
	mDirectory = pDirectory;
	mFilename = pFilename;
	mPythonWorker.Start(&PythonRunner::WorkerEntry, 0);
}



void PythonRunner::WorkerEntry(void*)
{
	if (Py_IsInitialized())
	{
		Py_Finalize();
	}
	astr lFullPathname = astrutil::Encode(mDirectory+mFilename);
	astr lFilename = astrutil::Encode(mFilename);
	FILE* fp = fopen(lFullPathname.c_str(), "r");
	if (!fp)
	{
		printf("Error: could not open file %s!", lFilename.c_str());
		return;
	}
	Py_SetProgramName((wchar_t*)mDirectory.c_str());
	Py_Initialize();
	wchar_t* wargv[3] = { (wchar_t*)mFilename.c_str(), (wchar_t*)L"addr=localhost", (wchar_t*)L"osname=ios" };
	PySys_SetArgv(3, wargv);
	PyRun_SimpleFileEx(fp, lFilename.c_str(), 1);
	Py_Finalize();

	// No use showing simulator still frame?
	TrabantSim::mApp->FoldSimulator();
}



StaticThread PythonRunner::mPythonWorker("PythonWorker");
wstr PythonRunner::mDirectory;
wstr PythonRunner::mFilename;



}
