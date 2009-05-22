
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include <pthread.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
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



PosixLock::PosixLock()
{
	::pthread_mutex_init(&mMutex, 0);
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
