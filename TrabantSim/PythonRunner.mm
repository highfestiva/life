
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#include "../lepra/include/lepratarget.h"
#ifdef LEPRA_IOS

#include "pythonrunner.h"
#import "animatedapp.h"
#include "trabantsimticker.h"



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



namespace TrabantSim {



int quit(void*) {
	PyErr_SetString(PyExc_KeyboardInterrupt, "...");
	PyErr_SetInterrupt();
	return -1;
}



void PythonRunner::Run(const wchar_t* directory, const wchar_t* filename) {
	Break();
	directory_ = directory;
	filename_ = filename;
	python_worker_.Start(&PythonRunner::WorkerEntry, 0);
	std_out_reader_.Start(&PythonRunner::StdOutReadEntry, 0);
}

bool PythonRunner::IsRunning() {
	return python_worker_.IsRunning();
}

void PythonRunner::Break() {
	if (!is_stopping_ && python_worker_.IsRunning()) {
		is_stopping_ = true;
		kill_simulator_ = false;
		for (int x = 0; x < 3 && !Py_IsInitialized(); ++x) {
			Thread::Sleep(0.1f);
		}
		if (Py_IsInitialized()) {
			PyGILState_STATE state = PyGILState_Ensure();
			Py_AddPendingCall(&quit, NULL);
			PyGILState_Release(state);
			python_worker_.GraceJoin(0.7);
		}
		if (python_worker_.IsRunning()) {
			printf("Warning: killing python thread!\n");
			pthread_cancel((pthread_t)python_worker_.GetThreadHandle());
			python_worker_.Kill();
		}
		Py_Jb_ClearPendingCalls();
		kill_simulator_ = true;
		is_stopping_ = false;
	}
}

str PythonRunner::GetStdOut() {
	ScopeLock _(&std_out_lock_);
	return std_out_;
}

void PythonRunner::ClearStdOut() {
	if (std_out_reader_.IsRunning()) {
		ScopeLock _(&std_out_lock_);
		std_out_.clear();
	}
}



void PythonRunner::WorkerEntry(void*) {
	int _;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &_);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &_);

	HiResTimer timer(false);
	str full_pathname = filename_;
	strutil::strvec parts = strutil::Split(full_pathname, "/");
	str _filename = parts[parts.size()-1];
	FILE* fp = fopen(full_pathname.c_str(), "r");
	if (!fp) {
		printf("Error: could not open file %s!\n", _filename.c_str());
		Thread::Sleep(MINIMUM_PYTHON_RUN_TIME-timer.QueryTimeDiff());
		TrabantSim::app_->FoldSimulator();
		TrabantSim::app_->Suspend(false);
		return;
	}
	if (Py_IsInitialized()) {
		// Here we try to repair if we had to force-kill the previous thread.
		PyGILState_Ensure();
		Py_Finalize();
	}
	Py_SetProgramName((wchar_t*)directory_.c_str());
	Py_OptimizeFlag = 1;
	//Py_VerboseFlag = 1;
	Py_Initialize();
	PyEval_InitThreads();
	wchar_t* wargv[3] = { (wchar_t*)filename_.c_str(), (wchar_t*)L"addr=localhost", (wchar_t*)L"osname=ios" };
	PySys_SetArgv(3, wargv);
	PyRun_SimpleFileEx(fp, _filename.c_str(), 1);
	Py_Finalize();
	PyEval_ReleaseLock();

	if (kill_simulator_) {
		// No use showing simulator still frame?
	 	is_stopping_ = true;
		Thread::Sleep(MINIMUM_PYTHON_RUN_TIME-timer.QueryTimeDiff());
		TrabantSim::app_->Suspend(false);
		TrabantSim::app_->FoldSimulator();
		is_stopping_ = false;
	}
}

void PythonRunner::StdOutReadEntry(void*) {
	int _;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &_);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &_);

	static int pipe_pair[2];
	if (pipe(pipe_pair) != 0) {
		std_out_ = "Unable to read stdout.";
		return;
	}
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	dup2(pipe_pair[1], STDOUT_FILENO);
	dup2(pipe_pair[1], STDERR_FILENO);
	char* buffer = (char*)malloc(100);
	for (;;) {
		ssize_t read_count = read(pipe_pair[0], buffer, 10);
		if (read_count > 0) {
			buffer[read_count] = 0;
			ScopeLock _(&std_out_lock_);
			std_out_ += buffer;
#ifdef LEPRA_DEBUG
			//NSLog(@"%s", buffer);
#endif // Debug
		}
	}
	free(buffer);
	std_out_ += "\nBroken pipe, unable to read more of stdout!";
}



StaticThread PythonRunner::python_worker_("PythonWorker");
StaticThread PythonRunner::std_out_reader_("StdOutReader");
Lock PythonRunner::std_out_lock_;
str PythonRunner::std_out_;
wstr PythonRunner::directory_;
wstr PythonRunner::filename_;
bool PythonRunner::kill_simulator_ = true;
bool PythonRunner::is_stopping_ = false;



}

#endif // iOS
