
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/application.h"
#include "../thirdparty/FastDelegate/FastDelegate.h"
#include "../lepra/include/loglistener.h"
#include "life.h"



namespace cure {
class ApplicationTicker;
class ResourceManager;
}



namespace life {



class Application: public lepra::Application {
public:
	typedef fastdelegate::FastDelegate0<void> ZombieTick;

	Application(const str& base_name, const strutil::strvec& argument_list);
	virtual ~Application();
	virtual void Init();
	virtual void Destroy();
	virtual int Run();
	virtual bool MainLoop();
	virtual bool Tick();

	virtual str GetTypeName() const = 0;
	virtual str GetVersion() const = 0;
	virtual cure::ApplicationTicker* CreateTicker() const = 0;
	cure::ApplicationTicker* GetTicker() const;
	void SetZombieTick(const ZombieTick& zombie_tick);

	static Application* GetApplication();
	static str GetIoFile(const str& name, const str& ext, bool add_quotes = true);

protected:
	virtual LogListener* CreateConsoleLogListener() const;

	cure::ResourceManager* resource_manager_;
	cure::ApplicationTicker* game_ticker_;
	LogListener* console_logger_;

private:
	void TickSleep(double main_loop_time) const;
	void HandleZombieMode();

	static str base_name_;
	ZombieTick zombie_tick_;
	mutable bool is_power_saving_;

	LogListener* debug_logger_;
	FileLogListener* file_logger_;
	LogListener* performance_logger_;
	MemFileLogListener* mem_logger_;

	logclass();
};



}
