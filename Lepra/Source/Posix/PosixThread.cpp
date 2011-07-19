
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "../../Include/HiResTimer.h"
#include "../../Include/Thread.h"



namespace Lepra
{



static ThreadPointerStorage gThreadStorage;
static ThreadPointerStorage gExtraDataStorage;



void GetAbsTime(float64 pDeltaTime, timespec& pTimeSpec)
{
	float64 lSeconds = ::floor(pDeltaTime);
	float64 lNanoSeconds = (pDeltaTime - lSeconds) * 1000000000.0;
#if defined(LEPRA_MAC)
	timeval lTimeSpecNow;
	gettimeofday(&lTimeSpecNow, NULL);
	pTimeSpec.tv_sec = lTimeSpecNow.tv_sec;
	pTimeSpec.tv_nsec = lTimeSpecNow.tv_usec * 1000;
#else
	::clock_gettime(CLOCK_REALTIME, &pTimeSpec);
#endif
	pTimeSpec.tv_sec += (time_t)lSeconds;
	pTimeSpec.tv_nsec += (long)lNanoSeconds;
	if (pTimeSpec.tv_nsec > 1000000000)
	{
		pTimeSpec.tv_nsec -= 1000000000;
		pTimeSpec.tv_sec += 1;
	}
}


	
ThreadPointerStorage::ThreadPointerStorage()
{
     ::pthread_key_create(&mKey, NULL);
}

ThreadPointerStorage::~ThreadPointerStorage()
{
     ::pthread_key_delete(mKey);
}

void ThreadPointerStorage::SetPointer(void* pThread)
{
     ::pthread_setspecific(mKey, pThread);
}

void* ThreadPointerStorage::GetPointer()
{
     return ::pthread_getspecific(mKey);
}



static void InitializeSignalMask()
{
#if !defined(LEPRA_MAC)
	sigset_t lMask;
	::sigemptyset(&lMask);
	::sigaddset(&lMask, SIGHUP);
	::pthread_sigmask(SIG_BLOCK, &lMask, 0);
#endif
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
	Reference();
}

bool PosixLock::TryAcquire()
{
	bool lAcquired = (::pthread_mutex_trylock(&mMutex) == 0);
	if (lAcquired)
	{
		Reference();
	}
	return (lAcquired);
}

void PosixLock::Release()
{
	Dereference();
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
	mMaxPermitCount(100000),
	mPermitCount(0)
{
	if (::pthread_mutex_init(&mMutex, 0) != 0)
	{
		mLog.Errorf(_T("Problem initializing Posix semaphore (mutex). errno=%i."), errno);
	}
	if (::pthread_cond_init(&mCondition, 0) != 0)
	{
		mLog.Errorf(_T("Problem initializing Posix semaphore (condition). errno=%i."), errno);
	}
}

PosixSemaphore::PosixSemaphore(unsigned pMaxCount):
	mMaxPermitCount(pMaxCount),
	mPermitCount(0)
{
	if (::pthread_mutex_init(&mMutex, 0) != 0)
	{
		mLog.Errorf(_T("Problem #2 initializing Posix semaphore (mutex). errno=%i."), errno);
	}
	if (::pthread_cond_init(&mCondition, 0) != 0)
	{
		mLog.Errorf(_T("Problem #2 initializing Posix semaphore (condition). errno=%i."), errno);
	}
}

PosixSemaphore::~PosixSemaphore()
{
	::pthread_cond_destroy(&mCondition);
	::pthread_mutex_destroy(&mMutex);
}

void PosixSemaphore::Wait()
{
	pthread_mutex_lock(&mMutex);
	while (mPermitCount == 0)
	{
		pthread_cond_wait(&mCondition, &mMutex);
	}
	--mPermitCount;
	pthread_mutex_unlock(&mMutex);
}

bool PosixSemaphore::Wait(float64 pMaxWaitTime)
{
	HiResTimer lTimer;
	pthread_mutex_lock(&mMutex);
	while (mPermitCount == 0 && lTimer.GetTimeDiff() < pMaxWaitTime)
	{
		timespec lTimeSpec;
		GetAbsTime(pMaxWaitTime-lTimer.GetTimeDiff(), lTimeSpec);
		int lResult = pthread_cond_timedwait(&mCondition, &mMutex, &lTimeSpec);
		assert(lResult != EINVAL);
		if (lResult == ETIMEDOUT)
		{
			break;
		}
		lTimer.UpdateTimer();
	}
	bool lTimeout = (mPermitCount == 0);
	if (!lTimeout)
	{
		--mPermitCount;
	}
	pthread_mutex_unlock(&mMutex);
	return (!lTimeout);
}

void PosixSemaphore::Signal()
{
	pthread_mutex_lock(&mMutex);
	if (mPermitCount < mMaxPermitCount)
	{
		++mPermitCount;
		pthread_cond_signal(&mCondition);	// Fairness may be compromized when outside lock, but glibc hang bug found inside! :(
	}
	pthread_mutex_unlock(&mMutex);
}



PosixRWLock::PosixRWLock(const astr& pRWLockName):
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



// This is where the thread starts. A global standard C-function.
void* ThreadEntry(void* pThread)
{
	Thread* lThread = (Thread*)pThread;
	gThreadStorage.SetPointer(lThread);
	gExtraDataStorage.SetPointer(0);
	assert(gThreadStorage.GetPointer() == lThread);
	assert(Thread::GetCurrentThread() == lThread);
	//InitializeSignalMask();
	RunThread(lThread);
	return (0);
}

void Thread::InitializeThread(Thread* pThread)
{
	gThreadStorage.SetPointer(pThread);
	gExtraDataStorage.SetPointer(0);
	pThread->SetThreadId(GetCurrentThreadId());
	assert(gThreadStorage.GetPointer() == pThread);
	assert(Thread::GetCurrentThread() == pThread);
	InitializeSignalMask();
}

size_t Thread::GetCurrentThreadId()
{
	return (size_t)pthread_self();
/*#if defined(LEPRA_MAC)
#else
#ifndef gettid
#define gettid()	::syscall(SYS_gettid)
#endif
	return (gettid());
#endif
*/
}

Thread* Thread::GetCurrentThread()
{
	return ((Thread*)gThreadStorage.GetPointer());
}

void* Thread::GetExtraData()
{
	return (gExtraDataStorage.GetPointer());
}

void Thread::SetExtraData(void* pData)
{
	gExtraDataStorage.SetPointer(pData);
}

void Thread::SetCpuAffinityMask(uint64 /*pAffinityMask*/)
{
	// This is not yet implemented on a per-thread basis in GNU/Linux.
	// I'm also guessing that we won't need it.
}

void Thread::Sleep(unsigned int pMilliSeconds)
{
	if (pMilliSeconds <= 0)
	{
		return;
	}
	time_t lSeconds = pMilliSeconds/1000;
	long lNanoSeconds = (pMilliSeconds - lSeconds*1000) * 1000000.0;
	timespec lTimeSpec;
	lTimeSpec.tv_sec = lSeconds;
	lTimeSpec.tv_nsec = lNanoSeconds;
	::nanosleep(&lTimeSpec, 0);
}

bool Thread::Start()
{
	bool lOk = true;

	if (!IsRunning())
	{
		pthread_t	lThreadHandle;
		pthread_attr_t	lThreadAttributes;

		SetStopRequest(false);

		pthread_attr_init(&lThreadAttributes);
		pthread_attr_setdetachstate(&lThreadAttributes, PTHREAD_CREATE_JOINABLE);

		if(::pthread_create(&lThreadHandle, &lThreadAttributes, ThreadEntry, this) == 0)
		{
			mThreadHandle = (size_t)lThreadHandle;

			// Wait for newly created thread to kickstart.
			mSemaphore.Wait(5.0);
		}
		else
		{
			SetRunning(false);
			mLog.Errorf((_T("Failed to start thread ")+strutil::Encode(GetThreadName())+_T("!")).c_str());
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
		::pthread_join((pthread_t)mThreadHandle, 0);
		assert(!IsRunning());
		mThreadHandle = 0;
		mThreadId = 0;
	}
	SetStopRequest(false);
	return (true);
}

bool Thread::GraceJoin(float64 pTimeOut)
{
	if (GetThreadHandle() != 0)
	{
		assert(GetThreadId() != GetCurrentThreadId());
		if (!mSemaphore.Wait(pTimeOut))
		{
			// Possible dead lock...
			mLog.Warningf((_T("Failed to timed join thread \"") + strutil::Encode(GetThreadName()) + _T("\"! Deadlock?")).c_str());
			return (false);	// RAII simplifies here.
		}
		Join();
	}
	SetStopRequest(false);
	return (true);
}

void Thread::Signal(int) // Ignore signal for now.
{
	if (mThreadHandle != 0)
	{
		::pthread_kill((pthread_t)mThreadHandle, SIGHUP);
	}
}

void Thread::Kill()
{
	if (GetThreadHandle() != 0)
	{
		assert(GetThreadId() != GetCurrentThreadId());
		mLog.Warning(_T("Forcing kill of thread ") + strutil::Encode(GetThreadName()));
		::pthread_kill((pthread_t)mThreadHandle, SIGHUP);
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
	mSemaphore.Signal();
}



LOG_CLASS_DEFINE(GENERAL, PosixSemaphore);



}
