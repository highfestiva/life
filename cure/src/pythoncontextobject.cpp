
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "pch.h"
//#include "../Include/NetworkManager.h"
#include "../include/pythoncontextmanager.h"
#include "../include/pythoncontextobject.h"
#include "../include/pythoninterface.h"



#if 0
namespace cure {



PythonContextObject::PythonContextObject(GameObjectId id, PyObject* instance, pythoninterface::PythonObjectBase* cpp_instance):
	ContextObject(id),
	instance_(instance),
	cpp_instance_(cpp_instance) {
	Py_XINCREF(instance_);
	if (cpp_instance_) {
		cpp_instance_->SetContextObject(this);
	}
}

PythonContextObject::~PythonContextObject() {
	Clear();
}

PythonContextObject* PythonContextObject::Load(bool cpp_interface, const lepra::String& python_name, PyObject* arguments) {
	PythonContextObject* context_object = 0;
	PyObject* python_instance = LoadNative(python_name, arguments);
	if (python_instance != 0) {
		pythoninterface::PythonObjectWrapper* _cpp_instance = 0;
		if (cpp_interface) {
			_cpp_instance = boost::python::extract<pythoninterface::PythonObjectWrapper*>(python_instance);
		}
		GameObjectId _id = ContextManager::Get()->GetNetworkManager()->GetNewGameObjectId();
		context_object = new PythonContextObject(_id, python_instance, _cpp_instance);
	}
	return (context_object);
}



void PythonContextObject::Clear() {
	if (cpp_instance_) {
		cpp_instance_->Clear();
		cpp_instance_ = 0;	// TODO: verify freeing of memory.
	}
	Py_XDECREF(instance_);
	instance_ = 0;
}

PyObject* PythonContextObject::GetInstance() const {
	return (instance_);
}

pythoninterface::PythonObjectBase* PythonContextObject::GetCppInstance() const {
	return (cpp_instance_);
}



void PythonContextObject::OnTick(double frame_time) {
	if (cpp_instance_) {
		cpp_instance_->OnTick(frame_time);
	}
}

void PythonContextObject::OnAlarm(int alarm_id) {
	cpp_instance_->OnAlarm(alarm_id);
}



PyObject* PythonContextObject::LoadNative(const lepra::String& python_name, PyObject* arguments) {
	lepra::String module_name;
	lepra::AnsiString class_name;
	int index = python_name.Find('.');
	if (index < 0) {
		module_name = python_name;
	} else {
		module_name = python_name.Left(index);
		class_name = python_name.Mid(index+1).ToAnsi();
	}
	PyObject* _instance = 0;
	PyObject* module = ((PythonContextManager*)ContextManager::Get())->ImportModule(module_name);
	if (module) {
		// Here we either 1) settle with loading the module, or 2) create a new instance.
		if (class_name.GetLength() > 0) {
			PyObject* module_dictionary = PyModule_GetDict(module);
			PyObject* clazz = PyDict_GetItemString(module_dictionary, class_name);
			if (clazz) {
				_instance = PyObject_CallObject(clazz, arguments);
				Py_DECREF(module);
				if (!_instance) {
					PyErr_Print();
					PyErr_Clear();
				}
			} else {
				Py_DECREF(module);
				perror((const char*)("bad class name: "+class_name));
			}
		} else {
			// Everything OK, module object is requested and returned.
			_instance = module;
		}
	} else {
		PyErr_Print();
		PyErr_Clear();
	}
	return (_instance);
}

void PythonContextObject::OnForceApplied(const lepra::Vector3DF& force, const lepra::Vector3DF& torque) {
	torque;
	cpp_instance_->OnCollision(boost::python::make_tuple(0, 0, 0), boost::python::make_tuple(force.x, force.y, force.z), 1, *Py_None);
}



}
#endif // 0
