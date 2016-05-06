
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../cure/include/runtimevariable.h"
#include "../../lepra/include/lepraos.h"
#include "../../life/lifeserver/masterserverconnection.h"
#include "../../life/lifeapplication.h"
#include "../pushmaster/masterserverport.h"
#include "../push.h"
#include "../rtvar.h"
#include "../version.h"
#include "pushserverticker.h"



namespace Push {



class PushServer: public life::Application {
	typedef life::Application Parent;
public:
	PushServer(const strutil::strvec& argument_list);
	virtual ~PushServer();
	virtual void Init();

private:
	str GetTypeName() const;
	str GetVersion() const;
	cure::ApplicationTicker* CreateTicker() const;
	LogListener* CreateConsoleLogListener() const;
};



}



LEPRA_RUN_APPLICATION(Push::PushServer, lepra::Main);



namespace Push {



PushServer::PushServer(const strutil::strvec& argument_list):
	Parent(kPushApplicationName, argument_list) {
}

PushServer::~PushServer() {
	Destroy();

	cure::Shutdown();
	tbc::Shutdown();
	lepra::Shutdown();
};

void PushServer::Init() {
	lepra::Init();
	tbc::Init();
	cure::Init();

	Parent::Init();
}

str PushServer::GetTypeName() const {
	return "Server";
}

str PushServer::GetVersion() const {
	return kPlatformVersion;
}

cure::ApplicationTicker* PushServer::CreateTicker() const {
	life::GameServerTicker* ticker = new PushServerTicker(resource_manager_, 2000, 7, 1);
	ticker->StartConsole((InteractiveStdioConsoleLogListener*)console_logger_);
	ticker->SetMasterServerConnection(new life::MasterServerConnection(kMasterServerAddress ":" kMasterServerPort));
	return ticker;
}

LogListener* PushServer::CreateConsoleLogListener() const {
	return (new InteractiveStdioConsoleLogListener());
}



}
