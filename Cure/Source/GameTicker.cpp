
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/gameticker.h"
#include "../include/runtimevariablename.h"
#include "../../lepra/include/number.h"
#include "../../lepra/include/systemmanager.h"
#include "../../tbc/include/physicsmanager.h"
#include "../../tbc/include/physicsmanagerfactory.h"
#include "../include/consolemanager.h"
#include "../include/contextmanager.h"
#include "../include/contextobject.h"
#include "../include/networkagent.h"
#include "../include/runtimevariable.h"
#include "../include/terrainmanager.h"
#include "../include/timemanager.h"



namespace cure {



ApplicationTicker::ApplicationTicker() {
}

ApplicationTicker::~ApplicationTicker() {
}

void ApplicationTicker::Resume(bool hard) {
	(void)hard;
	HiResTimer::StepCounterShadow();
}

void ApplicationTicker::Suspend(bool hard) {
	(void)hard;
}

void ApplicationTicker::Profile() {
}

bool ApplicationTicker::QueryQuit() {
	return (SystemManager::GetQuitRequest() != 0);
}



GameTicker::GameTicker(float physics_radius, int physics_levels, float physics_sensitivity):
	time_manager_(new TimeManager),
	physics_manager_(tbc::PhysicsManagerFactory::Create(tbc::PhysicsManagerFactory::kEngineOde, physics_radius, physics_levels,  physics_sensitivity)),
	physics_worker_thread_(0),
	physics_tick_start_semaphore_(0),
	physics_tick_done_semaphore_(0) {
	physics_manager_->SetTriggerListener(this);
	physics_manager_->SetForceFeedbackListener(this);
	physics_manager_->InitCurrentThread();
}

GameTicker::~GameTicker() {
	DeletePhysicsThread();
	delete (physics_manager_);
	physics_manager_ = 0;
	delete time_manager_;
	time_manager_ = 0;
}

const TimeManager* GameTicker::GetTimeManager() const {
	return time_manager_;
}

TimeManager* GameTicker::GetTimeManager() {
	return time_manager_;
}

tbc::PhysicsManager* GameTicker::GetPhysicsManager(bool is_thread_safe) const {
	(void)is_thread_safe;
#ifdef LEPRA_DEBUG
	// This is a check to see if the code that fetches the physics manager
	// is thread-safe. We definitely don't want to impose mutex locks on
	// the non-physics threads, since that would just wait and kill all
	// performance on a dual or faster system.

	// Check if we are
	//   a) in thread-safe part of code (=physics thread not running), or
	//   b) we don't have a physics thread (we run physics using the main thread).
	if (!is_thread_safe && physics_worker_thread_) {
		// We have a physics thread. Are we it?
		size_t physics_thread_id = physics_worker_thread_->GetThreadId();
		size_t this_thread_id = Thread::GetCurrentThreadId();
		// Make sure we're the physics thread, otherwise we're not allowed to
		// read/write any physical stuff.
		deb_assert(physics_thread_id == this_thread_id);
	}
#endif // LEPRA_DEBUG
	return physics_manager_;
}

Lock* GameTicker::GetPhysicsLock() {
	return &physics_lock_;
}



void GameTicker::StartPhysicsTick() {
	if (time_manager_->GetAffordedPhysicsStepCount() <= 0) {
		log_debug("Could not afford a physics step.");
		return;
	}

	bool parallel_physics;
	v_get(parallel_physics, =, GetSettings(), kRtvarPhysicsParallel, true);
	if (parallel_physics) {
		CreatePhysicsThread();
	} else {
		DeletePhysicsThread();
	}

	//LEPRA_MEASURE_SCOPE(StartPhysics);
	// Physics thread
	// 1. does *NOT* add/delete objects,
	// 2. processes context objects ("scripts"),
	// 3. applies movement indata (from network, kUi and script), and
	// 4. integrates physics.
	// system performance determines if this is done by a worker thread, or done synchronously
	// by the current thread. (Single CPU => single thread...)

	if (physics_tick_start_semaphore_) {
		// We use a bool to check if all accesses are thread-safe.
		physics_tick_start_semaphore_->Signal();
	} else {
		PhysicsTick();
	}
}

void GameTicker::WaitPhysicsTick() {
	if (time_manager_->GetAffordedPhysicsStepCount() > 0) {
		if (physics_tick_done_semaphore_) {
			LEPRA_MEASURE_SCOPE(WaitPhysics);
#ifdef LEPRA_DEBUG
			physics_tick_done_semaphore_->Wait();
#else // !Debugging
			physics_tick_done_semaphore_->Wait(3.0);
#endif // Debugging/!debugging.
		}

		// We gotta run this from the main thread, as we're working with the Context.
		DidPhysicsTick();
	}
}



void GameTicker::PhysicsTick() {
	LEPRA_MEASURE_SCOPE(Physics);

	bool stop_physics;
	v_get(stop_physics, =, GetSettings(), kRtvarPhysicsHalt, false);
	if (stop_physics) {
		return;
	}

	physics_manager_->InitCurrentThread();

	const int afforded_step_count = time_manager_->GetAffordedPhysicsStepCount();
	const int afforded_micro_steps = time_manager_->GetAffordedPhysicsMicroStepCount();
	const int micro_step_count = afforded_step_count * afforded_micro_steps;
	const float step_time = time_manager_->GetAffordedPhysicsStepTime();
	const float step_increment = step_time / afforded_micro_steps;
	/*if (afforded_step_count != 1) {
		log_.Warningf("Game time allows for %i physics steps in increments of %f.",
			micro_step_count, step_increment);
	}*/
	bool fast_algo;
	v_get(fast_algo, =, GetSettings(), kRtvarPhysicsFastalgo, true);
	bool no_clip;
	v_get(no_clip, =, GetSettings(), kRtvarPhysicsNoclip, false);
	{
		LEPRA_MEASURE_SCOPE(PreSteps);
		physics_manager_->PreSteps();
	}
	try {
		for (int x = 0; x < micro_step_count; ++x) {
			WillMicroTick(step_increment);	// Ticks engines, so needs to be run every physics step.
			if (fast_algo) {
				physics_manager_->StepFast(step_increment, !no_clip);
			} else {
				physics_manager_->StepAccurate(step_increment, !no_clip);
			}
		}
	} catch (...) {
		log_.Errorf("Got some crash or major problem in physics simulation!");
	}
	{
		//LEPRA_MEASURE_SCOPE(PostSteps);
		physics_manager_->PostSteps();
	}
}



void GameTicker::CreatePhysicsThread() {
	// If we have more than one CPU, we run a separate physics thread.
	if (!physics_worker_thread_ && (SystemManager::GetPhysicalCpuCount() > 1 || SystemManager::GetCoreCount() > 1)) {
		physics_tick_start_semaphore_ = new Semaphore();
		physics_tick_done_semaphore_ = new Semaphore();

		physics_worker_thread_ = new MemberThread<GameTicker>("PhysicsThread");
		physics_worker_thread_->Start(this, &GameTicker::PhysicsThreadEntry);
	}
}

void GameTicker::DeletePhysicsThread() {
	if (physics_worker_thread_) {
		physics_worker_thread_->RequestStop();
		physics_tick_start_semaphore_->Signal();
		physics_worker_thread_->Join(3.0);
		delete (physics_worker_thread_);
		physics_worker_thread_ = 0;

		delete (physics_tick_start_semaphore_);
		physics_tick_start_semaphore_ = 0;
		delete (physics_tick_done_semaphore_);
		physics_tick_done_semaphore_ = 0;
	}
}

void GameTicker::PhysicsThreadEntry() {
	// We set affinity to the second processor. This is due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between
	// different cores. The main thread is locked to the first CPU, therefore this one
	// goes to the last one.
	// JB 2009-12: dropped this, probably not a good idea since we need to run multiple
	// physics instances when running split screen.
	// JB 2012-11: this may be a good idea again as the physics now is a singleton
	// again, shared by server+split screen clients.
	//Thread::GetCurrentThread()->SetCpuAffinityMask(1<<(SystemManager::GetCoreCount()-1));

	physics_manager_->InitCurrentThread();

	while (physics_worker_thread_ && !physics_worker_thread_->GetStopRequest()) {
		physics_tick_start_semaphore_->Wait();
		if (!physics_worker_thread_->GetStopRequest()) {
			{
				ScopeLock lock(GetPhysicsLock());
				PhysicsTick();
			}
			physics_tick_done_semaphore_->Signal();
		}
	}
}



loginstance(kGame, GameTicker);



}
