
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Thread.h"
#include "../Include/FastLock.h"



namespace Lepra
{



StaticThread gMainThread("MainThread");



bool OwnedLock::IsOwner() const
{
	return (mOwner == Thread::GetCurrentThread());
}

Thread* OwnedLock::GetOwner() const
{
	return (mOwner);
}

int OwnedLock::GetReferenceCount() const
{
	return mAcquireCount;
}

OwnedLock::OwnedLock():
	mOwner(0),
	mAcquireCount(0)
{
}

OwnedLock::~OwnedLock()
{
	deb_assert(mAcquireCount == 0);
}

void OwnedLock::Reference()
{
	if (mAcquireCount == 0)
	{
		deb_assert(!mOwner);
		mOwner = Thread::GetCurrentThread();
	}
	++mAcquireCount;
}

void OwnedLock::Dereference()
{
	deb_assert(IsOwner());
	--mAcquireCount;
	if (mAcquireCount == 0)
	{
		mOwner = 0;
	}
}



Lock::Lock():
	mSystemLock(new FastLock)
{
}

Lock::~Lock()
{
	FastLock* lLock = (FastLock*)mSystemLock;
	mSystemLock = 0;
	delete lLock;
}

void Lock::Acquire()
{
	((FastLock*)mSystemLock)->Acquire();
	Reference();
}

bool Lock::TryAcquire()
{
	bool lAcquired = ((FastLock*)mSystemLock)->TryAcquire();
	if (lAcquired)
	{
		Reference();
	}
	return lAcquired;
}

void Lock::Release()
{
	Dereference();
	((FastLock*)mSystemLock)->Release();
}

void* Lock::GetSystemLock() const
{
	return mSystemLock;
}

void Lock::operator=(const Lock&)
{
	deb_assert(false);
}



ScopeLock::ScopeLock(Lock* pLock):
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



Condition::Condition(Lock* pExternalLock):
	mExternalLock(pExternalLock),
	mSystemCondition(new FastCondition(pExternalLock? (FastLock*)pExternalLock->GetSystemLock() : 0))
{
}

Condition::~Condition()
{
	FastCondition* lCondition = (FastCondition*)mSystemCondition;
	mSystemCondition = 0;
	delete lCondition;
}

void Condition::Wait()
{
	mExternalLock->Dereference();
	((FastCondition*)mSystemCondition)->Wait();
	mExternalLock->Reference();
}

bool Condition::Wait(float64 pMaxWaitTime)
{
	mExternalLock->Dereference();
	const bool lSignalled = ((FastCondition*)mSystemCondition)->Wait(pMaxWaitTime);
	mExternalLock->Reference();
	return lSignalled;
}

void Condition::Signal()
{
	((FastCondition*)mSystemCondition)->Signal();
}

void Condition::SignalAll()
{
	((FastCondition*)mSystemCondition)->SignalAll();
}


Semaphore::Semaphore():
	mSystemSemaphore(new FastSemaphore)
{
}

Semaphore::Semaphore(unsigned pMaxCount):
	mSystemSemaphore(new FastSemaphore(pMaxCount))
{
}

Semaphore::~Semaphore()
{
	FastSemaphore* lSemaphore = (FastSemaphore*)mSystemSemaphore;
	mSystemSemaphore = 0;
	delete lSemaphore;
}

void Semaphore::Wait()
{
	((FastSemaphore*)mSystemSemaphore)->Wait();
}

bool Semaphore::Wait(float64 pMaxWaitTime)
{
	return ((FastSemaphore*)mSystemSemaphore)->Wait(pMaxWaitTime);
}

void Semaphore::Signal()
{
	((FastSemaphore*)mSystemSemaphore)->Signal();
}


RwLock::RwLock(const astr& pRwLockName):
	mName(pRwLockName),
	mSystemRwLock(new FastRwLock)
{
}

RwLock::~RwLock()
{
	FastRwLock* lLock = (FastRwLock*)mSystemRwLock;
	mSystemRwLock = 0;
	delete lLock;
}

void RwLock::AcquireRead()
{
	((FastRwLock*)mSystemRwLock)->AcquireRead();
}

void RwLock::AcquireWrite()
{
	((FastRwLock*)mSystemRwLock)->AcquireWrite();
}

void RwLock::Release()
{
	((FastRwLock*)mSystemRwLock)->Release();
}

astr RwLock::GetName()
{
	return mName;
}



Thread::Thread(const astr& pThreadName):
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
		if (!Join(20.0))
		{
			Kill();
		}
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
		lSelf = new StaticThread("OnTheFly");
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
	log_volatile(mLog.Debug(_T("Thread ") + strutil::Encode(mThreadName) + _T(" is ") + (pRunning? _T("starting") : _T("stopping"))));
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

const astr& Thread::GetThreadName() const
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
	YieldCpu();
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

loginstance(GENERAL, Thread);



StaticThread::StaticThread(const astr& pThreadName):
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
