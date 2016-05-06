
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/lepraos.h"
#include "../../lepra/include/loglevel.h"
#include "../../life/lifemaster/masterserver.h"
#include "../../life/lifeapplication.h"
#include "../../tbc/include/tbc.h"
#include "../push.h"
#include "../version.h"
#include "masterserverport.h"



namespace Push {



class MasterApplication: public life::Application {
	typedef life::Application Parent;
public:
	MasterApplication(const strutil::strvec& argument_list);
	virtual ~MasterApplication();
	virtual void Init();

private:
	virtual cure::ApplicationTicker* CreateTicker() const;
	virtual str GetTypeName() const;
	virtual str GetVersion() const;
};



}



LEPRA_RUN_APPLICATION(Push::MasterApplication, lepra::Main);



namespace Push {



MasterApplication::MasterApplication(const strutil::strvec& argument_list):
	Parent(kPushApplicationName, argument_list) {
}

MasterApplication::~MasterApplication() {
	Destroy();

	cure::Shutdown();
	tbc::Shutdown();
	lepra::Shutdown();
};

void MasterApplication::Init() {
	lepra::Init();
	tbc::Init();
	cure::Init();

	Parent::Init();

	//LogType::SetLogLevel(kLevelTrace);
}

cure::ApplicationTicker* MasterApplication::CreateTicker() const {
	return new life::MasterServer(kMasterServerPort);
}

str MasterApplication::GetTypeName() const {
	return "Master";
}

str MasterApplication::GetVersion() const {
	return kPlatformVersion;
}



}
