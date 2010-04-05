/*
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include <assert.h>
#include <../src/stlport_prefix.h>
#include <stl/_threads.h>
#include "../Include/Timer.h"
#include "../Include/HiResTimer.h"
#include "../Include/Log.h"
#include "../Include/MemberThread.h"
#include "../Include/Socket.h"
#include "../Include/SpinLock.h"

using namespace Lepra;

void ReportTestResult(const Lepra::LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult);

const int STATE_ALLOC_INIT = 0x70010210;
const int STATE_ALLOC_FOUND = 0x70111091;

volatile int gThreadTestCounter;

Lock gThreadTestLock;
Semaphore gThreadTestSemaphore;
std::_STLP_mutex gStlLock;
int* gAllocMem = 0;

void IncreaseThread(void*)
{
	++gThreadTestCounter;
}

void LockThread(void*)
{
	gThreadTestCounter = 0;
	gThreadTestLock.Acquire();
	++gThreadTestCounter;
	gThreadTestLock.Release();
	++gThreadTestCounter;
	gThreadTestSemaphore.Wait();
	++gThreadTestCounter;
	gThreadTestSemaphore.Signal();
	++gThreadTestCounter;
	gThreadTestSemaphore.Wait();
	gThreadTestSemaphore.Wait();
	++gThreadTestCounter;
	gThreadTestSemaphore.Signal();
	++gThreadTestCounter;
	while (!Thread::GetCurrentThread()->GetStopRequest())
	{
		Thread::Sleep(0.001);
	}
	++gThreadTestCounter;
}

void StlThreadEntry(void*)
{
	++gThreadTestCounter;
	gStlLock._M_acquire_lock();
	++gThreadTestCounter;
	gStlLock._M_release_lock();
	++gThreadTestCounter;
}

void MemAllocThreadEntry(void*)
{
	assert(gAllocMem == 0);
	gAllocMem = new int[3];
	gAllocMem[1] = STATE_ALLOC_INIT;
}

void MemUseThreadEntry(void*)
{
	while (gAllocMem == 0)
		;
	while (gAllocMem[1] != STATE_ALLOC_FOUND)
		;
	delete[] (gAllocMem);
	gAllocMem = 0;
}

class MemberThreadTestClass
{
public:
	void OnTest()
	{
		gThreadTestCounter = -4;
	}
};

class ThreadIdTestClass
{
public:
	size_t mThreadId;
	void StoreId()
	{
		mThreadId = Thread::GetCurrentThreadId();
	}
};

bool TestThreading(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = _T("thread start");
		for (unsigned x = 0; x < 100 && lTestOk; ++x)
		{
			StaticThread lThread(_T("StartupTest"));
			gThreadTestCounter = 7;
			lTestOk = lThread.Start(IncreaseThread, 0);
			assert(lTestOk);
			if (lTestOk)
			{
				lContext = _T("thread run and self termination");
				//Thread::Sleep(0.001);	// Make sure we sleep at least once.
				for (unsigned y = 0; y < 200 && lThread.IsRunning(); ++y)
				{
					Thread::Sleep(0.001);
				}
				lTestOk = (gThreadTestCounter == 8);
				assert(lTestOk);
				if (lTestOk)
				{
					lTestOk = !lThread.IsRunning();
					assert(lTestOk);
				}
			}
		}
	}

	if (lTestOk)
	{
		lContext = _T("mutex ownership");
		lTestOk = (gThreadTestLock.IsOwner() == false);
		if (lTestOk)
		{
			gThreadTestLock.Acquire();
			lTestOk = (gThreadTestLock.IsOwner() == true);
		}
		if (lTestOk)
		{
			gThreadTestLock.Release();
			lTestOk = (gThreadTestLock.IsOwner() == false);
		}
		assert(lTestOk);
	}

	StaticThread lThread(_T("LockTest"));
	if (lTestOk)
	{
		lContext = _T("mutex exclusiveness");
		gThreadTestCounter = -1;
		gThreadTestLock.Acquire();
		lTestOk = lThread.Start(LockThread, 0);
		if (lTestOk)
		{
			lTestOk = lThread.IsRunning();
			assert(lTestOk);
		}
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 0 && lThread.IsRunning());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("spin lock");
		SpinLock lLock;
		lTestOk = lLock.TryAcquire();
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("semaphore exclusiveness");
		gThreadTestSemaphore.Signal();
		gThreadTestSemaphore.Wait();
		gThreadTestLock.Release();
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 2 && lThread.IsRunning());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("semaphore wait timeout - 1");
		HiResTimer lTimer;
		lTestOk = !gThreadTestSemaphore.Wait(0.5);
		assert(lTestOk);
		lTimer.UpdateTimer();
		if (lTestOk)
		{
			lTestOk = Math::IsInRange(lTimer.GetTimeDiff(), 0.4, 0.9);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (gThreadTestCounter == 2 && lThread.IsRunning());
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = !gThreadTestSemaphore.Wait(0.1);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			gThreadTestSemaphore.Signal();
			Thread::Sleep(0.1);
			lTestOk = !gThreadTestSemaphore.Wait(0.1);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (gThreadTestCounter == 4 && lThread.IsRunning());
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("thread operation");
		gThreadTestSemaphore.Signal();
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 6 && lThread.IsRunning());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("thread grace termination");
		lThread.RequestStop();
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 7 && !lThread.IsRunning());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("MemberThread");
		{
			MemberThreadTestClass lTestClass;
			MemberThread<MemberThreadTestClass> lThread(_T("TestMemberThread"));
			gThreadTestCounter = -5;
			lTestOk = lThread.Start(&lTestClass, &MemberThreadTestClass::OnTest);
			assert(lTestOk);
			if (lTestOk)
			{
				lTestOk = lThread.Join();
				assert(lTestOk);
			}
		}
		if (lTestOk)
		{
			lTestOk = (gThreadTestCounter == -4);
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("main thread ID");
		lTestOk = (Lepra::Thread::GetCurrentThreadId() != 0);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("sub thread ID");
		ThreadIdTestClass lTestInstance;
		lTestInstance.mThreadId = 0;
		MemberThread<ThreadIdTestClass> lThread(_T("TestThreadId"));
		if (lTestOk)
		{
			lTestOk = lThread.Start(&lTestInstance, &ThreadIdTestClass::StoreId);
			assert(lTestOk);
		}
		size_t lActualThreadId = lThread.GetThreadId();
		if (lTestOk)
		{
			lTestOk = lThread.Join();
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (lTestInstance.mThreadId != 0 && lTestInstance.mThreadId == lActualThreadId);
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("semaphore wait timeout");
		Lepra::Timer lTimer;
		Lepra::Semaphore lSemaphore;
		lTestOk = !lSemaphore.Wait(1.0);
		assert(lTestOk);
		if (lTestOk)
		{
			double lTime = lTimer.PopTimeDiffF();
			lTestOk = (lTime >= 0.9 && lTime < 1.1);
			assert(lTestOk);
		}
	}
	Lepra::Semaphore lSemaphore;
	if (lTestOk)
	{
		lContext = _T("semaphore satisfied timed wait 1");
		Lepra::Timer lTimer;
		lSemaphore.Signal();
		lSemaphore.Signal();
		lTestOk = lSemaphore.Wait(1.0);
		assert(lTestOk);
		if (lTestOk)
		{
			double lTime = lTimer.PopTimeDiffF();
			lTestOk = (lTime >= 0.0 && lTime < 0.1);
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("semaphore satisfied timed wait 2");
		Lepra::Timer lTimer;
		lTestOk = lSemaphore.Wait(1.0);
		assert(lTestOk);
		if (lTestOk)
		{
			double lTime = lTimer.PopTimeDiffF();
			lTestOk = (lTime >= 0.0 && lTime < 0.1);
			assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("semaphore permit reset");
		lTestOk = !lSemaphore.Wait(0.0001);
		assert(lTestOk);
	}

	StaticThread lStlLockerThread(_T("STL locker"));
	if (lTestOk)
	{
		lContext = _T("STL lock exclusive");
		gThreadTestCounter = 0;
		gStlLock._M_acquire_lock();
		lStlLockerThread.Start(StlThreadEntry, 0);
		for (int x = 0; x < 10 && gThreadTestCounter != 1; ++x)
		{
			Thread::Sleep(0.001);
		}
		lTestOk = (gThreadTestCounter == 1 && lStlLockerThread.IsRunning());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("STL lock release");
		gStlLock._M_release_lock();
		Thread::Sleep(0.01);
		lTestOk = (gThreadTestCounter == 3 && !lStlLockerThread.IsRunning());
		assert(lTestOk);
	}

	StaticThread lMemAllocThread(_T("MemAllocator"));
	StaticThread lMemUseThread(_T("MemUser"));
	if (lTestOk)
	{
		lContext = _T("Memory allocator");
		lMemUseThread.Start(MemUseThreadEntry, 0);
		lMemAllocThread.Start(MemAllocThreadEntry, 0);
		Thread::Sleep(0.01);
		lTestOk = (gAllocMem != 0 && gAllocMem[1] == STATE_ALLOC_INIT && !lMemAllocThread.IsRunning() && lMemUseThread.IsRunning());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("Memory freer");
		gAllocMem[1] = STATE_ALLOC_FOUND;
		Thread::Sleep(0.01);
		lTestOk = (gAllocMem == 0 && !lMemUseThread.IsRunning());
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("Threading"), lContext, lTestOk);
	return (lTestOk);
}
