
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
	inline void UncheckedAcquire();
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
	assert(!IsOwner());
	UncheckedAcquire();
}

inline void SpinLock::UncheckedAcquire()
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
	assert(!IsOwner());
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



class ScopeSpinLock
{
public:
	inline ScopeSpinLock(SpinLock* pLock, bool pAcquire = true):
		mLock(pLock)
	{
		if (pAcquire)
		{
			mLock->Acquire();
		}
	}
	inline ~ScopeSpinLock()
	{
		mLock->Release();
	}

protected:
	SpinLock* mLock;
};



class VerifySoleAccessor: public ScopeSpinLock
{
public:
	inline VerifySoleAccessor(SpinLock* pLock):
		ScopeSpinLock(pLock, false)
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
