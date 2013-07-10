
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/SpinLock.h"
#include "../Include/LepraAssert.h"
#include <stdexcept>



namespace Lepra
{



void VerifySoleAccessor::OnError(const Thread* pOwner)
{
	str lOwnerName = pOwner? strutil::Encode(pOwner->GetThreadName()) : _T("<Unknown>");
	mLog.Errorf(_T("Someone else is accessing our resource, namely thread %s at %p!"),
		lOwnerName.c_str(), pOwner);
	deb_assert(false);
	throw std::runtime_error("Resource collision failure!");
}



LOG_CLASS_DEFINE(TEST, VerifySoleAccessor);



}
