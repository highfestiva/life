
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

namespace lepra {



#define	SMART_POINTER_REFERENCE	public:	SmartPointerReference _mRef;



class SmartPointerReference {
public:
	mutable int reference_count_;	// Public for optimization reasons only.
	inline SmartPointerReference():	// Inlined for optimization reasons only.
		reference_count_(0) {
	};
};

#define TEMPLATE template<class Type>
#define QUAL SmartPointer<Type>

TEMPLATE class SmartPointer {
public:

	SmartPointer(Type* object = 0);
	SmartPointer(const SmartPointer& copy);
	// TRICKY: (optimized) this class does not have a virtual destructor.
	~SmartPointer();

	void operator=(Type* object);
	void operator=(const SmartPointer& copy);
	operator Type*() const;
	Type* operator->() const;

	void SetObject(Type* object);
	void ReleaseObject();

private:
	Type* object_;
};



TEMPLATE QUAL::SmartPointer(Type* object):
	object_(0) {
	SetObject(object);
}

TEMPLATE QUAL::SmartPointer(const SmartPointer& copy):
	object_(0) {
	SetObject(copy.object_);
}

TEMPLATE QUAL::~SmartPointer() {
	ReleaseObject();
}



TEMPLATE void QUAL::operator=(Type* object) {
	SetObject(object);
}

TEMPLATE void QUAL::operator=(const SmartPointer& copy) {
	SetObject(copy.object_);
}

TEMPLATE QUAL::operator Type*() const {
	return (object_);
}

TEMPLATE Type* QUAL::operator->() const {
	return (object_);
}



TEMPLATE void QUAL::SetObject(Type* object) {
	if (object != object_) {
		ReleaseObject();

		object_ = object;
		if (object_) {
			++object_->_mRef.reference_count_;
		}
	}
}

TEMPLATE void QUAL::ReleaseObject() {
	if (object_) {
		if (--object_->_mRef.reference_count_ == 0) {
			delete (object_);
		}
		object_ = 0;
	}
}

#undef TEMPLATE
#undef QUAL


}
