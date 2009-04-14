
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#ifndef CUREPYTHONCONTEXTMANAGER_H
#define CUREPYTHONCONTEXTMANAGER_H



#if 0
#include <Python.h>
#include "ContextManager.h"



namespace Cure
{



class PythonContextManager: public ContextManager
{
public:
	static void OpenPythonEngine(int argc, char* argv[]);
	static void ClosePythonEngine();

	PythonContextManager(double pFixedFrameRate);
	virtual ~PythonContextManager();

	static PyObject* ImportModule(const Lepra::String& pModule);
	static int RunModule(const Lepra::String& pModule);

	ContextObject* CreateObject(const Lepra::String& pType);
};



}
#endif // 0


#endif // !CUREPYTHONCONTEXTMANAGER_H
