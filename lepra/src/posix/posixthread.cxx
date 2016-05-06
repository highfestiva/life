
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "../../include/lepraassert.h"
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "../../include/lepraos.h"
#include "../../include/hirestimer.h"
#include "../../include/posix/posixthread.h"



namespace lepra {



static ThreadPointerStorage g_thread_storage;
static ThreadPointerStorage g_extra_data_storage;



void GetAbsTime(float64 delta_time, timespec& time_spec) {
	float64 seconds = ::floor(delta_time);
	float64 nano_seconds = (delta_time - seconds) * 1000000000.0;
#if defined(LEPRA_MAC)
	timeval time_spec_now;
	gettimeofday(&time_spec_now, NULL);
	time_spec.tv_sec = time_spec_now.tv_sec;
	time_spec.tv_nsec = time_spec_now.tv_usec * 1000;
#else
	::clock_gettime(CLOCK_REALTIME, &time_spec);
#endif
	time_spec.tv_sec += (time_t)seconds;
	time_spec.tv_nsec += (long)nano_seconds;
	if (time_spec.tv_nsec > 1000000000) {
		time_spec.tv_nsec -= 1000000000;
		time_spec.tv_sec += 1;
	}
}



ThreadPointerStorage::ThreadPointerStorage() {
     ::pthread_key_create(&key_, NULL);
}

ThreadPointerStorage::~ThreadPointerStorage() {
     ::pthread_key_delete(key_);
}

void ThreadPointerStorage::SetPointer(void* thread) {
     ::pthread_setspecific(key_, thread);
}

void* ThreadPointerStorage::GetPointer() {
     return ::pthread_getspecific(key_);
}



static void InitializeSignalMask() {
#if !defined(LEPRA_MAC)
	sigset_t mask;
	::sigemptyset(&mask);
	::sigaddset(&mask, SIGHUP);
	::pthread_sigmask(SIG_BLOCK, &mask, 0);
#endif
}



PosixLock::PosixLock() {
	::pthread_mutexattr_t mutex_attr;
	::pthread_mutexattr_init(&mutex_attr);
	::pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
	::pthread_mutex_init(&mutex_, &mutex_attr);
	::pthread_mutexattr_destroy(&mutex_attr);
}

PosixLock::~PosixLock() {
	::pthread_mutex_destroy(&mutex_);
}

void PosixLock::Acquire() {
	::pthread_mutex_lock(&mutex_);
}

bool PosixLock::TryAcquire() {
	return (::pthread_mutex_trylock(&mutex_) == 0);
}

void PosixLock::Release() {
	::pthread_mutex_unlock(&mutex_);
}




PosixCondition::PosixCondition(PosixLock* external_lock):
	external_lock_(external_lock) {
	::pthread_cond_init(&condition_, 0);
}

PosixCondition::~PosixCondition() {
	::pthread_cond_destroy(&condition_);
}

void PosixCondition::Wait() {
	::pthread_cond_wait(&condition_, &external_lock_->mutex_);
}

bool PosixCondition::Wait(float64 max_wait_time) {
	timespec _time_spec;
	GetAbsTime(max_wait_time, _time_spec);
	return (::pthread_cond_timedwait(&condition_, &external_lock_->mutex_, &_time_spec) == 0);
}

void PosixCondition::Signal() {
	::pthread_cond_signal(&condition_);
}

void PosixCondition::SignalAll() {
	::pthread_cond_broadcast(&condition_);
}



PosixSemaphore::PosixSemaphore():
	max_permit_count_(100000),
	permit_count_(0) {
	if (::pthread_mutex_init(&mutex_, 0) != 0) {
		log_.Errorf("Problem initializing Posix semaphore (mutex. errno=%i).", errno);
	}
	if (::pthread_cond_init(&condition_, 0) != 0) {
		log_.Errorf("Problem initializing Posix semaphore (condition. errno=%i).", errno);
	}
}

PosixSemaphore::PosixSemaphore(unsigned max_count):
	max_permit_count_(max_count),
	permit_count_(0) {
	if (::pthread_mutex_init(&mutex_, 0) != 0) {
		log_.Errorf("Problem #2 initializing Posix semaphore (mutex. errno=%i).", errno);
	}
	if (::pthread_cond_init(&condition_, 0) != 0) {
		log_.Errorf("Problem #2 initializing Posix semaphore (condition. errno=%i).", errno);
	}
}

PosixSemaphore::~PosixSemaphore() {
	::pthread_cond_destroy(&condition_);
	::pthread_mutex_destroy(&mutex_);
}

void PosixSemaphore::Wait() {
	pthread_mutex_lock(&mutex_);
	while (permit_count_ == 0) {
		pthread_cond_wait(&condition_, &mutex_);
	}
	--permit_count_;
	deb_assert(permit_count_ >= 0);
	pthread_mutex_unlock(&mutex_);
}

bool PosixSemaphore::Wait(float64 max_wait_time) {
	HiResTimer timer(false);
	pthread_mutex_lock(&mutex_);
	while (permit_count_ == 0 && timer.GetTimeDiff() < max_wait_time) {
		timespec _time_spec;
		GetAbsTime(max_wait_time-timer.GetTimeDiff(), _time_spec);
		int result = pthread_cond_timedwait(&condition_, &mutex_, &_time_spec);
		deb_assert(result != EINVAL);
		if (result == ETIMEDOUT) {
			break;
		}
		timer.UpdateTimer();
	}
	bool timeout = (permit_count_ == 0);
	if (!timeout) {
		--permit_count_;
	}
	deb_assert(permit_count_ >= 0);
	pthread_mutex_unlock(&mutex_);
	return (!timeout);
}

void PosixSemaphore::Signal() {
	pthread_mutex_lock(&mutex_);
	if (permit_count_ < max_permit_count_) {
		++permit_count_;
		pthread_cond_signal(&condition_);	// Fairness may be compromized when outside lock, but glibc hang bug found inside! :(
	}
	pthread_mutex_unlock(&mutex_);
}



PosixRwLock::PosixRwLock() {
	::pthread_rwlock_init(&read_write_lock_, 0);
}

PosixRwLock::~PosixRwLock() {
	::pthread_rwlock_destroy(&read_write_lock_);
}

void PosixRwLock::AcquireRead() {
	::pthread_rwlock_rdlock(&read_write_lock_);
}

void PosixRwLock::AcquireWrite() {
	::pthread_rwlock_wrlock(&read_write_lock_);
}

void PosixRwLock::Release() {
	::pthread_rwlock_unlock(&read_write_lock_);
}



// This is where the thread starts. A global standard C-function.
void* ThreadEntry(void* thread) {
	Thread* _thread = (Thread*)thread;
	g_thread_storage.SetPointer(_thread);
	g_extra_data_storage.SetPointer(0);
	deb_assert(g_thread_storage.GetPointer() == _thread);
	deb_assert(Thread::GetCurrentThread() == _thread);
	//InitializeSignalMask();
#if defined(LEPRA_MAC) && !defined(LEPRA_IOS)
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
#endif // Mac
	RunThread(_thread);
#if defined(LEPRA_MAC) && !defined(LEPRA_IOS)
	[pool release];
#endif // Mac
	return (0);
}

void Thread::InitializeThread(Thread* thread) {
	g_thread_storage.SetPointer(thread);
	g_extra_data_storage.SetPointer(0);
	thread->SetThreadId(GetCurrentThreadId());
	deb_assert(g_thread_storage.GetPointer() == thread);
	deb_assert(Thread::GetCurrentThread() == thread);
	InitializeSignalMask();
#if defined(LEPRA_MAC) && !defined(LEPRA_IOS)
	[[NSAutoreleasePool alloc] init];
#endif // Mac
}

size_t Thread::GetCurrentThreadId() {
	return (size_t)pthread_self();
/*#if defined(LEPRA_MAC)
#else
#ifndef gettid
#define gettid()	::syscall(SYS_gettid)
#endif
	return (gettid());
#endif
*/
}

Thread* Thread::GetCurrentThread() {
	return ((Thread*)g_thread_storage.GetPointer());
}

void* Thread::GetExtraData() {
	return (g_extra_data_storage.GetPointer());
}

void Thread::SetExtraData(void* data) {
	g_extra_data_storage.SetPointer(data);
}

void Thread::SetCpuAffinityMask(uint64 /*affinity_mask*/) {
	// This is not yet implemented on a per-thread basis in GNU/Linux.
	// I'm also guessing that we won't need it.
}

void Thread::Sleep(unsigned int micro_seconds) {
	if (micro_seconds <= 0) {
		return;
	}
	const int seconds = micro_seconds/1000000;
	const int nano_seconds = (micro_seconds-seconds*1000000) * 1000;
	timespec t = {seconds, nano_seconds};
	::nanosleep(&t, 0);
}

bool Thread::Start() {
	bool ok = true;

	if (!IsRunning()) {
		pthread_t	thread_handle;
		pthread_attr_t	thread_attributes;

		SetStopRequest(false);
		semaphore_.Wait(0.001);	// Reset the internal counter if this is a restart and GraceJoin() wasn't called (or called early).

		pthread_attr_init(&thread_attributes);
		pthread_attr_setdetachstate(&thread_attributes, PTHREAD_CREATE_JOINABLE);

		if(::pthread_create(&thread_handle, &thread_attributes, ThreadEntry, this) == 0) {
			thread_handle_ = (size_t)thread_handle;

			// Wait for newly created thread to kickstart.
			semaphore_.Wait(5.0);
		} else {
			SetRunning(false);
			log_.Errorf(("Failed to start thread "+strutil::Encode(GetThreadName())+"!").c_str());
			ok = false;
		}
	}
	return (ok);
}

bool Thread::Join() {
	SetStopRequest(true);
	if (GetThreadHandle() != 0) {
		deb_assert(GetThreadId() != GetCurrentThreadId());
		::pthread_join((pthread_t)thread_handle_, 0);
		//deb_assert(!IsRunning());
		thread_handle_ = 0;
		thread_id_ = 0;
	}
	SetStopRequest(false);
	return (true);
}

bool Thread::GraceJoin(float64 time_out) {
	if (GetThreadHandle() != 0) {
		deb_assert(GetThreadId() != GetCurrentThreadId());
		if (!semaphore_.Wait(time_out)) {
			// Possible dead lock...
			log_.Warningf(("Failed to timed join thread \"" + GetThreadName() + "\"! Deadlock?").c_str());
			return (false);	// RAII simplifies here.
		}
		Join();
	}
	SetStopRequest(false);
	return (true);
}

void Thread::Signal(int) { // Ignore signal for now.
	if (thread_handle_ != 0) {
		::pthread_kill((pthread_t)thread_handle_, SIGHUP);
	}
}

void Thread::Kill() {
	if (GetThreadHandle() != 0) {
		deb_assert(GetThreadId() != GetCurrentThreadId());
		log_.Warning("Forcing kill of thread " + GetThreadName());
		::pthread_kill((pthread_t)thread_handle_, SIGHUP);
		Join();
		SetRunning(false);
	}
	SetStopRequest(false);
}

void Thread::YieldCpu() {
	::sched_yield();
}

void Thread::PostRun() {
	semaphore_.Signal();
}



loginstance(kGeneral, PosixSemaphore);



}
