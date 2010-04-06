
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "Thread.h"
#include <assert.h>
#include "BusLock.h"



namespace Lepra
{



class SpinLock: public OwnedLock
{
public:
	inline SpinLock();
	inline ~SpinLock();

	inline void Acquire();
	inline bool TryAcquire();
	inline void Release();

private:

	enum
	{
		UNLOCKED = 0,
		LOCKED,
	};

	long mLocked;
};

inline SpinLock::SpinLock():
	mLocked(UNLOCKED)
{
}

inline SpinLock::~SpinLock()
{
}

inline void SpinLock::Acquire()
{
	bool lAcquired = BusLock::CompareAndSwap(&mLocked, LOCKED, UNLOCKED);
	while (!lAcquired)
	{
		for (int x = 0; x < 4000 && !lAcquired; ++x)
		{
			lAcquired = BusLock::CompareAndSwap(&mLocked, LOCKED, UNLOCKED);
		}
		if (!lAcquired)
		{
			Thread::YieldCpu();
		}
	}
	Reference();
}

inline bool SpinLock::TryAcquire()
{
	bool lAcquired = BusLock::CompareAndSwap(&mLocked, LOCKED, UNLOCKED);
	if (lAcquired)
	{
		Reference();
	}
	return (lAcquired);
}

inline void SpinLock::Release()
{
	Dereference();
	assert(mLocked == LOCKED);
	BusLock::CompareAndSwap(&mLocked, UNLOCKED, LOCKED);
}



class VerifySoleAccessor
{
public:
	inline VerifySoleAccessor(SpinLock* pLock):
		mLock(pLock)
	{
		bool lLock = mLock->TryAcquire();
		if (!lLock)
		{
			OnError(mLock->GetOwner());
		}
	}
	inline ~VerifySoleAccessor()
	{
		mLock->Release();
	}

protected:
	void OnError(const Thread* pOwner);

	SpinLock* mLock;
	LOG_CLASS_DECLARE();
};

#define VERIFY_ALONE(lock)	VerifySoleAccessor __lVerifier(lock)
#define DO_ASSERT_ALONE
#ifdef DO_ASSERT_ALONE
#define ASSERT_ALONE(lock)	VERIFY_ALONE(lock)
#else
#define ASSERT_ALONE(lock)
#endif



}
