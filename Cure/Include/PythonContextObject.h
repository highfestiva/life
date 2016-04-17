
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#if 0
#include "../../Lepra/Include/String.h"
#include "ContextObject.h"



namespace Cure
{



namespace PythonInterface
{
class PythonObjectBase;
}



class PythonContextObject: public ContextObject
{
public:
	PythonContextObject(GameObjectId pId, PyObject* pInstance, PythonInterface::PythonObjectBase* pCppInstance);
	virtual ~PythonContextObject();
	static PythonContextObject* Load(bool pCppInterface, const Lepra::String& pPythonName, PyObject* pArguments = 0);

	void Clear();
	PyObject* GetInstance() const;
	PythonInterface::PythonObjectBase* GetCppInstance() const;

	void OnTick(double pFrameTime);
	void OnAlarm(int pAlarmId);
	void OnForceApplied(const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque);

private:
	static PyObject* LoadNative(const Lepra::String& pPythonName, PyObject* pArguments = 0);

	PyObject* mInstance;
	PythonInterface::PythonObjectBase* mCppInstance;
};



}
#endif // 0
