
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/LepraAssert.h"
#include "../../Include/Win32/Win32Thread.h"
#include "../../Include/Lepra.h"
#include "../../Include/Log.h"



namespace Lepra
{



static ThreadPointerStorage gThreadStorage;
static ThreadPointerStorage gExtraDataStorage;



ThreadPointerStorage::ThreadPointerStorage()
{
	mTLSIndex = ::TlsAlloc();
}

ThreadPointerStorage::~ThreadPointerStorage()
{
	::TlsFree(mTLSIndex);
}

void ThreadPointerStorage::SetPointer(void* pData)
{
	::TlsSetValue(mTLSIndex, (LPVOID)pData);
}

void* ThreadPointerStorage::GetPointer()
{
	return ::TlsGetValue(mTLSIndex);
}



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
	gThreadStorage.SetPointer(lThread);
	gExtraDataStorage.SetPointer(0);
	deb_assert(gThreadStorage.GetPointer() == lThread);
	deb_assert(Thread::GetCurrentThread() == lThread);
	SetVisualStudioThreadName(lThread->GetThreadName().c_str(), (DWORD)-1);
	RunThread(lThread);
	return 0;
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




Win32Condition::Win32Condition(Win32Lock* pExternalLock):
	mExternalLock(pExternalLock),
	mSemaphore(::CreateSemaphore(0, 0, 0x7fffffff, 0)),
	mWaitThreadCount(0)
{
}

Win32Condition::~Win32Condition()
{
	::CloseHandle(mSemaphore);
}

void Win32Condition::Wait()
{
	::InterlockedIncrement(&mWaitThreadCount);
	mExternalLock->Release();
	::WaitForSingleObject(mSemaphore, INFINITE);
	mExternalLock->Acquire();
	::InterlockedDecrement(&mWaitThreadCount);
}

bool Win32Condition::Wait(float64 pMaxWaitTime)
{
	if (pMaxWaitTime < 0)
	{
		pMaxWaitTime = 0;
	}
	::InterlockedIncrement(&mWaitThreadCount);
	mExternalLock->Release();
	bool lSignalled = (::WaitForSingleObject(mSemaphore, (DWORD)(pMaxWaitTime*1000.0)) == WAIT_OBJECT_0);
	mExternalLock->Acquire();
	::InterlockedDecrement(&mWaitThreadCount);
	return (lSignalled);
}

void Win32Condition::Signal()
{
	if (mWaitThreadCount)
	{
		::ReleaseSemaphore(mSemaphore, 1, 0);
	}
}

void Win32Condition::SignalAll()
{
	if (mWaitThreadCount)
	{
		::ReleaseSemaphore(mSemaphore, mWaitThreadCount, 0);
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
	bool lSignalled = false;
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




Win32RwLock::Win32RwLock():
	mReadCondition(0),
	mWriteCondition(0),
	mNumPendingReaders(0),
	mNumActiveReaders(0),
	mNumPendingWriters(0),
	mIsWriting(false)
{
}

Win32RwLock::~Win32RwLock()
{
}

void Win32RwLock::AcquireRead()
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

void Win32RwLock::AcquireWrite()
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

void Win32RwLock::Release()
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



void Thread::InitializeThread(Thread* pThread)
{
	gThreadStorage.SetPointer(pThread);
	gExtraDataStorage.SetPointer(0);
	pThread->SetThreadId(GetCurrentThreadId());
	deb_assert(gThreadStorage.GetPointer() == pThread);
	deb_assert(Thread::GetCurrentThread() == pThread);
	SetVisualStudioThreadName(pThread->GetThreadName().c_str(), (DWORD)-1);
}

size_t Thread::GetCurrentThreadId()
{
	return (::GetCurrentThreadId());
}

Thread* Thread::GetCurrentThread()
{
	return ((Thread*)gThreadStorage.GetPointer());
}

void* Thread::GetExtraData()
{
	return (gExtraDataStorage.GetPointer());
}

void Thread::SetExtraData(void* pData)
{
	gExtraDataStorage.SetPointer(pData);
}

void Thread::SetCpuAffinityMask(uint64 pAffinityMask)
{
	DWORD_PTR lMask = (DWORD_PTR)pAffinityMask;
	::SetThreadAffinityMask((HANDLE)GetThreadHandle(), lMask);
	// Cut timeslice short and allow OS to reschedule us to the given CPU immediately.
	YieldCpu();
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

		DWORD lThreadId = 0;
		mThreadHandle = (size_t)::CreateThread(0, 0, ThreadEntry, this, 0, &lThreadId);
		if (mThreadHandle)
		{
			// Try to wait for newly created thread.
			mSemaphore.Wait(5.0);
		}
		else
		{
			SetRunning(false);
			mLog.Errorf((_T("Failed to start thread ")+strutil::Encode(GetThreadName())+_T("!")).c_str());
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
		deb_assert(GetThreadId() != GetCurrentThreadId());
		Thread::YieldCpu();	// Try to let thread self destruct.
		::WaitForSingleObject((HANDLE)GetThreadHandle(), INFINITE);
		deb_assert(!IsRunning());
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

bool Thread::GraceJoin(float64 pTimeOut)
{
	if (GetThreadHandle() != 0)
	{
		deb_assert(GetThreadId() != GetCurrentThreadId());
		const astr lThreadNameCopy(GetThreadName());
		if (::WaitForSingleObject((HANDLE)GetThreadHandle(), (DWORD)(pTimeOut * 1000.0)) == WAIT_TIMEOUT)
		{
			// Possible dead lock...
			mLog.Warningf((_T("Failed to join thread \"") + strutil::Encode(lThreadNameCopy) + _T("\"! Deadlock?")).c_str());
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
	return true;
}

void Thread::Signal(int)
{
	// We don't need this on Windows, since nothing really blocks hard.
}

void Thread::Kill()
{
	if (GetThreadHandle() != 0)
	{
		if (GetThreadId() != GetCurrentThreadId())
		{
			mLog.Warning(_T("Forcing kill of thread ") + strutil::Encode(GetThreadName()));
			::TerminateThread((HANDLE)GetThreadHandle(), 0);
			SetRunning(false);
			::CloseHandle((HANDLE)GetThreadHandle());
			mThreadHandle = 0;
		}
		else
		{
			mLog.Warning(_T("Thread ") + strutil::Encode(GetThreadName()) + _T(" terminating self."));
			HANDLE lSelf = (HANDLE)mThreadHandle;
			SetRunning(false);
			delete this;
			::TerminateThread(lSelf, 0);
			::CloseHandle(lSelf);
		}
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



}
