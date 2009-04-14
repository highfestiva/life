
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/NetworkManager.h"
#include "../Include/PythonContextManager.h"
#include "../Include/PythonContextObject.h"
#include "../Include/PythonInterface.h"



#if 0
namespace Cure
{



PythonContextObject::PythonContextObject(GameObjectId pId, PyObject* pInstance, PythonInterface::PythonObjectBase* pCppInstance):
	ContextObject(pId),
	mInstance(pInstance),
	mCppInstance(pCppInstance)
{
	Py_XINCREF(mInstance);
	if (mCppInstance)
	{
		mCppInstance->SetContextObject(this);
	}
}

PythonContextObject::~PythonContextObject()
{
	Clear();
}

PythonContextObject* PythonContextObject::Load(bool pCppInterface, const Lepra::String& pPythonName, PyObject* pArguments)
{
	PythonContextObject* lContextObject = 0;
	PyObject* lPythonInstance = LoadNative(pPythonName, pArguments);
	if (lPythonInstance != 0)
	{
		PythonInterface::PythonObjectWrapper* lCppInstance = 0;
		if (pCppInterface)
		{
			lCppInstance = boost::python::extract<PythonInterface::PythonObjectWrapper*>(lPythonInstance);
		}
		GameObjectId lId = ContextManager::Get()->GetNetworkManager()->GetNewGameObjectId();
		lContextObject = new PythonContextObject(lId, lPythonInstance, lCppInstance);
	}
	return (lContextObject);
}



void PythonContextObject::Clear()
{
	if (mCppInstance)
	{
		mCppInstance->Clear();
		mCppInstance = 0;	// TODO: verify freeing of memory.
	}
	Py_XDECREF(mInstance);
	mInstance = 0;
}

PyObject* PythonContextObject::GetInstance() const
{
	return (mInstance);
}

PythonInterface::PythonObjectBase* PythonContextObject::GetCppInstance() const
{
	return (mCppInstance);
}



void PythonContextObject::OnTick(double pFrameTime)
{
	if (mCppInstance)
	{
		mCppInstance->OnTick(pFrameTime);
	}
}

void PythonContextObject::OnAlarm(int pAlarmId)
{
	mCppInstance->OnAlarm(pAlarmId);
}



PyObject* PythonContextObject::LoadNative(const Lepra::String& pPythonName, PyObject* pArguments)
{
	Lepra::String lModuleName;
	Lepra::AnsiString lClassName;
	int lIndex = pPythonName.Find('.');
	if (lIndex < 0)
	{
		lModuleName = pPythonName;
	}
	else
	{
		lModuleName = pPythonName.Left(lIndex);
		lClassName = pPythonName.Mid(lIndex+1).ToAnsi();
	}
	PyObject* lInstance = 0;
	PyObject* lModule = ((PythonContextManager*)ContextManager::Get())->ImportModule(lModuleName);
	if (lModule)
	{
		// Here we either 1) settle with loading the module, or 2) create a new instance.
		if (lClassName.GetLength() > 0)
		{
			PyObject* lModuleDictionary = PyModule_GetDict(lModule);
			PyObject* lClass = PyDict_GetItemString(lModuleDictionary, lClassName);
			if (lClass)
			{
				lInstance = PyObject_CallObject(lClass, pArguments);
				Py_DECREF(lModule);
				if (!lInstance)
				{
					PyErr_Print();
					PyErr_Clear();
				}
			}
			else
			{
				Py_DECREF(lModule);
				perror((const char*)("bad class name: "+lClassName));
			}
		}
		else
		{
			// Everything OK, module object is requested and returned.
			lInstance = lModule;
		}
	}
	else
	{
		PyErr_Print();
		PyErr_Clear();
	}
	return (lInstance);
}

void PythonContextObject::OnForceApplied(const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque)
{
	pTorque;
	mCppInstance->OnCollision(boost::python::make_tuple(0, 0, 0), boost::python::make_tuple(pForce.x, pForce.y, pForce.z), 1, *Py_None);
}



}
#endif // 0
