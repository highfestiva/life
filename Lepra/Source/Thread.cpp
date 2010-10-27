
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Thread.h"
#include <assert.h>
#include <stdio.h>
#include "../Include/LepraTypes.h"
#include "../Include/BusLock.h"
#include "../Include/Timer.h"
#include "../Include/Log.h"



namespace Lepra
{



StaticThread gMainThread(_T("MainThread"));



bool OwnedLock::IsOwner() const
{
	return (mOwner == Thread::GetCurrentThread());
}

Thread* OwnedLock::GetOwner() const
{
	return (mOwner);
}

OwnedLock::OwnedLock():
	mOwner(0),
	mAcquireCount(0)
{
}

OwnedLock::~OwnedLock()
{
	assert(mAcquireCount == 0);
}

void OwnedLock::Reference()
{
	if (mAcquireCount == 0)
	{
		assert(!mOwner);
		mOwner = Thread::GetCurrentThread();
	}
	++mAcquireCount;
}

void OwnedLock::Dereference()
{
	assert(IsOwner());
	--mAcquireCount;
	if (mAcquireCount == 0)
	{
		mOwner = 0;
	}
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




RWLockBC::RWLockBC(const str& pRWLockName) :
	mName(pRWLockName)
{
}

RWLockBC::~RWLockBC()
{
}

str RWLockBC::GetName()
{
	return mName;
}



Thread::Thread(const str& pThreadName) :
	mThreadName(pThreadName),
	mRunning(false),
	mStopRequested(false),
	mSelfDestruct(false),
	mThreadHandle(0),
	mThreadId(0)
{
}

Thread::~Thread()
{
	if (!GetSelfDestruct())
	{
		Join();
	}
}

void Thread::InitializeMainThread()
{
	InitializeThread(&gMainThread);
}

bool Thread::QueryInitializeThread()
{
	Thread* lSelf = GetCurrentThread();
	if (!lSelf)
	{
		lSelf = new StaticThread(_T("OnTheFly"));
		lSelf->RequestSelfDestruct();
		InitializeThread(lSelf);
		return true;
	}
	return false;
}

bool Thread::IsRunning() const
{
	return (mRunning);
}

void Thread::SetRunning(bool pRunning)
{
	log_volatile(mLog.Debugf(_T("Thread %s is %s."), mThreadName.c_str(), pRunning? _T("starting") : _T("stopping")));
	mRunning = pRunning;
}

void Thread::SetThreadId(size_t pThreadId)
{
	mThreadId = pThreadId;
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

const str& Thread::GetThreadName() const
{
	return (mThreadName);
}

size_t Thread::GetThreadId() const
{
	return (mThreadId);
}

size_t Thread::GetThreadHandle() const
{
	return (mThreadHandle);
}

void Thread::Sleep(float64 pTime)
{
	if (pTime > 0)
	{
		Sleep((unsigned int)(pTime * 1000.0));
	}
}

bool Thread::Join(float64 pTimeOut)
{
	SetStopRequest(true);
	return GraceJoin(pTimeOut);
}

void Thread::RunThread()
{
	SetThreadId(GetCurrentThreadId());
	SetRunning(true);
	mSemaphore.Signal();
	Run();
	SetRunning(false);
	PostRun();
}

void RunThread(Thread* pThread)
{
	pThread->RunThread();
	if (pThread->GetSelfDestruct())
	{
		delete (pThread);
	}
}

LOG_CLASS_DEFINE(GENERAL, Thread);



StaticThread::StaticThread(const str& pThreadName):
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



}
