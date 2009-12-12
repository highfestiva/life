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



template<class _Base> class MemberThread: public Thread
{
public:
	MemberThread(const String& pThreadName);
	virtual ~MemberThread();

	bool Start(_Base* pObject, void (_Base::*pThreadEntry)());

private:
	bool Start();	// Obfuscates Thread::Start(void).
	void Run();

	_Base* mObject;
	void (_Base::*mThreadEntry)();
};



template<class _Base> MemberThread<_Base>::MemberThread(const String& pThreadName):
	Thread(pThreadName),
	mObject(0),
	mThreadEntry(0)
{
}

template<class _Base> MemberThread<_Base>::~MemberThread()
{
	// Parent class Join()s.
}


template<class _Base> bool MemberThread<_Base>::Start(_Base* pObject, void (_Base::*pThreadEntry)())
{
	Join(10.0);
	Kill();
	assert(pObject);
	assert(pThreadEntry);
	mObject = pObject;
	mThreadEntry = pThreadEntry;
	return (Thread::Start());	// Will call Run() when the new thread starts.
}

template<class _Base> void MemberThread<_Base>::Run()
{
	assert(mObject);
	assert(mThreadEntry);
	(mObject->*mThreadEntry)();
}



}



#endif // !LEPRA_MEMBERTHREAD_H
