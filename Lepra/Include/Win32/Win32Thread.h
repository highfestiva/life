
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



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



class Win32Lock
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



class Win32Condition
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



class Win32Semaphore
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



class Win32RwLock
{
public:
	Win32RwLock();
	virtual ~Win32RwLock();

	void AcquireRead();
	void AcquireWrite();
	void Release();

private:
	Win32Lock mWriteLock;
	Win32Condition mReadCondition;
	Win32Condition mWriteCondition;
	int mNumPendingReaders;
	int mNumActiveReaders;
	int mNumPendingWriters;
	bool mIsWriting;
};



}
