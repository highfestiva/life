
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "win32target.h"
#include "../thread.h"



namespace lepra {



class Thread;



class ThreadPointerStorage {
public:
	ThreadPointerStorage();
	virtual ~ThreadPointerStorage();

	void SetPointer(void* thread);
	void* GetPointer();

private:
	DWORD tls_index_;
};



class Win32Lock {
public:
	Win32Lock();
	virtual	~Win32Lock();
	void Acquire();
	bool TryAcquire();
	void Release();

private:
	Win32Lock(const Win32Lock&);	// Just to forbid copying.
	void operator=(const Win32Lock&);	// Just to forbid copying.

	CRITICAL_SECTION mutex_;
};



class Win32Condition {
public:
	Win32Condition(Win32Lock* external_lock);
	virtual	~Win32Condition();

	void Wait();
	bool Wait(float64 max_wait_time);
	void Signal();
	void SignalAll();

private:
	long Increase();
	long Decrease();

	Win32Lock* external_lock_;
	HANDLE semaphore_;
	volatile long wait_thread_count_;
};



class Win32Semaphore {
public:
	Win32Semaphore();
	Win32Semaphore(unsigned max_count);
	virtual ~Win32Semaphore();

	void Wait();
	bool Wait(float64 max_wait_time);
	void Signal();

protected:
	HANDLE semaphore_;
};



class Win32RwLock {
public:
	Win32RwLock();
	virtual ~Win32RwLock();

	void AcquireRead();
	void AcquireWrite();
	void Release();

private:
	Win32Lock write_lock_;
	Win32Condition read_condition_;
	Win32Condition write_condition_;
	int num_pending_readers_;
	int num_active_readers_;
	int num_pending_writers_;
	bool is_writing_;
};



}
