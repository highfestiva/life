
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

protected:
	OwnedLock();
	virtual	~OwnedLock();
	void Reference();
	void Dereference();

private:
	Thread* mOwner;
	int mAcquireCount;
};

class LockBC: public OwnedLock
{
public:
	LockBC();
	virtual	~LockBC();
	virtual void Acquire() = 0;
	virtual bool TryAcquire() = 0;
	virtual void Release() = 0;
};

// Try using this whenever possible.
class ScopeLock
{
public:
	ScopeLock(LockBC* pLock);
	~ScopeLock();
	void Acquire();
	void Release();

protected:
	LockBC* mLock;
};



class ConditionBC
{
public:

	// You may set pExternalLock to NULL. But if you want to implement
	// a monitor class, you should pass the monitor's Lock as parameter
	// to all its conditions. The lock must then be acquired before all
	// calls to Wait(), Signal() and SignalAll().
	ConditionBC();
	virtual ~ConditionBC();

	// Will block calling thread, until Signal() or SignalAll() 
	// has been called by another thread.
	virtual void Wait() = 0;

	// Will block calling thread, until Signal() or SignalAll() 
	// has been called by another thread, or the given time has 
	// elapsed. If timeout occurs, the function returns false, 
	// otherwise true. Time is given in seconds.
	virtual bool Wait(float64 pMaxWaitTime) = 0;

	// Signal that condition has occured.
	// Will unblock one thread that is currently waiting for this 
	// condition to occur.
	virtual void Signal() = 0;

	// Signal that condition has occured.
	// Will unblock all threads that are currently waiting for this 
	// condition to occur.
	virtual void SignalAll() = 0;
};



class SemaphoreBC
{
public:
	SemaphoreBC();
	SemaphoreBC(unsigned pMaxCount);
	virtual ~SemaphoreBC();

	virtual void Wait() = 0;
	virtual bool Wait(float64 pMaxWaitTime) = 0;
	// Unblocks one currently, or future, waiting thread.
	virtual void Signal() = 0;
};



class RWLockBC
{
public:

	RWLockBC(const astr& pRWLockName);
	virtual ~RWLockBC();

	virtual void AcquireRead() = 0;
	virtual void AcquireWrite() = 0;
	virtual void Release() = 0;

	astr GetName();

private:
	astr mName;
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

	SemaphoreBC* mSemaphore;

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
