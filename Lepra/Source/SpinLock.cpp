
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Include/SpinLock.h"
#include <assert.h>
#include <stdexcept>



namespace Lepra
{



void VerifySoleAccessor::OnError(const Thread* pOwner)
{
	str lOwnerName = pOwner? pOwner->GetThreadName() : _T("<Unknown>");
	mLog.Errorf(_T("Someone else is accessing our resource, namely thread %s at %p!"),
		lOwnerName.c_str(), pOwner);
	assert(false);
	throw std::runtime_error("Resource collision failure!");
}



LOG_CLASS_DEFINE(TEST, VerifySoleAccessor);



}
