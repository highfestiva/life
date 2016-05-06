
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#if 0
#include <Python.h>
#include "contextmanager.h"



namespace cure {



class PythonContextManager: public ContextManager {
public:
	static void OpenPythonEngine(int argc, char* argv[]);
	static void ClosePythonEngine();

	PythonContextManager(double fixed_frame_rate);
	virtual ~PythonContextManager();

	static PyObject* ImportModule(const lepra::String& module);
	static int RunModule(const lepra::String& module);

	ContextObject* CreateObject(const lepra::String& type);
};



}
#endif // 0
