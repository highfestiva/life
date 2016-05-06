
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/thread.h"
#include "../include/fastlock.h"



namespace lepra {



StaticThread gMainThread("MainThread");



bool OwnedLock::IsOwner() const {
	return (owner_ == Thread::GetCurrentThread());
}

Thread* OwnedLock::GetOwner() const {
	return (owner_);
}

int OwnedLock::GetReferenceCount() const {
	return acquire_count_;
}

OwnedLock::OwnedLock():
	owner_(0),
	acquire_count_(0) {
}

OwnedLock::~OwnedLock() {
	deb_assert(acquire_count_ == 0);
}

void OwnedLock::Reference() {
	if (acquire_count_ == 0) {
		deb_assert(!owner_);
		owner_ = Thread::GetCurrentThread();
	}
	++acquire_count_;
}

void OwnedLock::Dereference() {
	deb_assert(IsOwner());
	--acquire_count_;
	if (acquire_count_ == 0) {
		owner_ = 0;
	}
}



Lock::Lock():
	system_lock_(new FastLock) {
}

Lock::~Lock() {
	FastLock* _lock = (FastLock*)system_lock_;
	system_lock_ = 0;
	delete _lock;
}

void Lock::Acquire() {
	((FastLock*)system_lock_)->Acquire();
	Reference();
}

bool Lock::TryAcquire() {
	bool acquired = ((FastLock*)system_lock_)->TryAcquire();
	if (acquired) {
		Reference();
	}
	return acquired;
}

void Lock::Release() {
	Dereference();
	((FastLock*)system_lock_)->Release();
}

void* Lock::GetSystemLock() const {
	return system_lock_;
}

void Lock::operator=(const Lock&) {
	deb_assert(false);
}



ScopeLock::ScopeLock(Lock* lock):
	lock_(lock) {
	lock_->Acquire();
}

ScopeLock::~ScopeLock() {
	lock_->Release();
	lock_ = 0;
}

void ScopeLock::Acquire() {
	lock_->Acquire();
}

void ScopeLock::Release() {
	lock_->Release();
}



Condition::Condition(Lock* external_lock):
	external_lock_(external_lock),
	system_condition_(new FastCondition(external_lock? (FastLock*)external_lock->GetSystemLock() : 0)) {
}

Condition::~Condition() {
	FastCondition* condition = (FastCondition*)system_condition_;
	system_condition_ = 0;
	delete condition;
}

void Condition::Wait() {
	external_lock_->Dereference();
	((FastCondition*)system_condition_)->Wait();
	external_lock_->Reference();
}

bool Condition::Wait(float64 max_wait_time) {
	external_lock_->Dereference();
	const bool signalled = ((FastCondition*)system_condition_)->Wait(max_wait_time);
	external_lock_->Reference();
	return signalled;
}

void Condition::Signal() {
	((FastCondition*)system_condition_)->Signal();
}

void Condition::SignalAll() {
	((FastCondition*)system_condition_)->SignalAll();
}


Semaphore::Semaphore():
	system_semaphore_(new FastSemaphore) {
}

Semaphore::Semaphore(unsigned max_count):
	system_semaphore_(new FastSemaphore(max_count)) {
}

Semaphore::~Semaphore() {
	FastSemaphore* semaphore = (FastSemaphore*)system_semaphore_;
	system_semaphore_ = 0;
	delete semaphore;
}

void Semaphore::Wait() {
	((FastSemaphore*)system_semaphore_)->Wait();
}

bool Semaphore::Wait(float64 max_wait_time) {
	return ((FastSemaphore*)system_semaphore_)->Wait(max_wait_time);
}

void Semaphore::Signal() {
	((FastSemaphore*)system_semaphore_)->Signal();
}


RwLock::RwLock(const str& rw_lock_name):
	name_(rw_lock_name),
	system_rw_lock_(new FastRwLock) {
}

RwLock::~RwLock() {
	FastRwLock* _lock = (FastRwLock*)system_rw_lock_;
	system_rw_lock_ = 0;
	delete _lock;
}

void RwLock::AcquireRead() {
	((FastRwLock*)system_rw_lock_)->AcquireRead();
}

void RwLock::AcquireWrite() {
	((FastRwLock*)system_rw_lock_)->AcquireWrite();
}

void RwLock::Release() {
	((FastRwLock*)system_rw_lock_)->Release();
}

str RwLock::GetName() {
	return name_;
}



Thread::Thread(const str& thread_name):
	thread_name_(thread_name),
	running_(false),
	stop_requested_(false),
	self_destruct_(false),
	thread_handle_(0),
	thread_id_(0) {
}

Thread::~Thread() {
	if (!GetSelfDestruct()) {
		if (!Join(20.0)) {
			Kill();
		}
	}
}

void Thread::InitializeMainThread() {
	InitializeThread(&gMainThread);
}

bool Thread::QueryInitializeThread() {
	Thread* self = GetCurrentThread();
	if (!self) {
		self = new StaticThread("OnTheFly");
		self->RequestSelfDestruct();
		InitializeThread(self);
		return true;
	}
	return false;
}

bool Thread::IsRunning() const {
	return (running_);
}

void Thread::SetRunning(bool running) {
	log_volatile(log_.Debug("Thread " + thread_name_ + " is " + (running? "starting" : "stopping")));
	running_ = running;
}

void Thread::SetThreadId(size_t thread_id) {
	thread_id_ = thread_id;
}

bool Thread::GetStopRequest() const {
	return (stop_requested_);
}

void Thread::SetStopRequest(bool stop_request) {
	stop_requested_ = stop_request;
}

void Thread::RequestStop() {
	if (IsRunning() == true) {
		SetStopRequest(true);
	}
}

bool Thread::GetSelfDestruct() const {
	return (self_destruct_);
}

void Thread::RequestSelfDestruct() {
	self_destruct_ = true;
}

const str& Thread::GetThreadName() const {
	return (thread_name_);
}

size_t Thread::GetThreadId() const {
	return (thread_id_);
}

size_t Thread::GetThreadHandle() const {
	return (thread_handle_);
}

void Thread::Sleep(float64 time) {
	if (time > 0) {
		Sleep((unsigned int)(time * 1000000.0));
	}
}

bool Thread::Join(float64 time_out) {
	SetStopRequest(true);
	return GraceJoin(time_out);
}

void Thread::RunThread() {
	SetThreadId(GetCurrentThreadId());
	SetRunning(true);
	semaphore_.Signal();
	YieldCpu();
	Run();
	SetRunning(false);
	PostRun();
}

void RunThread(Thread* thread) {
	thread->RunThread();
	if (thread->GetSelfDestruct()) {
		delete (thread);
	}
}

loginstance(kGeneral, Thread);



StaticThread::StaticThread(const str& thread_name):
	Thread(thread_name),
	thread_entry_(0),
	data_(0) {
}

StaticThread::~StaticThread() {
	thread_entry_ = 0;
	data_ = 0;
}

bool StaticThread::Start(void (*thread_entry)(void*), void* data) {
	thread_entry_ = thread_entry;
	data_ = data;
	return (Start());
}

bool StaticThread::Start() {
	// Only implemented to protect the Start() method.
	return (Thread::Start());
}

void StaticThread::Run() {
	thread_entry_(data_);
}



}
