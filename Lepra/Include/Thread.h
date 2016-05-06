
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratarget.h"
#include "lepratypes.h"
#include "log.h"
#include "string.h"
#include "buslock.h"



namespace lepra {



class Thread;



class OwnedLock {
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
	Thread* owner_;
	int acquire_count_;
};

class Lock: public OwnedLock {
public:
	Lock();
	virtual	~Lock();
	void Acquire();
	bool TryAcquire();
	void Release();
	void* GetSystemLock() const;	// Don't use this!

private:
	void operator=(const Lock&);

	void* system_lock_;
};

// Try using this whenever possible.
class ScopeLock {
public:
	ScopeLock(Lock* lock);
	~ScopeLock();
	void Acquire();
	void Release();

protected:
	Lock* lock_;
};



class Condition {
public:
	Condition(Lock* external_lock = 0);
	virtual ~Condition();

	void Wait();
	bool Wait(float64 max_wait_time);
	void Signal();
	void SignalAll();	// Unblock all waiting threads.

private:
	Lock* external_lock_;
	void* system_condition_;
};



class Semaphore {
public:
	Semaphore();
	Semaphore(unsigned max_count);
	virtual ~Semaphore();

	void Wait();
	bool Wait(float64 max_wait_time);
	void Signal();	// Unblocks one currently, or future, waiting thread.

private:
	void* system_semaphore_;
};



class RwLock {
public:
	RwLock(const str& rw_lock_name);
	virtual ~RwLock();

	void AcquireRead();
	void AcquireWrite();
	void Release();

	str GetName();

private:
	str name_;
	void* system_rw_lock_;
};



class Thread {
public:
	Thread(const str& thread_name);
	virtual ~Thread();

	static void InitializeMainThread();
	static bool QueryInitializeThread();
	static void InitializeThread(Thread* thread);

	const str& GetThreadName() const;
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
	static void SetExtraData(void* data);
	void SetCpuAffinityMask(uint64 affinity_mask);

	bool Start();
	static void Sleep(float64 time);
	static void YieldCpu();

	bool Join();
	bool Join(float64 time_out);
	bool GraceJoin(float64 time_out);
	void Signal(int signal);
	void Kill();

protected:
	static void Sleep(unsigned int micro_seconds);
	virtual void Run() = 0;
	void PostRun();

	void RunThread();
	friend void RunThread(Thread* thread);

	void SetStopRequest(bool stop_request);
	void SetRunning(bool running);
	void SetThreadId(size_t thread_id);

private:
	str thread_name_;	// Must be ANSI, to be compliant with non-unicode builds.

	volatile bool running_;
	volatile bool stop_requested_;
	bool self_destruct_;

	size_t thread_handle_;
	size_t thread_id_;

	Semaphore semaphore_;

	logclass();
};



class StaticThread: public Thread {
public:
	StaticThread(const str& thread_name);
	virtual ~StaticThread();

	bool Start(void (*thread_entry)(void*), void* data);

protected:
	bool Start();
	void Run();

private:
	void (*thread_entry_)(void*);
	void* data_;
};



}
