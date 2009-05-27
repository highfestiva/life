
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "../../Include/Posix/PosixThread.h"



namespace Lepra
{



void GetAbsTime(float64 pDeltaTime, timespec& pTimeSpec)
{
	float64 lSeconds = floor(pDeltaTime);
	float64 lNanoSeconds = (pDeltaTime - lSeconds) * 1000000000.0;
	timeval lTimeVal;
	::gettimeofday(&lTimeVal, 0);
	pTimeSpec.tv_sec = (time_t)lTimeVal.tv_sec + (time_t)lSeconds;
	pTimeSpec.tv_nsec = (long int)lNanoSeconds + (long int)(lTimeVal.tv_usec * 1000);
}


	
ThreadPointerStorage ThreadPointerStorage::smTPS;

ThreadPointerStorage::ThreadPointerStorage()
{
     ::pthread_key_create(&smTPS.mKey, NULL);
}

ThreadPointerStorage::~ThreadPointerStorage()
{
     ::pthread_key_delete(smTPS.mKey);
}

void ThreadPointerStorage::SetPointer(Thread* pThread)
{
     ::pthread_setspecific(smTPS.mKey, (void*)pThread);
}

Thread* ThreadPointerStorage::GetPointer()
{
     return (Thread*)::pthread_getspecific(smTPS.mKey);
}



StaticThread gMainThread(_T("MainThread"));



// This is where the thread starts. A global standard C-function.
void* ThreadEntry(void* pThread)
{
	Thread* lThread = (Thread*)pThread;
	ThreadPointerStorage::SetPointer(lThread);
	assert(ThreadPointerStorage::GetPointer() == lThread);
	assert(Thread::GetCurrentThread() == lThread);
	RunThread(lThread);
	return (0);
}

void Thread::InitializeMainThread(const String& pThreadName)
{
	ThreadPointerStorage::SetPointer(&gMainThread);
	assert(ThreadPointerStorage::GetPointer() == &gMainThread);
	assert(Thread::GetCurrentThread() == &gMainThread);
}

size_t Thread::GetCurrentThreadId()
{
#ifndef gettid
#define gettid()	::syscall(SYS_gettid)
#endif
	return (gettid());
}

Thread* Thread::GetCurrentThread()
{
	return (ThreadPointerStorage::GetPointer());
}

void Thread::SetCpuAffinityMask(uint64 /*pAffinityMask*/)
{
	// This is not yet implemented on a per-thread basis in GNU/Linux.
	// I'm also guessing that we won't need it.
}

void Thread::Sleep(unsigned int pMilliSeconds)
{
	::usleep(pMilliSeconds*1000);
}

bool Thread::Start()
{
	bool lOk = true;

	if (!IsRunning())
	{
		SetStopRequest(false);

		if(::pthread_create(&mThreadHandle, 0, ThreadEntry, this) == 0)
		{
			// Try to wait for newly created thread.
			for (int x = 0; !IsRunning() && x < 1000; ++x)
			{
				Thread::YieldCpu();
			}
		}
		else
		{
			SetRunning(false);
			mLog.Errorf((_T("Failed to start thread ")+GetThreadName()+_T("!")).c_str());
			lOk = false;
		}
	}
	return (lOk);
}

bool Thread::Join()
{
	SetStopRequest(true);
	if (GetThreadHandle() != 0)
	{
		assert(GetThreadId() != GetCurrentThreadId());
		Thread::YieldCpu();	// Try to let thread self destruct.
		::pthread_join(mThreadHandle, 0);
		assert(!IsRunning());
		mThreadHandle = 0;
		mThreadId = 0;
	}
	SetStopRequest(false);
	return (true);
}

bool Thread::Join(float64 pTimeOut)
{
	SetStopRequest(true);
	if (GetThreadHandle() != 0)
	{
		assert(GetThreadId() != GetCurrentThreadId());
		timespec lTimeSpec;
		GetAbsTime(pTimeOut, lTimeSpec);
		if (::sem_timedwait(&mJoinSemaphore, &lTimeSpec) != 0)
		{
			// Possible dead lock...
			mLog.Warningf((_T("Failed to timed join thread \"") + GetThreadName() + _T("\"! Deadlock?")).c_str());
			return (false);	// RAII simplifies here.
		}
		Join();
	}
	SetStopRequest(false);
	return (true);
}

void Thread::Kill()
{
	if (GetThreadHandle() != 0)
	{
		assert(GetThreadId() != GetCurrentThreadId());
		mLog.Warningf(_T("Forcing kill of thread %s."), GetThreadName().c_str());
		::pthread_kill(mThreadHandle, 9);	// 9 = SIGKILL.
		Join();
		SetRunning(false);
	}
	SetStopRequest(false);
}

void Thread::YieldCpu()
{
	::sched_yield();
}

void Thread::PostRun()
{
	::sem_post(&mJoinSemaphore);
}





PosixLock::PosixLock()
{
	::pthread_mutexattr_t lMutexAttr;
	::pthread_mutexattr_init(&lMutexAttr);
	::pthread_mutexattr_settype(&lMutexAttr, PTHREAD_MUTEX_RECURSIVE);
	::pthread_mutex_init(&mMutex, &lMutexAttr);
	::pthread_mutexattr_destroy(&lMutexAttr);
}

PosixLock::~PosixLock()
{
	::pthread_mutex_destroy(&mMutex);
}

void PosixLock::Acquire()
{
	::pthread_mutex_lock(&mMutex);
}

bool PosixLock::TryAcquire()
{
	return (::pthread_mutex_trylock(&mMutex) == 0);
}

void PosixLock::Release()
{
	::pthread_mutex_unlock(&mMutex);
}




PosixCondition::PosixCondition(PosixLock* pExternalLock):
	mExternalLock(pExternalLock)
{
	::pthread_cond_init(&mCondition, 0);
	
	if (mExternalLock == 0)
	{
		// There must be a lock associated with the condition,
		// since the behaviour of ::pthread_cond_wait() is undefined
		// otherwise.
		mExternalLock = &mExternalLockImpl;
	}
}

PosixCondition::~PosixCondition()
{
	::pthread_cond_destroy(&mCondition);
}

void PosixCondition::Wait()
{
	::pthread_cond_wait(&mCondition, &mExternalLock->mMutex);
}

bool PosixCondition::Wait(float64 pMaxWaitTime)
{
	timespec lTimeSpec;
	GetAbsTime(pMaxWaitTime, lTimeSpec);
	
	return (::pthread_cond_timedwait(&mCondition, &mExternalLock->mMutex, &lTimeSpec) == 0);
}

void PosixCondition::Signal()
{
	::pthread_cond_signal(&mCondition);
}

void PosixCondition::SignalAll()
{
	::pthread_cond_broadcast(&mCondition);
}



PosixSemaphore::PosixSemaphore():
	mMaxPermitCount(0)
{
	::sem_init(&mSemaphore, 0, 0);
}

PosixSemaphore::PosixSemaphore(unsigned pMaxCount):
	mMaxPermitCount(pMaxCount)
{
	::sem_init(&mSemaphore, 0, 0);
}

PosixSemaphore::~PosixSemaphore()
{
	::sem_destroy(&mSemaphore);
}

void PosixSemaphore::Wait()
{
	::sem_wait(&mSemaphore);
}

bool PosixSemaphore::Wait(float64 pMaxWaitTime)
{
	timespec lTimeSpec;
	GetAbsTime(pMaxWaitTime, lTimeSpec);
	return (::sem_timedwait(&mSemaphore, &lTimeSpec) == 0);
}

void PosixSemaphore::Signal()
{
	::sem_post(&mSemaphore);
}




PosixRWLock::PosixRWLock(const String& pRWLockName):
	RWLockBC(pRWLockName)
{
	::pthread_rwlock_init(&mReadWriteLock, 0);
}

PosixRWLock::~PosixRWLock()
{
	::pthread_rwlock_destroy(&mReadWriteLock);
}

void PosixRWLock::AcquireRead()
{
	::pthread_rwlock_rdlock(&mReadWriteLock);
}

void PosixRWLock::AcquireWrite()
{
	::pthread_rwlock_wrlock(&mReadWriteLock);
}

void PosixRWLock::Release()
{
	::pthread_rwlock_unlock(&mReadWriteLock);
}



}
