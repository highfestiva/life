
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/lepraassert.h"
#include "../../include/win32/win32thread.h"
#include "../../include/lepra.h"
#include "../../include/log.h"



namespace lepra {



static ThreadPointerStorage g_thread_storage;
static ThreadPointerStorage g_extra_data_storage;



ThreadPointerStorage::ThreadPointerStorage() {
	tls_index_ = ::TlsAlloc();
}

ThreadPointerStorage::~ThreadPointerStorage() {
	::TlsFree(tls_index_);
}

void ThreadPointerStorage::SetPointer(void* data) {
	::TlsSetValue(tls_index_, (LPVOID)data);
}

void* ThreadPointerStorage::GetPointer() {
	return ::TlsGetValue(tls_index_);
}



// This gives the thread a name when debugging in VS.
void SetVisualStudioThreadName(const char* thread_name_, DWORD dwThreadId) {
#pragma pack(push,8)
	struct THREADNAME_INFO {
		DWORD dwType;		// must be 0x1000
		LPCSTR name_;		// pointer to name (in user addr space)
		DWORD dwThreadId;	// thread Id (-1=caller thread)
		DWORD dwFlags;		// reserved for future use, must be zero
	};
#pragma pack(pop)
	THREADNAME_INFO thread_name_info;
	thread_name_info.dwType = 0x1000;
	thread_name_info.name_ = thread_name_;
	thread_name_info.dwThreadId = dwThreadId;
	thread_name_info.dwFlags = 0;
	__try {
		RaiseException(0x406D1388, 0, sizeof(thread_name_info)/sizeof(DWORD), (DWORD*)&thread_name_info);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION) {
	}
}

// This is where the thread starts. A global standard C-function.
DWORD __stdcall ThreadEntry(void* thread) {
	Thread* _thread = (Thread*)thread;
	g_thread_storage.SetPointer(_thread);
	g_extra_data_storage.SetPointer(0);
	deb_assert(g_thread_storage.GetPointer() == _thread);
	deb_assert(Thread::GetCurrentThread() == _thread);
	SetVisualStudioThreadName(_thread->GetThreadName().c_str(), (DWORD)-1);
	RunThread(_thread);
	return 0;
}



Win32Lock::Win32Lock() {
	::InitializeCriticalSection(&mutex_);
}

Win32Lock::~Win32Lock() {
	::DeleteCriticalSection(&mutex_);
}

void Win32Lock::Acquire() {
	::EnterCriticalSection(&mutex_);
}

bool Win32Lock::TryAcquire() {
	return (::TryEnterCriticalSection(&mutex_) == TRUE);
}

void Win32Lock::Release() {
	::LeaveCriticalSection(&mutex_);
}




Win32Condition::Win32Condition(Win32Lock* external_lock):
	external_lock_(external_lock),
	semaphore_(::CreateSemaphore(0, 0, 0x7fffffff, 0)),
	wait_thread_count_(0) {
}

Win32Condition::~Win32Condition() {
	::CloseHandle(semaphore_);
}

void Win32Condition::Wait() {
	::InterlockedIncrement(&wait_thread_count_);
	external_lock_->Release();
	::WaitForSingleObject(semaphore_, INFINITE);
	external_lock_->Acquire();
	::InterlockedDecrement(&wait_thread_count_);
}

bool Win32Condition::Wait(float64 max_wait_time) {
	if (max_wait_time < 0) {
		max_wait_time = 0;
	}
	::InterlockedIncrement(&wait_thread_count_);
	external_lock_->Release();
	bool signalled = (::WaitForSingleObject(semaphore_, (DWORD)(max_wait_time*1000.0)) == WAIT_OBJECT_0);
	external_lock_->Acquire();
	::InterlockedDecrement(&wait_thread_count_);
	return (signalled);
}

void Win32Condition::Signal() {
	if (wait_thread_count_) {
		::ReleaseSemaphore(semaphore_, 1, 0);
	}
}

void Win32Condition::SignalAll() {
	if (wait_thread_count_) {
		::ReleaseSemaphore(semaphore_, wait_thread_count_, 0);
	}
}



Win32Semaphore::Win32Semaphore() {
	semaphore_ = ::CreateSemaphore(0, 0, 10000, 0);
}

Win32Semaphore::Win32Semaphore(unsigned max_count) {
	semaphore_ = ::CreateSemaphore(0, 0, max_count, 0);
}

Win32Semaphore::~Win32Semaphore() {
	::CloseHandle(semaphore_);
	semaphore_ = 0;
}

void Win32Semaphore::Wait() {
	::WaitForSingleObject(semaphore_, INFINITE);
}

bool Win32Semaphore::Wait(float64 max_wait_time) {
	bool signalled = false;
	if (max_wait_time > 0) {
		signalled = (::WaitForSingleObject(semaphore_, (DWORD)(max_wait_time*1000.0)) == WAIT_OBJECT_0);
	}
	return (signalled);
}

void Win32Semaphore::Signal() {
	::ReleaseSemaphore(semaphore_, 1, 0);
}




Win32RwLock::Win32RwLock():
	read_condition_(0),
	write_condition_(0),
	num_pending_readers_(0),
	num_active_readers_(0),
	num_pending_writers_(0),
	is_writing_(false) {
}

Win32RwLock::~Win32RwLock() {
}

void Win32RwLock::AcquireRead() {
	// If someone's writing, we will sit tight here.
	write_lock_.Acquire();

	// Wait until there are no more pending writers.
	// (Writing has higher priority than reading).
	while (num_pending_writers_ > 0 || is_writing_ == true) {
		num_pending_readers_++;
		read_condition_.Wait();	// Wait until the writer says GO!
		num_pending_readers_--;
	}

	num_active_readers_++;
	write_lock_.Release();
}

void Win32RwLock::AcquireWrite() {
	// If someone else is writing, we will sit tight here.
	write_lock_.Acquire();

	// Wait until all active readers are done.
	while (num_active_readers_ > 0) {
		num_pending_writers_++;
		write_condition_.Wait(); // Wait until the reader says GO!
		num_pending_writers_--;
	}

	is_writing_ = true;
}

void Win32RwLock::Release() {
	if (is_writing_ == true) {
		// The current thread has been writing, and the mutex is locked.
		is_writing_ = false;

		// If there are any other writers waiting, let them in.
		if (num_pending_writers_ > 0) {
			write_condition_.Signal();
			write_lock_.Release();
		} else {
			// ...otherwise let the readers in.
			read_condition_.SignalAll();
			write_lock_.Release();
		}
	} else {
		// The current thread has been reading...
		write_lock_.Acquire();

		if (num_active_readers_ > 0) {
			num_active_readers_--;

			if (num_active_readers_ == 0) {
				// Let the next writer in...
				write_condition_.Signal();
			}
		}

		write_lock_.Release();
	}
}



void Thread::InitializeThread(Thread* thread) {
	g_thread_storage.SetPointer(thread);
	g_extra_data_storage.SetPointer(0);
	thread->SetThreadId(GetCurrentThreadId());
	deb_assert(g_thread_storage.GetPointer() == thread);
	deb_assert(Thread::GetCurrentThread() == thread);
	SetVisualStudioThreadName(thread->GetThreadName().c_str(), (DWORD)-1);
}

size_t Thread::GetCurrentThreadId() {
	return (::GetCurrentThreadId());
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

void Thread::SetCpuAffinityMask(uint64 affinity_mask) {
	DWORD_PTR mask = (DWORD_PTR)affinity_mask;
	::SetThreadAffinityMask((HANDLE)GetThreadHandle(), mask);
	// Cut timeslice short and allow OS to reschedule us to the given CPU immediately.
	YieldCpu();
}

void Thread::Sleep(unsigned int micro_seconds) {
	::Sleep(micro_seconds/1000);
}

bool Thread::Start() {
	bool ok = true;

	if (!IsRunning()) {
		SetStopRequest(false);

		DWORD thread_id = 0;
		thread_handle_ = (size_t)::CreateThread(0, 0, ThreadEntry, this, 0, &thread_id);
		if (thread_handle_) {
			// Try to wait for newly created thread.
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
	bool return_value = true;

	SetStopRequest(true);
	if (GetThreadHandle() != 0) {
		deb_assert(GetThreadId() != GetCurrentThreadId());
		Thread::YieldCpu();	// Try to let thread self destruct.
		::WaitForSingleObject((HANDLE)GetThreadHandle(), INFINITE);
		deb_assert(!IsRunning());
		if (GetThreadHandle()) {
			::CloseHandle((HANDLE)GetThreadHandle());
		}
		thread_handle_ = 0;
		thread_id_ = 0;
	}
	SetStopRequest(false);

	return (return_value);
}

bool Thread::GraceJoin(float64 time_out) {
	if (GetThreadHandle() != 0) {
		deb_assert(GetThreadId() != GetCurrentThreadId());
		const str thread_name_copy(GetThreadName());
		if (::WaitForSingleObject((HANDLE)GetThreadHandle(), (DWORD)(time_out * 1000.0)) == WAIT_TIMEOUT) {
			// Possible dead lock...
			log_.Warningf(("Failed to join thread \"" + thread_name_copy + "\"! Deadlock?").c_str());
			return (false);	// RAII simplifies here.
		}
		if (GetThreadHandle()) {
			::CloseHandle((HANDLE)GetThreadHandle());
		}
		thread_handle_ = 0;
		thread_id_ = 0;
	}
	SetStopRequest(false);
	return true;
}

void Thread::Signal(int) {
	// We don't need this on Windows, since nothing really blocks hard.
}

void Thread::Kill() {
	if (GetThreadHandle() != 0) {
		if (GetThreadId() != GetCurrentThreadId()) {
			log_.Warning("Forcing kill of thread " + GetThreadName());
			::TerminateThread((HANDLE)GetThreadHandle(), 0);
			SetRunning(false);
			::CloseHandle((HANDLE)GetThreadHandle());
			thread_handle_ = 0;
		} else {
			log_.Warning("Thread " + GetThreadName() + " terminating self.");
			HANDLE self = (HANDLE)thread_handle_;
			SetRunning(false);
			delete this;
			::TerminateThread(self, 0);
			::CloseHandle(self);
		}
	}
	SetStopRequest(false);
}

void Thread::YieldCpu() {
	::SwitchToThread();
}

void Thread::PostRun() {
}



}
