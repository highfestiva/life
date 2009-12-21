/*
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include <assert.h>
#include "../Include/Log.h"
#include "../Include/MemberThread.h"
#include "../Include/Socket.h"

using namespace Lepra;

void ReportTestResult(const Lepra::LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult);

volatile int gThreadTestCounter;

void IncreaseThread(void*)
{
	++gThreadTestCounter;
}

Lock gThreadTestLock;
Semaphore gThreadTestSemaphore;

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
	while (!Thread::GetCurrentThread()->GetStopRequest())
	{
		Thread::Sleep(0.001);
	}
	++gThreadTestCounter;
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
				Thread::Sleep(0.001);	// Make sure we sleep at least once.
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
		lContext = _T("thread operation");
		gThreadTestSemaphore.Signal();
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 4 && lThread.IsRunning());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("thread grace termination");
		lThread.RequestStop();
		Thread::Sleep(0.1);
		lTestOk = (gThreadTestCounter == 5 && !lThread.IsRunning());
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

	ReportTestResult(pAccount, _T("Threading"), lContext, lTestOk);
	return (lTestOk);
}
