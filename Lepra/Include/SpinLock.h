
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "Thread.h"
#include "BusLock.h"
#include <stdexcept>



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

inline SpinLock::SpinLock() :
	mLocked(UNLOCKED)
{
}

inline SpinLock::~SpinLock()
{
}

inline void SpinLock::Acquire()
{
	while (BusLock::CompareAndSwap(&mLocked, LOCKED, UNLOCKED) == false)
		;
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
			const Thread* lOwner = mLock->GetOwner();
			str lOwnerName = lOwner? lOwner->GetThreadName() : _T("<Unknown>");
			mLog.Errorf(_T("Someone else is accessing our resource, namely thread %s at %p!"),
				lOwnerName.c_str(), lOwner);
			assert(false);
			throw std::runtime_error("Resource collision failure!");
		}
	}
	inline ~VerifySoleAccessor()
	{
		mLock->Release();
	}

protected:
	SpinLock* mLock;
	LOG_CLASS_DECLARE();
};

LOG_CLASS_DEFINE(TEST, VerifySoleAccessor);

#define ASSERT_ALONE(lock)	VerifySoleAccessor __lVerifier(lock)



}
