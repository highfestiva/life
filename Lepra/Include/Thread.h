
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTarget.h"
#include "LepraTypes.h"
#define EXCLUDE_LOG_CLASS
#include "Log.h"
#undef EXCLUDE_LOG_CLASS
#include "String.h"
#include "BusLock.h"



namespace Lepra
{



/*
//
// A state lock is a spin lock that will loop through a number of states,
// and can only be acquired when the current state is the state previous
// to the desired one (special case if the desired state is the first state,
// in which case the current state has to be the last one).
//
// The states must be enumerated from 0 to pNumStates - 1.
//
class StateLock
{
public:
	inline StateLock(long pNumStates);
	inline StateLock(const StateLock& pStateLock);
	inline ~StateLock();

	// Will be acquired when the state is the previous one of pDesiredState,
	// and updated to pDesiredState.
	inline void AcquireState(long pDesiredState);

	// Will wait until the state has been acquired to pDesiredState by another
	// thread.
	inline void WaitForState(long pDesiredState);
private:
	long mNumStates;
	long mState;
};

StateLock::StateLock(long pNumStates) :
	mNumStates(pNumStates),
	mState(0)
{
}

StateLock::StateLock(const StateLock& pStateLock) :
	mNumStates(pStateLock.mNumStates),
	mState(pStateLock.mState)
{
}

StateLock::~StateLock()
{
}

void StateLock::AcquireState(long pDesiredState)
{
	long lPrevState = (pDesiredState - 1);
	if (pDesiredState == 0)
		lPrevState = mNumStates - 1;
	while (CompareAndSwap(mState, pDesiredState, lPrevState) == false)
	{
		// Yielding the Cpu is a good thing to do on a single core
		// machine, while it's probably better to keep looping if we 
		// are running on a multi core computer.
		 Thread::YieldCpu();
	}
}

void StateLock::WaitForState(long pDesiredState)
{
	while (CompareAndSwap(mState, pDesiredState, pDesiredState) == false)
	{
		// Yielding the Cpu is a good thing to do on a single core
		// machine, while it's probably better to keep looping if we 
		// are running on a multi core computer.
		 Thread::YieldCpu();
	}
}*/



// Try using this instead of CompatibleLock wherever possible.
class LockBC
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





class CompatibleLockBC
{
public:
	CompatibleLockBC();
	virtual	~CompatibleLockBC();

	// Will block calling thread until mutex is obtained.
	virtual void Acquire() = 0;

	// Will return (false) immediately if already locked by 
	// another thread. Returns true if mutex was acquired. 
	virtual bool TryAcquire() = 0;

	// Release lock. Must be called after the lock has been 
	// obtained, otherwise it might block other threads during 
	// an infinite time.
	virtual void Release() = 0;
};

class CompatibleScopeLock
{
public:
	CompatibleScopeLock(CompatibleLockBC* pLock);
	~CompatibleScopeLock();

protected:
	CompatibleLockBC* mLock;
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

class CompatibleConditionBC
{
public:

	// You may set pExternalLock to NULL. But if you want to implement
	// a monitor class, you should pass the monitor's Lock as parameter
	// to all its conditions. The lock must then be acquired before all
	// calls to Wait(), Signal() and SignalAll().
	CompatibleConditionBC(CompatibleLockBC* pExternalLock);
	virtual ~CompatibleConditionBC();

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

protected:
	CompatibleLockBC* mExternalLock;
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


/*class CompatibleSemaphoreBC
{
public:
	CompatibleSemaphoreBC();
	virtual ~CompatibleSemaphoreBC();

	virtual void Acquire(int pNumPermits = 1) = 0;
	virtual bool Acquire(float64 pMaxWaitTime, int pNumPermits = 1) = 0;
	virtual bool TryAcquire(int pNumPermits = 1) = 0;

	// Acquire all permits. 
	virtual void Drain() = 0;

	virtual void Release(int pNumPermits = 1) = 0;

protected:
};*/



class RWLockBC
{
public:

	RWLockBC(const String& pRWLockName);
	virtual ~RWLockBC();

	virtual void AcquireRead() = 0;
	virtual void AcquireWrite() = 0;
	virtual void Release() = 0;

	String GetName();

private:
	String mName;
};



}



#if defined(LEPRA_WINDOWS)

#include "Win32/Win32Thread.h"
#define Lock			Win32Lock
#define CompatibleLock		Win32CompatibleLock
#define Condition		Win32Condition
#define CompatibleCondition	Win32CompatibleCondition
#define Semaphore		Win32Semaphore
//#define CompatibleSemaphore	Win32Semaphore
#define RWLock			Win32RWLock

#elif defined(LEPRA_POSIX)

#include "Posix/PosixThread.h"
#define Lock			PosixLock
#define CompatibleLock		PosixLock
#define Condition		PosixCondition
#define CompatibleCondition	PosixCondition
#define Semaphore		PosixSemaphore
//#define CompatibleSemaphore	PosixSemaphore
#define RWLock			Win32RWLock

#else

#error "Not implemented for target system!"

#endif // LEPRA_WINDOWS/LEPRA_POSIX



namespace Lepra
{



class Thread
{
public:
	Thread(const String& pThreadName);
	virtual ~Thread();

	static void InitializeMainThread(const String& pThreadName);

	const String& GetThreadName();
	size_t GetThreadId();
	size_t GetThreadHandle();

	bool IsRunning() const;
	bool GetStopRequest() const;
	void RequestStop();	// Tells thread to gracefully terminate by setting the stop request flag.
	bool GetSelfDestruct() const;
	void RequestSelfDestruct();	// Deletes thead object when thread terminates.

	// All public functions below are NOT implemented in Thread.cpp, but somewhere
	// else instead. Win32Thread.cpp on a Win32 application, for instance.

	static size_t GetCurrentThreadId();
	static Thread* GetCurrentThread();
	void SetCpuAffinityMask(uint64 pAffinityMask);

	bool Start();
	static void Sleep(float64 pTime);
	static void Sleep(unsigned int pMilliSeconds);
	static void YieldCpu();

	bool Join();
	bool Join(float64 pTimeOut);
	void Signal(int pSignal);
	void Kill();

protected:
	virtual void Run() = 0;
	void PostRun();

	void RunThread();
	friend void RunThread(Thread* pThread);

	void SetStopRequest(bool pStopRequest);
	void SetRunning(bool pRunning);

private:
	String mThreadName;

	volatile bool mRunning;
	volatile bool mStopRequested;
	bool mSelfDestruct;

	size_t mThreadHandle;
	size_t mThreadId;

	Semaphore mSemaphore;

	LOG_CLASS_DECLARE();
};



class StaticThread: public Thread
{
public:
	StaticThread(const String& pThreadName);
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
