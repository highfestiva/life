/*
	Class:  MemberThread
	Author: Jonas Byström
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	The MemberThread template does not require inheritance to
	start a thread in a member method:

	MemberThread<ThisClass> x;
	x.Start(this, Thread1Start);
	MemberThread<ThisClass> y;
	y.Start(this, Thread2Start);
*/



#ifndef LEPRA_MEMBERTHREAD_H
#define LEPRA_MEMBERTHREAD_H



#include <assert.h>
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
	Join(10.0);
	Kill();
	assert(pObject);
	assert(pThreadEntry);
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
	assert(mObject);
	assert(mThreadEntry);
	(mObject->*mThreadEntry)();
}



}



#endif // !LEPRA_MEMBERTHREAD_H
