
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include <assert.h>
#include "../../Include/Win32/Win32Thread.h"
#include "../../Include/Lepra.h"
#include "../../Include/Log.h"



namespace Lepra
{



ThreadPointerStorage ThreadPointerStorage::smTPS;



ThreadPointerStorage::ThreadPointerStorage()
{
	mTLSIndex = ::TlsAlloc();
}

ThreadPointerStorage::~ThreadPointerStorage()
{
	::TlsFree(mTLSIndex);
}

void ThreadPointerStorage::SetPointer(Thread* pThread)
{
	::TlsSetValue(smTPS.mTLSIndex, (LPVOID)pThread);
}

Thread* ThreadPointerStorage::GetPointer()
{
	return (Thread*)::TlsGetValue(smTPS.mTLSIndex);
}



StaticThread gMainThread(_T("MainThread"));

// This gives the thread a name when debugging in VS.
void SetVisualStudioThreadName(const char* szThreadName, DWORD dwThreadId)
{
#pragma pack(push,8)
	struct THREADNAME_INFO
	{
		DWORD dwType;		// must be 0x1000
		LPCSTR szName;		// pointer to name (in user addr space)
		DWORD dwThreadId;	// thread Id (-1=caller thread)
		DWORD dwFlags;		// reserved for future use, must be zero
	};
#pragma pack(pop)
	THREADNAME_INFO lThreadNameInfo;
	lThreadNameInfo.dwType = 0x1000;
	lThreadNameInfo.szName = szThreadName;
	lThreadNameInfo.dwThreadId = dwThreadId;
	lThreadNameInfo.dwFlags = 0;
	__try
	{
		RaiseException(0x406D1388, 0, sizeof(lThreadNameInfo)/sizeof(DWORD), (DWORD*)&lThreadNameInfo);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

// This is where the thread starts. A global standard C-function.
DWORD __stdcall ThreadEntry(void* pThread)
{
	Thread* lThread = (Thread*)pThread;
	ThreadPointerStorage::SetPointer(lThread);
	assert(ThreadPointerStorage::GetPointer() == lThread);
	assert(Thread::GetCurrentThread() == lThread);
	SetVisualStudioThreadName(AnsiStringUtility::ToOwnCode(lThread->GetThreadName()).c_str(), (DWORD)-1);
	RunThread(lThread);
	return 0;
}

void Thread::InitializeMainThread(const String& pThreadName)
{
	ThreadPointerStorage::SetPointer(&gMainThread);
	assert(ThreadPointerStorage::GetPointer() == &gMainThread);
	assert(Thread::GetCurrentThread() == &gMainThread);
	SetVisualStudioThreadName(AnsiStringUtility::ToOwnCode(pThreadName).c_str(), (DWORD)-1);
}

size_t Thread::GetCurrentThreadId()
{
	return (::GetCurrentThreadId());
}

Thread* Thread::GetCurrentThread()
{
	return ThreadPointerStorage::GetPointer();
}

void Thread::SetCpuAffinityMask(uint64 pAffinityMask)
{
	DWORD_PTR lMask = (DWORD_PTR)pAffinityMask;
	::SetThreadAffinityMask((HANDLE)GetThreadHandle(), lMask);
	// Cut timeslice short and allow OS to reschedule us to the given CPU immediately.
	::Sleep(1);
}

void Thread::Sleep(unsigned int pMilliSeconds)
{
	::Sleep(pMilliSeconds);
}

bool Thread::Start()
{
	bool lOk = true;

	if (!IsRunning())
	{
		SetStopRequest(false);

		DWORD lThreadId;
		mThreadHandle = (size_t)::CreateThread(0, 0, ThreadEntry, this, 0, &lThreadId);
		if (mThreadHandle)
		{
			mThreadId = lThreadId;
			// Try to wait for newly created thread.
			for (int x = 0; !IsRunning() && x < 1000; ++x)
			{
				Thread::YieldCpu();
			}
		}
		else
		{
			SetRunning(false);
			mLog.Errorf((_T("Failed to start thread ")+GetThreadName()+_T("!")).c_str());
			lOk = false;
		}
	}
	return (lOk);
}

bool Thread::Join()
{
	bool lReturnValue = true;

	SetStopRequest(true);
	if (GetThreadHandle() != 0)
	{
		assert(GetThreadId() != GetCurrentThreadId());
		Thread::YieldCpu();	// Try to let thread self destruct.
		::WaitForSingleObject((HANDLE)GetThreadHandle(), INFINITE);
		assert(!IsRunning());
		if (GetThreadHandle())
		{
			::CloseHandle((HANDLE)GetThreadHandle());
		}
		mThreadHandle = 0;
		mThreadId = 0;
	}
	SetStopRequest(false);

	return (lReturnValue);
}

bool Thread::Join(float64 pTimeOut)
{
	bool lReturnValue = true;

	SetStopRequest(true);
	if (GetThreadHandle() != 0)
	{
		assert(GetThreadId() != GetCurrentThreadId());
		if (::WaitForSingleObject((HANDLE)GetThreadHandle(), (DWORD)(pTimeOut * 1000.0)) == WAIT_TIMEOUT)
		{
			// Possible dead lock...
			mLog.Warningf((_T("Failed to join thread \"") + GetThreadName() + _T("\"! Deadlock?")).c_str());
			return (false);	// RAII simplifies here.
		}
		if (GetThreadHandle())
		{
			::CloseHandle((HANDLE)GetThreadHandle());
		}
		mThreadHandle = 0;
		mThreadId = 0;
	}
	SetStopRequest(false);

	return lReturnValue;
}

void Thread::Kill()
{
	if (GetThreadHandle() != 0)
	{
		assert(GetThreadId() != GetCurrentThreadId());
		mLog.Warningf(_T("Forcing kill of thread %s."), GetThreadName().c_str());
		::TerminateThread((HANDLE)GetThreadHandle(), 0);
		SetRunning(false);
		::CloseHandle((HANDLE)GetThreadHandle());
		mThreadHandle = 0;
	}
	SetStopRequest(false);
}

void Thread::YieldCpu()
{
	::SwitchToThread();
}

void Thread::PostRun()
{
}





Win32Lock::Win32Lock()
{
	::InitializeCriticalSection(&mMutex);
}

Win32Lock::~Win32Lock()
{
	::DeleteCriticalSection(&mMutex);
}

void Win32Lock::Acquire()
{
	::EnterCriticalSection(&mMutex);
}

bool Win32Lock::TryAcquire()
{
	return (::TryEnterCriticalSection(&mMutex) == TRUE);
}

void Win32Lock::Release()
{
	::LeaveCriticalSection(&mMutex);
}


Win32CompatibleLock::Win32CompatibleLock() :
	mMutex(::CreateMutex(NULL, FALSE, NULL)),
	mLockCount(0)
{
}

Win32CompatibleLock::~Win32CompatibleLock()
{
	::CloseHandle(mMutex);
}

void Win32CompatibleLock::Acquire()
{
	::WaitForSingleObject(mMutex, INFINITE);
	mLockCount++;
}

bool Win32CompatibleLock::Acquire(float64 pMaxWaitTime)
{
	bool lSuccess = (::WaitForSingleObject(mMutex, (DWORD)(pMaxWaitTime * 1000.0)) == WAIT_OBJECT_0);

	if (lSuccess == true)
	{
		mLockCount++;
	}

	return lSuccess;
}

bool Win32CompatibleLock::TryAcquire()
{
	bool lReturnValue = false;

	if (::WaitForSingleObject(mMutex, 0) == WAIT_OBJECT_0)
	{
		mLockCount++;
		lReturnValue = true;
	}

	return lReturnValue;
}

void Win32CompatibleLock::Release()
{
	mLockCount--;

	if (mLockCount < 0)
	{
		// One release too much. This is harmless at the moment, but means that the 
		// number of calls to Release() don't match the number of calls to Lock().
		mLockCount = 0;
	}
	else
	{
		::ReleaseMutex(mMutex);
	}
}








Win32Condition::Win32Condition():
	mSemaphore(::CreateSemaphore(0, 0, 0x7fffffff, 0)),
	mWaitThreadCount(0)
{
	::InitializeCriticalSection(&mSignalLock);
}

Win32Condition::~Win32Condition()
{
	::CloseHandle(mSemaphore);
}

void Win32Condition::Wait()
{
	::InterlockedIncrement(&mWaitThreadCount);
	::WaitForSingleObject(mSemaphore, INFINITE);
	::InterlockedDecrement(&mWaitThreadCount);
}

bool Win32Condition::Wait(float64 pMaxWaitTime)
{
	if (pMaxWaitTime < 0)
	{
		pMaxWaitTime = 0;
	}
	::InterlockedIncrement(&mWaitThreadCount);
	bool lSignalled = (::WaitForSingleObject(mSemaphore, (DWORD)(pMaxWaitTime*1000.0)) == WAIT_OBJECT_0);
	::InterlockedDecrement(&mWaitThreadCount);
	return (lSignalled);
}

void Win32Condition::Signal()
{
	if (mWaitThreadCount)
	{
		::EnterCriticalSection(&mSignalLock);
		if (mWaitThreadCount)	// Double check (required after lock).
		{
			::ReleaseSemaphore(mSemaphore, 1, 0);
		}
		::LeaveCriticalSection(&mSignalLock);
	}
}

void Win32Condition::SignalAll()
{
	if (mWaitThreadCount)
	{
		::EnterCriticalSection(&mSignalLock);
		::ReleaseSemaphore(mSemaphore, mWaitThreadCount, 0);
		::LeaveCriticalSection(&mSignalLock);
	}
}



Win32CompatibleCondition::Win32CompatibleCondition(Win32CompatibleLock* pExternalLock) :
	CompatibleConditionBC(pExternalLock)
{
	mNumWaiters   = 0;
	mWasBroadcast = false;

	::InitializeCriticalSection(&mNumWaitersLock);

	mSemaphore = ::CreateSemaphore(NULL,       // No security.
									0,          // Initially 0.
									0x7fffffff, // Max count.
									NULL);      // Unnamed.

	mWaitersDoneEvent = ::CreateEvent(NULL,    // No security.
									   FALSE,   // Auto-reset.
									   FALSE,   // Non-signaled initially.
									   NULL);   // Unnamed.

	mMutex = ::CreateMutex(NULL, FALSE, NULL);
}

Win32CompatibleCondition::~Win32CompatibleCondition()
{
	::DeleteCriticalSection(&mNumWaitersLock);
	::CloseHandle(mSemaphore);
	::CloseHandle(mWaitersDoneEvent);
	::CloseHandle(mMutex);
}

void Win32CompatibleCondition::Wait()
{
	HANDLE lMutex = mMutex;
	if (mExternalLock != 0)
	{
		lMutex = ((Win32CompatibleLock*)mExternalLock)->mMutex;
	}
	else
	{
		::WaitForSingleObject(lMutex, INFINITE);
	}

	// Avoid race conditions.
	::EnterCriticalSection(&mNumWaitersLock);
	mNumWaiters++;
	::LeaveCriticalSection(&mNumWaitersLock);


	// This call atomically releases the mutex and waits on the
	// semaphore until Signal() or Broadcast() are called by another 
	// thread.
	::SignalObjectAndWait(lMutex, mSemaphore, INFINITE, FALSE);


	// We're no longer waiting...
	// Reacquire lock to avoid race conditions.
	::EnterCriticalSection(&mNumWaitersLock);
	mNumWaiters--;
	// Check to see if we're the last waiter after Broadcast().
	bool lLastWaiter = (mWasBroadcast == true) && (mNumWaiters == 0);
	::LeaveCriticalSection(&mNumWaitersLock);


	// If we're the last waiter thread during this particular broadcast
	// then let all the other threads proceed.
	if (lLastWaiter == true)
	{
		// This call atomically signals the mWaitersDoneEvent and waits until
		// it can acquire the external mutex.  This is required to ensure fairness.
		::SignalObjectAndWait(mWaitersDoneEvent, lMutex, INFINITE, FALSE);
	}
	else
	{
		// Always regain the external mutex since that's the guarantee we
		// give to our callers. 
		::WaitForSingleObject(lMutex, INFINITE);
	}

	if (mExternalLock == 0)
	{
		::ReleaseMutex(lMutex);
	}
}

bool Win32CompatibleCondition::Wait(float64 pMaxWaitTime)
{
	HANDLE lMutex = mMutex;
	if (mExternalLock != 0)
	{
		lMutex = ((Win32CompatibleLock*)mExternalLock)->mMutex;
	}
	else
	{
		::WaitForSingleObject(lMutex, INFINITE);
	}

	// Avoid race conditions.
	::EnterCriticalSection(&mNumWaitersLock);
	mNumWaiters++;
	::LeaveCriticalSection(&mNumWaitersLock);

	bool lSignaled = false;

	// This call atomically releases the mutex and waits on the
	// semaphore until Signal() or Broadcast() are called by another 
	// thread.
	lSignaled = (::SignalObjectAndWait(lMutex, mSemaphore, (DWORD)(pMaxWaitTime * 1000.0), FALSE) == WAIT_OBJECT_0);

	// We're no longer waiting...
	// Reacquire lock to avoid race conditions.
	::EnterCriticalSection(&mNumWaitersLock);
	mNumWaiters--;
	// Check to see if we're the last waiter after Broadcast().
	bool lLastWaiter = (lSignaled == true) && (mWasBroadcast == true) && (mNumWaiters == 0);
	::LeaveCriticalSection(&mNumWaitersLock);


	// If we're the last waiter thread during this particular broadcast
	// then let all the other threads proceed.
	if (lLastWaiter == true)
	{
		// This call atomically signals the mWaitersDoneEvent and waits until
		// it can acquire the external mutex.  This is required to ensure fairness.
		::SignalObjectAndWait(mWaitersDoneEvent, lMutex, INFINITE, FALSE);
	}
	else
	{
		// Always regain the external mutex since that's the guarantee we
		// give to our callers. 
		::WaitForSingleObject(lMutex, INFINITE);
	}

	if (mExternalLock == 0)
	{
		::ReleaseMutex(lMutex);
	}

	return lSignaled;
}

void Win32CompatibleCondition::Signal()
{
	HANDLE lMutex = mMutex;
	if (mExternalLock != 0)
	{
		lMutex = ((Win32CompatibleLock*)mExternalLock)->mMutex;
	}
	else
	{
		::WaitForSingleObject(lMutex, INFINITE);
	}

	::EnterCriticalSection(&mNumWaitersLock);
	bool lHaveWaiters = (mNumWaiters > 0);
	::LeaveCriticalSection(&mNumWaitersLock);

	// If there aren't any waiters, then this is a no-op.
	if (lHaveWaiters == true)
	{
		::ReleaseSemaphore(mSemaphore, 1, 0);
	}

	if (mExternalLock == 0)
	{
		::ReleaseMutex(lMutex);
	}
}

void Win32CompatibleCondition::SignalAll()
{
	HANDLE lMutex = mMutex;
	if (mExternalLock != 0)
	{
		lMutex = ((Win32CompatibleLock*)mExternalLock)->mMutex;
	}
	else
	{
		::WaitForSingleObject(lMutex, INFINITE);
	}

	// This is needed to ensure that <waiters_count_> and <was_broadcast_> are
	// consistent relative to each other.
	::EnterCriticalSection(&mNumWaitersLock);
	bool lHaveWaiters = false;

	if (mNumWaiters > 0) 
	{
		// We are broadcasting, even if there is just one waiter...
		// Record that we are broadcasting, which helps optimize
		// Wait() for the non-broadcast case.
		mWasBroadcast = true;
		lHaveWaiters = true;
	}

	if (lHaveWaiters == true)
	{
		// Wake up all the waiters atomically.
		::ReleaseSemaphore(mSemaphore, mNumWaiters, 0);
		::LeaveCriticalSection(&mNumWaitersLock);

		// Wait for all the awakened threads to acquire the counting semaphore. 
		::WaitForSingleObject(mWaitersDoneEvent, INFINITE);

		// This assignment is okay, even without the mNumWaitersLock held 
		// because no other waiter threads can wake up to access it.
		mWasBroadcast = false;
	}
	else
	{
		::LeaveCriticalSection(&mNumWaitersLock);
	}

	if (mExternalLock == 0)
	{
		::ReleaseMutex(lMutex);
	}
}












Win32Semaphore::Win32Semaphore()
{
	mSemaphore = ::CreateSemaphore(0, 0, 10000, 0);
}

Win32Semaphore::Win32Semaphore(unsigned pMaxCount)
{
	mSemaphore = ::CreateSemaphore(0, 0, pMaxCount, 0);
}

Win32Semaphore::~Win32Semaphore()
{
	::CloseHandle(mSemaphore);
	mSemaphore = 0;
}

void Win32Semaphore::Wait()
{
	::WaitForSingleObject(mSemaphore, INFINITE);
}

bool Win32Semaphore::Wait(float64 pMaxWaitTime)
{
	bool lSignalled = true;
	if (pMaxWaitTime > 0)
	{
		lSignalled = (::WaitForSingleObject(mSemaphore, (DWORD)(pMaxWaitTime*1000.0)) == WAIT_OBJECT_0);
	}
	return (lSignalled);
}

void Win32Semaphore::Signal()
{
	::ReleaseSemaphore(mSemaphore, 1, 0);
}


/*Win32CompatibleSemaphore::Win32CompatibleSemaphore(int pCount):
	mCount(pCount),
	mNumWaiters(0),
	mCountNonZero(&mLock)
{
}

Win32CompatibleSemaphore::~Win32CompatibleSemaphore()
{
}

void Win32CompatibleSemaphore::Acquire(int pNumPermits)
{
	mLock.Acquire();

	// Keep track of the number of waiters so that Release() works correctly.
	mNumWaiters++;

	// Wait until the semaphore count is > 0, then atomically release
	// mLock and wait for mCountNonZero to be signaled. 
	while (mCount <= 0)
	{
		mCountNonZero.Wait();
	}

	// mLock is now held.

	// Decrement the waiters count.
	mNumWaiters--;

	// Decrement the semaphore's count.
	mCount -= pNumPermits;

	// Release mutex to leave critical section.
	mLock.Release();
}

bool Win32CompatibleSemaphore::Acquire(float64 pMaxWaitTime, int pNumPermits)
{
	if (mLock.Acquire(pMaxWaitTime) == false)
	{
		return false;
	}

	// If time is zero, there's no need to wait for the condition.
	if (pMaxWaitTime == 0.0 && mCount == 0)
	{
		mLock.Release();
		return false;
	}

	// Keep track of the number of waiters so that Release() works correctly.
	mNumWaiters++;

	// Wait until the semaphore count is > 0, then atomically release
	// mLock and wait for mCountNonZero to be signaled. 
	while (mCount <= 0)
	{
		if (mCountNonZero.Wait(pMaxWaitTime) == false)
		{
			return false;
		}
	}

	// mLock is now held.

	// Decrement the waiters count.
	mNumWaiters--;

	// Decrement the semaphore's count.
	mCount -= pNumPermits;

	// Release mutex to leave critical section.
	mLock.Release();

	return true;
}

bool Win32CompatibleSemaphore::TryAcquire(int pNumPermits)
{
	return Acquire(0, pNumPermits);
}

void Win32CompatibleSemaphore::Drain()
{
	mLock.Acquire();

	// If count is less than or equal to zero, the semaphore is 
	// already drained.
	if (mCount > 0)
	{
		// Drain the semaphore.
		mCount = 0;
	}

	// Release mutex to leave critical section.
	mLock.Release();
}

void Win32CompatibleSemaphore::Release(int pNumPermits)
{
	mLock.Acquire();

	// Increment the semaphore's count.
	mCount += pNumPermits;

	if (mCount > 0 && mNumWaiters > 0)
	{
		mCountNonZero.SignalAll();
	}

	mLock.Release();
}*/










Win32RWLock::Win32RWLock(const String& pRWLockName) :
	RWLockBC(pRWLockName),
	mReadCondition(&mWriteLock),
	mWriteCondition(&mWriteLock),
	mNumPendingReaders(0),
	mNumActiveReaders(0),
	mNumPendingWriters(0),
	mIsWriting(false)
{

}

Win32RWLock::~Win32RWLock()
{
}

void Win32RWLock::AcquireRead()
{
	// If someone's writing, we will sit tight here.
	mWriteLock.Acquire();

	// Wait until there are no more pending writers.
	// (Writing has higher priority than reading).
	while (mNumPendingWriters > 0 || mIsWriting == true)
	{
		mNumPendingReaders++;
		mReadCondition.Wait();	// Wait until the writer says GO!
		mNumPendingReaders--;
	}

	mNumActiveReaders++;
	mWriteLock.Release();
}

void Win32RWLock::AcquireWrite()
{
	// If someone else is writing, we will sit tight here.
	mWriteLock.Acquire();

	// Wait until all active readers are done.
	while (mNumActiveReaders > 0)
	{
		mNumPendingWriters++;
		mWriteCondition.Wait(); // Wait until the reader says GO!
		mNumPendingWriters--;
	}

	mIsWriting = true;
}

void Win32RWLock::Release()
{
	if (mIsWriting == true)
	{
		// The current thread has been writing, and the mutex is locked.
		mIsWriting = false;

		// If there are any other writers waiting, let them in.
		if (mNumPendingWriters > 0)
		{
			mWriteCondition.Signal();
			mWriteLock.Release();
		}
		else
		{
			// ...otherwise let the readers in.
			mReadCondition.SignalAll();
			mWriteLock.Release();
		}
	}
	else
	{
		// The current thread has been reading...
		mWriteLock.Acquire();
		
		if (mNumActiveReaders > 0)
		{
			mNumActiveReaders--;

			if (mNumActiveReaders == 0)
			{
				// Let the next writer in...
				mWriteCondition.Signal();
			}
		}

		mWriteLock.Release();
	}
}



}
