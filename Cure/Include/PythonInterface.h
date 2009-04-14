
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#ifndef CUREPYTHONINTERFACE_H
#define CUREPYTHONINTERFACE_H



#if 0
#pragma warning(push, 3)	// MSVC: Move to warning level 3.
#pragma warning(disable: 4244)	// MSVC: Return: conversion from <x> to <y>, possible loss of data.
#pragma warning(disable: 4267)	// MSVC: Argument: conversion from <x> to <y>, possible loss of data.
#pragma warning(disable: 4541)	// MSVC: <runtime> used on polymorphic type 'Cure::PythonContextManager' with /GR-; unpredictable behavior may result
#include <boost/python.hpp>
#pragma warning(pop)



namespace Cure
{
namespace PythonInterface
{



typedef boost::python::tuple tuple;



class TerrainPatch
{
public:
	TerrainPatch();
	virtual ~TerrainPatch();
	void Create(int x, int y);
	void Clear();
	void SetSurfaceCoordinates(const tuple& pSurfaceCoordinates) const;
	static int GetResolution();

private:
	int mTerrainId;
};

class PythonObjectBase
{
public:
	PythonObjectBase();
	PythonObjectBase(const PythonObjectBase& pObject);
	virtual ~PythonObjectBase();
	void operator=(const PythonObjectBase& pObject);
	void SetContextObject(PythonContextObject* pContextObject);
	PythonContextObject* GetContextObject() const;
	void Clear();

	void EnableTick();
	void DisableTick();
	void SetAlarm(int pAlarmId, double pSeconds);
	void SetPosition(double pX, double pY, double pZ);
	tuple GetPosition() const;
	void SetOrientation(double pTheta, double pPhi, double pGimbal);
	tuple GetOrientation() const;
	void ApplyForce(double pForce, double pTheta, double pPhi, double pXOffset, double pYOffset, double pZOffset);
	void ApplyTransformedForce(double pForce, double pTheta, double pPhi, double pXOffset, double pYOffset, double pZOffset);
	void LoadGroup(double pScale, const char* pModel);

	virtual void OnCreate() const = 0;
	virtual void OnTick(double pFrameTime) const = 0;
	virtual void OnAlarm(int pAlarmId) const = 0;
	virtual void OnCollision(const tuple& pCoordinate, const tuple& pForceDirection, double pArea, PyObject& pPythonObject) const = 0;

private:
	PythonContextObject* mContextObject;
};

class PythonObjectWrapper: public PythonObjectBase, public boost::python::wrapper<PythonObjectBase>
{
public:
	PythonObjectWrapper();
	PythonObjectWrapper(const PythonObjectBase& pObject);
	PythonObjectWrapper(const PythonObjectWrapper& pObject);
	~PythonObjectWrapper();
	void operator=(const PythonObjectWrapper& pObject);
	operator const PythonObjectBase&();

	void OnCreate() const;
	void OnTick(double pFrameTime) const;
	void OnAlarm(int pAlarmId) const;
	void OnCollision(const tuple& pCoordinate, const tuple& pForceDirection, double pArea, PyObject& pPythonObject) const;
};

static PyObject* CreatePythonObject(const char* pPythonName, const tuple& pArguments, bool pReplicated);


}
}
#endif // 0



#endif // !CUREPYTHONINTERFACE_H
