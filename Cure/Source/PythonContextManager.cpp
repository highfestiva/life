
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "pch.h"
#include "../include/pythoncontextmanager.h"



#if 0
extern "C" void initcure();



namespace cure {



void PythonContextManager::OpenPythonEngine(int /*argc*/, char* argv[]) {
	// The Py..._Set...() must be called before Py_Initialize.
	Py_SetProgramName(argv[0]);
	//PySys_SetArgv(argc, argv);
	// Py_...Ex() can go without initializing signal handlers within the Python interpreter.
	Py_InitializeEx(0);

	initcure();
}

void PythonContextManager::ClosePythonEngine() {
	Py_Finalize();
}



PythonContextManager::PythonContextManager(double fixed_frame_rate):
	ContextManager(fixed_frame_rate) {
	char* __argv[] = {"", 0};
	cure::PythonContextManager::OpenPythonEngine(0, __argv);
}

PythonContextManager::~PythonContextManager() {
	Clear();	// Drop all references to PyObjects.
	cure::PythonContextManager::ClosePythonEngine();
}



PyObject* PythonContextManager::ImportModule(const lepra::String& module) {
	return (PyImport_ImportModule(module.ToAnsi()));
}

int PythonContextManager::RunModule(const lepra::String& module) {
	int result = -1;
	lepra::AnsiString filename = module.ToAnsi();
	FILE* file = fopen(filename, "rt");
	if (file) {
		result = PyRun_SimpleFileExFlags(file, filename, 1, 0);
		if (PyErr_Occurred()) {
			PyErr_Print();
			PyErr_Clear();
		}
	}
	return (result);
}



}
#endif // 0
