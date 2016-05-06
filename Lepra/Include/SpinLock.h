
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "thread.h"
#include "lepraassert.h"
#include "buslock.h"



namespace lepra {



class SpinLock: public OwnedLock {
public:
	inline SpinLock();
	inline ~SpinLock();

	inline void Acquire();
	inline void UncheckedAcquire();
	inline bool TryAcquire();
	inline void Release();

private:

	enum {
		kUnlocked = 0,
		kLocked,
	};

	int32 locked_;
};

inline SpinLock::SpinLock():
	locked_(kUnlocked) {
}

inline SpinLock::~SpinLock() {
}

inline void SpinLock::Acquire() {
	deb_assert(!IsOwner());
	UncheckedAcquire();
}

inline void SpinLock::UncheckedAcquire() {
	bool acquired = BusLock::CompareAndSwap(&locked_, kLocked, kUnlocked);
	while (!acquired) {
		for (int x = 0; x < 4000 && !acquired; ++x) {
			acquired = BusLock::CompareAndSwap(&locked_, kLocked, kUnlocked);
		}
		if (!acquired) {
			Thread::YieldCpu();
		}
	}
	Reference();
}

inline bool SpinLock::TryAcquire() {
	deb_assert(!IsOwner());
	bool acquired = BusLock::CompareAndSwap(&locked_, kLocked, kUnlocked);
	if (acquired) {
		Reference();
	}
	return (acquired);
}

inline void SpinLock::Release() {
	Dereference();
	deb_assert(locked_ == kLocked);
	BusLock::CompareAndSwap(&locked_, kUnlocked, kLocked);
}



class ScopeSpinLock {
public:
	inline ScopeSpinLock(SpinLock* _lock, bool acquire = true):
		lock_(_lock) {
		if (acquire) {
			lock_->Acquire();
		}
	}
	inline ~ScopeSpinLock() {
		lock_->Release();
	}

protected:
	SpinLock* lock_;
};



class VerifySoleAccessor: public ScopeSpinLock {
public:
	inline VerifySoleAccessor(SpinLock* _lock):
		ScopeSpinLock(_lock, false) {
		bool __lock = lock_->TryAcquire();
		if (!__lock) {
			OnError(lock_->GetOwner());
		}
	}
	inline ~VerifySoleAccessor() {
		lock_->Release();
	}

protected:
	void OnError(const Thread* owner);

	logclass();
};

#define VERIFY_ALONE(lock)	VerifySoleAccessor __lVerifier(lock)
#define DO_ASSERT_ALONE
#ifdef DO_ASSERT_ALONE
#define ASSERT_ALONE(lock)	VERIFY_ALONE(lock)
#else
#define ASSERT_ALONE(lock)
#endif



}
