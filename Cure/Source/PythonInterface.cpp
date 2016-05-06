
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "pch.h"
#if 0
#include <stdexcept>
#include "../../lepra/include/PerformanceScope.h"
#include "../include/pythoncontextmanager.h"
#include "../include/pythoncontextobject.h"
#include "../include/pythoninterface.h"
#include "../include/terrainmanager.h"



BOOST_PYTHON_MODULE(cure) {
	boost::python::class_<cure::pythoninterface::TerrainPatch, boost::noncopyable>("TerrainPatch")
		.def("create", &cure::pythoninterface::TerrainPatch::Create)
		.def("clear", &cure::pythoninterface::TerrainPatch::Clear)
		.def("setSurfaceCoordinates", &cure::pythoninterface::TerrainPatch::SetSurfaceCoordinates)
		.def("getResolution", &cure::pythoninterface::TerrainPatch::GetResolution)
		.staticmethod("getResolution")
	;

	boost::python::class_<cure::pythoninterface::PythonObjectWrapper, boost::noncopyable>("ContextObject")
		.def("enableTick", &cure::pythoninterface::PythonObjectWrapper::EnableTick)
		.def("disableTick", &cure::pythoninterface::PythonObjectWrapper::DisableTick)
		.def("setAlarm", &cure::pythoninterface::PythonObjectWrapper::SetAlarm)
		.def("setPosition", &cure::pythoninterface::PythonObjectWrapper::SetPosition)
		.def("getPosition", &cure::pythoninterface::PythonObjectWrapper::GetPosition)
		.def("setOrientation", &cure::pythoninterface::PythonObjectWrapper::SetOrientation)
		.def("getOrientation", &cure::pythoninterface::PythonObjectWrapper::GetOrientation)
		.def("applyForce", &cure::pythoninterface::PythonObjectWrapper::ApplyForce)
		.def("applyTransformedForce", &cure::pythoninterface::PythonObjectWrapper::ApplyTransformedForce)
		.def("loadGroup", &cure::pythoninterface::PythonObjectWrapper::LoadGroup)
		.def("onCreate", boost::python::pure_virtual(&cure::pythoninterface::PythonObjectWrapper::OnCreate))
		.def("onTick", boost::python::pure_virtual(&cure::pythoninterface::PythonObjectWrapper::OnTick))
		.def("onAlarm", boost::python::pure_virtual(&cure::pythoninterface::PythonObjectWrapper::OnAlarm))
		.def("onCollision", boost::python::pure_virtual(&cure::pythoninterface::PythonObjectWrapper::OnCollision))
	;

	boost::python::def("createPythonObject", &cure::pythoninterface::CreatePythonObject);
}



