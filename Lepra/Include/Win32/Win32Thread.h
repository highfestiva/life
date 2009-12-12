
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "Win32Target.h"
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
	DWORD mTLSIndex;
};



class Win32Lock: public LockBC
{
public:
	Win32Lock();
	virtual	~Win32Lock();
	void Acquire();
	bool TryAcquire();
	void Release();

private:
	Win32Lock(const Win32Lock&);	// Just to forbid copying.
	void operator=(const Win32Lock&);	// Just to forbid copying.

	CRITICAL_SECTION mMutex;
};



class Win32CompatibleLock: public CompatibleLockBC
{
public:
	friend class Win32CompatibleCondition;

	Win32CompatibleLock();
	virtual	~Win32CompatibleLock();

	void Acquire();
	bool Acquire(float64 pMaxWaitTime);
	bool TryAcquire();
	void Release();

private:
	HANDLE mMutex;
	int mLockCount;
};



class Win32Condition: public ConditionBC
{
public:
	Win32Condition();
	virtual	~Win32Condition();

	void Wait();
	bool Wait(float64 pMaxWaitTime);
	void Signal();
	void SignalAll();

private:
	long Increase();
	long Decrease();

	HANDLE mSemaphore;
	volatile long mWaitThreadCount;
	CRITICAL_SECTION mSignalLock;
};



class Win32CompatibleCondition: public CompatibleConditionBC
{
public:
	Win32CompatibleCondition(Win32CompatibleLock* pExternalLock = 0);
	virtual	~Win32CompatibleCondition();

	void Wait();
	bool Wait(float64 pMaxWaitTime);
	void Signal();
	void SignalAll();
private:

	// All these members are required in order to implement a fair and bug free condition
	// variable on Windows. The comments and the algorithms are taken from a paper on how
	// to implement POSIX condition variables on Win32.

	int mNumWaiters;					// Number of waiting threads.
	CRITICAL_SECTION mNumWaitersLock;	// Serialize access to mNumWaiters.
	HANDLE mSemaphore;					// Semaphore used to queue up threads waiting for 
										// the condition to become signaled.
	HANDLE mWaitersDoneEvent;			// An auto-reset event used by the broadcast/signal 
										// thread to wait for all the waiting thread(s) to 
										// wake up and be released from the semaphore. 
	bool mWasBroadcast;				// Keeps track of whether we were broadcasting or 
										// signaling.  This allows us to optimize the code if 
										// we're just signaling.

	// Remove this and send it as a parameter to all member functions.
	// Thus, this should be replace by an external mutex if you need
	// conditions within a monitor. Remove the surrounding 
	// WaitForSingleObject(mMutex) and ReleaseMutex(mMutex) in all 
	// functions as well.
	HANDLE mMutex;
};



class Win32Semaphore: public SemaphoreBC
{
public:
	Win32Semaphore();
	Win32Semaphore(unsigned pMaxCount);
	virtual ~Win32Semaphore();

	void Wait();
	bool Wait(float64 pMaxWaitTime);
	void Signal();

protected:
	HANDLE mSemaphore;
};



/*class Win32CompatibleSemaphore: public CompatibleSemaphoreBC
{
public:
	Win32CompatibleSemaphore(int pCount);
	virtual ~Win32CompatibleSemaphore();

	void Acquire(int pNumPermits = 1);
	bool Acquire(float64 pMaxWaitTime, int pNumPermits = 1);
	bool TryAcquire(int pNumPermits = 1);
	
	void Drain();

	void Release(int pNumPermits = 1);

protected:
	int mCount;
	unsigned int mNumWaiters;
	Win32CompatibleLock mLock;
	Win32CompatibleCondition mCountNonZero;
};*/



class Win32RWLock: public RWLockBC
{
public:
	Win32RWLock(const String& pRWLockName);
	virtual ~Win32RWLock();

	void AcquireRead();
	void AcquireWrite();
	void Release();

private:
	Win32CompatibleLock mWriteLock;
	Win32CompatibleCondition mReadCondition;
	Win32CompatibleCondition mWriteCondition;
	int mNumPendingReaders;
	int mNumActiveReaders;
	int mNumPendingWriters;
	bool mIsWriting;
};



}
