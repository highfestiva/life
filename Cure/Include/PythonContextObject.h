
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#if 0
#include "../../lepra/include/string.h"
#include "contextobject.h"



namespace cure {



namespace pythoninterface {
class PythonObjectBase;
}



class PythonContextObject: public ContextObject {
public:
	PythonContextObject(GameObjectId id, PyObject* instance, pythoninterface::PythonObjectBase* cpp_instance);
	virtual ~PythonContextObject();
	static PythonContextObject* Load(bool cpp_interface, const lepra::String& python_name, PyObject* arguments = 0);

	void Clear();
	PyObject* GetInstance() const;
	pythoninterface::PythonObjectBase* GetCppInstance() const;

	void OnTick(double frame_time);
	void OnAlarm(int alarm_id);
	void OnForceApplied(const lepra::Vector3DF& force, const lepra::Vector3DF& torque);

private:
	static PyObject* LoadNative(const lepra::String& python_name, PyObject* arguments = 0);

	PyObject* instance_;
	pythoninterface::PythonObjectBase* cpp_instance_;
};



}
#endif // 0
