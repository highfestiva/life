
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// The MemberThread template does not require inheritance to start a thread in a member method:
//	MemberThread<ThisClass> x;
//	x.Start(this, &ThisClass::MyThreadedMethod);



#pragma once

#include "lepraassert.h"
#include "thread.h"



namespace lepra {



template<class _Base, class _Object = void*> class MemberThread: public Thread {
public:
	typedef void (_Base::*ThreadEntry)();
	typedef void (_Base::*ThreadEntryData)(_Object);

	MemberThread(const str& thread_name);
	virtual ~MemberThread();

	bool Start(_Base* object, ThreadEntry thread_entry);
	bool Start(_Base* object, ThreadEntryData thread_entry_data, _Object object_data);

	_Object GetObjectData();
	void SetObjectData(_Object data);

private:
	bool Start();	// Obfuscates Thread::Start(void).
	void Run();

	_Base* object_;
	ThreadEntry thread_entry_;
	ThreadEntryData thread_entry_data_;
	_Object object_data_;
};



template<class _Base, class _Object> MemberThread<_Base, _Object>::MemberThread(const str& thread_name):
	Thread(thread_name),
	object_(0),
	thread_entry_(0),
	thread_entry_data_(0) {
}

template<class _Base, class _Object> MemberThread<_Base, _Object>::~MemberThread() {
	// Parent class Join()s.
}


template<class _Base, class _Object> bool MemberThread<_Base, _Object>::Start(_Base* object, ThreadEntry thread_entry) {
	if (IsRunning()) {
		Join(10.0);
		Kill();
	}
	deb_assert(object);
	deb_assert(thread_entry);
	object_ = object;
	thread_entry_ = thread_entry;
	return (Thread::Start());	// Will call Run() when the new thread starts.
}

template<class _Base, class _Object> bool MemberThread<_Base, _Object>::Start(_Base* object, ThreadEntryData thread_entry_data, _Object object_data) {
	if (IsRunning()) {
		Join(10.0);
		Kill();
	}
	deb_assert(object);
	deb_assert(thread_entry_data);
	object_ = object;
	thread_entry_data_ = thread_entry_data;
	object_data_ = object_data;
	return (Thread::Start());	// Will call Run() when the new thread starts.
}

template<class _Base, class _Object> _Object MemberThread<_Base, _Object>::GetObjectData() {
	return object_data_;
}

template<class _Base, class _Object> void MemberThread<_Base, _Object>::SetObjectData(_Object data) {
	object_data_ = data;
}

template<class _Base, class _Object> void MemberThread<_Base, _Object>::Run() {
	deb_assert(object_);
	deb_assert(thread_entry_ || thread_entry_data_);
	if (thread_entry_) {
		(object_->*thread_entry_)();
	} else {
		(object_->*thread_entry_data_)(object_data_);
	}
}



}
