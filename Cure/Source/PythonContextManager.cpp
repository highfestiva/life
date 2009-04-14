
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/PythonContextManager.h"



#if 0
extern "C" void initcure();



namespace Cure
{



void PythonContextManager::OpenPythonEngine(int /*argc*/, char* argv[])
{
	// The Py..._Set...() must be called before Py_Initialize.
	Py_SetProgramName(argv[0]);
	//PySys_SetArgv(argc, argv);
	// Py_...Ex() can go without initializing signal handlers within the Python interpreter.
	Py_InitializeEx(0);

	initcure();
}

void PythonContextManager::ClosePythonEngine()
{
	Py_Finalize();
}



PythonContextManager::PythonContextManager(double pFixedFrameRate):
	ContextManager(pFixedFrameRate)
{
	char* lArgv[] = {"", 0};
	Cure::PythonContextManager::OpenPythonEngine(0, lArgv);
}

PythonContextManager::~PythonContextManager()
{
	Clear();	// Drop all references to PyObjects.
	Cure::PythonContextManager::ClosePythonEngine();
}



PyObject* PythonContextManager::ImportModule(const Lepra::String& pModule)
{
	return (PyImport_ImportModule(pModule.ToAnsi()));
}

int PythonContextManager::RunModule(const Lepra::String& pModule)
{
	int lResult = -1;
	Lepra::AnsiString lFilename = pModule.ToAnsi();
	FILE* lFile = fopen(lFilename, "rt");
	if (lFile)
	{
		lResult = PyRun_SimpleFileExFlags(lFile, lFilename, 1, 0);
		if (PyErr_Occurred())
		{
			PyErr_Print();
			PyErr_Clear();
		}
	}
	return (lResult);
}



}
#endif // 0
