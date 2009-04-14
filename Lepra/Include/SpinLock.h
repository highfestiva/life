/*
	Class:  SpinLock
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	A spin lock is the most primitive type of lock that ensures mutex.
	It is also the fastest lock there is, if used on a multi-CPU machine.

	Be careful though! It's not a reentrant lock! Thus, you can't call Acquire()
	more than once at a time.

	It's implemented here instead of in Thread.h because we need some sort
	of primitive lock which can be used at a very basic level without including
	anything else.
*/

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "BusLock.h"

namespace Lepra
{

class SpinLock
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

SpinLock::SpinLock() :
	mLocked(UNLOCKED)
{
}

SpinLock::~SpinLock()
{
}

void SpinLock::Acquire()
{
	while (BusLock::CompareAndSwap(&mLocked, LOCKED, UNLOCKED) == false){}
}

bool SpinLock::TryAcquire()
{
	return BusLock::CompareAndSwap(&mLocked, LOCKED, UNLOCKED);
}

void SpinLock::Release()
{
	BusLock::CompareAndSwap(&mLocked, UNLOCKED, LOCKED);
}

} // End namespace.

#endif
