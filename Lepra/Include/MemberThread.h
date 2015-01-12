
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// The MemberThread template does not require inheritance to start a thread in a member method:
//	MemberThread<ThisClass> x;
//	x.Start(this, &ThisClass::MyThreadedMethod);



#pragma once

#include "LepraAssert.h"
#include "Thread.h"



namespace Lepra
{



template<class _Base, class _Object = void*> class MemberThread: public Thread
{
public:
	MemberThread(const astr& pThreadName);
	virtual ~MemberThread();

	bool Start(_Base* pObject, void (_Base::*pThreadEntry)());

	_Object GetObjectData();
	void SetObjectData(_Object pData);

private:
	bool Start();	// Obfuscates Thread::Start(void).
	void Run();

	_Base* mObject;
	void (_Base::*mThreadEntry)();
	_Object mObjectData;
};



template<class _Base, class _Object> MemberThread<_Base, _Object>::MemberThread(const astr& pThreadName):
	Thread(pThreadName),
	mObject(0),
	mThreadEntry(0)
{
}

template<class _Base, class _Object> MemberThread<_Base, _Object>::~MemberThread()
{
	// Parent class Join()s.
}


template<class _Base, class _Object> bool MemberThread<_Base, _Object>::Start(_Base* pObject, void (_Base::*pThreadEntry)())
{
	if (IsRunning())
	{
		Join(10.0);
		Kill();
	}
	deb_assert(pObject);
	deb_assert(pThreadEntry);
	mObject = pObject;
	mThreadEntry = pThreadEntry;
	return (Thread::Start());	// Will call Run() when the new thread starts.
}

template<class _Base, class _Object> _Object MemberThread<_Base, _Object>::GetObjectData()
{
	return mObjectData;
}

template<class _Base, class _Object> void MemberThread<_Base, _Object>::SetObjectData(_Object pData)
{
	mObjectData = pData;
}

template<class _Base, class _Object> void MemberThread<_Base, _Object>::Run()
{
	deb_assert(mObject);
	deb_assert(mThreadEntry);
	(mObject->*mThreadEntry)();
}



}
