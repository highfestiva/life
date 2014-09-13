
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Lepra/Include/LepraOS.h"
#include "../../Lepra/Include/LogLevel.h"
#include "../../Life/LifeMaster/MasterServer.h"
#include "../../Life/LifeApplication.h"
#include "../../Tbc/Include/Tbc.h"
#include "../Push.h"
#include "../Version.h"
#include "MasterServerPort.h"



namespace Push
{



class MasterApplication: public Life::Application
{
	typedef Life::Application Parent;
public:
	MasterApplication(const strutil::strvec& pArgumentList);
	virtual ~MasterApplication();
	virtual void Init();

private:
	virtual Cure::ApplicationTicker* CreateTicker() const;
	virtual str GetTypeName() const;
	virtual str GetVersion() const;
};



}



LEPRA_RUN_APPLICATION(Push::MasterApplication, Lepra::Main);



namespace Push
{



MasterApplication::MasterApplication(const strutil::strvec& pArgumentList):
	Parent(_T(PUSH_APPLICATION_NAME), pArgumentList)
{
}

MasterApplication::~MasterApplication()
{
	Destroy();

	Cure::Shutdown();
	Tbc::Shutdown();
	Lepra::Shutdown();
};

void MasterApplication::Init()
{
	Lepra::Init();
	Tbc::Init();
	Cure::Init();

	Parent::Init();

	//LogType::SetLogLevel(LEVEL_TRACE);
}

Cure::ApplicationTicker* MasterApplication::CreateTicker() const
{
	return new Life::MasterServer(_T(MASTER_SERVER_PORT));
}

str MasterApplication::GetTypeName() const
{
	return _T("Master");
}

str MasterApplication::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}



}
