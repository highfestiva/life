
// Author:	Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#pragma once

#include <pthread.h>
#include <semaphore.h>
#include "../Thread.h"



namespace Lepra
{



class ThreadPointerStorage
{
public:
	ThreadPointerStorage();
	virtual ~ThreadPointerStorage();

	static void SetPointer(Thread* pThread);
	static Thread* GetPointer();

private:
	pthread_key_t mKey;
	static ThreadPointerStorage smTPS;
};



class PosixLock: public LockBC, public CompatibleLockBC
{
public:
	PosixLock();
	virtual	~PosixLock();
	void Acquire();
	bool TryAcquire();
	void Release();

private:
	friend class PosixCondition;
	pthread_mutex_t mMutex;
};



class PosixCondition: public ConditionBC
{
public:
	PosixCondition(PosixLock* pExternalLock = 0);
	virtual	~PosixCondition();

	void Wait();
	bool Wait(float64 pMaxWaitTime);
	void Signal();
	void SignalAll();

private:
	PosixLock mExternalLockImpl;
	PosixLock* mExternalLock;
	pthread_cond_t mCondition;
};



class PosixSemaphore: public SemaphoreBC
{
public:
	PosixSemaphore();
	PosixSemaphore(unsigned pMaxCount);
	virtual ~PosixSemaphore();

	void Wait();
	bool Wait(float64 pMaxWaitTime);
	void Signal();

protected:
	unsigned mMaxPermitCount;
	sem_t mSemaphore;
};



class PosixRWLock: public RWLockBC
{
public:
	PosixRWLock(const String& pRWLockName);
	virtual ~PosixRWLock();

	void AcquireRead();
	void AcquireWrite();
	void Release();

private:
	pthread_rwlock_t mReadWriteLock;
};



}
