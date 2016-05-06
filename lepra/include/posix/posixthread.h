
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <pthread.h>
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
	pthread_key_t key_;
};



class PosixLock {
public:
	PosixLock();
	virtual	~PosixLock();
	void Acquire();
	bool TryAcquire();
	void Release();

private:
	friend class PosixCondition;
	pthread_mutex_t mutex_;
};



class PosixCondition {
public:
	PosixCondition(PosixLock* external_lock);
	virtual	~PosixCondition();

	void Wait();
	bool Wait(float64 max_wait_time);
	void Signal();
	void SignalAll();

private:
	PosixLock* external_lock_;
	pthread_cond_t condition_;
};



class PosixSemaphore {
public:
	PosixSemaphore();
	PosixSemaphore(unsigned max_count);
	virtual ~PosixSemaphore();

	void Wait();
	bool Wait(float64 max_wait_time);
	void Signal();

protected:
	unsigned max_permit_count_;
	volatile unsigned permit_count_;
	pthread_mutex_t mutex_;
	pthread_cond_t condition_;

	logclass();
};



class PosixRwLock {
public:
	PosixRwLock();
	virtual ~PosixRwLock();

	void AcquireRead();
	void AcquireWrite();
	void Release();

private:
	pthread_rwlock_t read_write_lock_;
};



}
