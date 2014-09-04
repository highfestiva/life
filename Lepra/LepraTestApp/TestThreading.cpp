/*
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../Lepra/Include/LepraAssert.h"
#include <locale>
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

void MemAllocThreadEntry(void*)
{
	deb_assert(gAllocMem == 0);
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
			StaticThread lThread("StartupTest");
			gThreadTestCounter = 7;
			lTestOk = lThread.Start(IncreaseThread, 0);
			deb_assert(lTestOk);
			if (lTestOk)
			{
				lContext = _T("thread run and self termination");
				//Thread::Sleep(0.001);	// Make sure we sleep at least once.
				for (unsigned y = 0; y < 200 && lThread.IsRunning(); ++y)
				{
					Thread::Sleep(0.001);
				}
				lTestOk = (gThreadTestCounter == 8);
				deb_assert(lTestOk);
				if (lTestOk)
				{
					lTestOk = !lThread.IsRunning();
					deb_assert(lTestOk);
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
		deb_assert(lTestOk);
	}

	StaticThread lThread("LockTest");
	if (lTestOk)
	{
		lContext = _T("mutex exclusiveness");
		gThreadTestCounter = -1;
		gThreadTestLock.Acquire();
		lTestOk = lThread.Start(LockThread, 0);
		if (lTestOk)
		{
			lTestOk = lThread.IsRunning();
			deb_assert(lTestOk);
		}
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 0 && lThread.IsRunning());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("spin lock");
		SpinLock lLock;
		lTestOk = lLock.TryAcquire();
		deb_assert(lTestOk);
		lLock.Release();
	}
	if (lTestOk)
	{
		lContext = _T("semaphore exclusiveness");
		gThreadTestSemaphore.Signal();
		gThreadTestSemaphore.Wait();
		gThreadTestLock.Release();
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 2 && lThread.IsRunning());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("semaphore wait timeout - 1");
		HiResTimer lTimer;
		lTestOk = !gThreadTestSemaphore.Wait(0.5);
		deb_assert(lTestOk);
		lTimer.UpdateTimer();
		if (lTestOk)
		{
			lTestOk = Math::IsInRange(lTimer.GetTimeDiff(), 0.4, 0.9);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (gThreadTestCounter == 2 && lThread.IsRunning());
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = !gThreadTestSemaphore.Wait(0.1);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			gThreadTestSemaphore.Signal();
			Thread::Sleep(0.1);
			lTestOk = !gThreadTestSemaphore.Wait(0.1);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (gThreadTestCounter == 4 && lThread.IsRunning());
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("thread operation");
		gThreadTestSemaphore.Signal();
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 6 && lThread.IsRunning());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("thread grace termination");
		lThread.RequestStop();
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 7 && !lThread.IsRunning());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("MemberThread");
		{
			MemberThreadTestClass lTestClass;
			MemberThread<MemberThreadTestClass> lThread("TestMemberThread");
			gThreadTestCounter = -5;
			lTestOk = lThread.Start(&lTestClass, &MemberThreadTestClass::OnTest);
			deb_assert(lTestOk);
			if (lTestOk)
			{
				lTestOk = lThread.Join();
				deb_assert(lTestOk);
			}
		}
		if (lTestOk)
		{
			lTestOk = (gThreadTestCounter == -4);
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("main thread ID");
		lTestOk = (Lepra::Thread::GetCurrentThreadId() != 0);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("sub thread ID");
		ThreadIdTestClass lTestInstance;
		lTestInstance.mThreadId = 0;
		MemberThread<ThreadIdTestClass> lThread("TestThreadId");
		if (lTestOk)
		{
			lTestOk = lThread.Start(&lTestInstance, &ThreadIdTestClass::StoreId);
			deb_assert(lTestOk);
		}
		size_t lActualThreadId = lThread.GetThreadId();
		if (lTestOk)
		{
			lTestOk = lThread.Join();
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (lTestInstance.mThreadId != 0 && lTestInstance.mThreadId == lActualThreadId);
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("semaphore wait timeout");
		Lepra::Timer lTimer;
		Lepra::Semaphore lSemaphore;
		lTestOk = !lSemaphore.Wait(1.0);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			double lTime = lTimer.PopTimeDiff();
			lTestOk = (lTime >= 0.9 && lTime < 1.1);
			deb_assert(lTestOk);
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
		deb_assert(lTestOk);
		if (lTestOk)
		{
			double lTime = lTimer.PopTimeDiff();
			lTestOk = (lTime >= 0.0 && lTime < 0.1);
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("semaphore satisfied timed wait 2");
		Lepra::Timer lTimer;
		lTestOk = lSemaphore.Wait(1.0);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			double lTime = lTimer.PopTimeDiff();
			lTestOk = (lTime >= 0.0 && lTime < 0.1);
			deb_assert(lTestOk);
		}
	}
	if (lTestOk)
	{
		lContext = _T("semaphore permit reset");
		lTestOk = !lSemaphore.Wait(0.0001);
		deb_assert(lTestOk);
	}

	StaticThread lMemAllocThread("MemAllocator");
	StaticThread lMemUseThread("MemUser");
	if (lTestOk)
	{
		lContext = _T("Memory allocator");
		lMemUseThread.Start(MemUseThreadEntry, 0);
		lMemAllocThread.Start(MemAllocThreadEntry, 0);
		Thread::Sleep(0.01);
		lTestOk = (gAllocMem != 0 && gAllocMem[1] == STATE_ALLOC_INIT && !lMemAllocThread.IsRunning() && lMemUseThread.IsRunning());
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("Memory freer");
		gAllocMem[1] = STATE_ALLOC_FOUND;
		Thread::Sleep(0.01);
		lTestOk = (gAllocMem == 0 && !lMemUseThread.IsRunning());
		deb_assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("Threading"), lContext, lTestOk);
	return (lTestOk);
}
