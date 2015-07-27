
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "LepraTarget.h"
#include "LepraTypes.h"
#include "Log.h"
#include "String.h"
#include "BusLock.h"



namespace Lepra
{



class Thread;



class OwnedLock
{
public:
	bool IsOwner() const;
	Thread* GetOwner() const;
	int GetReferenceCount() const;

	// Avoid using the below.
	OwnedLock();
	virtual	~OwnedLock();
	void Reference();
	void Dereference();

private:
	Thread* mOwner;
	int mAcquireCount;
};

class Lock: public OwnedLock
{
public:
	Lock();
	virtual	~Lock();
	void Acquire();
	bool TryAcquire();
	void Release();
	void* GetSystemLock() const;	// Don't use this!

private:
	void operator=(const Lock&);

	void* mSystemLock;
};

// Try using this whenever possible.
class ScopeLock
{
public:
	ScopeLock(Lock* pLock);
	~ScopeLock();
	void Acquire();
	void Release();

protected:
	Lock* mLock;
};



class Condition
{
public:
	Condition(Lock* pExternalLock = 0);
	virtual ~Condition();

	void Wait();
	bool Wait(float64 pMaxWaitTime);
	void Signal();
	void SignalAll();	// Unblock all waiting threads.

private:
	Lock* mExternalLock;
	void* mSystemCondition;
};



class Semaphore
{
public:
	Semaphore();
	Semaphore(unsigned pMaxCount);
	virtual ~Semaphore();

	void Wait();
	bool Wait(float64 pMaxWaitTime);
	void Signal();	// Unblocks one currently, or future, waiting thread.

private:
	void* mSystemSemaphore;
};



class RwLock
{
public:
	RwLock(const astr& pRwLockName);
	virtual ~RwLock();

	void AcquireRead();
	void AcquireWrite();
	void Release();

	astr GetName();

private:
	astr mName;
	void* mSystemRwLock;
};



class Thread
{
public:
	Thread(const astr& pThreadName);
	virtual ~Thread();

	static void InitializeMainThread();
	static bool QueryInitializeThread();
	static void InitializeThread(Thread* pThread);

	const astr& GetThreadName() const;
	size_t GetThreadId() const;
	size_t GetThreadHandle() const;

	bool IsRunning() const;
	bool GetStopRequest() const;
	void RequestStop();	// Tells thread to gracefully terminate by setting the stop request flag.
	bool GetSelfDestruct() const;
	void RequestSelfDestruct();	// Deletes thead object when thread terminates.

	// All public functions below are NOT implemented in Thread.cpp, but somewhere
	// else instead. Win32Thread.cpp on a Win32 application, for instance.

	static size_t GetCurrentThreadId();
	static Thread* GetCurrentThread();
	static void* GetExtraData();
	static void SetExtraData(void* pData);
	void SetCpuAffinityMask(uint64 pAffinityMask);

	bool Start();
	static void Sleep(float64 pTime);
	static void Sleep(unsigned int pMilliSeconds);
	static void YieldCpu();

	bool Join();
	bool Join(float64 pTimeOut);
	bool GraceJoin(float64 pTimeOut);
	void Signal(int pSignal);
	void Kill();

protected:
	virtual void Run() = 0;
	void PostRun();

	void RunThread();
	friend void RunThread(Thread* pThread);

	void SetStopRequest(bool pStopRequest);
	void SetRunning(bool pRunning);
	void SetThreadId(size_t pThreadId);

private:
	astr mThreadName;	// Must be ANSI, to be compliant with non-unicode builds.

	volatile bool mRunning;
	volatile bool mStopRequested;
	bool mSelfDestruct;

	size_t mThreadHandle;
	size_t mThreadId;

	Semaphore mSemaphore;

	logclass();
};



class StaticThread: public Thread
{
public:
	StaticThread(const astr& pThreadName);
	virtual ~StaticThread();

	bool Start(void (*pThreadEntry)(void*), void* pData);

protected:
	bool Start();
	void Run();

private:
	void (*mThreadEntry)(void*);
	void* mData;
};



}
