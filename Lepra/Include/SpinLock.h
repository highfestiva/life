/*
	Class:  SpinLock
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	A spin lock is the most primitive type of lock that ensures mutex.
	It is also the fastest lock there is, if used on a multi-CPU machine.

	Be careful though! It's not a reentrant lock! Thus, you can't call Acquire()
	more than once at a time.

	It's implemented here instead of in Thread.h because we need some sort
	of primitive lock which can be used at a very basic level without including
	anything else.
*/

#pragma once

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

inline SpinLock::SpinLock() :
	mLocked(UNLOCKED)
{
}

inline SpinLock::~SpinLock()
{
}

inline void SpinLock::Acquire()
{
	while (BusLock::CompareAndSwap(&mLocked, LOCKED, UNLOCKED) == false){}
}

inline bool SpinLock::TryAcquire()
{
	return BusLock::CompareAndSwap(&mLocked, LOCKED, UNLOCKED);
}

inline void SpinLock::Release()
{
	BusLock::CompareAndSwap(&mLocked, UNLOCKED, LOCKED);
}



}

