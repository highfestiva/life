
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/spinlock.h"
#include "../include/lepraassert.h"
#include <stdexcept>



namespace lepra {



void VerifySoleAccessor::OnError(const Thread* owner) {
	str owner_name = owner? owner->GetThreadName() : "<Unknown>";
	log_.Errorf("Someone else is accessing our resource, namely thread %s at %p!",
		owner_name.c_str(), owner);
	deb_assert(false);
	throw std::runtime_error("Resource collision failure!");
}



loginstance(kTest, VerifySoleAccessor);



}
