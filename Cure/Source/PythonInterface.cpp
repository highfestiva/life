
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "pch.h"
#if 0
#include <stdexcept>
#include "../../Lepra/Include/PerformanceScope.h"
#include "../Include/PythonContextManager.h"
#include "../Include/PythonContextObject.h"
#include "../Include/PythonInterface.h"
#include "../Include/TerrainManager.h"



BOOST_PYTHON_MODULE(cure)
{
	boost::python::class_<Cure::PythonInterface::TerrainPatch, boost::noncopyable>("TerrainPatch")
		.def("create", &Cure::PythonInterface::TerrainPatch::Create)
		.def("clear", &Cure::PythonInterface::TerrainPatch::Clear)
		.def("setSurfaceCoordinates", &Cure::PythonInterface::TerrainPatch::SetSurfaceCoordinates)
		.def("getResolution", &Cure::PythonInterface::TerrainPatch::GetResolution)
		.staticmethod("getResolution")
	;

	boost::python::class_<Cure::PythonInterface::PythonObjectWrapper, boost::noncopyable>("ContextObject")
		.def("enableTick", &Cure::PythonInterface::PythonObjectWrapper::EnableTick)
		.def("disableTick", &Cure::PythonInterface::PythonObjectWrapper::DisableTick)
		.def("setAlarm", &Cure::PythonInterface::PythonObjectWrapper::SetAlarm)
		.def("setPosition", &Cure::PythonInterface::PythonObjectWrapper::SetPosition)
		.def("getPosition", &Cure::PythonInterface::PythonObjectWrapper::GetPosition)
		.def("setOrientation", &Cure::PythonInterface::PythonObjectWrapper::SetOrientation)
		.def("getOrientation", &Cure::PythonInterface::PythonObjectWrapper::GetOrientation)
		.def("applyForce", &Cure::PythonInterface::PythonObjectWrapper::ApplyForce)
		.def("applyTransformedForce", &Cure::PythonInterface::PythonObjectWrapper::ApplyTransformedForce)
		.def("loadGroup", &Cure::PythonInterface::PythonObjectWrapper::LoadGroup)
		.def("onCreate", boost::python::pure_virtual(&Cure::PythonInterface::PythonObjectWrapper::OnCreate))
		.def("onTick", boost::python::pure_virtual(&Cure::PythonInterface::PythonObjectWrapper::OnTick))
		.def("onAlarm", boost::python::pure_virtual(&Cure::PythonInterface::PythonObjectWrapper::OnAlarm))
		.def("onCollision", boost::python::pure_virtual(&Cure::PythonInterface::PythonObjectWrapper::OnCollision))
	;

	boost::python::def("createPythonObject", &Cure::PythonInterface::CreatePythonObject);
}



