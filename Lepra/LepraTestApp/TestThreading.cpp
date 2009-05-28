/*
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include <assert.h>
#include "../Include/Log.h"
#include "../Include/MemberThread.h"
#include "../Include/Socket.h"

using namespace Lepra;

void ReportTestResult(const Lepra::LogDecorator& pLog, const String& pTestName, const String& pContext, bool pResult);

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

bool TestThreading(const LogDecorator& pAccount)
{
	String lContext;
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
				lTestOk = (gThreadTestCounter == 8 && !lThread.IsRunning());
				assert(lTestOk);
			}
		}
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

	ReportTestResult(pAccount, _T("Threading"), lContext, lTestOk);
	return (lTestOk);
}
