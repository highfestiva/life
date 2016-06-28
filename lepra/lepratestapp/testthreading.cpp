/*
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include <locale>
#include "../include/timer.h"
#include "../include/hirestimer.h"
#include "../include/log.h"
#include "../include/memberthread.h"
#include "../include/socket.h"
#include "../include/spinlock.h"

using namespace lepra;

void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);

const int STATE_ALLOC_INIT = 0x70010210;
const int STATE_ALLOC_FOUND = 0x70111091;

volatile int g_thread_test_counter;

Lock g_thread_test_lock;
Semaphore g_thread_test_semaphore;
int* g_alloc_mem = 0;

void IncreaseThread(void*) {
	++g_thread_test_counter;
}

void LockThread(void*) {
	g_thread_test_counter = 0;
	g_thread_test_lock.Acquire();
	++g_thread_test_counter;
	g_thread_test_lock.Release();
	++g_thread_test_counter;
	g_thread_test_semaphore.Wait();
	++g_thread_test_counter;
	g_thread_test_semaphore.Signal();
	++g_thread_test_counter;
	g_thread_test_semaphore.Wait();
	g_thread_test_semaphore.Wait();
	++g_thread_test_counter;
	g_thread_test_semaphore.Signal();
	++g_thread_test_counter;
	while (!Thread::GetCurrentThread()->GetStopRequest()) {
		Thread::Sleep(0.001);
	}
	++g_thread_test_counter;
}

void MemAllocThreadEntry(void*) {
	deb_assert(g_alloc_mem == 0);
	g_alloc_mem = new int[3];
	g_alloc_mem[1] = STATE_ALLOC_INIT;
}

void MemUseThreadEntry(void*) {
	while (g_alloc_mem == 0)
		;
	while (g_alloc_mem[1] != STATE_ALLOC_FOUND)
		;
	delete[] (g_alloc_mem);
	g_alloc_mem = 0;
}

class MemberThreadTestClass {
public:
	void OnTest() {
		g_thread_test_counter = -4;
	}
};

class ThreadIdTestClass {
public:
	size_t thread_id_;
	void StoreId() {
		thread_id_ = Thread::GetCurrentThreadId();
	}
};

bool TestThreading(const LogDecorator& account) {
	str _context;
	bool test_ok = true;

	if (test_ok) {
		_context = "thread start";
		for (unsigned x = 0; x < 100 && test_ok; ++x) {
			StaticThread thread("StartupTest");
			g_thread_test_counter = 7;
			test_ok = thread.Start(IncreaseThread, 0);
			deb_assert(test_ok);
			if (test_ok) {
				_context = "thread run and self termination";
				//Thread::Sleep(0.001);	// Make sure we sleep at least once.
				for (unsigned y = 0; y < 200 && thread.IsRunning(); ++y) {
					Thread::Sleep(0.001);
				}
				test_ok = (g_thread_test_counter == 8);
				deb_assert(test_ok);
				if (test_ok) {
					test_ok = !thread.IsRunning();
					deb_assert(test_ok);
				}
			}
		}
	}

	if (test_ok) {
		_context = "mutex ownership";
		test_ok = (g_thread_test_lock.IsOwner() == false);
		if (test_ok) {
			g_thread_test_lock.Acquire();
			test_ok = (g_thread_test_lock.IsOwner() == true);
		}
		if (test_ok) {
			g_thread_test_lock.Release();
			test_ok = (g_thread_test_lock.IsOwner() == false);
		}
		deb_assert(test_ok);
	}

	StaticThread thread("LockTest");
	if (test_ok) {
		_context = "mutex exclusiveness";
		g_thread_test_counter = -1;
		g_thread_test_lock.Acquire();
		test_ok = thread.Start(LockThread, 0);
		if (test_ok) {
			test_ok = thread.IsRunning();
			deb_assert(test_ok);
		}
		Thread::Sleep(0.1);
		test_ok = (g_thread_test_counter == 0 && thread.IsRunning());
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "spin lock";
		SpinLock lock;
		test_ok = lock.TryAcquire();
		deb_assert(test_ok);
		lock.Release();
	}
	if (test_ok) {
		_context = "semaphore exclusiveness";
		g_thread_test_semaphore.Signal();
		g_thread_test_semaphore.Wait();
		g_thread_test_lock.Release();
		Thread::Sleep(0.1);
		test_ok = (g_thread_test_counter == 2 && thread.IsRunning());
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "semaphore wait timeout - 1";
		HiResTimer timer(false);
		test_ok = !g_thread_test_semaphore.Wait(0.5);
		deb_assert(test_ok);
		timer.UpdateTimer();
		if (test_ok) {
			test_ok = Math::IsInRange(timer.GetTimeDiff(), 0.4, 0.9);
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = (g_thread_test_counter == 2 && thread.IsRunning());
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = !g_thread_test_semaphore.Wait(0.1);
			deb_assert(test_ok);
		}
		if (test_ok) {
			g_thread_test_semaphore.Signal();
			Thread::Sleep(0.1);
			test_ok = !g_thread_test_semaphore.Wait(0.1);
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = (g_thread_test_counter == 4 && thread.IsRunning());
			deb_assert(test_ok);
		}
	}
	if (test_ok) {
		_context = "thread operation";
		g_thread_test_semaphore.Signal();
		Thread::Sleep(0.1);
		test_ok = (g_thread_test_counter == 6 && thread.IsRunning());
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "thread grace termination";
		thread.RequestStop();
		Thread::Sleep(0.1);
		test_ok = (g_thread_test_counter == 7 && !thread.IsRunning());
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "MemberThread";
		{
			MemberThreadTestClass test_class;
			MemberThread<MemberThreadTestClass> mthread("TestMemberThread");
			g_thread_test_counter = -5;
			test_ok = mthread.Start(&test_class, &MemberThreadTestClass::OnTest);
			deb_assert(test_ok);
			if (test_ok) {
				test_ok = mthread.Join();
				deb_assert(test_ok);
			}
		}
		if (test_ok) {
			test_ok = (g_thread_test_counter == -4);
			deb_assert(test_ok);
		}
	}
	if (test_ok) {
		_context = "main thread ID";
		test_ok = (lepra::Thread::GetCurrentThreadId() != 0);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "sub thread ID";
		ThreadIdTestClass test_instance;
		test_instance.thread_id_ = 0;
		MemberThread<ThreadIdTestClass> mthread("TestThreadId");
		if (test_ok) {
			test_ok = mthread.Start(&test_instance, &ThreadIdTestClass::StoreId);
			deb_assert(test_ok);
		}
		size_t actual_thread_id = mthread.GetThreadId();
		if (test_ok) {
			test_ok = mthread.Join();
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = (test_instance.thread_id_ != 0 && test_instance.thread_id_ == actual_thread_id);
			deb_assert(test_ok);
		}
	}
	if (test_ok) {
		_context = "semaphore wait timeout";
		lepra::Timer timer;
		lepra::Semaphore semaphore;
		test_ok = !semaphore.Wait(1.0);
		deb_assert(test_ok);
		if (test_ok) {
			double time = timer.PopTimeDiff();
			test_ok = (time >= 0.9 && time < 1.1);
			deb_assert(test_ok);
		}
	}
	lepra::Semaphore semaphore;
	if (test_ok) {
		_context = "semaphore satisfied timed wait 1";
		lepra::Timer timer;
		semaphore.Signal();
		semaphore.Signal();
		test_ok = semaphore.Wait(1.0);
		deb_assert(test_ok);
		if (test_ok) {
			double time = timer.PopTimeDiff();
			test_ok = (time >= 0.0 && time < 0.1);
			deb_assert(test_ok);
		}
	}
	if (test_ok) {
		_context = "semaphore satisfied timed wait 2";
		lepra::Timer timer;
		test_ok = semaphore.Wait(1.0);
		deb_assert(test_ok);
		if (test_ok) {
			double time = timer.PopTimeDiff();
			test_ok = (time >= 0.0 && time < 0.1);
			deb_assert(test_ok);
		}
	}
	if (test_ok) {
		_context = "semaphore permit reset";
		test_ok = !semaphore.Wait(0.0001);
		deb_assert(test_ok);
	}

	StaticThread mem_alloc_thread("MemAllocator");
	StaticThread mem_use_thread("MemUser");
	if (test_ok) {
		_context = "Memory allocator";
		mem_use_thread.Start(MemUseThreadEntry, 0);
		mem_alloc_thread.Start(MemAllocThreadEntry, 0);
		Thread::Sleep(0.01);
		test_ok = (g_alloc_mem != 0 && g_alloc_mem[1] == STATE_ALLOC_INIT && !mem_alloc_thread.IsRunning() && mem_use_thread.IsRunning());
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "Memory freer";
		g_alloc_mem[1] = STATE_ALLOC_FOUND;
		Thread::Sleep(0.01);
		test_ok = (g_alloc_mem == 0 && !mem_use_thread.IsRunning());
		deb_assert(test_ok);
	}

	ReportTestResult(account, "Threading", _context, test_ok);
	return (test_ok);
}
