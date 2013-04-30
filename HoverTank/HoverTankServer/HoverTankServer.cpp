
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Life/LifeServer/MasterServerConnection.h"
#include "../../Life/LifeApplication.h"
#include "../HoverTankMaster/MasterServerPort.h"
#include "../HoverTank.h"
#include "../RtVar.h"
#include "../Version.h"
#include "HoverTankServerTicker.h"



namespace HoverTank
{



class HoverTankServer: public Life::Application
{
	typedef Life::Application Parent;
public:
	HoverTankServer(const strutil::strvec& pArgumentList);
	virtual ~HoverTankServer();
	virtual void Init();

private:
	str GetTypeName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;
	LogListener* CreateConsoleLogListener() const;
};



}



LEPRA_RUN_APPLICATION(HoverTank::HoverTankServer, Lepra::Main);



namespace HoverTank
{



HoverTankServer::HoverTankServer(const strutil::strvec& pArgumentList):
	Parent(_T(HT_APPLICATION_NAME), pArgumentList)
{
}

HoverTankServer::~HoverTankServer()
{
	Destroy();

	Cure::Shutdown();
	TBC::Shutdown();
	Lepra::Shutdown();
};

void HoverTankServer::Init()
{
	Lepra::Init();
	TBC::Init();
	Cure::Init();

	Parent::Init();
}

str HoverTankServer::GetTypeName() const
{
	return _T("Server");
}

str HoverTankServer::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* HoverTankServer::CreateTicker() const
{
	Life::GameServerTicker* lTicker = new HoverTankServerTicker(mResourceManager, 2000, 7, 1);
	lTicker->StartConsole((InteractiveStdioConsoleLogListener*)mConsoleLogger);
	lTicker->SetMasterServerConnection(new Life::MasterServerConnection(_T(MASTER_SERVER_ADDRESS) _T(":") _T(MASTER_SERVER_PORT)));
	return lTicker;
}

LogListener* HoverTankServer::CreateConsoleLogListener() const
{
	return (new InteractiveStdioConsoleLogListener());
}



}
