
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/SpinLock.h"
#include "../Include/LepraAssert.h"
#include <stdexcept>



namespace Lepra
{



void VerifySoleAccessor::OnError(const Thread* pOwner)
{
	str lOwnerName = pOwner? pOwner->GetThreadName() : "<Unknown>";
	mLog.Errorf("Someone else is accessing our resource, namely thread %s at %p!",
		lOwnerName.c_str(), pOwner);
	deb_assert(false);
	throw std::runtime_error("Resource collision failure!");
}



loginstance(TEST, VerifySoleAccessor);



}
