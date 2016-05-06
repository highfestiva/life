
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../cure/include/runtimevariable.h"
#include "../../life/lifeserver/masterserverconnection.h"
#include "../../life/lifeapplication.h"
#include "../hovertankmaster/masterserverport.h"
#include "../hovertank.h"
#include "../rtvar.h"
#include "../version.h"
#include "hovertankserverticker.h"



namespace HoverTank {



class HoverTankServer: public life::Application {
	typedef life::Application Parent;
public:
	HoverTankServer(const strutil::strvec& argument_list);
	virtual ~HoverTankServer();
	virtual void Init();

private:
	str GetTypeName() const;
	str GetVersion() const;
	cure::ApplicationTicker* CreateTicker() const;
	LogListener* CreateConsoleLogListener() const;
};



}



LEPRA_RUN_APPLICATION(HoverTank::HoverTankServer, lepra::Main);



namespace HoverTank {



HoverTankServer::HoverTankServer(const strutil::strvec& argument_list):
	Parent(kHtApplicationName, argument_list) {
}

HoverTankServer::~HoverTankServer() {
	Destroy();

	cure::Shutdown();
	tbc::Shutdown();
	lepra::Shutdown();
};

void HoverTankServer::Init() {
	lepra::Init();
	tbc::Init();
	cure::Init();

	Parent::Init();
}

str HoverTankServer::GetTypeName() const {
	return "Server";
}

str HoverTankServer::GetVersion() const {
	return kPlatformVersion;
}

cure::ApplicationTicker* HoverTankServer::CreateTicker() const {
	life::GameServerTicker* ticker = new HoverTankServerTicker(resource_manager_, 2000, 7, 1);
	ticker->StartConsole((InteractiveStdioConsoleLogListener*)console_logger_);
	ticker->SetMasterServerConnection(new life::MasterServerConnection(kMasterServerAddress ":" kMasterServerPort));
	return ticker;
}

LogListener* HoverTankServer::CreateConsoleLogListener() const {
	return (new InteractiveStdioConsoleLogListener());
}



}
