
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#if 0
#pragma warning(push, 3)	// MSVC: Move to warning level 3.
#pragma warning(disable: 4244)	// MSVC: Return: conversion from <x> to <y>, possible loss of data.
#pragma warning(disable: 4267)	// MSVC: Argument: conversion from <x> to <y>, possible loss of data.
#pragma warning(disable: 4541)	// MSVC: <runtime> used on polymorphic type 'Cure::PythonContextManager' with /GR-; unpredictable behavior may result
#include <boost/python.hpp>
#pragma warning(pop)



namespace cure {
namespace pythoninterface {



typedef boost::python::tuple tuple;



class TerrainPatch {
public:
	TerrainPatch();
	virtual ~TerrainPatch();
	void Create(int x, int y);
	void Clear();
	void SetSurfaceCoordinates(const tuple& surface_coordinates) const;
	static int GetResolution();

private:
	int terrain_id_;
};

class PythonObjectBase {
public:
	PythonObjectBase();
	PythonObjectBase(const PythonObjectBase& object);
	virtual ~PythonObjectBase();
	void operator=(const PythonObjectBase& object);
	void SetContextObject(PythonContextObject* context_object);
	PythonContextObject* GetContextObject() const;
	void Clear();

	void EnableTick();
	void DisableTick();
	void SetAlarm(int alarm_id, double seconds);
	void SetPosition(double _x, double _y, double z);
	tuple GetPosition() const;
	void SetOrientation(double theta, double phi, double gimbal);
	tuple GetOrientation() const;
	void ApplyForce(double force, double theta, double phi, double x_offset, double y_offset, double z_offset);
	void ApplyTransformedForce(double force, double theta, double phi, double x_offset, double y_offset, double z_offset);
	void LoadGroup(double scale, const char* model);

	virtual void OnCreate() const = 0;
	virtual void OnTick(double frame_time) const = 0;
	virtual void OnAlarm(int alarm_id) const = 0;
	virtual void OnCollision(const tuple& coordinate, const tuple& force_direction, double area, PyObject& python_object) const = 0;

private:
	PythonContextObject* context_object_;
};

class PythonObjectWrapper: public PythonObjectBase, public boost::python::wrapper<PythonObjectBase>
{
public:
	PythonObjectWrapper();
	PythonObjectWrapper(const PythonObjectBase& object);
	PythonObjectWrapper(const PythonObjectWrapper& object);
	~PythonObjectWrapper();
	void operator=(const PythonObjectWrapper& object);
	operator const PythonObjectBase&();

	void OnCreate() const;
	void OnTick(double frame_time) const;
	void OnAlarm(int alarm_id) const;
	void OnCollision(const tuple& coordinate, const tuple& force_direction, double area, PyObject& python_object) const;
};

static PyObject* CreatePythonObject(const char* python_name, const tuple& arguments, bool replicated);


}
}
#endif // 0
