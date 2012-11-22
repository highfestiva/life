
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../LifeMaster/MasterServerPort.h"
#include "../LifeApplication.h"
#include "GameServerTicker.h"
#include "MasterServerConnection.h"
#include "RtVar.h"
#include "Version.h"



namespace Life
{



class ServerApplication: public Application
{
public:
	typedef Application Parent;

	ServerApplication(const strutil::strvec& pArgumentList);
	virtual ~ServerApplication();
	virtual void Init();

private:
	str GetName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;
	LogListener* CreateConsoleLogListener() const;
};



}



LEPRA_RUN_APPLICATION(Life::ServerApplication, Lepra::Main);



namespace Life
{



ServerApplication::ServerApplication(const strutil::strvec& pArgumentList):
	Application(pArgumentList)
{
}

ServerApplication::~ServerApplication()
{
	Destroy();

	Cure::Shutdown();
	TBC::Shutdown();
	Lepra::Shutdown();
};

void ServerApplication::Init()
{
	Lepra::Init();
	TBC::Init();
	Cure::Init();

	Parent::Init();
}

str ServerApplication::GetName() const
{
	return _T("Server");
}

str ServerApplication::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* ServerApplication::CreateTicker() const
{
	GameServerTicker* lTicker = new GameServerTicker(mResourceManager, (InteractiveStdioConsoleLogListener*)mConsoleLogger, 2000, 7, 1);
	lTicker->SetMasterServerConnection(new MasterServerConnection(_T(MASTER_SERVER_ADDRESS) _T(":") _T(MASTER_SERVER_PORT)));
	return lTicker;
}

LogListener* ServerApplication::CreateConsoleLogListener() const
{
	return (new InteractiveStdioConsoleLogListener());
}



}
