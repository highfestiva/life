
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <pthread.h>
#include "../Thread.h"



namespace Lepra
{



class Thread;



class ThreadPointerStorage
{
public:
	ThreadPointerStorage();
	virtual ~ThreadPointerStorage();

	void SetPointer(void* pThread);
	void* GetPointer();

private:
	pthread_key_t mKey;
};



class PosixLock: public LockBC
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
	volatile unsigned mPermitCount;
	pthread_mutex_t mMutex;
	pthread_cond_t mCondition;

	logclass();
};



class PosixRWLock: public RWLockBC
{
public:
	PosixRWLock(const astr& pRWLockName);
	virtual ~PosixRWLock();

	void AcquireRead();
	void AcquireWrite();
	void Release();

private:
	pthread_rwlock_t mReadWriteLock;
};



}