namespace cure {
namespace pythoninterface {



TerrainPatch::TerrainPatch():
	terrain_id_(0x7FFFFFFF) {
}

TerrainPatch::~TerrainPatch() {
	Clear();
}

void TerrainPatch::Create(int x, int y) {
	Clear();
	terrain_id_ = ContextManager::Get()->GetTerrainManager()->CreatePatch(x, y);
}

void TerrainPatch::Clear() {
	if (terrain_id_ != 0x7FFFFFFF) {
		ContextManager::Get()->GetTerrainManager()->DeletePatch(terrain_id_);
		terrain_id_ = 0x7FFFFFFF;
	}
}

void TerrainPatch::SetSurfaceCoordinates(const tuple& surface_coordinates) const {
	const int count = (int)PyObject_Length(surface_coordinates.ptr());
	if (count != GetResolution()*GetResolution()*3) {
		throw (std::exception("Cure: setSurfaceCoordinates() fails with wrong number of coordinates"));
	}
	if (terrain_id_ == 0x7FFFFFFF) {
		throw (std::exception("Cure: setSurfaceCoordinates() must be preceded by create()"));
	}

	float* coordinates = new float[count];
	for (int x = 0; x < count; ++x) {
		coordinates[x] = (float)boost::python::extract<double>(surface_coordinates[x]);
	}
	ContextManager::Get()->GetTerrainManager()->SetSurfaceCoordinates(terrain_id_, coordinates);
	delete (coordinates);
}

int TerrainPatch::GetResolution() {
	return (TerrainManager::GetResolution());
}



PythonObjectBase::PythonObjectBase():
	context_object_(0) {
}

PythonObjectBase::PythonObjectBase(const PythonObjectBase& object):
	context_object_(object.context_object_) {
}

PythonObjectBase::~PythonObjectBase() {
	// TODO: add smart pointer references to context object?
	context_object_ = 0;
}

void PythonObjectBase::operator=(const PythonObjectBase& object) {
	assert(false);
	context_object_ = object.context_object_;
}

void PythonObjectBase::SetContextObject(PythonContextObject* context_object) {
	context_object_ = context_object;
}

PythonContextObject* PythonObjectBase::GetContextObject() const {
	return (context_object_);
}

void PythonObjectBase::Clear() {
	context_object_ = 0;
}



void PythonObjectBase::EnableTick() {
	if (context_object_) {
		ContextManager::Get()->EnableTick(context_object_);
	} else {
		throw (std::exception("Cure: setAlarm() fails for instance with no context object"));
	}
}

void PythonObjectBase::DisableTick() {
	if (context_object_) {
		ContextManager::Get()->DisableTick(context_object_);
	} else {
		throw (std::exception("Cure: disableTick() fails for instance with no context object"));
	}
}

void PythonObjectBase::SetAlarm(int alarm_id, double seconds) {
	if (context_object_) {
		ContextManager::Get()->SetAlarm(context_object_, alarm_id, seconds);
	} else {
		throw (std::exception("Cure: setAlarm() fails on instance with no context object"));
	}
}

void PythonObjectBase::SetPosition(double _x, double _y, double _z) {
	context_object_->SetPosition(lepra::Vector3DD(_x, _y, _z));
}

tuple PythonObjectBase::GetPosition() const {
	lepra::Vector3DD position = context_object_->GetPosition();
	return (boost::python::make_tuple(position.x, position.y, position.z));
}

void PythonObjectBase::SetOrientation(double theta, double phi, double gimbal) {
	context_object_->SetOrientation(lepra::Vector3DD(theta, phi, gimbal));
}

tuple PythonObjectBase::GetOrientation() const {
	lepra::Vector3DD orientation = context_object_->GetPosition();
	return (boost::python::make_tuple(orientation.x, orientation.y, orientation.z));
}

void PythonObjectBase::ApplyForce(double force, double theta, double phi, double x_offset, double y_offset, double z_offset) {
	context_object_->ApplyForce(lepra::Vector3DD(force, theta, phi), lepra::Vector3DD(x_offset, y_offset, z_offset));
}

void PythonObjectBase::ApplyTransformedForce(double force, double theta, double phi, double x_offset, double y_offset, double z_offset) {
	context_object_->ApplyTransformedForce(lepra::Vector3DD(force, theta, phi), lepra::Vector3DD(x_offset, y_offset, z_offset));
}

void PythonObjectBase::LoadGroup(double scale, const char* model) {
	lepra::AnsiString model_name(model);
	context_object_->LoadGroup(scale, model_name.ToCurrentCode());

}



PythonObjectWrapper::PythonObjectWrapper() {
}

PythonObjectWrapper::PythonObjectWrapper(const PythonObjectBase& object):
	PythonObjectBase(object) {
}

PythonObjectWrapper::PythonObjectWrapper(const PythonObjectWrapper& object):
	PythonObjectBase(object) {
}

PythonObjectWrapper::~PythonObjectWrapper() {
}

void PythonObjectWrapper::operator=(const PythonObjectWrapper& object) {
	(*(PythonObjectBase*)this) = (const PythonObjectBase&)object;
}

PythonObjectWrapper::operator const PythonObjectBase&() {
	return (*this);
}


void PythonObjectWrapper::OnCreate() const {
	boost::python::override override_method = get_override("onCreate");
	if (override_method) {
		override_method();
	}
}

void PythonObjectWrapper::OnTick(double frame_time) const {
	boost::python::override override_method = get_override("onTick");
	if (override_method) {
		override_method(frame_time);
	}
	if (PyErr_Occurred()) {
		PyErr_Print();
		PyErr_Clear();
	}
}

void PythonObjectWrapper::OnAlarm(int alarm_id) const {
	boost::python::override override_method = get_override("onAlarm");
	if (override_method) {
		override_method(alarm_id);
	}
	if (PyErr_Occurred()) {
		PyErr_Print();
		PyErr_Clear();
	}
}

void PythonObjectWrapper::OnCollision(const tuple& coordinate, const tuple& force_direction, double area, PyObject& python_object) const {
	LEPRA_INFO_PERFORMANCE_SCOPE("Py:onCollision");

	python_object;
	boost::python::override override_method = get_override("onCollision");
	if (override_method) {
		try {
			override_method(coordinate, force_direction, area);
		} catch (...) {
		}
	}
	if (PyErr_Occurred()) {
		PyErr_Print();
		PyErr_Clear();
	}
}


PyObject* CreatePythonObject(const char* python_name, const tuple& arguments, bool replicated) {
	LEPRA_INFO_PERFORMANCE_SCOPE("CreatePythonObject");

	lepra::String _python_name(lepra::AnsiString(python_name).ToCurrentCode());
	PythonContextObject* _context_object = PythonContextObject::Load(true, _python_name, arguments.ptr());
	if (_context_object) {
		ContextManager::Get()->AddObject(_context_object, replicated? kNetworkObjectRemoteControlled : kNetworkObjectLocalOnly);

		LEPRA_INFO_PERFORMANCE_SCOPE("onCreate");

		_context_object->GetCppInstance()->OnCreate();
	} else {
		throw (std::exception("Cure: could not create instance from "+_python_name.ToAnsi()));
	}
	return (_context_object->GetInstance());
}



}
}
#endif // 0
