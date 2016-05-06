
// Author: Jonas Bystrˆm
// Copyright (c) 2002-2012, Pixel Doctrine



#pragma once

#include "trabantsim.h"
#include "../lepra/include/thread.h"



namespace TrabantSim {



class PythonRunner {
public:
	static void Run(const wchar_t* directory, const wchar_t* filename);
	static bool IsRunning();
	static void Break();
	static str GetStdOut();
	static void ClearStdOut();

private:
	static void WorkerEntry(void*);
	static void StdOutReadEntry(void*);

	static StaticThread python_worker_;
	static StaticThread std_out_reader_;
	static Lock std_out_lock_;
	static str std_out_;
	static wstr directory_;
	static wstr filename_;
	static bool kill_simulator_;
	static bool is_stopping_;
};



}
