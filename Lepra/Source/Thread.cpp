/*
	Class:  Thread
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/LepraTypes.h"
#include "../Include/Thread.h"
#include "../Include/BusLock.h"
#include "../Include/Timer.h"
#include "../Include/Log.h"

#include <stdio.h>

namespace Lepra
{

Thread::Thread(const String& pThreadName) :
	mThreadName(pThreadName),
	mRunning(false),
	mStopRequested(false),
	mSelfDestruct(false),
	mThreadHandle(0),
	mThreadId(0)
{
#ifdef LEPRA_POSIX
	::sem_init(&mJoinSemaphore, 0, 0);
#endif // Posix.
}

Thread::~Thread()
{
	if (!GetSelfDestruct())
	{
		Join();
	}
#ifdef LEPRA_POSIX
	::sem_destroy(&mJoinSemaphore);
#endif // Posix.
}

bool Thread::IsRunning() const
{
	return mRunning;
}

void Thread::SetRunning(bool pRunning)
{
	mRunning = pRunning;
}

bool Thread::GetStopRequest() const
{
	return (mStopRequested);
}

void Thread::SetStopRequest(bool pStopRequest)
{
	mStopRequested = pStopRequest;
}

void Thread::RequestStop()
{
	if (IsRunning() == true)
	{
		SetStopRequest(true);
	}
}

bool Thread::GetSelfDestruct() const
{
	return (mSelfDestruct);
}

void Thread::RequestSelfDestruct()
{
	mSelfDestruct = true;
}

const String& Thread::GetThreadName()
{
	return mThreadName;
}

size_t Thread::GetThreadId()
{
	return mThreadId;
}

size_t Thread::GetThreadHandle()
{
	return mThreadHandle;
}

void Thread::Sleep(float64 pTime)
{
	if (pTime > 0)
	{
		Sleep((unsigned int)(pTime * 1000.0));
	}
}

void RunThread(Thread* pThread)
{
	pThread->SetRunning(true);
	pThread->Run();
	pThread->SetRunning(false);
	if (pThread->GetSelfDestruct())
	{
		delete (pThread);
	}
}

LOG_CLASS_DEFINE(GENERAL, Thread);



StaticThread::StaticThread(const String& pThreadName):
	Thread(pThreadName),
	mThreadEntry(0),
	mData(0)
{
}

StaticThread::~StaticThread()
{
	mThreadEntry = 0;
	mData = 0;
}

bool StaticThread::Start(void (*pThreadEntry)(void*), void* pData)
{
	mThreadEntry = pThreadEntry;
	mData = pData;
	return (Start());
}

bool StaticThread::Start()
{
	// Only implemented to protect the Start() method.
	return (Thread::Start());
}

void StaticThread::Run()
{
	mThreadEntry(mData);
}





LockBC::LockBC()
{
}

LockBC::~LockBC()
{
}


ScopeLock::ScopeLock(LockBC* pLock) :
	mLock(pLock)
{
	mLock->Acquire();
}

ScopeLock::~ScopeLock()
{
	mLock->Release();
	mLock = 0;
}

void ScopeLock::Acquire()
{
	mLock->Acquire();
}

void ScopeLock::Release()
{
	mLock->Release();
}



CompatibleLockBC::CompatibleLockBC()
{
}

CompatibleLockBC::~CompatibleLockBC()
{
}


CompatibleScopeLock::CompatibleScopeLock(CompatibleLockBC* pLock) :
	mLock(pLock)
{
	mLock->Acquire();
}

CompatibleScopeLock::~CompatibleScopeLock()
{
	mLock->Release();
	mLock = 0;
}



ConditionBC::ConditionBC()
{
}

ConditionBC::~ConditionBC()
{
}


CompatibleConditionBC::CompatibleConditionBC(CompatibleLockBC* pExternalLock) :
	mExternalLock(pExternalLock)
{
}

CompatibleConditionBC::~CompatibleConditionBC()
{
}




SemaphoreBC::SemaphoreBC()
{
}

SemaphoreBC::~SemaphoreBC()
{
}

/*CompatibleSemaphoreBC::CompatibleSemaphoreBC()
{
}

CompatibleSemaphoreBC::~CompatibleSemaphoreBC()
{
}*/




RWLockBC::RWLockBC(const String& pRWLockName) :
	mName(pRWLockName)
{
}

RWLockBC::~RWLockBC()
{
}

String RWLockBC::GetName()
{
	return mName;
}

} // End namespace.
