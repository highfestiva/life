
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Life/LifeServer/GameServerTicker.h"
#include "../../Life/LifeServer/MasterServerConnection.h"
#include "../../Life/LifeApplication.h"
#include "../PushMaster/MasterServerPort.h"
#include "../Push.h"
#include "../Version.h"



namespace Push
{



class PushServer: public Life::Application
{
	typedef Life::Application Parent;
public:
	PushServer(const strutil::strvec& pArgumentList);
	virtual ~PushServer();
	virtual void Init();

private:
	str GetName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;
	LogListener* CreateConsoleLogListener() const;
};



}



LEPRA_RUN_APPLICATION(Push::PushServer, Lepra::Main);



namespace Push
{



PushServer::PushServer(const strutil::strvec& pArgumentList):
	Parent(_T(PUSH_APPLICATION_NAME), pArgumentList)
{
}

PushServer::~PushServer()
{
	Destroy();

	Cure::Shutdown();
	TBC::Shutdown();
	Lepra::Shutdown();
};

void PushServer::Init()
{
	Lepra::Init();
	TBC::Init();
	Cure::Init();

	Parent::Init();
}

str PushServer::GetName() const
{
	return _T("Server");
}

str PushServer::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* PushServer::CreateTicker() const
{
	Life::GameServerTicker* lTicker = new Life::GameServerTicker(mResourceManager, (InteractiveStdioConsoleLogListener*)mConsoleLogger, 2000, 7, 1);
	lTicker->SetMasterServerConnection(new Life::MasterServerConnection(_T(MASTER_SERVER_ADDRESS) _T(":") _T(MASTER_SERVER_PORT)));
	return lTicker;
}

LogListener* PushServer::CreateConsoleLogListener() const
{
	return (new InteractiveStdioConsoleLogListener());
}



}
