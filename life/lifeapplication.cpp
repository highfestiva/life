
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "lifeapplication.h"
#include "../cure/include/consolemanager.h"
#include "../cure/include/gameticker.h"
#include "../cure/include/resourcemanager.h"
#include "../cure/include/runtimevariable.h"
#include "../lepra/include/anticrack.h"
#include "../lepra/include/loglistener.h"
#include "../lepra/include/network.h"
#include "../lepra/include/path.h"
#include "../lepra/include/random.h"
#include "../lepra/include/systemmanager.h"
#include "life.h"
#include "rtvar.h"



namespace {
// Run before main() is started.
AntiCrack _r__;

life::Application* g_application = 0;
}



namespace life {



Application::Application(const str& base_name, const strutil::strvec& argument_list):
	lepra::Application(argument_list),
	resource_manager_(0),
	game_ticker_(0),
	console_logger_(0),
	is_power_saving_(false),
	debug_logger_(0),
	file_logger_(0),
	performance_logger_(0),
	mem_logger_(0) {
	g_application = this;
	base_name_ = base_name;
}

Application::~Application() {
	log_.RawPrint(kLevelHeadline, "The end. Baba!\n");
	if (file_logger_) {
		file_logger_->WriteLog("---\n\n", kLevelInfo);
	}

	// Kill all loggers, hopefully we don't need to log anything else.
	delete (console_logger_);
	console_logger_ = 0;
	delete (mem_logger_);
	mem_logger_ = 0;
	delete (performance_logger_);
	performance_logger_ = 0;
	delete (file_logger_);
	file_logger_ = 0;
	delete (debug_logger_);
	debug_logger_ = 0;

	// Drop performance measurement resources.
	ScopePerformanceData::EraseAll();

	LogType::Close();
}

void Application::Init() {
	v_set(cure::GetSettings(), kRtvarPhysicsFps, PHYSICS_FPS);
	v_set(cure::GetSettings(), kRtvarPowersaveFactor, 1.0);
	v_set(cure::GetSettings(), kRtvarDebugExtrasleeptime, 0.0);

	console_logger_ = CreateConsoleLogListener();
	//console_logger_->SetLevelThreashold(kLevelInfo);
#ifndef NO_LOG_DEBUG_INFO
#ifndef LEPRA_MAC
	debug_logger_ = new DebuggerLogListener();
#endif // !Apple
#endif // Showing debug information.
#ifdef LEPRA_DEBUG
	//file_logger_ = new FileLogListener(GetIoFile(GetTypeName(), "log", false));
	//file_logger_->SetLevelThreashold(kLevelInfo);
	//file_logger_->WriteLog("\n---\n", kLevelInfo);
	//performance_logger_ = new FileLogListener(GetIoFile(GetTypeName()+"Performance", "log", false));
	//mem_logger_ = new MemFileLogListener(20*1024);
#endif // Debug
	LogType::GetLogger(LogType::kRoot)->SetupBasicListeners(console_logger_, debug_logger_, file_logger_, performance_logger_, mem_logger_);

	strutil::strvec name_parts;
	name_parts.push_back(base_name_);
	name_parts.push_back(GetTypeName());
	str app_name = strutil::Join(name_parts, " ");
	str start_message = "Starting " + app_name + ", version " + GetVersion() + ", build type: " kLepraStringTypeText " " kLepraBuildTypeText ".\n";
	log_.RawPrint(kLevelHeadline, start_message);

	resource_manager_ = new cure::ResourceManager(1);
	game_ticker_ = CreateTicker();
}

void Application::Destroy() {
	// Delete in order of priority.

	// Drop all major components first, while keeping all loggers intact.
	delete (game_ticker_);
	game_ticker_ = 0;
	delete (resource_manager_);	// Resource manager lives long enough for all volontary resources to disappear.
	resource_manager_ = 0;

	Network::Stop();
}

int Application::Run() {
	// We like all threads that measure time to be on a single CPU. This is due to high resolution timers,
	// which may differ between different CPU cores. Several seconds can differ between different cores.
	// The main thread is locked to the first CPU. If there are more CPUs, the physics thread will lock to
	// another CPU later on.
	// JB 2009-12: dropped this, probably not a good idea since we need to run multiple
	// physics instances when running split screen.
	//Thread::GetCurrentThread()->SetCpuAffinityMask(0x0001);

	bool ok = true;
	if (ok) {
		ok = Network::Start();
	}
	if (ok) {
		ok = game_ticker_->Initialize();
	}
	if (ok) {
		ok = MainLoop();
	}
	return ok? 0 : 1;
}

bool Application::MainLoop() {
	bool ok = true;
	bool quit = false;
	while (ok && !quit) {
		ok = Tick();
		quit = game_ticker_->QueryQuit();
	}

	if (quit) {
		log_.Headline("User requested application termination.");
	} else if (!ok) {
		if (mem_logger_) {
			LogType::GetLogger(LogType::kRoot)->RemoveListener(mem_logger_);
		}
		log_.Fatal("Terminating application due to fatal error.");
		if (mem_logger_ && file_logger_) {
			log_.RawPrint(kLevelFatal, "\nStart dump hires logs.\n--------------------------------------------------------------------------------\n");
			mem_logger_->Dump(file_logger_->GetFile());
			log_.RawPrint(kLevelFatal, "--------------------------------------------------------------------------------\nEnd dump hires logs.\n\n");
		}
	}
	return ok;
}

bool Application::Tick() {
	LEPRA_DO_MEASURE_SCOPE(AppTick);
	HiResTimer loop_time(false);

	HiResTimer::StepCounterShadow();

	bool debug;
	v_get(debug, =, cure::GetSettings(), kRtvarDebugEnable, false);
	if (debug) {
		game_ticker_->Profile();
	}
	Random::GetRandomNumber();	// To move seed ahead.
	bool ok = game_ticker_->Tick();
	float extra_sleep;
	v_get(extra_sleep, =(float), cure::GetSettings(), kRtvarDebugExtrasleeptime, 0.0);
	if (extra_sleep > 0) {
		Thread::Sleep(extra_sleep);
	}
	if (ok) {
		LEPRA_DO_MEASURE_SCOPE(AppSleep);
		TickSleep(loop_time.QueryTimeDiff());
	}
	if (ok) {
		HandleZombieMode();
	}
	return ok;
}



cure::ApplicationTicker* Application::GetTicker() const {
	return game_ticker_;
}

void Application::SetZombieTick(const ZombieTick& zombie_tick) {
	zombie_tick_ = zombie_tick;
}



Application* Application::GetApplication() {
	return g_application;
}

str Application::GetIoFile(const str& name, const str& ext, bool add_quotes) {
	str app_name = base_name_;
	str io_name = Path::JoinPath(
		SystemManager::GetIoDirectory(app_name),
		name, ext);
	if (add_quotes) {
		io_name = "\"" + io_name + "\"";
	}
	return (io_name);
}



LogListener* Application::CreateConsoleLogListener() const {
	return (new StdioConsoleLogListener());
}



void Application::TickSleep(double main_loop_time) const {
	float power_save_factor;
	v_get(power_save_factor, =(float), cure::GetSettings(), kRtvarPowersaveFactor, 1.0);
	const float power_save_amount = game_ticker_->GetPowerSaveAmount() * power_save_factor;
	if (power_save_amount > 0) {
		if (!is_power_saving_) {
#ifndef LEPRA_TOUCH
			log_.Info("Entering power save mode.");
#endif // !touch.
			is_power_saving_ = true;
		}
		Thread::Sleep(1.0*power_save_amount);
	} else {
		if (is_power_saving_) {
#ifndef LEPRA_TOUCH
			log_.Info("Leaving power save mode.");
#endif // !touch.
			is_power_saving_ = false;
		}

		int fps;
		v_get(fps, =, cure::GetSettings(), kRtvarPhysicsFps, 2);
		double wanted_frame_time = fps? 1.0/fps : 1;
		const double sleep_time = wanted_frame_time - main_loop_time - game_ticker_->GetTickTimeReduction();
		const double MAXIMUM_SLEEP_TIME = 0.01;
		if (sleep_time >= 0) {
			double sleep_time_left = sleep_time;
			HiResTimer sleep_timer(false);
			while (sleep_time_left >= 0) {
				if (sleep_time_left > MAXIMUM_SLEEP_TIME*1.7) {
					game_ticker_->PollRoundTrip();
					Thread::Sleep(MAXIMUM_SLEEP_TIME);
				} else if (sleep_time_left > SystemManager::GetSleepResolution()) {
					Thread::Sleep(sleep_time_left - SystemManager::GetSleepResolution()/2);
				} else {
					Thread::YieldCpu();	// For lousy computers that have very big sleep intervals.
				}
				sleep_time_left = sleep_time - sleep_timer.QueryTimeDiff();
			}
#ifdef LEPRA_TOUCH
			if (sleep_time_left < -0.5)
#else // Computer
			if (sleep_time_left < -0.01)
#endif // touch device / computer.
			{
				log_.Warningf("Overslept %f s!", -sleep_time_left);
			}
			//log_volatile(mLog.Debugf("Done tick sleeping, %f s left in sleep loop (measured, %f s reduction, %f s measured."), lSleepTimeLeft, mGameTicker->GetTickTimeReduction(), lSleepTime));
		} else {
#ifndef LEPRA_IOS
			//log_volatile(mLog.Debugf("Skipping tick sleep (yield only, %f s left in sleep loop, %f s reduction."), lSleepTime, mGameTicker->GetTickTimeReduction()));
			Thread::YieldCpu();	// Play nice even though time's up!
#endif	// Not on iOS (which sleeps during timer ticks).
		}
	}
}

void Application::HandleZombieMode() {
	if (zombie_tick_) {
		zombie_tick_();
		Destroy();
		zombie_tick_();
		SystemManager::AddQuitRequest(-1);
		ScopePerformanceData::EraseAll();
		Init();
		Network::Start();
		game_ticker_->Initialize();
		zombie_tick_();
	}
}



str Application::base_name_;

loginstance(kGame, Application);



}