namespace Cure
{
namespace PythonInterface
{



TerrainPatch::TerrainPatch():
	mTerrainId(0x7FFFFFFF)
{
}

TerrainPatch::~TerrainPatch()
{
	Clear();
}

void TerrainPatch::Create(int x, int y)
{
	Clear();
	mTerrainId = ContextManager::Get()->GetTerrainManager()->CreatePatch(x, y);
}

void TerrainPatch::Clear()
{
	if (mTerrainId != 0x7FFFFFFF)
	{
		ContextManager::Get()->GetTerrainManager()->DeletePatch(mTerrainId);
		mTerrainId = 0x7FFFFFFF;
	}
}

void TerrainPatch::SetSurfaceCoordinates(const tuple& pSurfaceCoordinates) const
{
	const int lCount = (int)PyObject_Length(pSurfaceCoordinates.ptr());
	if (lCount != GetResolution()*GetResolution()*3)
	{
		throw (std::exception("Cure: setSurfaceCoordinates() fails with wrong number of coordinates"));
	}
	if (mTerrainId == 0x7FFFFFFF)
	{
		throw (std::exception("Cure: setSurfaceCoordinates() must be preceded by create()"));
	}

	float* lCoordinates = new float[lCount];
	for (int x = 0; x < lCount; ++x)
	{
		lCoordinates[x] = (float)boost::python::extract<double>(pSurfaceCoordinates[x]);
	}
	ContextManager::Get()->GetTerrainManager()->SetSurfaceCoordinates(mTerrainId, lCoordinates);
	delete (lCoordinates);
}

int TerrainPatch::GetResolution()
{
	return (TerrainManager::GetResolution());
}



PythonObjectBase::PythonObjectBase():
	mContextObject(0)
{
}

PythonObjectBase::PythonObjectBase(const PythonObjectBase& pObject):
	mContextObject(pObject.mContextObject)
{
}

PythonObjectBase::~PythonObjectBase()
{
	// TODO: add smart pointer references to context object?
	mContextObject = 0;
}

void PythonObjectBase::operator=(const PythonObjectBase& pObject)
{
	assert(false);
	mContextObject = pObject.mContextObject;
}

void PythonObjectBase::SetContextObject(PythonContextObject* pContextObject)
{
	mContextObject = pContextObject;
}

PythonContextObject* PythonObjectBase::GetContextObject() const
{
	return (mContextObject);
}

void PythonObjectBase::Clear()
{
	mContextObject = 0;
}



void PythonObjectBase::EnableTick()
{
	if (mContextObject)
	{
		ContextManager::Get()->EnableTick(mContextObject);
	}
	else
	{
		throw (std::exception("Cure: setAlarm() fails for instance with no context object"));
	}
}

void PythonObjectBase::DisableTick()
{
	if (mContextObject)
	{
		ContextManager::Get()->DisableTick(mContextObject);
	}
	else
	{
		throw (std::exception("Cure: disableTick() fails for instance with no context object"));
	}
}

void PythonObjectBase::SetAlarm(int pAlarmId, double pSeconds)
{
	if (mContextObject)
	{
		ContextManager::Get()->SetAlarm(mContextObject, pAlarmId, pSeconds);
	}
	else
	{
		throw (std::exception("Cure: setAlarm() fails on instance with no context object"));
	}
}

void PythonObjectBase::SetPosition(double pX, double pY, double pZ)
{
	mContextObject->SetPosition(Lepra::Vector3DD(pX, pY, pZ));
}

tuple PythonObjectBase::GetPosition() const
{
	Lepra::Vector3DD lPosition = mContextObject->GetPosition();
	return (boost::python::make_tuple(lPosition.x, lPosition.y, lPosition.z));
}

void PythonObjectBase::SetOrientation(double pTheta, double pPhi, double pGimbal)
{
	mContextObject->SetOrientation(Lepra::Vector3DD(pTheta, pPhi, pGimbal));
}

tuple PythonObjectBase::GetOrientation() const
{
	Lepra::Vector3DD lOrientation = mContextObject->GetPosition();
	return (boost::python::make_tuple(lOrientation.x, lOrientation.y, lOrientation.z));
}

void PythonObjectBase::ApplyForce(double pForce, double pTheta, double pPhi, double pXOffset, double pYOffset, double pZOffset)
{
	mContextObject->ApplyForce(Lepra::Vector3DD(pForce, pTheta, pPhi), Lepra::Vector3DD(pXOffset, pYOffset, pZOffset));
}

void PythonObjectBase::ApplyTransformedForce(double pForce, double pTheta, double pPhi, double pXOffset, double pYOffset, double pZOffset)
{
	mContextObject->ApplyTransformedForce(Lepra::Vector3DD(pForce, pTheta, pPhi), Lepra::Vector3DD(pXOffset, pYOffset, pZOffset));
}

void PythonObjectBase::LoadGroup(double pScale, const char* pModel)
{
	Lepra::AnsiString lModelName(pModel);
	mContextObject->LoadGroup(pScale, lModelName.ToCurrentCode());

}



PythonObjectWrapper::PythonObjectWrapper()
{
}

PythonObjectWrapper::PythonObjectWrapper(const PythonObjectBase& pObject):
	PythonObjectBase(pObject)
{
}

PythonObjectWrapper::PythonObjectWrapper(const PythonObjectWrapper& pObject):
	PythonObjectBase(pObject)
{
}

PythonObjectWrapper::~PythonObjectWrapper()
{
}

void PythonObjectWrapper::operator=(const PythonObjectWrapper& pObject)
{
	(*(PythonObjectBase*)this) = (const PythonObjectBase&)pObject;
}

PythonObjectWrapper::operator const PythonObjectBase&()
{
	return (*this);
}


void PythonObjectWrapper::OnCreate() const
{
	boost::python::override lOverrideMethod = get_override("onCreate");
	if (lOverrideMethod)
	{
		lOverrideMethod();
	}
}

void PythonObjectWrapper::OnTick(double pFrameTime) const
{
	boost::python::override lOverrideMethod = get_override("onTick");
	if (lOverrideMethod)
	{
		lOverrideMethod(pFrameTime);
	}
	if (PyErr_Occurred())
	{
		PyErr_Print();
		PyErr_Clear();
	}
}

void PythonObjectWrapper::OnAlarm(int pAlarmId) const
{
	boost::python::override lOverrideMethod = get_override("onAlarm");
	if (lOverrideMethod)
	{
		lOverrideMethod(pAlarmId);
	}
	if (PyErr_Occurred())
	{
		PyErr_Print();
		PyErr_Clear();
	}
}

void PythonObjectWrapper::OnCollision(const tuple& pCoordinate, const tuple& pForceDirection, double pArea, PyObject& pPythonObject) const
{
	LEPRA_INFO_PERFORMANCE_SCOPE("Py:onCollision");

	pPythonObject;
	boost::python::override lOverrideMethod = get_override("onCollision");
	if (lOverrideMethod)
	{
		try
		{
			lOverrideMethod(pCoordinate, pForceDirection, pArea);
		}
		catch (...)
		{
		}
	}
	if (PyErr_Occurred())
	{
		PyErr_Print();
		PyErr_Clear();
	}
}


PyObject* CreatePythonObject(const char* pPythonName, const tuple& pArguments, bool pReplicated)
{
	LEPRA_INFO_PERFORMANCE_SCOPE("CreatePythonObject");

	Lepra::String lPythonName(Lepra::AnsiString(pPythonName).ToCurrentCode());
	PythonContextObject* lContextObject = PythonContextObject::Load(true, lPythonName, pArguments.ptr());
	if (lContextObject)
	{
		ContextManager::Get()->AddObject(lContextObject, pReplicated? NETWORK_OBJECT_REMOTE_CONTROLLED : NETWORK_OBJECT_LOCAL_ONLY);

		LEPRA_INFO_PERFORMANCE_SCOPE("onCreate");

		lContextObject->GetCppInstance()->OnCreate();
	}
	else
	{
		throw (std::exception("Cure: could not create instance from "+lPythonName.ToAnsi()));
	}
	return (lContextObject->GetInstance());
}



}
}
#endif // 0
